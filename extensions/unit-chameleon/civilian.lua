local frames = pck_image_set("$(xcom)/units/civm.pck", 0, 8 * 9 + 3)
if not frames then return end -- check if the user actually has this pck file

local function sprite_renderer(img, dir, phase, info)
    if info.is_falling then
        img:draw(0, 0, frames[8 * 9 + math.floor(phase / 3)])
        return
    end
    if phase == 0 then
        img:draw(0, 0, frames[1 + dir])
    else
        img:draw(0, 0, frames[8 + dir * 8 + phase])
    end
end

AddUnit {
    name  = "civilian",
    pMap  = sprite_renderer,
    pInv  = pck_image("$(xcom)/units/civm.pck", 3),
}

AddXcomItem {
    cost = 0,
    name = "Civilian Corpse",
    pInv = pck_image("$(xcom)/units/bigobs.pck", 3),
    pMap = pck_image("$(xcom)/units/floorob.pck", 50),
    health = 20,
    importance = 30,
    width = 1,
    height = 2,
    pHeld = pck_image_set("$(xcom)/units/handob.pck", 96, 8),
    weight = 30,
    minimapMark = 1,
}
