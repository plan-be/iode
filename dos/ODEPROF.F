
PAGE vkp_profload {
    Title "Load Profile"
    $PGHEAD
    END_FN {C_FN return(C_ProfileLoad());}
    SCREEN {
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ                    L O A D   P R O F I L E                   บ
ฬออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออน
บ Profile name: $name                                          บ
บ                                                              บ
วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤบ
บ This function replaces all the current windows contents      บ
บ by those defined in the named profile.                       บ
วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
บ    &F10              &ESC                 &F1                บ
บ                                                              บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
}

    FIELD name name nc 255 ncv 45 string DIR /*FNKEY _fk_f10 /* JMP 27-05-2015 */ $FLDBOX
    $BUTTONESC " Cancel "
    $BUTTONF10 "   Ok   "
    $BUTTONF1  "  Help  "
}

PAGE vkp_profsave {
    Title "Save Profile"
    $PGHEAD
    END_FN {C_FN return(C_ProfileSave());}
    SCREEN {
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ                         S A V E   P R O F I L E              บ
ฬออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออน
บ Profile name:  $name                                         บ
บ                                                              บ
วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
บ This function saves all the current windows contents         บ
บ in the named profile.                                        บ
วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
บ    &F10              &ESC                 &F1                บ
บ                                                              บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
}

    FIELD name name nc 255 ncv 43 string DIR /* FNKEY _fk_f10 /* JMP 27-05-2015 */ $FLDBOX
    $BUTTONESC " Cancel "
    $BUTTONF10 "   Ok   "
    $BUTTONF1  "  Help  "
}
