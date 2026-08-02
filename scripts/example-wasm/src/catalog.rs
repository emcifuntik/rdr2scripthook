#[derive(Clone, Copy, Debug, Eq, PartialEq)]
pub enum Screen {
    Player,
    Horses,
    PlayerModels,
    Peds,
    Vehicles,
    Teleports,
    Weather,
    Time,
}

impl Screen {
    pub const fn from_index(index: usize) -> Option<Self> {
        match index {
            0 => Some(Self::Player),
            1 => Some(Self::Horses),
            2 => Some(Self::PlayerModels),
            3 => Some(Self::Peds),
            4 => Some(Self::Vehicles),
            5 => Some(Self::Teleports),
            6 => Some(Self::Weather),
            7 => Some(Self::Time),
            _ => None,
        }
    }

    pub const fn index(self) -> usize {
        self as usize
    }

    pub fn len(self) -> usize {
        match self {
            Self::Player => PLAYER_MENU.len(),
            Self::Horses => HORSES.len(),
            Self::PlayerModels | Self::Peds => CHARACTERS.len(),
            Self::Vehicles => VEHICLES.len(),
            Self::Teleports => TELEPORTS.len(),
            Self::Weather => WEATHER.len(),
            Self::Time => TIME_MENU.len(),
        }
    }

    pub fn entry(self, index: usize) -> Option<MenuEntry> {
        match self {
            Self::Player => PLAYER_MENU.get(index).copied(),
            Self::Horses => HORSES.get(index).map(|model| MenuEntry {
                label: model.label,
                action: Action::LoadModel {
                    model: model.model,
                    purpose: ModelPurpose::SpawnHorse,
                },
            }),
            Self::PlayerModels => CHARACTERS.get(index).map(|model| MenuEntry {
                label: model.label,
                action: Action::LoadModel {
                    model: model.model,
                    purpose: ModelPurpose::ChangePlayer,
                },
            }),
            Self::Peds => CHARACTERS.get(index).map(|model| MenuEntry {
                label: model.label,
                action: Action::LoadModel {
                    model: model.model,
                    purpose: ModelPurpose::SpawnPed,
                },
            }),
            Self::Vehicles => VEHICLES.get(index).map(|model| MenuEntry {
                label: model.label,
                action: Action::LoadModel {
                    model: model.model,
                    purpose: ModelPurpose::SpawnVehicle,
                },
            }),
            Self::Teleports => TELEPORTS.get(index).map(|location| MenuEntry {
                label: location.label,
                action: Action::Teleport {
                    x: location.x,
                    y: location.y,
                    z: location.z,
                },
            }),
            Self::Weather => WEATHER.get(index).map(|weather| MenuEntry {
                label: weather,
                action: Action::SetWeather(weather),
            }),
            Self::Time => TIME_MENU.get(index).copied(),
        }
    }
}

#[derive(Clone, Copy)]
pub struct MenuEntry {
    pub label: &'static str,
    pub action: Action,
}

#[derive(Clone, Copy)]
pub enum Action {
    LoadModel {
        model: &'static str,
        purpose: ModelPurpose,
    },
    Teleport {
        x: f32,
        y: f32,
        z: f32,
    },
    SetWeather(&'static str),
    AddTime {
        hours: i32,
        minutes: i32,
    },
    Heal,
    RefillStamina,
    ToggleInvincibility,
}

#[derive(Clone, Copy, Debug, Eq, PartialEq)]
pub enum ModelPurpose {
    SpawnHorse,
    ChangePlayer,
    SpawnPed,
    SpawnVehicle,
}

impl ModelPurpose {
    pub const fn description(self) -> &'static str {
        match self {
            Self::SpawnHorse => "horse",
            Self::ChangePlayer => "player model",
            Self::SpawnPed => "ped",
            Self::SpawnVehicle => "vehicle",
        }
    }
}

#[derive(Clone, Copy)]
struct ModelOption {
    label: &'static str,
    model: &'static str,
}

#[derive(Clone, Copy)]
struct Location {
    label: &'static str,
    x: f32,
    y: f32,
    z: f32,
}

const PLAYER_MENU: &[MenuEntry] = &[
    MenuEntry {
        label: "Restore health",
        action: Action::Heal,
    },
    MenuEntry {
        label: "Restore stamina",
        action: Action::RefillStamina,
    },
    MenuEntry {
        label: "Toggle invincibility",
        action: Action::ToggleInvincibility,
    },
];

const TIME_MENU: &[MenuEntry] = &[
    MenuEntry {
        label: "+ 4 hours",
        action: Action::AddTime {
            hours: 4,
            minutes: 0,
        },
    },
    MenuEntry {
        label: "+ 1 hour",
        action: Action::AddTime {
            hours: 1,
            minutes: 0,
        },
    },
    MenuEntry {
        label: "+ 15 minutes",
        action: Action::AddTime {
            hours: 0,
            minutes: 15,
        },
    },
    MenuEntry {
        label: "- 4 hours",
        action: Action::AddTime {
            hours: -4,
            minutes: 0,
        },
    },
    MenuEntry {
        label: "- 1 hour",
        action: Action::AddTime {
            hours: -1,
            minutes: 0,
        },
    },
    MenuEntry {
        label: "- 15 minutes",
        action: Action::AddTime {
            hours: 0,
            minutes: -15,
        },
    },
];

const HORSES: &[ModelOption] = &[
    ModelOption {
        label: "American Paint - Overo",
        model: "a_c_horse_americanpaint_overo",
    },
    ModelOption {
        label: "American Paint - Tobiano",
        model: "a_c_horse_americanpaint_tobiano",
    },
    ModelOption {
        label: "Appaloosa - Blanket",
        model: "a_c_horse_appaloosa_blanket",
    },
    ModelOption {
        label: "Appaloosa - Leopard",
        model: "a_c_horse_appaloosa_leopard",
    },
    ModelOption {
        label: "Arabian - Black",
        model: "a_c_horse_arabian_black",
    },
    ModelOption {
        label: "Arabian - White",
        model: "a_c_horse_arabian_white",
    },
    ModelOption {
        label: "Ardennes - Bay Roan",
        model: "a_c_horse_ardennes_bayroan",
    },
    ModelOption {
        label: "Ardennes - Iron Grey",
        model: "a_c_horse_ardennes_irongreyroan",
    },
    ModelOption {
        label: "Belgian - Blond Chestnut",
        model: "a_c_horse_belgian_blondchestnut",
    },
    ModelOption {
        label: "Belgian - Mealy Chestnut",
        model: "a_c_horse_belgian_mealychestnut",
    },
    ModelOption {
        label: "Gang Horse - Arthur",
        model: "a_c_horse_gang_arthur",
    },
    ModelOption {
        label: "Gang Horse - Bill",
        model: "a_c_horse_gang_bill",
    },
    ModelOption {
        label: "Gang Horse - Charles",
        model: "a_c_horse_gang_charles",
    },
    ModelOption {
        label: "Gang Horse - Dutch",
        model: "a_c_horse_gang_dutch",
    },
    ModelOption {
        label: "Gang Horse - Hosea",
        model: "a_c_horse_gang_hosea",
    },
    ModelOption {
        label: "Gang Horse - Javier",
        model: "a_c_horse_gang_javier",
    },
    ModelOption {
        label: "Gang Horse - John",
        model: "a_c_horse_gang_john",
    },
    ModelOption {
        label: "Gang Horse - Lenny",
        model: "a_c_horse_gang_lenny",
    },
    ModelOption {
        label: "Gang Horse - Micah",
        model: "a_c_horse_gang_micah",
    },
    ModelOption {
        label: "Gang Horse - Sadie",
        model: "a_c_horse_gang_sadie",
    },
    ModelOption {
        label: "Gang Horse - Sean",
        model: "a_c_horse_gang_sean",
    },
    ModelOption {
        label: "Gang Horse - Trelawny",
        model: "a_c_horse_gang_trelawney",
    },
    ModelOption {
        label: "Missouri Fox Trotter",
        model: "a_c_horse_missourifoxtrotter_silverdapplepinto",
    },
    ModelOption {
        label: "Mustang - Wild Bay",
        model: "a_c_horse_mustang_wildbay",
    },
    ModelOption {
        label: "Nokota - Blue Roan",
        model: "a_c_horse_nokota_blueroan",
    },
    ModelOption {
        label: "Shire - Raven Black",
        model: "a_c_horse_shire_ravenblack",
    },
    ModelOption {
        label: "Thoroughbred - Dapple Grey",
        model: "a_c_horse_thoroughbred_dapplegrey",
    },
    ModelOption {
        label: "Turkoman - Albino",
        model: "a_c_horse_turkoman_albino",
    },
    ModelOption {
        label: "Turkoman - Gold",
        model: "a_c_horse_turkoman_gold",
    },
    ModelOption {
        label: "Winter Horse",
        model: "a_c_horse_winter02_01",
    },
];

const CHARACTERS: &[ModelOption] = &[
    ModelOption {
        label: "Arthur Morgan",
        model: "player_zero",
    },
    ModelOption {
        label: "John Marston",
        model: "player_three",
    },
    ModelOption {
        label: "Dutch van der Linde",
        model: "cs_dutch",
    },
    ModelOption {
        label: "Hosea Matthews",
        model: "cs_hoseamatthews",
    },
    ModelOption {
        label: "Micah Bell",
        model: "cs_micahbell",
    },
    ModelOption {
        label: "Sadie Adler",
        model: "cs_mrsadler",
    },
    ModelOption {
        label: "Abigail Roberts",
        model: "cs_abigailroberts",
    },
    ModelOption {
        label: "Charles Smith",
        model: "cs_charlessmith",
    },
    ModelOption {
        label: "Bill Williamson",
        model: "cs_billwilliamson",
    },
    ModelOption {
        label: "Javier Escuella",
        model: "cs_javierescuella",
    },
    ModelOption {
        label: "Lenny Summers",
        model: "cs_lenny",
    },
    ModelOption {
        label: "Sean MacGuire",
        model: "cs_sean",
    },
    ModelOption {
        label: "Uncle",
        model: "cs_uncle",
    },
    ModelOption {
        label: "Jack Marston",
        model: "cs_jackmarston",
    },
    ModelOption {
        label: "Angelo Bronte",
        model: "cs_bronte",
    },
    ModelOption {
        label: "Colm O'Driscoll",
        model: "cs_colmodriscoll",
    },
    ModelOption {
        label: "Leviticus Cornwall",
        model: "cs_leviticuscornwall",
    },
    ModelOption {
        label: "Edgar Ross",
        model: "cs_edgarross",
    },
    ModelOption {
        label: "Andrew Milton",
        model: "cs_miltonandrews",
    },
    ModelOption {
        label: "Valentine Deputy",
        model: "s_m_m_valdeputy_01",
    },
    ModelOption {
        label: "Saint Denis Police",
        model: "s_m_m_ambientsdpolice_01",
    },
    ModelOption {
        label: "Rhodes Cowpoke",
        model: "s_m_m_rhdcowpoke_01",
    },
    ModelOption {
        label: "Valentine Cowpoke",
        model: "s_m_m_valcowpoke_01",
    },
    ModelOption {
        label: "Train Station Worker",
        model: "s_m_m_trainstationworker_01",
    },
];

const VEHICLES: &[ModelOption] = &[
    ModelOption {
        label: "Canoe",
        model: "canoe",
    },
    ModelOption {
        label: "Tree Trunk Canoe",
        model: "canoeTreeTrunk",
    },
    ModelOption {
        label: "Rowboat",
        model: "rowboat",
    },
    ModelOption {
        label: "Swamp Rowboat",
        model: "rowboatSwamp",
    },
    ModelOption {
        label: "Pirogue",
        model: "pirogue",
    },
    ModelOption {
        label: "Keelboat",
        model: "keelboat",
    },
    ModelOption {
        label: "Steam Boat",
        model: "boatSteam02x",
    },
    ModelOption {
        label: "Cart 01",
        model: "cart01",
    },
    ModelOption {
        label: "Cart 02",
        model: "cart02",
    },
    ModelOption {
        label: "Cart 03",
        model: "cart03",
    },
    ModelOption {
        label: "Cart 04",
        model: "cart04",
    },
    ModelOption {
        label: "Cart 05",
        model: "cart05",
    },
    ModelOption {
        label: "Cart 06",
        model: "cart06",
    },
    ModelOption {
        label: "Cart 07",
        model: "cart07",
    },
    ModelOption {
        label: "Cart 08",
        model: "cart08",
    },
    ModelOption {
        label: "Coach 2",
        model: "coach2",
    },
    ModelOption {
        label: "Coach 3",
        model: "coach3",
    },
    ModelOption {
        label: "Coach 4",
        model: "coach4",
    },
    ModelOption {
        label: "Coach 5",
        model: "coach5",
    },
    ModelOption {
        label: "Coach 6",
        model: "coach6",
    },
    ModelOption {
        label: "Buggy 01",
        model: "buggy01",
    },
    ModelOption {
        label: "Buggy 02",
        model: "buggy02",
    },
    ModelOption {
        label: "Buggy 03",
        model: "buggy03",
    },
    ModelOption {
        label: "Army Supply Wagon",
        model: "ArmySupplyWagon",
    },
    ModelOption {
        label: "Chuck Wagon",
        model: "chuckwagon000x",
    },
    ModelOption {
        label: "Supply Wagon",
        model: "supplywagon",
    },
    ModelOption {
        label: "Supply Wagon 2",
        model: "supplywagon2",
    },
    ModelOption {
        label: "Log Wagon",
        model: "logwagon",
    },
    ModelOption {
        label: "Log Wagon 2",
        model: "logwagon2",
    },
    ModelOption {
        label: "Coal Wagon",
        model: "coal_wagon",
    },
    ModelOption {
        label: "Gatling Gun",
        model: "gatling_gun",
    },
    ModelOption {
        label: "Gatling Maxim",
        model: "gatlingMaxim02",
    },
    ModelOption {
        label: "Handcart",
        model: "handcart",
    },
    ModelOption {
        label: "Horse Boat",
        model: "horseBoat",
    },
    ModelOption {
        label: "Hot Air Balloon",
        model: "hotAirBalloon01",
    },
    ModelOption {
        label: "Hotchkiss Cannon",
        model: "hotchkiss_cannon",
    },
    ModelOption {
        label: "Police Wagon",
        model: "policeWagon01x",
    },
    ModelOption {
        label: "Police Gatling Wagon",
        model: "policeWagongatling01x",
    },
    ModelOption {
        label: "Oil Wagon 01",
        model: "oilWagon01x",
    },
    ModelOption {
        label: "Oil Wagon 02",
        model: "oilWagon02x",
    },
    ModelOption {
        label: "Stagecoach 001",
        model: "stagecoach001x",
    },
    ModelOption {
        label: "Stagecoach 002",
        model: "stagecoach002x",
    },
    ModelOption {
        label: "Stagecoach 003",
        model: "stagecoach003x",
    },
    ModelOption {
        label: "Stagecoach 004",
        model: "stagecoach004x",
    },
    ModelOption {
        label: "Stagecoach 005",
        model: "stagecoach005x",
    },
    ModelOption {
        label: "Stagecoach 006",
        model: "stagecoach006x",
    },
    ModelOption {
        label: "Trolley",
        model: "trolley01x",
    },
    ModelOption {
        label: "Wagon 02",
        model: "wagon02x",
    },
    ModelOption {
        label: "Wagon 03",
        model: "wagon03x",
    },
    ModelOption {
        label: "Wagon 04",
        model: "wagon04x",
    },
    ModelOption {
        label: "Wagon 05",
        model: "wagon05x",
    },
    ModelOption {
        label: "Wagon 06",
        model: "wagon06x",
    },
    ModelOption {
        label: "Circus Wagon 01",
        model: "wagonCircus01x",
    },
    ModelOption {
        label: "Circus Wagon 02",
        model: "wagonCircus02x",
    },
    ModelOption {
        label: "Doctor Wagon",
        model: "wagonDoc01x",
    },
    ModelOption {
        label: "Prison Wagon",
        model: "wagonPrison01x",
    },
    ModelOption {
        label: "Work Wagon",
        model: "wagonWork01x",
    },
    ModelOption {
        label: "Dairy Wagon",
        model: "wagonDairy01x",
    },
    ModelOption {
        label: "Traveller Wagon",
        model: "wagonTraveller01x",
    },
    ModelOption {
        label: "Armored Car",
        model: "armoredCar01x",
    },
    ModelOption {
        label: "Breach Cannon",
        model: "breach_cannon",
    },
    ModelOption {
        label: "Private Passenger Car",
        model: "privatepassenger01x",
    },
    ModelOption {
        label: "Private Dining Car",
        model: "privateDining01x",
    },
    ModelOption {
        label: "Private Steamer",
        model: "privateSteamer01x",
    },
    ModelOption {
        label: "North Steamer",
        model: "northSteamer01x",
    },
    ModelOption {
        label: "Ghost Train Steamer",
        model: "GhostTrainSteamer",
    },
    ModelOption {
        label: "Ghost Train Passenger",
        model: "GhostTrainPassenger",
    },
    ModelOption {
        label: "Private Sleeper 02",
        model: "privateopensleeper02x",
    },
    ModelOption {
        label: "Private Sleeper 01",
        model: "privateopensleeper01x",
    },
    ModelOption {
        label: "Dummy Steamer",
        model: "steamerDummy",
    },
    ModelOption {
        label: "Armored Car 03",
        model: "armoredCar03x",
    },
    ModelOption {
        label: "Private Baggage Car",
        model: "privatebaggage01x",
    },
    ModelOption {
        label: "Smuggler Boat",
        model: "smuggler02",
    },
    ModelOption {
        label: "Refrigerator Car",
        model: "midlandrefrigeratorCar",
    },
    ModelOption {
        label: "Midland Boxcar",
        model: "midlandboxcar05x",
    },
    ModelOption {
        label: "Caboose",
        model: "caboose01x",
    },
    ModelOption {
        label: "Cutscene Coach",
        model: "coach3_cutscene",
    },
    ModelOption {
        label: "Chuck Wagon 002",
        model: "chuckwagon002x",
    },
    ModelOption {
        label: "Mine Cart",
        model: "mineCart01x",
    },
    ModelOption {
        label: "North Flatcar",
        model: "northflatcar01x",
    },
    ModelOption {
        label: "Private Flatcar",
        model: "privateflatcar01x",
    },
    ModelOption {
        label: "North Passenger 01",
        model: "northpassenger01x",
    },
    ModelOption {
        label: "North Passenger 03",
        model: "northpassenger03x",
    },
    ModelOption {
        label: "Pirogue 2",
        model: "pirogue2",
    },
    ModelOption {
        label: "Private Coal Car",
        model: "privateCoalCar01x",
    },
    ModelOption {
        label: "North Coal Car",
        model: "NorthCoalCar01x",
    },
    ModelOption {
        label: "Winter Steamer",
        model: "winterSteamer",
    },
    ModelOption {
        label: "Winter Coal Car",
        model: "wintercoalcar",
    },
    ModelOption {
        label: "Private Boxcar 04",
        model: "privateboxcar04x",
    },
    ModelOption {
        label: "Private Boxcar 02",
        model: "privateboxcar02x",
    },
    ModelOption {
        label: "Private Boxcar 01",
        model: "privateboxcar01x",
    },
    ModelOption {
        label: "Coal Hopper",
        model: "coalHopper01x",
    },
    ModelOption {
        label: "Private Observation Car",
        model: "privateObservationcar",
    },
    ModelOption {
        label: "Private Armoured Car",
        model: "privateArmoured",
    },
    ModelOption {
        label: "Private Rooms Car",
        model: "privateRooms01x",
    },
    ModelOption {
        label: "Ghost Train Coal Car",
        model: "GhostTrainCoalCar",
    },
    ModelOption {
        label: "Ghost Train Caboose",
        model: "GhostTrainCaboose",
    },
    ModelOption {
        label: "RC Boat",
        model: "rcBoat",
    },
    ModelOption {
        label: "Swamp Rowboat 02",
        model: "rowboatSwamp02",
    },
    ModelOption {
        label: "Guarma Ship",
        model: "ship_guama02",
    },
    ModelOption {
        label: "Turbine Boat",
        model: "turbineboat",
    },
    ModelOption {
        label: "NBD Guarma Ship",
        model: "ship_nbdGuama",
    },
    ModelOption {
        label: "NBD Guarma Ship 2",
        model: "ship_nbdGuama2",
    },
    ModelOption {
        label: "Skiff",
        model: "skiff",
    },
    ModelOption {
        label: "Tugboat 2",
        model: "TugBoat2",
    },
    ModelOption {
        label: "Utility Wagon",
        model: "utilliwag",
    },
    ModelOption {
        label: "Gatling Chuck Wagon",
        model: "gatchuck",
    },
    ModelOption {
        label: "Gatling Chuck Wagon 2",
        model: "gatchuck_2",
    },
];

const TELEPORTS: &[Location] = &[
    Location {
        label: "Saint Denis",
        x: 2700.1,
        y: -1403.39,
        z: 46.6373,
    },
    Location {
        label: "Van Horn",
        x: 2962.82,
        y: 583.162,
        z: 44.2948,
    },
    Location {
        label: "Annesburg",
        x: 2941.17,
        y: 1358.08,
        z: 44.0665,
    },
    Location {
        label: "Maclean's House",
        x: 2252.3,
        y: -135.647,
        z: 46.2262,
    },
    Location {
        label: "Hagen Orchards",
        x: 2069.06,
        y: -861.575,
        z: 42.4468,
    },
    Location {
        label: "Caliga Hall",
        x: 1797.61,
        y: -1352.64,
        z: 43.8385,
    },
    Location {
        label: "Braithwaite Manor",
        x: 889.869,
        y: -1910.26,
        z: 45.2703,
    },
    Location {
        label: "Emerald Ranch",
        x: 1424.09,
        y: 316.904,
        z: 88.6065,
    },
    Location {
        label: "Scarlett Meadows",
        x: 1211.79,
        y: -201.815,
        z: 101.436,
    },
    Location {
        label: "Alone Tree",
        x: 491.451,
        y: -307.623,
        z: 143.75,
    },
    Location {
        label: "Flatneck Station",
        x: -344.127,
        y: -363.465,
        z: 88.0377,
    },
    Location {
        label: "Wallace Station",
        x: -1308.38,
        y: 400.834,
        z: 95.3829,
    },
    Location {
        label: "Riggs Station",
        x: -1087.97,
        y: -585.786,
        z: 81.4831,
    },
    Location {
        label: "Valentine",
        x: -262.849,
        y: 793.404,
        z: 118.587,
    },
    Location {
        label: "Chadwick Farm",
        x: -391.389,
        y: 916.105,
        z: 117.644,
    },
    Location {
        label: "Fort Wallace",
        x: 366.483,
        y: 1456.73,
        z: 178.916,
    },
    Location {
        label: "Downes Ranch",
        x: -822.636,
        y: 325.741,
        z: 95.2731,
    },
    Location {
        label: "Strawberry",
        x: -1815.63,
        y: -396.749,
        z: 161.602,
    },
    Location {
        label: "Big Valley",
        x: -1819.9,
        y: -599.968,
        z: 154.616,
    },
    Location {
        label: "Holding Camp",
        x: -1588.19,
        y: -936.054,
        z: 84.1072,
    },
    Location {
        label: "Blackwater",
        x: -858.065,
        y: -1337.73,
        z: 44.4866,
    },
    Location {
        label: "Great Plains",
        x: -898.997,
        y: -1654.69,
        z: 68.5928,
    },
    Location {
        label: "Quaker's Cove",
        x: -1191.3,
        y: -1950.71,
        z: 43.5789,
    },
    Location {
        label: "Thieves Landing",
        x: -1452.17,
        y: -2329.4,
        z: 42.9603,
    },
];

const WEATHER: &[&str] = &[
    "OVERCAST",
    "RAIN",
    "FOG",
    "SNOWLIGHT",
    "THUNDER",
    "BLIZZARD",
    "SNOW",
    "MISTY",
    "SUNNY",
    "HIGHPRESSURE",
    "CLEARING",
    "SLEET",
    "DRIZZLE",
    "SHOWER",
    "SNOWCLEARING",
    "OVERCASTDARK",
    "THUNDERSTORM",
    "SANDSTORM",
    "HURRICANE",
    "HAIL",
    "WHITEOUT",
    "GROUNDBLIZZARD",
];
