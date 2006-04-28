--------------------------------------------------------------------------------------
--
-- STANDARD-GUI.LUA
--
-- WARNING, SOME PROPERTIES AND PANELS ARE NOT SUPPORTED YET.
--------------------------------------------------------------------------------------
-- DESCRIPTION
--------------------------------------------------------------------------------------
--
-- LUA file that controls the whole appearance of UFO2000
--
--
-- The used panel names and their features are :
--
-- Game_init        Screen on which the game initializes.
--  Init_stream
--
-- Main_menu        The main menu right after the Game-init screen is done.
--
-- Lobby        The Network multiplayer lobby.
--
-- Loading      Screen shown when something is loading.
-- 
--
--------------------------------------------------------------------------------------
-- GENERAL USAGE
--------------------------------------------------------------------------------------
-- Positions are described as 1/10000th of the screen plus a pixel correction.
-- 10000ths are used because percents are not accurate enough ( 8x6 pixels on 800x600 )
-- This means that both precise relative and absolute positions are possible.
-- For example
-- {0,100} means position 0/10000th + 100 pixels
-- This is 100 pixels from top or left regardless of screen resolution.
-- {10000,0} means position 10000/10000th + 0 pixels
-- This is the lower or right boundary of the screen at any screen resolution
-- {10000,-10} means position 10000/10000th - 10 pixels
-- This is 10 pixels from the lower or right boundary
--------------------------------------------------------------------------------------
-- DETAILED USAGE
--------------------------------------------------------------------------------------
--
-- AddPanel {
--  Name = "String",                            -- Name of the screen (predefined)
--  Background = "bitmap file",                     -- Background image, transparent if not set
--  Features = {
--      String = {                                                      -- Name of the feature (predefined)                         
--      Placement = {
--          Top = {1/10000th of screen, pixel correction},      -- Position from the top, 0 if not set
--          Left = {1/10000th of screen, pixel correction},     -- Position from the left, 0 if not set
--          Bottom = {1/10000th of screen, pixel correction},   -- Position from the bottom, auto if not set
--          Right = {1/10000th of screen, pixel correction},    -- Position from the right, auto if not set
--          Width = {1/10000th of screen, pixel correction},    -- Width of feature, 10000 if not set
--          Height = {1/10000th of screen, pixel correction},   -- Height of feature, 10000 if not set
--          Padding = "pixels"                  -- Margin between feature boundaries and the text
--          },
--      Font = "keyword",                       -- "small", "normal", "big", small if not set
--      Color = "RGB color list",                   -- Color of the text, white if not set
--      Color_active = "RGB color list",                -- Color of the text on mouseover, white if not set
--      Bg_color = "RGB color list",                    -- Color of the background, transparent if not set
--      Bg_color_active = "RGB color list",             -- Color of the background on mouseover, transparent if not set
--      Bg_image = "bitmap file",                   -- background image file, transparent if not set
--      Bg_image_active = "bitmap file",                -- background image file on mouseover, transparent if not set
--      }
--  }
-- }
--
--------------------------------------------------------------------------------------
-- DATA
--------------------------------------------------------------------------------------

AddPanel {
    Name = "Game_init",
    Background = png_image("$(ufo2000)/arts/loading_screen.png"),
    Features = {
        InitStream = {
            Placement = {
                Top = {6667, 0},
                Left = {0, 0},
                Width = {10000, 0},
                Height = {2333, 0},
                Padding = {188, 0},
            },
            Font = "small",
            Color = {255,255,255},
        },
    },
}

AddPanel {
    Name = "Main_menu",
    Background = png_image("$(ufo2000)/arts/main.jpg"),
    Features = {
        ButtonInternet = {
            Placement = {
                Top = {6667, 8},
                Right = {0, 8},
                Width = {10000, -16},
                Height = {2333, -16}
            },
            Font = "small",
            Color = {255,255,255},
            Color_active = {255,255,255},
            Bg_color = {20,20,20},
            Bg_color_active = {10,10,10},
        },
        ButtonHotseat = {
            Placement = {
                Top = {6667, 8},
                Right = {0, 8},
                Width = {10000, -16},
                Height = {2333, -16}
            },
            Font = "small",
            Color = {255,255,255},
        },
    },
}