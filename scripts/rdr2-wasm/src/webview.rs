//! Full-screen WebView2 overlay and its JSON message bridge.

use crate::ffi;

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum Error {
    InvalidArgument,
    RendererUnavailable,
    GameWindowUnavailable,
    GraphicsDeviceUnavailable,
    StartFailed,
    NotOpen,
    CursorNotOwned,
    TooManyCursorReferences,
    InvalidMessage,
    Host(i32),
}

fn status(value: i32) -> Result<(), Error> {
    match value {
        0 => Ok(()),
        -1 => Err(Error::InvalidArgument),
        -2 => Err(Error::RendererUnavailable),
        -3 => Err(Error::GameWindowUnavailable),
        -4 => Err(Error::GraphicsDeviceUnavailable),
        -5 => Err(Error::StartFailed),
        -6 => Err(Error::NotOpen),
        -7 => Err(Error::CursorNotOwned),
        -8 => Err(Error::TooManyCursorReferences),
        value => Err(Error::Host(value)),
    }
}

/// Opens or navigates the singleton overlay. A zero dimension uses the current
/// RDR2 client size.
pub fn open(url: &str, width: u32, height: u32) -> Result<(), Error> {
    if url.len() > i32::MAX as usize || width > i32::MAX as u32 || height > i32::MAX as u32 {
        return Err(Error::InvalidArgument);
    }
    status(unsafe {
        ffi::webview_open(url.as_ptr(), url.len() as i32, width as i32, height as i32)
    })
}

pub fn close() {
    unsafe { ffi::webview_close() }
}

pub fn is_open() -> bool {
    unsafe { ffi::webview_is_open() != 0 }
}

/// Shows or hides rendering immediately without destroying the browser.
/// Keeping a hidden WebView alive makes the next show operation instantaneous.
pub fn set_visible(visible: bool) -> Result<(), Error> {
    status(unsafe { ffi::webview_set_visible(i32::from(visible)) })
}

pub fn is_visible() -> bool {
    unsafe { ffi::webview_is_visible() != 0 }
}

pub fn set_focus(focused: bool) -> Result<(), Error> {
    status(unsafe { ffi::webview_set_focus(i32::from(focused)) })
}

pub fn is_focused() -> bool {
    unsafe { ffi::webview_is_focused() != 0 }
}

/// Acquires one cursor-visibility reference for this mod. Every successful
/// call must be paired with [`hide_cursor`]. Remaining references are released
/// automatically when the mod unloads or closes the WebView.
pub fn show_cursor() -> Result<(), Error> {
    status(unsafe { ffi::webview_show_cursor() })
}

/// Releases one cursor-visibility reference owned by this mod.
pub fn hide_cursor() -> Result<(), Error> {
    status(unsafe { ffi::webview_hide_cursor() })
}

/// Returns whether any loaded mod currently owns a cursor-visibility request.
pub fn is_cursor_visible() -> bool {
    unsafe { ffi::webview_is_cursor_visible() != 0 }
}

/// Returns this mod's outstanding cursor-visibility reference count.
pub fn cursor_ref_count() -> u32 {
    unsafe { ffi::webview_cursor_ref_count().max(0) as u32 }
}

pub fn navigate(url: &str) -> Result<(), Error> {
    if url.len() > i32::MAX as usize {
        return Err(Error::InvalidArgument);
    }
    status(unsafe { ffi::webview_navigate(url.as_ptr(), url.len() as i32) })
}

/// Sends a JSON value to `window.chrome.webview` in the page.
pub fn post_json(json: &str) -> Result<(), Error> {
    if json.len() > i32::MAX as usize {
        return Err(Error::InvalidArgument);
    }
    status(unsafe { ffi::webview_post_json(json.as_ptr(), json.len() as i32) })
}

/// Returns the next JSON value posted by page script with
/// `window.chrome.webview.postMessage(...)`.
pub fn poll_json() -> Result<Option<String>, Error> {
    let length = unsafe { ffi::webview_poll_json(std::ptr::null_mut(), 0) };
    if length == -1 {
        return Ok(None);
    }
    if length < 0 {
        return Err(Error::Host(length));
    }

    let mut bytes = vec![0u8; length as usize];
    let copied = unsafe { ffi::webview_poll_json(bytes.as_mut_ptr(), bytes.len() as i32) };
    if copied != length {
        return Err(Error::Host(copied));
    }
    String::from_utf8(bytes)
        .map(Some)
        .map_err(|_| Error::InvalidMessage)
}
