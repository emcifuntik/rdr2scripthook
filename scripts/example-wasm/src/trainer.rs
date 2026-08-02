use crate::catalog::{Action, ModelPurpose, Screen};
use crate::ui::{self, Message};

use rdr2_wasm::{global, hash, log, natives, vk, webview, NativeError, Vehicle};

use std::cell::RefCell;
use std::collections::VecDeque;
use std::fmt;

const PLAYER_MODEL_CHANGE_GLOBAL: u32 = 1_835_009;
const MODEL_LOAD_TIMEOUT_TICKS: u32 = 900;
const MAX_UI_MESSAGES_PER_TICK: usize = 32;

thread_local! {
    static TRAINER: RefCell<Trainer> = RefCell::new(Trainer::new());
}

struct Trainer {
    open: bool,
    cursor_held: bool,
    pending_models: VecDeque<PendingModel>,
    last_vehicle: Vehicle,
    invincible: bool,
    status: Option<Status>,
    poll_error_reported: bool,
}

struct PendingModel {
    name: &'static str,
    hash: u32,
    purpose: ModelPurpose,
    waited_ticks: u32,
}

struct Status {
    text: String,
    error: bool,
}

#[derive(Debug)]
enum TrainerError {
    Native(NativeError),
    NativeCall {
        operation: &'static str,
        source: NativeError,
    },
    Rejected(&'static str),
}

impl From<NativeError> for TrainerError {
    fn from(error: NativeError) -> Self {
        Self::Native(error)
    }
}

impl fmt::Display for TrainerError {
    fn fmt(&self, formatter: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            Self::Native(error) => error.fmt(formatter),
            Self::NativeCall { operation, source } => {
                write!(formatter, "{operation} failed: {source}")
            }
            Self::Rejected(message) => formatter.write_str(message),
        }
    }
}

impl Trainer {
    fn new() -> Self {
        Self {
            open: false,
            cursor_held: false,
            pending_models: VecDeque::new(),
            last_vehicle: 0,
            invincible: false,
            status: None,
            poll_error_reported: false,
        }
    }

    fn tick(&mut self) {
        self.process_model_queue();
        if self.open && !webview::is_open() {
            self.release_cursor();
            self.open = false;
            log::warn("Trainer WebView closed unexpectedly");
            return;
        }
        if webview::is_open() {
            self.poll_messages();
        }
    }

    fn key_down(&mut self, key: u32) {
        if key == vk::F3 {
            if self.open {
                self.close_menu();
            } else {
                self.open_menu();
            }
            return;
        }

        // WebView2 reserves F8 as a focus-release accelerator. Close the
        // trainer when that happens so focus and cursor ownership cannot drift.
        if self.open && key == vk::F8 {
            self.close_menu();
        }
    }

    fn open_menu(&mut self) {
        if !webview::is_open() {
            let url = ui::page_url(self.invincible);
            if let Err(error) = webview::open(&url, 0, 0) {
                log::error(format!("Could not open trainer WebView: {error:?}"));
                return;
            }
        }

        if let Err(error) = webview::set_visible(true) {
            log::error(format!("Could not show trainer WebView: {error:?}"));
            return;
        }
        if let Err(error) = webview::set_focus(true) {
            log::error(format!("Could not focus trainer WebView: {error:?}"));
            let _ = webview::set_visible(false);
            return;
        }

        if let Err(error) = webview::show_cursor() {
            log::error(format!("Could not show trainer cursor: {error:?}"));
            let _ = webview::set_focus(false);
            let _ = webview::set_visible(false);
            return;
        }

        self.open = true;
        self.cursor_held = true;
        self.poll_error_reported = false;
        self.publish_snapshot();
        log::info("Trainer WebView opened");
    }

    fn close_menu(&mut self) {
        if !self.open {
            self.release_cursor();
            return;
        }

        let _ = webview::set_visible(false);
        self.release_cursor();
        let _ = webview::set_focus(false);
        self.open = false;
        self.poll_error_reported = false;
        log::info("Trainer WebView hidden");
    }

    fn prewarm(&mut self) {
        if webview::is_open() {
            let _ = webview::set_visible(false);
            return;
        }

        let url = ui::page_url(self.invincible);
        match webview::open(&url, 0, 0) {
            Ok(()) => {
                let _ = webview::set_visible(false);
                log::info("Trainer WebView prewarming in the background");
            }
            Err(error) => log::warn(format!(
                "Could not prewarm trainer WebView; F3 will retry: {error:?}"
            )),
        }
    }

    fn release_cursor(&mut self) {
        if !self.cursor_held {
            return;
        }
        if let Err(error) = webview::hide_cursor() {
            log::warn(format!("Could not release trainer cursor: {error:?}"));
        }
        self.cursor_held = false;
    }

    fn poll_messages(&mut self) {
        for _ in 0..MAX_UI_MESSAGES_PER_TICK {
            match webview::poll_json() {
                Ok(Some(json)) => {
                    self.poll_error_reported = false;
                    match ui::parse_message(&json) {
                        Some(Message::Ready) => self.publish_snapshot(),
                        Some(Message::Close) => {
                            self.close_menu();
                            break;
                        }
                        Some(Message::Activate { screen, index }) => self.activate(screen, index),
                        None => log::warn(format!("Ignored invalid trainer UI message: {json}")),
                    }
                }
                Ok(None) => break,
                Err(error) => {
                    if !self.poll_error_reported {
                        self.poll_error_reported = true;
                        log::error(format!("Trainer WebView message poll failed: {error:?}"));
                    }
                    break;
                }
            }
        }
    }

    fn activate(&mut self, screen: Screen, index: usize) {
        let Some(entry) = screen.entry(index) else {
            self.report_error(TrainerError::Rejected("Invalid trainer menu action"));
            return;
        };

        match entry.action {
            Action::LoadModel { model, purpose } => self.queue_model(model, purpose),
            action => match self.perform_action(action) {
                Ok(message) => {
                    self.set_status(message);
                    self.publish_state();
                }
                Err(error) => self.report_error(error),
            },
        }
    }

    fn perform_action(&mut self, action: Action) -> Result<String, TrainerError> {
        unsafe {
            match action {
                Action::Teleport { x, y, z } => {
                    let player_ped = natives::player_ped_id()?;
                    natives::set_entity_coords_no_offset(player_ped, x, y, z, false, false, false)?;
                    Ok(format!("Teleported to {x:.0}, {y:.0}, {z:.0}"))
                }
                Action::SetWeather(name) => {
                    let weather = hash::joaat(name);
                    natives::set_curr_weather_state(weather, weather, 0.5, true)?;
                    Ok(format!("Weather set to {name}"))
                }
                Action::AddTime { hours, minutes } => {
                    natives::add_to_clock_time(hours, minutes, 0)?;
                    Ok(format!("Clock changed by {hours}h {minutes}m"))
                }
                Action::Heal => {
                    let player_ped = natives::player_ped_id()?;
                    let maximum = natives::get_entity_max_health(player_ped, true)?;
                    natives::set_entity_health(player_ped, maximum, 0)?;
                    Ok("Health restored".to_owned())
                }
                Action::RefillStamina => {
                    let player = natives::player_id()?;
                    natives::restore_player_stamina(player, 1.0)?;
                    Ok("Stamina restored".to_owned())
                }
                Action::ToggleInvincibility => {
                    let enabled = !self.invincible;
                    let player = natives::player_id()?;
                    natives::set_player_invincible(player, enabled)?;
                    self.invincible = enabled;
                    Ok(format!(
                        "Invincibility {}",
                        if self.invincible {
                            "enabled"
                        } else {
                            "disabled"
                        }
                    ))
                }
                Action::LoadModel { .. } => unreachable!(),
            }
        }
    }

    fn queue_model(&mut self, name: &'static str, purpose: ModelPurpose) {
        let model = hash::joaat(name);
        if self
            .pending_models
            .iter()
            .any(|pending| pending.hash == model && pending.purpose == purpose)
        {
            self.set_status(format!("Already loading {name}"));
            return;
        }

        let result = match unsafe { natives::is_model_valid(model) } {
            Ok(true) => unsafe { natives::request_model(model, false) },
            Ok(false) => {
                self.report_error(TrainerError::Rejected("Game rejected the selected model"));
                return;
            }
            Err(error) => {
                self.report_error(TrainerError::NativeCall {
                    operation: "IS_MODEL_VALID",
                    source: error,
                });
                return;
            }
        };

        match result {
            Ok(()) => {
                self.pending_models.push_back(PendingModel {
                    name,
                    hash: model,
                    purpose,
                    waited_ticks: 0,
                });
                self.set_status(format!("Loading {}: {name}", purpose.description()));
            }
            Err(error) => self.report_error(TrainerError::NativeCall {
                operation: "REQUEST_MODEL",
                source: error,
            }),
        }
    }

    fn process_model_queue(&mut self) {
        let Some(mut pending) = self.pending_models.pop_front() else {
            return;
        };

        match unsafe { natives::has_model_loaded(pending.hash) } {
            Ok(true) => {
                let result = unsafe { self.complete_model_action(&pending) };
                if let Err(error) = unsafe { natives::set_model_as_no_longer_needed(pending.hash) }
                {
                    log::warn(format!("Failed to release model {}: {error}", pending.name));
                }

                match result {
                    Ok(message) => self.set_status(message),
                    Err(error) => self.report_error(error),
                }
            }
            Ok(false) => {
                pending.waited_ticks += 1;
                if pending.waited_ticks >= MODEL_LOAD_TIMEOUT_TICKS {
                    self.report_error(TrainerError::Rejected("Timed out while loading the model"));
                } else {
                    match unsafe { natives::request_model(pending.hash, false) } {
                        Ok(()) => self.pending_models.push_back(pending),
                        Err(error) => self.report_error(TrainerError::NativeCall {
                            operation: "REQUEST_MODEL",
                            source: error,
                        }),
                    }
                }
            }
            Err(error) => self.report_error(TrainerError::NativeCall {
                operation: "HAS_MODEL_LOADED",
                source: error,
            }),
        }
    }

    unsafe fn complete_model_action(
        &mut self,
        pending: &PendingModel,
    ) -> Result<String, TrainerError> {
        unsafe {
            match pending.purpose {
                ModelPurpose::SpawnHorse => {
                    let (x, y, z, heading) = player_spawn_point(3.5)?;
                    let horse = natives::create_ped(
                        pending.hash,
                        x,
                        y,
                        z + 0.5,
                        heading,
                        false,
                        false,
                        false,
                        false,
                    )
                    .map_err(|source| TrainerError::NativeCall {
                        operation: "CREATE_PED",
                        source,
                    })?;
                    if horse == 0 {
                        return Err(TrainerError::Rejected("Game did not create the horse"));
                    }
                    natives::set_entity_visible(horse, true)?;
                    natives::set_entity_alpha(horse, 255, false)?;
                    natives::set_random_outfit_variation(horse, true)?;
                    natives::set_blocking_of_non_temporary_events(horse, true)?;
                    let _ = natives::place_entity_on_ground_properly(horse, true);
                    Ok(format!("Spawned horse: {}", pending.name))
                }
                ModelPurpose::ChangePlayer => {
                    global::set_int(PLAYER_MODEL_CHANGE_GLOBAL, 1);
                    let player = natives::player_id()?;
                    natives::set_player_model(player, pending.hash, true)?;
                    let player_ped = natives::player_ped_id()?;
                    natives::set_random_outfit_variation(player_ped, true)?;
                    if self.invincible {
                        natives::set_player_invincible(player, true)?;
                    }
                    Ok(format!("Player model changed: {}", pending.name))
                }
                ModelPurpose::SpawnPed => {
                    let (x, y, z, heading) = player_spawn_point(3.0)?;
                    let ped = natives::create_ped(
                        pending.hash,
                        x,
                        y,
                        z + 0.2,
                        heading,
                        false,
                        false,
                        false,
                        false,
                    )
                    .map_err(|source| TrainerError::NativeCall {
                        operation: "CREATE_PED",
                        source,
                    })?;
                    if ped == 0 {
                        return Err(TrainerError::Rejected("Game did not create the ped"));
                    }
                    natives::set_entity_visible(ped, true)?;
                    natives::set_entity_alpha(ped, 255, false)?;
                    natives::set_random_outfit_variation(ped, true)?;
                    let _ = natives::place_entity_on_ground_properly(ped, true);
                    natives::task_wander_standard(ped, 10.0, 10)?;
                    Ok(format!("Spawned ped: {}", pending.name))
                }
                ModelPurpose::SpawnVehicle => {
                    if self.last_vehicle != 0
                        && natives::does_entity_exist(self.last_vehicle).unwrap_or(false)
                    {
                        natives::delete_vehicle(&mut self.last_vehicle)?;
                    }

                    let (x, y, z, heading) = player_spawn_point(5.0)?;
                    let vehicle = natives::create_vehicle(
                        pending.hash,
                        x,
                        y,
                        z + 0.5,
                        heading,
                        false,
                        false,
                        false,
                        false,
                    )?;
                    if vehicle == 0 {
                        return Err(TrainerError::Rejected("Game did not create the vehicle"));
                    }
                    self.last_vehicle = vehicle;
                    let _ = natives::set_vehicle_on_ground_properly(vehicle, true);
                    Ok(format!("Spawned vehicle: {}", pending.name))
                }
            }
        }
    }

    fn set_status(&mut self, message: String) {
        log::info(&message);
        self.status = Some(Status {
            text: message,
            error: false,
        });
        self.publish_status();
    }

    fn report_error(&mut self, error: TrainerError) {
        let message = format!("Trainer action failed: {error}");
        log::error(&message);
        self.status = Some(Status {
            text: message,
            error: true,
        });
        self.publish_status();
    }

    fn publish_snapshot(&self) {
        self.publish_state();
        self.publish_status();
    }

    fn publish_state(&self) {
        self.publish_json(&ui::state_json(self.invincible));
    }

    fn publish_status(&self) {
        if let Some(status) = &self.status {
            self.publish_json(&ui::status_json(&status.text, status.error));
        }
    }

    fn publish_json(&self, json: &str) {
        if !self.open {
            return;
        }
        if let Err(error) = webview::post_json(json) {
            log::warn(format!("Could not update trainer WebView: {error:?}"));
        }
    }
}

unsafe fn player_spawn_point(distance: f32) -> Result<(f32, f32, f32, f32), NativeError> {
    unsafe {
        let player_ped = natives::player_ped_id()?;
        let coords = natives::get_entity_coords(player_ped, true, true)?;
        let heading = natives::get_entity_heading(player_ped)?;
        let radians = heading.to_radians();
        Ok((
            coords.x - radians.sin() * distance,
            coords.y + radians.cos() * distance,
            coords.z,
            heading,
        ))
    }
}

pub fn initialize() {
    global::set_int(PLAYER_MODEL_CHANGE_GLOBAL, 1);
    TRAINER.with(|trainer| trainer.borrow_mut().prewarm());
}

pub fn tick() {
    TRAINER.with(|trainer| trainer.borrow_mut().tick());
}

pub fn key_down(key: u32) {
    TRAINER.with(|trainer| trainer.borrow_mut().key_down(key));
}
