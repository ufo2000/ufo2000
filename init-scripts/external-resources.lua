------------------------------------------------------------------------------
-- A module responsible for processing data files in external formats
-- (binary resources from x-com for example) and supports exporting
-- them to native ufo2000 format
------------------------------------------------------------------------------

------------------------------------------------------------------------------
-- Reads mcd file from x-com and makes ufo2000 tileset from it,
-- returns result as a large string containing ufo2000 tileset
-- description
------------------------------------------------------------------------------
function convert_xcom_tileset(mcd_file_name, scang, loftemps)
    local result = {}
    local function write(...)
        for _, v in ipairs(arg) do
            table.insert(result, v)
        end
    end    

    local function basename(fname)
        local _, _, x = string.find(fname, "([^/\\]+)%.[^%./\\]+$")
        if x then return x end
        local _, _, x = string.find(fname, "([^/\\]+)$")
        if x then return x end
        return fname
    end
    
    local mcd_struct = [[
        unsigned char IsometricImage[8];
        unsigned char Shape[12];      
        unsigned char MinimapImage[2];
        unsigned char u23;
        unsigned char u24;
        unsigned char u25;
        unsigned char u26;
        unsigned char u27;
        unsigned char u28;
        unsigned char u29;
        unsigned char u30;
        unsigned char UFO_Door;
        unsigned char Stop_LOS;
        unsigned char No_Floor;
        unsigned char Big_Wall;      
        unsigned char Gravlift;
        unsigned char Door;
        unsigned char Block_Fire;
        unsigned char Block_Smoke;
        unsigned char u39;
        unsigned char TU_Walk;
        unsigned char TU_Fly;
        unsigned char TU_Slide;
        unsigned char Armour;
        unsigned char HE_Block;
        unsigned char Die_MCD;
        unsigned char Flammable;
        unsigned char Alt_MCD;       
        unsigned char u48;
        signed char T_Level;      
        unsigned char P_Level;
        unsigned char u51;
        unsigned char Light_Block;     
        unsigned char Footstep;         
        unsigned char Tile_Type;
        unsigned char HE_Type;         
        unsigned char HE_Strength;
        unsigned char Smoke_Blockage;      
        unsigned char Fuel;
        unsigned char Light_Source;
        unsigned char Target_Type;
        unsigned char u61;
        unsigned char u62;
    ]]
    
    -- parse C++ structure definition to extract information about every every property 
    -- offset and size
    
    local mcd_info = {}
    local mcd_size = 0
    local mcd_prop_names = {}
    
    string.gsub(mcd_struct, "char%s+([%a_][%a%d_]*)%[?(%d*)%]?%;%s*[\r\n/$]",
        function(prop_name, prop_size)
            if prop_size == "" then prop_size = 1 else prop_size = tonumber(prop_size) end
            mcd_info[prop_name] = { mcd_size, prop_size }
            mcd_size = mcd_size + prop_size
            table.insert(mcd_prop_names, prop_name)
        end)
    
    local function read_mcd(fh)
        local tbl = {}
        local data = fh:read(mcd_size)
        if not data then return nil end
        for k, v in mcd_info do
            local prop_data = string.sub(data, 1 + v[1], v[1] + v[2])
            if k == "name" then
                tbl[k] = string.gsub(prop_data, "%z", "")
            elseif string.len(prop_data) == 1 then
                tbl[k] = string.byte(prop_data)
            else
                tbl[k] = {}
                for i = 1, string.len(prop_data) do 
                    table.insert(tbl[k], string.byte(prop_data, i)) 
                end
            end
        end
    
        return tbl
    end

    local function png_name(num)
        return basename(mcd_file_name) .. "/32x40-" .. string.format("%03d", num) .. ".png"
    end

    local function scang_name(num)
        local suffix = "xcom"
        if tftd_flag then suffix = "tftd" end
        return "scang-" .. suffix .. "/4x4-" .. string.format("%03d", num) .. ".png"
    end

    local function process_property(name, value)
        if string.find(name, "^u%d+") then return end
        if name == "Shape" then
			write("            Shape = [[\n")
			local shape_data = ""
			for k = 12, 1, -1 do
				v = value[k]
				shape_data = shape_data .. "                "
				for i = 15, 0, -1 do
					local x = string.byte(loftemps, v * 16 * 2 + i * 2 + 1) + 
					      string.byte(loftemps, v * 16 * 2 + i * 2 + 2) * 256
					shape_data = shape_data .. string.format("%04X,", x)
				end
				shape_data = shape_data .. "\n"
			end
			write(shape_data)
			write("            ]],\n")
			return
		end
        if name == "MinimapImage" then
            write("            MinimapImage = png_image(\"", scang_name(value[1] + value[2] * 256 + 1), "\"),\n")
            return
        end
        if name == "IsometricImage" then
            local num = value[1]
            for i = 2, 8 do
                if value[i] ~= num then
                    write("            IsometricImage = {\n")
                    for i = 1, 8 do
                        write("                png_image(\"", png_name(value[i] + 1), "\"),\n")
                    end
                    write("            },\n")
                    return
                end
            end
            write("            IsometricImage = png_image(\"", png_name(num + 1), "\"),\n")
            return
        end
        if string.find(name, "MCD$") then if value == 0 then return else return value + 1 end end
        if name == "T_Level" and value > 127 then value = value - 256 end
        return value
    end
    
    fh = io.open(mcd_file_name, "rb")
    local index = 1
    write("-- <pre>\n")
    write("AddTileset {\n")
    write("    Name = \"", basename(mcd_file_name), "\",\n")
    write("    Tiles = {\n")
    while 1 do
        local tbl = read_mcd(fh)
        if not tbl then break end
        write("        [", index, "] = { ")
        write("-- </pre><table>")
        for _, num in tbl.IsometricImage do
            write("<tr width='70'><img src='",
                basename(mcd_file_name), "/32x40-", string.format("%03d", num + 1), ".png' width='64' height='80'>")
        end
        write("</table><pre>\n")
        for i, k in ipairs(mcd_prop_names) do
            local v = process_property(k, tbl[k])
            if v then
                write("            ", k, " = ")
                if type(v) == "table" then
                    write("{")
                    for i = 1, table.getn(v) do
                        write(v[i])
                        if i ~= table.getn(v) then write(", ") end
                    end
                    write("},\n")
                elseif type(v) == "string" then
                    write("\"", v, "\",\n")
                else
                    write(v, ",\n")
                end
            end
        end
        write("        },\n")
        index = index + 1
    end
    write("    }\n")
    write("}\n")
    write("-- </pre>")
    fh:close()
    return table.concat(result)
end
