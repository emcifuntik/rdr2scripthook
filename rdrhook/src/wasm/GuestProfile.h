#pragma once

#include <cstdint>
#include <string_view>

namespace rdr2wasm {

enum class WasiPolicy {
    Disabled,
    Restricted,
};

struct GuestProfile {
    std::string_view id;
    WasiPolicy wasi;
    std::uint64_t fuelPerCall;
    std::string_view startupExport;
    std::string_view abiVersionExport;
    std::string_view initExport;
    std::string_view tickExport;
    std::string_view keyDownExport;
    std::string_view keyUpExport;
    std::string_view shutdownExport;

    bool RequiresAbiVersion() const noexcept {
        return !abiVersionExport.empty();
    }

    bool AllowsImportModule(std::string_view module) const noexcept;
};

const GuestProfile* FindGuestProfile(std::string_view id) noexcept;

} // namespace rdr2wasm
