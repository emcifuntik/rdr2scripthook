#include "wasm/Mod.h"
#include "wasm/Runtime.h"
#include "wasm/Bindings.h"

#include <cstring>
#include <cstdint>
#include <filesystem>
#include <iterator>

namespace {

struct NativeContext {
    uint64_t* retVal;
    uint64_t argCount;
    uint64_t* stackPtr;
    uint64_t dataCount;
    uint64_t spaceForResults[24];
    uint64_t stack[24];
};

int g_createdPeds = 0;
int g_createPedArgumentCount = 0;
bool g_createPedArgumentsValid = false;
int g_requestModelArgumentCount = 0;
bool g_requestModelArgumentsValid = false;
int g_createdVehicles = 0;
int g_playerModelChanges = 0;
int g_teleports = 0;
int g_weatherChanges = 0;
int g_timeChanges = 0;

void __cdecl StubNative(NativeContext* context) {
    context->stack[0] = 0;
    context->stack[1] = 0;
    context->stack[2] = 0;
}

void __cdecl StubTrue(NativeContext* context) {
    context->stack[0] = 1;
}

void __cdecl StubPlayerPed(NativeContext* context) {
    context->stack[0] = 7;
}

void __cdecl StubPlayer(NativeContext* context) {
    context->stack[0] = 0;
}

void __cdecl StubCoords(NativeContext* context) {
    const float coordinates[] = { 100.0f, 200.0f, 30.0f };
    for (size_t index = 0; index < std::size(coordinates); ++index) {
        uint32_t bits = 0;
        std::memcpy(&bits, &coordinates[index], sizeof(bits));
        context->stack[index] = bits;
    }
}

void __cdecl StubHeading(NativeContext* context) {
    const float heading = 90.0f;
    uint32_t bits = 0;
    std::memcpy(&bits, &heading, sizeof(bits));
    context->stack[0] = bits;
}

void __cdecl StubCreatePed(NativeContext* context) {
    g_createPedArgumentCount = static_cast<int>(context->argCount);
    g_createPedArgumentsValid = context->argCount == 9 &&
                                context->stack[5] == 0 &&
                                context->stack[6] == 0 &&
                                context->stack[7] == 0 &&
                                context->stack[8] == 0;
    context->stack[0] = 100 + ++g_createdPeds;
}

void __cdecl StubRequestModel(NativeContext* context) {
    g_requestModelArgumentCount = static_cast<int>(context->argCount);
    g_requestModelArgumentsValid = context->argCount == 2 &&
                                   context->stack[1] == 0;
}

void __cdecl StubCreateVehicle(NativeContext* context) {
    context->stack[0] = 200 + ++g_createdVehicles;
}

void __cdecl StubSetPlayerModel(NativeContext*) {
    ++g_playerModelChanges;
}

void __cdecl StubTeleport(NativeContext*) {
    ++g_teleports;
}

void __cdecl StubWeather(NativeContext*) {
    ++g_weatherChanges;
}

void __cdecl StubTime(NativeContext*) {
    ++g_timeChanges;
}

uintptr_t GetNativeAddress(uint64_t hash) {
    switch (hash) {
        case 0x392C8D8E07B70EFC: // IS_MODEL_VALID
        case 0x1283B8B89DD5D1B6: // HAS_MODEL_LOADED
        case 0x9587913B9E772D29: // PLACE_ENTITY_ON_GROUND_PROPERLY
            return reinterpret_cast<uintptr_t>(&StubTrue);
        case 0x096275889B8E0EE0: // PLAYER_PED_ID
            return reinterpret_cast<uintptr_t>(&StubPlayerPed);
        case 0x217E9DC48139933D: // PLAYER_ID
            return reinterpret_cast<uintptr_t>(&StubPlayer);
        case 0xA86D5F069399F44D: // GET_ENTITY_COORDS
            return reinterpret_cast<uintptr_t>(&StubCoords);
        case 0xC230DD956E2F5507: // GET_ENTITY_HEADING
            return reinterpret_cast<uintptr_t>(&StubHeading);
        case 0xD49F9B0955C367DE: // CREATE_PED
            return reinterpret_cast<uintptr_t>(&StubCreatePed);
        case 0xFA28FE3A6246FC30: // REQUEST_MODEL
            return reinterpret_cast<uintptr_t>(&StubRequestModel);
        case 0xAF35D0D2583051B0: // CREATE_VEHICLE
            return reinterpret_cast<uintptr_t>(&StubCreateVehicle);
        case 0xED40380076A31506: // SET_PLAYER_MODEL
            return reinterpret_cast<uintptr_t>(&StubSetPlayerModel);
        case 0x239A3351AC1DA385: // SET_ENTITY_COORDS_NO_OFFSET
            return reinterpret_cast<uintptr_t>(&StubTeleport);
        case 0xFA3E3CA8A1DE6D5D: // SET_CURR_WEATHER_STATE
            return reinterpret_cast<uintptr_t>(&StubWeather);
        case 0xAB7C251C7701D336: // ADD_TO_CLOCK_TIME
            return reinterpret_cast<uintptr_t>(&StubTime);
        default:
            break;
    }
    return reinterpret_cast<uintptr_t>(&StubNative);
}

void* GetGlobalPointer(uint32_t index) {
    static uint64_t globals[1024]{};
    return index < std::size(globals) ? &globals[index] : nullptr;
}

} // namespace

int main() {
    auto& runtime = rdr2wasm::GetRuntime();
    if (!runtime.IsValid()) return 1;

    rdr2wasm::InstallGameBridge(GetNativeAddress, GetGlobalPointer);
    rdr2wasm::ModManifest manifest{
        .name = "WASM smoke test",
        .version = "1.0.0",
        .author = "RDR2 Script Hook",
        .description = "Loads the compiled Rust example",
        .entrypoint = "main.wasm",
        .modPath = std::filesystem::path(WASM_EXAMPLE_MOD_PATH),
    };

    rdr2wasm::Mod mod(runtime, std::move(manifest));
    if (!mod.LoadEntrypoint()) return 2;

    // The Rust trainer is WebView-driven. Initialization prewarms a hidden
    // browser; F3 now toggles visibility, focus and cursor ownership without
    // destroying it. The test host models this lifecycle without WebView2.
    mod.OnKeyDown(0x72); // F3: show the prewarmed trainer.
    if (!mod.Tick()) return 3;
    mod.OnKeyDown(0x72); // F3: hide it while keeping the host open.
    if (!mod.Tick()) return 4;

    rdr2wasm::ModManifest javyManifest{
        .name = "Javy WASM smoke test",
        .version = "1.0.0",
        .author = "RDR2 Script Hook",
        .description = "Loads the compiled JavaScript example",
        .entrypoint = "main.wasm",
        .runtime = "javy",
        .modPath = std::filesystem::path(WASM_JAVY_MOD_PATH),
    };
    rdr2wasm::Mod javyMod(runtime, std::move(javyManifest));
    if (!javyMod.LoadEntrypoint()) return 10;

    const int initialCreatedPeds = g_createdPeds;
    const int initialCreatedVehicles = g_createdVehicles;
    const int initialPlayerModelChanges = g_playerModelChanges;
    const int initialTeleports = g_teleports;
    const int initialWeatherChanges = g_weatherChanges;
    const int initialTimeChanges = g_timeChanges;
    bool javyHealthy = true;
    const auto pressJavyKey = [&](uint32_t key) {
        rdr2wasm::bindings::SetKeyStateForTesting(key, true);
        if (!javyMod.Tick()) javyHealthy = false;
        rdr2wasm::bindings::SetKeyStateForTesting(key, false);
        if (!javyMod.Tick()) javyHealthy = false;
    };

    pressJavyKey(0x73); // F4: open menu.
    pressJavyKey(0x26); // Up: wrap from player options to time.
    pressJavyKey(0x28); // Down: wrap back to player options.
    pressJavyKey(0x28); // Down: select horse menu.
    pressJavyKey(0x27); // Right: enter horse menu.
    pressJavyKey(0x27); // Right: spawn the first horse.
    if (g_requestModelArgumentCount != 2 || !g_requestModelArgumentsValid ||
        g_createdPeds != initialCreatedPeds + 1 ||
        g_createPedArgumentCount != 9 || !g_createPedArgumentsValid) return 11;

    pressJavyKey(0x25); // Left: return to main menu.
    pressJavyKey(0x28); // Down: select player models.
    pressJavyKey(0x0D); // Enter: open player models.
    pressJavyKey(0x27); // Right: change to Arthur.
    if (g_playerModelChanges != initialPlayerModelChanges + 1) return 12;

    pressJavyKey(0x25); // Left: return to main menu.
    pressJavyKey(0x28); // Down: select ped spawner.
    pressJavyKey(0x27); // Right: enter ped menu.
    pressJavyKey(0x27); // Right: spawn Arthur.
    if (g_createdPeds != initialCreatedPeds + 2) return 13;

    pressJavyKey(0x25); // Left: return to main menu.
    pressJavyKey(0x28); // Down: select vehicles.
    pressJavyKey(0x27); // Right: enter vehicle menu.
    pressJavyKey(0x22); // Page Down.
    pressJavyKey(0x21); // Page Up.
    pressJavyKey(0x27); // Right: spawn the selected vehicle.
    if (g_createdVehicles != initialCreatedVehicles + 1) return 14;

    pressJavyKey(0x25); // Left: return to main menu.
    pressJavyKey(0x28); // Down: select teleports.
    pressJavyKey(0x27); // Right: enter teleport menu.
    pressJavyKey(0x27); // Right: teleport to Saint Denis.
    if (g_teleports != initialTeleports + 1) return 15;

    pressJavyKey(0x25); // Left: return to main menu.
    pressJavyKey(0x28); // Down: select weather.
    pressJavyKey(0x27); // Right: enter weather menu.
    pressJavyKey(0x27); // Right: select overcast.
    if (g_weatherChanges != initialWeatherChanges + 1) return 16;

    pressJavyKey(0x25); // Left: return to main menu.
    pressJavyKey(0x28); // Down: select time.
    pressJavyKey(0x27); // Right: enter time menu.
    pressJavyKey(0x27); // Right: add four hours.
    if (g_timeChanges != initialTimeChanges + 1) return 17;

    // Keep the visible menu active long enough to catch per-tick QuickJS
    // allocation leaks and repeated-module initialization regressions.
    for (int tick = 0; tick < 2'000; ++tick) {
        if (!javyMod.Tick()) return 18;
    }

    pressJavyKey(0x73); // F4: close menu.
    if (!javyHealthy) return 18;
    return 0;
}
