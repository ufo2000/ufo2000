##############################################################################
# NOTE: this file is NOT used to build the game right now, it is just an
# experimental SCons support, it might or might not work for you, better use 
# makefile instead
#
# Check http://ufo2k.lxnt.info/mantis/view.php?id=255 for more details
##############################################################################

BuildDir('#' + "obj", "#.", duplicate=0)
SConsignFile()
SetOption('implicit_cache', 1)
SetOption('max_drift', 1)

env = Environment()

HAVE_DUMBOGG=False
HAVE_TTF=False

def getsources():
    import os
    import os.path

    allsource = []
    prefix = ''
    for root, dirs, files in os.walk('src'):
        for name in files:
            if name[-4:] == ".cpp" or name[-2:] == ".c":
                if name != 'server_main.cpp':
                    allsource.append(os.path.join('obj', root, name))
        if 'dumbogg' in dirs and not HAVE_DUMBOGG:
            dirs.remove('dumbogg')
        if 'glyphkeeper' in dirs and not HAVE_TTF:
            dirs.remove('glyphkeeper')
        if 'exchndl' in dirs:
            dirs.remove('exchndl')
        if '.svn' in dirs:
            dirs.remove('.svn')
        if 'luac' in dirs:
            dirs.remove('luac')
        if 'lua' in dirs:
            dirs.remove('lua')

    return allsource

game_sources = getsources()

env.Append(LIBS = ["expat", "lua", "lualib", "sqlite3", "png", "z"])

if env["CC"] == "gcc":
    env.Append(CCFLAGS = ["-funsigned-char", "-Wall", "-Wno-deprecated-declarations"])

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
    if HAVE_DUMBOGG:
        env.Append(CPPDEFINES = ["HAVE_DUMBOGG"])
    env.Append(LIBS = ["NL"])
    if False:
        dict = env.Dictionary()
        for key in dict:
            print "key = %s, value = %s" % (key, dict[key])
    env.Append(LIBPATH=["/usr/local/lib"])
    env.Append(LINKFLAGS=["-pthread"])
    env.Append(CPPPATH=["/usr/local/include"])
    env.ParseConfig("allegro-config --cflags --libs")

# debug stuff for *n*x
debug_env = env.Copy(CCFLAGS = "-g")

if str(Platform()) == "win32":
    if env["CC"] == "gcc":
        game_sources.append("obj/Seccast.o")
    else:
        game_sources.append("obj/ufo2000.res")

env.Program("#ufo2000", game_sources)
