# .NET Wasmtime guest SDK

This directory compiles C# ahead of time into one self-contained
`wasi-wasm` core module. RDR2 Script Hook loads that module into the same
per-mod Wasmtime sandbox used by Rust, JavaScript, and Lua guests. CoreCLR,
hostfxr, Mono, managed assemblies, and runtime compilation are not used in the
game process.

Build the example with:

```powershell
.\scripts\dotnet-wasm\build.ps1
```

The build is pinned to .NET SDK 10.0.203 and NativeAOT-LLVM
10.0.0-rc.1.26357.1. NativeAOT-LLVM is experimental, so both versions must be
updated deliberately and validated by `wasmtest` before release.

The first build downloads the official x86-64 Windows WASI SDK 29.0 into
`BUILD/dotnet-toolchain` and verifies its SHA-256 digest. Set `WASI_SDK_PATH`
to reuse an existing complete SDK. A standalone sysroot can be supplied with
`-WasiSysroot`, provided compatible `clang`, `llvm-ar`, libc++, and compiler-rt
are available.

Guest exports must be static `[UnmanagedCallersOnly]` methods and must not let
exceptions escape. Host calls use `[WasmImportLinkage]` and the `rdr2` import
module. Reference types are never passed across the boundary; strings use
UTF-8 spans backed by guest linear memory.

The bundled example registers F5 and opens a visible test WebView with focus
and a reference-counted game cursor. Press F5 again to hide it. Its source
manifest is opt-in (`mod.toml.disabled`); once an active `mod.toml` exists in a
`BIN` output, `build.ps1` refreshes that output's `main.wasm` automatically.
