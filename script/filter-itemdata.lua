ItemsTable = {}
function AddXcomItem(item) ItemsTable[item.index] = item end

require "standard-items.lua"

function is_badline(l)
	if string.find(l, "^%s*_") then return true end
	if string.find(l, "^%s*throw_accuracy") then return true end
	if string.find(l, "^%s*primeType") then return true end
	if string.find(l, "^%s*rounds = 0") then return true end
	if string.find(l, "^%s*isAlien = 0") then return true end
	if string.find(l, "^%s*damage = 0") then return true end
	if string.find(l, "^%s*twoHanded = 0") then return true end
	if string.find(l, "^%s*isGun = 0") then return true end
	if string.find(l, "^%s*isWeapon = 0") then return true end
	if string.find(l, "^%s*isAmmo = 0") then return true end
	if string.find(l, "^%s*isShootable = 0") then return true end
	if string.find(l, "^%s*wayPoints = 0") then return true end
	if string.find(l, "^%s*time = {0, 0, 0}") then return true end
	if string.find(l, "^%s*accuracy = {0, 0, 0}") then return true end
	if string.find(l, "^%s*ammo = {255, 255, 255}") then return true end
end

function process_ammo(l)
	local a = {}
	_, _, prefix, a[1], a[2], a[3] = string.find(l, "^(%s*ammo = ){(%d+), (%d+), (%d+)}")
	if _ then
		x = {}
		io.write(prefix, "{")
		for i = 1, 3 do
			if ItemsTable[tonumber(a[i])] then
				table.insert(x, ItemsTable[tonumber(a[i])].name)
			end
		end
		for k, v in ipairs(x) do
			io.write("\"", v, "\"")
			if x[k + 1] then io.write(", ") end
		end
		io.write("},\n")
		return true
	end
end

function process_pInv(l)
	local _, _, x = string.find(l, "^%s*pInv = (%d+)")
	if _ then
		io.write("\tpInv = pck_image(\"$(xcom)/units/bigobs.pck\", ", x, "),\n")
		return true
	end
end

function process_pMap(l)
	local _, _, x = string.find(l, "^%s*pMap = (%d+)")
	if _ then
		io.write("\tpMap = pck_image(\"$(xcom)/units/floorob.pck\", ", x, "),\n")
		return true
	end
end

for l in io.lines("standard-items.lua") do
	if not is_badline(l) then
		if not process_ammo(l) and not process_pInv(l) and not process_pMap(l) then
			print(l)
		end
	end
end
