; UFO2000 NSIS Installer v1.3.1 (see bottom for version history)
;
; This script was made by Daniel "SupSuper" Albano (supsuper@mail.pt) with Venis IX.
; UFO2000 is a massive multiplayer game based on XCOM saga: http://ufo2000.sourceforge.net

;--------------------------------
;Include Modern UI

	!include "MUI.nsh"
	!addPluginDir  "../nsis-plugins"
	!addincludedir "../nsis-plugins"
;--------------------------------
;General

	Name "UFO2000"
	OutFile "ufo2000-%VERSION_ID%.exe"
	SetCompressor lzma
	SetCompressorDictSize 4

	XPStyle on

	InstallDir $PROGRAMFILES\UFO2000
	InstallDirRegKey HKLM "Software\UFO2000" "Install_Dir"
	
	Var STARTMENU_FOLDER
	
;--------------------------------
;Initialization

Function .onInit
	!insertmacro MUI_INSTALLOPTIONS_EXTRACT "xcom_folder.ini"
FunctionEnd

;--------------------------------
;Interface Configuration

	!define MUI_HEADERIMAGE
	!define MUI_HEADERIMAGE_BITMAP arts\installer-logo.bmp
	!define MUI_HEADERIMAGE_UNBITMAP arts\installer-logo.bmp
	!define MUI_WELCOMEFINISHPAGE_BITMAP arts\installer-welcome.bmp
	!define MUI_UNWELCOMEFINISHPAGE_BITMAP arts\installer-welcome.bmp
	!define MUI_FINISHPAGE_RUN $INSTDIR\ufo2000.exe
	!define MUI_UNCONFIRMPAGE_TEXT_TOP "UFO2000 will be uninstalled from the following folder.\
	Click Uninstall to start the uninstallation. NOTE: If you selected 'Download X-Com Demo\
	' when installing, that demo will also be uninstalled."
	!define MUI_ABORTWARNING
	
;--------------------------------
;Pages

	!insertmacro MUI_PAGE_WELCOME
	!insertmacro MUI_PAGE_COMPONENTS
	!insertmacro MUI_PAGE_DIRECTORY
	!define MUI_STARTMENUPAGE_REGISTRY_ROOT HKLM 
	!define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\UFO2000" 
	!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"
	!insertmacro MUI_PAGE_STARTMENU Application $STARTMENU_FOLDER
	Page custom XComFolder
	!insertmacro MUI_PAGE_INSTFILES
	!insertmacro MUI_PAGE_FINISH
	!insertmacro MUI_UNPAGE_WELCOME
	!insertmacro MUI_UNPAGE_CONFIRM
	!insertmacro MUI_UNPAGE_INSTFILES
	!insertmacro MUI_UNPAGE_FINISH
  
;--------------------------------
;Languages
 
	!insertmacro MUI_LANGUAGE "English"
	!include "ZipDLL.nsh"

;--------------------------------
;Custom pages

;(uses InstallOptions.dll)

LangString TEXT_XCOMFOLDER_TITLE ${LANG_ENGLISH} "Choose X-Com Location"
LangString TEXT_XCOMFOLDER_SUBTITLE ${LANG_ENGLISH} "Choose the folder where you have X-Com installed in."

Function XComFolder
	!insertmacro MUI_HEADER_TEXT "$(TEXT_XCOMFOLDER_TITLE)" "$(TEXT_XCOMFOLDER_SUBTITLE)"
	!insertmacro MUI_INSTALLOPTIONS_DISPLAY "xcom_folder.ini"
	ReadINIStr $R0 "$PLUGINSDIR\xcom_folder.ini" "Field 3" "State"
	ReadINIStr $R1 "$PLUGINSDIR\xcom_folder.ini" "Field 5" "State"
	ReadINIStr $R2 "$PLUGINSDIR\xcom_folder.ini" "Field 7" "State"
	ReadINIStr $R3 "$PLUGINSDIR\xcom_folder.ini" "Field 9" "State"
FunctionEnd

;--------------------------------
;Version Information

	VIProductVersion "%VERSION_ID%"
	VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "UFO2000"
	VIAddVersionKey /LANG=${LANG_ENGLISH} "Comments" "Massive multiplayer game based on XCOM saga"
	VIAddVersionKey /LANG=${LANG_ENGLISH} "CompanyName" "UFO2000 Development Team"
	VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "Alexander Ivanov aka Sanami"
	VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "UFO2000"
	VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "%VERSION_ID%"
	
;--------------------------------
;Installer Sections

Section "UFO2000 (required)" MainSec

	SectionIn RO
  
	CreateDirectory "$INSTDIR\arts"
	CreateDirectory "$INSTDIR\init-scripts"
	CreateDirectory "$INSTDIR\newmaps"
	CreateDirectory "$INSTDIR\newmusic"
	CreateDirectory "$INSTDIR\newunits"
	CreateDirectory "$INSTDIR\script"
	CreateDirectory "$INSTDIR\TFTD"
	CreateDirectory "$INSTDIR\TFTDDEMO"
	CreateDirectory "$INSTDIR\XCOM"
	CreateDirectory "$INSTDIR\XCOMDEMO"

	SetOutPath $INSTDIR
  
	File "AUTHORS"
	File "ChangeLog"
	File "COPYING"
	File "INSTALL"
	File "readme_en.txt"
	File "readme_es.txt"
	File "readme_fi.txt"
	File "readme_fr.txt"
	File "readme_pl.txt"
	File "readme_ru.html"
	File "soundmap.xml"
	File "armoury.default.set"
	File "geodata.default.dat"
	File "items.default.dat"
	File "soldier.default.dat"
	File "ufo2000.dat"
	File "keyboard.dat"
	File "ufo2000.exe"
	File "ufo2000-srv.exe"
	File "ufo2000.default.ini"
	File "ufo2000-srv.conf"
	
	SetOutPath $INSTDIR\arts
	
	File "arts\empty.spk"
	File "arts\menu.jpg"
	File "arts\text_back.jpg"
  
	SetOutPath $INSTDIR\init-scripts
	
	File init-scripts\*
	
	SetOutPath $INSTDIR\newmaps
	
	File newmaps\*
	
	SetOutPath $INSTDIR\newmusic

	File ..\newmusic\*
	
	SetOutPath $INSTDIR\newunits
	
	File newunits\*
	
	SetOutPath $INSTDIR\script
	
	File script\*
	
	IfFileExists $R0 xcom xcom_no
	xcom: CopyFiles $R0\*.* $INSTDIR\XCOM
	xcom_no:
	
	IfFileExists $R1 xcomdemo xcomdemo_no
	xcomdemo: CopyFiles $R1\*.* $INSTDIR\XCOMDEMO
	xcomdemo_no:
	
	IfFileExists $R2 tftd tftd_no
	tftd: CopyFiles $R2\*.* $INSTDIR\TFTD
	tftd_no:
	
	IfFileExists $R3 tftddemo tftddemo_no
	tftddemo: CopyFiles $R3\*.* $INSTDIR\TFTDDEMO
	tftddemo_no:
	
	!insertmacro MUI_STARTMENU_WRITE_BEGIN Application

		CreateDirectory "$SMPROGRAMS\$STARTMENU_FOLDER"
		CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
		CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\UFO2000.lnk" "$INSTDIR\ufo2000.exe" "" "$INSTDIR\ufo2000.exe" 0
		CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\UFO2000 server.lnk" "$INSTDIR\ufo2000-srv.exe" "" "$INSTDIR\ufo2000-srv.exe" 0
		CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Readme - English.lnk" "$INSTDIR\readme_eng.txt" "" "$INSTDIR\readme_eng.txt" 0
		CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Readme - Spanish.lnk" "$INSTDIR\readme_es.txt" "" "$INSTDIR\readme_es.txt" 0
		CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Readme - Polish.lnk" "$INSTDIR\readme_pl.txt" "" "$INSTDIR\readme_pl.txt" 0
		CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Readme - French.lnk" "$INSTDIR\readme_fr.txt" "" "$INSTDIR\readme_fr.txt" 0
		CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Manual - Russian.lnk" "$INSTDIR\readme_ru.html" "" "$INSTDIR\readme_ru.html" 0
  
	!insertmacro MUI_STARTMENU_WRITE_END
	
	WriteRegStr HKLM SOFTWARE\UFO2000 "Install_Dir" "$INSTDIR"
  
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\UFO2000" "DisplayName" "UFO2000"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\UFO2000" "UninstallString" '"$INSTDIR\uninstall.exe"'
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\UFO2000" "NoModify" 1
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\UFO2000" "NoRepair" 1
	WriteUninstaller "uninstall.exe"
  
SectionEnd

Section "Desktop Shortcut" DesktopSec

	CreateShortCut "$DESKTOP\UFO2000.lnk" "$INSTDIR\ufo2000.exe" "" "$INSTDIR\ufo2000.exe" 0
  
SectionEnd

Section /o "Download X-Com Demo" XComDemoSec

	;(uses NSISdl.dll)
	NSISdl::download "http://freelancer.ag.ru/demo/xcomdemo.zip" "$TEMP\xcomdemo.zip"
	Pop $0
	StrCmp $0 success success1
		SetDetailsView show
		DetailPrint "download failed: $0"
		Abort
	success1:

	;(uses ZipDLL.dll)
	!insertmacro ZIPDLL_EXTRACT "$TEMP\xcomdemo.zip" "$TEMP" "XCOM.EXE"
	Pop $0
	StrCmp $0 success sucess2
		SetDetailsView show
		DetailPrint "unzipping failed: $0"
		Abort
	sucess2:
	!insertmacro ZIPDLL_EXTRACT "$TEMP\XCOM.EXE" "$INSTDIR" "<ALL>"
	Pop $0
	StrCmp $0 success sucess3
		SetDetailsView show
		DetailPrint "unzipping failed: $0"
		Abort
	sucess3:

	Delete "$TEMP\xcomdemo.zip"
	Delete "$TEMP\XCOM.EXE"

SectionEnd

;--------------------------------
;Descriptions

	;Language strings
	LangString DESC_MainSec ${LANG_ENGLISH} "All of UFO2000's required files."
	LangString DESC_DesktopSec ${LANG_ENGLISH} "Creates a desktop shortcut for UFO2000."
	LangString DESC_XComDemoSec ${LANG_ENGLISH} "Downloads and installs X-Com: Terran Defense Demo.\
	Use this if you don't have any X-Com game installed. Download size: 1,1MB."

	;Assign language strings to sections
	!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
	!insertmacro MUI_DESCRIPTION_TEXT ${MainSec} $(DESC_MainSec)
	!insertmacro MUI_DESCRIPTION_TEXT ${DesktopSec} $(DESC_DesktopSec)
	!insertmacro MUI_DESCRIPTION_TEXT ${XComDemoSec} $(DESC_XComDemoSec)
	!insertmacro MUI_FUNCTION_DESCRIPTION_END
 
;--------------------------------
; Uninstaller

Section "Uninstall"
  
	ReadRegStr $STARTMENU_FOLDER HKLM "Software\UFO2000" "Start Menu Folder"
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\UFO2000"
	DeleteRegKey HKLM "Software\UFO2000"

	Delete "$DESKTOP\UFO2000.lnk"
	RMDir /r "$SMPROGRAMS\$STARTMENU_FOLDER"
	RMDir /r "$INSTDIR"

SectionEnd

; Version History
;
; 1.3.1 (5th Mar 2004)
;
; - "X-Com Demo wasn't correctly installed if UFO2000 was in a long-name folder" bug fixed
; - "Some stuff isn't deleted when uninstalling UFO2000" bug fixed
; - Made "Download X-Com Demo" section unselected by default
; - Changed compression to lzma
;
; 1.3 (4th Mar 2004)
;
; - Added "Download X-Com Demo" section
; - Enhanced the uninstaller
; - Replaced plugin Start Menu page with MUI's Start Menu page
; - Fixed some text strings
;
; 1.2 (2nd Mar 2004)
;
; - Added custom "X-Com Folders" page
; - Replaced Start Menu section with custom Start Menu page
; - Added custom Welcome/Finish bitmap
; - Added "Run application" checkbox to Finish screen
;
; 1.11 (28th Fev 2004)
;
; - Added Note to end of installer
; - Installer was published
;
; 1.1 (22th Fev 2004)
;
; - Upgraded to Modern UI
; - Added custom header bitmap
; - Added Welcome/Finish pages
;
; 1.0 (21th Fev 2004)
;
; - Created installer (NSIS 2.0, Basic UI)
