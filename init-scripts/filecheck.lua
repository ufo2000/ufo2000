local function X(a) return tonumber(a, 16) end

FilesTable = {
	["$(ufo2000)/ufo2000.dat"]  = { Crc32 = X("0x8EA4C104") },
	["$(ufo2000)/keyboard.dat"] = { Crc32 = X("0xC796755E") },
	["$(ufo2000)/soundmap.xml"] = { Crc32 = X("0x00000000") },

	["$(home)/geodata.dat"] = {
		Origin = "$(ufo2000)/geodata.dat" },
	["$(home)/soldier.dat"] = {
		Origin = "$(ufo2000)/soldier.dat" },
	["$(home)/armoury.set"] = {
		Origin = "$(ufo2000)/armoury.set" },
	["$(home)/items.dat"]   = {
		Origin = "$(ufo2000)/items.dat" },
	["$(home)/ufo2000.ini"] = {
		Origin = "$(ufo2000)/ufo2000.ini" },
	["$(home)/init-scripts.log"] = { },
	["$(home)/ufo2000.log"] = { },
	["$(home)/ufo2000.tmp"] = { },
	["$(home)/ufo2000.sav"] = { },
	["$(home)/cur_map.dat"] = { },
	["$(home)/cur_p1.dat"]  = { },
	["$(home)/cur_p2.dat"]  = { },

	["$(xcom)/geodata/biglets.dat"] = { Crc32 = X("0xEBC00F20") },
	["$(xcom)/geodata/smallset.dat"] = { Crc32 = X("0x0402BDDF") },

	["$(ufo2000)/arts/text_back.jpg"] = { Crc32 = X("0x00000000") },
	["$(ufo2000)/arts/menu.jpg"] = { Crc32 = X("0x00000000") },
	["$(ufo2000)/arts/empty.spk"] = { Crc32 = X("0x00000000") },

	["$(xcom)/ufointro/pict2.lbm"] = { Crc32 = X("0x783E3C9D"),
		Fallback = "$(ufo2000)/arts/menu.jpg" },
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

	["$(xcom)/ufograph/man_1f0.spk"] = { Crc32 = X("0xF87390C0") },
	["$(xcom)/ufograph/man_1f1.spk"] = { Crc32 = X("0x02CB471C") },
	["$(xcom)/ufograph/man_1f2.spk"] = { Crc32 = X("0x7E91A5E0") },
	["$(xcom)/ufograph/man_1f3.spk"] = { Crc32 = X("0xF050BD04") },
	["$(xcom)/ufograph/man_1m0.spk"] = { Crc32 = X("0xD919B6A5") },
	["$(xcom)/ufograph/man_1m1.spk"] = { Crc32 = X("0x3AD7592B") },
	["$(xcom)/ufograph/man_1m2.spk"] = { Crc32 = X("0x2A0840E1") },
	["$(xcom)/ufograph/man_1m3.spk"] = { Crc32 = X("0x47D7ACCB") },

	["$(xcom)/ufograph/man_2.spk"]   = { Crc32 = X("0x890C3076") },
	["$(xcom)/ufograph/man_3.spk"]   = { Crc32 = X("0xA8BB9C04") },

	["$(xcom)/ufograph/unibord.pck"] = { Crc32 = X("0x94576591") },
	["$(xcom)/ufograph/scanbord.pck"] = { Crc32 = X("0xE3F32D10") },

	["$(xcom)/units/handob.pck"] = { Crc32 = X("0x3EE61F8E") },
	["$(xcom)/units/handob.tab"] = { Crc32 = X("0x6EFF9E62") },
	["$(xcom)/units/floorob.pck"] = { Crc32 = X("0xDF863538") },
	["$(xcom)/units/floorob.tab"] = { Crc32 = X("0x833AC878") },

	["$(xcom)/units/xcom_0.pck"] = { Crc32 = X("0x8681EF39"), 
		Fallback = "$(xcom)/units/xcom_1.pck" },
	["$(xcom)/units/xcom_0.tab"] = { Crc32 = X("0x68930DB8"), 
		Fallback = "$(xcom)/units/xcom_1.tab" },

	["$(xcom)/units/xcom_1.pck"] = { Crc32 = X("0xF03FB73B") },
	["$(xcom)/units/xcom_1.tab"] = { Crc32 = X("0xA3839667") },
	["$(xcom)/units/xcom_2.pck"] = { Crc32 = X("0xC49CCBE5") },
	["$(xcom)/units/xcom_2.tab"] = { Crc32 = X("0x528354F7") },

	["$(xcom)/units/muton.pck"] = { Crc32 = X("0xD7E169A3"), 
		Fallback = "$(xcom)/units/xcom_1.pck" },
	["$(xcom)/units/muton.tab"] = { Crc32 = X("0x91FC80DE"), 
		Fallback = "$(xcom)/units/xcom_1.tab" },
	["$(xcom)/units/sectoid.pck"] = { Crc32 = X("0xF60F22DC"), 
		Fallback = "$(xcom)/units/xcom_1.pck" },
	["$(xcom)/units/sectoid.tab"] = { Crc32 = X("0x6831E47A"), 
		Fallback = "$(xcom)/units/xcom_1.tab" },

	["$(xcom)/ufograph/cursor.pck"] = { Crc32 = X("0x69DE75D6") },
	["$(xcom)/ufograph/cursor.tab"] = { Crc32 = X("0xF5FCB568") },
	["$(xcom)/ufograph/hit.pck"] = { Crc32 = X("0x29C07717") },
	["$(xcom)/ufograph/hit.tab"] = { Crc32 = X("0x4FF3EE8F") },
	["$(xcom)/ufograph/smoke.pck"] = { Crc32 = X("0x65CB2E8E") },
	["$(xcom)/ufograph/smoke.tab"] = { Crc32 = X("0xEE8E1054") },
	["$(xcom)/ufograph/icons.pck"] = { Crc32 = X("0xD407EF95") },

	["$(xcom)/geodata/loftemps.dat"] = { Crc32 = X("0x7B354479") },
	["$(xcom)/geodata/obdata.dat"] = { Crc32 = X("0x040A286A") },
	["$(xcom)/geodata/scang.dat"] = { Crc32 = X("0x26C1BD1B") },

	["$(xcom)/units/bigobs.pck"] = { Crc32 = X("0xBB5FB17D") },
	["$(xcom)/units/bigobs.tab"] = { Crc32 = X("0x422CC4A5") },

	["$(xcom)/ufograph/tac00.scr"] = { Crc32 = X("0x226E61A0") },
	["$(xcom)/ufograph/tac01.scr"] = { Crc32 = X("0xBBB1E67F") },

	["$(xcom)/sound/gmstory.mid"]  = { Crc32 = X("0x00000000"), Fallback = "" },
	["$(xcom)/sound/gmenbase.mid"] = { Crc32 = X("0x00000000"), Fallback = "" },
	["$(xcom)/sound/gmdefend.mid"] = { Crc32 = X("0x00000000"), Fallback = "" },
	["$(xcom)/sound/gmtactic.mid"] = { Crc32 = X("0x00000000"), Fallback = "" },
	["$(xcom)/sound/gmwin.mid"]    = { Crc32 = X("0x00000000"), Fallback = "" },
	["$(xcom)/sound/gmlose.mid"]   = { Crc32 = X("0x00000000"), Fallback = "" },
	["$(xcom)/sound/gmgeo1.mid"]   = { Crc32 = X("0x00000000"), Fallback = "" },
	["$(xcom)/sound/gmgeo2.mid"]   = { Crc32 = X("0x00000000"), Fallback = "" },
	["$(xcom)/sound/sound1.cat"]   = { Crc32 = X("0x00000000"), Fallback = "" },
	["$(xcom)/sound/sound2.cat"]   = { Crc32 = X("0x00000000"), Fallback = "" },
	["$(xcom)/sound/sample.cat"]   = { Crc32 = X("0x00000000"), Fallback = "" },
	["$(xcom)/sound/sample2.cat"]  = { Crc32 = X("0x00000000"), Fallback = "" },
	["$(xcom)/sound/sample3.cat"]  = { Crc32 = X("0x00000000"), Fallback = "" },
}

local function CheckSingleDataFile(name, info)
	if info.Crc32 then
	-- read only data file
		local data = ReadFile(name)
		if data == nil then
			return string.format("CheckDataFiles: can't open file '%s'", name)
		end
		local Crc32 = UpdateCrc32(0, data)
		if info.Crc32 ~= Crc32 and info.Crc32 ~= 0 then
			return string.format("CheckDataFiles: crc32 error for file '%s'", name)
		end
	else
	-- readwrite data file
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
						"CheckDataFiles: can't get original file '%s' for initialization", 
						info.Origin) 
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

local function CheckDataFiles()
	for key, info in FilesTable do
		local name = LocateFile(key)
		local errmsg = CheckSingleDataFile(name, info)
		if errmsg and info.Fallback == "" then
			errmsg = nil
			name = ""
		end

		if errmsg and info.Fallback then
			name = LocateFile(info.Fallback)
			if not FilesTable[info.Fallback] then
				Error("CheckDataFiles: invalid fallback '%s'", name)
			end
			errmsg = CheckSingleDataFile(name, FilesTable[info.Fallback])
		end

		if errmsg then Error(errmsg) end
		info.FileName = name
	end
end

CheckDataFiles()

function GetDataFileName(x)
	local _, _, p1, p2 = string.find(x, "(.*)(%#[^%#]+)$")
	if p1 and p2 and FilesTable[p1] then return FilesTable[p1].FileName .. p2 end
	if FilesTable[x] then return FilesTable[x].FileName end

	local fname = LocateFile(x)
	local fh = io.open(fname, "rb")
	if fh then
		fh:close()
		if string.find(x, "^%$") then
			Warning("Warning: '%s' is not listed in the 'FilesTable'", x)
			FilesTable[x] = { FileName = fname }
		end
		return fname
	end

	return nil
end
