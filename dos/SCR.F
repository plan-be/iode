GLOBAL {
    FLD_MEM     NO
    DRW_MEM     NO
    DEF_OBJ     NO
}

FNKEY scr_f3f10 {
    F1     "Help"
    ESCAPE {ABORT} "Cancel"
    F3     {ABORT} "Cancel"
    F10    "OK"
}
/*
PAGE "scr_err" {
    center back reverse shadow1
    comment "Press any key to continue"
    SCREEN {
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ                             M E S S A G E                                บ
วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
บ$msg                                                                      บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
}
    FIELD string nc 74 output center name msg
}
*/
/*
PAGE scr_prt {
    CENTER BACK REVERSE SHADOW1
    FNKEY { ESCAPE "Quit"
	    F3     "Quit"
	    F10    "Print" }
    SCREEN {
ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ                  P R I N T    S C R E E N                     บ
ฬอออออออออออออออออออออออออออออออัอออออออออออออออออออออออออออออออน
บ Output File (or printer) name ณ $name                         บ
บ Printer (P) or file (F)       ณ $type                         บ
วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤมฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
บ                  SCREEN PART TO BE PRINTED                    บ
วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤยฤฤฤฤฤฤฤฤฤยฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤยฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
บ From line      ณ $line   ณ From column      ณ $col            บ
บ Number of linesณ $nl     ณ Number of columnsณ $nc             บ
ศออออออออออออออออฯอออออออออฯออออออออออออออออออฯอออออออออออออออออผ
}
    FIELD nc 30                     name name "tmp"
    FIELD nc 1 allowed "PF" upper   name type "F"
    FIELD      natural "0 "         name line
    FIELD      natural "25"         name nl
    FIELD      natural "0 "         name col
    FIELD      natural "80"         name nc
}
*/



