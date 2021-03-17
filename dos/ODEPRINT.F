
PAGE vkp_viewtbl{
    TITLE "Display and Print Tables"
    $PGHEAD
    FNKEY {
	F1             "Help"
	ESCAPE {ABORT} "Cancel"
	F3     {ABORT} "Cancel"
	F6  {C_FN SB_FilePrintSetup();}        "Print Setup"
	F8  {C_FN return(C_ViewPrintTbl(1));} "Print"
	F10 {C_FN return(C_ViewPrintTbl(0));} "Display"
	}
    SCREEN {
ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ             D I S P L A Y   A N D   P R I N T   T A B L E S               บ
ฬอออออออออออออออัอัอออออออออออออออออออออออออออออออออออออออออออออัอออออออออออน
บ Table names   ณ$tabs                                          ณ &F10      บ
บ               ณ ณ                                             ณ           บ
วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤลฤลฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤด           บ
บ Sample        ณ$smpl                                          ณ &F8       บ
บ               ณ                                               ณ           บ
วฤฤฤฤฤฤฤฤยฤฤฤฤฤฤมฤมฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤด &F6       บ
บ File[1]ณWorkspace                                             ณ           บ
บ File[2]ณ$f2                                                   ณ &ESC      บ
บ File[3]ณ$f3                                                   ณ           บ
บ File[4]ณ$f4                                                   ณ &F1       บ
บ File[5]ณ$f5                                                   ณ           บ
วฤฤฤฤฤฤฤฤมฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤมฤฤฤฤฤฤฤฤฤฤฤถ
บ Language $lang       NbDec $ndec                                          บ
บ                                                                           บ
ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
}

    FIELD name smpl  nc 47 nl 2 EDITOR SBAR blue
    FIELD name tabs  nc 47 nl 2 EDITOR SBAR blue
    FIELD name f2    ncv 53 nc 255 string DIR $FLDNBOX
    FIELD name f3    ncv 53 nc 255 string DIR $FLDNBOX
    FIELD name f4    ncv 53 nc 255 string DIR $FLDNBOX
    FIELD name f5    ncv 53 nc 255 string DIR $FLDNBOX
    FIELD NAME ndec  nc 2  integer    $FLDBOX
    FIELD NAME lang  MENU vkm_lang    $FLDBOX

    $BUTTONESC " Cancel  "
    $BUTTONF10 " Display "
    $BUTTONF1  "  Help   "
    $BUTTONF8  "  Print  "
    $BUTTONF6  "  Setup  "
}

PAGE vkp_dataeditgraph {
    TITLE "Graph from Variables"
    $PGHEAD
    FNKEY {
	F1             "Help"
	ESCAPE {ABORT} "Cancel"
	F3     {ABORT} "Cancel"
	F7  {ABORT}                            "Apply Graph Settings"
	F6  {C_FN SB_FilePrintSetup();}        "Print Setup"
	F10 {C_FN return(C_DataEditGraph(0));} "Display graph"
	F8  {C_FN return(C_DataEditGraph(1));} "Print graph"
	}
    SCREEN {
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ                     G R A P H   V A R I A B L E S                        บ
ฬออออออออออออัอัออออออออออออออออออออออออออออออออออออออออออออัออออออออออออออน
บ Variables  ณ$names                                        ณ &F10         บ
บ            ณ ณ                                            ณ              บ
บ            ณ ณ                                            ณ &ESC         บ
วฤฤฤฤฤฤฤฤฤฤฤฤลฤมฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤด              บ
บ Sample     ณ$mode          From $from       To $to        ณ &F8          บ
บ            ณ                                              ณ              บ
บ Layout     ณ$chart                      $axis             ณ &F7          บ
บ            ณ                                              ณ              บ
บ            ณX grids=$Xgrid         MinY=$Ymin             ณ &F6          บ
บ            ณ                                              ณ              บ
บ            ณY grids=$Ygrid         MaxY=$Ymax             ณ &F1          บ
บ            ณ                                              ณ              บ
วฤฤฤฤฤฤฤฤฤฤฤฤมฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤมฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
บ Language  $lang                                                          บ
บ                                                                          บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
}
    FIELD name names nc 46 nl 3 EDITOR SBAR blue
    FIELD name mode  MENU vkm_mode  $FLDBOX
    FIELD name chart MENU vkm_chart $FLDBOX
    FIELD name xgrid menu vkm_grids $FLDBOX
    FIELD name ygrid menu vkm_grids $FLDBOX
    FIELD name axis  MENU vkm_axis  $FLDBOX
    FIELD name ymin  real           $FLDBOX "--      "
    FIELD name ymax  real           $FLDBOX "--      "
    FIELD name from nc 10 REQUIRED "1990Y1" $FLDBOX
    FIELD name to   nc 10 REQUIRED "2000Y1" $FLDBOX
    FIELD name lang MENU vkm_lang           $FLDBOX

    $BUTTONESC " Cancel  "
    $BUTTONF10 " Display "
    $BUTTONF1  "  Help   "
    $BUTTONF8  "  Print  "
    $BUTTONF7  "  Apply  "
    $BUTTONF6  "  Setup  "
}

PAGE vkp_viewbytbl{
    TITLE "Display and Update Variables from Tables"
    $PGHEAD
    FNKEY {
	F1             "Help"
	ESCAPE {ABORT} "Cancel"
	F10 {C_FN return(C_ViewByTbl());} "View by Tbl"
	}
    SCREEN {
ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ  D I S P L A Y   A N D   U P D A T E  V A R I A B L E S  B Y  T A B L E S บ
ฬอออออออออออออออัอัอออออออออออออออออออออออออออออออออออออออออออออัอออออออออออน
บ Table names   ณ$tabs                                          ณ &F10      บ
บ               ณ ณ                                             ณ           บ
บ               ณ ณ                                             ณ &ESC      บ
บ               ณ ณ                                             ณ           บ
บ               ณ ณ                                             ณ &F1       บ
บ               ณ ณ                                             ณ           บ
ศอออออออออออออออฯอฯอออออออออออออออออออออออออออออออออออออออออออออฯอออออออออออผ
}

    FIELD name tabs  nc 47 nl 6 EDITOR SBAR blue

    $BUTTONESC " Cancel  "
    $BUTTONF1  "  Help   "
    $BUTTONF10 " Update  "
}

PAGE vkp_viewgr{
    TITLE "Display and Print Graphics"
    $PGHEAD
    FNKEY {
	F1             "Help"
	ESCAPE {ABORT} "Quit"
	F3  {ABORT} "Quit"
	F6  {C_FN SB_FilePrintSetup();}        "Print Setup"
	F8  {C_FN return(C_ViewPrintGr(1));} "Print"
	F10 {C_FN return(C_ViewPrintGr(0));} "Display"
	}
    SCREEN {
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ           D I S P L A Y   A N D   P R I N T   G R A P H I C S              บ
ฬอออออออออออออออัอัออออออออออออออออออออออออออออออออออออออออออออัอออออออออออออน
บ Table names   ณ$tabs                                         ณ &F10        บ
บ               ณ                                              ณ             บ
วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤลฤลฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤด             บ
บ Sample        ณ$smpl                                         ณ &F8         บ
บ               ณ                                              ณ             บ
วฤฤฤฤฤยฤฤฤฤฤฤฤฤฤมฤมฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤด &F6         บ
บ     ณ[1] Workspace                                           ณ             บ
บ Var.ณ[2] $f2                                                 ณ &ESC        บ
บFilesณ[3] $f3                                                 ณ             บ
บ     ณ[4] $f4                                                 ณ &F1         บ
บ     ณ[5] $f5                                                 ณ             บ
วฤฤฤฤฤมฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤมฤฤฤฤฤฤฤฤฤฤฤฤฤถ
บ Language $lang                                                             บ
บ                                                                            บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
}

    FIELD name smpl  nc 46 nl 2 EDITOR SBAR blue
    FIELD name tabs  nc 46 nl 2 EDITOR SBAR blue
    FIELD name f2    nc 255 ncv 52 string DIR $FLDNBOX
    FIELD name f3    nc 255 ncv 52 string DIR $FLDNBOX
    FIELD name f4    nc 255 ncv 52 string DIR $FLDNBOX
    FIELD name f5    nc 255 ncv 52 string DIR $FLDNBOX
    FIELD NAME lang  MENU vkm_lang    $FLDBOX

    $BUTTONESC " Cancel  "
    $BUTTONF10 " Display "
    $BUTTONF1  "  Help   "
    $BUTTONF8  "  Print  "
    $BUTTONF6  "  Setup  "
}

PAGE vkp_viewvar{
    TITLE "Display and Print Variables"
    $PGHEAD
    FNKEY {
	F1             "Help"
	ESCAPE {ABORT} "Quit"
	F3     {ABORT} "Quit"
	F6  {C_FN SB_FilePrintSetup();}        "Print Setup"
	F8  {C_FN return(C_ViewPrintVar(1));} "Print"
	F10 {C_FN return(C_ViewPrintVar(0));} "Display"
	}
    SCREEN {
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ          D I S P L A Y   A N D   P R I N T   V A R I A B L E S             บ
ฬอออออออออออออออัอัออออออออออออออออออออออออออออออออออออออออออออัอออออออออออออน
บ Variable namesณ$vars                                         ณ &F10        บ
บ or LEC forms  ณ ณ                                            ณ             บ
วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤลฤลฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤด             บ
บ Sample        ณ$smpl                                         ณ &F8         บ
บ               ณ ณ                                            ณ             บ
วฤฤฤฤฤยฤฤฤฤฤฤฤฤฤมฤมฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤด &F6         บ
บ     ณ[1] Workspace                                           ณ             บ
บ Var.ณ[2] $f2                                                 ณ &ESC        บ
บFilesณ[3] $f3                                                 ณ             บ
บ     ณ[4] $f4                                                 ณ &F1         บ
บ     ณ[5] $f5                                                 ณ             บ
วฤฤฤฤฤมฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤมฤฤฤฤฤฤฤฤฤฤฤฤฤถ
บ Language $lang       NbDec $ndec                                           บ
บ                                                                            บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
}

    FIELD name smpl  nc 46 nl 2 EDITOR SBAR blue
    FIELD name vars  nc 46 nl 2 EDITOR SBAR blue
    FIELD name f2    nc 255 ncv 52 string DIR $FLDNBOX
    FIELD name f3    nc 255 ncv 52 string DIR $FLDNBOX
    FIELD name f4    nc 255 ncv 52 string DIR $FLDNBOX
    FIELD name f5    nc 255 ncv 52 string DIR $FLDNBOX
    FIELD NAME ndec  nc 2  integer    $FLDBOX
    FIELD NAME lang  MENU vkm_lang    $FLDBOX

    $BUTTONESC " Cancel  "
    $BUTTONF10 " Display "
    $BUTTONF1  "  Help   "
    $BUTTONF8  "  Print  "
    $BUTTONF6  "  Setup  "

}

MENU vkm_title {
    Auto
    OPTION "Print full Definition"
    OPTION "Print only titles"
}
MENU vkm_einfos {
    Auto
    OPTION "Equation only"
    OPTION "Equation + comments"
    OPTION "All (eq, comment, estimation)"
}

MENU vkm_elec {
    Auto
    OPTION "As is"
    OPTION "Replace coeffs by values"
    OPTION "Replace coeffs by values + t-test"
}

PAGE vkp_probjs {
    TITLE "Print Object Definitions"
    $PGHEAD
    FNKEY {
	    F1             "Help"
	    ESCAPE {ABORT} "Quit"
	    F3     {ABORT} "Quit"
	    F6  {C_FN SB_FilePrintSetup();}     "Print Setup"
	    F10 {C_FN return(C_PrintObjDef());} "Print"
	  }
    SCREEN {
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ             P R I N T   O B J E C T S   D E F I N I T I O N                บ
ฬออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออัอออออออออออออออน
บ Object type     : $type                                    ณ &F10          บ
บ                                                            ณ               บ
บ For Tables      : $title                                   ณ &ESC          บ
บ                                                            ณ               บ
บ Print LEC forms : $elec                                    ณ &F6           บ
บ                                                            ณ               บ
บ Equations : Print $einfos                                  ณ               บ
บ                                                            ณ               บ
วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤยฤยฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤด               บ
บ  Object       ณ$objs                                       ณ &F1           บ
บ  Names        ณ ณ                                          ณ               บ
บ               ณ ณ                                          ณ               บ
วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤมฤมฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤมฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
บ Language $lang       NbDec $ndec                                           บ
บ                                                                            บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
}
    FIELD name type   MENU vkm_obj    $FLDBOX
    FIELD name title  MENU vkm_title  $FLDBOX
    FIELD name einfos MENU vkm_einfos $FLDBOX
    FIELD name elec   MENU vkm_elec   $FLDBOX
    FIELD name objs  nc 44 nl 3 upper EDITOR SBAR
    FIELD NAME ndec  nc 2  integer $FLDBOX
    FIELD NAME lang  MENU vkm_lang $FLDBOX

    $BUTTONESC " Cancel  "
    $BUTTONF1  "  Help   "
    $BUTTONF10 "  Print  "
    $BUTTONF6  "  Setup  "
}


PAGE vkp_prtfile {
    Title "Print A2M File"
    $PGHEAD
    FNKEY {
	F1             "Help"
	ESCAPE {ABORT} "Cancel"
	F6  {C_FN SB_FilePrintSetup();}        "Print Setup"
	F10 {C_FN return(C_A2MPrint()); ABORT}
    }

    SCREEN {
ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ           P R I N T    A   A 2 M   F I L E                  บ
ฬอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออน
บ                                                             บ
บ File  name : $name                                          บ
บ                                                             บ
วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
บ      &F10      &ESC       &F1         &F6                   บ
บ                                                             บ
ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
}
    FIELD name name nc 255 ncv 45 string DIR $FLDBOX REQUIRED
/*    FIELD name mode MENU vkm_ny $FLDBOX REQUIRED */

    $BUTTONESC " Cancel "
    $BUTTONF1  "  Help  "
    $BUTTONF10 " Print  "
    $BUTTONF6  "  Setup "
}
