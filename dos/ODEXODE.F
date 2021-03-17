MENU xm_obj {
    AUTO
    OPTION  "Comments"
    OPTION  "Equations"
    OPTION  "Identities"
    OPTION  "Lists"
    OPTION  "Scalars"
    OPTION  "Tables"
    OPTION  "Variables"
}

MENU xm_format {
    OPTION "Ascii"
    OPTION "Troll" OFF
    OPTION "TSP"   OFF
}

MENU xm_ruleformat {
    OPTION "Ascii"
    OPTION "Rotated Ascii"
    OPTION "DIF"
    OPTION "DIF Belgostat"
    OPTION "NIS"
    OPTION "GEM (chronos)"
    OPTION "PRN-AREMOS"
    OPTION "TXT-BELGOSTAT"
}

MENU xm_rulelang {
    OPTION "English"     OFF
    OPTION "French"
    OPTION "Dutch"
}

/*
PAGE xp_import {
    $PGHEAD
    END_FN {C_FN return(C_XodeImport());}

    SCREEN {
ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ                  I M P O R T   A S C I I   F I L E S                    บ
ฬอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออัอออออออออออน
บ Object type  $type                                          ณ &F10      บ
บ                                                             ณ           บ
บ Input file   $file                                          ณ &ESC      บ
บ                                                             ณ           บ
บ Input format $format                                        ณ           บ
บ                                                             ณ           บ
บ List file    $list                                          ณ &F1       บ
บ (optional)                                                  ณ           บ
วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤด           บ
บ Save in IODE file $ode                                      ณ           บ
บ                                                             ณ           บ
ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออฯอออออออออออผ
}
    FIELD MENU  xm_obj                      name type   $FLDBOX
    FIELD string nc 63 ncv 46 dir required "*.asc" name file   $FLDBOX
    FIELD MENU xm_format                    name format $FLDBOX
    FIELD string nc 63 ncv 41 dir required         name ode    $FLDBOX
    FIELD string nc 63 ncv 46 dir "   "            name list   $FLDBOX

    $BUTTONESC " Cancel "
    $BUTTONF10 " Ok     "
    $BUTTONF1  " Help   "
}
*/
PAGE xp_ruleimport {
    $PGHEAD
    END_FN {C_FN return(C_XodeRuleImport());}

    SCREEN {
ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ                I M P O R T   F I L E   B Y   R U L E                    บ
ฬออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออัออออออออออออน
บ Input type   $type         Input Format $format            ณ &F10       บ
บ                                                            ณ            บ
บ Input file   $file                                         ณ            บ
บ                                                            ณ            บ
บ Rule file    $rule                                         ณ            บ
บ                                                            ณ &ESC       บ
วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤด            บ
บ Save in IODE file $ode                                     ณ            บ
บ                                                            ณ            บ
บ Output sample  From $from       To $to                     ณ &F1        บ
บ                                                            ณ            บ
บ Output Language     $lang                                  ณ            บ
บ                                                            ณ            บ
วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤด            บ
บ Debug file  $trace                                         ณ            บ
บ                                                            ณ            บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออฯออออออออออออผ
}
    FIELD MENU  xm_obj                      name type   $FLDBOX
    FIELD string nc 63 ncv 44 dir required "*.*"   name file   $FLDBOX
    FIELD string nc 63 ncv 44 dir          ""      name rule   $FLDBOX
    FIELD MENU xm_ruleformat                name format $FLDBOX
    FIELD MENU xm_rulelang                  name lang   $FLDBOX
    FIELD string nc 10              ""      name from   $FLDBOX
    FIELD string nc 10              ""      name to     $FLDBOX
    FIELD string nc 63 ncv 40 dir required         name ode    $FLDBOX
    FIELD string nc 40                      name trace  $FLDBOX

    $BUTTONESC " Cancel "
    $BUTTONF10 " Ok     "
    $BUTTONF1  " Help   "
}
/*
PAGE xp_export {
    $PGHEAD
    END_FN {C_FN return(C_XodeExport());}

    SCREEN {
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ                        E X P O R T   F I L E S                           บ
ฬอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออัออออออออออออน
บ Object type $type                                           ณ &F10       บ
บ                                                             ณ            บ
บ IODE file   $ode                                            ณ &ESC       บ
บ                                                             ณ            บ
วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤด            บ
บ Output format $format                                       ณ &F1        บ
บ                                                             ณ            บ
บ Output file   $file                                         ณ            บ
บ                                                             ณ            บ
ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออฯออออออออออออผ
}
    FIELD MENU  xm_obj               name type   $FLDBOX
    FIELD nc 45 dir required         name ode    $FLDBOX
    FIELD MENU xm_format             name format $FLDBOX
    FIELD nc 45  dir   required      name file   $FLDBOX

    $BUTTONESC " Cancel "
    $BUTTONF10 " Ok     "
    $BUTTONF1  " Help   "
}

*/

MENU xm_rulefmt {
    OPTION "CSV"
    OPTION "DIF"
    OPTION "WKS"
    OPTION "TSP"
    OPTION "Reverse CSV"
}

PAGE xp_ruleexport {
    $PGHEAD
    END_FN {C_FN return(C_XodeRuleExport());}
    TITLE "XODE-Export file by rule"
    SCREEN {
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ               E X P O R T   F I L E   B Y   R U L E                    บ
ฬอออออออออออออออออออ I N P U T อออออออออออออออออออออออออออออัออออออออออออน
บ Variable file $vfile                                      ณ &F10       บ
บ                                                           ณ            บ
บ Comment file  $cfile                                      ณ            บ
บ                                                           ณ            บ
บ Rule file     $rule                                       ณ            บ
บ                                                           ณ &ESC       บ
วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ O U T P U T ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤด            บ
บ Sample  $from       $to         Format $fmt               ณ            บ
บ                                                           ณ            บ
บ File    $out                                              ณ            บ
บ                                                           ณ            บ
บ CSV only : NaN=$na        Separator=$sep                  ณ &F1        บ
บ                                                           ณ            บ
บ Debug file $trace                                         ณ            บ
บ                                                           ณ            บ
ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออฯออออออออออออผ
}
    FIELD string nc 63 ncv 43 dir required "" name vfile  $FLDBOX
    FIELD string nc 63 ncv 43 dir          "" name cfile  $FLDBOX
    FIELD string nc 63 ncv 43 dir          ""      name rule   $FLDBOX
    FIELD string nc 63 ncv 43 dir   required       name out    $FLDBOX
    FIELD string nc 63 ncv 43 dir                  name trace  $FLDBOX
    FIELD string nc 10              ""      name from   $FLDBOX
    FIELD string nc 10              ""      name to     $FLDBOX
    FIELD string nc 10 required     "#N/A"  name na     $FLDBOX
    FIELD string nc 10              ";"     name sep    $FLDBOX
    FIELD MENU xm_rulefmt                   name fmt    $FLDBOX

    $BUTTONESC " Cancel "
    $BUTTONF10 " Ok     "
    $BUTTONF1  " Help   "
}










