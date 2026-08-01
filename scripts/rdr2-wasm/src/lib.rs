//! Rust guest SDK for RDR2 ScriptHook WebAssembly mods.
//!
//! Build mods for `wasm32-unknown-unknown` and use [`entrypoint!`] once in the
//! final mod crate. The macro exports the stable ABI functions consumed by the
//! Wasmtime host.

mod ffi;

pub mod core;
pub mod event;
pub mod global;
pub mod hash;
pub mod log;
pub mod metadata;
pub mod native;
#[rustfmt::skip]
pub mod natives;
pub mod timer;
pub mod types;
pub mod vk;

pub use native::{NativeArg, NativeError, NativeResult, NativeString};
pub use types::*;

pub const ABI_VERSION: u32 = 1;

/// Export the ScriptHook ABI and designate the mod's initialization function.
#[macro_export]
macro_rules! entrypoint {
    ($initialize:path) => {
        #[unsafe(no_mangle)]
        pub extern "C" fn rdr2_abi_version() -> u32 {
            $crate::ABI_VERSION
        }

        #[unsafe(no_mangle)]
        pub extern "C" fn rdr2_init() {
            $initialize();
        }

        #[unsafe(no_mangle)]
        pub extern "C" fn rdr2_tick() {
            $crate::event::__dispatch_tick();
            $crate::timer::__dispatch();
        }

        #[unsafe(no_mangle)]
        pub extern "C" fn rdr2_key_down(key: u32) {
            $crate::event::__dispatch_key_down(key);
        }

        #[unsafe(no_mangle)]
        pub extern "C" fn rdr2_key_up(key: u32) {
            $crate::event::__dispatch_key_up(key);
        }

        #[unsafe(no_mangle)]
        pub extern "C" fn rdr2_shutdown() {
            $crate::event::__clear();
            $crate::timer::__clear();
        }
    };
}
