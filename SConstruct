BuildDir('#' + "obj", "#.", duplicate=0)
SConsignFile()
SetOption('implicit_cache', 1)
SetOption('max_drift', 1)

env = Environment()

game_sources = Split("""
	bullet.cpp cell.cpp config.cpp connect.cpp dirty.cpp
	editor.cpp explo.cpp font.cpp icon.cpp inventory.cpp item.cpp
	keys.cpp main.cpp mainmenu.cpp map.cpp map_pathfind.cpp
	multiplay.cpp packet.cpp pck.cpp place.cpp
	platoon.cpp soldier.cpp sound.cpp spk.cpp terrapck.cpp
	units.cpp video.cpp wind.cpp crc32.cpp persist.cpp
	minimap.cpp about.cpp stats.cpp server_protocol.cpp 
	server_transport.cpp server_gui.cpp server_config.cpp music.cpp 
	scenario.cpp

	jpgalleg/jpgalleg.c jpgalleg/decode.c jpgalleg/encode.c jpgalleg/io.c
""")

env.Append(LIBS = ["expat", "lua", "lualib"])

if env["CC"] == "gcc":
	env.Append(CPPFLAGS = ["-O2", "-funsigned-char", "-Wall", "-Wno-deprecated-declarations"])

if env["CC"] == "cl":
	env.Append(CPPFLAGS = ["-O2", "-J", "-GX", "-MD"])
	env.Append(CPPPATH=["#msvc-libs/include"])
	env.Append(LIBPATH=["#msvc-libs/lib"])

if str(Platform()) == "win32": 
	env.Append(CPPDEFINES = ["WIN32", "ALLEGRO_STATICLINK", "DEBUGMODE"])
	env.Append(LIBS = ["NL_s", "alleg_s", "ws2_32", 
		"kernel32", "user32", "gdi32", "comdlg32", "ole32", "dinput", "ddraw", 
		"dxguid", "winmm", "dsound"])
	if env["CC"] == "gcc":
		game_sources.append("exchndl/exchndl.c")
		env.Append(LIBS = ["bfd", "iberty"])
		env.Append(CPPPATH=["#mingw-libs/include"])
		env.Append(LIBPATH=["#mingw-libs/lib"])
else:
	env.Append(CPPDEFINES = ["LINUX", "DEBUGMODE"])
	env.Append(LIBS = ["NL"]) 
	env.ParseConfig("allegro-config --cflags --libs") 

tmp = []
for x in game_sources: tmp.append("obj/src/" + x)
game_sources = tmp

if str(Platform()) == "win32": 
	if env["CC"] == "gcc":
		game_sources.append("obj/Seccast.o")
	else:
		game_sources.append("obj/ufo2000.res")

env.Program("#ufo2000", game_sources)
