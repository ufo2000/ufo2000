------------------------------------------------------------------------------
-- Initialize items
------------------------------------------------------------------------------

AddXcomItem {
    cost = 0,
    name = "CORPSE",
    pInv = pck_image("$(xcom)/units/bigobs.pck", 45) or 
        png_image("$(ufo2000)/extensions/unit-chameleon/soldier_corpse.png"),
    pMap = pck_image("$(xcom)/units/floorob.pck", 39) or
        png_image("$(ufo2000)/extensions/unit-chameleon/soldier_corpse_f.png"),
    health = 20,
    importance = 20,
    width = 2,
    height = 3,
    pHeld = pck_image_set("$(xcom)/units/handob.pck", 120, 8) or
        png_image_set("$(ufo2000)/arts/empty_h/32x40", 1, 8),
    weight = 22,
    minimapMark = 1,
}

AddXcomItem {
    cost = 0,
    name = "CORPSE & ARMOUR",
    pInv = pck_image("$(xcom)/units/bigobs.pck", 28) or 
        png_image("$(ufo2000)/extensions/unit-chameleon/soldier_corpse.png"),
    pMap = pck_image("$(xcom)/units/floorob.pck", 40) or
        png_image("$(ufo2000)/extensions/unit-chameleon/soldier_corpse_f.png"),
    health = 30,
    importance = 22,
    width = 2,
    height = 3,
    pHeld = pck_image_set("$(xcom)/units/handob.pck", 120, 8) or
        png_image_set("$(ufo2000)/arts/empty_h/32x40", 1, 8),
    weight = 24,
    minimapMark = 1,
}

AddXcomItem {
    cost = 0,
    name = "CORPSE & POWER SUIT",
    pInv = pck_image("$(xcom)/units/bigobs.pck", 44) or
        png_image("$(ufo2000)/extensions/unit-chameleon/soldier_corpse.png"),
    pMap = pck_image("$(xcom)/units/floorob.pck", 41) or
        png_image("$(ufo2000)/extensions/unit-chameleon/soldier_corpse_f.png"),
    health = 50,
    importance = 24,
    width = 2,
    height = 3,
    pHeld = pck_image_set("$(xcom)/units/handob.pck", 120, 8) or
        png_image_set("$(ufo2000)/arts/empty_h/32x40", 1, 8),
    weight = 26,
    minimapMark = 1,
}


AddXcomItem {
    cost = 0,
    name = "Sectoid Corpse",
    pInv = pck_image("$(xcom)/units/bigobs.pck", 46) or
        png_image("$(ufo2000)/extensions/unit-chameleon/soldier_corpse.png"),
    pMap = pck_image("$(xcom)/units/floorob.pck", 42) or
        png_image("$(ufo2000)/extensions/unit-chameleon/soldier_corpse_f.png"),
    health = 5,
    importance = 30,
    width = 2,
    height = 3,
    pHeld = pck_image_set("$(xcom)/units/handob.pck", 0, 8) or
        png_image_set("$(ufo2000)/arts/empty_h/32x40", 1, 8),
    weight = 30,
    minimapMark = 1,
}

AddXcomItem {
    cost = 0,
    name = "Muton Corpse",
    pInv = pck_image("$(xcom)/units/bigobs.pck", 48) or
        png_image("$(ufo2000)/extensions/unit-chameleon/soldier_corpse.png"),
    pMap = pck_image("$(xcom)/units/floorob.pck", 44) or
        png_image("$(ufo2000)/extensions/unit-chameleon/soldier_corpse_f.png"),
    health = 40,
    importance = 40,
    width = 2,
    height = 3,
    pHeld = pck_image_set("$(xcom)/units/handob.pck", 0, 8) or
        png_image_set("$(ufo2000)/arts/empty_h/32x40", 1, 8),
    weight = 40,
    minimapMark = 1,
}
