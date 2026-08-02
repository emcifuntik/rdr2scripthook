#include "stdafx.h"
#include "CMemory.h"
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <thread>
#include <unordered_map>
#include <toml.hpp>
#include "Logger.h"
#include "input/InputBindingManager.h"
#include "scripting/CScriptManager.h"

std::wstring _moduleDir;
static std::wstring GetModulePath(HMODULE module)
{
	DWORD size = MAX_PATH;
	std::vector<wchar_t> buffer(size);

	do
	{
		buffer.resize(size);
		GetModuleFileNameW(module, buffer.data(), size);
		size = (DWORD)(size * 1.5);
	} while (GetLastError() == ERROR_INSUFFICIENT_BUFFER);

	std::wstring modulePath = std::wstring(buffer.begin(), buffer.end());

	size_t slashPos = modulePath.size();
	for (int i = int(modulePath.size() - 1); i >= 0; --i)
	{
		if (modulePath[i] == L'/' || modulePath[i] == L'\\') {
			slashPos = i;
			break;
		}
	}

	std::wstring moduleDir = modulePath.substr(0, slashPos);
	return moduleDir;
}

static void PredeclareInputBindings(const std::filesystem::path& modsPath)
{
	std::vector<std::filesystem::path> manifests;
	try {
		if (!std::filesystem::exists(modsPath)) return;
		for (const auto& entry : std::filesystem::directory_iterator(modsPath)) {
			if (!entry.is_directory()) continue;
			const auto manifest = entry.path() / "mod.toml";
			if (std::filesystem::exists(manifest)) manifests.push_back(manifest);
		}
	} catch (const std::exception& exception) {
		spdlog::warn("[Input] Could not scan binding declarations in '{}': {}",
			modsPath.string(), exception.what());
		return;
	}

	std::sort(manifests.begin(), manifests.end());
	std::size_t loaded = 0;
	for (const auto& manifestPath : manifests) {
		try {
			const auto data = toml::parse(manifestPath.string());
			if (!data.contains("mod") || !data.contains("input")) continue;
			const auto& mod = toml::find(data, "mod");
			const auto& input = toml::find(data, "input");
			if (!mod.contains("name") || !input.contains("bindings")) continue;

			const std::string modName = toml::find<std::string>(mod, "name");
			const std::string directory = manifestPath.parent_path()
				.filename().string();
			const std::string ownerName = directory + "/" + modName;
			for (const auto& declaration :
				 toml::find(input, "bindings").as_array()) {
				if (!declaration.is_table() || !declaration.contains("id") ||
					!declaration.contains("description") ||
					!declaration.contains("default")) {
					spdlog::warn(
						"[Input] Ignoring an incomplete binding declaration in '{}'",
						manifestPath.string());
					continue;
				}
				const std::string mapper = declaration.contains("mapper")
					? toml::find<std::string>(declaration, "mapper")
					: "keyboard";
				const auto registration =
					rdr2::input::InputBindingManager::Instance().Predeclare(
						ownerName,
						toml::find<std::string>(declaration, "id"),
						toml::find<std::string>(declaration, "description"),
						mapper,
						toml::find<std::string>(declaration, "default"));
				if (registration.status == rdr2::input::BindingStatus::Ok ||
					registration.status ==
						rdr2::input::BindingStatus::AlreadyRegistered) {
					++loaded;
				} else {
					spdlog::warn(
						"[Input] Rejected binding declaration in '{}': status {}",
						manifestPath.string(),
						static_cast<int>(registration.status));
				}
			}
		} catch (const std::exception& exception) {
			spdlog::warn("[Input] Could not parse binding declarations in '{}': {}",
				manifestPath.string(), exception.what());
		}
	}
	spdlog::info("[Input] Predeclared {} native binding(s)", loaded);
}

void Init()
{
	rdr2::input::InputBindingManager::Instance().SetStoragePath(
		std::filesystem::path(_moduleDir) / "input-bindings.toml");
	PredeclareInputBindings(std::filesystem::path(_moduleDir) / "mods");
	CMemory::RunHooks();
	CScriptManager::Instance().Init();
	spdlog::info("RDR2 Scripthook fully initialized");
}

LPSTR (*GetCommandLineA_Orig)() = nullptr;
LPSTR WINAPI GetCommandLineA_Hook()
{
	static bool inited = false;
	if (!inited) {
		inited = true;
		Init();
	}
	return GetCommandLineA_Orig();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
	{
		_moduleDir = GetModulePath(hModule);
		CScriptManager::Instance().SetClientPath(_moduleDir);
		CMemory::Base() = (uintptr_t)GetModuleHandle(NULL);

		// Initialize logger to write to launcher folder
		std::filesystem::path logPath = std::filesystem::path(_moduleDir) / "log.txt";
		rdr2::Logger::Initialize(logPath);

		std::wstring crossMapPath = _moduleDir + L"/crossmap.dat";
		std::ifstream crossIn(crossMapPath, std::ifstream::binary);

		uint64_t oldHash;
		uint64_t newHash;
		if (crossIn.good())
		{
			while (!crossIn.eof())
			{
				crossIn.read((char*)& oldHash, sizeof(uint64_t));
				crossIn.read((char*)& newHash, sizeof(uint64_t));
				CScriptManager::Instance().AddCrossMapEntry(oldHash, newHash);
			}
		}

		spdlog::info("RDR2 Scripthook initialized");

		MH_Initialize();
		CMemory(GetCommandLineA).Detour(GetCommandLineA_Hook, &GetCommandLineA_Orig);
		break;
	}
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
		break;
    case DLL_PROCESS_DETACH:
		CScriptManager::Instance().ShutdownWasmMods();
		MH_Uninitialize();
		spdlog::info("RDR2 Scripthook deinitialized");
		rdr2::Logger::Shutdown();
        break;
    }
    return TRUE;
}
