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

    pub fn input_register_binding(
        id_pointer: *const u8,
        id_length: i32,
        description_pointer: *const u8,
        description_length: i32,
        mapper_pointer: *const u8,
        mapper_length: i32,
        parameter_pointer: *const u8,
        parameter_length: i32,
    ) -> i32;
    pub fn input_unregister_binding(handle: i32) -> i32;
    pub fn input_poll_binding_event(handle: i32) -> i32;
    pub fn input_is_binding_down(handle: i32) -> i32;
    pub fn input_get_binding_parameter(handle: i32, destination: *mut u8, capacity: i32) -> i32;
    pub fn input_set_binding(
        handle: i32,
        mapper_pointer: *const u8,
        mapper_length: i32,
        parameter_pointer: *const u8,
        parameter_length: i32,
    ) -> i32;
    pub fn input_reset_binding(handle: i32) -> i32;

    pub fn webview_open(pointer: *const u8, length: i32, width: i32, height: i32) -> i32;
    pub fn webview_close();
    pub fn webview_is_open() -> i32;
    pub fn webview_set_visible(visible: i32) -> i32;
    pub fn webview_is_visible() -> i32;
    pub fn webview_set_focus(focused: i32) -> i32;
    pub fn webview_is_focused() -> i32;
    pub fn webview_show_cursor() -> i32;
    pub fn webview_hide_cursor() -> i32;
    pub fn webview_is_cursor_visible() -> i32;
    pub fn webview_cursor_ref_count() -> i32;
    pub fn webview_navigate(pointer: *const u8, length: i32) -> i32;
    pub fn webview_post_json(pointer: *const u8, length: i32) -> i32;
    pub fn webview_poll_json(destination: *mut u8, capacity: i32) -> i32;
}
