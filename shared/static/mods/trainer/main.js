/**
 * Simple Trainer - JavaScript Version
 *
 * A trainer menu with vehicles, peds, teleport, weather, and time controls.
 * Press F2 to toggle the menu.
 */

import natives from 'natives';
import { addTickCallback, addKeyDownCallback } from 'core';

// ============================================================================
// Configuration
// ============================================================================

const SCREEN_WIDTH = 1920;
const SCREEN_HEIGHT = 1080;
const MENU_X = 100;
const MENU_Y = 48;
const MENU_WIDTH = 420;
const ITEM_HEIGHT = 32;
const HEADER_HEIGHT = 45;
const ITEM_GAP = 2;
const MAX_VISIBLE_ITEMS = 12;

// Color Scheme - Modern Dark Theme
const Colors = {
    headerBg:       { r: 20,  g: 20,  b: 25,  a: 240 },
    headerAccent:   { r: 180, g: 50,  b: 50,  a: 255 },
    itemBg:         { r: 30,  g: 30,  b: 35,  a: 220 },
    itemSelected:   { r: 180, g: 50,  b: 50,  a: 255 },
    itemHover:      { r: 50,  g: 50,  b: 60,  a: 230 },
    textPrimary:    { r: 255, g: 255, b: 255, a: 255 },
    textSecondary:  { r: 180, g: 180, b: 180, a: 255 },
    textAccent:     { r: 255, g: 200, b: 100, a: 255 },
    backOption:     { r: 60,  g: 60,  b: 70,  a: 220 },
    scrollIndicator:{ r: 180, g: 50,  b: 50,  a: 200 }
};

// ============================================================================
// Menu System
// ============================================================================

const MenuType = {
    MAIN_MENU: 0,
    SUB_MENU: 1,
    ACTION: 2
};

class MenuItem {
    constructor(title, type = MenuType.ACTION) {
        this.title = title;
        this.type = type;
        this.parent = null;
        this.children = [];
    }

    addChild(item) {
        item.parent = this;
        this.children.push(item);
        return this;
    }

    get childCount() {
        return this.children.length;
    }
}

class MenuAction extends MenuItem {
    constructor(title, action) {
        super(title, MenuType.ACTION);
        this.action = action;
    }

    execute() {
        if (this.action) {
            this.action();
        }
    }
}

class SubMenu extends MenuItem {
    constructor(title, parent) {
        super(title, MenuType.SUB_MENU);
        this.parent = parent;
    }
}

class MainMenu extends MenuItem {
    constructor(title) {
        super(title, MenuType.MAIN_MENU);
    }
}

// ============================================================================
// Drawing Functions
// ============================================================================

function drawRect(x, y, width, height, color) {
    const fWidth = width / SCREEN_WIDTH;
    const fHeight = height / SCREEN_HEIGHT;
    const fX = (x + width / 2) / SCREEN_WIDTH;
    const fY = (y + height / 2) / SCREEN_HEIGHT;

    natives.drawRect(fX, fY, fWidth, fHeight, color.r, color.g, color.b, color.a, true);
}

function drawText(x, y, text, color, scaleX = 0.32, scaleY = 0.32) {
    const fX = x / SCREEN_WIDTH;
    const fY = y / SCREEN_HEIGHT;

    natives.bgSetTextScale(scaleX, scaleY);
    natives.bgSetTextColor(color.r, color.g, color.b, color.a);
    const varString = natives.varString(10, "LITERAL_STRING", text);
    natives.bgDisplayText(varString, fX, fY);
}

// ============================================================================
// Promise-based Model Loading
// ============================================================================

const modelLoadQueue = [];

function loadModelAsync(model) {
    return new Promise((resolve, reject) => {
        const hash = typeof model === 'string' ? Hash.joaat(model) : model;

        if (!natives.isModelValid(hash)) {
            reject(new Error('Invalid model: ' + model));
            return;
        }

        modelLoadQueue.push({ hash, resolve, reject });
    });
}

function processModelQueue() {
    for (let i = modelLoadQueue.length - 1; i >= 0; i--) {
        const item = modelLoadQueue[i];

        if (natives.hasModelLoaded(item.hash)) {
            modelLoadQueue.splice(i, 1);
            item.resolve(item.hash);
        } else {
            natives.requestModel(item.hash);
        }
    }
}

// ============================================================================
// Game Functions (Async)
// ============================================================================

let lastSpawnedVehicle = 0;

async function spawnVehicle(name) {
    try {
        const hash = Hash.joaat(name);
        await loadModelAsync(hash);

        if (lastSpawnedVehicle !== 0) {
            natives.deleteVehicle(lastSpawnedVehicle);
            lastSpawnedVehicle = 0;
        }

        const playerPed = natives.getPlayerPed(0);
        const coords = natives.getEntityCoords(playerPed, false);

        lastSpawnedVehicle = natives.createVehicle(
            hash,
            coords.x + 2, coords.y + 2, coords.z + 0.5,
            49.29,
            true, true, false, false
        );

        natives.setModelAsNoLongerNeeded(hash);
        console.log("Spawned vehicle: " + name);
    } catch (err) {
        console.log("Failed to spawn vehicle: " + err.message);
    }
}

async function spawnPed(name) {
    try {
        const hash = Hash.joaat(name);
        await loadModelAsync(hash);

        const playerPed = natives.getPlayerPed(0);
        const coords = natives.getEntityCoords(playerPed, false);

        const ped = natives.createPed(
            hash,
            coords.x + 1, coords.y + 1, coords.z,
            0.0,
            false, false, false, false, true, true
        );

        natives.setEntityVisible(ped, true);
        natives.setEntityAlpha(ped, 255, false);
        natives.setRandomOutfitVariation(ped, true);
        natives.setModelAsNoLongerNeeded(hash);
        console.log("Spawned ped: " + name);
    } catch (err) {
        console.log("Failed to spawn ped: " + err.message);
    }
}

async function changePlayerModel(name) {
    try {
        const hash = Hash.joaat(name);
        await loadModelAsync(hash);

        natives.setPlayerModel(0, hash, true);
        natives.setModelAsNoLongerNeeded(hash);
        console.log("Changed player model to: " + name);
    } catch (err) {
        console.log("Failed to change model: " + err.message);
    }
}

function teleportPlayer(x, y, z) {
    const playerPed = natives.getPlayerPed(0);
    natives.setEntityCoordsNoOffset(playerPed, x, y, z, false, false, false, false);
    console.log("Teleported to: " + x.toFixed(1) + ", " + y.toFixed(1) + ", " + z.toFixed(1));
}

function setWeather(weather) {
    const hash = Hash.joaat(weather);
    natives.setCurrWeatherState(hash, hash, 0.5, true);
    console.log("Weather set to: " + weather);
}

function addClockTime(hours, minutes, seconds) {
    natives.addToClockTime(hours, minutes, seconds);
    console.log("Added " + hours + "h " + minutes + "m " + seconds + "s to clock");
}

// ============================================================================
// Data Arrays
// ============================================================================

const vehicles = [
    "privateopensleeper02x", "privateopensleeper01x", "steamerDummy",
    "armoredCar01x", "armoredCar03x", "privatebaggage01x",
    "smuggler02", "keelboat", "boatSteam02x",
    "canoe", "canoeTreeTrunk", "cart01", "cart02", "cart03",
    "coach2", "coach3", "coach4", "coach5", "coach6",
    "buggy01", "buggy02", "buggy03",
    "ArmySupplyWagon", "chuckwagon000x", "supplywagon",
    "logwagon", "coal_wagon", "gatling_gun",
    "handcart", "horseBoat", "hotAirBalloon01",
    "mineCart01x", "northflatcar01x", "oilWagon01x",
    "pirogue", "pirogue2", "policeWagon01x",
    "privateCoalCar01x", "winterSteamer", "privateboxcar01x",
    "privateSteamer01x", "northSteamer01x", "GhostTrainSteamer",
    "rowboat", "rowboatSwamp", "skiff",
    "stagecoach001x", "stagecoach002x", "stagecoach003x",
    "trolley01x", "TugBoat2",
    "wagon02x", "wagon03x", "wagon04x", "wagon05x",
    "wagonCircus01x", "wagonPrison01x", "wagonWork01x"
];

const peds = [
    "cs_dutch", "cs_micahbell", "cs_billwilliamson", "cs_javierescuella",
    "cs_johnmarston", "cs_abigailroberts", "cs_jackmarston",
    "cs_charlessmith", "cs_hoseamatthews", "cs_lenny", "cs_sean",
    "cs_karen", "cs_marybeth", "cs_tilly", "cs_mollyoshea",
    "cs_susangrimshaw", "cs_uncle", "cs_revswanson",
    "cs_leostrauss", "cs_mrpearson", "cs_kieran",
    "cs_mrsadler", "cs_josiahtrelawny",
    "a_c_horse_arabian_white", "a_c_horse_arabian_black",
    "a_c_horse_turkoman_gold", "a_c_horse_missourifoxtrotter_silverdapplepinto",
    "a_c_bear_01", "a_c_wolf", "a_c_cougar_01", "a_c_panther_01",
    "a_c_alligator_01", "a_c_buffalo_01", "a_c_elk_01", "a_c_moose_01",
    "a_c_doghusky_01", "a_c_donkey_01"
];

const teleportLocations = [
    { name: "Saint Denis", x: 2700.1, y: -1403.39, z: 46.6373 },
    { name: "Van Horn", x: 2962.82, y: 583.162, z: 44.2948 },
    { name: "Annesburg", x: 2941.17, y: 1358.08, z: 44.0665 },
    { name: "Valentine", x: -262.849, y: 793.404, z: 118.587 },
    { name: "Strawberry", x: -1815.63, y: -396.749, z: 161.602 },
    { name: "Blackwater", x: -858.065, y: -1337.73, z: 44.4866 },
    { name: "Rhodes", x: 1233.95, y: -1307.43, z: 76.9055 },
    { name: "Emerald Ranch", x: 1424.09, y: 316.904, z: 88.6065 },
    { name: "Tumbleweed", x: -5407.27, y: -2974.92, z: -1.82 },
    { name: "Armadillo", x: -3686.85, y: -2634.91, z: -13.86 },
    { name: "Thieves Landing", x: -1452.17, y: -2329.4, z: 42.9603 },
    { name: "Braithwaite Manor", x: 889.869, y: -1910.26, z: 45.2703 },
    { name: "Fort Wallace", x: 366.483, y: 1456.73, z: 178.916 },
    { name: "Wallace Station", x: -1308.38, y: 400.834, z: 95.3829 }
];

const weatherTypes = [
    "SUNNY", "HIGHPRESSURE", "CLEARING", "OVERCAST", "OVERCASTDARK",
    "MISTY", "FOG", "DRIZZLE", "RAIN", "SHOWER", "THUNDER", "THUNDERSTORM",
    "SNOWLIGHT", "SNOW", "BLIZZARD", "WHITEOUT", "GROUNDBLIZZARD",
    "SLEET", "HAIL", "SANDSTORM", "HURRICANE"
];

// ============================================================================
// Menu State
// ============================================================================

let currentMenu = null;
let menuCursor = 0;
let scrollOffset = 0;
let menuEnabled = false;

// ============================================================================
// Build Menu Structure
// ============================================================================

function buildMenu() {
    const mainMenu = new MainMenu("TRAINER");

    // Vehicles submenu
    const vehiclesMenu = new SubMenu("Vehicles", mainMenu);
    for (const veh of vehicles) {
        vehiclesMenu.addChild(new MenuAction(veh, () => spawnVehicle(veh)));
    }
    mainMenu.addChild(vehiclesMenu);

    // Change Player Model submenu
    const changeModelMenu = new SubMenu("Change Model", mainMenu);
    for (const ped of peds) {
        changeModelMenu.addChild(new MenuAction(ped, () => changePlayerModel(ped)));
    }
    mainMenu.addChild(changeModelMenu);

    // Spawn Ped submenu
    const spawnPedMenu = new SubMenu("Spawn Ped", mainMenu);
    for (const ped of peds) {
        spawnPedMenu.addChild(new MenuAction(ped, () => spawnPed(ped)));
    }
    mainMenu.addChild(spawnPedMenu);

    // Teleport submenu
    const teleportMenu = new SubMenu("Teleport", mainMenu);
    for (const loc of teleportLocations) {
        teleportMenu.addChild(new MenuAction(loc.name, () => teleportPlayer(loc.x, loc.y, loc.z)));
    }
    mainMenu.addChild(teleportMenu);

    // Weather submenu
    const weatherMenu = new SubMenu("Weather", mainMenu);
    for (const weather of weatherTypes) {
        weatherMenu.addChild(new MenuAction(weather, () => setWeather(weather)));
    }
    mainMenu.addChild(weatherMenu);

    // Time submenu
    const timeMenu = new SubMenu("Time", mainMenu);
    timeMenu.addChild(new MenuAction("+ 4 hours", () => addClockTime(4, 0, 0)));
    timeMenu.addChild(new MenuAction("+ 1 hour", () => addClockTime(1, 0, 0)));
    timeMenu.addChild(new MenuAction("+ 15 minutes", () => addClockTime(0, 15, 0)));
    timeMenu.addChild(new MenuAction("- 4 hours", () => addClockTime(-4, 0, 0)));
    timeMenu.addChild(new MenuAction("- 1 hour", () => addClockTime(-1, 0, 0)));
    timeMenu.addChild(new MenuAction("- 15 minutes", () => addClockTime(0, -15, 0)));
    mainMenu.addChild(timeMenu);

    return mainMenu;
}

// ============================================================================
// Menu Rendering
// ============================================================================

function getDisplayItems() {
    const items = [];
    const hasBackOption = currentMenu.type === MenuType.SUB_MENU;

    if (hasBackOption) {
        items.push({ title: "Back", isBack: true, type: MenuType.ACTION });
    }

    for (const child of currentMenu.children) {
        items.push({
            title: child.title,
            isBack: false,
            type: child.type,
            item: child
        });
    }

    return items;
}

function drawMenu() {
    if (!menuEnabled || !currentMenu) return;

    const displayItems = getDisplayItems();
    const totalItems = displayItems.length;

    if (totalItems === 0) return;

    // Ensure cursor is within bounds
    if (menuCursor >= totalItems) menuCursor = totalItems - 1;
    if (menuCursor < 0) menuCursor = 0;

    // Calculate scroll offset to keep cursor visible
    if (menuCursor < scrollOffset) {
        scrollOffset = menuCursor;
    } else if (menuCursor >= scrollOffset + MAX_VISIBLE_ITEMS) {
        scrollOffset = menuCursor - MAX_VISIBLE_ITEMS + 1;
    }

    // Clamp scroll offset
    const maxScroll = Math.max(0, totalItems - MAX_VISIBLE_ITEMS);
    if (scrollOffset > maxScroll) scrollOffset = maxScroll;
    if (scrollOffset < 0) scrollOffset = 0;

    // Calculate menu height
    const visibleCount = Math.min(totalItems, MAX_VISIBLE_ITEMS);
    const menuHeight = HEADER_HEIGHT + (ITEM_HEIGHT + ITEM_GAP) * visibleCount + ITEM_GAP;

    // Draw background panel
    drawRect(MENU_X - 5, MENU_Y - 5, MENU_WIDTH + 10, menuHeight + 10, { r: 10, g: 10, b: 15, a: 200 });

    // Draw header background
    drawRect(MENU_X, MENU_Y, MENU_WIDTH, HEADER_HEIGHT, Colors.headerBg);

    // Draw header accent line
    drawRect(MENU_X, MENU_Y + HEADER_HEIGHT - 3, MENU_WIDTH, 3, Colors.headerAccent);

    // Draw header text
    drawText(MENU_X + 15, MENU_Y + 8, currentMenu.title, Colors.textPrimary, 0.45, 0.45);

    // Draw scroll indicators if needed
    if (scrollOffset > 0) {
        drawText(MENU_X + MENU_WIDTH - 30, MENU_Y + 10, "^", Colors.scrollIndicator, 0.35, 0.35);
    }
    if (scrollOffset + MAX_VISIBLE_ITEMS < totalItems) {
        drawText(MENU_X + MENU_WIDTH - 30, MENU_Y + menuHeight - 25, "v", Colors.scrollIndicator, 0.35, 0.35);
    }

    // Draw visible items
    const startY = MENU_Y + HEADER_HEIGHT + ITEM_GAP;

    for (let i = 0; i < visibleCount; i++) {
        const itemIndex = scrollOffset + i;
        if (itemIndex >= totalItems) break;

        const displayItem = displayItems[itemIndex];
        const isSelected = itemIndex === menuCursor;
        const yPos = startY + i * (ITEM_HEIGHT + ITEM_GAP);

        // Choose background color
        let bgColor;
        if (isSelected) {
            bgColor = Colors.itemSelected;
        } else if (displayItem.isBack) {
            bgColor = Colors.backOption;
        } else {
            bgColor = Colors.itemBg;
        }

        // Draw item background
        drawRect(MENU_X, yPos, MENU_WIDTH, ITEM_HEIGHT, bgColor);

        // Draw item text
        let textColor = isSelected ? Colors.textPrimary : Colors.textSecondary;
        let itemText = displayItem.title;

        if (displayItem.isBack) {
            itemText = "< " + itemText;
            textColor = isSelected ? Colors.textPrimary : Colors.textAccent;
        } else if (displayItem.type === MenuType.SUB_MENU) {
            itemText = itemText + " >";
        }

        drawText(MENU_X + 15, yPos + 6, itemText, textColor, 0.32, 0.32);
    }

    // Draw item counter at bottom
    const counterText = (menuCursor + 1) + " / " + totalItems;
    drawText(MENU_X + MENU_WIDTH - 80, MENU_Y + menuHeight - 20, counterText, Colors.textSecondary, 0.28, 0.28);
}

// ============================================================================
// Tick Handler
// ============================================================================

function onTick() {
    processModelQueue();
    drawMenu();
}

// ============================================================================
// Key Handler
// ============================================================================

function onKeyDown(key) {
    // F2 - Toggle menu
    if (key === VK_F2) {
        menuEnabled = !menuEnabled;
        if (menuEnabled) {
            console.log("Trainer menu opened");
        }
        return;
    }

    if (!menuEnabled) return;

    const displayItems = getDisplayItems();
    const totalItems = displayItems.length;

    // UP - Move cursor up
    if (key === VK_UP) {
        menuCursor--;
        if (menuCursor < 0) {
            menuCursor = totalItems - 1;
            scrollOffset = Math.max(0, totalItems - MAX_VISIBLE_ITEMS);
        }
    }

    // DOWN - Move cursor down
    if (key === VK_DOWN) {
        menuCursor++;
        if (menuCursor >= totalItems) {
            menuCursor = 0;
            scrollOffset = 0;
        }
    }

    // PAGE UP - Move cursor up by page
    if (key === VK_PRIOR) {
        menuCursor -= MAX_VISIBLE_ITEMS;
        if (menuCursor < 0) {
            menuCursor = 0;
            scrollOffset = 0;
        }
    }

    // PAGE DOWN - Move cursor down by page
    if (key === VK_NEXT) {
        menuCursor += MAX_VISIBLE_ITEMS;
        if (menuCursor >= totalItems) {
            menuCursor = totalItems - 1;
            scrollOffset = Math.max(0, totalItems - MAX_VISIBLE_ITEMS);
        }
    }

    // HOME - Go to start
    if (key === VK_HOME) {
        menuCursor = 0;
        scrollOffset = 0;
    }

    // END - Go to end
    if (key === VK_END) {
        menuCursor = totalItems - 1;
        scrollOffset = Math.max(0, totalItems - MAX_VISIBLE_ITEMS);
    }

    // ENTER - Select item
    if (key === VK_RETURN) {
        const displayItem = displayItems[menuCursor];

        if (displayItem.isBack) {
            currentMenu = currentMenu.parent;
            menuCursor = 0;
            scrollOffset = 0;
            return;
        }

        if (displayItem.type === MenuType.ACTION) {
            displayItem.item.execute();
        } else if (displayItem.type === MenuType.SUB_MENU) {
            currentMenu = displayItem.item;
            menuCursor = 0;
            scrollOffset = 0;
        }
    }

    // BACKSPACE - Go back
    if (key === VK_BACK) {
        if (currentMenu.type === MenuType.SUB_MENU) {
            currentMenu = currentMenu.parent;
            menuCursor = 0;
            scrollOffset = 0;
        } else if (currentMenu.type === MenuType.MAIN_MENU) {
            menuEnabled = false;
        }
    }
}

// ============================================================================
// Initialization
// ============================================================================

console.log("=== Simple Trainer Initializing ===");
console.log("Press F3 to open/close the trainer menu");
console.log("Use Arrow Keys to navigate, Enter to select, Backspace to go back");
console.log("Page Up/Down for fast scrolling, Home/End for start/end");

// Enable player model changes (set global 1835009 to 1)
Global.setInt(1835009, 1);
console.log("Player model changes enabled");

// Build menu
currentMenu = buildMenu();

// Register callbacks
addTickCallback(onTick);
addKeyDownCallback(onKeyDown);

console.log("=== Simple Trainer Ready ===");
