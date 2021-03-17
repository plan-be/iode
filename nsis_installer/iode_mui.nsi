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
Name "iODE Installer"

; The Icon of the installer
Icon iode2.ico

; The file to write
OutFile "iode663.exe"

; The default installation directory
InstallDir c:\iode

; Registry key to check for directory (so if you install again, it will
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\IODE" "Install_Dir"


; Request application privileges for Windows Vista
;RequestExecutionLevel admin
RequestExecutionLevel admin


; Pages

;Page components
;Page directory
;Page instfiles

LangString MUI_TEXT_LICENSE_TITLE ${LANG_ENGLISH} "${VERSION} Installer."
!insertmacro MUI_PAGE_LICENSE "iode.rtf"

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
  ; File c:\usr\iode_src\man\iode.hlp
  File c:\usr\iode_src\man\iode.cnt
  File c:\usr\iode_src\man\iode.chm
  File c:\usr\iode_src\man\iode.pdf
  File c:\usr\iode_src\dos\iode.ini

  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\IODE "Install_Dir" "$INSTDIR"

  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\IODE" "DisplayName" "IODE 6"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\IODE" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\IODE" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\IODE" "NoRepair" 1
  WriteUninstaller "uninstall.exe"

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
  ;${registerExtension} "$INSTDIR\iode.exe" ".rep" "IODE Reports"

SectionEnd ; end the section

; Optional section (can be disabled by the user)

Section "Start Menu Shortcuts" SecStartMenu
  SetShellVarContext all

  CreateDirectory "$SMPROGRAMS\IODE"
  CreateShortCut "$SMPROGRAMS\IODE\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\IODE\IODE.lnk" "$INSTDIR\iode.exe" "" "$INSTDIR\iode.exe" 0
  CreateShortCut "$SMPROGRAMS\IODE\IODE Help.lnk" "$INSTDIR\iode.chm" "" "$INSTDIR\iode.chm" 0
  CreateShortCut "$SMPROGRAMS\IODE\IODE Manual.lnk" "$INSTDIR\iode.pdf" "" "$INSTDIR\iode.pdf" 0

SectionEnd

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

  CreateDirectory "$SMPROGRAMS\IODE\Example"
  CreateShortCut "$SMPROGRAMS\IODE\Example\comment.lnk" "$INSTDIR\Example\fun.cmt" "" "$$INSTDIR\Example\fun.cmt" 0
  CreateShortCut "$SMPROGRAMS\IODE\Example\equation.lnk" "$INSTDIR\Example\fun.eqs" "" "$$INSTDIR\Example\fun.eqs" 0
  CreateShortCut "$SMPROGRAMS\IODE\Example\identity.lnk" "$INSTDIR\Example\fun.idt" "" "$$INSTDIR\Example\fun.idt" 0
  CreateShortCut "$SMPROGRAMS\IODE\Example\list.lnk" "$INSTDIR\Example\fun.lst" "" "$$INSTDIR\Example\fun.lst" 0
  CreateShortCut "$SMPROGRAMS\IODE\Example\scalar.lnk" "$INSTDIR\Example\fun.scl" "" "$$INSTDIR\Example\fun.scl" 0
  CreateShortCut "$SMPROGRAMS\IODE\Example\table.lnk" "$INSTDIR\Example\fun.tbl" "" "$$INSTDIR\Example\fun.tbl" 0
  CreateShortCut "$SMPROGRAMS\IODE\Example\variable.lnk" "$INSTDIR\Example\fun.var" "" "$$INSTDIR\Example\fun.var" 0


SectionEnd ; end the section

; Optional section (disabled by default)
Section "IODE COM" SecIodeCom

  ; Set output path to the installation directory.
  SetOutPath $INSTDIR

  ; Put file there
  File c:\usr\iode_src\iodecom\iodecom.exe
  File c:\usr\iode_src\iodecom\borlndmm.dll
  File c:\usr\iode_src\iodecom\iodecom.w3
  File c:\usr\iode_src\iodecom\errors.ini
  File c:\usr\iode_src\iodecom\iodecom.xls
  ;IodeCom is linked without runtime packages
  ;File c:\usr\iode_src\iodecom\rtl120.bpl
  ;File c:\usr\iode_src\iodecom\vcl120.bpl

  CreateShortCut "$SMPROGRAMS\IODE\IodecomXls.lnk" "$INSTDIR\iodecom.xls" "" "$INSTDIR\iodecom.xls" 0
  CreateShortCut "$SMPROGRAMS\IODE\IodecomAPL.lnk" "$INSTDIR\iodecom.w3" "" "$INSTDIR\iodecom.w3" 0

  CopyFiles /SILENT '$INSTDIR\errors.ini' '$WINDIR\errors.ini'

  Exec '"$INSTDIR\iodecom.exe" /regserver'

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


; Uninstaller

Section "Uninstall"
  SetShellVarContext all

  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\IODE"
  DeleteRegKey HKLM SOFTWARE\IODE

  ; Remove files and uninstaller
  DetailPrint "Deleting $INSTDIR"
  Delete $INSTDIR\iode.*
  Delete $INSTDIR\readme.htm
  Delete $INSTDIR\*.thm
  Delete $INSTDIR\*.gif
  Delete $INSTDIR\*.css
  Delete $INSTDIR\swf.ini
  Delete $INSTDIR\iode.ini


  ; Remove shortcuts, if any
  Delete $SMPROGRAMS\IODE\*.*

  ;${unregisterExtension} ".cmt" "IODE Comments"
  ;${unregisterExtension} ".eqs" "IODE Equations"
  ;${unregisterExtension} ".idt" "IODE Identities"
  ;${unregisterExtension} ".lst" "IODE Lists"
  ;${unregisterExtension} ".scl" "IODE Scalars"
  ;${unregisterExtension} ".tbl" "IODE Tables"
  ;${unregisterExtension} ".var" "IODE Variables"
  ;${unregisterExtension} ".ac" "IODE Ascii Comments"
  ;${unregisterExtension} ".ae" "IODE Ascii Equations"
  ;${unregisterExtension} ".ai" "IODE Ascii Identities"
  ;${unregisterExtension} ".al" "IODE Ascii Lists"
  ;${unregisterExtension} ".as" "IODE Ascii Scalars"
  ;${unregisterExtension} ".at" "IODE Ascii Tables"
  ;${unregisterExtension} ".av" "IODE Ascii Variables"
  ;${unregisterExtension} ".rep" "IODE Reports"

  ;Remove IODE COM part
IfFileExists "$INSTDIR\iodecom.exe" IodeComExists IodeComPass
IodeComExists:
  DetailPrint "Removing IodeCom"
  Exec '"$INSTDIR\iodecom.exe" /unregserver'
  Delete "$INSTDIR\iodecom.*"
  Delete "$INSTDIR\*.bpl"
IodeComPass:

  ;Remove FUN part
IfFileExists "$INSTDIR\Example\fun.var" FunExists FunPass
FunExists:
  DetailPrint "Removing Examples"
  Delete "$INSTDIR\Example\*.*"
  RMDir "$INSTDIR\Example"
  Delete "$SMPROGRAMS\IODE\Example\*.*"
  RMDir $SMPROGRAMS\IODE\Example

FunPass:

IfFileExists "$INSTDIR\syntax\ioderep.syn" SyntaxExists SyntaxPass
SyntaxExists:
  DetailPrint "Removing syntax"

  Delete "$INSTDIR\syntax\*.*"
  RMDir "$INSTDIR\syntax"

SyntaxPass:

  ; Remove directories used
  RMDir $SMPROGRAMS\IODE

  Delete "$INSTDIR\Uninstall.exe"
  RMDir $INSTDIR

SectionEnd


LangString DESC_SecExec ${LANG_ENGLISH} "Iode Executable, manuals and file associations"
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
