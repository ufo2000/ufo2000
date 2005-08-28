local frames = pck_image_set("$(xcom)/units/ethereal.pck", 0, 8 * 9 + 3)
if frames then return end -- check if the user actually has this pck file

local function sprite_renderer(img, dir, phase, info)
    if info.collapse then
        img:draw(0, 0, frames[8 * 9 + math.floor(phase / 3)])
        return
    end

    local function draw_body()
        if phase == 0 then
            img:draw(0, 0, frames[1 + dir])
        else
            img:draw(0, 0, frames[8 + dir * 8 + phase])
        end
    end

    if not info.hand_object_image then draw_body() return end

    -- depending on the direction we may have to draw weapon or the body first
    if (dir < 1 or dir > 5) then
        -- show a gun first, then overwrite it with a soldier body
        img:draw(0, 0, info.hand_object_image)
        draw_body()
    else
        -- show soldier body first, then overwrite it with a gun
        draw_body()
        img:draw(0, 0, info.hand_object_image)
    end
end

AddUnit {
    name  = "ethereal",
    pMap  = sprite_renderer,
    pInv  = pck_image("$(xcom)/units/ethereal.pck", 3),
}
