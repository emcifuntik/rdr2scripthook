# Javy WASM trainer

This example mirrors the Rust trainer while using JavaScript and F4. It is
compiled with Bytecode Alliance Javy v9.1.0 and a custom Javy plugin that
exposes the ScriptHook WASM host API through `globalThis.RDR2Host`.

WebView interaction is exposed as `webviewSetFocus(bool)`,
`webviewIsFocused()`, `webviewShowCursor()`, `webviewHideCursor()`,
`webviewIsCursorVisible()`, and `webviewCursorRefCount()`. Show/hide return a
status code and use per-mod reference counting; every successful show should be
balanced by a hide.

Build from PowerShell at the repository root:

```powershell
.\scripts\javy-wasm\build.ps1
```

The first build checks out and builds the pinned Javy release under `BUILD/`.
Later builds reuse that compiler. The script builds the custom plugin, combines
the source catalog and trainer, writes `BUILD/Javy/trainer.wasm`, updates the
static mod, and deploys it to existing Debug/Release output directories.

The generated module installs its catalog and trainer implementation once per
Wasmtime instance. Its exported functions are small wrappers, avoiding repeated
allocation of the full JavaScript module on every game tick.
The custom plugin also caches resolved QuickJS export functions inside the
runtime, because Javy's default invocation path reloads a module for every call.

Javy's generated WIT wrappers normally initialize passive bytecode data for a
single invocation. ScriptHook calls `init`, `tick`, and `shutdown` repeatedly,
so `javy-9.1-runtime.patch` makes those wrappers retain their allocations for
the lifetime of the Wasmtime instance. It also enables all WebAssembly features
while initializing plugins built by current Rust.
