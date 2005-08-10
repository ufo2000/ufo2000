------------------------------------------------------------------------------
-- Warning: API for units modding is still not finalized and can be changed
--          in the future !!!
------------------------------------------------------------------------------

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

local falling_frames = {}

for row = 0, 7, 1 do
    table.insert(falling_frames, png_image_fragment("$(extension)/soldier.png",
         298, 1 + row * 41, 32, 40))
end

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
-- info   - table with some additional information, currently it can contain 
--          the following properties:
--            "hand_object_image" - contains bitmap with the weapon image
--            "aimed_hand_object_image" - contains bitmap with the weapon image rotated 90 degrees
--            "is_crouching"      - boolean flag, shows that the soldier is crouching
--            "is_falling"        - boolean flag, indicates request for death or stun animation
--            "is_flying"         - boolean flag, indicates that the soldier is flying
------------------------------------------------------------------------------

local function sprite_renderer(img, dir, phase, info)
    -- falling unit (dead or stunned)
    if info.is_falling then
        img:draw(0, 0, falling_frames[phase + 1])
        return
    end
    -- ordinary image, no weapons in hands
    if not info.hand_object_image then
        img:draw(0, 0, frames[1 + dir * 9 + phase])
        return
    end
    -- crouching soldiers hold their weapons closer to the ground
    local delta_y = 0
    if info.is_crouching then delta_y = 4 end
    if (dir < 1 or dir > 5) then
        -- show a gun first, then overwrite it with a soldier body
        img:draw(0, delta_y, info.hand_object_image)
        img:draw(0, 0, frames[1 + dir * 9 + phase])
    else
        -- show soldier body first, then overwrite it with a gun
        img:draw(0, 0, frames[1 + dir * 9 + phase])
        img:draw(0, delta_y, info.hand_object_image)
    end
end

------------------------------------------------------------------------------
-- Here comes unit description itself (only map sprite rendering function 
-- and inventory picture are defined right now)
--
-- Currenly AddUnit only adds appearances for a temporary unit
-- skin called 'chameleon'
------------------------------------------------------------------------------

AddUnit {
    name = "soldier",
    pMap  = sprite_renderer,
    pInv  = png_image_fragment("$(extension)/soldier.png", 344, 0, 420 - 350, 150)
}
