#!/usr/bin/env lua

if arg[1] == nil or arg[2] == nil then
    print("Usage: upgrade_weapon_sounds.lua soundmap.xml some_weapon_set.lua")
    os.exit(1)
end

local current_catfile = nil
local current_identifier = nil

local identifiers_table = {}
local aliases_table = {}

for l in io.lines(arg[1]) do
    local _, _, catfile = string.find(l, "(\".-%.cat\")")
    if _ then current_catfile = catfile end
    local _, _, idx, identifier = string.find(l, "%#(%d+).*sym%=(\".-\")")
    if _ then
        identifier = string.lower(identifier)
        current_identifier = identifier
        if identifiers_table[identifier] then
            identifiers_table[identifier] = identifiers_table[identifier] .. " or "
        else
            identifiers_table[identifier] = ""
        end
        identifiers_table[identifier] = identifiers_table[identifier] .. string.format(
            "cat_sample(%s, %d)", current_catfile, tonumber(idx))
    end
    local _, _, identifier = string.find(l, "alias%s*sym%=(\".-\")")
    if _ then
        identifier = string.lower(identifier)
        aliases_table[identifier] = current_identifier
    end
end

for l in io.lines(arg[2]) do
    local _, _, prefix, identifier, suffix = string.find(l, "^(%s*sound%s*=%s*)(\".-\")(.*)$")
    if identifier then
        local identifier_orig = identifier
        identifier = string.lower(identifier)
        identifier = aliases_table[identifier] or identifier
        print(string.format("%s%s%s", prefix, identifiers_table[identifier] or identifier_orig, suffix))
    else
        print(l)
    end
end
