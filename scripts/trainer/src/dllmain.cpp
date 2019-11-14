// dllmain.cpp : Определяет точку входа для приложения DLL.
#include "pch.h"
#include "IMenu.h"
#include <functional>
#include <deque>

#include "peds.h"

ALT_LOG_IMPL;

bool playerChanged = false;
bool playerTeleported = false;

uint32_t vehicles[] = { 47200842, 87577242, 89913743, 93893176, 150966765, 164947977, 217912393, 219205323, 253923078, 265500599, 374792535, 405368030, 470870473, 479008570, 518773733, 546463094, 572854005, 583628516, 661519966, 703203753, 742064790, 749266870, 822759307, 944661538, 962088794, 1016623270, 1115303244, 1192745176, 1297830025, 1308722738, 1468884243, 1493442814, 1582724764, 1588640480, 1596452133, 1642867136, 1745694728, 1761016051, 1770617692, 1786827347, 1871675108, 1874711736, 1975376854, 1998899080, 2109471093, 2111085321, 2135054537, 2186197941, 2241085408, 2272161463, 2306418508, 2348950726, 2428834674, 2433126343, 2466530028, 2468662606, 2536874959, 2541765679, 2584352890, 2586870135, 2606273646, 2681649752, 2743460295, 2750181085, 2818287139, 2868816046, 2907791640, 2919595783, 2947683355, 2986591923, 3005186165, 3015988546, 3194579596, 3262735550, 3268542718, 3286679556, 3289055845, 3295963005, 3306145232, 3338296319, 3371110816, 3435547054, 3470709364, 3628942640, 3658820774, 3671853927, 3724275886, 3750150746, 3791540180, 3889770730, 3897453428, 3901912305, 3916234491, 3917809588, 3962179608, 3973572878, 3999552582, 4001518838, 4011804713, 4019278719, 4036476012, 4059979704, 4114212256, 4114498448, 4130514530, 4151691040, 4158133943, 4159216269 };
constexpr uint32_t vehiclesCount = sizeof(vehicles) / 4;

static std::wstring GetModulePath(HMODULE module)
{
	DWORD size = MAX_PATH;
	std::vector<wchar_t> buffer(size);

	do
	{
		buffer.resize(size);
		GetModuleFileNameW(module, buffer.data(), size);
		size *= 1.5;
	} while (GetLastError() == ERROR_INSUFFICIENT_BUFFER);

	std::wstring modulePath = std::wstring(buffer.begin(), buffer.end());

	size_t slashPos = modulePath.size();
	for (int i = modulePath.size() - 1; i >= 0; --i)
	{
		if (modulePath[i] == L'/' || modulePath[i] == L'\\') {
			slashPos = i;
			break;
		}
	}

	std::wstring moduleDir = modulePath.substr(0, slashPos);
	return moduleDir;
}

//We will imagine that our screen is 1920x1080. Default FullHD
void DrawGameRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	float fWidth = width / (float)1920;
	float fHeight = height / (float)1080;
	float fX = (x + width / 2) / (float)1920;
	float fY = (y + height / 2) / (float)1080;
	
	Native::Invoke<void, float, float, float, float, uint8_t, uint8_t, uint8_t, uint8_t, bool>(N::DRAW_RECT, fX, fY, fWidth, fHeight, r, g, b, a, true);
}

void DrawGameText(uint16_t x, uint16_t y, const char* text, uint8_t r, uint8_t g, uint8_t b, uint8_t a, float scaleX = 0.342f, float scaleY = 0.342f)
{
	float fX = x / (float)1920;
	float fY = y / (float)1080;
	Native::Invoke<void, float, float>(N::SET_TEXT_SCALE, scaleX, scaleY);//0.342f
	Native::Invoke<void, uint8_t, uint8_t, uint8_t, uint8_t>(0x50A41AD966910F03, r, g, b, a);
	const char* varString = Native::Invoke<const char*, int, const char*, const char*>(N::_CREATE_VAR_STRING, 10, "LITERAL_STRING", text);
	Native::Invoke<void, const char*, float, float>(0xD79334A4BB99BAD1, varString, fX, fY);
}

uint32_t myLastVehicle = 0;

using mlDeque = std::deque<std::pair<uint32_t, std::function<void(void)>>>;
mlDeque modelLoadQueue;

void LoadModel(uint32_t model, std::function<void(void)> callback)
{
	bool modelExist = Native::Invoke<bool, uint32_t>(N::IS_MODEL_VALID, model);
	if (modelExist)
	{
		modelLoadQueue.push_back({ model, callback });
	}
}

void RequestModel(uint32_t model)
{
	Native::Invoke<void, uint32_t>(N::REQUEST_MODEL, model);
}

bool HasModelLoaded(uint32_t model)
{
	return Native::Invoke<bool, uint32_t>(N::HAS_MODEL_LOADED, model);
}

void SpawnVehicle(const char* vehName)
{
	uint32_t model = String::Hash(vehName);
	if (myLastVehicle)
	{
		Native::Invoke<void, uint32_t*>(N::DELETE_VEHICLE, &myLastVehicle);
		myLastVehicle = 0;
	}
	uint32_t myPed = Native::Invoke<uint32_t, uint32_t>(N::GET_PLAYER_PED, 0);
	Vector3f myPos = Native::Invoke<Vector3f, uint32_t, bool>(N::GET_ENTITY_COORDS, myPed, false);

	myLastVehicle = Native::Invoke<uint32_t, uint32_t, float, float, float, float, bool, bool, bool, bool>(N::CREATE_VEHICLE, model, myPos.x + 2, myPos.y + 2, myPos.z + 0.5, 49.29f, true, true, false, false);
	Native::Invoke<void, uint32_t>(N::SET_MODEL_AS_NO_LONGER_NEEDED, model);
}

void SpawnPed(const char* pedName)
{
	uint32_t model = String::Hash(pedName);
	uint32_t myPed = Native::Invoke<uint32_t, uint32_t>(N::GET_PLAYER_PED, 0);
	Vector3f myPos = Native::Invoke<Vector3f, uint32_t, bool>(N::GET_ENTITY_COORDS, myPed, false);

	uint32_t createdPed = Native::Invoke<uint32_t, uint32_t, float, float, float, float, bool, bool, bool, bool, bool, bool>(N::CREATE_PED, model, myPos.x + 1, myPos.y + 1, myPos.z, 0.f, false, false, false, false, true, true);
	Native::Invoke<void, uint32_t, bool>(N::SET_ENTITY_VISIBLE, createdPed, true);
	Native::Invoke<void, uint32_t, int, bool>(N::SET_ENTITY_ALPHA, createdPed, 255, false);
	Native::Invoke<void, uint32_t, bool>(0x283978A15512B2FE, createdPed, true);
	Native::Invoke<void, uint32_t>(N::SET_MODEL_AS_NO_LONGER_NEEDED, model);
}

void ChangePlayerModel(const char* modelName)
{
	uint32_t model = String::Hash(modelName);
	uint32_t myPed = Native::Invoke<uint32_t, uint32_t>(N::GET_PLAYER_PED, 0);

	Native::Invoke<void, uint32_t, uint32_t, bool>(N::SET_PLAYER_MODEL, myPed, model, false);
	Native::Invoke<void, uint32_t>(N::SET_MODEL_AS_NO_LONGER_NEEDED, model);
}

void TeleportPlayer(float x, float y, float z)
{
	uint32_t myPed = Native::Invoke<uint32_t, uint32_t>(N::GET_PLAYER_PED, 0);
	Native::Invoke<void, uint32_t, float, float, float, bool, bool, bool, bool>(N::SET_ENTITY_COORDS_NO_OFFSET, myPed, x, y, z, false, false, false, false);
}

void SetWeather(const char* weather)
{
	Native::Invoke<void, uint32_t, uint32_t, float, bool>(N::_SET_WEATHER_TYPE_TRANSITION, String::Hash(weather), String::Hash(weather), 0.5f, true);
}

void AddToClockTime(int hours, int minutes, int seconds)
{
	Native::Invoke<void, int, int, int>(N::ADD_TO_CLOCK_TIME, hours, minutes, seconds);
}

std::function<void()> GetSpawnPed(const char* model)
{ 
	return [model] { 
		LoadModel(String::Hash(model), [model] {
			SpawnPed(model);
		});
	};
}

std::function<void()> GetChangePlayerModel(const char* model)
{
	return [model] {
		LoadModel(String::Hash(model), [model] {
			ChangePlayerModel(model);
		});
	};
}

std::function<void()> GetAddToClockTime(int hours, int minutes, int seconds)
{
	return [hours, minutes, seconds] {
		AddToClockTime(hours, minutes, seconds);
	};
}
std::function<void()> GetSetWeather(const char* weather) {
	return [weather] {
		SetWeather(weather);
	};
}

std::function<void()> GetTeleportPlayer(float x, float y, float z)
{
	return [x, y, z] {
		TeleportPlayer(x, y, z);
	};
}

std::function<void()> GetSpawnVehicle(const char* vehName)
{
	return [vehName] {
		LoadModel(String::Hash(vehName), [vehName] {
			SpawnVehicle(vehName);
		});
	};
}

IMenu* currentMenu = nullptr;
int32_t menuCursor = 0;
bool menuEnabled = true;

void TickCpp()
{
	if (menuEnabled)
	{
		DrawGameRect(100, 48, 500, 50, 200, 200, 0, 200);
		DrawGameText(105, 53, currentMenu->GetTitle().c_str(), 255, 255, 255, 255);

		const std::vector<IMenu*>& items = currentMenu->GetChildItems();

		int from = 0;
		int to = 0;

		int offset = 0;
		if (menuCursor < 6)
		{
			if (currentMenu->GetType() == eMenuType::SUB_MENU)
			{
				DrawGameRect(100, 100, 500, 35, 200, 200, 0, menuCursor == 0 ? 255 : 150);
				DrawGameText(105, 105, "<< BACK >>", 255, 255, 255, 255);
				offset++;
			}
			from = 0;
			to = (items.size() < 11 ? items.size() : 11);
		}
		else if (menuCursor >= 6 && menuCursor <= (items.size() - 6))
		{
			from = menuCursor - 6;
			to = menuCursor + 6;
		}
		else
		{
			from = items.size() - 12;
			to = items.size();
		}

		for (int i = from; i < to; ++i, ++offset)
		{
			DrawGameRect(100, 100 + 37 * offset, 500, 35, 200, 0, 0, menuCursor == (i + (int)(currentMenu->GetType() == eMenuType::SUB_MENU)) ? 255 : 150);
			DrawGameText(105, 105 + 37 * offset, items[i]->GetTitle().c_str(), 255, 255, 255, 255);
		}
	}
	
	for (int i = 0; i < modelLoadQueue.size(); ++i)
	{
		auto& it = modelLoadQueue[i];
		if (HasModelLoaded(it.first))
		{
			auto callbackFunc = it.second;
			callbackFunc();
			modelLoadQueue.erase(modelLoadQueue.begin() + i, modelLoadQueue.begin() + i + 1);
			--i;
		}
		else
			RequestModel(it.first);
	}
}

uint32_t currentVehNr = 0;

extern "C" {
	DLL_EXPORT void Init(GetNativeAddressFunc getAddress)
	{
		Native::SetEssentialFunction(getAddress);
		Log::Info << "Natives registered" << Log::Endl;

		CMainMenu* mainMenu = new CMainMenu("TRAINER MENU");
		currentMenu = mainMenu;

		CSubMenu* vehiclesMenu = new CSubMenu("VEHICLES", mainMenu);
		vehiclesMenu->AddChildItem(new CMenuAction("privateopensleeper02x", GetSpawnVehicle("privateopensleeper02x")));
		vehiclesMenu->AddChildItem(new CMenuAction("privateopensleeper01x", GetSpawnVehicle("privateopensleeper01x")));
		vehiclesMenu->AddChildItem(new CMenuAction("steamerDummy", GetSpawnVehicle("steamerDummy")));
		vehiclesMenu->AddChildItem(new CMenuAction("armoredCar01x", GetSpawnVehicle("armoredCar01x")));
		vehiclesMenu->AddChildItem(new CMenuAction("armoredCar03x", GetSpawnVehicle("armoredCar03x")));
		vehiclesMenu->AddChildItem(new CMenuAction("privatebaggage01x", GetSpawnVehicle("privatebaggage01x")));
		vehiclesMenu->AddChildItem(new CMenuAction("smuggler02", GetSpawnVehicle("smuggler02")));
		vehiclesMenu->AddChildItem(new CMenuAction("keelboat", GetSpawnVehicle("keelboat")));
		vehiclesMenu->AddChildItem(new CMenuAction("boatSteam02x", GetSpawnVehicle("boatSteam02x")));
		vehiclesMenu->AddChildItem(new CMenuAction("midlandrefrigeratorCar", GetSpawnVehicle("midlandrefrigeratorCar")));
		vehiclesMenu->AddChildItem(new CMenuAction("midlandboxcar05x", GetSpawnVehicle("midlandboxcar05x")));
		vehiclesMenu->AddChildItem(new CMenuAction("caboose01x", GetSpawnVehicle("caboose01x")));
		vehiclesMenu->AddChildItem(new CMenuAction("canoe", GetSpawnVehicle("canoe")));
		vehiclesMenu->AddChildItem(new CMenuAction("canoeTreeTrunk", GetSpawnVehicle("canoeTreeTrunk")));
		vehiclesMenu->AddChildItem(new CMenuAction("cart01", GetSpawnVehicle("cart01")));
		vehiclesMenu->AddChildItem(new CMenuAction("cart02", GetSpawnVehicle("cart02")));
		vehiclesMenu->AddChildItem(new CMenuAction("cart03", GetSpawnVehicle("cart03")));
		vehiclesMenu->AddChildItem(new CMenuAction("cart04", GetSpawnVehicle("cart04")));
		vehiclesMenu->AddChildItem(new CMenuAction("cart05", GetSpawnVehicle("cart05")));
		vehiclesMenu->AddChildItem(new CMenuAction("cart06", GetSpawnVehicle("cart06")));
		vehiclesMenu->AddChildItem(new CMenuAction("cart07", GetSpawnVehicle("cart07")));
		vehiclesMenu->AddChildItem(new CMenuAction("cart08", GetSpawnVehicle("cart08")));
		vehiclesMenu->AddChildItem(new CMenuAction("coach2", GetSpawnVehicle("coach2")));
		vehiclesMenu->AddChildItem(new CMenuAction("coach3", GetSpawnVehicle("coach3")));
		vehiclesMenu->AddChildItem(new CMenuAction("coach3_cutscene", GetSpawnVehicle("coach3_cutscene")));
		vehiclesMenu->AddChildItem(new CMenuAction("coach4", GetSpawnVehicle("coach4")));
		vehiclesMenu->AddChildItem(new CMenuAction("coach5", GetSpawnVehicle("coach5")));
		vehiclesMenu->AddChildItem(new CMenuAction("coach6", GetSpawnVehicle("coach6")));
		vehiclesMenu->AddChildItem(new CMenuAction("buggy01", GetSpawnVehicle("buggy01")));
		vehiclesMenu->AddChildItem(new CMenuAction("buggy02", GetSpawnVehicle("buggy02")));
		vehiclesMenu->AddChildItem(new CMenuAction("buggy03", GetSpawnVehicle("buggy03")));
		vehiclesMenu->AddChildItem(new CMenuAction("ArmySupplyWagon", GetSpawnVehicle("ArmySupplyWagon")));
		vehiclesMenu->AddChildItem(new CMenuAction("chuckwagon000x", GetSpawnVehicle("chuckwagon000x")));
		vehiclesMenu->AddChildItem(new CMenuAction("supplywagon", GetSpawnVehicle("supplywagon")));
		vehiclesMenu->AddChildItem(new CMenuAction("supplywagon2", GetSpawnVehicle("supplywagon2")));
		vehiclesMenu->AddChildItem(new CMenuAction("logwagon", GetSpawnVehicle("logwagon")));
		vehiclesMenu->AddChildItem(new CMenuAction("logwagon2", GetSpawnVehicle("logwagon2")));
		vehiclesMenu->AddChildItem(new CMenuAction("coal_wagon", GetSpawnVehicle("coal_wagon")));
		vehiclesMenu->AddChildItem(new CMenuAction("chuckwagon002x", GetSpawnVehicle("chuckwagon002x")));
		vehiclesMenu->AddChildItem(new CMenuAction("gatling_gun", GetSpawnVehicle("gatling_gun")));
		vehiclesMenu->AddChildItem(new CMenuAction("gatlingMaxim02", GetSpawnVehicle("gatlingMaxim02")));
		vehiclesMenu->AddChildItem(new CMenuAction("handcart", GetSpawnVehicle("handcart")));
		vehiclesMenu->AddChildItem(new CMenuAction("horseBoat", GetSpawnVehicle("horseBoat")));
		vehiclesMenu->AddChildItem(new CMenuAction("hotAirBalloon01", GetSpawnVehicle("hotAirBalloon01")));
		vehiclesMenu->AddChildItem(new CMenuAction("hotchkiss_cannon", GetSpawnVehicle("hotchkiss_cannon")));
		vehiclesMenu->AddChildItem(new CMenuAction("mineCart01x", GetSpawnVehicle("mineCart01x")));
		vehiclesMenu->AddChildItem(new CMenuAction("northflatcar01x", GetSpawnVehicle("northflatcar01x")));
		vehiclesMenu->AddChildItem(new CMenuAction("privateflatcar01x", GetSpawnVehicle("privateflatcar01x")));
		vehiclesMenu->AddChildItem(new CMenuAction("northpassenger01x", GetSpawnVehicle("northpassenger01x")));
		vehiclesMenu->AddChildItem(new CMenuAction("northpassenger03x", GetSpawnVehicle("northpassenger03x")));
		vehiclesMenu->AddChildItem(new CMenuAction("privatepassenger01x", GetSpawnVehicle("privatepassenger01x")));
		vehiclesMenu->AddChildItem(new CMenuAction("oilWagon01x", GetSpawnVehicle("oilWagon01x")));
		vehiclesMenu->AddChildItem(new CMenuAction("oilWagon02x", GetSpawnVehicle("oilWagon02x")));
		vehiclesMenu->AddChildItem(new CMenuAction("pirogue", GetSpawnVehicle("pirogue")));
		vehiclesMenu->AddChildItem(new CMenuAction("pirogue2", GetSpawnVehicle("pirogue2")));
		vehiclesMenu->AddChildItem(new CMenuAction("policeWagon01x", GetSpawnVehicle("policeWagon01x")));
		vehiclesMenu->AddChildItem(new CMenuAction("policeWagongatling01x", GetSpawnVehicle("policeWagongatling01x")));
		vehiclesMenu->AddChildItem(new CMenuAction("privateCoalCar01x", GetSpawnVehicle("privateCoalCar01x")));
		vehiclesMenu->AddChildItem(new CMenuAction("NorthCoalCar01x", GetSpawnVehicle("NorthCoalCar01x")));
		vehiclesMenu->AddChildItem(new CMenuAction("winterSteamer", GetSpawnVehicle("winterSteamer")));
		vehiclesMenu->AddChildItem(new CMenuAction("wintercoalcar", GetSpawnVehicle("wintercoalcar")));
		vehiclesMenu->AddChildItem(new CMenuAction("privateboxcar04x", GetSpawnVehicle("privateboxcar04x")));
		vehiclesMenu->AddChildItem(new CMenuAction("privateboxcar02x", GetSpawnVehicle("privateboxcar02x")));
		vehiclesMenu->AddChildItem(new CMenuAction("privateboxcar01x", GetSpawnVehicle("privateboxcar01x")));
		vehiclesMenu->AddChildItem(new CMenuAction("coalHopper01x", GetSpawnVehicle("coalHopper01x")));
		vehiclesMenu->AddChildItem(new CMenuAction("privateObservationcar", GetSpawnVehicle("privateObservationcar")));
		vehiclesMenu->AddChildItem(new CMenuAction("privateArmoured", GetSpawnVehicle("privateArmoured")));
		vehiclesMenu->AddChildItem(new CMenuAction("privateDining01x", GetSpawnVehicle("privateDining01x")));
		vehiclesMenu->AddChildItem(new CMenuAction("privateRooms01x", GetSpawnVehicle("privateRooms01x")));
		vehiclesMenu->AddChildItem(new CMenuAction("privateSteamer01x", GetSpawnVehicle("privateSteamer01x")));
		vehiclesMenu->AddChildItem(new CMenuAction("northSteamer01x", GetSpawnVehicle("northSteamer01x")));
		vehiclesMenu->AddChildItem(new CMenuAction("GhostTrainSteamer", GetSpawnVehicle("GhostTrainSteamer")));
		vehiclesMenu->AddChildItem(new CMenuAction("GhostTrainCoalCar", GetSpawnVehicle("GhostTrainCoalCar")));
		vehiclesMenu->AddChildItem(new CMenuAction("GhostTrainPassenger", GetSpawnVehicle("GhostTrainPassenger")));
		vehiclesMenu->AddChildItem(new CMenuAction("GhostTrainCaboose", GetSpawnVehicle("GhostTrainCaboose")));
		vehiclesMenu->AddChildItem(new CMenuAction("rcBoat", GetSpawnVehicle("rcBoat")));
		vehiclesMenu->AddChildItem(new CMenuAction("rowboat", GetSpawnVehicle("rowboat")));
		vehiclesMenu->AddChildItem(new CMenuAction("rowboatSwamp", GetSpawnVehicle("rowboatSwamp")));
		vehiclesMenu->AddChildItem(new CMenuAction("rowboatSwamp02", GetSpawnVehicle("rowboatSwamp02")));
		vehiclesMenu->AddChildItem(new CMenuAction("ship_guama02", GetSpawnVehicle("ship_guama02")));
		vehiclesMenu->AddChildItem(new CMenuAction("turbineboat", GetSpawnVehicle("turbineboat")));
		vehiclesMenu->AddChildItem(new CMenuAction("ship_nbdGuama", GetSpawnVehicle("ship_nbdGuama")));
		vehiclesMenu->AddChildItem(new CMenuAction("ship_nbdGuama2", GetSpawnVehicle("ship_nbdGuama2")));
		vehiclesMenu->AddChildItem(new CMenuAction("skiff", GetSpawnVehicle("skiff")));
		vehiclesMenu->AddChildItem(new CMenuAction("stagecoach001x", GetSpawnVehicle("stagecoach001x")));
		vehiclesMenu->AddChildItem(new CMenuAction("stagecoach002x", GetSpawnVehicle("stagecoach002x")));
		vehiclesMenu->AddChildItem(new CMenuAction("stagecoach003x", GetSpawnVehicle("stagecoach003x")));
		vehiclesMenu->AddChildItem(new CMenuAction("stagecoach004x", GetSpawnVehicle("stagecoach004x")));
		vehiclesMenu->AddChildItem(new CMenuAction("stagecoach005x", GetSpawnVehicle("stagecoach005x")));
		vehiclesMenu->AddChildItem(new CMenuAction("stagecoach006x", GetSpawnVehicle("stagecoach006x")));
		vehiclesMenu->AddChildItem(new CMenuAction("trolley01x", GetSpawnVehicle("trolley01x")));
		vehiclesMenu->AddChildItem(new CMenuAction("TugBoat2", GetSpawnVehicle("TugBoat2")));
		vehiclesMenu->AddChildItem(new CMenuAction("wagon02x", GetSpawnVehicle("wagon02x")));
		vehiclesMenu->AddChildItem(new CMenuAction("wagon03x", GetSpawnVehicle("wagon03x")));
		vehiclesMenu->AddChildItem(new CMenuAction("wagon04x", GetSpawnVehicle("wagon04x")));
		vehiclesMenu->AddChildItem(new CMenuAction("wagon05x", GetSpawnVehicle("wagon05x")));
		vehiclesMenu->AddChildItem(new CMenuAction("wagon06x", GetSpawnVehicle("wagon06x")));
		vehiclesMenu->AddChildItem(new CMenuAction("wagonCircus01x", GetSpawnVehicle("wagonCircus01x")));
		vehiclesMenu->AddChildItem(new CMenuAction("wagonCircus02x", GetSpawnVehicle("wagonCircus02x")));
		vehiclesMenu->AddChildItem(new CMenuAction("wagonDoc01x", GetSpawnVehicle("wagonDoc01x")));
		vehiclesMenu->AddChildItem(new CMenuAction("wagonPrison01x", GetSpawnVehicle("wagonPrison01x")));
		vehiclesMenu->AddChildItem(new CMenuAction("wagonWork01x", GetSpawnVehicle("wagonWork01x")));
		vehiclesMenu->AddChildItem(new CMenuAction("wagonDairy01x", GetSpawnVehicle("wagonDairy01x")));
		vehiclesMenu->AddChildItem(new CMenuAction("wagonTraveller01x", GetSpawnVehicle("wagonTraveller01x")));
		vehiclesMenu->AddChildItem(new CMenuAction("breach_cannon", GetSpawnVehicle("breach_cannon")));
		vehiclesMenu->AddChildItem(new CMenuAction("utilliwag", GetSpawnVehicle("utilliwag")));
		vehiclesMenu->AddChildItem(new CMenuAction("gatchuck", GetSpawnVehicle("gatchuck")));
		vehiclesMenu->AddChildItem(new CMenuAction("gatchuck_2", GetSpawnVehicle("gatchuck_2")));

		size_t pedsCount = sizeof(pedsArray) / 8;

		CSubMenu* changePlayerModel = new CSubMenu("CHANGE MODEL", mainMenu);
		for (int i = 0; i < pedsCount; ++i)
		{
			changePlayerModel->AddChildItem(new CMenuAction(pedsArray[i], GetChangePlayerModel(pedsArray[i])));
		}

		CSubMenu* spawnPedMenu = new CSubMenu("SPAWN PED", mainMenu);
		for (int i = 0; i < pedsCount; ++i)
		{
			spawnPedMenu->AddChildItem(new CMenuAction(pedsArray[i], GetSpawnPed(pedsArray[i])));
		}
		
		CSubMenu* teleportMenu = new CSubMenu("TELEPORT", mainMenu);
		teleportMenu->AddChildItem(new CMenuAction("Saint Denis", GetTeleportPlayer(2700.1, -1403.39, 46.6373)));
		teleportMenu->AddChildItem(new CMenuAction("Van Horn", GetTeleportPlayer(2962.82, 583.162, 44.2948)));
		teleportMenu->AddChildItem(new CMenuAction("Annesburg", GetTeleportPlayer(2941.17, 1358.08, 44.0665)));
		teleportMenu->AddChildItem(new CMenuAction("Maclean's House", GetTeleportPlayer(2252.3, -135.647, 46.2262)));
		teleportMenu->AddChildItem(new CMenuAction("Hagen Orchards", GetTeleportPlayer(2069.06, -861.575, 42.4468)));
		teleportMenu->AddChildItem(new CMenuAction("Caliga Hall", GetTeleportPlayer(1797.61, -1352.64, 43.8385)));
		teleportMenu->AddChildItem(new CMenuAction("Braithwaite Manor", GetTeleportPlayer(889.869, -1910.26, 45.2703)));
		teleportMenu->AddChildItem(new CMenuAction("Emerald Ranch", GetTeleportPlayer(1424.09, 316.904, 88.6065)));
		teleportMenu->AddChildItem(new CMenuAction("Scarlett Meadows", GetTeleportPlayer(1211.79, -201.815, 101.436)));
		teleportMenu->AddChildItem(new CMenuAction("Alone Tree", GetTeleportPlayer(491.451, -307.623, 143.75)));
		teleportMenu->AddChildItem(new CMenuAction("Flatneck Station", GetTeleportPlayer(-344.127, -363.465, 88.0377)));
		teleportMenu->AddChildItem(new CMenuAction("Wallace Station", GetTeleportPlayer(-1308.38, 400.834, 95.3829)));
		teleportMenu->AddChildItem(new CMenuAction("Riggs Station", GetTeleportPlayer(-1087.97, -585.786, 81.4831)));
		teleportMenu->AddChildItem(new CMenuAction("Valentine", GetTeleportPlayer(-262.849, 793.404, 118.587)));
		teleportMenu->AddChildItem(new CMenuAction("Chadwick Farm", GetTeleportPlayer(-391.389, 916.105, 117.644)));
		teleportMenu->AddChildItem(new CMenuAction("Fort Wallace", GetTeleportPlayer(366.483, 1456.73, 178.916)));
		teleportMenu->AddChildItem(new CMenuAction("Downes Ranch", GetTeleportPlayer(-822.636, 325.741, 95.2731)));
		teleportMenu->AddChildItem(new CMenuAction("Strawberry", GetTeleportPlayer(-1815.63, -396.749, 161.602)));
		teleportMenu->AddChildItem(new CMenuAction("Big Valley", GetTeleportPlayer(-1819.9, -599.968, 154.616)));
		teleportMenu->AddChildItem(new CMenuAction("Holding Camp", GetTeleportPlayer(-1588.19, -936.054, 84.1072)));
		teleportMenu->AddChildItem(new CMenuAction("Blackwater", GetTeleportPlayer(-858.065, -1337.73, 44.4866)));
		teleportMenu->AddChildItem(new CMenuAction("Great Plains", GetTeleportPlayer(-898.997, -1654.69, 68.5928)));
		teleportMenu->AddChildItem(new CMenuAction("Quaker's Cove", GetTeleportPlayer(-1191.3, -1950.71, 43.5789)));
		teleportMenu->AddChildItem(new CMenuAction("Thieves Landing", GetTeleportPlayer(-1452.17, -2329.4, 42.9603)));

		CSubMenu* weatherMenu = new CSubMenu("WEATHER", mainMenu);
		weatherMenu->AddChildItem(new CMenuAction("OVERCAST", GetSetWeather("OVERCAST")));
		weatherMenu->AddChildItem(new CMenuAction("RAIN", GetSetWeather("RAIN")));
		weatherMenu->AddChildItem(new CMenuAction("FOG", GetSetWeather("FOG")));
		weatherMenu->AddChildItem(new CMenuAction("SNOWLIGHT", GetSetWeather("SNOWLIGHT")));
		weatherMenu->AddChildItem(new CMenuAction("THUNDER", GetSetWeather("THUNDER")));
		weatherMenu->AddChildItem(new CMenuAction("BLIZZARD", GetSetWeather("BLIZZARD")));
		weatherMenu->AddChildItem(new CMenuAction("SNOW", GetSetWeather("SNOW")));
		weatherMenu->AddChildItem(new CMenuAction("MISTY", GetSetWeather("MISTY")));
		weatherMenu->AddChildItem(new CMenuAction("SUNNY", GetSetWeather("SUNNY")));
		weatherMenu->AddChildItem(new CMenuAction("HIGHPRESSURE", GetSetWeather("HIGHPRESSURE")));
		weatherMenu->AddChildItem(new CMenuAction("CLEARING", GetSetWeather("CLEARING")));
		weatherMenu->AddChildItem(new CMenuAction("SLEET", GetSetWeather("SLEET")));
		weatherMenu->AddChildItem(new CMenuAction("DRIZZLE", GetSetWeather("DRIZZLE")));
		weatherMenu->AddChildItem(new CMenuAction("SHOWER", GetSetWeather("SHOWER")));
		weatherMenu->AddChildItem(new CMenuAction("SNOWCLEARING", GetSetWeather("SNOWCLEARING")));
		weatherMenu->AddChildItem(new CMenuAction("OVERCASTDARK", GetSetWeather("OVERCASTDARK")));
		weatherMenu->AddChildItem(new CMenuAction("THUNDERSTORM", GetSetWeather("THUNDERSTORM")));
		weatherMenu->AddChildItem(new CMenuAction("SANDSTORM", GetSetWeather("SANDSTORM")));
		weatherMenu->AddChildItem(new CMenuAction("HURRICANE", GetSetWeather("HURRICANE")));
		weatherMenu->AddChildItem(new CMenuAction("HAIL", GetSetWeather("HAIL")));
		weatherMenu->AddChildItem(new CMenuAction("WHITEOUT", GetSetWeather("WHITEOUT")));
		weatherMenu->AddChildItem(new CMenuAction("GROUNDBLIZZARD", GetSetWeather("GROUNDBLIZZARD")));

		CSubMenu* timeMenu = new CSubMenu("TIME", mainMenu);
		timeMenu->AddChildItem(new CMenuAction("+ 4 hours", GetAddToClockTime(4, 0, 0)));
		timeMenu->AddChildItem(new CMenuAction("+ 1 hour", GetAddToClockTime(1, 0, 0)));
		timeMenu->AddChildItem(new CMenuAction("+ 15 minutes", GetAddToClockTime(0, 15, 0)));
		timeMenu->AddChildItem(new CMenuAction("- 4 hours", GetAddToClockTime(-4, 0, 0)));
		timeMenu->AddChildItem(new CMenuAction("- 1 hour", GetAddToClockTime(-1, 0, 0)));
		timeMenu->AddChildItem(new CMenuAction("- 15 minutes", GetAddToClockTime(0, -15, 0)));


		mainMenu->AddChildItem(vehiclesMenu);
		mainMenu->AddChildItem(changePlayerModel);
		mainMenu->AddChildItem(spawnPedMenu);
		mainMenu->AddChildItem(teleportMenu);
		mainMenu->AddChildItem(weatherMenu);
		mainMenu->AddChildItem(timeMenu);
	}

	DLL_EXPORT void Tick()
	{
		TickCpp();
	}

	DLL_EXPORT void OnKeyDown(uint32_t key)
	{
		if (key == 0x72)
		{
			menuEnabled = !menuEnabled;
		}
		if (key == 0x26 && menuEnabled)
		{
			menuCursor--;
			if (menuCursor < 0)
				menuCursor = currentMenu->ChildCount() - (currentMenu->GetType() == eMenuType::SUB_MENU ? 0 : 1);
		}
		if (key == 0x28 && menuEnabled)
		{
			menuCursor++;
			int maxItems = currentMenu->ChildCount() + (currentMenu->GetType() == eMenuType::SUB_MENU ? 1 : 0);
			if (menuCursor >= maxItems)
				menuCursor = 0;
		}
		if (key == 0x21 && menuEnabled)
		{
			menuCursor -= 10;
			if (menuCursor < 0)
				menuCursor = currentMenu->ChildCount() - (currentMenu->GetType() == eMenuType::SUB_MENU ? 0 : 1);
		}
		if (key == 0x22 && menuEnabled)
		{
			menuCursor += 10;
			int maxItems = currentMenu->ChildCount() + (currentMenu->GetType() == eMenuType::SUB_MENU ? 1 : 0);
			if (menuCursor >= maxItems)
				menuCursor = 0;
		}
		if (key == 0x0D && menuEnabled)
		{
			int cursorPos = menuCursor;
			if (menuCursor == 0 && currentMenu->GetType() == eMenuType::SUB_MENU)
			{
				menuCursor = 0;
				currentMenu = currentMenu->GetParent();
				return;
			}

			if (currentMenu->GetType() == eMenuType::SUB_MENU)
				cursorPos--;
			
			IMenu* childItem = currentMenu->GetChildItems()[cursorPos];
			if (childItem->GetType() == eMenuType::MENU_ACTION)
			{
				CMenuAction* menuAction = (CMenuAction*)childItem;
				auto func = menuAction->GetActionFunc();
				func();
				return;
			}
			else if (childItem->GetType() == eMenuType::SUB_MENU)
			{
				menuCursor = 0;
				currentMenu = childItem;
			}
		}
		if (key == 0x08 && menuEnabled)
		{
			if (currentMenu->GetType() == eMenuType::SUB_MENU)
			{
				menuCursor = 0;
				currentMenu = currentMenu->GetParent();
				return;
			}
			if (currentMenu->GetType() == eMenuType::MAIN_MENU)
			{
				menuEnabled = false;
				return;
			}
		}
	}

	DLL_EXPORT void OnKeyUp(uint32_t key)
	{
	}
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
	{
		std::wstring _moduleDir = GetModulePath(hModule);

		std::wstring logPath = _moduleDir + L"/log.txt";
		Log::Push(new Log::FileStream(logPath));
		break;
	}
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

