
PAGE vkp_edrep {
    Title "Edit Report"
    $PGHEAD
    END_FN {C_FN return(C_ReportEdit());}
    SCREEN {
�������������������������������������������������������������������������ͻ
�                      E D I T    A   R E P O R T                         �
�������������������������������������������������������������������������͹
�                                                             � &F10      �
�                                                             �           �
� Report name : $name                                         � &ESC      �
�                                                             �           �
�                                                             � &F1       �
�                                                             �           �
�������������������������������������������������������������������������ͼ
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
�������������������������������������������������������������ͻ
�               P R I N T    A   R E P O R T                  �
�������������������������������������������������������������͹
�                                                             �
� Report name : $name                                         �
�                                                             �
�������������������������������������������������������������Ķ
�      &F10      &ESC       &F1         &F6                   �
�                                                             �
�������������������������������������������������������������ͼ
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
�������������������������������������������������������������������������ͻ
�                    E X E C U T E   A   R E P O R T                      �
�������������������������������������������������������������������������͹
� Report name : $name                                         � &F10      �
�                                                             �           �
�                                                             � &ESC      �
�������������������������������������������������������������Ĵ           �
� Report      �$parms                                         � &F1       �
� Parameters  � �                                             �           �
�������������������������������������������������������������������������Ķ
� Language $lang           NbDec $ndec                                    �
�                                                                         �
�������������������������������������������������������������������������ͼ
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
�������������������������������������������������������������������������ͻ
�      E X E C U T E   A   S I N G L E   R E P O R T   C O M M A N D      �
�������������������������������������������������������������������������͹
�                                                       �   &F10          �
� Command Line $cmd                                     �                 �
�                                                       �   &ESC          �
�                                                       �                 �
�                                                       �   &F1           �
�                                                       �                 �
�������������������������������������������������������������������������Ķ
� Language $lang       NbDec $ndec                                        �
�                                                                         �
�������������������������������������������������������������������������ͼ
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
�������������������������������������������������������������������������ͻ
� *prompt                                               �   &F10          �
�                                                       �                 �
�                                                       �   &ESC          �
� $define                                               �                 �
�                                                       �   &F1           �
�                                                       �                 �
�                                                       �                 �
�������������������������������������������������������������������������ͼ
}

    FIELD NAME prompt   nc 40 nl 2  string $FLDBOX center
    FIELD NAME define   nc 40 nl 3  string $FLDBOX
    FIELD NAME macro    nc 10  HIDDEN

    $BUTTONESC " Cancel "
    $BUTTONF1  "  Help  "
    $BUTTONF10 "   OK   "
}







