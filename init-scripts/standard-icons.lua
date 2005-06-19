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

return {
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

