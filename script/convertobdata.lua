#!/usr/bin/lua

-- A small script which can convert OBDATA.DAT binary file into lua script

if arg[1] == nil then
	print("Usage: convertobdata.lua [x-com OBDATA.DAT file]")
	os.exit()
end

local obdata_struct = [[
	char          name[20];       //The name of the object; used internally only
	unsigned char pInv;           //The image on the inventory screen
	unsigned char pMap;           //floorob//The image on the isometric map screen
	unsigned char damage;         //The damage value
	unsigned char importance;     //In a stack of items, the item with the highest importance is shown
	unsigned char width;          //The width on the inventory
	unsigned char height;         //The height on the inventory
	unsigned char ammo[3];        //The three item numbers that can be ammo, 0xFF=unused
	unsigned char pHeld;          //handob	//The picture to use on the map while it's being held
	unsigned char _1E;            //Changing this seems to have no effect...
	unsigned char damageType;     //The type of attack; refer to DT_nnnn
	unsigned char accuracy[3];    //The accuracy for each of the three kinds of shots; 0=can't take this type of shot.  [0]=Auto  [1]=Snap  [2]=Aimed
	unsigned char time[3];        //The TU% for each of the three kinds of shots.  0=can't take this type of shot.  [0]=Auto  [1]=Snap  [2]=Aimed
	unsigned char rounds;         //The number of rounds in the clip
	unsigned char hitType;        //The type of damage that hitting with it does.  Only matters on stun rods/thermal tazers.  Refer to DT_nnnn
	unsigned char throw_accuracy; //Always 0x64
	unsigned char _29;            //Always 0x32
	unsigned char _2A;
	unsigned char weight;         //The weight of the object
	unsigned char primeType;      //The way in which the object can be primed.  See PT_nnnn
	unsigned char _2D;
	unsigned char isShootable;    //This item can be shot. (0=no,1=yes)
	unsigned char isWeapon;       //This item is a weapon. (0=no,1=yes)
	unsigned char isGun;          //This is a gun which requires ammo. (0=no,1=yes)
	unsigned char isAmmo;         //This item is a clip. (0=no,1=yes)
	unsigned char twoHanded;      //Is more accurate with two hands (0=no,1=yes)
	unsigned char wayPoints;      //Has a 'launch missile' option  (0=no,1=yes)
	unsigned char isAlien;        //Scott T. Jones has this marked as 'Research'.  This makes sense, but I can find no evidence to support it...
	unsigned char _35;            //Scott T. Jones has this marked as 'Points'...
]]

-- parse C++ structure definition to extract information about every every property 
-- offset and size

local obdata_info = {}
local obdata_size = 0
local obdata_prop_names = {}

string.gsub(obdata_struct, "char%s+([%a_][%a%d_]*)%[?(%d*)%]?%;%s*[\r\n/$]",
	function(prop_name, prop_size)
		if prop_size == "" then prop_size = 1 else prop_size = tonumber(prop_size) end
		obdata_info[prop_name] = { obdata_size, prop_size }
		obdata_size = obdata_size + prop_size
		table.insert(obdata_prop_names, prop_name)
	end)

function read_obdata(fh)
	local tbl = {}
	local data = fh:read(obdata_size)
	if not data then return nil end
	for k, v in obdata_info do
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

fh = io.open(arg[1], "rb")
local index = 0
while 1 do
	local tbl = read_obdata(fh)
	if not tbl then break end

	io.write("AddXcomItem {\n")
	io.write("\tindex = ", index, ",\n")
	io.write("\tcost = ", tbl["damage"], ",\n")
	for i, k in ipairs(obdata_prop_names) do
		local v = tbl[k]
	
		io.write("\t", k, " = ")
		if type(v) == "table" then
			io.write("{")
			for i = 1, table.getn(v) do
				io.write(v[i])
				if i ~= table.getn(v) then io.write(", ") end
			end
			io.write("},\n")
		elseif type(v) == "string" then
			io.write("\"", v, "\",\n")
		else
			io.write(v, ",\n")
		end
	end
	io.write("}\n\n")
	
	index = index + 1
end
