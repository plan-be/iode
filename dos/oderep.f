
PAGE vkp_edrep {
    Title "Edit Report"
    $PGHEAD
    END_FN {C_FN return(C_ReportEdit());}
    SCREEN {
ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ                      E D I T    A   R E P O R T                         บ
ฬอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออัอออออออออออน
บ                                                             ณ &F10      บ
บ                                                             ณ           บ
บ Report name : $name                                         ณ &ESC      บ
บ                                                             ณ           บ
บ                                                             ณ &F1       บ
บ                                                             ณ           บ
ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออฯอออออออออออผ
}
    FIELD name name nc 120 ncv 45 string DIR $FLDBOX REQUIRED

    $BUTTONESC " Cancel "
    $BUTTONF1  "  Help  "
    $BUTTONF10 "  Edit  "
}


PAGE vkp_prtrep {
    Title "Print Report"
    $PGHEAD
    FNKEY {
	F1             "Help"
	ESCAPE {ABORT} "Cancel"
	F6  {C_FN SB_FilePrintSetup();}        "Print Setup"
	F10 {C_FN return(C_ReportPrint()); ABORT}
    }

    SCREEN {
ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ               P R I N T    A   R E P O R T                  บ
ฬอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออน
บ                                                             บ
บ Report name : $name                                         บ
บ                                                             บ
วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
บ      &F10      &ESC       &F1         &F6                   บ
บ                                                             บ
ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
}
    FIELD name name nc 120 ncv 45 string DIR $FLDBOX REQUIRED

    $BUTTONESC " Cancel "
    $BUTTONF1  "  Help  "
    $BUTTONF10 " Print  "
    $BUTTONF6  "  Setup "
}

PAGE vkp_execrep {
    Title "Execute Report"
    $PGHEAD
    END_FN {C_FN return(C_ReportExec());}
    SCREEN {
ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ                    E X E C U T E   A   R E P O R T                      บ
ฬอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออัอออออออออออน
บ Report name : $name                                         ณ &F10      บ
บ                                                             ณ           บ
บ                                                             ณ &ESC      บ
วฤฤฤฤฤฤฤฤฤฤฤฤฤยฤยฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤด           บ
บ Report      ณ$parms                                         ณ &F1       บ
บ Parameters  ณ ณ                                             ณ           บ
วฤฤฤฤฤฤฤฤฤฤฤฤฤมฤมฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤมฤฤฤฤฤฤฤฤฤฤฤถ
บ Language $lang           NbDec $ndec                                    บ
บ                                                                         บ
ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
}

    FIELD name name  nc 120 ncv 45 string DIR           $FLDBOX REQUIRED
    FIELD name parms nc 40 nl 2 EDITOR SBAR     blue
    FIELD NAME ndec  nc 2  integer              $FLDBOX
    FIELD NAME lang  MENU vkm_lang              $FLDBOX

    $BUTTONESC " Cancel "
    $BUTTONF1  "  Help  "
    $BUTTONF10 "   OK   "
}

PAGE vkp_repline {
    Title "Execute a Report Line"
    $PGHEAD
    END_FN {C_FN return(C_ReportLine());}
    SCREEN {
ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ      E X E C U T E   A   S I N G L E   R E P O R T   C O M M A N D      บ
ฬอออออออออออออออออออออออออออออออออออออออออออออออออออออออัอออออออออออออออออน
บ                                                       ณ   &F10          บ
บ Command Line $cmd                                     ณ                 บ
บ                                                       ณ   &ESC          บ
บ                                                       ณ                 บ
บ                                                       ณ   &F1           บ
บ                                                       ณ                 บ
วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤมฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
บ Language $lang       NbDec $ndec                                        บ
บ                                                                         บ
ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
}

    FIELD name cmd nc 40 nl 3 string $FLDBOX
    FIELD NAME ndec  nc 2  integer   $FLDBOX
    FIELD NAME lang  MENU vkm_lang   $FLDBOX

    $BUTTONESC " Cancel "
    $BUTTONF1  "  Help  "
    $BUTTONF10 "   OK   "
}

PAGE vkp_prompt {
    $PGHEAD
    END_FN {C_FN return(C_ReportPrompt());}
    SCREEN {
ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออัอออออออออออออออออป
บ *prompt                                               ณ   &F10          บ
บ                                                       ณ                 บ
บ                                                       ณ   &ESC          บ
บ $define                                               ณ                 บ
บ                                                       ณ   &F1           บ
บ                                                       ณ                 บ
บ                                                       ณ                 บ
ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออฯอออออออออออออออออผ
}

    FIELD NAME prompt   nc 40 nl 2  string $FLDBOX center
    FIELD NAME define   nc 40 nl 3  string $FLDBOX
    FIELD NAME macro    nc 10  HIDDEN

    $BUTTONESC " Cancel "
    $BUTTONF1  "  Help  "
    $BUTTONF10 "   OK   "
}







