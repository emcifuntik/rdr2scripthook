// Auto-generated TypeScript declarations for core module
// Do not edit manually - regenerate with tools/codegen/generate_natives.py

declare module "core" {
    /** Callback function type for tick callbacks */
    type TickCallback = () => void;

    /** Callback function type for key callbacks */
    type KeyCallback = (key: number) => void;

    /**
     * Register a callback to be called every game tick
     * @param callback Function to call every tick
     */
    export function addTickCallback(callback: TickCallback): void;

    /**
     * Register a callback to be called when a key is pressed
     * @param callback Function to call with the virtual key code
     */
    export function addKeyDownCallback(callback: KeyCallback): void;

    /**
     * Register a callback to be called when a key is released
     * @param callback Function to call with the virtual key code
     */
    export function addKeyUpCallback(callback: KeyCallback): void;
}
