# RDR2 WebAssembly SDK

This crate is the guest-side API for Rust mods loaded by the ScriptHook
Wasmtime runtime. It provides:

- generated wrappers for every named entry in `nativedb/natives.json`;
- tick, key-down, and key-up callback registration;
- one-shot and repeating timers;
- logging, JOAAT hashing, mod metadata, keyboard state, and game time;
- integer and floating-point global access;
- typed native arguments, output buffers, vectors, and native strings.

Native wrappers are `unsafe` because Wasmtime can isolate guest memory, but it
cannot validate game handles or the semantic contract of a RAGE native.

## Mod entrypoint

Build the final mod as a `cdylib` for `wasm32-unknown-unknown`, register any
callbacks in an initialization function, and export the runtime ABI with:

```rust
fn initialize() {
    rdr2_wasm::event::add_tick_callback(|| {
        // Called once per game frame.
    });
}

rdr2_wasm::entrypoint!(initialize);
```

The macro exports the ABI version, initialization, tick, keyboard, and shutdown
functions expected by the host. Modules are limited to 64 MiB of linear memory
and receive a fresh execution-fuel budget for each dispatched export.

## Building the example

From the repository root:

```text
rustup target add wasm32-unknown-unknown
cargo build --manifest-path scripts/example-wasm/Cargo.toml --target wasm32-unknown-unknown --release
```

Copy `scripts/example-wasm/target/wasm32-unknown-unknown/release/example_mod.wasm`
to a mod directory and name it in that directory's `mod.toml`. The repository's
prebuilt copy is at `shared/static/mods/example-mod/main.wasm`.
