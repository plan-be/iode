; iode.nsi
;--------------------------------

!ifndef VERSION
  !define VERSION 'IODE6.63'
!endif

!include "FileAssociation.nsh"
!include "MUI.nsh"
!include Sections.nsh

!insertmacro MUI_LANGUAGE "English"

; The name of the installer
Name "iODE update Updater"

; The Icon of the installer
Icon iode2.ico

; The file to write
OutFile "iode663upd.exe"

; The default installation directory
InstallDir c:\iode

; Registry key to check for directory (so if you install again, it will
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\IODE" "Install_Dir"


; Request application privileges for Windows Vista
RequestExecutionLevel user


; Pages

;Page components
;Page directory
;Page instfiles

LangString MUI_TEXT_LICENSE_TITLE ${LANG_ENGLISH} "${VERSION} Installer."
!insertmacro MUI_PAGE_LICENSE "iode_upd.rtf"

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
Section "IODE executable" SecExec

  SectionIn RO
  SetShellVarContext all

  ; Set output path to the installation directory.
  SetOutPath $INSTDIR

  ; Put file there
  File c:\usr\iode_src\dos\iode.exe
  File c:\usr\iode_src\dos\iode.scr
  File c:\usr\iode_src\cmd\iodecmd.exe
  File c:\usr\iode_src\cmd\iodecmd.scr
  File c:\usr\iode_src\man\readme.htm
  ;File c:\usr\iode_src\man\iode.hlp
  File c:\usr\iode_src\man\iode.cnt
  File c:\usr\iode_src\man\iode.chm
  File c:\usr\iode_src\man\iode.pdf
  File c:\usr\iode_src\dos\iode.ini

  ; Write the installation path into the registry
  ;WriteRegStr HKLM SOFTWARE\IODE "Install_Dir" "$INSTDIR"

  ; Write the uninstall keys for Windows
  ;WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\IODE" "DisplayName" "IODE 6"
  ;WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\IODE" "UninstallString" '"$INSTDIR\uninstall.exe"'
  ;WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\IODE" "NoModify" 1
  ;WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\IODE" "NoRepair" 1
  ;WriteUninstaller "uninstall.exe"

  ; Register extensions
  ;${registerExtension} "$INSTDIR\iode.exe" ".cmt" "IODE Comments"
  ;${registerExtension} "$INSTDIR\iode.exe" ".eqs" "IODE Equations"
  ;${registerExtension} "$INSTDIR\iode.exe" ".idt" "IODE Identities"
  ;${registerExtension} "$INSTDIR\iode.exe" ".lst" "IODE Lists"
  ;${registerExtension} "$INSTDIR\iode.exe" ".scl" "IODE Scalars"
  ;${registerExtension} "$INSTDIR\iode.exe" ".tbl" "IODE Tables"
  ;${registerExtension} "$INSTDIR\iode.exe" ".var" "IODE Ascii Variables"
  ;${registerExtension} "$INSTDIR\iode.exe" ".ac" "IODE Ascii Comments"
  ;${registerExtension} "$INSTDIR\iode.exe" ".ae" "IODE Ascii Equations"
  ;${registerExtension} "$INSTDIR\iode.exe" ".ai" "IODE Ascii Identities"
  ;${registerExtension} "$INSTDIR\iode.exe" ".al" "IODE Ascii Lists"
  ;${registerExtension} "$INSTDIR\iode.exe" ".as" "IODE Ascii Scalars"
  ;${registerExtension} "$INSTDIR\iode.exe" ".at" "IODE Ascii Tables"
  ;${registerExtension} "$INSTDIR\iode.exe" ".av" "IODE Ascii Variables"
;
SectionEnd ; end the section

; Optional section (can be disabled by the user)
Section "IODE themes and styles" ;No components page, name is not important

  ; Set output path to the installation directory.
  SetOutPath $INSTDIR

  ; Put file there
  File c:\usr\iode_src\dos\style.css
  File c:\usr\iode_src\dos\listing.gif
  File c:\usr\iode_src\dos\iode2i.gif
  File c:\usr\iode_src\dos\title.gif
  File c:\usr\iode_src\dos\default.thm
  File c:\usr\iode_src\dos\bw.thm
  File c:\usr\iode_src\dos\swf.ini
  
SectionEnd ; end the section


; Optional section (disabled by default)
Section /o "IODE Example Model"  SecExample

  ; Set output path to the installation directory.
  SetOutPath $INSTDIR\Example

  ; Put file there
  File c:\usr\iode_src\fun\fun.cmt
  File c:\usr\iode_src\fun\fun.eqs
  File c:\usr\iode_src\fun\fun.idt
  File c:\usr\iode_src\fun\fun.lst
  File c:\usr\iode_src\fun\fun.scl
  File c:\usr\iode_src\fun\fun.tbl
  File c:\usr\iode_src\fun\fun.var

  ;CreateDirectory "$SMPROGRAMS\IODE\Example"
  ;CreateShortCut "$SMPROGRAMS\IODE\Example\comment.lnk" "$INSTDIR\Example\fun.cmt" "" "$$INSTDIR\Example\fun.cmt" 0
  ;CreateShortCut "$SMPROGRAMS\IODE\Example\equation.lnk" "$INSTDIR\Example\fun.eqs" "" "$$INSTDIR\Example\fun.eqs" 0
  ;CreateShortCut "$SMPROGRAMS\IODE\Example\identity.lnk" "$INSTDIR\Example\fun.idt" "" "$$INSTDIR\Example\fun.idt" 0
  ;CreateShortCut "$SMPROGRAMS\IODE\Example\list.lnk" "$INSTDIR\Example\fun.lst" "" "$$INSTDIR\Example\fun.lst" 0
  ;CreateShortCut "$SMPROGRAMS\IODE\Example\scalar.lnk" "$INSTDIR\Example\fun.scl" "" "$$INSTDIR\Example\fun.scl" 0
  ;CreateShortCut "$SMPROGRAMS\IODE\Example\table.lnk" "$INSTDIR\Example\fun.tbl" "" "$$INSTDIR\Example\fun.tbl" 0
  ;CreateShortCut "$SMPROGRAMS\IODE\Example\variable.lnk" "$INSTDIR\Example\fun.var" "" "$$INSTDIR\Example\fun.var" 0

SectionEnd ; end the section

; Optional section (disabled by default)
Section "IODE COM" SecIodeCom

  ; Set output path to the installation directory.
  SetOutPath $INSTDIR

  ; Put file there
  File c:\usr\iode_src\iodecom\iodecom.exe
  File c:\usr\iode_src\iodecom\iodecom.w3
  File c:\usr\iode_src\iodecom\iodecom.xls
  File c:\usr\iode_src\iodecom\borlndmm.dll
  File c:\usr\iode_src\iodecom\errors.ini

;  File c:\usr\iode_src\iodecom\rtl120.bpl
;  File c:\usr\iode_src\iodecom\vcl120.bpl

;  CreateShortCut "$SMPROGRAMS\IODE\IodecomXls.lnk" "$INSTDIR\iodecom.xls" "" "$INSTDIR\iodecom.xls" 0
;  CreateShortCut "$SMPROGRAMS\IODE\IodecomAPL.lnk" "$INSTDIR\iodecom.w3" "" "$INSTDIR\iodecom.w3" 0


;  Exec '"$INSTDIR\iodecom.exe" /regserver'

SectionEnd ; end the section

; Optional section (disabled by default)
Section /o "IODE syntax" SecSyntax

  ; Set output path to the installation directory.
  SetOutPath $INSTDIR\syntax

  ; Put file there
  File c:\usr\iode_src\goodies\ioderep.syn
  File c:\usr\iode_src\goodies\npp_syntax.xml

SectionEnd ; end the section


; Optional section (can be disabled by the user)
;Section /o "IODE utilities (Excel, APL)" SecUtils

  ; Set output path to the installation directory.
;  SetOutPath $INSTDIR

  ; Put file there
;  File c:\usr\iode_src\goodies\iode.xls
;  File c:\usr\iode_src\goodies\iode.w3

;SectionEnd ; end the section



LangString DESC_SecExec ${LANG_ENGLISH} "Iode Executable and Manuals"
LangString DESC_SecStartMenu ${LANG_ENGLISH} "Add IODE shortcuts to the Start Menu"
LangString DESC_SecUtils ${LANG_ENGLISH} "IODE DDE utilities in Excel and APL"
LangString DESC_SecExample ${LANG_ENGLISH} "IODE example model (FUN)"
LangString DESC_SecIodeCom ${LANG_ENGLISH} "Install IodeCom server and Excel, APL clients"
LangString DESC_SecSyntax ${LANG_ENGLISH} "Textpad and Notepad++ syntax file for IODE reports"

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SecExec} $(DESC_SecExec)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecStartMenu} $(DESC_SecStartMenu)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecUtils} $(DESC_SecUtils)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecExample} $(DESC_SecExample)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecIodeCom} $(DESC_SecIodeCom)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecSyntax} $(DESC_SecSyntax)
!insertmacro MUI_FUNCTION_DESCRIPTION_END
