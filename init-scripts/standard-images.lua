------------------------------------------------------------------------------
-- Set explosion animation
-- (experimental feature, interface can change any time)
------------------------------------------------------------------------------

if prefer_xcom_gfx and ReadFile(LocateFile("$(xcom)/ufograph/x1.pck")) then
    SetExplosionAnimation {
        pck_image_ex(false, 128, 64, "$(xcom)/ufograph/x1.pck", 0),
        pck_image_ex(false, 128, 64, "$(xcom)/ufograph/x1.pck", 1),
        pck_image_ex(false, 128, 64, "$(xcom)/ufograph/x1.pck", 2),
        pck_image_ex(false, 128, 64, "$(xcom)/ufograph/x1.pck", 3),
        pck_image_ex(false, 128, 64, "$(xcom)/ufograph/x1.pck", 4),
        pck_image_ex(false, 128, 64, "$(xcom)/ufograph/x1.pck", 5),
        pck_image_ex(false, 128, 64, "$(xcom)/ufograph/x1.pck", 6),
        pck_image_ex(false, 128, 64, "$(xcom)/ufograph/x1.pck", 7),
    }
else
    SetExplosionAnimation {
        png_image_ex("$(ufo2000)/arts/explosion/256x178-001.png", true),
        png_image_ex("$(ufo2000)/arts/explosion/256x178-002.png", true),
        png_image_ex("$(ufo2000)/arts/explosion/256x178-003.png", true),
        png_image_ex("$(ufo2000)/arts/explosion/256x178-004.png", true),
        png_image_ex("$(ufo2000)/arts/explosion/256x178-005.png", true),
        png_image_ex("$(ufo2000)/arts/explosion/256x178-006.png", true),
        png_image_ex("$(ufo2000)/arts/explosion/256x178-007.png", true),
        png_image_ex("$(ufo2000)/arts/explosion/256x178-008.png", true),
        png_image_ex("$(ufo2000)/arts/explosion/256x178-009.png", true),
        png_image_ex("$(ufo2000)/arts/explosion/256x178-010.png", true),
    }
end

------------------------------------------------------------------------------
-- Add auxiliary images
-- (such as cursors, selectors etc.)
------------------------------------------------------------------------------
AddImage("mouse", png_image("$(ufo2000)/arts/mouse.png"))
AddImage("mouse_menu", png_image("$(ufo2000)/arts/mouse_menu.png"))
AddImage("selector", png_image_set("$(ufo2000)/arts/selector/19x9", 1, 4))

-- Our own images, 2 fire animation, 3 smoke animations, cursors, 4 hit animations.
-------------------
AddImage("fire_small", png_image_set("$(ufo2000)/arts/fire_frames/32x40", 1, 4, true))
AddImage("fire_large", png_image_set("$(ufo2000)/arts/fire_frames/32x40", 5, 4, true))
AddImage("smoke_small", png_image_set("$(ufo2000)/arts/smoke_frames/32x40", 1, 4, true))
AddImage("smoke_medium", png_image_set("$(ufo2000)/arts/smoke_frames/32x40", 5, 4, true))
AddImage("smoke_large", png_image_set("$(ufo2000)/arts/smoke_frames/32x40", 9, 4, true))
AddImage("selectbox", png_image_set("$(ufo2000)/arts/cursor_frames/32x40", 1, 6))
AddImage("aimbox", png_image_set("$(ufo2000)/arts/reticle_aim/32x40", 1, 10))
AddImage("throwbox", png_image_set("$(ufo2000)/arts/reticle_throw/32x40", 1, 12))
AddImage("hit_bullet", png_image_set("$(ufo2000)/arts/impact_frames/32x40", 1, 8, true))
AddImage("hit_laser", png_image_set("$(ufo2000)/arts/impact_frames/32x40", 9, 8, true))
AddImage("hit_plasma", png_image_set("$(ufo2000)/arts/impact_frames/32x40", 17, 8, true))
AddImage("hit_punch", png_image_set("$(ufo2000)/arts/impact_frames/32x40", 25, 8, true))

-- Original X-Com images (uncomment the following lines to use original images
-- if available)
-- Warning: throwing cursor is displayed incorrectly (no front edges)
-------------------
if prefer_xcom_gfx then
    AddImage("fire_small", pck_image_set("$(xcom)/ufograph/smoke.pck", 4, 4))
    AddImage("fire_large", pck_image_set("$(xcom)/ufograph/smoke.pck", 0, 4))
    AddImage("smoke_small", pck_image_set("$(xcom)/ufograph/smoke.pck", 8, 4))
    AddImage("smoke_medium", pck_image_set("$(xcom)/ufograph/smoke.pck", 12, 4))
    AddImage("smoke_large", pck_image_set("$(xcom)/ufograph/smoke.pck", 16, 4))
    AddImage("selectbox", pck_image_set_ex(false, 32, 40, "$(xcom)/ufograph/cursor.pck", 
        {3, 4, 5, 0, 1, 2}))
    AddImage("aimbox", pck_image_set_ex(false, 32, 40, "$(xcom)/ufograph/cursor.pck", 
        {6, 7, 8, 9, 10, 6, 7, 8, 9, 10}))
    AddImage("throwbox", pck_image_set_ex(false, 32, 40, "$(xcom)/ufograph/cursor.pck", 
        {15, 16, 0, 0}))
    AddImage("hit_bullet", pck_image_set("$(xcom)/ufograph/smoke.pck", 26, 10))
    AddImage("hit_laser", pck_image_set("$(xcom)/ufograph/smoke.pck", 36, 10))
    AddImage("hit_plasma", pck_image_set("$(xcom)/ufograph/smoke.pck", 46, 10))
    AddImage("hit_punch", pck_image_set("$(xcom)/ufograph/smoke.pck", 26, 10))
end
