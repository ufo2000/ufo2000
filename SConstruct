##############################################################################
# NOTE: this file is NOT used to build the game right now, it is just an
# experimental SCons support, it might or might not work for you, better use 
# makefile instead
#
# Check http://ufo2k.lxnt.info/mantis/view.php?id=255 for more details
##############################################################################

import os
import os.path

build_dir = ".build-dir"
BuildDir(build_dir, ".", duplicate=0)
SConsignFile()
SetOption('implicit_cache', 1)
SetOption('max_drift', 1)

if str(Platform()) == "win32":
    env = Environment(tools=['mingw'])
    if env["CC"] == "cl":
        env.Append(CPPPATH=["#msvc-libs/include"])
        env.Append(LIBPATH=["#msvc-libs/lib"])
    else:
        env.Append(CPPPATH=["#mingw-libs/include"])
        env.Append(LIBPATH=["#mingw-libs/lib"])
else:
    env = Environment()

conf = env.Configure()
if not conf.CheckCHeader("expat.h"):
    print "You need 'expat' library installed"
    Exit(1)
if not conf.CheckCHeader("zlib.h"):
    print "You need 'zlib' library installed (http://www.zlib.net)"
    Exit(1)
if not conf.CheckCHeader("png.h"):
    print "You need 'libpng' library installed"
    Exit(1)
if not conf.CheckCHeader("sqlite3.h"):
    print "You need 'sqlite3' library installed (http://www.sqlite.org)"
    Exit(1)
if not conf.CheckCHeader("allegro.h"):
    print "You need 'allegro' library installed (http://www.sqlite.org)"
    Exit(1)
if not conf.CheckCHeader("nl.h"):
    print "You need 'HawkNL' library installed"
    Exit(1)
env = conf.Finish()

HAVE_DUMBOGG=False
HAVE_TTF=True

def getsources():

    allsource = []
    prefix = ''
    for root, dirs, files in os.walk('src'):
        for name in files:
            if name[-4:] == ".cpp" or name[-2:] == ".c":
                if name != 'server_main.cpp' and name != 'lua.c':
                    allsource.append(os.path.join(root, name))
        if 'dumbogg' in dirs and not HAVE_DUMBOGG:
            dirs.remove('dumbogg')
        if 'glyphkeeper' in dirs:
            dirs.remove('glyphkeeper')
        if 'exchndl' in dirs:
            dirs.remove('exchndl')
        if '.svn' in dirs:
            dirs.remove('.svn')
        if 'luac' in dirs:
            dirs.remove('luac')

    return allsource

game_sources = getsources()

env.Append(LIBS = ["expat", "sqlite3", "png", "z"])
env.Append(CPPPATH = ["src/lua"])
env.Append(CPPDEFINES = ["DEBUGMODE", "HAVE_PNG"])
if HAVE_TTF:
    env.Append(CPPDEFINES = ["HAVE_FREETYPE", "GLYPH_TARGET=GLYPH_TARGET_ALLEGRO", "GK_NO_LEGACY"])
    game_sources.append("src/glyphkeeper/glyph.c")

if env["CC"] == "gcc":
    env.Append(CCFLAGS = ["-funsigned-char", "-Wall", "-Wno-deprecated-declarations"])

if env["CC"] == "cl":
    env.Append(CPPFLAGS = ["-O2", "-J", "-GX", "-MD"])

if str(Platform()) == "win32":
    env.Append(CPPDEFINES = ["WIN32", "ALLEGRO_STATICLINK"])
    env.Append(LIBS = ["NL_s", "alleg_s", "ws2_32",
        "kernel32", "user32", "gdi32", "comdlg32", "ole32", "dinput", "ddraw",
        "dxguid", "winmm", "dsound"])
    if env["CC"] == "gcc":
        game_sources.append("src/exchndl/exchndl.c")
        env.Append(LIBS = ["bfd", "iberty"])
    if HAVE_TTF:
        env.Append(LIBS = ["freetype"])
else:
    env.Append(CPPDEFINES = ["LINUX"])
    env.Append(LIBS = ["NL"])
    env.Append(LINKFLAGS=["-pthread"])
    env.ParseConfig("allegro-config --cflags --libs")
    if HAVE_TTF:
        env.ParseConfig("freetype-config --cflags --libs")

if str(Platform()) == "win32":
    if env["CC"] == "gcc":
        game_sources.append("Seccast.o")
    else:
        game_sources.append("ufo2000.res")

game_sources_prefixed = []
for filename in game_sources: game_sources_prefixed.append(os.path.join(build_dir, filename))

ufo2000 = env.Program("#ufo2000", game_sources_prefixed)

# Generate MSVC project file

envp = Environment()
envp.MSVSProject(target = 'ufo2000' + envp['MSVSPROJECTSUFFIX'],
                          srcs = game_sources,
                          buildtarget = ufo2000,
                          variant = 'Release')
