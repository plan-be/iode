
PAGE vkp_dupl {
    Title "Duplicate Objects"
    $PGHEAD
    END_FN {C_FN return(C_DataDuplicate());}
    SCREEN {
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ          D U P L I C A T E  O B J E C T S   I N    W S         บ
ฬอออออออออออออัออออออออออออออออออออออออออออออออออออัอออออออออออออน
บ Object type ณ $type                              ณ &F10        บ
บ             ณ                                    ณ             บ
บ Duplicate   ณ $from                              ณ &ESC        บ
บ             ณ                                    ณ             บ
บ To          ณ $to                                ณ &F1         บ
บ             ณ                                    ณ             บ
ศอออออออออออออฯออออออออออออออออออออออออออออออออออออฯอออออออออออออผ
}
    FIELD  name type   MENU vkm_obj   $FLDBOX
    FIELD  name from   nc 20 REQUIRED $FLDBOX
    FIELD  name to     nc 20 REQUIRED $FLDBOX
    $BUTTONESC " Cancel "
    $BUTTONF10 "   Ok   "
    $BUTTONF1  "  Help  "
}

PAGE vkp_search {
    Title "Search Text in WS Objects"
    $PGHEAD
    FNKEY {
	    F1             "Help"
	    ESCAPE {ABORT} "Cancel"
	    F3     {ABORT} "Cancel"
	    F10    {C_FN C_DataSearch();} "Search"
	  }
    SCREEN {
ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ           S E A R C H  T E X T   I N   W S   O B J E C T S              บ
ฬออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออัออออออออออน
บ Text : $name                                                 ณ &F10     บ
บ                                                              ณ          บ
บ Whole word : $word   Exact case : $case                      ณ &ESC     บ
บ                                                              ณ          บ
วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤด          บ
บ Objects to scan : $type                                      ณ &F1      บ
บ                                                              ณ          บ
บ Search in Names ? $names Formulas ? $forms Free text ? $textsณ          บ
บ                                                              ณ          บ
ฬออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออต          บ
บ Save result in list : $out                                   ณ          บ
บ                                                              ณ          บ
ฬออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออต          บ
บ Result : *res                                                ณ          บ
บ                                                              ณ          บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออฯออออออออออผ
}
    FIELD  name name   nc 40 sqz    $FLDBOX
    FIELD  name word   MENU vkm_ny  $FLDBOX
    FIELD  name case   MENU vkm_ny  $FLDBOX
    FIELD  name type   MENU vkm_obj $FLDBOX
    FIELD  name names  MENU vkm_ny  $FLDBOX
    FIELD  name forms  MENU vkm_ny  $FLDBOX
    FIELD  name texts  MENU vkm_ny  $FLDBOX
    FIELD  name out    "_RES" REQUIRED nc 20 $FLDBOX
    FIELD  name res    nl 2 nc 40 DEF_OBJ
    $BUTTONESC " Cancel "
    $BUTTONF10 "   Ok   "
    $BUTTONF1  "  Help  "
}

PAGE vkp_dlist {
    Title "Search Objects in WS or File"
    $PGHEAD
    FNKEY {
	    F1             "Help"
	    ESCAPE {ABORT} "Cancel"
	    F3     {ABORT} "Cancel"
	    F10    {C_FN C_DataList();} "Browse"
	  }
    SCREEN {
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ                  F I L E   O R   W S   C O N T E N T S                 บ
ฬอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออัออออออออออน
บ Search : $pattern              of  $type                    ณ &F10     บ
บ                                                             ณ          บ
บ File   : $file                                              ณ &ESC     บ
บ                                                             ณ          บ
ฬอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออต          บ
บ Save result in list : $name                *cnt  element    ณ &F1      บ
บ ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ              ณ          บ
บ      *res                                                   ณ          บ
บ                                                             ณ          บ
บ                                                             ณ          บ
บ                                                             ณ          บ
ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออฯออออออออออผ
}
    FIELD  name pattern nc 20 sqz    $FLDBOX
    FIELD  name file   DIR nc 255 ncv 50 sqz    $FLDBOX
    FIELD  name name   "_RES" REQUIRED nc 20 blue
    FIELD  name type   MENU vkm_obj $FLDBOX
    FIELD  name res    nl 4 nc 40  DEF_OBJ
    FIELD  name cnt    integer nc 5  cyan rjust

    $BUTTONESC " Cancel "
    $BUTTONF10 "   Ok   "
    $BUTTONF1  "  Help  "
}

PAGE vkp_clist {
    Title "Compare WS objects to File Objects"
    $PGHEAD
    FNKEY {
	    F1             "Help"
	    ESCAPE {ABORT} "Cancel"
	    F3     {ABORT} "Cancel"
	    F10    {C_FN C_DataCompare();} "Compare"
	  }
    SCREEN {
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ       C O M P A R E  O B J E C T S  F R O M   W S   W I T H  F I L E       บ
ฬอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออัออออออออออน
บ Compare: $type          Threshold (vars only) : $eps            ณ &F10     บ
บ                                                                 ณ          บ
บ File   : $file                                                  ณ &ESC     บ
บ                                                                 ณ          บ
บ Save results in lists                                           ณ          บ
บ ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ                                           ณ          บ
บ Found in WS only         $one                  *cnt1 elements   ณ &F1      บ
บ                                                                 ณ          บ
บ Found in file only       $two                  *cnt2 elements   ณ          บ
บ                                                                 ณ          บ
บ Found in both, equal     $three                *cnt3 elements   ณ          บ
บ                                                                 ณ          บ
บ Found in both, different $for                  *cnt4 elements   ณ          บ
บ                                                                 ณ          บ
ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออฯออออออออออผ
}
    FIELD  name file   DIR nc 255 ncv 50 sqz    $FLDBOX
    FIELD  name type   MENU vkm_obj            $FLDBOX
    FIELD  name eps    REAL "0.0000001" nc 10  $FLDBOX
    FIELD  name one    "OLD"        REQUIRED nc 20 $FLDBOX
    FIELD  name two    "NEW"        REQUIRED nc 20 $FLDBOX
    FIELD  name three  "SAME"       REQUIRED nc 20 $FLDBOX
    FIELD  name for    "CHANGED"    REQUIRED nc 20 $FLDBOX
    FIELD  name cnt1   integer nc 5 rjust cyan
    FIELD  name cnt2   integer nc 5 rjust cyan
    FIELD  name cnt3   integer nc 5 rjust cyan
    FIELD  name cnt4   integer nc 5 rjust cyan

    $BUTTONESC " Cancel "
    $BUTTONF10 "   Ok   "
    $BUTTONF1  "  Help  "
}

PAGE vktp_tattr {
    Title "Graphic attributes"
    $PGHEAD
    SCREEN {
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ           G R A P H I C   A T T R I B U T E S                  บ
วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤยฤฤฤฤฤฤฤฤฤฤฤถ
บ 1. Current Line type : $graph                      ณ &F10      บ
บ    ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ                               ณ           บ
บ                        $laxis                      ณ           บ
บ                                                    ณ           บ
บ 2. Global Attributes                               ณ           บ
บ    ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ                               ณ           บ
บ    YMin $YMin          X-Grids $Xgrid              ณ &ESC      บ
บ                                                    ณ           บ
บ    YMax $YMax          Y-Grids $Ygrid              ณ           บ
บ                                                    ณ           บ
บ    ZMin $ZMin          Align   $Align              ณ           บ
บ                                                    ณ           บ
บ    ZMax $ZMax          Shadow  $Shad               ณ &F1       บ
บ                                                    ณ           บ
บ    Axis $Axis          Box  $Box                   ณ           บ
บ                                                    ณ           บ
วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤมฤฤฤฤฤฤฤฤฤฤฤถ
บ YMin,YMax,ZMin,ZMax : Set to -- for automatic scaling          บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
}
    FIELD name graph     MENU vkm_chart $FLDBOX
    FIELD name laxis     MENU vkm_laxis $FLDBOX
    FIELD name box       menu vkm_ny    $FLDBOX
    FIELD name shad      menu vkm_ny    $FLDBOX
    FIELD name xgrid     menu vkm_grids $FLDBOX
    FIELD name ygrid     menu vkm_grids $FLDBOX
    FIELD name axis      MENU vkm_axis  $FLDBOX
    FIELD name align     MENU vkm_align $FLDBOX
    FIELD name ymin      real "--     " $FLDBOX
    FIELD name ymax      real "--     " $FLDBOX
    FIELD name zmin      real "--     " $FLDBOX
    FIELD name zmax      real "--     " $FLDBOX

    $BUTTONESC " Cancel "
    $BUTTONF1  "  Help  "
    $BUTTONF10 "   OK   "
}
/*
PAGE vktp_div {
    $PGHEAD
    SCREEN {
ษอออออออออออออออออออออออออออออออออออออออออออออออออป
บ          C O L U M N   D I V I S O R            บ
บ                 Column *n                       บ
ฬอออออออออออออออออออออออออออออออออออออัอออออออออออน
บ$div                                 ณ &F10      บ
บ ณ                                   ณ           บ
บ ณ                                   ณ &ESC      บ
บ ณ                                   ณ           บ
บ ณ                                   ณ &F1       บ
บ ณ                                   ณ           บ
ศอฯอออออออออออออออออออออออออออออออออออฯอออออออออออผ
}
    FIELD name n integer nc 2
    FIELD EDITOR nl 6 nc 37 name div sbar blue

    $BUTTONESC " Cancel "
    $BUTTONF1  "  Help  "
    $BUTTONF10 "   OK   "
}
*/
PAGE vktp_create {
    Title "Create a New Table"
    $PGHEAD
    SCREEN {
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ                  C R E A T E   A   N E W   T A B L E                     บ
ฬอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออัออออออออออออออน
บ Table name : $name                   Nbr of cols : $dim   ณ &F10         บ
บ                                                           ณ              บ
วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤด &ESC         บ
บ Auto Table : to automatically create a table, enter       ณ              บ
บ ฤฤฤฤฤฤฤฤฤฤ   LEC forms (separator=';'), the title         ณ &F1          บ
บ              and the selected footnotes                   ณ              บ
วฤฤฤฤฤฤฤฤฤฤฤฤฤฤยฤยฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤด              บ
บ LEC formulas ณ$lec                                        ณ              บ
บ separated by ณ ณ                                          ณ              บ
บ semi-colons  ณ ณ                                          ณ              บ
บ              ณ ณ                                          ณ              บ
วฤฤฤฤฤฤฤฤฤฤฤฤฤฤมฤมฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤมฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
บ Table title : $title                                                     บ
บ ฤฤฤฤฤฤฤฤฤฤฤ                                                              บ
บ Footnotes : Mode $mode   Files $Files   Date $date                       บ
บ ฤฤฤฤฤฤฤฤฤ                                                                บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
}
    FIELD string nc 20 name name upper required $FLDBOX
    FIELD natural " 2"  name dim REQUIRED       $FLDBOX
    FIELD EDITOR nl 4 nc 40 name lec sbar       blue
    FIELD string nc 55 name title               $FLDBOX
    FIELD MENU vkm_ny name mode                 $FLDBOX
    FIELD MENU vkm_ny name files                $FLDBOX
    FIELD MENU vkm_ny name date                 $FLDBOX

    $BUTTONESC " Cancel "
    $BUTTONF1  "  Help  "
    $BUTTONF10 "   OK   "
}

PAGE vkp_tins {
    Title "Insert Table Line"
    center back cyan  PROFILE
    SCREEN {
ษออออออออออออออออออออออออออออออออออัอออออออออออป
บ      INSERT NEW TABLE LINE       ณ &F10      บ
ฬออออออออออออออออออออออออออออออออออต           บ
บ   Line type : $type              ณ &ESC      บ
บ                                  ณ           บ
บ   Position  : $where             ณ &F1       บ
บ                                  ณ           บ
ศออออออออออออออออออออออออออออออออออฯอออออออออออผ
}

    FIELD name type menu {
	    OPTION "CELL"
	    OPTION "TITLE "
	    OPTION "LINE"
	    OPTION "FILES "
	    OPTION "MODE  "
	    OPTION "DATE  "
	    } $FLDBOX

    FIELD name where menu {
	    OPTION "After current  "
	    OPTION "Before current "
	    } $FLDBOX

    $BUTTONESC " Cancel "
    $BUTTONF1  "  Help  "
    $BUTTONF10 "   OK   "
}


PAGE vkvp_create {
    Title "Create a Scalar"
    center back cyan  PROFILE
    SCREEN {
ษอออออออออออออออออออออออออออออออออออออออออออัอออออออออออป
บ            CREATE A SCALAR                ณ &F10      บ
ฬอออออออออออออออออออออออออออออออออออออออออออต           บ
บ                                           ณ &ESC      บ
บ   Scalar name : $name                     ณ           บ
บ                                           ณ &F1       บ
บ                                           ณ           บ
ศอออออออออออออออออออออออออออออออออออออออออออฯอออออออออออผ
}

    FIELD nc 20 upper name name required FNKEY _fk_f10 $FLDBOX

    $BUTTONESC " Cancel "
    $BUTTONF1  "  Help  "
    $BUTTONF10 "   OK   "
}

PAGE vkp_list {
    Title "Objects selection"
    $PGHEAD
    HELP "Slection de objets … diter"
    SCREEN {
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ                   O B J E C T    E D I T O R                   บ
ฬออออออออออออออออออออออออออออออออออออออออออออออออออออัอออออออออออน
บ Object type : *type                                ณ &F10      บ
บ                                                    ณ           บ
วฤฤฤฤฤฤฤฤฤฤฤฤฤยฤยฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤด           บ
บ  List of    ณ$list                                 ณ &ESC      บ
บ objects to  ณ ณ                                    ณ           บ
บ be edited   ณ ณ                                    ณ           บ
บ             ณ ณ                                    ณ &F1       บ
บ (empty for  ณ ณ                                    ณ           บ
บ   all WS)   ณ ณ                                    ณ           บ
บ             ณ ณ                                    ณ           บ
ศอออออออออออออฯอฯออออออออออออออออออออออออออออออออออออฯอออออออออออผ
}
    FIELD name type MENU vkm_obj output $FLDBOX
    FIELD nc 37 nl 7 name list EDITOR sbar input blue

    $BUTTONESC " Cancel "
    $BUTTONF1  "  Help  "
    $BUTTONF10 "   OK   "
}

PAGE vkp_edit {
    Title "Object Definition"
    $PGHEAD
    END_FN {C_FN return(C_DataEditCil1());}
    SCREEN {
ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ                        E D I T   O B J E C T                              บ
ฬอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออัอออออออออออน
บObject type : *type        Object name : $name                 ณ &F10      บ
บ                                                               ณ           บ
วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤด &ESC      บ
บ                    Object Definition                          ณ           บ
วฤยฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤด           บ
บ$val                                                           ณ &F1       บ
บ ณ                                                             ณ           บ
บ ณ                                                             ณ           บ
บ ณ                                                             ณ           บ
บ ณ                                                             ณ           บ
บ ณ                                                             ณ           บ
บ ณ                                                             ณ           บ
บ ณ                                                             ณ           บ
ศอฯอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออฯอออออออออออผ
}
    FIELD name type MENU vkm_obj        $FLDBOX
    FIELD name name nc 20 required      $FLDBOX
    FIELD name val  nc 61 nl 8 EDITOR SBAR blue


    $BUTTONESC " Cancel "
    $BUTTONF1  "  Help  "
    $BUTTONF10 "   OK   "
}

PAGE vkp_eqsadj {
    Title "Equation Dynamic Adjustment"
    $PGHEAD
    END_FN {C_FN return(E_DynamicAdjustment(
		(int) vkp_eqsadj_METHOD,
		&(vkp_eqsadj_LEC), vkp_eqsadj_C1, vkp_eqsadj_C2));}
    SCREEN {
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ       ADJUST EQUATION DYNAMICALLY                              บ
ฬอออออออออออออัออออออออออออออออออออออออออออออออออออัอออออออออออออน
บ Equation    ณ *lec                               ณ &F10        บ
บ             ณ                                    ณ             บ
บ             ณ                                    ณ             บ
บ             ณ                                    ณ &ESC        บ
บ             ณ                                    ณ             บ
บ Method      ณ $method                            ณ             บ
บ             ณ                                    ณ &F1         บ
บ Coefficientsณ $c1                                ณ             บ
บ             ณ                                    ณ             บ
บ             ณ $c2                                ณ             บ
บ             ณ                                    ณ             บ
ศอออออออออออออฯออออออออออออออออออออออออออออออออออออฯอออออออออออออผ
}
    FIELD  name lec nc 32 nl 4 upper EDITOR blue
    FIELD  name method MENU
	{ OPTION "Partial Adjustement"
	  OPTION "Error Correction Model"}   $FLDBOX
    FIELD  name c1     nc 20 REQUIRED $FLDBOX
    FIELD  name c2     nc 20 REQUIRED $FLDBOX
    $BUTTONESC " Cancel "
    $BUTTONF10 " Adjust "
    $BUTTONF1  "  Help  "
}


PAGE vkp_eqs {
    Title "Equation Definition"
    center Back Cyan
    BEGIN_FN {C_FN ODE_blk_begin();}
    FNKEY {
	F1                                   "Help"
	ESCAPE  {C_FN {ODE_blk_end_fn();} }  "Quit"
	F3      {C_FN {ODE_blk_unitroot();}} "Unit Root"
	F4      {C_FN {ODE_blk_coef_fn();} } "Coefs"
	F5      {C_FN {ODE_blk_est_fn();}  } "Estimate"
	F6      {C_FN {ODE_blk_next_fn();} } "Next"
	F7      {C_FN {ODE_blk_dadj_fn(vkp_eqs_NAME);} } "Dyn. Adj"
	F8      {C_FN {ODE_blk_res_fn();}  } "Results"
	F10     {C_FN {ODE_blk_save_fn();} } "Save"
    }
    SCREEN {
ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ                      E Q U A T I O N   D E F I N I T I O N                  บ
วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤยฤฤฤฤฤฤฤฤฤฤฤถ
บ Name $name                     RýAdj=*rsquare   DurbW=*dw       ณ&F10       บ
บ                                Fstat=*fstat     LogLk=*llik     ณ           บ
บ Est.method $method        From $from     To $to                 ณ&ESC       บ
บ                                                                 ณ           บ
วฤฤฤฤฤฤฤยฤยฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤด&F4        บ
บ Lec   ณ$lec                                                     ณ           บ
บ       ณ ณ                                                       ณ&F5        บ
บ       ณ ณ                                                       ณ           บ
บ       ณ ณ                                                       ณ&F6        บ
บ       ณ ณ                                                       ณ           บ
บ       ณ ณ                                                       ณ&F7        บ
วฤฤฤฤฤฤฤลฤลฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤด           บ
บCommentณ$cmt                                                     ณ&F8        บ
บ       ณ ณ                                                       ณ           บ
วฤฤฤฤฤฤฤลฤลฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤยฤฤฤฤฤฤยฤยฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤด&F3        บ
บ Block ณ$blk                    ณInstrsณ$instrs                  ณ           บ
บ       ณ ณ                      ณ      ณ ณ                       ณ&F1        บ
บ       ณ ณ                      ณ      ณ ณ                       ณ           บ
ศอออออออฯอฯออออออออออออออออออออออฯออออออฯอฯอออออออออออออออออออออออฯอออออออออออผ
}
    FIELD  nc 20            name name COMPULSORY    upper $FLDBOX
	END_FN {C_FN ODE_blk_name_fn();}
    FIELD  NC 57 NL 6       name lec EDITOR          sbar blue
	END_FN {C_FN ODE_blk_save_cur();}
    FIELD  NC 57 NL 2       name cmt EDITOR          sbar blue
    FIELD  nc 8 string      name from upper allowed "0123456789YQMW " REQUIRED $FLDBOX
    FIELD  nc 8 string      name to   upper allowed "0123456789YQMW " REQUIRED $FLDBOX
    FIELD  MENU vkm_method  name method             $FLDBOX
    FIELD  NC 24 NL 2       name blk     EDITOR      sbar blue
	END_FN {C_FN ODE_blk_fn();}
    FIELD  NC 25 NL 2       name instrs  EDITOR      sbar blue
/*  FIELD DATE name date FORMAT "dd/mm/yy"          green */
    FIELD REAL nc 8 Name rsquare nb_dec 3             cyan
    FIELD REAL nc 8 Name dw      nb_dec 3             cyan
    FIELD REAL nc 8 Name fstat   nb_dec 3             cyan
    FIELD REAL nc 8 Name llik    nb_dec 3             cyan

    $BUTTONF1    "Help      "
    $BUTTONESC   "Quit      "
    $BUTTONF3    "Unit Root "
    $BUTTONF4    "Coefs     "
    $BUTTONF5    "Estimate  "
    $BUTTONF6    "Next      "
    $BUTTONF7    "Dyn. Adj. "
    $BUTTONF8    "Results   "
    $BUTTONF10   "Save      "
}

PAGE vkp_eqs_gr {
    Title "Print estimation graph"
    $PGHEAD
    FNKEY {
	    F1             "Help"
	    ESCAPE {ABORT} "Cancel"
	    F3     {ABORT} "Cancel"
	    F6     {C_FN SB_FilePrintSetup();}
	    F10
	  }
    END_FN {C_FN return(ODE_blk_gr_print());}
    SCREEN {
ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ              P R I N T   E S T I M A T I O N   G R A P H                  บ
ฬออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออัออออออออออออน
บ                                                              ณ &F10       บ
บ                                                              ณ            บ
บ Graphic to print $type                                       ณ &ESC       บ
บ                                                              ณ            บ
บ Language         $lang                                       ณ &F5        บ
บ                                                              ณ            บ
บ                                                              ณ &F1        บ
บ                                                              ณ            บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออฯออออออออออออผ
}
    FIELD name type MENU {
		    Auto back reverse
		    OPTION "YObs - YEst"
		    OPTION "Residuals"
		    }                           $FLDBOX
    FIELD name lang MENU vkm_lang               $FLDBOX

    $BUTTONESC " Cancel "
    $BUTTONF1  "  Help  "
    $BUTTONF10 "   OK   "
    $BUTTONF5  " Setup  "
}

PAGE vkp_eqs_print {
    Title "Print Results"
    $PGHEAD
    FNKEY {
	    F1             "Help"
	    ESCAPE {ABORT} "Cancel"
	    F3     {ABORT} "Cancel"
	    F6     {C_FN SB_FilePrintSetup();}
	    F10
	  }

    END_FN {C_FN return(ODE_blk_print_res());}
    SCREEN {
ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ                       P R I N T   R E S U L T S                         บ
ฬออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออัออออออออออออน
บ Language         $lang                                     ณ &F10       บ
บ                                                            ณ            บ
บ Print Options  - Correlation matrix of residuals $corru    ณ &ESC       บ
บ ฤฤฤฤฤฤฤฤฤฤฤฤฤ                                              ณ            บ
บ                - Correlation matrix of coefs     $corr     ณ &F5        บ
บ                                                            ณ            บ
บ                - YOBS-YCALC values               $obs      ณ            บ
บ                                                            ณ            บ
บ                - YOBS-YCALC graphic              $grobs    ณ &F1        บ
บ                                                            ณ            บ
บ                - Residuals graphic               $grres    ณ            บ
บ                                                            ณ            บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออฯออออออออออออผ
}
    FIELD name lang  MENU vkm_lang            $FLDBOX
    FIELD name corru MENU vkm_ny              $FLDBOX
    FIELD name corr  MENU vkm_ny              $FLDBOX
    FIELD name obs   MENU vkm_ny              $FLDBOX
    FIELD name grobs MENU vkm_ny              $FLDBOX
    FIELD name grres MENU vkm_ny              $FLDBOX

    $BUTTONESC " Cancel "
    $BUTTONF1  "  Help  "
    $BUTTONF10 "   OK   "
    $BUTTONF5  " Setup  "
}

PAGE vkp_listsort {
    Title "Sort List"
    $PGHEAD
    END_FN {C_FN return(C_DataListSort());}
    SCREEN {
ษอออออออออออออออออออออออออออออออออัออออออออออออป
บ        S O R T  L I S T         ณ &F10       บ
ฬอออออออออออออออออออออออออออออออออต            บ
บ Sort  $in                       ณ &ESC       บ
บ                                 ณ            บ
บ Save  $out                      ณ &F1        บ
บ                                 ณ            บ
บ       *cnt  elements            ณ            บ
บ                                 ณ            บ
ศอออออออออออออออออออออออออออออออออฯออออออออออออผ
}
    FIELD name in   nc 20 REQUIRED $FLDBOX
    FIELD name out  nc 20 REQUIRED $FLDBOX
    FIELD name cnt    integer nc 5 rjust cyan

    $BUTTONESC " Cancel "
    $BUTTONF1  "  Help  "
    $BUTTONF10 "   OK   "
}

PAGE vkip_scan {
    Title "Scan Objects"
    $PGHEAD
    END_FN {C_FN return(C_DataScan());}
    SCREEN {
ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ                SCAN OBJECTS FOR VARIABLES AND SCALARS                   บ
ฬอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออัอออออออออออออน
บ Object type to scan $type                                 ณ &F10        บ
บ                                                           ณ             บ
วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤยฤยฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤด             บ
บ List to scan    ณ$lst                                     ณ &ESC        บ
บ (empty = all)   ณ ณ                                       ณ             บ
บ                 ณ ณ                                       ณ             บ
บ                 ณ ณ                                       ณ &F1         บ
บ                 ณ ณ                                       ณ             บ
ฬอออออออออออออออออฯอฯอออออออออออออออออออออออออออออออออออออออต             บ
บ Note : The results are saved in the lists (and sub-lists) ณ             บ
บ ฤฤฤฤ   named :                                            ณ             บ
บ                variables in _EXO                          ณ             บ
บ                scalars in   _SCAL                         ณ             บ
ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออฯอออออออออออออผ
    }

    FIELD name type MENU vkm_obj        $FLDBOX
    FIELD nc 41 nl 5  name lst EDITOR SBAR blue

    $BUTTONESC " Cancel "
    $BUTTONF10 " Scan   "
    $BUTTONF1  " Help   "
}

MENU vkm_llist {
    OPTION " + "   COMMENT "Union"
    OPTION " * "   COMMENT "Intersection"
    OPTION " - "   COMMENT "Difference"
    OPTION " x "   COMMENT "Times"
}

PAGE vkp_calclist {
    Title "List Calculus"
    Center Back Cyan PROFILE
    FNKEY {
	    F1             "Help"
	    ESCAPE {ABORT} "Cancel"
	    F3     {ABORT} "Cancel"
	    F10    {C_FN C_DataCalcLst();} "Exec"
	  }
/*    END_FN {C_FN return(C_DataCalcLst());} */
    SCREEN {
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ                L I S T  C A L C U L U S                        บ
ฬอออออออออออออออออออออออออออออออออออออออออออออออออัออออออออออออออน
บ     $list1                                      ณ  &F10        บ
บ                                                 ณ              บ
บ $op $list2                                      ณ              บ
บ                                                 ณ              บ
บ ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ                 ณ              บ
บ =   $out                                        ณ              บ
บ                                                 ณ              บ
บฺฤฤฤResultฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฟ ณ              บ
บณ  Count : *cnt                                ณ ณ  &ESC        บ
บณ  *res                                        ณ ณ              บ
บณ                                              ณ ณ              บ
บณ                                              ณ ณ  &F1         บ
บณ                                              ณ ณ              บ
บภฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤู ณ              บ
ศอออออออออออออออออออออออออออออออออออออออออออออออออฯออออออออออออออผ
}
    FIELD name list1 nc 20                    $FLDBOX
    FIELD name op MENU vkm_llist
    FIELD name list2 nc 20                    $FLDBOX
    FIELD name out nc 20 upper "_RES"         $FLDBOX

    GROUP {
	Title "Result"
	FIELDS {
	    FIELD name res nc 40 nl 4
	    FIELD name cnt integer nc 5 cyan rjust
	    }
	}

    $BUTTONESC " Cancel "
    $BUTTONF10 "  Exec  "
    $BUTTONF1  "  Help  "
}



/* test */
PAGE vkp_editc{
    Title "Object Edition"
    $PGHEAD
    END_FN {C_FN return(C_DataEditCil1());}
    SCREEN {
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ                        E D I T   O B J E C T                             บ
ฬออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออัอออออออออออน
บObject type : *type        Object name : $name                ณ &F10      บ
บ                                                              ณ           บ
วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤด &ESC      บ
บ                    Object Definition                         ณ           บ
วฤยฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤด           บ
บ$val                                                          ณ &F1       บ
บ ณ                                                            ณ           บ
บ ณ                                                            ณ           บ
บ ณ                                                            ณ           บ
บ ณ                                                            ณ           บ
บ ณ                                                            ณ           บ
บ ณ                                                            ณ           บ
บ ณ                                                            ณ           บ
ศอฯออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออฯอออออออออออผ
}
    FIELD name type MENU vkm_obj        $FLDBOX
    FIELD name name nc 20 required      $FLDBOX
    FIELD name val  nc 61 nl 8 upper EDITOR SBAR blue


    $BUTTONESC " Cancel "
    $BUTTONF1  "  Help  "
    $BUTTONF10 "   OK   "
}
PAGE vkp_edite{
    Title "Edit Object"
    $PGHEAD
    END_FN {C_FN return(C_DataEditCil1());}
    SCREEN {
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ                        E D I T   O B J E C T                             บ
ฬออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออัอออออออออออน
บObject type : *type        Object name : $name                ณ &F10      บ
บ                                                              ณ           บ
วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤด &ESC      บ
บ                    Object Definition                         ณ           บ
วฤยฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤด           บ
บ$val                                                          ณ &F1       บ
บ ณ                                                            ณ           บ
บ ณ                                                            ณ           บ
บ ณ                                                            ณ           บ
บ ณ                                                            ณ           บ
บ ณ                                                            ณ           บ
บ ณ                                                            ณ           บ
บ ณ                                                            ณ           บ
ศอฯออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออฯอออออออออออผ
}
    FIELD name type MENU vkm_obj        $FLDBOX
    FIELD name name nc 20 required      $FLDBOX
    FIELD name val  nc 61 nl 8 upper EDITOR SBAR blue


    $BUTTONESC " Cancel "
    $BUTTONF1  "  Help  "
    $BUTTONF10 "   OK   "
}
PAGE vkp_editi{
    Title "Edit Object"
    $PGHEAD
    END_FN {C_FN return(C_DataEditCil1());}
    SCREEN {
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ                        E D I T   O B J E C T                             บ
ฬออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออัอออออออออออน
บObject type : *type        Object name : $name                ณ &F10      บ
บ                                                              ณ           บ
วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤด &ESC      บ
บ                    Object Definition                         ณ           บ
วฤยฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤด           บ
บ$val                                                          ณ &F1       บ
บ ณ                                                            ณ           บ
บ ณ                                                            ณ           บ
บ ณ                                                            ณ           บ
บ ณ                                                            ณ           บ
บ ณ                                                            ณ           บ
บ ณ                                                            ณ           บ
บ ณ                                                            ณ           บ
ศอฯออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออฯอออออออออออผ
}
    FIELD name type MENU vkm_obj        $FLDBOX
    FIELD name name nc 20 required      $FLDBOX
    FIELD name val  nc 61 nl 8 upper EDITOR SBAR blue


    $BUTTONESC " Cancel "
    $BUTTONF1  "  Help  "
    $BUTTONF10 "   OK   "
}
PAGE vkp_editl{
    Title "Edit Object"
    $PGHEAD
    END_FN {C_FN return(C_DataEditCil1());}
    SCREEN {
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ                        E D I T   O B J E C T                             บ
ฬออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออัอออออออออออน
บObject type : *type        Object name : $name                ณ &F10      บ
บ                                                              ณ           บ
วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤด &ESC      บ
บ                    Object Definition                         ณ           บ
วฤยฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤด           บ
บ$val                                                          ณ &F1       บ
บ ณ                                                            ณ           บ
บ ณ                                                            ณ           บ
บ ณ                                                            ณ           บ
บ ณ                                                            ณ           บ
บ ณ                                                            ณ           บ
บ ณ                                                            ณ           บ
บ ณ                                                            ณ           บ
ศอฯออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออฯอออออออออออผ
}
    FIELD name type MENU vkm_obj        $FLDBOX
    FIELD name name nc 20 required      $FLDBOX
    FIELD name val  nc 61 nl 8 upper EDITOR SBAR blue


    $BUTTONESC " Cancel "
    $BUTTONF1  "  Help  "
    $BUTTONF10 "   OK   "
}
PAGE vkp_edits{
    Title "Edit Object"
    $PGHEAD
    END_FN {C_FN return(C_DataEditCil1());}
    SCREEN {
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ                        E D I T   O B J E C T                             บ
ฬออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออัอออออออออออน
บObject type : *type        Object name : $name                ณ &F10      บ
บ                                                              ณ           บ
วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤด &ESC      บ
บ                    Object Definition                         ณ           บ
วฤยฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤด           บ
บ$val                                                          ณ &F1       บ
บ ณ                                                            ณ           บ
บ ณ                                                            ณ           บ
บ ณ                                                            ณ           บ
บ ณ                                                            ณ           บ
บ ณ                                                            ณ           บ
บ ณ                                                            ณ           บ
บ ณ                                                            ณ           บ
ศอฯออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออฯอออออออออออผ
}
    FIELD name type MENU vkm_obj        $FLDBOX
    FIELD name name nc 20 required      $FLDBOX
    FIELD name val  nc 61 nl 8 upper EDITOR SBAR blue


    $BUTTONESC " Cancel "
    $BUTTONF1  "  Help  "
    $BUTTONF10 "   OK   "
}
PAGE vkp_editt{
    Title "Edit Object"
    $PGHEAD
    END_FN {C_FN return(C_DataEditCil1());}
    SCREEN {
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ                        E D I T   O B J E C T                             บ
ฬออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออัอออออออออออน
บObject type : *type        Object name : $name                ณ &F10      บ
บ                                                              ณ           บ
วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤด &ESC      บ
บ                    Object Definition                         ณ           บ
วฤยฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤด           บ
บ$val                                                          ณ &F1       บ
บ ณ                                                            ณ           บ
บ ณ                                                            ณ           บ
บ ณ                                                            ณ           บ
บ ณ                                                            ณ           บ
บ ณ                                                            ณ           บ
บ ณ                                                            ณ           บ
บ ณ                                                            ณ           บ
ศอฯออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออฯอออออออออออผ
}
    FIELD name type MENU vkm_obj        $FLDBOX
    FIELD name name nc 20 required      $FLDBOX
    FIELD name val  nc 61 nl 8 upper EDITOR SBAR blue


    $BUTTONESC " Cancel "
    $BUTTONF1  "  Help  "
    $BUTTONF10 "   OK   "
}
PAGE vkp_editv{
    Title "Edit Object"
    $PGHEAD
    END_FN {C_FN return(C_DataEditCil1());}
    SCREEN {
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ                        E D I T   O B J E C T                             บ
ฬออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออัอออออออออออน
บObject type : *type        Object name : $name                ณ &F10      บ
บ                                                              ณ           บ
วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤด &ESC      บ
บ                    Object Definition                         ณ           บ
วฤยฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤด           บ
บ$val                                                          ณ &F1       บ
บ ณ                                                            ณ           บ
บ ณ                                                            ณ           บ
บ ณ                                                            ณ           บ
บ ณ                                                            ณ           บ
บ ณ                                                            ณ           บ
บ ณ                                                            ณ           บ
บ ณ                                                            ณ           บ
ศอฯออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออฯอออออออออออผ
}
    FIELD name type MENU vkm_obj        $FLDBOX
    FIELD name name nc 20 required      $FLDBOX
    FIELD name val  nc 61 nl 8 upper EDITOR SBAR blue


    $BUTTONESC " Cancel "
    $BUTTONF1  "  Help  "
    $BUTTONF10 "   OK   "
}






