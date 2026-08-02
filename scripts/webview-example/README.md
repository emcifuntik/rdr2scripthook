# WebView2 WASM example

This mod opens a transparent, full-screen composition WebView, forwards focus
to it, and demonstrates JSON in both directions. Press F8 to move focus between
the page and RDR2. The example acquires exactly one cursor visibility reference
while focused and releases it when focus returns to the game.

Build it from the repository root:

```text
cargo build --manifest-path scripts/webview-example/Cargo.toml --target wasm32-unknown-unknown --release
```

Copy `target/wasm32-unknown-unknown/release/webview_example.wasm` into a mod
folder as `main.wasm` and use this manifest:

```toml
[mod]
name = "WebView2 Example"
version = "0.1.0"
author = "RDR2 Script Hook"
description = "Transparent WebView2 render, input, and JSON bridge test"
entrypoint = "main.wasm"
```
