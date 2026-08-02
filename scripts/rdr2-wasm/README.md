# RDR2 WebAssembly SDK

This crate is the guest-side API for Rust mods loaded by the ScriptHook
Wasmtime runtime. It provides:

- generated wrappers for every named entry in `nativedb/natives.json`;
- tick, key-down, and key-up callback registration;
- one-shot and repeating timers;
- logging, JOAAT hashing, mod metadata, keyboard state, and game time;
- persistent named input bindings with Down/Up subscriptions and remapping;
- integer and floating-point global access;
- a full-screen WebView2 overlay with independent focus, reference-counted
  cursor visibility, and bidirectional JSON messages;
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

## Input bindings

Register user-facing actions with `input::Binding::register_keyboard` or
`input::subscribe_keyboard`. IDs are stable within a mod; the selected key is
stored in `input-bindings.toml`. Bindings expose queued `Down`/`Up` edges,
current state, the selected parameter, `set_keyboard`, and `reset`. The host
automatically unregisters every binding owned by an unloading mod.

To list a binding in the game's `Settings -> Controls -> Script Bindings`
category (which is inserted before the retail categories), declare the same ID,
description, mapper, and default parameter in the mod's manifest:

```toml
[[input.bindings]]
id = "open_inventory"
description = "Open custom inventory"
mapper = "keyboard"
default = "F6"
```

The host reserves these controls before the frontend caches its settings UI,
then the runtime registration adopts the reservation. Up to 2048 declared
bindings are visible. Reassigning either key there takes effect immediately.
The primary slot uses the same persisted override as `set_keyboard`, while the
alternate slot is managed by the Controls UI. Runtime-only bindings still
receive events and can be remapped through the SDK, but are not added to an
already constructed Controls screen.

## WebView focus and cursor

Focus and cursor visibility are separate controls. `webview::set_focus(true)`
routes mouse and keyboard messages to WebView2 and suppresses RAGE input.
`webview::show_cursor()` acquires one visibility reference owned by the calling
mod; each successful call must be paired with `webview::hide_cursor()`. Use
`webview::is_focused()`, `webview::is_cursor_visible()`, and
`webview::cursor_ref_count()` to inspect the current state. The host releases
remaining references and closes an owned WebView automatically when a mod
unloads.

`webview::set_visible(false)` hides the overlay without tearing down Chromium,
its composition controller, or the synchronized shared surfaces. Prefer this
for menus that are opened repeatedly. Cursor visibility is provided by RDR2's
native in-game cursor rather than a separate host-rendered cursor.

## Building the WebView trainer

From the repository root:

```text
rustup target add wasm32-unknown-unknown
cargo build --manifest-path scripts/example-wasm/Cargo.toml --target wasm32-unknown-unknown --release
```

Copy `scripts/example-wasm/target/wasm32-unknown-unknown/release/example_mod.wasm`
to a mod directory and name it in that directory's `mod.toml`. The repository's
prebuilt copy is at `shared/static/mods/example-mod/main.wasm`. Its
`toggle_trainer` binding defaults to F3. The menu routes focus and a
reference-counted cursor to WebView2 while open, and releases both when closed
with its binding or Escape. It prewarms the browser in the background at
startup; subsequent toggles only change visibility.
