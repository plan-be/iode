; see https://nsis.sourceforge.io/Docs/Chapter4.html for scripting syntac.

; In particular, see 4.2.3 section for constants.
; $INSTDIR = The installation directory
; $OUTDIR = The current output directory (set implicitly via SetOutPath)
; $CMDLINE = The command line of the installer
; $LANGUAGE = The identifier of the language that is currently used

; see https://nsis.sourceforge.io/Docs/Modern%20UI%202/Readme.html for Modern UI 2

; see https://nsis.sourceforge.io/Examples/Modern%20UI/MultiLanguage.nsi 
; for multiple languages

; see https://nsis.sourceforge.io/Examples/Modern%20UI/StartMenu.nsi 
; for start menu

;--------------------------------
;Include Modern UI

  !include "MUI2.nsh"

;--------------------------------
;Include definitions

  !include "iode_definitions.nsh"

;--------------------------------
;General

  ;Name and file
  Name "IODE ${VERSION} Installer"
  OutFile "iode_${VERSION_PATH}_setup.exe"
  Unicode True
  
  ; Set compression.
  SetCompressor /SOLID /FINAL lzma

  ;The default installation directory
  InstallDir $PROFILE\IODE

  ;Get installation folder from registry if available
  InstallDirRegKey HKCU "Software\IODE" "Install Directory"

  ;Request application privileges
  RequestExecutionLevel user

;--------------------------------
;Variables

  Var StartMenuFolder

;--------------------------------
;Pages

  ; Installer pages
  !define MUI_WELCOMEPAGE_TITLE "IODE ${VERSION}"
  !insertmacro MUI_PAGE_WELCOME

  !define MUI_COMPONENTSPAGE_TEXT_TOP "IODE ${VERSION}"
  !insertmacro MUI_PAGE_COMPONENTS

  !define MUI_DIRECTORYPAGE_TEXT_TOP "IODE ${VERSION}"
  !insertmacro MUI_PAGE_DIRECTORY

  !define MUI_STARTMENUPAGE_TEXT_TOP "IODE ${VERSION}"
  !define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU" 
  !define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\IODE" 
  !define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"
  !insertmacro MUI_PAGE_STARTMENU "IODE${VERSION}" $StartMenuFolder

  !insertmacro MUI_PAGE_INSTFILES
  !insertmacro MUI_PAGE_FINISH
  
  ; Uninstaller pages
  !insertmacro MUI_UNPAGE_WELCOME
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  !insertmacro MUI_UNPAGE_FINISH
  
;--------------------------------
;Languages
 
  !insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Reserve Files
  
  ;If you are using solid compression, files that are required before
  ;the actual installation should be stored first in the data block,
  ;because this will make your installer start faster.
  
  !insertmacro MUI_RESERVEFILE_LANGDLL

;--------------------------------
;Installer Sections

Section "IODE ${VERSION} executable" SecInstall

  !verbose push
  !verbose 4

  ;Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ;GUI executable file
  File "<QT DIR>\${EXE_NAME}"
  ;Qt DLL
  File /r "<QT DIR>\*.dll"
  ;User doc
  SetOutPath $INSTDIR\doc
  File "<DOC DIR>\*.pdf" 
  File "<DOC DIR>\*.chm" 
  File "<DOC DIR>\*.htm" 
  SetOutPath $INSTDIR
  ;Misc
  <MISC INSTALL>

  !verbose pop
  
  ;Store installation folder
  WriteRegStr HKCU "Software\IODE" "Install_Dir" $INSTDIR
  
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"
  
  !insertmacro MUI_STARTMENU_WRITE_BEGIN "IODE${VERSION}"

  ;Create shortcuts
  ;Parameters: link.lnk target.file [parameters [icon.file [icon_index_number 
  ;            [start_options [keyboard_shortcut [description]]]]]]
  ; CreateShortcut "$SMPROGRAMS\My Company\My Program.lnk" "$INSTDIR\My Program.exe" \
  ; "some command line parameters" "$INSTDIR\My Program.exe" 2 SW_SHOWNORMAL \
  ; ALT|CONTROL|SHIFT|F5 "a description"
  CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
  CreateShortcut "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk" "$INSTDIR\Uninstall.exe" "" "$INSTDIR\Uninstall.exe" 0
  CreateShortcut "$SMPROGRAMS\$StartMenuFolder\IODE ${VERSION}.lnk" "$INSTDIR\${EXE_NAME}" "" "$INSTDIR\${EXE_NAME}" 0
  CreateShortcut "$SMPROGRAMS\$StartMenuFolder\IODE Help.lnk" "$INSTDIR\doc\iode.chm" "" "$INSTDIR\doc\iode.chm" 0
  CreateShortcut "$SMPROGRAMS\$StartMenuFolder\IODE Manual.lnk" "$INSTDIR\doc\iode.pdf" "" "$INSTDIR\doc\iode.pdf" 0

  !insertmacro MUI_STARTMENU_WRITE_END

SectionEnd

;--------------------------------
; Desktop Icon (disabled by default)
Section /o "Desktop Icon" SecDesktopIcon
  CreateShortCut "$DESKTOP\IODE ${VERSION}.lnk" "$INSTDIR\${EXE_NAME}" "" "$INSTDIR\${EXE_NAME}" 0
SectionEnd

;--------------------------------
; Example (disabled by default)
Section /o "IODE Example Model"  SecExample

  !verbose push
  !verbose 4

  ;files example
  SetOutPath $INSTDIR\example
  File /r <EXAMPLE PATH>

  !verbose pop

  ;CreateDirectory "$SMPROGRAMS\$StartMenuFolder\example"
  ;CreateShortCut "$SMPROGRAMS\$StartMenuFolder\example\comment.lnk" "$INSTDIR\example\fun.cmt" "" "$INSTDIR\example\fun.cmt" 0
  ;CreateShortCut "$SMPROGRAMS\$StartMenuFolder\example\equation.lnk" "$INSTDIR\example\fun.eqs" "" "$INSTDIR\example\fun.eqs" 0
  ;CreateShortCut "$SMPROGRAMS\$StartMenuFolder\example\identity.lnk" "$INSTDIR\example\fun.idt" "" "$INSTDIR\example\fun.idt" 0
  ;CreateShortCut "$SMPROGRAMS\$StartMenuFolder\example\list.lnk" "$INSTDIR\example\fun.lst" "" "$INSTDIR\example\fun.lst" 0
  ;CreateShortCut "$SMPROGRAMS\$StartMenuFolder\example\scalar.lnk" "$INSTDIR\example\fun.scl" "" "$INSTDIR\example\fun.scl" 0
  ;CreateShortCut "$SMPROGRAMS\$StartMenuFolder\example\table.lnk" "$INSTDIR\example\fun.tbl" "" "$INSTDIR\example\fun.tbl" 0
  ;CreateShortCut "$SMPROGRAMS\$StartMenuFolder\example\variable.lnk" "$INSTDIR\example\fun.var" "" "$INSTDIR\example\fun.var" 0

SectionEnd ; end the section

;--------------------------------
; Syntax Highlight (disabled by default)
Section /o "IODE syntax" SecSyntax

  !verbose push
  !verbose 4

  ;files syntax
  SetOutPath $INSTDIR
  File /r <SYNTAX PATH>

  !verbose pop

SectionEnd ; end the section

;--------------------------------
;Uninstaller Section

Section "Uninstall" SecUninstall

  !insertmacro MUI_STARTMENU_GETFOLDER "IODE${VERSION}" $StartMenuFolder

  ;Delete installed files
  DetailPrint "Deleting $INSTDIR"

  !verbose push
  !verbose 4

  ;GUI executable file
  Delete "$INSTDIR\${EXE_NAME}"
  ;Qt DLL
  Delete $INSTDIR\*.dll
  FindFirst $0 $1 "$INSTDIR\*.*"
  loop:
    StrCmp $1 "" done
    ${If} ${FileExists} "$INSTDIR\$1\*.dll"
      Delete "$INSTDIR\$1\*.dll"
      RMDir "$INSTDIR\$1"
    ${EndIf}
    FindNext $0 $1
    Goto loop
  done:
    FindClose $0
  ;User doc
  Delete "$INSTDIR\doc\*.*"
	RMDir "$INSTDIR\doc"
  ;Misc
  <MISC DELETE>

  ;Desktop Icon
  IfFileExists "$DESKTOP\IODE ${VERSION}.lnk" DesktopIconExists DesktopIconPass
  DesktopIconExists:
    DetailPrint "Removing Desktop Icon"
    Delete "$DESKTOP\IODE ${VERSION}.lnk"
  DesktopIconPass:

  ;Example
  IfFileExists "$INSTDIR\example\fun.var" FunExists FunPass
  FunExists:
    DetailPrint "Removing Examples"
    Delete "$INSTDIR\example\*.*"
    RMDir "$INSTDIR\example"
    ;Delete "$SMPROGRAMS\$StartMenuFolder\example\*.*"
    ;RMDir $SMPROGRAMS\$StartMenuFolder\example
  FunPass:

  ;Syntax
  IfFileExists "$INSTDIR\syntax\ioderep.syn" SyntaxExists SyntaxPass
  SyntaxExists:
    DetailPrint "Removing syntax"
    Delete "$INSTDIR\syntax\*.*"
    RMDir "$INSTDIR\syntax"
  SyntaxPass:

  Delete $SMPROGRAMS\$StartMenuFolder\*.*
  RMDir $SMPROGRAMS\$StartMenuFolder

  Delete "$INSTDIR\Uninstall.exe"

  !verbose pop
  
  DeleteRegKey /ifempty HKCU "Software\IODE"

SectionEnd

;--------------------------------
;Descriptions
LangString DESC_SecInstall ${LANG_ENGLISH} "Iode Executable and manuals"
LangString DESC_DesktopIcon ${LANG_ENGLISH} "Desktop icon for IODE."
LangString DESC_SecExample ${LANG_ENGLISH} "IODE example model (FUN)"
LangString DESC_SecSyntax ${LANG_ENGLISH} "Textpad and Notepad++ syntax file for IODE reports"

;Assign language strings to sections
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SecInstall} $(DESC_SecInstall)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecDesktopIcon} $(DESC_DesktopIcon)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecExample} $(DESC_SecExample)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecSyntax} $(DESC_SecSyntax)
!insertmacro MUI_FUNCTION_DESCRIPTION_END
