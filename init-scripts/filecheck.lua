local function X(a) return tonumber(a, 16) end

------------------------------------------------------------------------------
-- Table which contains the list of the files needed by the game 
-- (that means they must at least exist and at most should pass crc check)
--
-- The fields of this table look like this:
-- [a prefixed name of the] = { additional information about the file }
--
-- Additional information:
--    Crc32    - the file is checked by the game (Note: zero value - a special 
--               case, only the file existence is checked but not crc32)
--    Origin   - The file can be modified by the game and is initialized 
--               from the file specified by 'Origin' value
--    Fallback - A special field that can specify a replacement file 
--               that is used if the original file is bad or missing
--
-- Note: when file information does not contain Crc32 field, it is considered 
--       modifiable and write permission availability is checked for it
------------------------------------------------------------------------------

local FilesTable = {
    ["$(ufo2000)/ufo2000.dat"]    = { Crc32 = X("0x29641bee") }, -- Rev.1068, added research palette
    ["$(ufo2000)/keyboard.dat"]   = { Crc32 = X("0xC796755E") },

    ["$(home)/squad.lua"]             = { Origin = "$(ufo2000)/squad.default.lua" },
    ["$(home)/ufo2000.ini"]           = { Origin = "$(ufo2000)/ufo2000.default.ini" },
    ["$(home)/init-scripts.log"]      = { },
    ["$(home)/ufo2000.log"]           = { },
    ["$(home)/ufo2000.tmp"]           = { },
    ["$(home)/ufo2000.sav"]           = { },
    ["$(home)/cur_map.lua"]           = { },
    ["$(home)/cur_p1.dat"]            = { },
    ["$(home)/cur_p2.dat"]            = { },

    ["$(ufo2000)/init-scripts/main.lua"]               = { Crc32 = X("0x00000000") },
    ["$(ufo2000)/init-scripts/standard-maps.lua"]      = { Crc32 = X("0x00000000") },
    ["$(ufo2000)/init-scripts/standard-items.lua"]     = { Crc32 = X("0x00000000") },
    ["$(ufo2000)/init-scripts/standard-icons.lua"]     = { Crc32 = X("0x00000000") },
    ["$(ufo2000)/init-scripts/standard-gui.lua"]     = { Crc32 = X("0x00000000") },
    
    ["$(ufo2000)/arts/menu.jpg"]      = { Crc32 = X("0x00000000") },
    ["$(ufo2000)/arts/empty.spk"]     = { Crc32 = X("0x00000000") },

    ["$(ufo2000)/newunits/muton.spk"] = { Crc32 = X("0x00000000"),
        Fallback = "$(ufo2000)/arts/empty.spk" },
    ["$(ufo2000)/newunits/sectoid.spk"] = { Crc32 = X("0x00000000"),
        Fallback = "$(ufo2000)/arts/empty.spk" },

    ["$(ufo2000)/newunits/add1.pck"] = { Crc32 = X("0x00000000") },
    ["$(ufo2000)/newunits/add1.tab"] = { Crc32 = X("0x00000000") },

    ["$(ufo2000)/newmusic/ATeX_-_BattleScape_Test_v1.0_-_VBR128.ogg"] = { Crc32 = X("0x00000000"),
        Fallback = "$(xcom)/sound/gmtactic.mid" },

    ["$(xcom)/ufointro/pict4.lbm"] = { Crc32 = X("0x8178ADD1"),
        Fallback = "$(ufo2000)/arts/text_back.jpg" },
    ["$(xcom)/ufointro/pict5.lbm"] = { Crc32 = X("0xA8543964"),
        Fallback = "$(ufo2000)/arts/text_back.jpg" },

    ["$(xcom)/geograph/back01.scr"] = { Crc32 = X("0xD071E645"),
        Fallback = "$(ufo2000)/arts/empty.spk" },
    ["$(xcom)/geograph/back02.scr"] = { Crc32 = X("0xCA501299"),
        Fallback = "$(ufo2000)/arts/empty.spk" },
    ["$(xcom)/geograph/back03.scr"] = { Crc32 = X("0x8098231A"),
        Fallback = "$(ufo2000)/arts/empty.spk" },
    ["$(xcom)/geograph/back09.scr"] = { Crc32 = X("0xF7A94FD1"),
        Fallback = "$(ufo2000)/arts/empty.spk" },
    ["$(xcom)/geograph/back14.scr"] = { Crc32 = X("0x7AF63F1D"),
        Fallback = "$(ufo2000)/arts/empty.spk" },

    ["$(xcom)/ufograph/man_0f0.spk"] = { Crc32 = X("0x9B75B5F8"),
        Fallback = "$(xcom)/ufograph/man_1f0.spk" },
    ["$(xcom)/ufograph/man_0f1.spk"] = { Crc32 = X("0xB199957A"),
        Fallback = "$(xcom)/ufograph/man_1f1.spk" },
    ["$(xcom)/ufograph/man_0f2.spk"] = { Crc32 = X("0x8ED17367"),
        Fallback = "$(xcom)/ufograph/man_1f2.spk" },
    ["$(xcom)/ufograph/man_0f3.spk"] = { Crc32 = X("0x740442DF"),
        Fallback = "$(xcom)/ufograph/man_1f3.spk" },
    ["$(xcom)/ufograph/man_0m0.spk"] = { Crc32 = X("0x18B23EF7"),
        Fallback = "$(xcom)/ufograph/man_1m0.spk" },
    ["$(xcom)/ufograph/man_0m1.spk"] = { Crc32 = X("0x1B90E3B2"),
        Fallback = "$(xcom)/ufograph/man_1m1.spk" },
    ["$(xcom)/ufograph/man_0m2.spk"] = { Crc32 = X("0x17F8E53B"),
        Fallback = "$(xcom)/ufograph/man_1m2.spk" },
    ["$(xcom)/ufograph/man_0m3.spk"] = { Crc32 = X("0xBC57FC2E"),
        Fallback = "$(xcom)/ufograph/man_1m3.spk" },

    ["$(xcom)/ufograph/man_1f0.spk"]  = { Crc32 = X("0xF87390C0"),
        Fallback = "$(ufo2000)/arts/empty.spk" },
    ["$(xcom)/ufograph/man_1f1.spk"]  = { Crc32 = X("0x02CB471C"),
        Fallback = "$(ufo2000)/arts/empty.spk" },
    ["$(xcom)/ufograph/man_1f2.spk"]  = { Crc32 = X("0x7E91A5E0"),
        Fallback = "$(ufo2000)/arts/empty.spk" },
    ["$(xcom)/ufograph/man_1f3.spk"]  = { Crc32 = X("0xF050BD04"),
        Fallback = "$(ufo2000)/arts/empty.spk" },
    ["$(xcom)/ufograph/man_1m0.spk"]  = { Crc32 = X("0xD919B6A5"),
        Fallback = "$(ufo2000)/arts/empty.spk" },
    ["$(xcom)/ufograph/man_1m1.spk"]  = { Crc32 = X("0x3AD7592B"),
        Fallback = "$(ufo2000)/arts/empty.spk" },
    ["$(xcom)/ufograph/man_1m2.spk"]  = { Crc32 = X("0x2A0840E1"),
        Fallback = "$(ufo2000)/arts/empty.spk" },
    ["$(xcom)/ufograph/man_1m3.spk"]  = { Crc32 = X("0x47D7ACCB"),
        Fallback = "$(ufo2000)/arts/empty.spk" },

    ["$(xcom)/ufograph/man_2.spk"]    = { Crc32 = X("0x890C3076"),
        Fallback = "$(ufo2000)/arts/empty.spk" },
    ["$(xcom)/ufograph/man_3.spk"]    = { Crc32 = X("0xA8BB9C04"),
        Fallback = "$(ufo2000)/arts/empty.spk" },

    ["$(xcom)/ufograph/unibord.pck"]  = { Crc32 = X("0x94576591"),
        Fallback = "$(ufo2000)/arts/empty.spk" },
    ["$(xcom)/ufograph/scanbord.pck"] = { Crc32 = X("0xE3F32D10"),
        Fallback = "$(ufo2000)/arts/empty.spk" },

    ["$(xcom)/units/xcom_0.pck"] = { Crc32 = X("0x8681EF39"), 
        Fallback = "$(xcom)/units/xcom_1.pck" },
    ["$(xcom)/units/xcom_0.tab"] = { Crc32 = X("0x68930DB8"), 
        Fallback = "$(xcom)/units/xcom_1.tab" },

    ["$(xcom)/units/muton.pck"] = { Crc32 = X("0x00000000"), 
        Fallback = "$(xcom)/units/xcom_1.pck" },
    ["$(xcom)/units/muton.tab"] = { Crc32 = X("0x00000000"), 
        Fallback = "$(xcom)/units/xcom_1.tab" },
    ["$(xcom)/units/sectoid.pck"] = { Crc32 = X("0x00000000"), 
        Fallback = "$(xcom)/units/xcom_1.pck" },
    ["$(xcom)/units/sectoid.tab"] = { Crc32 = X("0x00000000"), 
        Fallback = "$(xcom)/units/xcom_1.tab" },

    ["$(xcom)/ufograph/tac00.scr"]   = { Crc32 = X("0x226E61A0"),
        Fallback = "$(ufo2000)/arts/empty.spk" },
    ["$(xcom)/ufograph/tac01.scr"]   = { Crc32 = X("0x00000000"),
        Fallback = "$(ufo2000)/arts/empty.spk" },
}

------------------------------------------------------------------------------
-- Supplementary local function that performs the most of the work needed 
-- by CheckDataFiles function
------------------------------------------------------------------------------

local function CheckSingleDataFile(name, info)
    if info.Crc32 then
    -- read only data file
        local data = ReadFile(name)
        if data == nil then
            return string.format("CheckDataFiles: can't open file '%s'", name)
        end
        local Crc32 = UpdateCrc32(0, data)
        if info.Crc32 ~= Crc32 and info.Crc32 ~= 0 then
            return string.format("CheckDataFiles: crc32 error for file '%s' (0x%x / 0x%x)", name, info.Crc32, Crc32)
        end
    else
    -- read/write data file
        local fh = io.open(name, "r+b")
        if fh == nil then
            fh = io.open(name, "wb")
            if fh == nil then
                return string.format("CheckDataFiles: can't open '%s' for read/write", name)
            end

            if info.Origin then
                local data = ReadFile(LocateFile(info.Origin))
                if data == nil then
                    return string.format(
                        "CheckDataFiles: can't get original file '%s' for initialization of '%s'", 
                        info.Origin, name) 
                end
                fh:write(data)
                fh:close()
            else
                fh:close()
                os.remove(name)
            end
        else
            fh:close()
        end
    end
    return nil
end

------------------------------------------------------------------------------
-- The function which performs crc32 or availability check for all files 
-- from FilesTable.
------------------------------------------------------------------------------

local function CheckDataFiles()
    for key, info in FilesTable do
        local name = LocateFile(key)
        local errmsg = CheckSingleDataFile(name, info)

        local _key, _info = key, info

        while errmsg and _info.Fallback do
            errmsg = nil
            _key, _info = _info.Fallback, FilesTable[_info.Fallback]
            name = LocateFile(_key)
            if not _info then break end
            errmsg = CheckSingleDataFile(name, _info)
        end

        if errmsg then
            Error(
                errmsg ..
                "\n\n" ..
                "This problem may be caused by incorrect installation " ..
                "of the game (you probably forgot to copy original X-COM data files to the " ..
                "place where the game can locate them)." ..
                "\n\n" ..
                "Please make sure that you followed the instructions from 'INSTALL' " ..
                "file before reporting bugs.") 
        end

        info.FileName = name
    end
end

-- Perform data files integrity check
CheckDataFiles()

------------------------------------------------------------------------------
-- Function which is used from C++ code to perform filename prefixes 
-- expansion. It also handles fallbacks - ability to return some replacement
-- file instead of the missing one (there are lots of files missing in X-COM
-- demo that need to be handled this way)
------------------------------------------------------------------------------

function GetDataFileName(x)
    if FilesTable[x] then return FilesTable[x].FileName end
    return LocateFile(x)
end
