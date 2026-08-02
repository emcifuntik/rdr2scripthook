#include "InputBindingManager.h"

#include <Windows.h>

#include <algorithm>
#include <charconv>
#include <cctype>
#include <deque>
#include <fstream>
#include <iomanip>
#include <limits>
#include <sstream>
#include <unordered_map>

#include <spdlog/spdlog.h>

namespace rdr2::input {
namespace {

constexpr std::size_t MaxBindings =
    InputBindingManager::NativeBindingCapacity;
constexpr std::size_t MaxQueuedEvents = 32;
constexpr std::size_t MaxOwnerLength = 256;
constexpr std::size_t MaxIdLength = 128;
constexpr std::size_t MaxDescriptionLength = 512;
constexpr std::size_t MaxMapperLength = 32;
constexpr std::size_t MaxParameterLength = 64;

std::string Trim(std::string value)
{
    const auto first = value.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return {};
    const auto last = value.find_last_not_of(" \t\r\n");
    return value.substr(first, last - first + 1);
}

std::string Upper(std::string value)
{
    std::transform(value.begin(), value.end(), value.begin(),
                   [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
    return value;
}

std::string Lower(std::string value)
{
    std::transform(value.begin(), value.end(), value.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return value;
}

bool IsUnboundParameter(std::string_view parameter)
{
    return Upper(Trim(std::string(parameter))) == "UNBOUND";
}

std::string EscapeToml(std::string_view value)
{
    std::string result;
    result.reserve(value.size());
    for (const char c : value) {
        switch (c) {
        case '\\': result += "\\\\"; break;
        case '"': result += "\\\""; break;
        case '\n': result += "\\n"; break;
        case '\r': result += "\\r"; break;
        case '\t': result += "\\t"; break;
        default: result += c; break;
        }
    }
    return result;
}

bool ParseTomlString(std::string_view value, std::string& out)
{
    if (value.size() < 2 || value.front() != '"' || value.back() != '"')
        return false;
    out.clear();
    for (std::size_t i = 1; i + 1 < value.size(); ++i) {
        char c = value[i];
        if (c != '\\') {
            out += c;
            continue;
        }
        if (++i >= value.size() - 1) return false;
        switch (value[i]) {
        case '\\': out += '\\'; break;
        case '"': out += '"'; break;
        case 'n': out += '\n'; break;
        case 'r': out += '\r'; break;
        case 't': out += '\t'; break;
        default: return false;
        }
    }
    return true;
}

std::string Identity(std::string_view owner, std::string_view id)
{
    std::string result;
    result.reserve(owner.size() + id.size() + 1);
    result.append(owner);
    result.push_back(':');
    result.append(id);
    return result;
}

std::string KeyboardParameterName(std::uint8_t virtualKey)
{
    if ((virtualKey >= 'A' && virtualKey <= 'Z') ||
        (virtualKey >= '0' && virtualKey <= '9'))
        return std::string(1, static_cast<char>(virtualKey));
    if (virtualKey >= VK_F1 && virtualKey <= VK_F24)
        return "F" + std::to_string(virtualKey - VK_F1 + 1);
    if (virtualKey >= VK_NUMPAD0 && virtualKey <= VK_NUMPAD9)
        return "NUMPAD" + std::to_string(virtualKey - VK_NUMPAD0);

    switch (virtualKey) {
    case VK_BACK: return "BACKSPACE";
    case VK_TAB: return "TAB";
    case VK_RETURN: return "ENTER";
    case VK_SHIFT: return "SHIFT";
    case VK_CONTROL: return "CTRL";
    case VK_MENU: return "ALT";
    case VK_PAUSE: return "PAUSE";
    case VK_CAPITAL: return "CAPSLOCK";
    case VK_ESCAPE: return "ESCAPE";
    case VK_SPACE: return "SPACE";
    case VK_PRIOR: return "PAGEUP";
    case VK_NEXT: return "PAGEDOWN";
    case VK_END: return "END";
    case VK_HOME: return "HOME";
    case VK_LEFT: return "LEFT";
    case VK_UP: return "UP";
    case VK_RIGHT: return "RIGHT";
    case VK_DOWN: return "DOWN";
    case VK_SNAPSHOT: return "PRINTSCREEN";
    case VK_INSERT: return "INSERT";
    case VK_DELETE: return "DELETE";
    case VK_LWIN: return "LWIN";
    case VK_RWIN: return "RWIN";
    case VK_NUMLOCK: return "NUMLOCK";
    case VK_SCROLL: return "SCROLLLOCK";
    case VK_LSHIFT: return "LSHIFT";
    case VK_RSHIFT: return "RSHIFT";
    case VK_LCONTROL: return "LCTRL";
    case VK_RCONTROL: return "RCTRL";
    case VK_LMENU: return "LALT";
    case VK_RMENU: return "RALT";
    case VK_OEM_1: return "SEMICOLON";
    case VK_OEM_PLUS: return "PLUS";
    case VK_OEM_COMMA: return "COMMA";
    case VK_OEM_MINUS: return "MINUS";
    case VK_OEM_PERIOD: return "PERIOD";
    case VK_OEM_2: return "SLASH";
    case VK_OEM_3: return "TILDE";
    case VK_OEM_4: return "LBRACKET";
    case VK_OEM_5: return "BACKSLASH";
    case VK_OEM_6: return "RBRACKET";
    case VK_OEM_7: return "APOSTROPHE";
    default: break;
    }

    std::ostringstream formatted;
    formatted << "0x" << std::uppercase << std::hex << std::setw(2)
              << std::setfill('0') << static_cast<unsigned>(virtualKey);
    return formatted.str();
}

bool NormalizeMapper(std::string mapper, std::string& canonical)
{
    mapper = Lower(Trim(std::move(mapper)));
    if (mapper == "keyboard" || mapper == "ioms_keyboard") {
        canonical = "keyboard";
        return true;
    }
    return false;
}

BindingStatus NormalizeRegistration(
    std::string& ownerName, std::string& id, std::string& description,
    std::string mapper, std::string defaultParameter,
    std::string& canonicalMapper, std::string& canonicalParameter,
    std::uint8_t& virtualKey)
{
    ownerName = Trim(std::move(ownerName));
    id = Trim(std::move(id));
    description = Trim(std::move(description));
    if (ownerName.empty() || id.empty() || description.empty() ||
        ownerName.size() > MaxOwnerLength || id.size() > MaxIdLength ||
        description.size() > MaxDescriptionLength ||
        mapper.size() > MaxMapperLength ||
        defaultParameter.size() > MaxParameterLength)
        return BindingStatus::InvalidArgument;
    if (!NormalizeMapper(std::move(mapper), canonicalMapper))
        return BindingStatus::UnsupportedMapper;
    if (!InputBindingManager::NormalizeKeyboardParameter(
            defaultParameter, virtualKey, canonicalParameter))
        return BindingStatus::UnsupportedParameter;
    std::uint8_t rageKey = 0;
    if (!InputBindingManager::VirtualKeyToRageKey(virtualKey, rageKey))
        return BindingStatus::UnsupportedParameter;
    return BindingStatus::Ok;
}

const std::unordered_map<std::string, std::uint8_t>& NamedKeys()
{
    static const std::unordered_map<std::string, std::uint8_t> keys = {
        {"BACKSPACE", VK_BACK}, {"TAB", VK_TAB}, {"ENTER", VK_RETURN},
        {"RETURN", VK_RETURN}, {"SHIFT", VK_SHIFT}, {"CTRL", VK_CONTROL},
        {"CONTROL", VK_CONTROL}, {"ALT", VK_MENU}, {"PAUSE", VK_PAUSE},
        {"CAPSLOCK", VK_CAPITAL}, {"ESC", VK_ESCAPE}, {"ESCAPE", VK_ESCAPE},
        {"SPACE", VK_SPACE}, {"PAGEUP", VK_PRIOR}, {"PAGEDOWN", VK_NEXT},
        {"END", VK_END}, {"HOME", VK_HOME}, {"LEFT", VK_LEFT},
        {"UP", VK_UP}, {"RIGHT", VK_RIGHT}, {"DOWN", VK_DOWN},
        {"PRINTSCREEN", VK_SNAPSHOT}, {"INSERT", VK_INSERT},
        {"DELETE", VK_DELETE}, {"LWIN", VK_LWIN}, {"RWIN", VK_RWIN},
        {"NUMLOCK", VK_NUMLOCK}, {"SCROLLLOCK", VK_SCROLL},
        {"LSHIFT", VK_LSHIFT}, {"RSHIFT", VK_RSHIFT},
        {"LCTRL", VK_LCONTROL}, {"RCTRL", VK_RCONTROL},
        {"LALT", VK_LMENU}, {"RALT", VK_RMENU},
        {"SEMICOLON", VK_OEM_1}, {"PLUS", VK_OEM_PLUS},
        {"COMMA", VK_OEM_COMMA}, {"MINUS", VK_OEM_MINUS},
        {"PERIOD", VK_OEM_PERIOD}, {"SLASH", VK_OEM_2},
        {"TILDE", VK_OEM_3}, {"LBRACKET", VK_OEM_4},
        {"BACKSLASH", VK_OEM_5}, {"RBRACKET", VK_OEM_6},
        {"APOSTROPHE", VK_OEM_7},
    };
    return keys;
}

} // namespace

struct InputBindingManager::State {
    BindingHandle handle = 0;
    std::uint32_t controlId = 0;
    void* owner = nullptr;
    std::string ownerName;
    std::string id;
    std::string description;
    std::string mapper;
    std::string parameter;
    std::string alternateParameter = "UNBOUND";
    std::string defaultMapper;
    std::string defaultParameter;
    std::uint8_t virtualKey = 0;
    std::uint8_t rageKey = 0;
    std::uint8_t alternateVirtualKey = 0;
    std::uint8_t alternateRageKey = 0;
    bool predeclared = false;
    bool mapped = true;
    bool alternateMapped = false;
    bool down = false;
    std::deque<BindingEvent> events;
};

InputBindingManager& InputBindingManager::Instance()
{
    static InputBindingManager instance;
    return instance;
}

void InputBindingManager::SetStoragePath(std::filesystem::path path)
{
    std::scoped_lock lock(m_mutex);
    m_storagePath = std::move(path);
    m_loaded = false;
    m_savedMappings.clear();
    LoadLocked();
}

BindingRegistration InputBindingManager::Predeclare(
    std::string ownerName, std::string id, std::string description,
    std::string mapper, std::string defaultParameter)
{
    std::string canonicalMapper;
    std::string canonicalParameter;
    std::uint8_t virtualKey = 0;
    const BindingStatus normalized = NormalizeRegistration(
        ownerName, id, description, std::move(mapper),
        std::move(defaultParameter), canonicalMapper, canonicalParameter,
        virtualKey);
    if (normalized != BindingStatus::Ok) return {0, normalized};

    std::scoped_lock lock(m_mutex);
    if (!m_loaded) LoadLocked();
    for (auto& [handle, binding] : m_bindings) {
        if (binding.ownerName != ownerName || binding.id != id) continue;
        binding.predeclared = true;
        return {handle, BindingStatus::AlreadyRegistered};
    }
    if (m_bindings.size() >= MaxBindings)
        return {0, BindingStatus::CapacityExceeded};
    return InsertLocked(nullptr, true, std::move(ownerName), std::move(id),
                        std::move(description), std::move(canonicalMapper),
                        std::move(canonicalParameter), virtualKey);
}

BindingRegistration InputBindingManager::Register(
    void* owner, std::string ownerName, std::string id,
    std::string description, std::string mapper,
    std::string defaultParameter)
{
    std::string canonicalMapper;
    std::string canonicalParameter;
    std::uint8_t virtualKey = 0;
    if (!owner) return {0, BindingStatus::InvalidArgument};
    const BindingStatus normalized = NormalizeRegistration(
        ownerName, id, description, std::move(mapper),
        std::move(defaultParameter), canonicalMapper, canonicalParameter,
        virtualKey);
    if (normalized != BindingStatus::Ok) return {0, normalized};

    std::scoped_lock lock(m_mutex);
    if (!m_loaded) LoadLocked();
    for (auto& [handle, binding] : m_bindings) {
        if (binding.owner == owner && binding.id == id)
            return {handle, BindingStatus::AlreadyRegistered};
        if (!binding.predeclared || binding.owner ||
            binding.ownerName != ownerName || binding.id != id)
            continue;

        if (binding.description != description ||
            binding.defaultMapper != canonicalMapper ||
            binding.defaultParameter != canonicalParameter) {
            spdlog::warn(
                "[Input] Runtime registration for '{}:{}' differs from its "
                "mod.toml declaration; the declaration remains authoritative",
                ownerName, id);
        }
        binding.owner = owner;
        binding.down = false;
        binding.events.clear();
        return {handle, BindingStatus::Ok};
    }
    if (m_bindings.size() >= MaxBindings)
        return {0, BindingStatus::CapacityExceeded};

    return InsertLocked(owner, false, std::move(ownerName), std::move(id),
                        std::move(description), std::move(canonicalMapper),
                        std::move(canonicalParameter), virtualKey);
}

BindingRegistration InputBindingManager::InsertLocked(
    void* owner, bool predeclared, std::string ownerName, std::string id,
    std::string description, std::string canonicalMapper,
    std::string canonicalParameter, std::uint8_t virtualKey)
{
    State binding;
    while (m_bindings.contains(m_nextHandle)) {
        m_nextHandle = m_nextHandle == std::numeric_limits<BindingHandle>::max()
                           ? 1 : m_nextHandle + 1;
    }
    binding.handle = m_nextHandle;
    m_nextHandle = m_nextHandle == std::numeric_limits<BindingHandle>::max()
                       ? 1 : m_nextHandle + 1;
    binding.owner = owner;
    binding.predeclared = predeclared;
    binding.ownerName = std::move(ownerName);
    binding.id = std::move(id);
    binding.controlId = AllocateControlIdLocked(
        Identity(binding.ownerName, binding.id));
    binding.description = std::move(description);
    binding.defaultMapper = canonicalMapper;
    binding.defaultParameter = canonicalParameter;
    binding.mapper = canonicalMapper;
    binding.parameter = canonicalParameter;
    binding.virtualKey = virtualKey;
    VirtualKeyToRageKey(virtualKey, binding.rageKey);

    const auto saved = m_savedMappings.find(
        Identity(binding.ownerName, binding.id));
    if (saved != m_savedMappings.end()) {
        std::string savedMapper;
        std::string savedParameter;
        std::uint8_t savedKey = 0;
        if (NormalizeMapper(saved->second.mapper, savedMapper) &&
            IsUnboundParameter(saved->second.parameter)) {
            binding.mapper = std::move(savedMapper);
            binding.parameter = "UNBOUND";
            binding.virtualKey = 0;
            binding.rageKey = 0;
            binding.mapped = false;
        } else if (NormalizeMapper(saved->second.mapper, savedMapper) &&
                   NormalizeKeyboardParameter(saved->second.parameter, savedKey,
                                              savedParameter) &&
                   VirtualKeyToRageKey(savedKey, binding.rageKey)) {
            binding.mapper = std::move(savedMapper);
            binding.parameter = std::move(savedParameter);
            binding.virtualKey = savedKey;
            binding.mapped = true;
        }

        if (!IsUnboundParameter(saved->second.alternateParameter)) {
            std::string alternateParameter;
            std::uint8_t alternateKey = 0;
            if (NormalizeKeyboardParameter(
                    saved->second.alternateParameter, alternateKey,
                    alternateParameter) &&
                VirtualKeyToRageKey(alternateKey,
                                    binding.alternateRageKey)) {
                binding.alternateParameter =
                    std::move(alternateParameter);
                binding.alternateVirtualKey = alternateKey;
                binding.alternateMapped = true;
            }
        }
    }

    const BindingHandle handle = binding.handle;
    m_bindings.emplace(handle, std::move(binding));
    return {handle, BindingStatus::Ok};
}

BindingStatus InputBindingManager::Unregister(void* owner,
                                               BindingHandle handle)
{
    std::scoped_lock lock(m_mutex);
    const auto it = m_bindings.find(handle);
    if (it == m_bindings.end() || it->second.owner != owner)
        return BindingStatus::InvalidHandle;
    if (it->second.predeclared) {
        it->second.owner = nullptr;
        it->second.down = false;
        it->second.events.clear();
        return BindingStatus::Ok;
    }
    m_bindings.erase(it);
    PromoteNativeSlotsLocked();
    return BindingStatus::Ok;
}

void InputBindingManager::ReleaseOwner(void* owner)
{
    if (!owner) return;
    std::scoped_lock lock(m_mutex);
    for (auto it = m_bindings.begin(); it != m_bindings.end();) {
        State& binding = it->second;
        if (binding.owner != owner) {
            ++it;
            continue;
        }
        if (binding.predeclared) {
            binding.owner = nullptr;
            binding.down = false;
            binding.events.clear();
            ++it;
        } else {
            it = m_bindings.erase(it);
        }
    }
    PromoteNativeSlotsLocked();
}

BindingEvent InputBindingManager::PollEvent(void* owner,
                                             BindingHandle handle)
{
    std::scoped_lock lock(m_mutex);
    State* binding = FindOwnedLocked(owner, handle);
    if (!binding || binding->events.empty()) return BindingEvent::None;
    const BindingEvent event = binding->events.front();
    binding->events.pop_front();
    return event;
}

std::int32_t InputBindingManager::IsDown(void* owner,
                                         BindingHandle handle) const
{
    std::scoped_lock lock(m_mutex);
    const State* binding = FindOwnedLocked(owner, handle);
    return binding ? (binding->down ? 1 : 0)
                   : static_cast<std::int32_t>(BindingStatus::InvalidHandle);
}

BindingStatus InputBindingManager::GetMapping(void* owner,
                                               BindingHandle handle,
                                               std::string& mapper,
                                               std::string& parameter) const
{
    std::scoped_lock lock(m_mutex);
    const State* binding = FindOwnedLocked(owner, handle);
    if (!binding) return BindingStatus::InvalidHandle;
    mapper = binding->mapper;
    parameter = binding->parameter;
    return BindingStatus::Ok;
}

BindingStatus InputBindingManager::SetMapping(void* owner,
                                               BindingHandle handle,
                                               std::string mapper,
                                               std::string parameter)
{
    if (mapper.size() > MaxMapperLength ||
        parameter.size() > MaxParameterLength)
        return BindingStatus::InvalidArgument;
    std::string canonicalMapper;
    std::string canonicalParameter;
    std::uint8_t virtualKey = 0;
    if (!NormalizeMapper(std::move(mapper), canonicalMapper))
        return BindingStatus::UnsupportedMapper;
    const bool unbound = IsUnboundParameter(parameter);
    if (!unbound &&
        !NormalizeKeyboardParameter(parameter, virtualKey, canonicalParameter))
        return BindingStatus::UnsupportedParameter;
    if (unbound) canonicalParameter = "UNBOUND";
    std::uint8_t rageKey = 0;
    if (!unbound && !VirtualKeyToRageKey(virtualKey, rageKey))
        return BindingStatus::UnsupportedParameter;

    std::scoped_lock lock(m_mutex);
    State* binding = FindOwnedLocked(owner, handle);
    if (!binding) return BindingStatus::InvalidHandle;
    binding->mapper = std::move(canonicalMapper);
    binding->parameter = std::move(canonicalParameter);
    binding->virtualKey = virtualKey;
    binding->rageKey = rageKey;
    binding->mapped = !unbound;
    binding->down = false;
    binding->events.clear();
    m_savedMappings[Identity(binding->ownerName, binding->id)] = {
        binding->mapper, binding->parameter,
        binding->alternateParameter};
    SaveLocked();
    return BindingStatus::Ok;
}

BindingStatus InputBindingManager::ResetMapping(void* owner,
                                                 BindingHandle handle)
{
    std::scoped_lock lock(m_mutex);
    State* binding = FindOwnedLocked(owner, handle);
    if (!binding) return BindingStatus::InvalidHandle;
    std::string ignored;
    std::uint8_t virtualKey = 0;
    if (!NormalizeKeyboardParameter(binding->defaultParameter, virtualKey,
                                    ignored))
        return BindingStatus::UnsupportedParameter;
    std::uint8_t rageKey = 0;
    if (!VirtualKeyToRageKey(virtualKey, rageKey))
        return BindingStatus::UnsupportedParameter;
    binding->mapper = binding->defaultMapper;
    binding->parameter = binding->defaultParameter;
    binding->virtualKey = virtualKey;
    binding->rageKey = rageKey;
    binding->mapped = true;
    binding->alternateParameter = "UNBOUND";
    binding->alternateVirtualKey = 0;
    binding->alternateRageKey = 0;
    binding->alternateMapped = false;
    binding->down = false;
    binding->events.clear();
    m_savedMappings.erase(Identity(binding->ownerName, binding->id));
    SaveLocked();
    return BindingStatus::Ok;
}

BindingStatus InputBindingManager::SetKeyboardVirtualKeyByControlId(
    std::uint32_t controlId, std::uint32_t virtualKey)
{
    return SetKeyboardVirtualKeyByControlId(controlId, 0, virtualKey);
}

BindingStatus InputBindingManager::SetKeyboardVirtualKeyByControlId(
    std::uint32_t controlId, std::uint16_t mappingIndex,
    std::uint32_t virtualKey)
{
    if (controlId < NativeControlIdFirst || controlId > NativeControlIdLast)
        return BindingStatus::InvalidHandle;
    if (mappingIndex > 1) return BindingStatus::InvalidArgument;
    if (virtualKey > 255 && virtualKey != 0xFF000)
        return BindingStatus::UnsupportedParameter;

    const bool newMapped = virtualKey != 0xFF000;
    const std::uint8_t newVirtualKey =
        newMapped ? static_cast<std::uint8_t>(virtualKey) : 0;
    const std::string newParameter =
        newMapped ? KeyboardParameterName(newVirtualKey) : "UNBOUND";
    std::uint8_t newRageKey = 0;
    if (newMapped && !VirtualKeyToRageKey(newVirtualKey, newRageKey))
        return BindingStatus::UnsupportedParameter;

    std::scoped_lock lock(m_mutex);
    const auto it = std::find_if(
        m_bindings.begin(), m_bindings.end(),
        [controlId](const auto& entry) {
            return entry.second.controlId == controlId;
        });
    if (it == m_bindings.end()) return BindingStatus::InvalidHandle;

    State& binding = it->second;
    std::string& parameter = mappingIndex == 0
                                 ? binding.parameter
                                 : binding.alternateParameter;
    std::uint8_t& slotVirtualKey = mappingIndex == 0
                                       ? binding.virtualKey
                                       : binding.alternateVirtualKey;
    std::uint8_t& slotRageKey = mappingIndex == 0
                                    ? binding.rageKey
                                    : binding.alternateRageKey;
    bool& mapped = mappingIndex == 0 ? binding.mapped
                                     : binding.alternateMapped;
    binding.mapper = "keyboard";
    mapped = newMapped;
    parameter = newParameter;
    slotVirtualKey = newVirtualKey;
    slotRageKey = newRageKey;
    binding.down = false;
    binding.events.clear();
    m_savedMappings[Identity(binding.ownerName, binding.id)] = {
        binding.mapper, binding.parameter,
        binding.alternateParameter};
    SaveLocked();
    return BindingStatus::Ok;
}

std::vector<BindingInfo> InputBindingManager::Snapshot() const
{
    std::scoped_lock lock(m_mutex);
    std::vector<BindingInfo> result;
    result.reserve(m_bindings.size());
    for (const auto& [handle, binding] : m_bindings) {
        result.push_back({handle, binding.controlId,
                          binding.ownerName, binding.id,
                          binding.description, binding.mapper,
                          binding.parameter, binding.alternateParameter,
                          binding.defaultMapper, binding.defaultParameter,
                          binding.alternateMapped, binding.down});
    }
    std::sort(result.begin(), result.end(), [](const auto& lhs, const auto& rhs) {
        return std::tie(lhs.owner, lhs.id) < std::tie(rhs.owner, rhs.id);
    });
    return result;
}

std::optional<BindingInfo> InputBindingManager::FindByControlId(
    std::uint32_t controlId) const
{
    if (controlId < NativeControlIdFirst || controlId > NativeControlIdLast)
        return std::nullopt;
    std::scoped_lock lock(m_mutex);
    const auto it = std::find_if(
        m_bindings.begin(), m_bindings.end(),
        [controlId](const auto& entry) {
            return entry.second.controlId == controlId;
        });
    if (it == m_bindings.end()) return std::nullopt;
    const State& binding = it->second;
    return BindingInfo{binding.handle, binding.controlId,
                       binding.ownerName, binding.id, binding.description,
                       binding.mapper, binding.parameter,
                       binding.alternateParameter,
                       binding.defaultMapper, binding.defaultParameter,
                       binding.alternateMapped, binding.down};
}

void InputBindingManager::UpdateKeyboardState(
    std::span<const std::uint8_t, 256> state)
{
    std::scoped_lock lock(m_mutex);
    for (auto& [handle, binding] : m_bindings) {
        if (!binding.owner) {
            binding.down = false;
            binding.events.clear();
            continue;
        }
        // rage::ioKeyboard publishes DirectInput-compatible scan codes, not
        // Win32 VK values (for example F3 is 0x3D here, not VK_F3/0x72).
        const bool down =
            (binding.mapped && state[binding.rageKey] != 0) ||
            (binding.alternateMapped &&
             state[binding.alternateRageKey] != 0);
        if (down == binding.down) continue;
        binding.down = down;
        if (binding.events.size() == MaxQueuedEvents)
            binding.events.pop_front();
        binding.events.push_back(down ? BindingEvent::Down : BindingEvent::Up);
    }
}

bool InputBindingManager::NormalizeKeyboardParameter(
    std::string_view parameter, std::uint8_t& virtualKey,
    std::string& canonicalName)
{
    std::string value = Upper(Trim(std::string(parameter)));
    value.erase(std::remove_if(value.begin(), value.end(),
                               [](char c) { return c == ' ' || c == '_'; }),
                value.end());
    if (value.empty()) return false;

    if (value.size() == 1 &&
        ((value[0] >= 'A' && value[0] <= 'Z') ||
         (value[0] >= '0' && value[0] <= '9'))) {
        virtualKey = static_cast<std::uint8_t>(value[0]);
        canonicalName = value;
        return true;
    }
    if (value[0] == 'F' && value.size() <= 3) {
        int number = 0;
        const auto parsed = std::from_chars(value.data() + 1,
                                            value.data() + value.size(), number);
        if (parsed.ec == std::errc{} && parsed.ptr == value.data() + value.size() &&
            number >= 1 && number <= 24) {
            virtualKey = static_cast<std::uint8_t>(VK_F1 + number - 1);
            canonicalName = "F" + std::to_string(number);
            return true;
        }
    }
    if (value.rfind("NUMPAD", 0) == 0 && value.size() == 7 &&
        value[6] >= '0' && value[6] <= '9') {
        virtualKey = static_cast<std::uint8_t>(VK_NUMPAD0 + value[6] - '0');
        canonicalName = value;
        return true;
    }
    if (const auto it = NamedKeys().find(value); it != NamedKeys().end()) {
        virtualKey = it->second;
        canonicalName = value;
        if (value == "ESC") canonicalName = "ESCAPE";
        if (value == "RETURN") canonicalName = "ENTER";
        if (value == "CONTROL") canonicalName = "CTRL";
        return true;
    }

    int number = -1;
    int base = 10;
    const char* first = value.data();
    if (value.size() > 2 && value[0] == '0' && value[1] == 'X') {
        first += 2;
        base = 16;
    }
    const auto parsed = std::from_chars(first, value.data() + value.size(),
                                        number, base);
    if (parsed.ec == std::errc{} && parsed.ptr == value.data() + value.size() &&
        number >= 0 && number <= 255) {
        virtualKey = static_cast<std::uint8_t>(number);
        std::ostringstream formatted;
        formatted << "0x" << std::uppercase << std::hex << std::setw(2)
                  << std::setfill('0') << number;
        canonicalName = formatted.str();
        return true;
    }
    return false;
}

bool InputBindingManager::VirtualKeyToRageKey(std::uint8_t virtualKey,
                                              std::uint8_t& rageKey)
{
    // Pause and Print Screen use multi-byte make sequences; RAGE exposes the
    // corresponding DirectInput values directly.
    if (virtualKey == VK_PAUSE) {
        rageKey = 0xC5;
        return true;
    }
    if (virtualKey == VK_SNAPSHOT) {
        rageKey = 0xB7;
        return true;
    }

    const UINT scan = MapVirtualKeyW(virtualKey, MAPVK_VK_TO_VSC_EX);
    if (!scan) return false;
    const std::uint8_t base = static_cast<std::uint8_t>(scan & 0xFF);
    rageKey = static_cast<std::uint8_t>(
        base | ((scan & 0xFF00) == 0xE000 ? 0x80 : 0));
    return rageKey != 0;
}

InputBindingManager::State* InputBindingManager::FindOwnedLocked(
    void* owner, BindingHandle handle)
{
    const auto it = m_bindings.find(handle);
    return it != m_bindings.end() && it->second.owner == owner
               ? &it->second : nullptr;
}

const InputBindingManager::State* InputBindingManager::FindOwnedLocked(
    void* owner, BindingHandle handle) const
{
    const auto it = m_bindings.find(handle);
    return it != m_bindings.end() && it->second.owner == owner
               ? &it->second : nullptr;
}

std::uint32_t InputBindingManager::AllocateControlIdLocked(
    std::string_view identity) const
{
    std::uint32_t hash = 2166136261u;
    for (const unsigned char value : identity) {
        hash ^= value;
        hash *= 16777619u;
    }

    constexpr std::uint32_t slotCount =
        NativeControlIdLast - NativeControlIdFirst + 1;
    const std::uint32_t firstSlot = hash % slotCount;
    for (std::uint32_t probe = 0; probe < slotCount; ++probe) {
        const std::uint32_t controlId = NativeControlIdFirst +
            ((firstSlot + probe) % slotCount);
        const bool occupied = std::any_of(
            m_bindings.begin(), m_bindings.end(),
            [controlId](const auto& entry) {
                return entry.second.controlId == controlId;
            });
        if (!occupied) return controlId;
    }
    return 0;
}

void InputBindingManager::PromoteNativeSlotsLocked()
{
    std::vector<State*> waiting;
    for (auto& [handle, binding] : m_bindings) {
        if (binding.controlId == 0) waiting.push_back(&binding);
    }
    std::sort(waiting.begin(), waiting.end(), [](const State* lhs,
                                                  const State* rhs) {
        return std::tie(lhs->ownerName, lhs->id) <
               std::tie(rhs->ownerName, rhs->id);
    });
    for (State* binding : waiting) {
        binding->controlId = AllocateControlIdLocked(
            Identity(binding->ownerName, binding->id));
        if (binding->controlId == 0) break;
    }
}

void InputBindingManager::LoadLocked()
{
    m_loaded = true;
    if (m_storagePath.empty()) return;
    std::ifstream stream(m_storagePath);
    if (!stream) return;

    std::string identity;
    std::string mapper;
    std::string parameter;
    std::string alternateParameter = "UNBOUND";
    const auto commit = [&] {
        if (!identity.empty() && !mapper.empty() && !parameter.empty())
            m_savedMappings[identity] = {
                mapper, parameter, alternateParameter};
        identity.clear();
        mapper.clear();
        parameter.clear();
        alternateParameter = "UNBOUND";
    };

    std::string line;
    while (std::getline(stream, line)) {
        line = Trim(std::move(line));
        if (line == "[[binding]]") {
            commit();
            continue;
        }
        const auto equals = line.find('=');
        if (equals == std::string::npos) continue;
        const std::string key = Trim(line.substr(0, equals));
        const std::string value = Trim(line.substr(equals + 1));
        std::string decoded;
        if (!ParseTomlString(value, decoded)) continue;
        if (key == "identity") identity = std::move(decoded);
        else if (key == "mapper") mapper = std::move(decoded);
        else if (key == "parameter") parameter = std::move(decoded);
        else if (key == "alternate_parameter")
            alternateParameter = std::move(decoded);
    }
    commit();
    spdlog::info("[Input] Loaded {} binding override(s)",
                 m_savedMappings.size());
}

void InputBindingManager::SaveLocked() const
{
    if (m_storagePath.empty()) return;
    std::error_code error;
    std::filesystem::create_directories(m_storagePath.parent_path(), error);
    std::filesystem::path temporaryPath = m_storagePath;
    temporaryPath += L".tmp";
    std::ofstream stream(temporaryPath, std::ios::trunc);
    if (!stream) {
        spdlog::warn("[Input] Could not save binding configuration to {}",
                     m_storagePath.string());
        return;
    }
    stream << "version = 2\n";
    std::vector<std::string> identities;
    identities.reserve(m_savedMappings.size());
    for (const auto& [identity, mapping] : m_savedMappings)
        identities.push_back(identity);
    std::sort(identities.begin(), identities.end());
    for (const auto& identity : identities) {
        const auto& mapping = m_savedMappings.at(identity);
        stream << "\n[[binding]]\n"
               << "identity = \"" << EscapeToml(identity) << "\"\n"
               << "mapper = \"" << EscapeToml(mapping.mapper) << "\"\n"
               << "parameter = \"" << EscapeToml(mapping.parameter)
               << "\"\n"
               << "alternate_parameter = \""
               << EscapeToml(mapping.alternateParameter) << "\"\n";
    }
    stream.flush();
    const bool written = stream.good();
    stream.close();
    if (!written || !MoveFileExW(temporaryPath.c_str(), m_storagePath.c_str(),
                                 MOVEFILE_REPLACE_EXISTING |
                                     MOVEFILE_WRITE_THROUGH)) {
        spdlog::warn("[Input] Could not atomically replace binding "
                     "configuration at {}", m_storagePath.string());
        std::filesystem::remove(temporaryPath, error);
    }
}

#ifdef RDR2_WASM_TEST
void InputBindingManager::ResetForTesting()
{
    std::scoped_lock lock(m_mutex);
    m_storagePath.clear();
    m_bindings.clear();
    m_savedMappings.clear();
    m_nextHandle = 1;
    m_loaded = true;
}
#endif

} // namespace rdr2::input
