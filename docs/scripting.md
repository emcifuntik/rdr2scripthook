# WASM mods and scripting API

## Mod layout

The loader scans `mods/<directory>/mod.toml`. `name` and `entrypoint` are
required; `runtime` defaults to `wasmtime` and may also be `javy`.

```toml
[mod]
name = "Example Mod"
version = "1.0.0"
author = "Author"
description = "Example description"
entrypoint = "main.wasm"
runtime = "wasmtime"
```

The entrypoint must be a non-empty `.wasm` file in the same mod directory.

## Rust guest

Reference `scripts/rdr2-wasm` from a `cdylib` crate and build for
`wasm32-unknown-unknown`:

```rust
fn initialize() {
    rdr2_wasm::event::add_tick_callback(|| {
        // Runs once per game script tick.
    });

    rdr2_wasm::event::add_key_down_callback(|key| {
        rdr2_wasm::log::info(format!("key down: {key}"));
    });
}

rdr2_wasm::entrypoint!(initialize);
```

The macro exports ABI version 1 plus initialization, tick, key-down, key-up and
shutdown functions. Do not export these functions manually when using the
macro.

## SDK modules

| Module | API |
| --- | --- |
| `core` | Game time and current/edge keyboard queries. |
| `event` | Tick, key-down and key-up callbacks. |
| `timer` | One-shot and repeating timers driven by game time. |
| `natives` | Generated typed wrappers for named RDR3 natives. |
| `native` | Generic native invocation arguments, results and output buffers. |
| `global` | Integer and floating-point script-global access. |
| `metadata` | Manifest fields and the current mod path. |
| `hash` | JOAAT hashing. |
| `log` | Debug, info, warning and error logging. |
| `types` | Vector and game-handle aliases. |
| `vk` | Win32 virtual-key constants. |
| `webview` | Overlay lifecycle, focus, cursor and JSON messaging. |

Generated native wrappers are `unsafe`: guest-memory isolation cannot validate
game handles, pointer semantics, entity lifetime or native preconditions.

## WebView example

```rust
use rdr2_wasm::webview;

webview::open("https://example.invalid/ui", 0, 0)?;
webview::set_focus(true)?;
webview::show_cursor()?;

webview::post_json(r#"{"type":"state","open":true}"#)?;
if let Some(message) = webview::poll_json()? {
    // Handle one complete UTF-8 JSON value.
}

webview::hide_cursor()?;
webview::set_focus(false)?;
webview::set_visible(false)?;
# Ok::<(), webview::Error>(())
```

A zero width or height uses the current game client size. Prefer
`set_visible(false)` for menus that reopen frequently: it avoids rebuilding
Chromium and the GPU capture path. `show_cursor` acquires one per-mod reference;
every successful call must be paired with `hide_cursor`. Outstanding references
are released automatically during unload.

Only one WebView may currently be owned at a time. An owner may navigate it,
change visibility/focus, exchange JSON, or close it. Page JavaScript sends
messages with `window.chrome.webview.postMessage(value)` and receives host
messages through the WebView `message` event.

## JavaScript/Javy guest

The custom Javy plugin exposes host functions through `globalThis.RDR2Host`.
Javy modules use `init`, optional `tick`, and optional `shutdown` exports. See
`scripts/javy-wasm/README.md` and its trainer source for the supported bridge
methods and build flow.

## Lifecycle rules

- Keep per-frame work short; every guest export has a fuel budget.
- Balance cursor references and release focus before hiding interactive UI.
- Treat native wrappers as unsafe engine calls, even when Rust types compile.
- Do not retain raw game pointers across frames unless the native contract
  explicitly guarantees their lifetime.
- Use JSON messages as complete values; the host does not expose partial
  payloads.
