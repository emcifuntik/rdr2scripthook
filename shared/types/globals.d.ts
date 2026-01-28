// Auto-generated TypeScript declarations for global types
// Do not edit manually - regenerate with tools/codegen/generate_natives.py

/** Hash utility for converting strings to game hashes */
declare const Hash: {
    /**
     * Convert a string to a joaat hash (Jenkins one-at-a-time)
     * @param str String to hash
     * @returns Hash value as number
     */
    joaat(str: string): number;
};

/** Global script variables access */
declare const Global: {
    /**
     * Get an integer from a global variable
     * @param index Global variable index
     * @returns The integer value
     */
    getInt(index: number): number;

    /**
     * Set an integer in a global variable
     * @param index Global variable index
     * @param value Value to set
     */
    setInt(index: number, value: number): void;

    /**
     * Get a float from a global variable
     * @param index Global variable index
     * @returns The float value
     */
    getFloat(index: number): number;

    /**
     * Set a float in a global variable
     * @param index Global variable index
     * @param value Value to set
     */
    setFloat(index: number, value: number): void;
};

// Virtual Key Codes
/** Backspace key */
declare const VK_BACK: 8;
/** Tab key */
declare const VK_TAB: 9;
/** Enter key */
declare const VK_RETURN: 13;
/** Shift key */
declare const VK_SHIFT: 16;
/** Ctrl key */
declare const VK_CONTROL: 17;
/** Alt key */
declare const VK_MENU: 18;
/** Pause key */
declare const VK_PAUSE: 19;
/** Caps Lock key */
declare const VK_CAPITAL: 20;
/** Escape key */
declare const VK_ESCAPE: 27;
/** Space key */
declare const VK_SPACE: 32;
/** Page Up key */
declare const VK_PRIOR: 33;
/** Page Down key */
declare const VK_NEXT: 34;
/** End key */
declare const VK_END: 35;
/** Home key */
declare const VK_HOME: 36;
/** Left Arrow key */
declare const VK_LEFT: 37;
/** Up Arrow key */
declare const VK_UP: 38;
/** Right Arrow key */
declare const VK_RIGHT: 39;
/** Down Arrow key */
declare const VK_DOWN: 40;
/** Insert key */
declare const VK_INSERT: 45;
/** Delete key */
declare const VK_DELETE: 46;
/** F1 key */
declare const VK_F1: 112;
/** F2 key */
declare const VK_F2: 113;
/** F3 key */
declare const VK_F3: 114;
/** F4 key */
declare const VK_F4: 115;
/** F5 key */
declare const VK_F5: 116;
/** F6 key */
declare const VK_F6: 117;
/** F7 key */
declare const VK_F7: 118;
/** F8 key */
declare const VK_F8: 119;
/** F9 key */
declare const VK_F9: 120;
/** F10 key */
declare const VK_F10: 121;
/** F11 key */
declare const VK_F11: 122;
/** F12 key */
declare const VK_F12: 123;
/** Numpad 0 key */
declare const VK_NUMPAD0: 96;
/** Numpad 1 key */
declare const VK_NUMPAD1: 97;
/** Numpad 2 key */
declare const VK_NUMPAD2: 98;
/** Numpad 3 key */
declare const VK_NUMPAD3: 99;
/** Numpad 4 key */
declare const VK_NUMPAD4: 100;
/** Numpad 5 key */
declare const VK_NUMPAD5: 101;
/** Numpad 6 key */
declare const VK_NUMPAD6: 102;
/** Numpad 7 key */
declare const VK_NUMPAD7: 103;
/** Numpad 8 key */
declare const VK_NUMPAD8: 104;
/** Numpad 9 key */
declare const VK_NUMPAD9: 105;
/** Numpad * key */
declare const VK_MULTIPLY: 106;
/** Numpad + key */
declare const VK_ADD: 107;
/** Numpad - key */
declare const VK_SUBTRACT: 109;
/** Numpad . key */
declare const VK_DECIMAL: 110;
/** Numpad / key */
declare const VK_DIVIDE: 111;

// Letter keys (A-Z are 0x41-0x5A)
declare const VK_A: 65;
declare const VK_B: 66;
declare const VK_C: 67;
declare const VK_D: 68;
declare const VK_E: 69;
declare const VK_F: 70;
declare const VK_G: 71;
declare const VK_H: 72;
declare const VK_I: 73;
declare const VK_J: 74;
declare const VK_K: 75;
declare const VK_L: 76;
declare const VK_M: 77;
declare const VK_N: 78;
declare const VK_O: 79;
declare const VK_P: 80;
declare const VK_Q: 81;
declare const VK_R: 82;
declare const VK_S: 83;
declare const VK_T: 84;
declare const VK_U: 85;
declare const VK_V: 86;
declare const VK_W: 87;
declare const VK_X: 88;
declare const VK_Y: 89;
declare const VK_Z: 90;

// Number keys (0-9 are 0x30-0x39)
declare const VK_0: 48;
declare const VK_1: 49;
declare const VK_2: 50;
declare const VK_3: 51;
declare const VK_4: 52;
declare const VK_5: 53;
declare const VK_6: 54;
declare const VK_7: 55;
declare const VK_8: 56;
declare const VK_9: 57;
