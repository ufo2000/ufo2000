------------------------------------------------------------------------------
-- Loading frames from png file by specifying coordinates and size of each 
-- subframe and passing them to png_image_fragment() function
------------------------------------------------------------------------------

local frames = {}

for row = 7, 0, -1 do
    for col = 0, 8 do
        table.insert(frames, png_image_fragment("$(extension)/soldier.png", 
            1 + col * 33, 1 + math.mod(row + 4, 8) * 41, 32, 40))
    end
end

------------------------------------------------------------------------------
-- We do not have our own sprites for soldier death, so we "borrow" them from 
-- from ordinary X-COM soldier, also it is a good example of using mixed
-- frame sets:)
------------------------------------------------------------------------------

local collapse_frames = {
    pck_image("$(xcom)/units/xcom_0.pck", 33 * 8 + 0),
    pck_image("$(xcom)/units/xcom_0.pck", 33 * 8 + 0),
    pck_image("$(xcom)/units/xcom_0.pck", 33 * 8 + 1),
    pck_image("$(xcom)/units/xcom_0.pck", 33 * 8 + 1),
    pck_image("$(xcom)/units/xcom_0.pck", 33 * 8 + 1),
    pck_image("$(xcom)/units/xcom_0.pck", 33 * 8 + 2),
    pck_image("$(xcom)/units/xcom_0.pck", 33 * 8 + 2),
    pck_image("$(xcom)/units/xcom_0.pck", 33 * 8 + 2),
}

------------------------------------------------------------------------------
-- This function renders unit sprite image by probably combining several
-- frames together and drawing them all on img object which is
-- passed to this function as the first argument.
--
-- img    - target sprite object, it supports 'draw' method, which 
--          accepts 3 arguments: x, y coordinates and a bitmap 
--          object (loaded from png or pck file for example)
-- dir    - direction the soldier is facing (0, 1, 2, 3, 4, 5, 6, 7)
-- phase  - animation phase (0 - no animation, static image, 1-8 sequential 
--          frames)
--
-- extra  - table with some additional information, currently it can contain 
--          the following properties:
--            "gun"      - contains bitmap with the weapon image
--            "crouch"   - boolean flag, shows that the soldier is crouching
--            "collapse" - death or stun animation
------------------------------------------------------------------------------

local function sprite_renderer(img, dir, phase, extra)
    -- collapsing unit
    if extra.collapse then
        img:draw(0, 0, collapse_frames[phase + 1])
        return
    end
    -- ordinary image, no weapons in hands
    if not extra.gun then
        img:draw(0, 0, frames[1 + dir * 9 + phase])
        return
    end
    -- crouching soldiers hold their weapons closer to the ground
    local delta_y = 0
    if extra.crouch then delta_y = 4 end
    if (dir < 1 or dir > 5) then
        -- show a gun first, then overwrite it with a soldier body
        img:draw(0, delta_y, extra.gun)
        img:draw(0, 0, frames[1 + dir * 9 + phase])
    else
        -- show soldier body first, then overwrite it with a gun
        img:draw(0, 0, frames[1 + dir * 9 + phase])
        img:draw(0, delta_y, extra.gun)
    end
end

------------------------------------------------------------------------------
-- Here comes unit description itself (only map sprite rendering function 
-- and inventory picture are defined right now)
--
-- Currenly AddUnit only replaces appearance of the standard t-shirt x-com 
-- soldier for making experiments with replacing units battlescape sprites
------------------------------------------------------------------------------

AddUnit {
    pMap  = sprite_renderer,
    pInv  = png_image_fragment("$(extension)/soldier.png", 325, 0, 390 - 325, 150)
}
