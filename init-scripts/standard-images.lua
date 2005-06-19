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
AddImage("selector", png_image_set("$(ufo2000)/arts/selector/9x9", 1, 4))
