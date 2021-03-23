; iode.nsi
;--------------------------------

!ifndef VERSION
  !define VERSION 'IODE6.36'
!endif

!include "FileAssociation.nsh"
!include "MUI.nsh"
!include Sections.nsh

!insertmacro MUI_LANGUAGE "English"

; The name of the installer
Name "TramoSeats Installer"

; The Icon of the installer
Icon c:\usr\iode_src\pc\bricks.ico

; The file to write
OutFile "WinTramoSeatsInstaller.exe"

; The default installation directory
InstallDir c:\iode

; Registry key to check for directory (so if you install again, it will
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\WinTramoSeats" "Install_Dir"


; Request application privileges for Windows Vista
RequestExecutionLevel admin


; Pages

;Page components
;Page directory
;Page instfiles

LangString MUI_TEXT_LICENSE_TITLE ${LANG_ENGLISH} "${VERSION} Installer."
!insertmacro MUI_PAGE_LICENSE "wintramoseats.rtf"

LangString MUI_TEXT_COMPONENTS_TITLE ${LANG_ENGLISH} "${VERSION} Installer."
!insertmacro MUI_PAGE_COMPONENTS

LangString MUI_TEXT_DIRECTORY_TITLE ${LANG_ENGLISH} "${VERSION} Installer."
!insertmacro MUI_PAGE_DIRECTORY

LangString MUI_TEXT_INSTFILES_TITLE ${LANG_ENGLISH} "${VERSION} Installer."
!insertmacro MUI_PAGE_INSTFILES

;UninstPage uninstConfirm
;UninstPage instfiles
LangString MUI_UNTEXT_UNINSTALLING_TITLE ${LANG_ENGLISH} "${VERSION} UnInstaller."
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

;--------------------------------

; The stuff to install
Section "WinTramoSeats executable" SecExec

  SectionIn RO
  SetShellVarContext all

  ; Set output path to the installation directory.
  SetOutPath $INSTDIR

  ; Put file there
  File /r C:\usr\iode_src\tramoseats\WinTramoSeats\WinTramoSeats\bin\Release\*.*
  File WinTramoSeats.rtf

  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\WinTramoSeats "Install_Dir" "$INSTDIR"

SectionEnd ; end the section

; Optional section (can be disabled by the user)

Section "Start Menu Shortcuts" SecStartMenu
  SetShellVarContext all

  CreateDirectory "$SMPROGRAMS\IODE"
  CreateShortCut "$SMPROGRAMS\IODE\WinTramoSeats.lnk" "$INSTDIR\WinTramoSeats.exe" "" "$INSTDIR\WinTramoSeats.exe" 0
  CreateShortCut "$SMPROGRAMS\IODE\WinTramoSeats Help.lnk" "$INSTDIR\WinTramoSeats.rtf" "" "$INSTDIR\WinTramoSeats.rtf" 0

SectionEnd


LangString DESC_SecExec ${LANG_ENGLISH} "WinTramoSeats"
LangString DESC_SecStartMenu ${LANG_ENGLISH} "Add WinTramoSeats shortcut to the Start Menu"

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SecExec} $(DESC_SecExec)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecStartMenu} $(DESC_SecStartMenu)
!insertmacro MUI_FUNCTION_DESCRIPTION_END
