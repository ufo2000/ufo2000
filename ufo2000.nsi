; ufo2000.nsi
;
; This script was made for UFO2000's installer, by Daniel "SupSuper" Albano (super@portugalmail.pt).
; UFO2000 is a massive multiplayer game based on XCOM saga: http://ufo2000.sourceforge.net

;--------------------------------
;Include Modern UI

	!include "MUI.nsh"

;--------------------------------
;General

	Name "UFO2000"
	OutFile "ufo2000-%VERSION_ID%.exe"
	SetCompressor lzma
	SetCompressorDictSize 4

	XPStyle on

	InstallDir $PROGRAMFILES\UFO2000
	InstallDirRegKey HKLM "Software\UFO2000" "Install_Dir"

;--------------------------------
;Interface Configuration

	!define MUI_HEADERIMAGE
	!define MUI_HEADERIMAGE_BITMAP arts\installer-logo.bmp
	!define MUI_HEADERIMAGE_UNBITMAP arts\installer-logo.bmp
	!define MUI_ABORTWARNING
	
;--------------------------------
;Pages

	!insertmacro MUI_PAGE_WELCOME
	!insertmacro MUI_PAGE_COMPONENTS
	!insertmacro MUI_PAGE_DIRECTORY
	!insertmacro MUI_PAGE_INSTFILES
	!define MUI_FINISHPAGE_TEXT \
	"UFO2000 has been installed in your computer.\r\n\nTo complete the installation, copy all the files from \
	your X-Com: UFO Defense installation to the XCOM folder inside UFO2000's folder (or to XCOMDEMO if you have the demo). \
	The same applies to X-Com: Terror from the Deep and the TFTD/TFTDDEMO folders.\r\n\nClick Finish to close this wizard."
	!insertmacro MUI_PAGE_FINISH
	!insertmacro MUI_UNPAGE_CONFIRM
	!insertmacro MUI_UNPAGE_INSTFILES
  
;--------------------------------
;Languages
 
	!insertmacro MUI_LANGUAGE "English"

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
	
	SetOutPath $INSTDIR\newunits
	
	File newunits\*
	
	SetOutPath $INSTDIR\script
	
	File script\*

	WriteRegStr HKLM SOFTWARE\UFO2000 "Install_Dir" "$INSTDIR"
  
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\UFO2000" "DisplayName" "UFO2000"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\UFO2000" "UninstallString" '"$INSTDIR\uninstall.exe"'
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\UFO2000" "NoModify" 1
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\UFO2000" "NoRepair" 1
	WriteUninstaller "uninstall.exe"
  
SectionEnd

Section "Start Menu Shortcuts" SMSec

	CreateDirectory "$SMPROGRAMS\UFO2000"
	CreateShortCut "$SMPROGRAMS\UFO2000\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
	CreateShortCut "$SMPROGRAMS\UFO2000\UFO2000.lnk" "$INSTDIR\ufo2000.exe" "" "$INSTDIR\ufo2000.exe" 0
	CreateShortCut "$SMPROGRAMS\UFO2000\UFO2000 server.lnk" "$INSTDIR\ufo2000-srv.exe" "" "$INSTDIR\ufo2000-srv.exe" 0
	CreateShortCut "$SMPROGRAMS\UFO2000\Readme - English.lnk" "$INSTDIR\readme_en.txt" "" "$INSTDIR\readme_en.txt" 0
	CreateShortCut "$SMPROGRAMS\UFO2000\Readme - Spanish.lnk" "$INSTDIR\readme_es.txt" "" "$INSTDIR\readme_es.txt" 0
	CreateShortCut "$SMPROGRAMS\UFO2000\Readme - Polish.lnk" "$INSTDIR\readme_pl.txt" "" "$INSTDIR\readme_pl.txt" 0
	CreateShortCut "$SMPROGRAMS\UFO2000\Readme - French.lnk" "$INSTDIR\readme_fr.txt" "" "$INSTDIR\readme_fr.txt" 0
	CreateShortCut "$SMPROGRAMS\UFO2000\Manual - Russian.lnk" "$INSTDIR\readme_ru.html" "" "$INSTDIR\readme_ru.html" 0
  
SectionEnd

Section "Desktop Shortcut" DesktopSec

	CreateShortCut "$DESKTOP\UFO2000.lnk" "$INSTDIR\ufo2000.exe" "" "$INSTDIR\ufo2000.exe" 0
  
SectionEnd

;--------------------------------
;Descriptions

	; Language strings
	LangString DESC_MainSec ${LANG_ENGLISH} "All of UFO2000's required files."
	LangString DESC_SMSec ${LANG_ENGLISH} "Creates Start Menu shortcuts for UFO2000 and its documentation."
	LangString DESC_DesktopSec ${LANG_ENGLISH} "Creates a desktop shortcut for UFO2000."

	; Assign language strings to sections
	!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
	!insertmacro MUI_DESCRIPTION_TEXT ${MainSec} $(DESC_MainSec)
	!insertmacro MUI_DESCRIPTION_TEXT ${SMSec} $(DESC_SMSec)
	!insertmacro MUI_DESCRIPTION_TEXT ${DesktopSec} $(DESC_DesktopSec)
	!insertmacro MUI_FUNCTION_DESCRIPTION_END
 
;--------------------------------
; Uninstaller

Section "Uninstall"
  
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\UFO2000"
	DeleteRegKey HKLM SOFTWARE\UFO2000

	SetOutPath $INSTDIR
  
	Delete $INSTDIR\*
	Delete $INSTDIR\arts\*
	Delete $INSTDIR\init-scripts\*
	Delete $INSTDIR\newmaps\*
	Delete $INSTDIR\newunits\*
	Delete $INSTDIR\script\*
	
	Delete $INSTDIR\uninstall.exe
	
	Delete "$SMPROGRAMS\UFO2000\*.*"
	Delete "$DESKTOP\UFO2000.lnk"

	RMDir "$SMPROGRAMS\UFO2000"
	RMDir "$INSTDIR\arts"
	RMDir "$INSTDIR\init-scripts"
	RMDir "$INSTDIR\newmaps"
	RMDir "$INSTDIR\newmusic"
	RMDir "$INSTDIR\newunits"
	RMDir "$INSTDIR\script"
	RMDir "$INSTDIR\TFTD"
	RMDir "$INSTDIR\TFTDDEMO"
	RMDir "$INSTDIR\XCOM"
	RMDir "$INSTDIR\XCOMDEMO"
	RMDir "$INSTDIR"

SectionEnd
