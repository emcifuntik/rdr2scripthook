use crate::ffi;

pub fn joaat(value: &str) -> u32 {
    unsafe { ffi::joaat(value.as_ptr(), value.len() as i32) }
}
