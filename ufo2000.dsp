# Microsoft Developer Studio Project File - Name="ufo2000" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=ufo2000 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ufo2000.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ufo2000.mak" CFG="ufo2000 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ufo2000 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ufo2000 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/ufo2000", BAAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ufo2000 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir "obj/release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /G5 /MT /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "ALLEGRO_STATICLINK" /YX /J /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 user32.lib gdi32.lib ole32.lib ddraw.lib dplay.lib dsound.lib dinput.lib winmm.lib dxguid.lib alleg_s.lib wsock32.lib /nologo /subsystem:windows /map /machine:I386

!ELSEIF  "$(CFG)" == "ufo2000 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ufo2000___Win32_Devel"
# PROP BASE Intermediate_Dir "ufo2000___Win32_Devel"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir "obj/devel"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /J /FD /GZ /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MTd /W3 /Gm /Gi /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /J /FD /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib dplay.lib dxguid.lib alleg.lib /nologo /subsystem:windows /incremental:no /debug /machine:I386 /pdbtype:sept
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib dplay.lib dxguid.lib alleg.lib wsock32.lib /nologo /subsystem:windows /map /debug /machine:I386 /out:"ufo2000-debug.exe" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none /incremental:no

!ENDIF 

# Begin Target

# Name "ufo2000 - Win32 Release"
# Name "ufo2000 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\about.cpp
# End Source File
# Begin Source File

SOURCE=.\src\bullet.cpp
# End Source File
# Begin Source File

SOURCE=.\src\cell.cpp
# End Source File
# Begin Source File

SOURCE=.\src\config.cpp
# End Source File
# Begin Source File

SOURCE=.\src\connect.cpp
# End Source File
# Begin Source File

SOURCE=.\src\crc32.cpp
# End Source File
# Begin Source File

SOURCE=.\src\dirty.cpp
# End Source File
# Begin Source File

SOURCE=.\src\editor.cpp
# End Source File
# Begin Source File

SOURCE=.\src\explo.cpp
# End Source File
# Begin Source File

SOURCE=.\src\font.cpp
# End Source File
# Begin Source File

SOURCE=.\src\icon.cpp
# End Source File
# Begin Source File

SOURCE=.\src\inventory.cpp
# End Source File
# Begin Source File

SOURCE=.\src\item.cpp
# End Source File
# Begin Source File

SOURCE=.\src\jpeg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\keys.cpp
# End Source File
# Begin Source File

SOURCE=.\src\main.cpp
# End Source File
# Begin Source File

SOURCE=.\src\mainmenu.cpp
# End Source File
# Begin Source File

SOURCE=.\src\map.cpp
# End Source File
# Begin Source File

SOURCE=.\src\map_pathfind.cpp
# End Source File
# Begin Source File

SOURCE=.\src\minimap.cpp
# End Source File
# Begin Source File

SOURCE=.\src\minimap.h
# End Source File
# Begin Source File

SOURCE=.\src\multiplay.cpp
# End Source File
# Begin Source File

SOURCE=.\src\netsock.cpp
# End Source File
# Begin Source File

SOURCE=.\src\packet.cpp
# End Source File
# Begin Source File

SOURCE=.\src\pck.cpp
# End Source File
# Begin Source File

SOURCE=.\src\persist.cpp
# End Source File
# Begin Source File

SOURCE=.\src\pfxopen.cpp
# End Source File
# Begin Source File

SOURCE=.\src\place.cpp
# End Source File
# Begin Source File

SOURCE=.\src\platoon.cpp
# End Source File
# Begin Source File

SOURCE=.\src\soldier.cpp
# End Source File
# Begin Source File

SOURCE=.\src\sound.cpp
# End Source File
# Begin Source File

SOURCE=.\src\spk.cpp
# End Source File
# Begin Source File

SOURCE=.\src\terrapck.cpp
# End Source File
# Begin Source File

SOURCE=.\src\units.cpp
# End Source File
# Begin Source File

SOURCE=.\src\video.cpp
# End Source File
# Begin Source File

SOURCE=.\src\wind.cpp
# End Source File
# Begin Source File

SOURCE=.\src\word.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\src\about.h
# End Source File
# Begin Source File

SOURCE=.\src\bullet.h
# End Source File
# Begin Source File

SOURCE=.\src\cell.h
# End Source File
# Begin Source File

SOURCE=.\src\config.h
# End Source File
# Begin Source File

SOURCE=.\src\connect.h
# End Source File
# Begin Source File

SOURCE=.\src\crc32.h
# End Source File
# Begin Source File

SOURCE=.\src\dirty.h
# End Source File
# Begin Source File

SOURCE=.\src\editor.h
# End Source File
# Begin Source File

SOURCE=.\src\explo.h
# End Source File
# Begin Source File

SOURCE=.\src\font.h
# End Source File
# Begin Source File

SOURCE=.\src\global.h
# End Source File
# Begin Source File

SOURCE=.\src\icon.h
# End Source File
# Begin Source File

SOURCE=.\src\inventory.h
# End Source File
# Begin Source File

SOURCE=.\src\item.h
# End Source File
# Begin Source File

SOURCE=.\src\jpgalleg.h
# End Source File
# Begin Source File

SOURCE=.\src\keys.h
# End Source File
# Begin Source File

SOURCE=.\src\ldw.h
# End Source File
# Begin Source File

SOURCE=.\src\mainmenu.h
# End Source File
# Begin Source File

SOURCE=.\src\map.h
# End Source File
# Begin Source File

SOURCE=.\src\multiplay.h
# End Source File
# Begin Source File

SOURCE=.\src\netsock.h
# End Source File
# Begin Source File

SOURCE=.\src\packet.h
# End Source File
# Begin Source File

SOURCE=.\src\pck.h
# End Source File
# Begin Source File

SOURCE=.\src\persist.h
# End Source File
# Begin Source File

SOURCE=.\src\pfxopen.h
# End Source File
# Begin Source File

SOURCE=.\src\place.h
# End Source File
# Begin Source File

SOURCE=.\src\platoon.h
# End Source File
# Begin Source File

SOURCE=.\src\soldier.h
# End Source File
# Begin Source File

SOURCE=.\src\sound.h
# End Source File
# Begin Source File

SOURCE=.\src\spk.h
# End Source File
# Begin Source File

SOURCE=.\src\terrapck.h
# End Source File
# Begin Source File

SOURCE=.\src\units.h
# End Source File
# Begin Source File

SOURCE=.\src\version.h
# End Source File
# Begin Source File

SOURCE=.\src\video.h
# End Source File
# Begin Source File

SOURCE=.\src\wind.h
# End Source File
# Begin Source File

SOURCE=.\src\word.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\Seccast.ico
# End Source File
# Begin Source File

SOURCE=.\ufo2000.h
# End Source File
# Begin Source File

SOURCE=.\ufo2000.rc
# End Source File
# End Group
# End Target
# End Project
