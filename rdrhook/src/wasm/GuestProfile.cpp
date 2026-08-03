#include "GuestProfile.h"

#include <array>

namespace rdr2wasm {

namespace {

constexpr std::uint64_t DefaultFuelPerCall = 10'000'000;
constexpr std::uint64_t ManagedFuelPerCall = 100'000'000;

constexpr std::array Profiles{
    GuestProfile{
        "wasmtime", WasiPolicy::Disabled,
        DefaultFuelPerCall, {}, "rdr2_abi_version", "rdr2_init", "rdr2_tick",
        "rdr2_key_down", "rdr2_key_up", "rdr2_shutdown",
    },
    GuestProfile{
        "javy", WasiPolicy::Restricted,
        ManagedFuelPerCall, {}, {}, "init", "tick", {}, {}, "shutdown",
    },
    GuestProfile{
        "dotnet", WasiPolicy::Restricted,
        ManagedFuelPerCall, {}, "rdr2_abi_version", "rdr2_init", "rdr2_tick",
        "rdr2_key_down", "rdr2_key_up", "rdr2_shutdown",
    },
    GuestProfile{
        "lua", WasiPolicy::Restricted, ManagedFuelPerCall,
        "__wasm_call_ctors", "rdr2_abi_version", "rdr2_init", "rdr2_tick",
        "rdr2_key_down", "rdr2_key_up", "rdr2_shutdown",
    },
};

} // namespace

bool GuestProfile::AllowsImportModule(std::string_view module) const noexcept {
    if (module == "rdr2") return true;
    return wasi == WasiPolicy::Restricted &&
           module == "wasi_snapshot_preview1";
}

const GuestProfile* FindGuestProfile(std::string_view id) noexcept {
    for (const auto& profile : Profiles) {
        if (profile.id == id) return &profile;
    }
    return nullptr;
}

} // namespace rdr2wasm
