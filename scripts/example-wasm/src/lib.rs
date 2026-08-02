mod catalog;
mod trainer;
mod ui;

use rdr2_wasm::{event, log, metadata};

fn initialize() {
    log::info("=== RDR2 Rust WASM Trainer Initializing ===");
    log::info(format!("Mod Name: {}", metadata::name()));
    log::info(format!("Mod Version: {}", metadata::version()));
    log::info(format!("Mod Author: {}", metadata::author()));

    trainer::initialize();
    let tick_id = event::add_tick_callback(trainer::tick);
    let key_down_id = event::add_key_down_callback(trainer::key_down);

    log::info("Press F3 to open the WebView trainer");
    log::info("Use the mouse, search, or arrow keys; press Escape/F3 to close");
    log::info(format!(
        "Trainer loaded; tick callback id={tick_id}, key-down id={key_down_id}"
    ));
}

rdr2_wasm::entrypoint!(initialize);
