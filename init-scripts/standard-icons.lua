-- Last update: v0.4.1.471
-- 2004-10-10 new: ResTime* buttons now contain border-color and border-coords (border for selected state);
--                 comments about used colors are moved to the top of the file
-- 2004-09-13 new: ResTimeFree, ResTimeAim etc. for reserve-time-buttons

--      COLORS USED    --
-------------------------
--   1: COLOR_WHITE    --
--  16: COLOR_GOLD     --
--  21: COLOR_ORANGE05 --
--  32: COLOR_RED00    --
--  36: COLOR_RED04    --
--  37: COLOR_RED05    --
--  64: COLOR_OLIVE00  --
--  71: COLOR_OLIVE07  --
--  80: COLOR_WHITE1   --
-- 130: COLOR_BLUE02   --
-- 192: COLOR_VIOLET00 --
-- 197: COLOR_VIOLET05 --
-------------------------

return {
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

	ManUp       = { 52,  0,  82, 15},
	ManDown     = { 52, 16,  82, 31},

	ViewUp      = { 83,  0, 113, 15},
	ViewDown    = { 83, 16, 113, 31},

	Map         = {114,  0, 144, 15},
	Crouch      = {114, 16, 144, 31},

	Inventory   = {145,  0, 175, 15},
	CenterView  = {145, 16, 175, 31},

	NextMan     = {176,  0, 206, 15},
	NextMan2    = {176, 16, 206, 31},

	ToggleRoof  = {207,  0, 237, 15},
	Options     = {207, 16, 237, 31},

	Done        = {238,  0, 268, 15},
	Exit        = {238, 16, 268, 31},

	ManStats    = {106, 32, 134, 56},
	BarChart    = {135, 32, 268, 56},

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


	ResTimeAim = { 
		Button       = {78, 33, 104, 43},
		BorderCoords = {77, 32, 105, 44},
		BorderColor  = 1
	},


	ResTimeSnap = { 
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

