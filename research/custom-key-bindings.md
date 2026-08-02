# Native script key bindings

## Implemented contract

The runtime owns a registry of named keyboard actions. A registration contains
the owning mod, stable action ID, display description, default mapping, current
mapping, held state, and a bounded Down/Up event queue. Mapping overrides are
stored in `input-bindings.toml` beside the runtime DLL.

The WASM ABI exposes registration, unregistration, event polling, current
state, mapping query, remapping, and reset-to-default. Rust wraps registrations
in an RAII `Binding`; the Javy bridge exposes the equivalent host functions.
Runtime ownership and queued input state are released on unload. A manifest
declaration remains reserved so the cached native row stays valid if the mod is
loaded again.

## Keyboard state

RAGE publishes keyboard state through two 256-byte buffers and atomically
switches the active index after an update. The runtime resolves those buffers
from a version-tolerant instruction pattern, copies the current snapshot once
per script tick, and retries once if publication races the copy. A compatibility
fallback is used if a future executable can no longer be resolved. Buffer
indices are RAGE/DirectInput scan codes (for example F3 is `0x3D`), not Win32
virtual-key codes (`VK_F3` is `0x72`); the binding manager converts explicitly
at its public/persistence boundary.

## Native Controls menu

The RDR2 1.0.1491.50 retail control table contains 785 entries (`0` through
`784`). Script rows use a parallel array of 2048 synthetic UI IDs (`785`
through `2832`). Those IDs are intercepted before the retail registry or its
fixed-size arrays can index them. The supplemental entries are owned entirely
by `InputBindingManager`; the bridge does not construct, clone, or alias a
`CControl` or manufacture a native input object.

The game caches both category enumeration and its control row map before the
WASM script thread starts, so `[[input.bindings]]` declarations are scanned
during DLL initialization. Jitasm bridges make the menu consume a parallel
category/control array. RAGE constructs the visible row and its normal UI
widgets; after the retail refresh returns, the bridge keeps both mapping columns
interactive. The mapping renderer is intercepted only for the supplemental ID
range and appends the current Win32 virtual-key parameter through the game's
native keyboard-glyph formatter. RDR2 1.0.1491.50's Controls frontend uses
device `0` for keyboard mapping expressions, lookup, capture, and commit. Only
the per-frame RAGE keyboard-state buffer uses the DirectInput-compatible
scan-code representation; conversion happens at that runtime polling boundary.

RAGE refuses to enter its keyboard-capture state unless the selected action
owns a ref-counted native mapping object. For a supplemental action, the start
bridge lets the engine retain an existing retail keyboard mapping solely as a
capture sentinel and immediately changes the capture state's control ID back
to the supplemental ID. Native polling and confirmation then proceed normally;
the intercepted `ApplyBinding` request persists the captured key in the binding
manager and never mutates the sentinel. The bridges:

1. prepend a `Script Bindings` category to mapping-category enumeration;
2. append the predeclared synthetic control IDs to that category;
3. provide stable control names and localized action descriptions;
4. resolve each primary and alternate key through the stock keyboard-glyph
   formatter;
5. initialize native remap capture with a retained retail mapping sentinel;
6. intercept synthetic `ApplyBinding` requests and persist the new key in the
   requested primary or alternate slot;
7. isolate the retail conflict builder and fixed 785-action enabled-mask logic
   from the supplemental range;
8. return a safe null result for synthetic retail binding-object lookups.

The executable functions are resolved by unique byte patterns during DLL
initialization only. No signature search runs from a game callback. All
function bridges are installed as one MinHook transaction, followed by one
Jitasm inline bridge in the row builder. If any anchor is missing, no partial
menu integration is enabled; the scripting registry remains usable.

## Current limits

- Keyboard bindings only; settings and persistence use Win32 virtual keys,
  which are converted to RAGE scan codes for runtime polling.
- Two keyboard mappings (primary and alternate) per action.
- At most 2048 manifest-declared actions can have native Controls rows.
- Runtime-only actions registered after the frontend builds its cached rows
  continue to work through the scripting APIs but are not added retroactively
  to the native settings page.
- Mouse, wheel, and gamepad mapper support can be added without changing the
  public action/event model.
