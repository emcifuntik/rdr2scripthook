# WebView2 integration notes

## Scope and current state

RDR2 Script Hook hosts one transparent, full-screen WebView2 overlay, draws it
after the game frontend, forwards focused input, and exposes lifecycle and JSON
messaging to WebAssembly mods. Both RDR2 graphics backends are supported:

- Direct3D 12 imports the WebView capture texture and fences directly.
- Vulkan imports the same D3D11 shared texture as external memory and imports
  the D3D11 fences as external semaphores.

The implementation is GPU-only. It does not copy frames through system memory.

## Verified RDR2 anchors

Addresses refer to the analyzed `RDR2_dump.exe` image at base `0x140000000`.
Names were applied only after signature and behavioral validation.

| Address | IDA name | Role |
| --- | --- | --- |
| `0x1425FB89C` | `rage_sga_EndDraw` | Ends the engine draw and provides the post-frontend insertion point. |
| `0x1426314F0` | `rage_sga_WindowProc` | Dispatches the game window's `WM_*` messages. |
| `0x14060EF40` | `CDrawListMgr_Initialise` | Initializes draw-list metadata, including HUD and frontend phases. |
| `0x1425F777C` | `rage_sga_Factory_CreateTexture` | Creates an engine texture from image parameters. |
| `0x1425C7E60` | `rage_grcBegin` | Starts an immediate-mode draw. |
| `0x1425C9944` | `rage_grcVertex` | Appends a vertex to the current draw. |
| `0x1425C97D0` | `rage_grcEnd` | Submits the immediate-mode draw. |
| `0x142581C6C` | `rage_ioInput_RecaptureLostDevices` | Reacquires a lost DirectInput device. |

Runtime code still resolves these paths by signatures and validates pointers;
the absolute addresses are research references, not hard-coded production
dependencies.

## Render path

The post-frontend hook completes the current engine draw, binds the swap-chain
backbuffer, invokes the overlay callback, and restores the previous render
targets. `RageUiRenderer` uses the engine's unlit immediate-mode path to draw a
textured rectangle while preserving the required rasterizer, blend, and depth
state.

```text
WebView2 composition visual
    -> Windows.Graphics.Capture frame (BGRA D3D11 texture)
    -> reusable NT shared handle and shared D3D11 fences
    -> D3D12 resource/fence import OR Vulkan image/semaphore import
    -> RAGE texture wrapper and shader-resource view
    -> post-frontend textured rectangle
```

Capture surfaces form a three-entry ring. Imports are cached by surface
identity and generation; resize replaces the complete generation instead of
allocating a shared handle every frame.

## Direct3D 12 synchronization

The renderer opens the shared texture on the game's D3D12 device. Before the
draw, the direct queue waits for the capture producer-fence value. After the
engine has submitted the draw, the queue signals the consumer fence so the
D3D11 capture thread may reuse that ring entry.

## Vulkan synchronization

Vulkan initialization is observed early enough to append the required external
memory and semaphore extensions to device creation. The hook records the
created `VkDevice`, graphics queue, adapter LUID, and original queue submission
function.

The D3D11 texture is imported with
`VK_EXTERNAL_MEMORY_HANDLE_TYPE_D3D11_TEXTURE_BIT`; producer and consumer
fences are imported with
`VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_D3D11_FENCE_BIT`. Queue operations must obey
Vulkan's external-synchronization rule, so WebView waits and signals are folded
into empty submissions executed from the game's intercepted `vkQueueSubmit`
thread. They are never submitted directly from the render callback. The
consumer signal is delayed until a later engine submission so the texture is
not recycled while its draw may still be in flight.

## Thread ownership

| Thread | Responsibilities |
| --- | --- |
| Browser STA | COM/WinRT, WebView2, composition tree, capture pool, navigation, page messages and browser teardown. |
| Game render | RAGE texture wrappers, backend imports, render state and draw recording. |
| Vulkan submit | Imported-semaphore waits and signals integrated with engine queue submissions. |
| Game script | Mod API, overlay ownership, visibility/focus requests and browser event polling. |

Cross-thread state is either immutable frame metadata or a queued command.
The render thread never calls WebView2 UI APIs, and the browser thread never
creates RAGE resources.

## Input behavior

- The game window is the single Win32 input source.
- Mouse messages are translated to overlay coordinates and forwarded through
  `ICoreWebView2CompositionController::SendMouseInput`.
- Keyboard focus is assigned to the WebView controller while a mod requests
  focus.
- Focused overlays suppress matching game controls and request the native RDR2
  cursor. Cursor visibility is reference-counted per mod.
- Hiding the overlay releases focus and cursor ownership without destroying the
  browser, making repeated menu opens immediate.

## Scripting surface

The host currently exposes one overlay. The Rust SDK wraps these imports in
`rdr2_wasm::webview`:

- open, close, show/hide, navigate and focus operations;
- per-mod cursor acquire/release and status queries;
- page-to-host and host-to-page JSON messages.

Page messages use `window.chrome.webview.postMessage`; host messages use
`PostWebMessageAsJson`. The host retains an incoming message until the guest
provides a buffer large enough for the entire UTF-8 JSON value.

## Remaining work

- Replace the singleton overlay with explicit per-mod browser handles.
- Add an origin policy for remote navigation and page messages.
- Harden device-loss and live graphics-backend reset handling.
- Replace the conservative Vulkan reset barrier with fully deferred resource
  retirement.
- Complete gamepad and IME input validation.

## Platform references

- [WebView2 composition controller](https://learn.microsoft.com/microsoft-edge/webview2/reference/win32/icorewebview2compositioncontroller)
- [WebView2 controller focus](https://learn.microsoft.com/microsoft-edge/webview2/reference/win32/icorewebview2controller)
- [D3D12 shared handles](https://learn.microsoft.com/windows/win32/api/d3d12/nf-d3d12-id3d12device-createsharedhandle)
- [Vulkan external memory on Windows](https://registry.khronos.org/vulkan/specs/latest/man/html/VK_KHR_external_memory_win32.html)
- [Vulkan external semaphores on Windows](https://registry.khronos.org/vulkan/specs/latest/man/html/VK_KHR_external_semaphore_win32.html)
