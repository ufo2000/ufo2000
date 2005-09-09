------------------------------------------------------------------------------
-- Set explosion animation
-- (experimental feature, interface can change any time)
------------------------------------------------------------------------------

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

------------------------------------------------------------------------------
-- Add auxiliary images
-- (such as cursors, selectors etc.)
------------------------------------------------------------------------------
AddImage("mouse", png_image("$(ufo2000)/arts/mouse.png"))
AddImage("mouse_menu", png_image("$(ufo2000)/arts/mouse_menu.png"))
AddImage("selector", png_image_set("$(ufo2000)/arts/selector/19x9", 1, 4))

-- Our own images
-------------------
AddImage("fire_small", png_image_set("$(ufo2000)/arts/fire_frames/32x40", 1, 4))
AddImage("fire_large", png_image_set("$(ufo2000)/arts/fire_frames/32x40", 5, 4))
AddImage("selectbox", png_image_set("$(ufo2000)/arts/cursor_frames/32x40", 1, 6))
AddImage("aimbox", png_image_set("$(ufo2000)/arts/reticle_aim/32x40", 1, 10))
AddImage("throwbox", png_image_set("$(ufo2000)/arts/reticle_throw/32x40", 1, 12))

-- Original X-Com images (uncomment the following lines to use original images
-- if available)
-- Warning: throwing cursor is displayed incorrectly (no front edges)
-------------------
-- AddImage("fire_small", pck_image_set("$(xcom)/ufograph/smoke.pck", 4, 4))
-- AddImage("fire_large", pck_image_set("$(xcom)/ufograph/smoke.pck", 0, 4))
-- AddImage("selectbox", { pck_image_ex(false, 32, 40, "$(xcom)/ufograph/cursor.pck", 3),
--                         pck_image_ex(false, 32, 40, "$(xcom)/ufograph/cursor.pck", 4),
--                         pck_image_ex(false, 32, 40, "$(xcom)/ufograph/cursor.pck", 5),
--                         pck_image_ex(false, 32, 40, "$(xcom)/ufograph/cursor.pck", 0),
--                         pck_image_ex(false, 32, 40, "$(xcom)/ufograph/cursor.pck", 1),
--                         pck_image_ex(false, 32, 40, "$(xcom)/ufograph/cursor.pck", 2) } )
-- AddImage("aimbox", { pck_image_ex(false, 32, 40, "$(xcom)/ufograph/cursor.pck", 6),
--                      pck_image_ex(false, 32, 40, "$(xcom)/ufograph/cursor.pck", 7),
--                      pck_image_ex(false, 32, 40, "$(xcom)/ufograph/cursor.pck", 8),
--                      pck_image_ex(false, 32, 40, "$(xcom)/ufograph/cursor.pck", 9),
--                      pck_image_ex(false, 32, 40, "$(xcom)/ufograph/cursor.pck", 10),
--                      pck_image_ex(false, 32, 40, "$(xcom)/ufograph/cursor.pck", 6),
--                      pck_image_ex(false, 32, 40, "$(xcom)/ufograph/cursor.pck", 7),
--                      pck_image_ex(false, 32, 40, "$(xcom)/ufograph/cursor.pck", 8),
--                      pck_image_ex(false, 32, 40, "$(xcom)/ufograph/cursor.pck", 9),
--                      pck_image_ex(false, 32, 40, "$(xcom)/ufograph/cursor.pck", 10) } )
-- AddImage("throwbox", { pck_image_ex(false, 32, 40, "$(xcom)/ufograph/cursor.pck", 15),
--                        pck_image_ex(false, 32, 40, "$(xcom)/ufograph/cursor.pck", 16),
--                        pck_image_ex(false, 32, 40, "$(xcom)/ufograph/cursor.pck", 0),
--                        pck_image_ex(false, 32, 40, "$(xcom)/ufograph/cursor.pck", 0) } )
