-- This file replaces standart-icons.lua since ufo2000 v0.4.1.458
-- 2004-09-13 new: ResTimeFree, ResTimeAim etc. for reserve-time-buttons
return {
	LeftItem = {
		Button            = { 8,  4, 39, 52},
		Image             = { 8, 10},
		DigitsCoords      = {33, 47},
-- 1, 36: COLOR_WHITE, COLOR_RED04
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

 -- Buttons for reserving time:
	ResTimeFree = { 53, 32,  78, 43},
	ResTimeAim  = { 79, 32, 105, 43},
	ResTimeSnap = { 53, 44,  78, 56},
	ResTimeAuto = { 79, 44, 105, 56},

	ManStats    = {106, 32, 134, 56},
	BarChart    = {135, 32, 268, 56},

	TurnNumber  = {
		Coords = {112, 41},
		Color  = 1,
		Font   = "normal"
	},

	ManName = {
		Coords = {134, 32},
-- 130: COLOR_BLUE02
		Color  = 130,
		Font   = "small"
	},

	TimeUnits = {
		BarZeroCoords = {169, 41},
		BarDirection  = "horizontal",
-- 64, 71: COLOR_OLIVE00, COLOR_OLIVE07
		FColor = 64,
		BColor = 71,
		DigitsCoords = {136, 42},
		DigitsColor  = 64
	},

	Energy = {
		BarZeroCoords = {169, 45},
		BarDirection  = "horizontal",
-- 16, 21: COLOR_GOLD, COLOR_ORANGE05
		FColor = 16,
		BColor = 21,
		DigitsCoords = {154, 42},
		DigitsColor  = 16
	},

	Health = {
		BarZeroCoords = {169, 49},
		BarDirection  = "horizontal",
-- 32, 37: COLOR_RED00, COLOR_RED05
		FColor = 32,
		BColor = 37,
		DigitsCoords = {136, 50},
		DigitsColor  = 32
	},

	Morale = {
		BarZeroCoords = {169, 53},
		BarDirection  = "horizontal",
-- 192, 197: COLOR_VIOLET00, COLOR_VIOLET05
		FColor = 192,
		BColor = 197,
		DigitsCoords = {154, 50},
		DigitsColor  = 192
	},

-- 80: COLOR_WHITE1
	StunColor = 80
}

