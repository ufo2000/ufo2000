; UFO2000 NSIS Installer v1.4.2 (see bottom for version history)
;
; This script was made by Daniel "SupSuper" Albano (supsuper@gmail.com) with Venis IX.
; UFO2000 is a massive multiplayer game based on XCOM saga: http://ufo2000.sourceforge.net

;--------------------------------
;Includes

	!define GAME_NAME "UFO2000"
	!define GAME_VERSION "installer"

	!addPluginDir  "..\nsis-plugins"
	!addincludedir "..\nsis-plugins"
	!include "MUI.nsh"
	!include "zipdll.nsh"
	!include "Sections.nsh"
	!include "defines.nsh"
	
;--------------------------------
;General

	Name "${GAME_NAME}"
	OutFile "ufo2000-${GAME_VERSION}.exe"
	SetCompressor lzma
	SetCompressorDictSize 4

	XPStyle on

	InstallDir "$PROGRAMFILES\${GAME_NAME}"
	InstallDirRegKey HKLM "Software\${GAME_NAME}" "Install_Dir"
	
	Var STARTMENU_FOLDER
	Var XCOM_FOLDER
	Var XCOMDEMO_FOLDER
	Var TFTD_FOLDER
	Var TFTDDEMO_FOLDER
	Var INST_TYPE
	
;--------------------------------
;Reserve Files
  
  ;These files should be inserted before other files in the data block
  ;Keep these lines before any File command
  ;Only for solid compression (by default, solid compression is enabled for BZIP2 and LZMA)
	
	ReserveFile "select_option.ini"
	ReserveFile "xcom_folder.ini"
	ReserveFile "demo_select.ini"
	ReserveFile "readme_select.ini"
	!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS

;--------------------------------
;Initialization

Function .onInit
	!insertmacro MUI_INSTALLOPTIONS_EXTRACT "select_option.ini"
	!insertmacro MUI_INSTALLOPTIONS_EXTRACT "xcom_folder.ini"
	!insertmacro MUI_INSTALLOPTIONS_EXTRACT "demo_select.ini"
	!insertmacro MUI_INSTALLOPTIONS_EXTRACT "readme_select.ini"
	
	System::Call 'kernel32::CreateMutexA(i 0, i 0, t "myMutex") i .r1 ?e' 

  Pop $R0 

  StrCmp $R0 0 +3 

    MessageBox MB_OK "The installer is already running." 

    Abort
FunctionEnd

;--------------------------------
;Interface Configuration

	!define MUI_HEADERIMAGE
	!define MUI_HEADERIMAGE_BITMAP arts\installer-logo.bmp
	!define MUI_HEADERIMAGE_UNBITMAP arts\installer-logo.bmp
	!define MUI_WELCOMEFINISHPAGE_BITMAP arts\installer-welcome.bmp
	!define MUI_UNWELCOMEFINISHPAGE_BITMAP arts\installer-welcome.bmp
	!define MUI_FINISHPAGE_RUN $INSTDIR\ufo2000.exe
	!define MUI_UNCONFIRMPAGE_TEXT_TOP "${GAME_NAME} will be uninstalled from the following folder. \
	Click Uninstall to start the uninstallation.$\r$\nWARNING: All files and folders in the ${GAME_NAME} \
	folder will be deleted, including any downloaded X-Com/TFTD demos."
	!define MUI_ABORTWARNING
	
;--------------------------------
;Pages

 Function CheckDirectory
	ReadRegStr $0 HKLM "Software\${GAME_NAME}" "Install_Dir"
	StrCmp $0 $INSTDIR upgrade fresh
	upgrade: StrCpy $INST_TYPE "upgrade"
	Goto end
	fresh: StrCpy $INST_TYPE "fresh"
	IfFileExists $INSTDIR\*.* error end
	error: MessageBox MB_OK|MB_ICONSTOP|MB_DEFBUTTON1 "The installation folder is in use!"
	Abort
	end:
FunctionEnd
 
  !insertmacro MUI_PAGE_WELCOME
	!insertmacro MUI_PAGE_LICENSE gnu.txt
	!insertmacro MUI_PAGE_COMPONENTS
	!define MUI_PAGE_CUSTOMFUNCTION_LEAVE CheckDirectory
  !insertmacro MUI_PAGE_DIRECTORY
	Page custom SelectOption
	Page custom SearchXcom
	Page custom XComFolder
	Page custom DemoSelect
	!define MUI_STARTMENUPAGE_REGISTRY_ROOT HKLM 
  !define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\${GAME_NAME}" 
  !define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"
	!insertmacro MUI_PAGE_STARTMENU Application $STARTMENU_FOLDER
	Page custom ReadmeSelect
	!insertmacro MUI_PAGE_INSTFILES
	!insertmacro MUI_PAGE_FINISH
	!insertmacro MUI_UNPAGE_WELCOME
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
	!insertmacro MUI_UNPAGE_FINISH
  
;--------------------------------
;Languages
 
	!insertmacro MUI_LANGUAGE "English"

;--------------------------------
; GetParent
; input, top of stack  (e.g. C:\Program Files\Poop)
; output, top of stack (replaces, with e.g. C:\Program Files)
; modifies no other variables.
;
; Usage:
;   Push "C:\Program Files\Directory\Whatever"
;   Call GetParent
;   Pop $R0
;   at this point $R0 will equal "C:\Program Files\Directory"

Function GetParent

  Exch $R0
  Push $R1
  Push $R2
  Push $R3
  
  StrCpy $R1 0
  StrLen $R2 $R0
  
  loop:
    IntOp $R1 $R1 + 1
    IntCmp $R1 $R2 get 0 get
    StrCpy $R3 $R0 1 -$R1
    StrCmp $R3 "\" get
  Goto loop
  
  get:
    StrCpy $R0 $R0 -$R1
    
    Pop $R3
    Pop $R2
    Pop $R1
    Exch $R0
    
FunctionEnd

;Search function

Function FindFiles
  Exch $R5 # callback function
  Exch 
  Exch $R4 # file name
  Exch 2
  Exch $R0 # directory
  Push $R1
  Push $R2
  Push $R3
  Push $R6

  Push $R0 # first dir to search

  StrCpy $R3 1

  nextDir:
    Pop $R0
    IntOp $R3 $R3 - 1
    ClearErrors
    FindFirst $R1 $R2 "$R0\*.*"
    nextFile:
      StrCmp $R2 "." gotoNextFile
      StrCmp $R2 ".." gotoNextFile

      StrCmp $R2 $R4 0 isDir
        Push "$R0\$R2"
        Call $R5
        Pop $R6
        StrCmp $R6 "stop" 0 isDir
          loop:
            StrCmp $R3 0 done
            Pop $R0
            IntOp $R3 $R3 - 1
            Goto loop

      isDir:
        IfFileExists "$R0\$R2\*.*" 0 gotoNextFile
          IntOp $R3 $R3 + 1
          Push "$R0\$R2"

  gotoNextFile:
    FindNext $R1 $R2
    IfErrors 0 nextFile

  done:
    FindClose $R1
    StrCmp $R3 0 0 nextDir

  Pop $R6
  Pop $R3
  Pop $R2
  Pop $R1
  Pop $R0
  Pop $R5
  Pop $R4
FunctionEnd

!macro CallFindFiles DIR FILE CBFUNC
Push "${DIR}"
Push "${FILE}"
Push $0
GetFunctionAddress $0 "${CBFUNC}"
Exch $0
Call FindFiles
!macroend

Function SearchCallback
  Exch $0
	Push $0
	Call GetParent
	Pop $R0
	IfFileExists "$R0\maps\*.*" next1 end
	next1: IfFileExists "$R0\missdat\*.*" next2 end
	next2: IfFileExists "$R0\routes\*.*" next3 end
	next3: IfFileExists "$R0\sound\*.*" next4 end
	next4: IfFileExists "$R0\terrain\*.*" next5 end
	next5: IfFileExists "$R0\ufograph\*.*" next6 end
	next6: IfFileExists "$R0\units\*.*" success end
	success:
	IfFileExists "$R0\demolbm\*.*" demo demo_no
	demo: IfFileExists "$R0\state.rst" tftddemo xcomdemo
	demo_no: IfFileExists "$R0\ufointro\*.*" xcom tftd
	xcom: MessageBox MB_YESNOCANCEL|MB_DEFBUTTON1|MB_ICONQUESTION "X-Com was found in this folder:$\r$\n$R0$\r$\n$\r$\nIs this correct?" IDNO end IDCANCEL cancel
	StrCpy $XCOM_FOLDER $R0
	Goto check1
	xcomdemo: MessageBox MB_YESNOCANCEL|MB_DEFBUTTON1|MB_ICONQUESTION "X-Com Demo was found in this folder:$\r$\n$R0$\r$\n$\r$\nIs this correct?" IDNO end IDCANCEL cancel
	StrCpy $XCOMDEMO_FOLDER $R0
	Goto check1
	tftd: MessageBox MB_YESNOCANCEL|MB_DEFBUTTON1|MB_ICONQUESTION "Terror from the Deep was found in this folder:$\r$\n$R0$\r$\n$\r$\nIs this correct?" IDNO end IDCANCEL cancel
	StrCpy $TFTD_FOLDER $R0
	Goto check1
	tftddemo: MessageBox MB_YESNOCANCEL|MB_DEFBUTTON1|MB_ICONQUESTION "Terror from the Deep Demo was found in this folder:$\r$\n$R0$\r$\n$\r$\nIs this correct?" IDNO end IDCANCEL cancel
	StrCpy $TFTDDEMO_FOLDER $R0
		
	check1: StrCmp $XCOM_FOLDER "" check2
	StrCmp $TFTD_FOLDER "" check2 finish
	
	check2: StrCmp $XCOMDEMO_FOLDER "" check3
	StrCmp $TFTDDEMO_FOLDER "" check3 finish
	
	check3: StrCmp $XCOMDEMO_FOLDER "" check4
	StrCmp $TFTD_FOLDER "" check4 finish
	
	check4: StrCmp $XCOM_FOLDER "" check5
	StrCmp $TFTDDEMO_FOLDER "" check5 finish
	
	check5: StrCmp $TFTD_FOLDER "" check6
	StrCmp $XCOM_FOLDER "" check6 finish
	
	check6: StrCmp $TFTDDEMO_FOLDER "" check7
	StrCmp $XCOMDEMO_FOLDER "" check7 finish
	
	check7: StrCmp $TFTD_FOLDER "" check8
	StrCmp $XCOMDEMO_FOLDER "" check8 finish
	
	check8: StrCmp $TFTDDEMO_FOLDER "" end
	StrCmp $XCOM_FOLDER "" end finish
	
	finish: Push "stop"
	Goto end
	cancel:
	Push "stop"
	StrCpy $XCOM_FOLDER " "
	end:
FunctionEnd

;Custom pages

;(uses InstallOptions.dll)

LangString TEXT_SELOPT_TITLE ${LANG_ENGLISH} "How to Find X-Com"
LangString TEXT_SELOPT_SUBTITLE ${LANG_ENGLISH} "Select how can the installer find X-Com."
LangString TEXT_SEARCH_TITLE ${LANG_ENGLISH} "Search for X-Com"
LangString TEXT_SEARCH_SUBTITLE ${LANG_ENGLISH} "The installer is searching for installed X-Com games."
LangString TEXT_XCOMFOLDER_TITLE ${LANG_ENGLISH} "Choose X-Com Location"
LangString TEXT_XCOMFOLDER_SUBTITLE ${LANG_ENGLISH} "Choose the folder where you have X-Com installed in."
LangString TEXT_DEMOSEL_TITLE ${LANG_ENGLISH} "Download X-Com Demo"
LangString TEXT_DEMOSEL_SUBTITLE ${LANG_ENGLISH} "Choose which X-Com demos you want installed."
LangString TEXT_READSEL_TITLE ${LANG_ENGLISH} "Select Readmes"
LangString TEXT_READSEL_SUBTITLE ${LANG_ENGLISH} "Choose which readmes you want shortcuts to be created for."

Function SelectOption
	StrCmp $INST_TYPE "upgrade" success failed
	success: Abort
	failed:
	!insertmacro MUI_HEADER_TEXT "$(TEXT_SELOPT_TITLE)" "$(TEXT_SELOPT_SUBTITLE)"
  !insertmacro MUI_INSTALLOPTIONS_DISPLAY "select_option.ini"
FunctionEnd

Function SearchXcom
	StrCmp $INST_TYPE "upgrade" sucess fail
	sucess: Abort
	fail:
	!insertmacro MUI_INSTALLOPTIONS_READ $0 "select_option.ini" "Field 2" "State"
	StrCmp $0 "1" success failed
	failed: Abort
	success:
	!insertmacro MUI_HEADER_TEXT "$(TEXT_SEARCH_TITLE)" "$(TEXT_SEARCH_SUBTITLE)"
	;(uses Dialogs.dll)
	Dialogs::Folder $HWNDPARENT "Search Folder" "Select a folder to search in:" "C:\" ${VAR_0}
	StrCmp $0 "" end
	Push $0
	Exch $EXEDIR
	StrCpy $0 $EXEDIR
	Exch $EXEDIR
	!insertmacro CallFindFiles $0 "geodata" SearchCallback
	StrCmp $XCOM_FOLDER "" next reset
	next: StrCmp $XCOMDEMO_FOLDER "" xcom_err reset
	xcom_err: MessageBox MB_OK|MB_DEFBUTTON1|MB_ICONSTOP "X-Com wasn't found!"
	Call SelectOption
	reset: StrCmp $XCOM_FOLDER " " res end
	res: StrCpy $XCOM_FOLDER ""
	end:
FunctionEnd

Function XComFolder
	!insertmacro MUI_INSTALLOPTIONS_READ $0 "select_option.ini" "Field 4" "State"
	StrCmp $0 "1" success failed
	failed: Abort
	success:
  !insertmacro MUI_HEADER_TEXT "$(TEXT_XCOMFOLDER_TITLE)" "$(TEXT_XCOMFOLDER_SUBTITLE)"
  !insertmacro MUI_INSTALLOPTIONS_DISPLAY "xcom_folder.ini"
	!insertmacro MUI_INSTALLOPTIONS_READ $XCOM_FOLDER "xcom_folder.ini" "Field 3" "State"
	!insertmacro MUI_INSTALLOPTIONS_READ $XCOMDEMO_FOLDER "xcom_folder.ini" "Field 5" "State"
	!insertmacro MUI_INSTALLOPTIONS_READ $TFTD_FOLDER "xcom_folder.ini" "Field 7" "State"
	!insertmacro MUI_INSTALLOPTIONS_READ $TFTDDEMO_FOLDER "xcom_folder.ini" "Field 9" "State"
FunctionEnd


Function ReadmeSelect
  StrCpy $0 $STARTMENU_FOLDER 1
	StrCmp $0 ">" success failed
	success: Abort
	failed:
	!insertmacro MUI_HEADER_TEXT "$(TEXT_READSEL_TITLE)" "$(TEXT_READSEL_SUBTITLE)"
  !insertmacro MUI_INSTALLOPTIONS_DISPLAY "readme_select.ini"	
FunctionEnd

;--------------------------------
;Version Information

	VIProductVersion "0.${GAME_VERSION}"
	VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "${GAME_NAME}"
	VIAddVersionKey /LANG=${LANG_ENGLISH} "Comments" "Massive multiplayer game based on XCOM saga"
	VIAddVersionKey /LANG=${LANG_ENGLISH} "CompanyName" "UFO2000 Development Team"
	VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "Alexander Ivanov aka Sanami"
	VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "${GAME_NAME}"
	VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "0.${GAME_VERSION}"
	
;--------------------------------
;Installer Sections

Section "${GAME_NAME} (required)" MainSec

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
	File "readme_de.txt"
	File "readme_en.txt"
	File "readme_es.txt"
	File "readme_fr.txt"
	File "readme_pt.txt"
	File "readme_ru.html"
	File "soundmap.xml"
	File "squad.default.lua"
	File "ufo2000.dat"
	File "keyboard.dat"
	File "ufo2000.exe"
	File "ufo2000-srv.exe"
	File "ufo2000.default.ini"
	File "ufo2000-srv.conf"
	File "update_db.sql"
	
	SetOutPath $INSTDIR\arts
	
	File "arts\empty.spk"
	File "arts\menu.jpg"
	File "arts\text_back.jpg"
	File "arts\geoscape_texture.jpg"
  
	SetOutPath $INSTDIR\init-scripts
	
	File init-scripts\*
	
	SetOutPath $INSTDIR\newmaps
	File newmaps\*
	
	SetOutPath $INSTDIR\newmusic
	File ..\newmusic\*
	
	SetOutPath $INSTDIR\docs
	File docs\*

	SetOutPath $INSTDIR\extensions
	File extensions\*

	SetOutPath $INSTDIR\newunits
	File newunits\*
	
	SetOutPath $INSTDIR\script
	File script\*
	
	SetOutPath $INSTDIR\translations
	File translations\*

	SetOutPath $INSTDIR\fonts
	File fonts\*
	
	IfFileExists $XCOM_FOLDER xcom xcom_no
	xcom: CopyFiles $XCOM_FOLDER\*.* $INSTDIR\XCOM
	xcom_no:
	
	IfFileExists $XCOMDEMO_FOLDER xcomdemo xcomdemo_no
	xcomdemo: CopyFiles $XCOMDEMO_FOLDER\*.* $INSTDIR\XCOMDEMO
	xcomdemo_no:
	
	IfFileExists $TFTD_FOLDER tftd tftd_no
	tftd: CopyFiles $TFTD_FOLDER\*.* $INSTDIR\TFTD
	tftd_no:
	
	IfFileExists $TFTDDEMO_FOLDER tftddemo tftddemo_no
	tftddemo: CopyFiles $TFTDDEMO_FOLDER\*.* $INSTDIR\TFTDDEMO
	tftddemo_no:
	
	!insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    
    CreateDirectory "$SMPROGRAMS\$STARTMENU_FOLDER"
		!insertmacro MUI_INSTALLOPTIONS_READ $0 "readme_select.ini" "Field 3" "State"
		StrCmp $0 "1" en en1		
		en: CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Readme - English.lnk" "$INSTDIR\readme_en.txt"
		en1: !insertmacro MUI_INSTALLOPTIONS_READ $0 "readme_select.ini" "Field 9" "State"
		StrCmp $0 "1" sp sp1		
		sp: CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Readme - Spanish.lnk" "$INSTDIR\readme_es.txt"
		sp1: !insertmacro MUI_INSTALLOPTIONS_READ $0 "readme_select.ini" "Field 7" "State"
		StrCmp $0 "1" fr fr1
		fr: CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Readme - French.lnk" "$INSTDIR\readme_fr.txt"
		fr1: !insertmacro MUI_INSTALLOPTIONS_READ $0 "readme_select.ini" "Field 2" "State"
		StrCmp $0 "1" ru ru1
		ru: CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Russian Manual.lnk" "$INSTDIR\readme_ru.html"
		ru1: !insertmacro MUI_INSTALLOPTIONS_READ $0 "readme_select.ini" "Field 4" "State"
		StrCmp $0 "1" pt pt1
		pt: CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Readme - Portuguese.lnk" "$INSTDIR\readme_pt.txt"
		pt1: !insertmacro MUI_INSTALLOPTIONS_READ $0 "readme_select.ini" "Field 6" "State"
		StrCmp $0 "1" de de1
		de: CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Readme - German.lnk" "$INSTDIR\readme_de.txt"
		de1: CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Uninstall.lnk" "$INSTDIR\uninstall.exe"
		CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\${GAME_NAME}.lnk" "$INSTDIR\ufo2000.exe"
		CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\${GAME_NAME} server.lnk" "$INSTDIR\ufo2000-srv.exe"
  
  !insertmacro MUI_STARTMENU_WRITE_END
	
	WriteRegStr HKLM "SOFTWARE\${GAME_NAME}" "Install_Dir" "$INSTDIR"
  
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${GAME_NAME}" "DisplayName" "${GAME_NAME}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${GAME_NAME}" "UninstallString" '"$INSTDIR\uninstall.exe"'
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${GAME_NAME}" "NoModify" 1
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${GAME_NAME}" "NoRepair" 1
	WriteUninstaller "uninstall.exe"
  
SectionEnd

Section "Desktop Shortcut" DesktopSec

	CreateShortCut "$DESKTOP\${GAME_NAME}.lnk" "$INSTDIR\ufo2000.exe"
  
SectionEnd

Section /o -"XcomDemoSec" XcomDemoSec

	;(uses NSISdl.dll)
	NSISdl::download "http://ufo2000.lxnt.info/files/xcomdemo.zip" "$TEMP\xcomdemo.zip"
	Pop $0
	StrCmp $0 success success1
		SetDetailsView show
		DetailPrint "download failed: $0"
		Abort
	success1:

	;(uses ZipDLL.dll)
	!insertmacro ZIPDLL_EXTRACT "$TEMP\xcomdemo.zip" "$TEMP" "XCOM.EXE"
	Pop $0
	StrCmp $0 success success2
		SetDetailsView show
		DetailPrint "unzipping failed: $0"
		Abort
	success2:
	!insertmacro ZIPDLL_EXTRACT "$TEMP\XCOM.EXE" "$INSTDIR" "<ALL>"
	Pop $0
	StrCmp $0 success success3
		SetDetailsView show
		DetailPrint "unzipping failed: $0"
		Abort
	success3:

	Delete "$TEMP\xcomdemo.zip"
	Delete "$TEMP\XCOM.EXE"

SectionEnd

Section /o -"TFTDDemoSec" TFTDDemoSec

	;(uses NSISdl.dll)
	NSISdl::download "http://ufo2000.lxnt.info/files/terror.zip" "$TEMP\terror.zip"
	Pop $0
	StrCmp $0 success success1
		SetDetailsView show
		DetailPrint "download failed: $0"
		Abort
	success1:

	;(uses ZipDLL.dll)
	!insertmacro ZIPDLL_EXTRACT "$TEMP\terror.zip" "$TEMP" "TFTD.ZIP"
	Pop $0
	StrCmp $0 success success2
		SetDetailsView show
		DetailPrint "unzipping failed: $0"
		Abort
	success2:
	!insertmacro ZIPDLL_EXTRACT "$TEMP\TFTD.ZIP" "$INSTDIR\TFTDDEMO" "<ALL>"
	Pop $0
	StrCmp $0 success success3
		SetDetailsView show
		DetailPrint "unzipping failed: $0"
		Abort
	success3:

	Delete "$TEMP\terror.zip"
	Delete "$TEMP\TFTD.ZIP"

SectionEnd

Function DemoSelect
	!insertmacro MUI_INSTALLOPTIONS_READ $0 "select_option.ini" "Field 6" "State"
	StrCmp $0 "1" sucess fail
	fail: Abort
	sucess:
  !insertmacro MUI_HEADER_TEXT "$(TEXT_DEMOSEL_TITLE)" "$(TEXT_DEMOSEL_SUBTITLE)"
  !insertmacro MUI_INSTALLOPTIONS_DISPLAY "demo_select.ini"
	!insertmacro MUI_INSTALLOPTIONS_READ $0 "demo_select.ini" "Field 2" "State"
	StrCmp $0 "1" xcomdemo check
	xcomdemo: !insertmacro SelectSection ${XcomDemoSec}
	check: !insertmacro MUI_INSTALLOPTIONS_READ $0 "demo_select.ini" "Field 3" "State"
	StrCmp $0 "1" tftddemo end
	tftddemo: !insertmacro SelectSection ${TFTDDemoSec}
	end:
FunctionEnd

;--------------------------------
;Descriptions

	;Language strings
	LangString DESC_MainSec ${LANG_ENGLISH} "All of ${GAME_NAME}'s required files."
	LangString DESC_DesktopSec ${LANG_ENGLISH} "Creates a desktop shortcut for ${GAME_NAME}."

	;Assign language strings to sections
	!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
	!insertmacro MUI_DESCRIPTION_TEXT ${MainSec} $(DESC_MainSec)
	!insertmacro MUI_DESCRIPTION_TEXT ${DesktopSec} $(DESC_DesktopSec)
	!insertmacro MUI_FUNCTION_DESCRIPTION_END
 
;--------------------------------
; Uninstaller

Section "Uninstall"
  
	ReadRegStr $STARTMENU_FOLDER HKLM "Software\${GAME_NAME}" "Start Menu Folder"
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${GAME_NAME}"
	DeleteRegKey HKLM "Software\${GAME_NAME}"

	Delete "$DESKTOP\${GAME_NAME}.lnk"
	!insertmacro MUI_STARTMENU_GETFOLDER Application $R0
	RMDir /r "$SMPROGRAMS\$R0"
	RMDir /r "$INSTDIR"

SectionEnd

; Version History
;
; 1.4.2 (30th Oct 2004)
;
; - Fixed bug #0000095: "Option for installing only selected Readme-files"
; - Fixed bug #0000089: "Uninstaller deletes X-COM off computer"
; - Fixed some text and did some improvements.
;
; 1.4.1 (2nd Sep 2004)
;
; - Fixed bug #0000065: "Cancelling the search for X-Com files hangs up the installer."
; - Fixed bug #0000066: "Installer does not copy TFTD files correctly."
; - Fixed bug #0000046: "Search for x-com games installer option (was: Only Warehouse-maps)"
; - Did some cleanup on the Search X-Com code
;
; 1.4 (28th Jun 2004)
;
; - Added "Download TFTD Demo" section
; - Added GNU General Public License
; - Added check if multiple instances of the installer are running
; - Added feature for installer to search for X-Com games
; - Changed the installer structure to make it easier and simple
; - Updated uninstaller note
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
