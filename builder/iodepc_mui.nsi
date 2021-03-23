; iode.nsi
;-------------------------------

!ifndef VERSION
  !define VERSION 'Planning corner IODE 6.33'
!endif

!include "FileAssociation.nsh"
!include "MUI.nsh"
!include Sections.nsh


;Pages
  !insertmacro MUI_PAGE_WELCOME
  !insertmacro MUI_PAGE_LICENSE "iode_pc.rtf"  
  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_INSTFILES
 
    # These indented statements modify settings for MUI_PAGE_FINISH
    !define MUI_FINISHPAGE_NOAUTOCLOSE
    !define MUI_FINISHPAGE_RUN
    !define MUI_FINISHPAGE_RUN_CHECKED
    !define MUI_FINISHPAGE_RUN_TEXT "Start the Planning Corner"
    !define MUI_FINISHPAGE_RUN_FUNCTION "LaunchLink"
    !define MUI_FINISHPAGE_SHOWREADME ""
    !define MUI_FINISHPAGE_SHOWREADME_NOTCHECKED
    !define MUI_FINISHPAGE_SHOWREADME_TEXT "Create Desktop Shortcut"
    !define MUI_FINISHPAGE_SHOWREADME_FUNCTION finishpageaction  
  !insertmacro MUI_PAGE_FINISH
 
;Languages
!insertmacro MUI_LANGUAGE "English"

; The name of the installer
Name "iODE Planning Corner Updater"

; The Icon of the installer
Icon c:\usr\iode_src\pc\bricks.ico

; The file to write
OutFile "PlanningCornerInstaller.exe"

; The default installation directory
InstallDir $DOCUMENTS\PlanningCorner

; Registry key to check for directory (so if you install again, it will
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\IODEPC" "Install_Dir"


; Request application privileges for Windows Vista
RequestExecutionLevel user

; Pages

;Page components
;Page directory
;Page instfiles


;UninstPage uninstConfirm
;UninstPage instfiles
LangString MUI_UNTEXT_UNINSTALLING_TITLE ${LANG_ENGLISH} "${VERSION} UnInstaller."
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

;--------------------------------

; The stuff to install
Section "IODE Planning Corner executable" SecExec

  SectionIn RO

  ; Set output path to the installation directory.
  SetOutPath $INSTDIR\bin

  ; Put file there
  File C:\usr\iode_src\pc\Release\Win32\PlanningCorner.exe
  File C:\usr\iode_src\pc\iode.msg

;
SectionEnd ; end the section

Section "Model ML2 and Setup files" 
  CreateShortCut "$INSTDIR\ML2 Planning Corner.lnk" "$INSTDIR\bin\PlanningCorner.exe" "$INSTDIR\ml2"

  ; Set output path to the installation directory.
  CreateDirectory "$INSTDIR\ml2"  
  SetOutPath $INSTDIR\ml2

  ; Put file there
  File c:\usr\iode_src\pc\ml2\pc.parms
  File c:\usr\iode_src\pc\ml2\intro.html
  File c:\usr\iode_src\pc\ml2\ml2init.rep
  File c:\usr\iode_src\pc\ml2\ml2u.eqs
  File c:\usr\iode_src\pc\ml2\ml2u.scl
  File c:\usr\iode_src\pc\ml2\ml2.cmt
  File c:\usr\iode_src\pc\ml2\ml2.tbl
  File c:\usr\iode_src\pc\ml2\ml2.lst
  File c:\usr\iode_src\pc\ml2\restbl1.rep
  File c:\usr\iode_src\pc\ml2\restbl2.rep
  File c:\usr\iode_src\pc\ml2\restbl3.rep
  File c:\usr\iode_src\pc\ml2\print.rep
  
  File /r c:\usr\iode_src\pc\ml2\docs

  CreateDirectory "$INSTDIR\ml2\data"
  SetOutPath $INSTDIR\ml2\data
  File c:\usr\iode_src\pc\ml2\data\ml2u.var  
  
SectionEnd ; end the section

Function LaunchLink
  ExecShell "" "$INSTDIR\ML2 Planning Corner.lnk"
FunctionEnd

Function finishpageaction
  CreateShortCut "$DESKTOP\ML2 Planning Corner.lnk" "$INSTDIR\bin\PlanningCorner.exe" "$INSTDIR\ml2"
FunctionEnd


LangString DESC_SecExec ${LANG_ENGLISH} "Iode Planning Corner Executables"

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SecExec} $(DESC_SecExec)
!insertmacro MUI_FUNCTION_DESCRIPTION_END
