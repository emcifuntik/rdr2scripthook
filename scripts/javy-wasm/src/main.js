const VK = {
    BACK: 0x08,
    RETURN: 0x0d,
    PRIOR: 0x21,
    NEXT: 0x22,
    LEFT: 0x25,
    UP: 0x26,
    RIGHT: 0x27,
    DOWN: 0x28,
    ESCAPE: 0x1b,
    F4: 0x73,
};

const SCREEN = {
    MAIN: 0,
    PLAYER: 1,
    HORSES: 2,
    PLAYER_MODELS: 3,
    PEDS: 4,
    VEHICLES: 5,
    TELEPORTS: 6,
    WEATHER: 7,
    TIME: 8,
};

const SCREEN_TITLES = [
    "RDR2 JAVY TRAINER",
    "PLAYER OPTIONS",
    "SPAWN HORSE",
    "CHANGE PLAYER MODEL",
    "SPAWN PED",
    "SPAWN VEHICLE",
    "TELEPORT",
    "WEATHER",
    "TIME",
];

const PLAYER_MODEL_CHANGE_GLOBAL = 1835009;
const VISIBLE_ROWS = 11;
const MODEL_LOAD_TIMEOUT_TICKS = 900;
const STATUS_LIFETIME_TICKS = 300;
let toggleBinding = 0;

function hash64(value) {
    const compact = value.replace(/^0x/, "").replace(/_/g, "").padStart(16, "0");
    return [parseInt(compact.slice(0, 8), 16) | 0, parseInt(compact.slice(8), 16) | 0];
}

const NATIVE = {
    ADD_TO_CLOCK_TIME: hash64("AB7C251C7701D336"),
    DOES_ENTITY_EXIST: hash64("D42BD6EB2E0F1677"),
    GET_ENTITY_COORDS: hash64("A86D5F069399F44D"),
    GET_ENTITY_HEADING: hash64("C230DD956E2F5507"),
    GET_ENTITY_MAX_HEALTH: hash64("15D757606D170C3C"),
    PLACE_ENTITY_ON_GROUND_PROPERLY: hash64("9587913B9E772D29"),
    SET_ENTITY_ALPHA: hash64("0DF7692B1D9E7BA7"),
    SET_ENTITY_COORDS_NO_OFFSET: hash64("239A3351AC1DA385"),
    SET_ENTITY_HEALTH: hash64("AC2767ED8BDFAB15"),
    SET_ENTITY_VISIBLE: hash64("1794B4FCC84D812F"),
    DRAW_RECT: hash64("405224591DF02025"),
    SET_CURR_WEATHER_STATE: hash64("FA3E3CA8A1DE6D5D"),
    CREATE_PED: hash64("D49F9B0955C367DE"),
    SET_BLOCKING_OF_NON_TEMPORARY_EVENTS: hash64("9F8AA94D6D97DBF4"),
    SET_RANDOM_OUTFIT_VARIATION: hash64("283978A15512B2FE"),
    PLAYER_ID: hash64("217E9DC48139933D"),
    PLAYER_PED_ID: hash64("096275889B8E0EE0"),
    RESTORE_PLAYER_STAMINA: hash64("C41F4B6E23FE6A4A"),
    SET_PLAYER_INVINCIBLE: hash64("FEBEEBC9CBDF4B12"),
    SET_PLAYER_MODEL: hash64("ED40380076A31506"),
    HAS_MODEL_LOADED: hash64("1283B8B89DD5D1B6"),
    IS_MODEL_VALID: hash64("392C8D8E07B70EFC"),
    REQUEST_MODEL: hash64("FA28FE3A6246FC30"),
    SET_MODEL_AS_NO_LONGER_NEEDED: hash64("4AD96EF928BD4F9A"),
    TASK_WANDER_STANDARD: hash64("BB9CE077274F6A1B"),
    CREATE_VEHICLE: hash64("AF35D0D2583051B0"),
    DELETE_VEHICLE: hash64("E20A909D8C4A70F8"),
    SET_VEHICLE_ON_GROUND_PROPERLY: hash64("7263332501E07F52"),
    VAR_STRING: hash64("FA925AC00EB830B9"),
    BG_SET_TEXT_COLOR: hash64("16FA5CE47F184F1E"),
    BG_SET_TEXT_SCALE: hash64("A1253A3C870B6843"),
    BG_DISPLAY_TEXT: hash64("16794E044C9EFB58"),
};

const NATIVE_ERRORS = [
    "Ok",
    "InvalidMemory",
    "InvalidArgument",
    "BridgeUnavailable",
    "NotFound",
    "Crashed",
    "TooManyArguments",
];

const arg = {
    raw: (value) => ["raw", value],
    raw64: (high, low) => ["raw64", high >>> 0, low >>> 0],
    int: (value) => ["int", value | 0],
    hash: (value) => ["hash", value >>> 0],
    bool: (value) => ["bool", !!value],
    float: (value) => ["float", Number(value)],
    string: (value) => ["string", String(value)],
    inout32: (value) => ["inout32", value | 0],
};

function invoke(operation, hash, argumentsList = []) {
    let result;
    try {
        result = JSON.parse(RDR2Host.native(hash[0], hash[1], JSON.stringify(argumentsList)));
    } catch (error) {
        throw new Error(`${operation} bridge failed: ${error}`);
    }

    if (result.bridgeError) {
        throw new Error(`${operation} bridge failed: ${result.bridgeError}`);
    }
    if (result.status !== 0) {
        const detail = NATIVE_ERRORS[result.status] || `Unknown(${result.status})`;
        throw new Error(`${operation} failed: native call failed: ${detail}`);
    }
    return result;
}

function resultInt(result) {
    return result.low | 0;
}

function resultBool(result) {
    return result.low !== 0;
}

function joaat(value) {
    return RDR2Host.joaat(value) >>> 0;
}

function logInfo(message) {
    RDR2Host.log(0, message);
}

function logWarn(message) {
    RDR2Host.log(1, message);
}

function logError(message) {
    RDR2Host.log(2, message);
}

function openAction(screen) {
    return { kind: "open", screen };
}

function modelAction(model, purpose) {
    return { kind: "model", model, purpose };
}

const MAIN_ENTRIES = [
    { label: "Player options  >", action: openAction(SCREEN.PLAYER) },
    { label: "Spawn horse  >", action: openAction(SCREEN.HORSES) },
    { label: "Change player model  >", action: openAction(SCREEN.PLAYER_MODELS) },
    { label: "Spawn ped  >", action: openAction(SCREEN.PEDS) },
    { label: "Spawn vehicle  >", action: openAction(SCREEN.VEHICLES) },
    { label: "Teleport  >", action: openAction(SCREEN.TELEPORTS) },
    { label: "Weather  >", action: openAction(SCREEN.WEATHER) },
    { label: "Time  >", action: openAction(SCREEN.TIME) },
];

const PLAYER_ENTRIES = [
    { label: "Restore health", action: { kind: "heal" } },
    { label: "Restore stamina", action: { kind: "stamina" } },
    { label: "Toggle invincibility", action: { kind: "invincibility" } },
];

const TIME_ENTRIES = [
    { label: "+ 4 hours", action: { kind: "time", hours: 4, minutes: 0 } },
    { label: "+ 1 hour", action: { kind: "time", hours: 1, minutes: 0 } },
    { label: "+ 15 minutes", action: { kind: "time", hours: 0, minutes: 15 } },
    { label: "- 4 hours", action: { kind: "time", hours: -4, minutes: 0 } },
    { label: "- 1 hour", action: { kind: "time", hours: -1, minutes: 0 } },
    { label: "- 15 minutes", action: { kind: "time", hours: 0, minutes: -15 } },
];

const SCREEN_ENTRIES = [
    MAIN_ENTRIES,
    PLAYER_ENTRIES,
    HORSES.map((item) => ({ label: item[0], action: modelAction(item[1], "horse") })),
    CHARACTERS.map((item) => ({ label: item[0], action: modelAction(item[1], "player model") })),
    CHARACTERS.map((item) => ({ label: item[0], action: modelAction(item[1], "ped") })),
    VEHICLES.map((item) => ({ label: item[0], action: modelAction(item[1], "vehicle") })),
    TELEPORTS.map((item) => ({
        label: item[0],
        action: { kind: "teleport", x: item[1], y: item[2], z: item[3] },
    })),
    WEATHER.map((name) => ({ label: name, action: { kind: "weather", name } })),
    TIME_ENTRIES,
];

const trainer = {
    open: false,
    screen: SCREEN.MAIN,
    cursors: new Array(SCREEN_TITLES.length).fill(0),
    pendingModels: [],
    lastVehicle: 0,
    invincible: false,
    status: null,
    drawErrorReported: false,
};

function hasParent(screen = trainer.screen) {
    return screen !== SCREEN.MAIN;
}

function itemCount() {
    return SCREEN_ENTRIES[trainer.screen].length + (hasParent() ? 1 : 0);
}

function cursor() {
    return trainer.cursors[trainer.screen];
}

function moveCursor(amount) {
    const count = itemCount();
    if (count === 0) return;
    trainer.cursors[trainer.screen] = ((cursor() + amount) % count + count) % count;
}

function goBack() {
    if (hasParent()) {
        trainer.screen = SCREEN.MAIN;
    } else {
        trainer.open = false;
    }
}

function setStatus(message) {
    logInfo(message);
    trainer.status = { text: message, remainingTicks: STATUS_LIFETIME_TICKS };
}

function reportError(error) {
    const detail = error && error.message ? error.message : String(error);
    const message = `Trainer action failed: ${detail}`;
    logError(message);
    trainer.status = { text: message, remainingTicks: STATUS_LIFETIME_TICKS };
}

function queueModel(name, purpose) {
    const model = joaat(name);
    if (trainer.pendingModels.some((pending) => pending.hash === model && pending.purpose === purpose)) {
        setStatus(`Already loading ${name}`);
        return;
    }

    try {
        if (!resultBool(invoke("IS_MODEL_VALID", NATIVE.IS_MODEL_VALID, [arg.hash(model)]))) {
            throw new Error("Game rejected the selected model");
        }
        invoke("REQUEST_MODEL", NATIVE.REQUEST_MODEL, [arg.hash(model), arg.bool(false)]);
        trainer.pendingModels.push({ name, hash: model, purpose, waitedTicks: 0 });
        setStatus(`Loading ${purpose}: ${name}`);
    } catch (error) {
        reportError(error);
    }
}

function playerSpawnPoint(distance) {
    const playerPed = resultInt(invoke("PLAYER_PED_ID", NATIVE.PLAYER_PED_ID));
    const coords = invoke("GET_ENTITY_COORDS", NATIVE.GET_ENTITY_COORDS, [
        arg.int(playerPed), arg.bool(true), arg.bool(true),
    ]).vector;
    const heading = invoke("GET_ENTITY_HEADING", NATIVE.GET_ENTITY_HEADING, [arg.int(playerPed)]).float;
    const radians = heading * Math.PI / 180;
    return {
        x: coords[0] - Math.sin(radians) * distance,
        y: coords[1] + Math.cos(radians) * distance,
        z: coords[2],
        heading,
    };
}

function createPed(model, position) {
    return resultInt(invoke("CREATE_PED", NATIVE.CREATE_PED, [
        arg.hash(model),
        arg.float(position.x), arg.float(position.y), arg.float(position.z),
        arg.float(position.heading),
        arg.bool(false), arg.bool(false), arg.bool(false), arg.bool(false),
    ]));
}

function completeModelAction(pending) {
    if (pending.purpose === "horse") {
        const position = playerSpawnPoint(3.5);
        position.z += 0.5;
        const horse = createPed(pending.hash, position);
        if (horse === 0) throw new Error("Game did not create the horse");
        invoke("SET_ENTITY_VISIBLE", NATIVE.SET_ENTITY_VISIBLE, [arg.int(horse), arg.bool(true)]);
        invoke("SET_ENTITY_ALPHA", NATIVE.SET_ENTITY_ALPHA, [arg.int(horse), arg.int(255), arg.bool(false)]);
        invoke("SET_RANDOM_OUTFIT_VARIATION", NATIVE.SET_RANDOM_OUTFIT_VARIATION, [arg.int(horse), arg.bool(true)]);
        invoke("SET_BLOCKING_OF_NON_TEMPORARY_EVENTS", NATIVE.SET_BLOCKING_OF_NON_TEMPORARY_EVENTS, [arg.int(horse), arg.bool(true)]);
        try {
            invoke("PLACE_ENTITY_ON_GROUND_PROPERLY", NATIVE.PLACE_ENTITY_ON_GROUND_PROPERLY, [arg.int(horse), arg.bool(true)]);
        } catch (_) {
            // Ground placement is best-effort, matching the Rust trainer.
        }
        return `Spawned horse: ${pending.name}`;
    }

    if (pending.purpose === "player model") {
        RDR2Host.globalSetInt(PLAYER_MODEL_CHANGE_GLOBAL, 1);
        const player = resultInt(invoke("PLAYER_ID", NATIVE.PLAYER_ID));
        invoke("SET_PLAYER_MODEL", NATIVE.SET_PLAYER_MODEL, [arg.int(player), arg.hash(pending.hash), arg.bool(true)]);
        const playerPed = resultInt(invoke("PLAYER_PED_ID", NATIVE.PLAYER_PED_ID));
        invoke("SET_RANDOM_OUTFIT_VARIATION", NATIVE.SET_RANDOM_OUTFIT_VARIATION, [arg.int(playerPed), arg.bool(true)]);
        if (trainer.invincible) {
            invoke("SET_PLAYER_INVINCIBLE", NATIVE.SET_PLAYER_INVINCIBLE, [arg.int(player), arg.bool(true)]);
        }
        return `Player model changed: ${pending.name}`;
    }

    if (pending.purpose === "ped") {
        const position = playerSpawnPoint(3.0);
        position.z += 0.2;
        const ped = createPed(pending.hash, position);
        if (ped === 0) throw new Error("Game did not create the ped");
        invoke("SET_ENTITY_VISIBLE", NATIVE.SET_ENTITY_VISIBLE, [arg.int(ped), arg.bool(true)]);
        invoke("SET_ENTITY_ALPHA", NATIVE.SET_ENTITY_ALPHA, [arg.int(ped), arg.int(255), arg.bool(false)]);
        invoke("SET_RANDOM_OUTFIT_VARIATION", NATIVE.SET_RANDOM_OUTFIT_VARIATION, [arg.int(ped), arg.bool(true)]);
        try {
            invoke("PLACE_ENTITY_ON_GROUND_PROPERLY", NATIVE.PLACE_ENTITY_ON_GROUND_PROPERLY, [arg.int(ped), arg.bool(true)]);
        } catch (_) {
            // Ground placement is best-effort, matching the Rust trainer.
        }
        invoke("TASK_WANDER_STANDARD", NATIVE.TASK_WANDER_STANDARD, [arg.int(ped), arg.float(10.0), arg.int(10)]);
        return `Spawned ped: ${pending.name}`;
    }

    if (pending.purpose === "vehicle") {
        if (trainer.lastVehicle !== 0 && resultBool(invoke("DOES_ENTITY_EXIST", NATIVE.DOES_ENTITY_EXIST, [arg.int(trainer.lastVehicle)]))) {
            const deleted = invoke("DELETE_VEHICLE", NATIVE.DELETE_VEHICLE, [arg.inout32(trainer.lastVehicle)]);
            trainer.lastVehicle = deleted.buffers[0] | 0;
        }

        const position = playerSpawnPoint(5.0);
        const vehicle = resultInt(invoke("CREATE_VEHICLE", NATIVE.CREATE_VEHICLE, [
            arg.hash(pending.hash),
            arg.float(position.x), arg.float(position.y), arg.float(position.z + 0.5),
            arg.float(position.heading),
            arg.bool(false), arg.bool(false), arg.bool(false), arg.bool(false),
        ]));
        if (vehicle === 0) throw new Error("Game did not create the vehicle");
        trainer.lastVehicle = vehicle;
        try {
            invoke("SET_VEHICLE_ON_GROUND_PROPERLY", NATIVE.SET_VEHICLE_ON_GROUND_PROPERLY, [arg.int(vehicle), arg.bool(true)]);
        } catch (_) {
            // Ground placement is best-effort, matching the Rust trainer.
        }
        return `Spawned vehicle: ${pending.name}`;
    }

    throw new Error(`Unknown model purpose: ${pending.purpose}`);
}

function processModelQueue() {
    const pending = trainer.pendingModels.shift();
    if (!pending) return;

    try {
        if (resultBool(invoke("HAS_MODEL_LOADED", NATIVE.HAS_MODEL_LOADED, [arg.hash(pending.hash)]))) {
            let message;
            try {
                message = completeModelAction(pending);
            } finally {
                try {
                    invoke("SET_MODEL_AS_NO_LONGER_NEEDED", NATIVE.SET_MODEL_AS_NO_LONGER_NEEDED, [arg.hash(pending.hash)]);
                } catch (error) {
                    logWarn(`Failed to release model ${pending.name}: ${error.message || error}`);
                }
            }
            setStatus(message);
            return;
        }

        pending.waitedTicks += 1;
        if (pending.waitedTicks >= MODEL_LOAD_TIMEOUT_TICKS) {
            throw new Error("Timed out while loading the model");
        }
        invoke("REQUEST_MODEL", NATIVE.REQUEST_MODEL, [arg.hash(pending.hash), arg.bool(false)]);
        trainer.pendingModels.push(pending);
    } catch (error) {
        reportError(error);
    }
}

function performAction(action) {
    if (action.kind === "teleport") {
        const playerPed = resultInt(invoke("PLAYER_PED_ID", NATIVE.PLAYER_PED_ID));
        invoke("SET_ENTITY_COORDS_NO_OFFSET", NATIVE.SET_ENTITY_COORDS_NO_OFFSET, [
            arg.int(playerPed), arg.float(action.x), arg.float(action.y), arg.float(action.z),
            arg.bool(false), arg.bool(false), arg.bool(false),
        ]);
        return `Teleported to ${action.x.toFixed(0)}, ${action.y.toFixed(0)}, ${action.z.toFixed(0)}`;
    }

    if (action.kind === "weather") {
        const weather = joaat(action.name);
        invoke("SET_CURR_WEATHER_STATE", NATIVE.SET_CURR_WEATHER_STATE, [
            arg.hash(weather), arg.hash(weather), arg.float(0.5), arg.bool(true),
        ]);
        return `Weather set to ${action.name}`;
    }

    if (action.kind === "time") {
        invoke("ADD_TO_CLOCK_TIME", NATIVE.ADD_TO_CLOCK_TIME, [
            arg.int(action.hours), arg.int(action.minutes), arg.int(0),
        ]);
        return `Clock changed by ${action.hours}h ${action.minutes}m`;
    }

    if (action.kind === "heal") {
        const playerPed = resultInt(invoke("PLAYER_PED_ID", NATIVE.PLAYER_PED_ID));
        const maximum = resultInt(invoke("GET_ENTITY_MAX_HEALTH", NATIVE.GET_ENTITY_MAX_HEALTH, [arg.int(playerPed), arg.bool(true)]));
        invoke("SET_ENTITY_HEALTH", NATIVE.SET_ENTITY_HEALTH, [arg.int(playerPed), arg.int(maximum), arg.int(0)]);
        return "Health restored";
    }

    if (action.kind === "stamina") {
        const player = resultInt(invoke("PLAYER_ID", NATIVE.PLAYER_ID));
        invoke("RESTORE_PLAYER_STAMINA", NATIVE.RESTORE_PLAYER_STAMINA, [arg.int(player), arg.float(1.0)]);
        return "Stamina restored";
    }

    if (action.kind === "invincibility") {
        const enabled = !trainer.invincible;
        const player = resultInt(invoke("PLAYER_ID", NATIVE.PLAYER_ID));
        invoke("SET_PLAYER_INVINCIBLE", NATIVE.SET_PLAYER_INVINCIBLE, [arg.int(player), arg.bool(enabled)]);
        trainer.invincible = enabled;
        return `Invincibility ${enabled ? "enabled" : "disabled"}`;
    }

    throw new Error(`Unknown action: ${action.kind}`);
}

function activateSelection() {
    const selected = cursor();
    if (hasParent() && selected === 0) {
        goBack();
        return;
    }

    const entry = SCREEN_ENTRIES[trainer.screen][selected - (hasParent() ? 1 : 0)];
    if (!entry) return;
    const action = entry.action;

    if (action.kind === "open") {
        trainer.screen = action.screen;
        const count = itemCount();
        if (trainer.cursors[action.screen] === 0 && hasParent() && count > 1) {
            trainer.cursors[action.screen] = 1;
        } else if (trainer.cursors[action.screen] >= count) {
            trainer.cursors[action.screen] = Math.max(0, count - 1);
        }
        return;
    }

    if (action.kind === "model") {
        queueModel(action.model, action.purpose);
        return;
    }

    try {
        setStatus(performAction(action));
    } catch (error) {
        reportError(error);
    }
}

function handleKey(key) {
    if (key === VK.F4) {
        trainer.open = !trainer.open;
        logInfo(trainer.open ? "JavaScript trainer menu opened" : "JavaScript trainer menu closed");
        return;
    }
    if (!trainer.open) return;

    if (key === VK.UP) moveCursor(-1);
    else if (key === VK.DOWN) moveCursor(1);
    else if (key === VK.PRIOR) moveCursor(-VISIBLE_ROWS);
    else if (key === VK.NEXT) moveCursor(VISIBLE_ROWS);
    else if (key === VK.LEFT || key === VK.BACK) goBack();
    else if (key === VK.RIGHT || key === VK.RETURN) activateSelection();
    else if (key === VK.ESCAPE) trainer.open = false;
}

function pollKeys() {
    if (toggleBinding > 0) {
        for (let index = 0; index < 32; ++index) {
            const event = RDR2Host.pollBindingEvent(toggleBinding);
            if (event === 0) break;
            if (event < 0) {
                logError(`Javy trainer binding poll failed: ${event}`);
                break;
            }
            if (event === 1) handleKey(VK.F4);
        }
    } else if (RDR2Host.isKeyJustPressed(VK.F4)) {
        handleKey(VK.F4);
    }

    const keys = [VK.UP, VK.DOWN, VK.PRIOR, VK.NEXT, VK.LEFT, VK.BACK, VK.RIGHT, VK.RETURN, VK.ESCAPE];
    for (const key of keys) {
        if (RDR2Host.isKeyJustPressed(key)) handleKey(key);
    }
}

function drawRect(x, y, width, height, color) {
    invoke("DRAW_RECT", NATIVE.DRAW_RECT, [
        arg.float(x), arg.float(y), arg.float(width), arg.float(height),
        arg.int(color[0]), arg.int(color[1]), arg.int(color[2]), arg.int(color[3]),
        arg.bool(true), arg.bool(false),
    ]);
}

function drawText(text, x, y, scale, color) {
    invoke("BG_SET_TEXT_SCALE", NATIVE.BG_SET_TEXT_SCALE, [arg.float(scale), arg.float(scale)]);
    invoke("BG_SET_TEXT_COLOR", NATIVE.BG_SET_TEXT_COLOR, [
        arg.int(color[0]), arg.int(color[1]), arg.int(color[2]), arg.int(color[3]),
    ]);
    const nativeText = invoke("VAR_STRING", NATIVE.VAR_STRING, [
        arg.int(10), arg.string("LITERAL_STRING"), arg.string(text),
    ]);
    invoke("BG_DISPLAY_TEXT", NATIVE.BG_DISPLAY_TEXT, [
        arg.raw64(nativeText.high, nativeText.low), arg.float(x), arg.float(y),
    ]);
}

function labelAt(virtualIndex) {
    if (hasParent() && virtualIndex === 0) return "<  BACK";
    const entry = SCREEN_ENTRIES[trainer.screen][virtualIndex - (hasParent() ? 1 : 0)];
    if (!entry) return null;
    if (trainer.screen === SCREEN.PLAYER && entry.action.kind === "invincibility") {
        return `Invincibility: ${trainer.invincible ? "ON" : "OFF"}`;
    }
    return entry.label;
}

function drawMenu() {
    const LEFT = 0.052;
    const WIDTH = 0.305;
    const TITLE_Y = 0.070;
    const TITLE_HEIGHT = 0.052;
    const ROW_HEIGHT = 0.034;
    const ROW_GAP = 0.002;

    const count = itemCount();
    const selected = Math.min(cursor(), Math.max(0, count - 1));
    const maximumStart = Math.max(0, count - VISIBLE_ROWS);
    const start = Math.min(Math.max(0, selected - Math.floor(VISIBLE_ROWS / 2)), maximumStart);
    const end = Math.min(start + VISIBLE_ROWS, count);
    const title = `${SCREEN_TITLES[trainer.screen]}  ${selected + 1}/${count}`;

    drawRect(LEFT + WIDTH * 0.5, TITLE_Y, WIDTH, TITLE_HEIGHT, [156, 112, 32, 238]);
    drawText(title, LEFT + 0.010, TITLE_Y - 0.019, 0.35, [255, 255, 255, 255]);

    for (let virtualIndex = start; virtualIndex < end; ++virtualIndex) {
        const row = virtualIndex - start;
        const rowY = TITLE_Y + TITLE_HEIGHT * 0.5 + ROW_GAP + ROW_HEIGHT * 0.5 + row * (ROW_HEIGHT + ROW_GAP);
        const color = virtualIndex === selected
            ? [156, 112, 32, 240]
            : hasParent() && virtualIndex === 0
                ? [45, 42, 32, 215]
                : [18, 18, 18, 205];
        drawRect(LEFT + WIDTH * 0.5, rowY, WIDTH, ROW_HEIGHT, color);
        const label = labelAt(virtualIndex);
        if (label !== null) drawText(label, LEFT + 0.010, rowY - 0.012, 0.285, [245, 245, 245, 255]);
    }

    const footerY = TITLE_Y + TITLE_HEIGHT * 0.5 + (end - start) * (ROW_HEIGHT + ROW_GAP) + 0.019;
    drawRect(LEFT + WIDTH * 0.5, footerY, WIDTH, 0.030, [8, 8, 8, 220]);
    drawText("UP/DOWN Navigate   LEFT Back   RIGHT/ENTER Select", LEFT + 0.007, footerY - 0.010, 0.215, [210, 210, 210, 255]);

    if (trainer.status) {
        const statusY = footerY + 0.033;
        drawRect(LEFT + WIDTH * 0.5, statusY, WIDTH, 0.030, [28, 28, 28, 225]);
        drawText(trainer.status.text, LEFT + 0.007, statusY - 0.010, 0.225, [235, 214, 160, 255]);
    }
}

function initializeTrainer() {
    RDR2Host.globalSetInt(PLAYER_MODEL_CHANGE_GLOBAL, 1);
    toggleBinding = RDR2Host.registerBinding(
        "toggle_javy_trainer",
        "Open/close JavaScript trainer",
        "keyboard",
        "F4",
    );
    logInfo(`Javy trainer initialized: ${RDR2Host.metadata(0)}`);
    if (toggleBinding > 0) {
        logInfo(`Javy trainer input binding: ${RDR2Host.bindingParameter(toggleBinding)}`);
    } else {
        logError(`Could not register Javy trainer input binding: ${toggleBinding}`);
    }
    logInfo("Navigate with arrows or Page Up/Down; select with Right/Enter");
}

function tickTrainer() {
    pollKeys();
    processModelQueue();

    if (trainer.open) {
        try {
            drawMenu();
            trainer.drawErrorReported = false;
        } catch (error) {
            if (!trainer.drawErrorReported) {
                trainer.drawErrorReported = true;
                logError(`Failed to draw JavaScript trainer menu: ${error.message || error}`);
            }
        }
    }

    if (trainer.status) {
        trainer.status.remainingTicks -= 1;
        if (trainer.status.remainingTicks <= 0) trainer.status = null;
    }
}

function shutdownTrainer() {
    if (toggleBinding > 0) {
        RDR2Host.unregisterBinding(toggleBinding);
        toggleBinding = 0;
    }
    logInfo("Javy trainer shutting down");
}

globalThis.__rdr2JavyTrainer = {
    init: initializeTrainer,
    tick: tickTrainer,
    shutdown: shutdownTrainer,
};
