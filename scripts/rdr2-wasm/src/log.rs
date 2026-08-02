use crate::ffi;

fn write(level: i32, message: impl AsRef<str>) {
    let message = message.as_ref();
    unsafe { ffi::log(level, message.as_ptr(), message.len() as i32) }
}

pub fn info(message: impl AsRef<str>) {
    write(0, message);
}
pub fn warn(message: impl AsRef<str>) {
    write(1, message);
}
pub fn error(message: impl AsRef<str>) {
    write(2, message);
}
pub fn debug(message: impl AsRef<str>) {
    write(3, message);
}
