#![allow(static_mut_refs)]

use anyhow::{anyhow, bail, Result};
use javy::{
    from_js_error,
    quickjs::{self, prelude::Func, Ctx, Function, Module, Object, Value as JsValue},
    Config, Runtime,
};
use serde_json::{json, Value};
use std::{
    alloc::{self, Layout},
    cell::OnceCell,
    process, ptr, slice,
};

const IMPORT_NAMESPACE_BYTES: &[u8] = b"rdr2_javy";

#[unsafe(link_section = "import_namespace")]
pub static IMPORT_NAMESPACE: [u8; IMPORT_NAMESPACE_BYTES.len()] = *b"rdr2_javy";

const FUNCTION_MODULE_NAME: &str = "function.mjs";
const FUNCTION_CACHE_GLOBAL: &str = "__rdr2JavyFunctionCache";
const EVENT_LOOP_ERROR: &str = "pending jobs require an enabled event loop";

static mut RUNTIME: OnceCell<Runtime> = OnceCell::new();
static mut COMPILE_SRC_RETURN: [u32; 3] = [0; 3];

thread_local! {
    static COMPILED_BYTECODE: OnceCell<Vec<u8>> = const { OnceCell::new() };
}

const KIND_RAW: u32 = 0;
const KIND_FLOAT: u32 = 1;
const KIND_UTF8: u32 = 2;
const KIND_VECTOR3: u32 = 3;
const KIND_IN_OUT_BUFFER: u32 = 4;
const KIND_OUT_BUFFER: u32 = 5;

#[repr(C)]
struct NativeArg {
    kind: u32,
    data: u32,
    value: u64,
}

#[repr(C)]
#[derive(Default)]
struct NativeResult {
    value: u64,
    vector: [f32; 3],
    reserved: u32,
}

#[link(wasm_import_module = "rdr2")]
extern "C" {
    #[link_name = "log"]
    fn host_log(level: i32, pointer: *const u8, length: i32);
    #[link_name = "joaat"]
    fn host_joaat(pointer: *const u8, length: i32) -> u32;
    #[link_name = "mod_info"]
    fn host_mod_info(field: i32, destination: *mut u8, capacity: i32) -> i32;
    #[link_name = "native_invoke"]
    fn host_native_invoke(
        hash: u64,
        arguments: *const NativeArg,
        argument_count: i32,
        result: *mut NativeResult,
    ) -> i32;
    #[link_name = "global_get_i32"]
    fn host_global_get_i32(index: u32) -> i32;
    #[link_name = "global_set_i32"]
    fn host_global_set_i32(index: u32, value: i32);
    #[link_name = "global_get_f32"]
    fn host_global_get_f32(index: u32) -> f32;
    #[link_name = "global_set_f32"]
    fn host_global_set_f32(index: u32, value: f32);
    #[link_name = "game_time"]
    fn host_game_time() -> u32;
    #[link_name = "is_key_pressed"]
    fn host_is_key_pressed(key: u32) -> i32;
    #[link_name = "is_key_just_pressed"]
    fn host_is_key_just_pressed(key: u32) -> i32;
    #[link_name = "input_register_binding"]
    fn host_input_register_binding(
        id_pointer: *const u8,
        id_length: i32,
        description_pointer: *const u8,
        description_length: i32,
        mapper_pointer: *const u8,
        mapper_length: i32,
        parameter_pointer: *const u8,
        parameter_length: i32,
    ) -> i32;
    #[link_name = "input_unregister_binding"]
    fn host_input_unregister_binding(handle: i32) -> i32;
    #[link_name = "input_poll_binding_event"]
    fn host_input_poll_binding_event(handle: i32) -> i32;
    #[link_name = "input_is_binding_down"]
    fn host_input_is_binding_down(handle: i32) -> i32;
    #[link_name = "input_get_binding_parameter"]
    fn host_input_get_binding_parameter(handle: i32, destination: *mut u8, capacity: i32) -> i32;
    #[link_name = "input_set_binding"]
    fn host_input_set_binding(
        handle: i32,
        mapper_pointer: *const u8,
        mapper_length: i32,
        parameter_pointer: *const u8,
        parameter_length: i32,
    ) -> i32;
    #[link_name = "input_reset_binding"]
    fn host_input_reset_binding(handle: i32) -> i32;
    #[link_name = "webview_set_focus"]
    fn host_webview_set_focus(focused: i32) -> i32;
    #[link_name = "webview_is_focused"]
    fn host_webview_is_focused() -> i32;
    #[link_name = "webview_show_cursor"]
    fn host_webview_show_cursor() -> i32;
    #[link_name = "webview_hide_cursor"]
    fn host_webview_hide_cursor() -> i32;
    #[link_name = "webview_is_cursor_visible"]
    fn host_webview_is_cursor_visible() -> i32;
    #[link_name = "webview_cursor_ref_count"]
    fn host_webview_cursor_ref_count() -> i32;
}

fn config() -> Config {
    let mut config = Config::default();
    config.gc_threshold(4 * 1024 * 1024);
    config
}

fn modify_runtime(runtime: Runtime) -> Runtime {
    runtime.context().with(|context| {
        let bridge = Object::new(context.clone()).unwrap();
        bridge
            .set(
                "log",
                Func::from(|level: i32, message: String| unsafe {
                    host_log(level, message.as_ptr(), message.len() as i32);
                }),
            )
            .unwrap();
        bridge
            .set(
                "joaat",
                Func::from(|value: String| unsafe {
                    host_joaat(value.as_ptr(), value.len() as i32)
                }),
            )
            .unwrap();
        bridge
            .set("metadata", Func::from(|field: i32| read_metadata(field)))
            .unwrap();
        bridge
            .set(
                "native",
                Func::from(|high: i32, low: i32, arguments: String| {
                    invoke_native(high, low, &arguments)
                }),
            )
            .unwrap();
        bridge
            .set(
                "globalGetInt",
                Func::from(|index: i32| unsafe { host_global_get_i32(index as u32) }),
            )
            .unwrap();
        bridge
            .set(
                "globalSetInt",
                Func::from(|index: i32, value: i32| unsafe {
                    host_global_set_i32(index as u32, value)
                }),
            )
            .unwrap();
        bridge
            .set(
                "globalGetFloat",
                Func::from(|index: i32| unsafe { host_global_get_f32(index as u32) }),
            )
            .unwrap();
        bridge
            .set(
                "globalSetFloat",
                Func::from(|index: i32, value: f32| unsafe {
                    host_global_set_f32(index as u32, value)
                }),
            )
            .unwrap();
        bridge
            .set("gameTime", Func::from(|| unsafe { host_game_time() }))
            .unwrap();
        bridge
            .set(
                "isKeyPressed",
                Func::from(|key: i32| unsafe { host_is_key_pressed(key as u32) != 0 }),
            )
            .unwrap();
        bridge
            .set(
                "isKeyJustPressed",
                Func::from(|key: i32| unsafe { host_is_key_just_pressed(key as u32) != 0 }),
            )
            .unwrap();
        bridge
            .set(
                "registerBinding",
                Func::from(
                    |id: String, description: String, mapper: String, parameter: String| unsafe {
                        host_input_register_binding(
                            id.as_ptr(),
                            id.len() as i32,
                            description.as_ptr(),
                            description.len() as i32,
                            mapper.as_ptr(),
                            mapper.len() as i32,
                            parameter.as_ptr(),
                            parameter.len() as i32,
                        )
                    },
                ),
            )
            .unwrap();
        bridge
            .set(
                "unregisterBinding",
                Func::from(|handle: i32| unsafe { host_input_unregister_binding(handle) }),
            )
            .unwrap();
        bridge
            .set(
                "pollBindingEvent",
                Func::from(|handle: i32| unsafe { host_input_poll_binding_event(handle) }),
            )
            .unwrap();
        bridge
            .set(
                "isBindingDown",
                Func::from(|handle: i32| unsafe { host_input_is_binding_down(handle) }),
            )
            .unwrap();
        bridge
            .set(
                "bindingParameter",
                Func::from(|handle: i32| read_binding_parameter(handle)),
            )
            .unwrap();
        bridge
            .set(
                "setBinding",
                Func::from(|handle: i32, mapper: String, parameter: String| unsafe {
                    host_input_set_binding(
                        handle,
                        mapper.as_ptr(),
                        mapper.len() as i32,
                        parameter.as_ptr(),
                        parameter.len() as i32,
                    )
                }),
            )
            .unwrap();
        bridge
            .set(
                "resetBinding",
                Func::from(|handle: i32| unsafe { host_input_reset_binding(handle) }),
            )
            .unwrap();
        bridge
            .set(
                "webviewSetFocus",
                Func::from(|focused: bool| unsafe { host_webview_set_focus(focused.into()) }),
            )
            .unwrap();
        bridge
            .set(
                "webviewIsFocused",
                Func::from(|| unsafe { host_webview_is_focused() != 0 }),
            )
            .unwrap();
        bridge
            .set(
                "webviewShowCursor",
                Func::from(|| unsafe { host_webview_show_cursor() }),
            )
            .unwrap();
        bridge
            .set(
                "webviewHideCursor",
                Func::from(|| unsafe { host_webview_hide_cursor() }),
            )
            .unwrap();
        bridge
            .set(
                "webviewIsCursorVisible",
                Func::from(|| unsafe { host_webview_is_cursor_visible() != 0 }),
            )
            .unwrap();
        bridge
            .set(
                "webviewCursorRefCount",
                Func::from(|| unsafe { host_webview_cursor_ref_count() }),
            )
            .unwrap();
        context.globals().set("RDR2Host", bridge).unwrap();
    });
    runtime
}

#[unsafe(export_name = "initialize-runtime")]
pub extern "C" fn initialize_runtime() {
    let runtime = modify_runtime(Runtime::new(config()).expect("could not initialize QuickJS"));
    unsafe {
        RUNTIME.take();
        RUNTIME
            .set(runtime)
            .unwrap_or_else(|_| panic!("could not store the QuickJS runtime"));
    }
}

fn runtime() -> Result<&'static Runtime> {
    unsafe { RUNTIME.get() }.ok_or_else(|| anyhow!("QuickJS runtime is not initialized"))
}

fn compile_source(source: &[u8]) -> Result<Vec<u8>> {
    runtime()?.compile_to_bytecode(FUNCTION_MODULE_NAME, &String::from_utf8_lossy(source))
}

fn invoke(bytecode: &[u8], function_name: Option<&str>) -> Result<()> {
    let runtime = runtime()?;
    runtime
        .context()
        .with(|context| invoke_in_context(context, bytecode, function_name))
        .map_err(|error| {
            runtime
                .context()
                .with(|context| from_js_error(context, error))
        })?;

    if runtime.has_pending_jobs() {
        bail!(EVENT_LOOP_ERROR);
    }
    Ok(())
}

fn invoke_in_context<'js>(
    context: Ctx<'js>,
    bytecode: &[u8],
    function_name: Option<&str>,
) -> quickjs::Result<()> {
    let globals = context.globals();
    let function_cache: Object = match globals.get(FUNCTION_CACHE_GLOBAL) {
        Ok(cache) => cache,
        Err(_) => {
            let cache = Object::new(context.clone())?;
            globals.set(FUNCTION_CACHE_GLOBAL, cache.clone())?;
            cache
        }
    };

    if let Some(function_name) = function_name {
        if let Ok(function) = function_cache.get::<_, Function>(function_name) {
            return handle_maybe_promise(context, function.call(())?);
        }
    }

    let module = unsafe { Module::load(context.clone(), bytecode)? };
    let (module, evaluated) = module.eval()?;
    handle_maybe_promise(context.clone(), evaluated.into())?;

    if let Some(function_name) = function_name {
        let function: Function = module.get(function_name)?;
        function_cache.set(function_name, function.clone())?;
        handle_maybe_promise(context, function.call(())?)?;
    }
    Ok(())
}

fn handle_maybe_promise(context: Ctx<'_>, value: JsValue<'_>) -> quickjs::Result<()> {
    match value.as_promise() {
        Some(promise) => match promise.result::<JsValue>() {
            Some(result) => result.map(|_| ()),
            None => Err(javy::to_js_error(context, anyhow!(EVENT_LOOP_ERROR))),
        },
        None => Ok(()),
    }
}

const ZERO_SIZE_ALLOCATION_POINTER: *mut u8 = 1 as _;

#[unsafe(export_name = "cabi_realloc")]
unsafe extern "C" fn cabi_realloc(
    original_pointer: *mut u8,
    original_size: usize,
    alignment: usize,
    new_size: usize,
) -> *mut std::ffi::c_void {
    assert!(new_size >= original_size);
    let new_memory = if new_size == 0 {
        ZERO_SIZE_ALLOCATION_POINTER
    } else {
        unsafe { alloc::alloc(Layout::from_size_align(new_size, alignment).unwrap()) }
    };

    if !original_pointer.is_null() && original_size != 0 {
        unsafe {
            ptr::copy_nonoverlapping(original_pointer, new_memory, original_size);
            alloc::dealloc(
                original_pointer,
                Layout::from_size_align(original_size, alignment).unwrap(),
            );
        }
    }
    new_memory.cast()
}

#[unsafe(export_name = "compile-src")]
unsafe extern "C" fn compile_source_export(
    source_pointer: *const u8,
    source_length: usize,
) -> *const u32 {
    let source = unsafe { slice::from_raw_parts(source_pointer, source_length) };
    let (status, bytes) = match compile_source(source) {
        Ok(bytecode) => (0, bytecode),
        Err(error) => (1, error.to_string().into_bytes()),
    };
    let length = bytes.len();
    COMPILED_BYTECODE.with(|slot| slot.set(bytes).expect("compile-src called more than once"));
    unsafe {
        COMPILE_SRC_RETURN[0] = status;
        COMPILE_SRC_RETURN[1] = COMPILED_BYTECODE.with(|slot| slot.get().unwrap().as_ptr()) as u32;
        COMPILE_SRC_RETURN[2] = length as u32;
        COMPILE_SRC_RETURN.as_ptr()
    }
}

#[unsafe(export_name = "invoke")]
extern "C" fn invoke_export(
    bytecode_pointer: *const u8,
    bytecode_length: usize,
    function_name_discriminator: u32,
    function_name_pointer: *const u8,
    function_name_length: usize,
) {
    let bytecode = unsafe { slice::from_raw_parts(bytecode_pointer, bytecode_length) };
    let function_name = (function_name_discriminator != 0).then(|| {
        String::from_utf8_lossy(unsafe {
            slice::from_raw_parts(function_name_pointer, function_name_length)
        })
        .into_owned()
    });

    invoke(bytecode, function_name.as_deref()).unwrap_or_else(|error| {
        eprintln!("{error}");
        process::abort();
    });
}

fn read_metadata(field: i32) -> String {
    let length = unsafe { host_mod_info(field, std::ptr::null_mut(), 0) };
    if length < 0 {
        return String::new();
    }

    let mut bytes = vec![0_u8; length as usize + 1];
    let copied = unsafe { host_mod_info(field, bytes.as_mut_ptr(), bytes.len() as i32) };
    if copied < 0 || copied as usize >= bytes.len() {
        return String::new();
    }
    bytes.truncate(copied as usize);
    String::from_utf8_lossy(&bytes).into_owned()
}

fn read_binding_parameter(handle: i32) -> String {
    let length = unsafe { host_input_get_binding_parameter(handle, std::ptr::null_mut(), 0) };
    if length < 0 {
        return String::new();
    }
    let mut bytes = vec![0_u8; length as usize + 1];
    let copied =
        unsafe { host_input_get_binding_parameter(handle, bytes.as_mut_ptr(), bytes.len() as i32) };
    if copied < 0 {
        return String::new();
    }
    bytes.truncate(copied as usize);
    String::from_utf8_lossy(&bytes).into_owned()
}

fn invoke_native(high: i32, low: i32, encoded_arguments: &str) -> String {
    let values: Vec<Value> = match serde_json::from_str(encoded_arguments) {
        Ok(values) => values,
        Err(error) => return json!({ "bridgeError": error.to_string() }).to_string(),
    };

    let mut buffers = Vec::<Box<i32>>::new();
    let mut arguments = Vec::with_capacity(values.len());
    for value in &values {
        match convert_argument(value, &mut buffers) {
            Ok(argument) => arguments.push(argument),
            Err(error) => return json!({ "bridgeError": error }).to_string(),
        }
    }

    let hash = ((high as u32 as u64) << 32) | low as u32 as u64;
    let mut result = NativeResult::default();
    let status = unsafe {
        host_native_invoke(
            hash,
            arguments.as_ptr(),
            arguments.len() as i32,
            &mut result,
        )
    };

    json!({
        "status": status,
        "low": result.value as u32,
        "high": (result.value >> 32) as u32,
        "float": f32::from_bits(result.value as u32),
        "vector": result.vector,
        "buffers": buffers.iter().map(|buffer| **buffer).collect::<Vec<_>>(),
    })
    .to_string()
}

// Each allocation must keep a stable address while later arguments grow the vector.
#[allow(clippy::vec_box)]
fn convert_argument(value: &Value, buffers: &mut Vec<Box<i32>>) -> Result<NativeArg, String> {
    let parts = value
        .as_array()
        .ok_or_else(|| "native argument must be an array".to_owned())?;
    let kind = parts
        .first()
        .and_then(Value::as_str)
        .ok_or_else(|| "native argument is missing its type".to_owned())?;

    match kind {
        "raw" => Ok(NativeArg {
            kind: KIND_RAW,
            data: 0,
            value: number_i64(parts.get(1))? as u64,
        }),
        "raw64" => Ok(NativeArg {
            kind: KIND_RAW,
            data: 0,
            value: ((number_u32(parts.get(1))? as u64) << 32) | number_u32(parts.get(2))? as u64,
        }),
        "int" => Ok(NativeArg {
            kind: KIND_RAW,
            data: 0,
            value: number_i32(parts.get(1))? as i64 as u64,
        }),
        "hash" => Ok(NativeArg {
            kind: KIND_RAW,
            data: 0,
            value: number_u32(parts.get(1))? as u64,
        }),
        "bool" => Ok(NativeArg {
            kind: KIND_RAW,
            data: 0,
            value: parts.get(1).and_then(Value::as_bool).unwrap_or(false) as u64,
        }),
        "float" => Ok(NativeArg {
            kind: KIND_FLOAT,
            data: 0,
            value: (number_f32(parts.get(1))?).to_bits() as u64,
        }),
        "string" => {
            let text = parts
                .get(1)
                .and_then(Value::as_str)
                .ok_or_else(|| "string native argument is not a string".to_owned())?;
            Ok(NativeArg {
                kind: KIND_UTF8,
                data: text.len() as u32,
                value: text.as_ptr() as usize as u64,
            })
        }
        "vector" => {
            let vector = parts
                .get(1)
                .and_then(Value::as_array)
                .ok_or_else(|| "vector native argument is not an array".to_owned())?;
            let x = number_f32(vector.first())?.to_bits();
            let y = number_f32(vector.get(1))?.to_bits();
            let z = number_f32(vector.get(2))?.to_bits();
            Ok(NativeArg {
                kind: KIND_VECTOR3,
                data: x,
                value: y as u64 | ((z as u64) << 32),
            })
        }
        "inout32" | "out32" => {
            let initial = if kind == "inout32" {
                number_i32(parts.get(1))?
            } else {
                0
            };
            buffers.push(Box::new(initial));
            let pointer = &mut **buffers.last_mut().unwrap() as *mut i32;
            Ok(NativeArg {
                kind: if kind == "inout32" {
                    KIND_IN_OUT_BUFFER
                } else {
                    KIND_OUT_BUFFER
                },
                data: std::mem::size_of::<i32>() as u32,
                value: pointer as usize as u64,
            })
        }
        _ => Err(format!("unsupported native argument type: {kind}")),
    }
}

fn number_i64(value: Option<&Value>) -> Result<i64, String> {
    value
        .and_then(Value::as_i64)
        .ok_or_else(|| "native argument is not a signed integer".to_owned())
}

fn number_i32(value: Option<&Value>) -> Result<i32, String> {
    number_i64(value).and_then(|value| {
        i32::try_from(value).map_err(|_| "native integer is out of range".to_owned())
    })
}

fn number_u32(value: Option<&Value>) -> Result<u32, String> {
    let value = value
        .and_then(Value::as_u64)
        .ok_or_else(|| "native argument is not an unsigned integer".to_owned())?;
    u32::try_from(value).map_err(|_| "native unsigned integer is out of range".to_owned())
}

fn number_f32(value: Option<&Value>) -> Result<f32, String> {
    value
        .and_then(Value::as_f64)
        .map(|value| value as f32)
        .ok_or_else(|| "native argument is not a number".to_owned())
}
