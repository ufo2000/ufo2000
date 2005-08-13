##############################################################################
# NOTE: this file is NOT used to build the game right now, it is just an
# experimental SCons support, it might or might not work for you, better use 
# makefile instead
#
# Check http://ufo2k.lxnt.info/mantis/view.php?id=255 for more details
##############################################################################

import os
import os.path

debug_build = ARGUMENTS.get('debug', 0)

if debug_build:
    build_dir = ".build-dir-debug"
    exe_name = "ufo2000-debug"
else:
    build_dir = ".build-dir"
    exe_name = "ufo2000"

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

env.ParseConfig("allegro-config --cflags --libs")
env.ParseConfig("freetype-config --cflags --libs")

HAVE_TTF = True
HAVE_DUMBOGG = True

##############################################################################
# Download and extract archive with thirdparty libraries
##############################################################################

def download_libs(libdirname):
    import urllib
    import zipfile
    try:
        zipname = libdirname + ".zip"
        if not os.access(zipname, os.F_OK):
            urllib.urlretrieve("http://ufo2000.lxnt.info/files/" + zipname, zipname)
        zf = zipfile.ZipFile(zipname, "r")
        if zf.testzip():
            os.remove(zipname)
            return False
        for root, dirs, files in os.walk(libdirname, topdown=False):
            for name in files:
                os.remove(os.path.join(root, name))
            for name in dirs:
                os.rmdir(os.path.join(root, name))
        for filename in zf.namelist():
            data = zf.read(filename)
            if data:
                if not os.access(os.path.dirname(filename), os.F_OK):
                    os.makedirs(os.path.dirname(filename))
                fh = open(filename, "wb")
                fh.write(data)
                fh.close()
    except:
        return False
    return True

conf_error = False
conf = env.Configure()

def check_required_libs(conf):
    if not conf.CheckHeader("allegro.h"):
        print "Required library not found: 'allegro' (http://www.talula.demon.co.uk/allegro)"
        return False
    if not conf.CheckHeader("nl.h"):
        print "Required library not found: 'HawkNL' (http://www.hawksoft.com/hawknl)"
        return False
    if not conf.CheckHeader("expat.h"):
        print "Required library not found: 'expat' (http://expat.sourceforge.net)"
        return False
    if not conf.CheckHeader("zlib.h"):
        print "Required library not found: 'zlib' (http://www.zlib.net)"
        return False
    if not conf.CheckHeader("png.h"):
        print "Required library not found: 'libpng' (http://www.libpng.org)"
        return False
    if not conf.CheckHeader("sqlite3.h"):
        print "Required library not found: 'sqlite3' (http://www.sqlite.org)"
        return False
    return True

if not check_required_libs(conf):
    if str(Platform()) == "win32" and env["CC"] == "gcc":
        print("Trying to download 'mingw-libs.zip' package, please wait...")
        if not download_libs("mingw-libs"):
            print("Download failed!")
            Exit(1)
        print("Success, running configure step again...")
        if not check_required_libs(conf):
            print("Still no luck, giving up :(")
            Exit(1)
    else:
        Exit(1)

if not conf.CheckHeader("ft2build.h"):
    print "Library 'freetype2' not found, building without truetype fonts support"
    HAVE_TTF = False
if not conf.CheckHeader("dumb.h") or not conf.CheckHeader("vorbis/vorbisfile.h"):
    print "Either 'DUMB' or 'Ogg Vorbis' library not found, building without music support"
    HAVE_DUMBOGG = False

env = conf.Finish()

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
if HAVE_DUMBOGG:
    env.Append(CPPDEFINES = ["HAVE_DUMBOGG"])
    env.Append(LIBS = ["vorbisfile", "vorbis", "ogg", "aldmb", "dumb"])

if env["CC"] == "gcc":
    env.Append(CCFLAGS = ["-funsigned-char", "-Wall", "-Wno-deprecated-declarations"])
    if debug_build:
        env.Append(CCFLAGS = ["-g"])
    else:
        env.Append(CCFLAGS = ["-O2"])

if env["CC"] == "cl":
    env.Append(CPPFLAGS = ["-O2", "-J", "-GX", "-MT"])

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

if str(Platform()) == "win32":
    if env["CC"] == "gcc":
        game_sources.append("Seccast.o")
    else:
        game_sources.append("ufo2000.res")

game_sources_prefixed = []
for filename in game_sources: game_sources_prefixed.append(os.path.join(build_dir, filename))

ufo2000 = env.Program("#" + exe_name, game_sources_prefixed)

##############################################################################
# Generate Dev-C++ project file
##############################################################################

f = open("ufo2000.dev", "w")
f.write("""
[Project]
FileName=ufo2000.dev
Name=ufo2000
UnitCount=%d
Type=1
Ver=1
ObjFiles=
Includes=
Libs=
PrivateResource=
ResourceIncludes=
MakeIncludes=
Compiler=
CppCompiler=
Linker=
IsCpp=1
Icon=
ExeOutput=
ObjectOutput=
OverrideOutput=1
OverrideOutputName=ufo2000-debug.exe
HostApplication=
Folders=
CommandLine=
UseCustomMakefile=1
CustomMakefile=makefile.scons
IncludeVersionInfo=0
SupportXPThemes=0
CompilerSet=0
CompilerSettings=0000000000000000000000
""" % len(game_sources))

i = 1
for filename in game_sources:
    f.write("""
[Unit%d]
FileName=%s
CompileCpp=1
Folder=
Compile=1
Link=1
Priority=1000
OverrideBuildCmd=0
BuildCmd=
""" % (i, filename))
    i += 1

f.close()

f = open("makefile.scons", "w")
f.write("all:\n\tscons debug=1\nclean:\n\tscons -c debug=1")
f.close()
