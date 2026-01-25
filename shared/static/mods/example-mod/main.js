/**
 * Example JavaScript Mod for RDR2 Script Hook
 *
 * This mod demonstrates how to use the ES6 module API with the callback system.
 *
 * Available modules:
 *
 * import natives from 'natives';
 *   - All 7000+ game natives with camelCase names
 *   - Example: natives.getGameTimer(), natives.playerPedId()
 *
 * import { addTickCallback, addKeyDownCallback, ... } from 'core';
 *   - addTickCallback(fn) -> id          Register tick handler (called every frame)
 *   - removeTickCallback(id)             Unregister tick handler
 *   - addKeyDownCallback(fn) -> id       Register key press handler
 *   - removeKeyDownCallback(id)          Unregister key press handler
 *   - addKeyUpCallback(fn) -> id         Register key release handler
 *   - removeKeyUpCallback(id)            Unregister key release handler
 *   - getGameTime() -> ms                Get current game time
 *
 * Global objects (standard browser-like APIs):
 *   - setTimeout(fn, ms) / clearTimeout(id)     One-shot timers
 *   - setInterval(fn, ms) / clearInterval(id)   Repeating timers
 *   - console.log/warn/error(...args)           Logging
 *   - Vector3(x, y, z)                          3D vector class
 *   - Hash.joaat(str)                           Calculate JOAAT hash
 *   - Native.invoke(hash, ...args)              Low-level native call
 *   - Global.getInt/setInt/getFloat/setFloat    Script globals access
 *   - VK_* constants                            Virtual key codes
 */

// ES6 module imports
import natives from 'natives';
import { addTickCallback, addKeyDownCallback, addKeyUpCallback } from 'core';

// State variables
let menuOpen = false;
let tickCounter = 0;

// Initialize the mod
console.log("=== Example JavaScript Mod Initializing ===");
console.log("Mod Name: " + __MOD_NAME__);
console.log("Mod Version: " + __MOD_VERSION__);
console.log("Mod Author: " + __MOD_AUTHOR__);
console.log("Press F4 to toggle the example menu");
console.log("==========================================");

/**
 * Tick handler - called every frame (~60 times per second)
 */
function onTick() {
    tickCounter++;

    // Only do expensive operations occasionally
    if (tickCounter % 60 === 0) {
        // Get game timer using the natives module
        const gameTimer = natives.getGameTimer();

        // Log every 5 seconds
        if (tickCounter % 300 === 0) {
            console.log("Game timer: " + gameTimer + "ms");
        }
    }

    // Example: Show menu if open
    if (menuOpen) {
        // You could draw menu elements here using native UI functions
    }
}

/**
 * Key down handler
 * @param {number} key - Virtual key code (VK_* constants)
 */
function onKeyDown(key) {
    // F4 - Toggle menu
    if (key === VK_F4) {
        menuOpen = !menuOpen;
        console.log("Menu " + (menuOpen ? "opened" : "closed"));
    }

    // F5 - Get player position
    if (key === VK_F5) {
        const playerPed = natives.playerPedId();
        const coords = natives.getEntityCoords(playerPed, true);
        console.log("Player position: " + coords.x.toFixed(2) + ", " + coords.y.toFixed(2) + ", " + coords.z.toFixed(2));
    }

    // F6 - Teleport player forward
    if (key === VK_F6) {
        const playerPed = natives.playerPedId();
        const coords = natives.getEntityCoords(playerPed, true);
        const heading = natives.getEntityHeading(playerPed);

        // Calculate forward direction
        const headingRad = heading * Math.PI / 180;
        const newX = coords.x - Math.sin(headingRad) * 5.0;
        const newY = coords.y + Math.cos(headingRad) * 5.0;

        // Teleport
        natives.setEntityCoords(playerPed, newX, newY, coords.z, false, false, false, false);
        console.log("Teleported forward!");
    }

    // F7 - Change time to noon
    if (key === VK_F7) {
        natives.setClockTime(12, 0, 0);
        console.log("Time set to 12:00");
    }

    // F8 - Get current time
    if (key === VK_F8) {
        const hours = natives.getClockHours();
        const minutes = natives.getClockMinutes();
        console.log("Current time: " + hours + ":" + (minutes < 10 ? "0" : "") + minutes);
    }

    // Numpad + - Boost vehicle speed
    if (key === VK_ADD) {
        const playerPed = natives.playerPedId();
        const inVehicle = natives.isPedInAnyVehicle(playerPed, false);

        if (inVehicle) {
            const vehicle = natives.getVehiclePedIsIn(playerPed, false);
            natives.setVehicleForwardSpeed(vehicle, 50.0);
            console.log("Vehicle boosted!");
        } else {
            console.log("Not in a vehicle");
        }
    }
}

/**
 * Key up handler
 * @param {number} key - Virtual key code
 */
function onKeyUp(key) {
    // Usually less important than onKeyDown, but useful for:
    // - Detecting when a key is held vs tapped
    // - Releasing continuous actions
}

// Register callbacks using the core module
const tickId = addTickCallback(onTick);
const keyDownId = addKeyDownCallback(onKeyDown);
const keyUpId = addKeyUpCallback(onKeyUp);

// Example: Use setTimeout to do something after a delay
setTimeout(() => {
    console.log("Mod fully initialized after 1 second delay");
}, 1000);

// Example: Use setInterval for periodic tasks (every 10 seconds)
const intervalId = setInterval(() => {
    console.log("Periodic check: " + tickCounter + " ticks elapsed");
}, 10000);

// To unregister callbacks later (e.g., in a shutdown function):
// removeTickCallback(tickId);
// removeKeyDownCallback(keyDownId);
// removeKeyUpCallback(keyUpId);
// clearInterval(intervalId);

console.log("Example mod script loaded successfully!");
console.log("Registered tick callback id=" + tickId + ", keyDown id=" + keyDownId);
