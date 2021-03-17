
PAGE vkp_wscopy {
    TITLE "Copy Objects in WS"
    $PGHEAD
    END_FN {C_FN return(C_WsCopy());}
    SCREEN {
ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ            C O P Y     O B J E C T S   I N T O   W S                      บ
ฬอออออออออออออัอออออออออออออออออออออออออออออออออออออออออออออออออัอออออออออออน
บ Object type ณ $type                                           ณ &F10      บ
บ             ณ                                                 ณ           บ
บ Input file  ณ $file                                           ณ &ESC      บ
บ             ณ                                                 ณ           บ
บ Sample      ณ From $from      To $to                          ณ &F1       บ
บ             ณ                                                 ณ           บ
วฤฤฤฤฤฤฤฤฤฤฤฤฤลฤยฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤด           บ
บ Object namesณ$objs                                            ณ           บ
บ             ณ ณ                                               ณ           บ
บ             ณ ณ                                               ณ           บ
บ             ณ ณ                                               ณ           บ
บ             ณ ณ                                               ณ           บ
ศอออออออออออออฯอฯอออออออออออออออออออออออออออออออออออออออออออออออฯอออออออออออผ
}
    FIELD  name type   MENU vkm_obj $FLDBOX
    FIELD  name file   nc 255 ncv 46 dir    $FLDBOX
    FIELD  name objs   nc 41 nl 5 EDITOR SBAR
    FIELD  name from nc 10 "" $FLDBOX
    FIELD  name to   nc 10 "" $FLDBOX

    $BUTTONESC " Cancel "
    $BUTTONF10 "  Copy  "
    $BUTTONF1  "  Help  "
}

PAGE vkp_wsmerge {
    TITLE "Merge File into WS"
    $PGHEAD
    END_FN {C_FN return(C_WsMerge());}
    SCREEN {
ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ                 M E R G E   F I L E   I N T O   W S                       บ
ฬอออออออออออออัอออออออออออออออออออออออออออออออออออออออออออออออออัอออออออออออน
บ Object type ณ $type                                           ณ &F10      บ
บ             ณ                                                 ณ           บ
บ Input file  ณ $file                                           ณ &ESC      บ
บ             ณ                                                 ณ           บ
บ             ณ                                                 ณ &F1       บ
บ             ณ                                                 ณ           บ
ศอออออออออออออฯอออออออออออออออออออออออออออออออออออออออออออออออออฯอออออออออออผ
}
    FIELD  name type   MENU vkm_obj $FLDBOX
    FIELD  name file   nc 255 ncv 47 dir    $FLDBOX
    $BUTTONESC " Cancel "
    $BUTTONF10 "   Ok   "
    $BUTTONF1  "  Help  "
}


PAGE vkp_wsload {
    TITLE "Load Workspaces"
    $PGHEAD
    FNKEY _fk_all
    Comment "F1=Help ESC=Cancel F10=Load All F2(..F8)=Load Cmts(..Vars)"
    END_FN {C_FN return(C_WsLoad(SCR_LKEY));}

    SCREEN {
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ                            L O A D   W S                                 บ
ฬออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออัอออออออออออน
บ &F2                $cmt                                      ณ           บ
บ                                                              ณ           บ
บ &F3                $eqs                                      ณ&F10       บ
บ                                                              ณ           บ
บ &F4                $idt                                      ณ&ESC       บ
บ                                                              ณ           บ
บ &F5                $lst                                      ณ&F1        บ
บ                                                              ณ           บ
บ &F6                $scl                                      ณ           บ
บ                                                              ณ           บ
บ &F7                $tbl                                      ณ           บ
บ                                                              ณ           บ
บ &F8                $var                                      ณ           บ
บ                                                              ณ           บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออฯอออออออออออผ
}
    NO_VIEW_ORDER
    FIELD NAME cmt nc 255 ncv 40 input DIR $FLDBOX
    FIELD NAME eqs nc 255  ncv 40 input DIR $FLDBOX
    FIELD NAME idt nc 255  ncv 40 input DIR $FLDBOX
    FIELD NAME lst nc 255  ncv 40 input DIR $FLDBOX
    FIELD NAME scl nc 255  ncv 40 input DIR $FLDBOX
    FIELD NAME tbl nc 255  ncv 40 input DIR $FLDBOX
    FIELD NAME var nc 255  ncv 40 input DIR $FLDBOX

    $BUTTONF2  " Load Comments   "
    $BUTTONF3  " Load Equations  "
    $BUTTONF4  " Load Identities "
    $BUTTONF5  " Load Lists      "
    $BUTTONF6  " Load Scalars    "
    $BUTTONF7  " Load Tables     "
    $BUTTONF8  " Load Variables  "
    $BUTTONF10 " Load All "
    $BUTTONESC "  Cancel  "
    $BUTTONF1  "   Help   "
}

PAGE vkp_wssave {
    TITLE "Save Workspaces"
    $PGHEAD
    END_FN     {C_FN return(C_WsSave());}
    FNKEY _fk_all
    Comment "F1=Help ESC=Cancel F10=Save All F2(..F8)=Save Cmts(..Vars)"
    SCREEN {
ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ          S A V E   W O R K S P A C E S   I N   F I L E S                บ
ฬอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออัอออออออออออน
บ             FILENAME                              Cmp NbObjsณ           บ
วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤด           บ
บ&F2          $cmt                                  $cc *cnb  ณ&F10       บ
บ                                                             ณ           บ
บ&F3          $eqs                                  $ce *enb  ณ&ESC       บ
บ                                                             ณ           บ
บ&F4          $idt                                  $ci *inb  ณ&F9        บ
บ                                                             ณ           บ
บ&F5          $lst                                  $cl *lnb  ณ           บ
บ                                                             ณ           บ
บ&F6          $scl                                  $cs *snb  ณ&F1        บ
บ                                                             ณ           บ
บ&F7          $tbl                                  $ct *tnb  ณ           บ
บ                                                             ณ           บ
บ&F8          $var                                  $cv *vnb  ณ           บ
บ                                                             ณ           บ
ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออฯอออออออออออผ
}

    FIELD NAME cnb natural nc 6 rjust output $FLDNBOX green
    FIELD NAME enb natural nc 6 rjust output $FLDNBOX green
    FIELD NAME inb natural nc 6 rjust output $FLDNBOX green
    FIELD NAME lnb natural nc 6 rjust output $FLDNBOX green
    FIELD NAME snb natural nc 6 rjust output $FLDNBOX green
    FIELD NAME tnb natural nc 6 rjust output $FLDNBOX green
    FIELD NAME vnb natural nc 6 rjust output $FLDNBOX green

    FIELD NAME  cmt nc 255 ncv 36 input DIR $FLDBOX
    FIELD NAME  eqs nc 255 ncv 36 input DIR $FLDBOX
    FIELD NAME  idt nc 255 ncv 36 input DIR $FLDBOX
    FIELD NAME  lst nc 255 ncv 36 input DIR $FLDBOX
    FIELD NAME  scl nc 255 ncv 36 input DIR $FLDBOX
    FIELD NAME  tbl nc 255 ncv 36 input DIR $FLDBOX
    FIELD NAME  var nc 255 ncv 36 input DIR $FLDBOX

    FIELD NAME cc checkbox $FLDNBOX
    FIELD NAME ce checkbox $FLDNBOX
    FIELD NAME ci checkbox $FLDNBOX
    FIELD NAME cl checkbox $FLDNBOX
    FIELD NAME cs checkbox $FLDNBOX
    FIELD NAME ct checkbox $FLDNBOX
    FIELD NAME cv checkbox $FLDNBOX

/*    FIELD NAME  dbl input check_box */

    $BUTTONF2  " Save Cmts "
    $BUTTONF3  " Save Eqs  "
    $BUTTONF4  " Save Idts "
    $BUTTONF5  " Save Lsts "
    $BUTTONF6  " Save Scls "
    $BUTTONF7  " Save Tbls "
    $BUTTONF8  " Save Vars "
    $BUTTONF10 " Save All "
    $BUTTONESC "  Cancel  "
    $BUTTONF1  "   Help   "
/*    $BUTTONF9  " Descr.-> " */
}

PAGE vkp_wsdescr {
    TITLE "Change Workspaces Descriptions"
    $PGHEAD
    END_FN     {C_FN return(C_WsDescr());}
    SCREEN {
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ      C H A N G E    W O R K S P A C E S    D E S C R I P T I O N S       บ
ฬออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออัอออออออออน
บ Comments   $dcmt                                               ณ&F10     บ
บ                                                                ณ         บ
บ Equations  $deqs                                               ณ&ESC     บ
บ                                                                ณ         บ
บ Identities $didt                                               ณ&F1      บ
บ                                                                ณ         บ
บ Lists      $dlst                                               ณ         บ
บ                                                                ณ         บ
บ Scalars    $dscl                                               ณ         บ
บ                                                                ณ         บ
บ Tables     $dtbl                                               ณ         บ
บ                                                                ณ         บ
บ Variables  $dvar                                               ณ         บ
บ                                                                ณ         บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออฯอออออออออผ
}

    FIELD NAME dcmt nc 50 input $FLDBOX
    FIELD NAME deqs nc 50 input $FLDBOX
    FIELD NAME didt nc 50 input $FLDBOX
    FIELD NAME dlst nc 50 input $FLDBOX
    FIELD NAME dscl nc 50 input $FLDBOX
    FIELD NAME dtbl nc 50 input $FLDBOX
    FIELD NAME dvar nc 50 input $FLDBOX

    $BUTTONESC " Cancel "
    $BUTTONF1  "  Help  "
    $BUTTONF10 "   Ok   "
}

PAGE vkp_wsclear {
    TITLE "Clear Workspaces"
    $PGHEAD
/*    DISPLAY_FN {C_FN C_InitWsClear();} /* JMP_M 4.21 01-11-95 */
    END_FN   {C_FN return(C_WsClear());}
    FNKEY _fk_all
    Comment "F1=Help ESC=Cancel F10=Clear All F2(..F8)=Clear Cmts(..Vars)"
    SCREEN {
ษอออออออออออออออออออออออออออออออออออออออออออออออป
บ      C L E A R   W O R K   S P A C E S        บ
วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤยฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
บ &F2           [*cnb  objects] ณ &F10          บ
บ                               ณ               บ
บ &F3           [*enb  objects] ณ &ESC          บ
บ                               ณ               บ
บ &F4           [*inb  objects] ณ &F1           บ
บ                               ณ               บ
บ &F5           [*lnb  objects] ณ               บ
บ                               ณ               บ
บ &F6           [*snb  objects] ณ               บ
บ                               ณ               บ
บ &F7           [*tnb  objects] ณ               บ
บ                               ณ               บ
บ &F8           [*vnb  objects] ณ               บ
บ                               ณ               บ
ศอออออออออออออออออออออออออออออออฯอออออออออออออออผ
}
    FIELD NAME cnb natural nc 5 rjust output
    FIELD NAME enb natural nc 5 rjust output
    FIELD NAME inb natural nc 5 rjust output
    FIELD NAME lnb natural nc 5 rjust output
    FIELD NAME snb natural nc 5 rjust output
    FIELD NAME tnb natural nc 5 rjust output
    FIELD NAME vnb natural nc 5 rjust output

    $BUTTONF2  " Clear Cmts "
    $BUTTONF3  " Clear Eqs  "
    $BUTTONF4  " Clear Idts "
    $BUTTONF5  " Clear Lsts "
    $BUTTONF6  " Clear Scls "
    $BUTTONF7  " Clear Tbls "
    $BUTTONF8  " Clear Vars "
    $BUTTONF10 " Clear All "
    $BUTTONESC "   Cancel  "
    $BUTTONF1  "    Help   "
}

PAGE vkp_wssample {
    TITLE "Change Variables Sample"
    $PGHEAD
/*    DISPLAY_FN {C_FN C_InitWsSample();} /* JMP_M 4.21 01-11-95 */
    END_FN   {C_FN return(C_WsSample());}
    SCREEN {
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ        C H A N G E   V A R I A B L E S   S A M P L E           บ
ฬออออออออออออออออออออออออออออออออออออออออออออออออออออัอออออออออออน
บ New sample :         From $nfrom    To $nto        ณ &F10      บ
บ                                                    ณ           บ
บ Current sample :     From *ofrom    To *oto        ณ &ESC      บ
บ                                                    ณ           บ
วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤด &F1       บ
บ Sample format : yyyyPppp                           ณ           บ
บ        - yyyy is the year                          ณ           บ
บ        - P is the periodicity :                    ณ           บ
บ            Y=Yearly, Q=Quaterly,                   ณ           บ
บ            M=Monthly, W=Weekly                     ณ           บ
บ        - ppp is the period in the year             ณ           บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออฯอออออออออออผ
}
    FIELD NAME ofrom nc 8 output $FLDBOX green
    FIELD NAME oto   nc 8 output $FLDBOX green
    FIELD NAME nfrom nc 8 input allowed "0123456789 YQMW" upper $FLDBOX "1991Y1  "
    FIELD NAME nto   nc 8 input allowed "0123456789 YQMW" upper $FLDBOX "2030Y1  "

    $BUTTONESC " Cancel "
    $BUTTONF1  "  Help  "
    $BUTTONF10 "   Ok   "
}

PAGE vkp_asksample {
    TITLE "Variables Sample"
    $PGHEAD
    SCREEN {
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ      S P E C I F Y   V A R I A B L E S   S A M P L E           บ
ฬออออออออออออออออออออออออออออออออออออออออออออออออออออัอออออออออออน
บ Sample :         From $from        To $to          ณ &F10      บ
บ                                                    ณ           บ
วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤด &ESC      บ
บ Sample format : yyyyPppp                           ณ           บ
บ        - yyyy is the year                          ณ &F1       บ
บ        - P is the periodicity :                    ณ           บ
บ            Y=Yearly, Q=Quaterly,                   ณ           บ
บ            M=Monthly, W=Weekly                     ณ           บ
บ        - ppp is the period in the year             ณ           บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออฯอออออออออออผ
}
    FIELD NAME from nc 8 input allowed "0123456789 YQMW" upper $FLDBOX
    FIELD NAME to   nc 8 input allowed "0123456789 YQMW" upper $FLDBOX

    $BUTTONESC " Cancel "
    $BUTTONF1  "  Help  "
    $BUTTONF10 "   Ok   "
}


PAGE vkp_extrap {
    TITLE "Extrapolate Variables"
    $PGHEAD
    FNKEY  {
	F1             "Help"
	ESCAPE {ABORT} "Quit"
	F3     {ABORT} "Quit"
	F10            "Extrapolate"
    }
    END_FN {C_FN return(C_WsExtrapolate());}
    SCREEN {
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ               E X T R A P O L A T E  V A R I A B L E S                 บ
ฬออออออออออออออออออออออออออออออออออออออออออออออออออออออออออัอออออออออออออน
บ Extrapolate From $from     To $to                        ณ &F10        บ
บ                                                          ณ             บ
บ Method           $method                                 ณ &ESC        บ
บ                                                          ณ             บ
วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤยฤยฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤด             บ
บ Variables list  ณ$vars                                   ณ &F1         บ
บ (Empty=>All)    ณ ณ                                      ณ             บ
บ                 ณ ณ                                      ณ             บ
บ                 ณ ณ                                      ณ             บ
ศอออออออออออออออออฯอฯออออออออออออออออออออออออออออออออออออออฯอออออออออออออผ
}

    FIELD name from   "1990Y1  " allowed "0123456789YSQM " upper $FLDBOX
    FIELD name to     "1995Y1  " allowed "0123456789YSQM " upper $FLDBOX
    FIELD name vars nc 40 nl 4 upper EDITOR SBAR                 blue
    FIELD name method            MENU vkm_extrap                 $FLDBOX

    $BUTTONESC " Cancel "
    $BUTTONF1  "  Help  "
    $BUTTONF10 "   OK   "
}

PAGE vkp_wshtol {
    Title "High To Low Frequency"
    $PGHEAD
    END_FN {C_FN return(C_WsHtoL());}
    SCREEN {
ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ                      H I G H   T O   L O W                              บ
ฬอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออัอออออออออออน
บ Input file :      $file                                     ณ &F10      บ
บ                                                             ณ           บ
บ     METHOD                 LIST OF VARIABLES                ณ           บ
บฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤยฤยฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤด           บ
บ Last Observation ณ$last                                     ณ &ESC      บ
บ                  ณ ณ                                        ณ           บ
บ                  ณ ณ                                        ณ           บ
บฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤลฤลฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤด           บ
บ Mean             ณ$mean                                     ณ &F1       บ
บ                  ณ ณ                                        ณ           บ
บ                  ณ ณ                                        ณ           บ
บฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤลฤลฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤด           บ
บ Sum              ณ$sum                                      ณ           บ
บ                  ณ ณ                                        ณ           บ
บ                  ณ ณ                                        ณ           บ
ศออออออออออออออออออฯอฯออออออออออออออออออออออออออออออออออออออออฯอออออออออออผ
}
    FIELD  name file   nc 255 ncv 40 dir    $FLDBOX
    FIELD  name last   nc 41 nl 3 EDITOR SBAR
    FIELD  name mean   nc 41 nl 3 EDITOR SBAR
    FIELD  name sum    nc 41 nl 3 EDITOR SBAR

    $BUTTONESC " Cancel "
    $BUTTONF10 " H to L "
    $BUTTONF1  "  Help  "
}

MENU vkm_int {
    Option "Linear Interpolation"
    Option "Cubic Splining"
    Option "Step"
}

PAGE vkp_wsltoh {
    Title "Low to High Frequency"
    $PGHEAD
    END_FN {C_FN return(C_WsLtoH());}
    SCREEN {
ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ                      L O W   T O   H I G H                              บ
ฬอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออัอออออออออออน
บ Input file:       $file                                     ณ &F10      บ
บ                                                             ณ           บ
บ Method:           $method                                   ณ &ESC      บ
บ                                                             ณ           บ
บฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤยฤยฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤด           บ
บ Stock series     ณ$stock                                    ณ &F1       บ
บ                  ณ ณ                                        ณ           บ
บ                  ณ ณ                                        ณ           บ
บฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤลฤลฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤด           บ
บ Flow series      ณ$flow                                     ณ           บ
บ                  ณ ณ                                        ณ           บ
บ                  ณ ณ                                        ณ           บ
ศออออออออออออออออออฯอฯออออออออออออออออออออออออออออออออออออออออฯอออออออออออผ
}
    FIELD  name file   nc 255 ncv 40 dir    $FLDBOX
    FIELD  name method menu  vkm_int $FLDBOX
    FIELD  name stock  nc 41 nl 3 EDITOR SBAR
    FIELD  name flow   nc 41 nl 3 EDITOR SBAR

    $BUTTONESC " Cancel "
    $BUTTONF10 " L to H "
    $BUTTONF1  "  Help  "
}

PAGE vkp_wsseasadj {
    Title "Census II - Seasonal Adjustment"
    $PGHEAD
    END_FN {C_FN return(C_WsSeasonAdj());}
    SCREEN {
ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ         C E N S U S  II   S E A S O N A L   A D J U S T M E N T         บ
ฬอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออัอออออออออออน
บ Input file :      $file                                     ณ &F10      บ
บ                                                             ณ           บ
บฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤยฤยฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤด &ESC      บ
บ Series           ณ$series                                   ณ           บ
บ                  ณ ณ                                        ณ &F1       บ
บ                  ณ ณ                                        ณ           บ
บ                  ณ ณ                                        ณ           บ
บ                  ณ ณ                                        ณ           บ
บ                  รฤมฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤด           บ
บ EPS-test         ณ$eps                                      ณ           บ
บ                  ณ                                          ณ           บ
ศออออออออออออออออออฯออออออออออออออออออออออออออออออออออออออออออฯอออออออออออผ
}
    FIELD   name file   nc 255 ncv 40 dir    $FLDBOX
    FIELD   name series nc 41 nl 5 EDITOR SBAR
    FIELD   name eps    nc 3  "5.0" ALLOWED "012456789."

    $BUTTONESC " Cancel "
    $BUTTONF10 "   GO   "
    $BUTTONF1  "  Help  "
}

PAGE vkp_wstrend {
    Title "Hodrick-Prescott Trend Filter"
    $PGHEAD
    END_FN {C_FN return(C_WsTrend());}
    SCREEN {
ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ         T R E N D -  H O D R I C K  -  P R E S C O T T   F I L T E R    บ
ฬอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออัอออออออออออน
บ Input file :      $file                                     ณ &F10      บ
บ                                                             ณ           บ
บฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤยฤยฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤด &ESC      บ
บ Series           ณ$series                                   ณ           บ
บ                  ณ ณ                                        ณ &F1       บ
บ                  ณ ณ                                        ณ           บ
บ                  ณ ณ                                        ณ           บ
บ                  ณ ณ                                        ณ           บ
บ                  รฤมฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤด           บ
บ Lambda           ณ$lambda                                   ณ           บ
บ                  ณ                                          ณ           บ
บ Log              ณ$log                                      ณ           บ
บ                  ณ                                          ณ           บ
ศออออออออออออออออออฯออออออออออออออออออออออออออออออออออออออออออฯอออออออออออผ
}
    FIELD  name file   nc 255 ncv 40 dir    $FLDBOX
    FIELD  name series nc 41 nl 5 EDITOR SBAR
    FIELD  name lambda nc 10 REAL /* NATURAL */ $FLDBOX
    FIELD  name log   Menu vkm_ny       $FLDBOX

    $BUTTONESC " Cancel "
    $BUTTONF10 "   GO   "
    $BUTTONF1  "  Help  "
}

PAGE vkp_unitroot {
    Title "Unit Root Test"
    $PGHEAD
    FNKEY {
	    F1             "Help"
	    ESCAPE {ABORT} "Cancel"
	    F3     {ABORT} "Cancel"
	    F10    {C_FN C_StatUnitRoot();} "Analyse"
	  }
    SCREEN {
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ    U N I T   R O O T   T E S T                                     บ
ฬออออออออออออออออออออออออออออออออออออออออออออออออออออออออัอออออออออออน
บ Series  $series                                        ณ &F10      บ
บ                                                        ณ           บ
บ Order   $order                                         ณ           บ
บฤยฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤด &ESC      บ
บ$res                                                    ณ           บ
บ ณ                                                      ณ           บ
บ ณ                                                      ณ &F1       บ
บ ณ                                                      ณ           บ
บ ณ                                                      ณ           บ
บ ณ                                                      ณ           บ
บ ณ                                                      ณ           บ
บ ณ                                                      ณ           บ
บ ณ                                                      ณ           บ
บ ณ                                                      ณ           บ
บ ณ                                                      ณ           บ
บ ณ                                                      ณ           บ
บ ณ                                                      ณ           บ
ศอฯออออออออออออออออออออออออออออออออออออออออออออออออออออออฯอออออออออออผ
}
    FIELD NAME series nc 255 ncv 45  $FLDBOX
    FIELD NAME order integer

    FIELD nc 55 nl 12 name res EDITOR SBAR

    $BUTTONESC " Cancel  "
    $BUTTONF10 " Analyse "
    $BUTTONF1  "  Help   "
}

/*
PAGE vkp_wstimeseries {
    Title "Time Series Analysis"
    $PGHEAD
    FNKEY {
	    F1             "Help"
	    ESCAPE {ABORT} "Cancel"
	    F3     {ABORT} "Cancel"
	    F10    {C_FN C_WsTimeSeries();} "Analyse"
	  }
    SCREEN {
ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ         S T A S T I S T I C A L   A N A L Y S I S  O F  S E R I E S     บ
ฬอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออัอออออออออออน
บ Series  $series                                             ณ &F10      บ
บ                                                             ณ           บ
บ From    $from            To $to                             ณ           บ
บ                                                             ณ           บ
บ Lagged  scatter plot        $lag                            ณ           บ
บ                                                             ณ           บ
บฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤด &ESC      บ
บ Mean      *mean    DF     *df                               ณ           บ
บ                                                             ณ           บ
บ Variance  *var     Stderr *stderr   Stddev *stddev          ณ &F1       บ
บ                                                             ณ           บ
ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออฯอออออออออออผ
}
    FIELD NAME series nc 255 ncv 40  $FLDBOX
    FIELD NAME from nc 8 input allowed "0123456789 YQMW" upper $FLDBOX
    FIELD NAME to   nc 8 input allowed "0123456789 YQMW" upper $FLDBOX
    FIELD NAME lag nc 2 input allowed " 123456789"             $FLDBOX

    FIELD REAL nc 8 Name mean   nb_dec 3   cyan
    FIELD REAL nc 8 Name var    nb_dec 3   cyan
    FIELD REAL nc 8 Name stderr nb_dec 3   cyan
    FIELD REAL nc 8 Name stddev nb_dec 3   cyan
    FIELD REAL nc 8 Name df     nb_dec 3   cyan

    $BUTTONESC " Cancel  "
    $BUTTONF10 " Analyse "
    $BUTTONF1  "  Help   "
}

*/









