use crate::ffi;

pub fn get_int(index: u32) -> i32 {
    unsafe { ffi::global_get_i32(index) }
}
pub fn set_int(index: u32, value: i32) {
    unsafe { ffi::global_set_i32(index, value) }
}
pub fn get_float(index: u32) -> f32 {
    unsafe { ffi::global_get_f32(index) }
}
pub fn set_float(index: u32, value: f32) {
    unsafe { ffi::global_set_f32(index, value) }
}
