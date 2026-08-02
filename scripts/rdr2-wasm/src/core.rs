use crate::ffi;

pub fn game_time() -> u32 {
    unsafe { ffi::game_time() }
}
pub fn is_key_pressed(key: u32) -> bool {
    unsafe { ffi::is_key_pressed(key) != 0 }
}
pub fn is_key_just_pressed(key: u32) -> bool {
    unsafe { ffi::is_key_just_pressed(key) != 0 }
}
