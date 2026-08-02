//! Persistent, script-owned input actions backed by the game's input state.

use crate::{event, ffi};
use std::rc::Rc;

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum Error {
    InvalidHandle,
    InvalidArgument,
    UnsupportedMapper,
    UnsupportedParameter,
    AlreadyRegistered,
    CapacityExceeded,
    InvalidUtf8,
    Host(i32),
}

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum BindingEvent {
    Down,
    Up,
}

fn error(value: i32) -> Error {
    match value {
        -1 => Error::InvalidHandle,
        -2 => Error::InvalidArgument,
        -3 => Error::UnsupportedMapper,
        -4 => Error::UnsupportedParameter,
        -5 => Error::AlreadyRegistered,
        -6 => Error::CapacityExceeded,
        value => Error::Host(value),
    }
}

fn status(value: i32) -> Result<(), Error> {
    if value == 0 {
        Ok(())
    } else {
        Err(error(value))
    }
}

fn valid_guest_string(value: &str) -> Result<i32, Error> {
    i32::try_from(value.len()).map_err(|_| Error::InvalidArgument)
}

/// A binding registered by this mod. Its persisted mapping is keyed by the
/// mod directory/name plus `id`, so changing the description does not lose a
/// user's choice.
pub struct Binding {
    handle: i32,
}

impl Binding {
    pub fn register_keyboard(
        id: &str,
        description: &str,
        default_key: &str,
    ) -> Result<Self, Error> {
        let id_length = valid_guest_string(id)?;
        let description_length = valid_guest_string(description)?;
        let default_length = valid_guest_string(default_key)?;
        const MAPPER: &str = "keyboard";
        let handle = unsafe {
            ffi::input_register_binding(
                id.as_ptr(),
                id_length,
                description.as_ptr(),
                description_length,
                MAPPER.as_ptr(),
                MAPPER.len() as i32,
                default_key.as_ptr(),
                default_length,
            )
        };
        if handle <= 0 {
            return Err(error(handle));
        }
        Ok(Self { handle })
    }

    pub fn poll_event(&self) -> Result<Option<BindingEvent>, Error> {
        match unsafe { ffi::input_poll_binding_event(self.handle) } {
            0 => Ok(None),
            1 => Ok(Some(BindingEvent::Down)),
            2 => Ok(Some(BindingEvent::Up)),
            value => Err(error(value)),
        }
    }

    pub fn is_down(&self) -> Result<bool, Error> {
        match unsafe { ffi::input_is_binding_down(self.handle) } {
            0 => Ok(false),
            1 => Ok(true),
            value => Err(error(value)),
        }
    }

    pub fn parameter(&self) -> Result<String, Error> {
        let length =
            unsafe { ffi::input_get_binding_parameter(self.handle, std::ptr::null_mut(), 0) };
        if length < 0 {
            return Err(error(length));
        }
        let capacity = length.checked_add(1).ok_or(Error::InvalidArgument)?;
        let mut bytes = vec![0u8; capacity as usize];
        let copied =
            unsafe { ffi::input_get_binding_parameter(self.handle, bytes.as_mut_ptr(), capacity) };
        if copied != length {
            return Err(error(copied));
        }
        bytes.truncate(length as usize);
        String::from_utf8(bytes).map_err(|_| Error::InvalidUtf8)
    }

    pub fn set_keyboard(&self, key: &str) -> Result<(), Error> {
        let key_length = valid_guest_string(key)?;
        const MAPPER: &str = "keyboard";
        status(unsafe {
            ffi::input_set_binding(
                self.handle,
                MAPPER.as_ptr(),
                MAPPER.len() as i32,
                key.as_ptr(),
                key_length,
            )
        })
    }

    pub fn reset(&self) -> Result<(), Error> {
        status(unsafe { ffi::input_reset_binding(self.handle) })
    }
}

impl Drop for Binding {
    fn drop(&mut self) {
        if self.handle > 0 {
            unsafe { ffi::input_unregister_binding(self.handle) };
            self.handle = 0;
        }
    }
}

/// A tick-driven Down/Up subscription that owns its binding. Dropping the
/// subscription unregisters both the callback and the host binding.
pub struct Subscription {
    binding: Rc<Binding>,
    callback_id: event::CallbackId,
}

impl Subscription {
    pub fn binding(&self) -> &Binding {
        &self.binding
    }
}

impl Drop for Subscription {
    fn drop(&mut self) {
        event::remove_tick_callback(self.callback_id);
    }
}

pub fn subscribe_keyboard(
    id: &str,
    description: &str,
    default_key: &str,
    mut callback: impl FnMut(BindingEvent) + 'static,
) -> Result<Subscription, Error> {
    let binding = Rc::new(Binding::register_keyboard(id, description, default_key)?);
    let callback_binding = Rc::clone(&binding);
    let callback_id = event::add_tick_callback(move || {
        // Bound the work even if a script was paused for a long time.
        for _ in 0..32 {
            match callback_binding.poll_event() {
                Ok(Some(binding_event)) => callback(binding_event),
                Ok(None) | Err(_) => break,
            }
        }
    });
    Ok(Subscription {
        binding,
        callback_id,
    })
}
