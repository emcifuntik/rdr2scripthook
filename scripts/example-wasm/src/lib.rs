use rdr2_wasm::{event, log, metadata, natives, timer, vk};

use std::cell::Cell;

thread_local! {
    static MENU_OPEN: Cell<bool> = const { Cell::new(false) };
    static TICK_COUNTER: Cell<u32> = const { Cell::new(0) };
}

fn on_tick() {
    let ticks = TICK_COUNTER.with(|counter| {
        let ticks = counter.get().wrapping_add(1);
        counter.set(ticks);
        ticks
    });

    if ticks.is_multiple_of(60) {
        let timer = unsafe { natives::get_game_timer() };
        if ticks.is_multiple_of(300) {
            match timer {
                Ok(milliseconds) => log::info(format!("Game timer: {milliseconds}ms")),
                Err(error) => log::error(error.to_string()),
            }
        }
    }

    if MENU_OPEN.with(Cell::get) {
        // Draw menu elements here with the generated native wrappers.
    }
}

fn on_key_down(key: u32) {
    match key {
        vk::F4 => {
            let open = MENU_OPEN.with(|state| {
                let open = !state.get();
                state.set(open);
                open
            });
            log::info(if open { "Menu opened" } else { "Menu closed" });
        }
        vk::F5 => show_player_position(),
        vk::F6 => teleport_player_forward(),
        vk::F7 => match unsafe { natives::set_clock_time(12, 0, 0) } {
            Ok(()) => log::info("Time set to 12:00"),
            Err(error) => log::error(error.to_string()),
        },
        vk::F8 => show_current_time(),
        vk::ADD => boost_vehicle(),
        _ => {}
    }
}

fn show_player_position() {
    let result = (|| unsafe {
        let player = natives::player_ped_id()?;
        natives::get_entity_coords(player, true, true)
    })();
    match result {
        Ok(coords) => log::info(format!(
            "Player position: {:.2}, {:.2}, {:.2}",
            coords.x, coords.y, coords.z
        )),
        Err(error) => log::error(error.to_string()),
    }
}

fn teleport_player_forward() {
    let result = (|| unsafe {
        let player = natives::player_ped_id()?;
        let coords = natives::get_entity_coords(player, true, true)?;
        let heading = natives::get_entity_heading(player)?;
        let heading_radians = heading.to_radians();
        let x = coords.x - heading_radians.sin() * 5.0;
        let y = coords.y + heading_radians.cos() * 5.0;
        natives::set_entity_coords(player, x, y, coords.z, false, false, false, false)
    })();
    match result {
        Ok(()) => log::info("Teleported forward!"),
        Err(error) => log::error(error.to_string()),
    }
}

fn show_current_time() {
    let result = (|| unsafe {
        let hours = natives::get_clock_hours()?;
        let minutes = natives::get_clock_minutes()?;
        Ok::<_, rdr2_wasm::NativeError>((hours, minutes))
    })();
    match result {
        Ok((hours, minutes)) => log::info(format!("Current time: {hours}:{minutes:02}")),
        Err(error) => log::error(error.to_string()),
    }
}

fn boost_vehicle() {
    let result = (|| unsafe {
        let player = natives::player_ped_id()?;
        if !natives::is_ped_in_any_vehicle(player, false)? {
            return Ok(false);
        }
        let vehicle = natives::get_vehicle_ped_is_in(player, false)?;
        natives::set_vehicle_forward_speed(vehicle, 50.0)?;
        Ok::<_, rdr2_wasm::NativeError>(true)
    })();
    match result {
        Ok(true) => log::info("Vehicle boosted!"),
        Ok(false) => log::info("Not in a vehicle"),
        Err(error) => log::error(error.to_string()),
    }
}

fn initialize() {
    log::info("=== Example Rust WASM Mod Initializing ===");
    log::info(format!("Mod Name: {}", metadata::name()));
    log::info(format!("Mod Version: {}", metadata::version()));
    log::info(format!("Mod Author: {}", metadata::author()));
    log::info("Press F4 to toggle the example menu");
    log::info("========================================");

    let tick_id = event::add_tick_callback(on_tick);
    let key_down_id = event::add_key_down_callback(on_key_down);
    event::add_key_up_callback(|_key| {});

    timer::set_timeout(
        || log::info("Mod fully initialized after 1 second delay"),
        1_000,
    );
    timer::set_interval(
        || {
            let ticks = TICK_COUNTER.with(Cell::get);
            log::info(format!("Periodic check: {ticks} ticks elapsed"));
        },
        10_000,
    );

    log::info(format!(
        "Example mod loaded; tick callback id={tick_id}, key-down id={key_down_id}"
    ));
}

rdr2_wasm::entrypoint!(initialize);
