-- Data for Quick-Setup
-- v0.2  2004-11-14

AddSoldierSetup {
    Desc  = "Rifleman0 (A0, Laser)",
    Name  = "Rifleman #",
   --         TU STA  H   Br, R,  P1,P2, FA,TA, STR, F,App, Skin
    Attr  = { 80, 57, 52,  0, 50, 00,00, 80,51,  25, 0, 0, S_XCOM_0 },
    Items = { 
        {RHand, "LASER RIFLE"},
        {LHand, ""},
        {RShl,  "GRENADE"},
        {LShl,  "SMOKE GRENADE"},
        {RLeg,  ""},
        {LLeg,  ""},
        {Belt,  "HIGH EXPLOSIVE"},
        {BPack, "STUN ROD"},
    }
}

AddSoldierSetup {
    Desc  = "Rifleman1 (A1, PlasmaRifle)",
    Name  = "Rifleman #",
   --         TU STA  H   Br, R,  P1,P2, FA,TA, STR, F,App, Skin
    Attr  = { 80, 57, 52,  0, 50, 00,00, 80,51,  25, 0, 0, S_XCOM_1 },
    Items = { 
        {RHand, "PLASMA RIFLE","PLASMA RIFLE CLIP"},
        {LHand, ""},
        {RShl,  "SMOKE GRENADE"},
        {LShl,  "ALIEN GRENADE"},
        {RLeg,  "PLASMA RIFLE CLIP"},
        {LLeg,  ""},
        {Belt,  ""},
        {BPack, ""},
    }
}

AddSoldierSetup {
         -- "....+....1....+....2....+....3....+"
    Desc  = "Scout0 (A0, LaserPistol, Grenades)",
    Name  = "Scout #",
   --         TU STA  H   Br, R,  P1,P2, FA,TA, STR, F,App, Skin
    Attr  = { 80, 75, 51,  0, 50, 00,00, 62,52,  25, 1, 1, S_XCOM_1 },
    Items = { 
        {RHand, "Laser Pistol"},
        {LHand, "GRENADE"},
        {RShl,  "GRENADE", "GRENADE"},
        {LShl,  "SMOKE GRENADE", ""},
        {RLeg,  ""},
        {LLeg,  ""},
        {Belt,  "PROXIMITY GRENADE"},
        {BPack, ""},
    }
}

AddSoldierSetup {
         -- "....+....1....+....2....+....3....+"
    Desc  = "Scout1 (A1, Pl.Pistol, AlienGrenades)",
    Name  = "Scout #",
   --         TU STA  H   Br, R,  P1,P2, FA,TA, STR, F,App, Skin
    Attr  = { 80, 75, 51,  0, 50, 00,00, 62,52,  25, 1, 1, S_XCOM_1 },
    Items = { 
        {RHand, "Plasma Pistol", "Plasma Pistol Clip"},
        {LHand, "GRENADE"},
        {RShl,  "ALIEN GRENADE", "ALIEN GRENADE"},
        {LShl,  "SMOKE GRENADE", ""},
        {RLeg,  ""},
        {LLeg,  ""},
        {Belt,  "PROXIMITY GRENADE"},
        {BPack, ""},
    }
}

AddSoldierSetup {
    Desc  = "Marksman1 (A1, HeavyPlasma)",
    Name  = "Shooter #",
   --         TU STA  H   Br, R,  P1,P2, FA,TA, STR, F,App, Skin
    Attr  = { 80, 57, 52,  0, 50, 00,00, 80,51,  25, 0, 2, S_XCOM_1 },
    Items = { 
        {RHand, "Heavy Plasma", "Heavy Plasma Clip"},
        {LHand, ""},
        {RShl,  "ALIEN GRENADE"},
        {LShl,  "SMOKE GRENADE"},
        {RLeg,  "Heavy Plasma Clip"},
        {LLeg,  ""},
        {Belt,  ""},
        {BPack, ""},
    }
}

AddSoldierSetup {
    Desc  = "HeavyWeapon1 (A1, Autocannon)",
    Name  = "HeavyWeapon #",
   --         TU STA  H   Br, R,  P1,P2, FA,TA, STR, F,App, Skin
    Attr  = { 59, 51, 51,  0, 51, 00,00, 80,51,  39, 0, 3, S_XCOM_1 },
    Items = { 
        {RHand, "AUTO CANNON", "AUTO CANNON HE AMMO"},
        {LHand, ""},
        {RShl,  "AUTO CANNON HE AMMO"},
        {LShl,  "AUTO CANNON I AMMO"},
        {RLeg,  ""},
        {LLeg,  ""},
        {Belt,  "AUTO CANNON AP AMMO"},
        {BPack, ""},
    }
}

AddSoldierSetup {
    Desc  = "HeavyWeapon2 (A1, Rockets)",
    Name  = "Rocketman #",
   --         TU STA  H   Br, R,  P1,P2, FA,TA, STR, F,App, Skin
    Attr  = { 60, 51, 51,  0, 50, 00,00, 80,50,  40, 0, 3, S_XCOM_1 },
    Items = { 
        {RHand, "ROCKET_LAUNCHER", "LARGE_ROCKET"},
        {LHand, ""},
        {RShl,  ""},
        {LShl,  ""},
        {RLeg,  ""},
        {LLeg,  ""},
        {Belt,  ""},
        {BPack, "LARGE_ROCKET", "LARGE_ROCKET", "LARGE_ROCKET" },
    }
}

-- Todo: some Sectoid + Muton soldiers

AddSoldierSetup {
    Desc  = "(+ 80) Grenade, Smoke ",
    Name  = "",
    Attr  = {},
    Items = { 
        {RShl,  "GRENADE"},
        {LShl,  "SMOKE GRENADE"},
    }
}
AddSoldierSetup {
    Desc  = "(+100) ProxyGrenade, Smoke",
    Name  = "",
    Attr  = {},
    Items = { 
        {RShl,  "PROXIMITY_GRENADE"},
        {LShl,  "SMOKE GRENADE"},
    }
}
AddSoldierSetup {
    Desc  = "(+145) AlienGrenade, Smoke",
    Name  = "",
    Attr  = {},
    Items = { 
        {RShl,  "ALIEN GRENADE"},
        {LShl,  "SMOKE GRENADE"},
    }
}

