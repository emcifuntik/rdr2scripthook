use rdr2_wasm::{event, log, webview};
use std::sync::atomic::{AtomicBool, Ordering};

const VK_F8: u32 = 0x77;
static FOCUSED: AtomicBool = AtomicBool::new(false);
static CURSOR_HELD: AtomicBool = AtomicBool::new(false);

const PAGE: &str = r#"<!doctype html>
<meta charset="utf-8">
<style>
  :root { color-scheme: dark; font: 16px/1.4 system-ui, sans-serif; }
  html, body { margin: 0; width: 100%; height: 100%; background: transparent; }
  body { display: grid; place-items: center; }
  main { width: 360px; padding: 24px; border: 1px solid #ffffff38;
         border-radius: 18px; background: #10141de8; box-shadow: 0 18px 60px #0008; }
  h1 { margin: 0 0 8px; font-size: 24px; }
  p { color: #b9c1d1; }
  button { border: 0; border-radius: 10px; padding: 11px 16px; color: white;
           background: #c43e35; cursor: pointer; font-weight: 700; }
  code { color: #f0b9a8; }
</style>
<main>
  <h1>RDR2 WebView2</h1>
  <p>This transparent UI is rendered through RAGE after the HUD.</p>
  <button id="ping">Send JSON to WASM</button>
  <p id="status">Press <code>F8</code> to release or restore UI focus.</p>
</main>
<script>
  const status = document.querySelector('#status');
  document.querySelector('#ping').onclick = () =>
    chrome.webview.postMessage({ type: 'ping', sentAt: Date.now() });
  chrome.webview.addEventListener('message', event => {
    status.textContent = `Host replied: ${JSON.stringify(event.data)}`;
  });
</script>"#;

fn data_url(html: &str) -> String {
    const HEX: &[u8; 16] = b"0123456789ABCDEF";
    let mut result = String::from("data:text/html;charset=utf-8,");
    for byte in html.bytes() {
        if byte.is_ascii_alphanumeric() || matches!(byte, b'-' | b'_' | b'.' | b'~') {
            result.push(byte as char);
        } else {
            result.push('%');
            result.push(HEX[(byte >> 4) as usize] as char);
            result.push(HEX[(byte & 0x0F) as usize] as char);
        }
    }
    result
}

fn tick() {
    match webview::poll_json() {
        Ok(Some(message)) => {
            log::info(format!("WebView says: {message}"));
            let _ = webview::post_json(r#"{"type":"pong","from":"rdr2-wasm"}"#);
        }
        Ok(None) => {}
        Err(error) => log::error(format!("WebView poll failed: {error:?}")),
    }
}

fn set_interaction(focused: bool) {
    if let Err(error) = webview::set_focus(focused) {
        log::error(format!("Could not change WebView focus: {error:?}"));
        return;
    }

    FOCUSED.store(focused, Ordering::Relaxed);
    if focused {
        if !CURSOR_HELD.swap(true, Ordering::Relaxed) {
            if let Err(error) = webview::show_cursor() {
                CURSOR_HELD.store(false, Ordering::Relaxed);
                log::error(format!("Could not show WebView cursor: {error:?}"));
            }
        }
    } else if CURSOR_HELD.swap(false, Ordering::Relaxed) {
        if let Err(error) = webview::hide_cursor() {
            log::error(format!("Could not hide WebView cursor: {error:?}"));
        }
    }
}

fn key_down(key: u32) {
    if key != VK_F8 {
        return;
    }
    let was_focused = FOCUSED.load(Ordering::Relaxed);
    // When WebView2 consumes F8, the host releases focus before synthesizing
    // this guest key event. Treat that as a release, not a second toggle.
    let focused = if was_focused && !webview::is_focused() {
        false
    } else {
        !was_focused
    };
    set_interaction(focused);
}

fn initialize() {
    match webview::open(&data_url(PAGE), 0, 0) {
        Ok(()) => {
            set_interaction(true);
            event::add_tick_callback(tick);
            event::add_key_down_callback(key_down);
            log::info("WebView2 example opened; press F8 to toggle focus");
        }
        Err(error) => log::error(format!("Could not open WebView2: {error:?}")),
    }
}

rdr2_wasm::entrypoint!(initialize);
