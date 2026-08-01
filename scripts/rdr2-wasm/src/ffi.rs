#[link(wasm_import_module = "rdr2")]
unsafe extern "C" {
    pub fn log(level: i32, pointer: *const u8, length: i32);
    pub fn joaat(pointer: *const u8, length: i32) -> u32;
    pub fn mod_info(field: i32, destination: *mut u8, capacity: i32) -> i32;

    pub fn native_invoke(
        hash: u64,
        arguments: *const super::native::NativeArg,
        argument_count: i32,
        result: *mut super::native::NativeResult,
    ) -> i32;
    pub fn native_string_read(pointer: u64, destination: *mut u8, capacity: i32) -> i32;

    pub fn global_get_i32(index: u32) -> i32;
    pub fn global_set_i32(index: u32, value: i32);
    pub fn global_get_f32(index: u32) -> f32;
    pub fn global_set_f32(index: u32, value: f32);

    pub fn game_time() -> u32;
    pub fn is_key_pressed(key: u32) -> i32;
    pub fn is_key_just_pressed(key: u32) -> i32;
}
