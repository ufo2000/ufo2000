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

--------------------------------------------------------------------------------------
-- DATA
--------------------------------------------------------------------------------------

------------------------------------------------------------------------------
-- Add common images that are GUI related
-- (such as cursors, selectors etc.)
------------------------------------------------------------------------------
AddImage("mouse", png_image("$(ufo2000)/arts/mouse.png"))
AddImage("mouse_menu", png_image("$(ufo2000)/arts/mouse_menu.png"))
AddImage("selector", png_image_set("$(ufo2000)/arts/selector/19x9", 1, 4))
AddImage("button_left_arrow", png_image("$(ufo2000)/arts/button1.png"))
AddImage("button_right_arrow", png_image("$(ufo2000)/arts/button3.png"))

------------------------------------------------------------------------------
-- Add panels GUI
------------------------------------------------------------------------------
-- Main loading screen
------------------------------------------------------------------------------
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
------------------------------------------------------------------------------
-- Mainmenu
------------------------------------------------------------------------------
AddPanel {
    Name = "Main_menu",
    Background = png_image("$(ufo2000)/arts/main.jpg"),
    Features = {
        ButtonInternet = {
            Placement = {
                Top = {1383, 0},
                Left = {6750, 0},
                Width = {2813, 0},
                Height = {350, 0}
            },
            Font = "small",
            Color = {255,255,255},
            Color_active = {255,255,255},
            Bg_color = {0,0,0},
            Bg_color_active = {0,73,140},
            Background_active = png_image("$(ufo2000)/arts/menu_button_bg.png"),
        },
        ButtonHotseat = {
            Placement = {
                Top = {1883, 0},
                Left = {6750, 0},
                Width = {2813, 0},
                Height = {350, 0}
            },
            Font = "small",
            Color = {255,255,255},
            Color_active = {255,255,255},
            Bg_color = {0,0,0},
            Bg_color_active = {0,73,140},
            Background_active = png_image("$(ufo2000)/arts/menu_button_bg.png"),
        },
        ButtonLoadGame = {
            Placement = {
                Top = {2383, 0},
                Left = {6750, 0},
                Width = {2813, 0},
                Height = {350, 0}
            },
            Font = "small",
            Color = {255,255,255},
            Color_active = {255,255,255},
            Bg_color = {0,0,0},
            Bg_color_active = {0,73,140},
            Background_active = png_image("$(ufo2000)/arts/menu_button_bg.png"),
        },
        ButtonLoadReplay = {
            Placement = {
                Top = {2883, 0},
                Left = {6750, 0},
                Width = {2813, 0},
                Height = {350, 0}
            },
            Font = "small",
            Color = {255,255,255},
            Color_active = {255,255,255},
            Bg_color = {0,0,0},
            Bg_color_active = {0,73,140},
            Background_active = png_image("$(ufo2000)/arts/menu_button_bg.png"),
        },
        ButtonOptions = {
            Placement = {
                Top = {3900, 0},
                Left = {6750, 0},
                Width = {2813, 0},
                Height = {350, 0}
            },
            Font = "small",
            Color = {255,255,255},
            Color_active = {255,255,255},
            Bg_color = {0,0,0},
            Bg_color_active = {0,73,140},
            Background_active = png_image("$(ufo2000)/arts/menu_button_bg.png"),
        },
        ButtonTipOfDay = {
            Placement = {
                Top = {4400, 0},
                Left = {6750, 0},
                Width = {2813, 0},
                Height = {350, 0}
            },
            Font = "small",
            Color = {255,255,255},
            Color_active = {255,255,255},
            Bg_color = {0,0,0},
            Bg_color_active = {0,73,140},
            Background_active = png_image("$(ufo2000)/arts/menu_button_bg.png"),
        },
        ButtonGeoscape = {
            Placement = {
                Top = {4900, 0},
                Left = {6750, 0},
                Width = {2813, 0},
                Height = {350, 0}
            },
            Font = "small",
            Color = {255,255,255},
            Color_active = {255,255,255},
            Bg_color = {0,0,0},
            Bg_color_active = {0,73,140},
            Background_active = png_image("$(ufo2000)/arts/menu_button_bg.png"),
        },
        ButtonAbout = {
            Placement = {
                Top = {5833, 0},
                Left = {6750, 0},
                Width = {2813, 0},
                Height = {350, 0}
            },
            Font = "small",
            Color = {255,255,255},
            Color_active = {255,255,255},
            Bg_color = {0,0,0},
            Bg_color_active = {0,73,140},
            Background_active = png_image("$(ufo2000)/arts/menu_button_bg.png"),
        },
        ButtonQuit = {
            Placement = {
                Top = {6333, 0},
                Left = {6750, 0},
                Width = {2813, 0},
                Height = {350, 0}
            },
            Font = "small",
            Color = {255,255,255},
            Color_active = {255,255,255},
            Bg_color = {0,0,0},
            Bg_color_active = {0,73,140},
            Background_active = png_image("$(ufo2000)/arts/menu_button_bg.png"),
        },
        versionNumber = {
            Placement = {
                Top = {8350, 0},
                Left = {6750, 0},
                Width = {2813, 0},
                Height = {350, 0}
            },
            Font = "small",
            Color = {19,76,41},
            Bg_color = {0,0,0},
        },

    },
}