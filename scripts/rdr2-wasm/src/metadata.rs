use crate::ffi;

fn get(field: i32) -> String {
    let length = unsafe { ffi::mod_info(field, std::ptr::null_mut(), 0) };
    if length < 0 {
        return String::new();
    }

    let mut bytes = vec![0u8; length as usize + 1];
    let copied = unsafe { ffi::mod_info(field, bytes.as_mut_ptr(), bytes.len() as i32) };
    if copied < 0 {
        return String::new();
    }
    bytes.truncate(copied as usize);
    String::from_utf8_lossy(&bytes).into_owned()
}

pub fn name() -> String {
    get(0)
}
pub fn version() -> String {
    get(1)
}
pub fn author() -> String {
    get(2)
}
pub fn description() -> String {
    get(3)
}
pub fn path() -> String {
    get(4)
}
