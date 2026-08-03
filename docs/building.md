# Building and testing

## Requirements

- Windows x64
- Visual Studio C++ build tools and a developer PowerShell
- CMake 3.21 or newer
- Ninja and `clang-cl` on `PATH`
- Python 3 for native binding generation
- Git with submodule support
- Rust and the `wasm32-unknown-unknown` target for Rust guest examples
- PowerShell for the Javy build script
- .NET SDK 10.0.203 for the C# NativeAOT-LLVM example
- Microsoft Edge WebView2 Runtime on the game machine

Dependencies are declared in `vcpkg.json`. The checked-out `vcpkg` submodule
and `vcpkg-ports` overlay supply the CMake packages used by the presets.

## Configure and build

```powershell
git submodule update --init --recursive
cmake --preset windows-release
cmake --build --preset windows-release
```

For a debug build, replace `windows-release` with `windows-debug`. The presets
use Ninja, `clang-cl`, a static x64 vcpkg triplet, and `/MT` or `/MTd`.

The main targets are:

- `rdrhook.dll` — injected game runtime;
- `launcher.exe` — launcher and injector;
- `launcher-hook.dll` — process-creation hook;
- `wasmtest.exe` — headless WASM smoke test;
- `generate_natives` — regenerates `scripts/rdr2-wasm/src/natives.rs`;
- `javy_example` — builds and deploys the JavaScript example.
- `dotnet_example` — builds and deploys the C# NativeAOT-LLVM example.
- `lua_example_verify` — verifies the synchronized Lua artifact and ABI lock.

Artifacts are emitted to `BIN/Release` or `BIN/Debug`. CMake copies
`shared/static` into each output tree during configuration.

## Rust examples

Install the guest target once:

```powershell
rustup target add wasm32-unknown-unknown
```

Build the trainer:

```powershell
cargo build --manifest-path scripts/example-wasm/Cargo.toml `
  --target wasm32-unknown-unknown --release
```

The resulting module is
`scripts/example-wasm/target/wasm32-unknown-unknown/release/example_mod.wasm`.
Copy it to a mod directory as the filename declared by that directory's
`mod.toml`.

The minimal browser example builds the same way with
`scripts/webview-example/Cargo.toml`.

## JavaScript example

```powershell
.\scripts\javy-wasm\build.ps1
```

The first run builds the pinned Javy toolchain under `BUILD/`; later runs reuse
it. The script compiles the custom host plugin, writes the combined module, and
updates existing build output directories.

## C#/.NET example

```powershell
.\scripts\dotnet-wasm\build.ps1 -LockedMode
```

The script pins .NET SDK 10.0.203, NativeAOT-LLVM
10.0.0-rc.1.26357.1, and WASI SDK 29.0. The SDK archive is downloaded once
under `BUILD/dotnet-toolchain` and checksum-verified. Output is a WASI Preview
1 reactor core module; it contains no CoreCLR/Mono sidecar.

## Lua example

Lua compilation stays in the sibling `lua-to-asm` repository. After building
its compiler executable, synchronize the artifact explicitly:

```powershell
.\scripts\lua-wasm\sync.ps1
.\scripts\lua-wasm\verify.ps1
```

CI performs only the verification step and embedded Wasmtime smoke test. It
does not compile Lua or vendor the compiler into this repository.

## Verification

Run the native/WASM smoke test after a Release build:

```powershell
& .\BIN\Release\wasmtest.exe
if ($LASTEXITCODE -ne 0) { throw "wasmtest failed: $LASTEXITCODE" }
```

This verifies Wasmtime instantiation, the Rust ABI, representative native
marshalling, WebView host imports, the Javy lifecycle, C# NativeAOT exports,
the Lua lifecycle, import allowlists, and repeated ticks.
Graphics interop and live RAGE hooks still require in-game testing on both
Direct3D 12 and Vulkan.

## Runtime files and diagnostics

Keep the launcher, both DLLs and `mods/` tree in the same output directory.
The runtime writes `log.txt` beside the loaded DLL. Its unhandled-exception
filter writes a full-memory dump to the configured temporary location when it
receives an exception that reaches the filter.
