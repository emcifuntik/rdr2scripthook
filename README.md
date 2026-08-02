# RDR2 Script Hook

RDR2 Script Hook is a Windows x64 runtime for loading sandboxed WebAssembly
mods into Red Dead Redemption 2. It provides native calls, global access,
frame and keyboard events, timers, logging, and GPU-composited WebView2 user
interfaces on both Direct3D 12 and Vulkan.

The repository includes the injector, runtime DLL, Rust guest SDK, Rust and
JavaScript examples, generated native bindings, and a headless smoke-test host.

## Highlights

- Wasmtime-based mod isolation with ABI validation, memory limits and per-call
  execution fuel.
- Typed Rust wrappers generated from the RDR3 native database.
- Rust `wasm32-unknown-unknown` and Javy JavaScript guest examples.
- Transparent WebView2 overlays rendered after the game HUD without CPU frame
  copies.
- Direct3D 12 and Vulkan shared-texture synchronization.
- Overlay visibility, focus, native game cursor control and bidirectional JSON
  messaging.
- Persistent script-owned keyboard actions exposed in the game's Controls
  settings, with press/release events for WASM guests.
- Full-memory minidumps and structured runtime logging.

## Repository layout

| Path | Purpose |
| --- | --- |
| `launcher/` | Starts or attaches to RDR2 and injects the runtime. |
| `launcher-hook/` | Suspends a newly created game process until injection is complete. |
| `rdrhook/` | In-game runtime, RAGE integration, WASM host and WebView renderer. |
| `scripts/rdr2-wasm/` | Rust guest SDK and stable ABI wrappers. |
| `scripts/example-wasm/` | Rust WebView trainer with an F3 default binding. |
| `scripts/javy-wasm/` | JavaScript/Javy trainer with an F4 default binding. |
| `scripts/webview-example/` | Minimal WebView lifecycle and messaging example. |
| `shared/static/mods/` | Mod manifests and WASM files copied into build output. |
| `tools/codegen/` | Native-wrapper generator. |
| `wasmtest/` | Headless ABI and example-mod smoke tests. |
| `research/` | Reverse-engineering and engine-integration notes. |

## Quick start for developers

Use a Visual Studio developer PowerShell with CMake, Ninja, `clang-cl`, Python
3 and Rust available:

```powershell
git submodule update --init --recursive
cmake --preset windows-release
cmake --build --preset windows-release
& .\BIN\Release\wasmtest.exe
```

Release artifacts and the deployed `mods/` tree are written to `BIN/Release`.
Keep `launcher.exe`, `launcher-hook.dll`, `rdrhook.dll` and the `mods` directory
together. Start `launcher.exe`; pass `nolaunch` when the Rockstar launcher or
game is already being started separately.

The Microsoft Edge WebView2 Runtime must be installed for browser-backed UIs.
The project modifies a running game process and targets a specific executable
layout; use it only in environments where modding is permitted.

## Documentation

- [Building and testing](docs/building.md)
- [Runtime architecture](docs/architecture.md)
- [WASM mod and scripting API](docs/scripting.md)
- [WebView2 implementation notes](research/webview2-integration.md)
- [Native key-binding implementation notes](research/custom-key-bindings.md)

## License

The project is distributed under the [MIT License](LICENSE).
