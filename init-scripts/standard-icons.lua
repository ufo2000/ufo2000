-- Last update: v0.7.864
-- 2005-05-18 Adapted for new control panel
-- 2005-02-20 new: "Transparency", "HighlightFile"
-- 2004-11-05 Swapped ResTimeAim and ResTimeSnap, tweaked coordinates
-- 2004-10-10 new: ResTime* buttons now contain border-color and border-coords (border for selected state);
--                 comments about used colors are moved to the top of the file
-- 2004-09-13 new: ResTimeFree, ResTimeAim etc. for reserve-time-buttons

--      COLORS USED    --
-- For a complete list of colors, refer to x-com battle palette the palette index is the number used for that particular color --
--------------------------
--   1: COLOR_WHITE     --
--  16: COLOR_GOLD      --
--  21: COLOR_ORANGE05  --
--  32: COLOR_RED00     --
--  36: COLOR_RED04     --
--  37: COLOR_RED05     --
--  64: COLOR_OLIVE00   --
--  71: COLOR_OLIVE07   --
--  80: COLOR_WHITE1    --
-- 130: COLOR_BLUE02    --
-- 192: COLOR_VIOLET00  --
-- 197: COLOR_VIOLET05  --
-- 211: COLOR_SKYBLUE03 --
--------------------------

local new_control_panel = {
    Image = {
        File = "$(ufo2000)/arts/ct_panel_normal.bmp",
        HighlightFile = "$(ufo2000)/arts/ct_panel_over.bmp",
        Transparency = 0
    },

    LeftItem = {
        Button            = { 20, 28, 51, 75},
        Image             = { 20, 34},
        DigitsCoords      = {9, 71},
        DigitsRoundsColor = 211,
        DigitsPrimeColor  = 211
    },

    RightItem = {
        Button            = {336,  28, 367, 75},
        Image             = {336, 34},
        DigitsCoords      = {371, 71},
        DigitsRoundsColor = 211,
        DigitsPrimeColor  = 211
    },

    NextMan2    = {147,  1, 176, 15},
    NextMan     = {178,  1, 210, 15},

    Done        = {313,  1, 350, 15},
    Options     = {352,  1, 368, 15},
    Exit        = {370,  1, 386, 15},

    ManUp       = {130, 20, 171, 34},
    Crouch      = {173, 22, 214, 34},
    ManDown     = {216, 20, 257, 34},

    CenterView  = {263, 20, 288, 48},
    Map         = {263, 50, 287, 78},

    ViewUp      = {292, 20, 324, 38},
    ToggleRoof  = {290, 40, 323, 58},
    ViewDown    = {288, 60, 322, 78},

    Inventory   = { 99, 20, 124, 48},
    ManStats    = {100, 50, 124, 78},

    BarChart    = {131, 39, 256, 77},

    TurnNumber  = {
        Coords = {292, 5},
        Color  = 211,
        Font   = "normal"
    },

    ManName = {
        Coords = {5, 5},
        Color  = 211,
        Font   = "small"
    },

    TimeUnits = {
        BarZeroCoords = {152, 47},
        BarDirection  = "horizontal",
        FColor = 64,
        BColor = 71,
        DigitsCoords = {137, 46},
        DigitsColor  = 64
    },

    Energy = {
        BarZeroCoords = {152, 54},
        BarDirection  = "horizontal",
        FColor = 16,
        BColor = 21,
        DigitsCoords = {137, 53},
        DigitsColor  = 16
    },

    Health = {
        BarZeroCoords = {152, 61},
        BarDirection  = "horizontal",
        FColor = 32,
        BColor = 37,
        DigitsCoords = {137, 60},
        DigitsColor  = 32
    },

    Morale = {
        BarZeroCoords = {152, 68},
        BarDirection  = "horizontal",
        FColor = 192,
        BColor = 197,
        DigitsCoords = {137, 67},
        DigitsColor  = 192
    },

    StunColor = 80,

    ResTimeFree = { 
        Button       = { 63,  20,  95,  33},
        BorderCoords = { 64,  31,  94,  32},
        BorderColor  = 211
    },

    ResTimeSnap = { 
        Button       = { 63,  35,  96,  48},
        BorderCoords = { 65,  46,  95,  47},
        BorderColor  = 211
    }, 

    ResTimeAim = { 
        Button       = { 64,  50,  97,  63},
        BorderCoords = { 67,  61,  96,  62},
        BorderColor  = 211
    },

    ResTimeAuto = { 
        Button       = { 66,  65, 99,  78},
        BorderCoords = { 71,  76, 98,  77},
        BorderColor  = 211
    }
}

local old_control_panel = {

    LeftItem = {
        Button            = { 8,  4, 39, 52},
        Image             = { 8, 10},
        DigitsCoords      = {33, 47},
        DigitsRoundsColor = 1,
        DigitsPrimeColor  = 36
    },

    RightItem = {
        Button            = {280,  4, 311, 52},
        Image             = {280, 10},
        DigitsCoords      = {304, 47},
        DigitsRoundsColor = 1,
        DigitsPrimeColor  = 36
    },

    ManUp       = { 49,  0,  78, 15},
    ManDown     = { 49, 16,  78, 31},

    ViewUp      = { 81,  0, 110, 15},
    ViewDown    = { 81, 16, 110, 31},

    Map         = {113,  0, 142, 15},
    Crouch      = {113, 16, 142, 31},

    Inventory   = {145,  0, 174, 15},
    CenterView  = {145, 16, 174, 31},

    NextMan     = {177,  0, 206, 15},
    NextMan2    = {177, 16, 206, 31},

    ToggleRoof  = {209,  0, 238, 15},
    Options     = {209, 16, 238, 31},

    Done        = {241,  0, 270, 15},
    Exit        = {241, 16, 270, 31},

    ManStats    = {106, 32, 132, 56},
    BarChart    = {134, 32, 270, 56},

    TurnNumber  = {
        Coords = {112, 41},
        Color  = 1,
        Font   = "normal"
    },

    ManName = {
        Coords = {134, 32},
        Color  = 130,
        Font   = "small"
    },

    TimeUnits = {
        BarZeroCoords = {169, 41},
        BarDirection  = "horizontal",
        FColor = 64,
        BColor = 71,
        DigitsCoords = {136, 42},
        DigitsColor  = 64
    },

    Energy = {
        BarZeroCoords = {169, 45},
        BarDirection  = "horizontal",
        FColor = 16,
        BColor = 21,
        DigitsCoords = {154, 42},
        DigitsColor  = 16
    },

    Health = {
        BarZeroCoords = {169, 49},
        BarDirection  = "horizontal",
        FColor = 32,
        BColor = 37,
        DigitsCoords = {136, 50},
        DigitsColor  = 32
    },

    Morale = {
        BarZeroCoords = {169, 53},
        BarDirection  = "horizontal",
        FColor = 192,
        BColor = 197,
        DigitsCoords = {154, 50},
        DigitsColor  = 192
    },

    StunColor = 80,

    ResTimeFree = { 
        Button       = {49, 33,  76, 43},
        BorderCoords = {48, 32,  77, 44},
        BorderColor  = 1
    },

    ResTimeSnap = { 
        Button       = {78, 33, 104, 43},
        BorderCoords = {77, 32, 105, 44},
        BorderColor  = 1
    }, 

    ResTimeAim = { 
        Button       = {49, 45,  76, 55},
        BorderCoords = {48, 44,  77, 56},
        BorderColor  = 1
    },

    ResTimeAuto = { 
        Button       = {78, 45, 104, 55},
        BorderCoords = {77, 44, 105, 56},
        BorderColor  = 1
    }
}

if prefer_xcom_gfx and ReadFile(LocateFile("$(xcom)/ufograph/icons.pck")) then
    return old_control_panel
else
    return new_control_panel
end
