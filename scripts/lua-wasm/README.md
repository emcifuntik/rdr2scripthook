# Lua Wasmtime guest

Lua source is compiled by the separate sibling `lua-to-asm` project.
ScriptHook does not build or embed the Lua compiler. It consumes only the
resulting core Wasm module and executes it in a per-mod Wasmtime sandbox.

Build the compiler in the sibling `lua-to-asm` checkout, then synchronize the
example artifact:

```powershell
.\scripts\lua-wasm\sync.ps1
```

`compiler.lock.json` records the compiler commit, an exact source-tree
fingerprint (including uncommitted and untracked compiler sources), both ABI
versions, and the artifact SHA-256. When the sibling checkout is present,
`verify.ps1` checks that fingerprint too. CI runs the artifact check and the
normal embedded Wasmtime smoke test; it never recompiles Lua.

Lua scripts register lifecycle callbacks with `register_event("tick", fn)`,
`register_event("key_down", fn)`, `register_event("key_up", fn)`, and
`register_event("shutdown", fn)`.

The `rdr2` compiler profile also exposes capability globals for logging,
hashing, time and raw key queries; persistent input bindings; and the complete
WebView lifecycle, focus, cursor, navigation and JSON bridge. See
`docs/scripting.md` in ScriptHook for the function names.

The bundled example registers F6 and opens a styled Lua WebView with
focus and a reference-counted game cursor. F6 hides it again. The source
manifest is opt-in (`mod.toml.disabled`); after it is enabled in a `BIN`
output, `sync.ps1` refreshes that output's `main.wasm` automatically.
