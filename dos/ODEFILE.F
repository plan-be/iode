
TPAGES tp_pref
{
    TITLE "Print Preferences"

    PAGE p_a2mgdi  " Printer "
    PAGE p_a2mgnl  " A2m "
    PAGE p_a2mmif  " MIF "
    PAGE p_a2mrtf  " RTF "
    PAGE p_a2mhtml " HTML "
    PAGE p_a2mgif  " GIF "
}

MENU m_giffont {
    Auto Back Reverse
    OPTION "Tiny"
    OPTION "Small"
    OPTION "MediumBold"
    OPTION "Large"
    OPTION "Giant"
    OPTION "Grayed"
}


MENU m_pdest {
    AUTO BACK REVERSE
    OPTION  "Windows printer"
    OPTION  "File "
}

MENU m_colors {
    AUTO BACK REVERSE
    OPTION "Blue"
    OPTION "Yellow"
    OPTION "Black"
    OPTION "White"
    OPTION "Red"
    OPTION "Green"
    OPTION "Cyan"
    OPTION "Magenta"
}

MENU m_orient {
    Auto Back Reverse
    OPTION "Portrait"
    OPTION "Landscape"
}
MENU m_duplex {
    Auto Back Reverse
    OPTION "Simplex"
    OPTION "Duplex"
    OPTION "Vertical Duplex"
}

PAGE p_fprintsetup {
    // box 2
    Center BACK CYAN PROFILE
//    TITLE "Printer Setup"
    WIN_NLPNULL 6
    WIN_HLINE No
    END_FN {C_FN C_FilePrintSetup();}
    FNKEY {
	F10                                         "Change Printers Setup"
	F8      {C_FN SB_PrintPreferences();}       "Change Printout Preferences"
	ESCAPE  {ABORT}                             "Cancel"
	F1      {HELP ""}                           "Help"
	}

    SCREEN {
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³               P R I N T   S E T U P                   ³
ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
³ Print to : $dest                                      ³
³                                                       ³
ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
³    &F10          &ESC         &F8           &F1       ³
³                                                       ³
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
}
    Field MENU m_pdest              $FLDBOX name dest

    $BUTTONF10 "Apply    "
    $BUTTONF8  "Options  "
    $BUTTONF1  "Help     "
    $BUTTONESC "Cancel   "
}

MENU m_pformat {
    AUTO BACK REVERSE
    OPTION  "A2M file (IODE ...)"
    OPTION  "RTF file (Word, ...)"
    OPTION  "HTML file (Internet)"
    OPTION  "MIF file (Frame Maker)"
    OPTION  "CSV file (Excel, ....)"
}

PAGE vkp_fprintfile {
    box 2 Center BACK CYAN PROFILE
    TITLE "Print File Format"
    WIN_NLPNULL 6
    WIN_HLINE No
    END_FN  {C_FN C_FilePrintFile();}
    FNKEY {
	F10                         "Change Printers Setup"
	F8      {TP_edit tp_pref}   "Change Printout Preferences"
	ESCAPE  {ABORT}     "Cancel"
	F1      {HELP ""}   "Help"
	}

    SCREEN {
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³ Format    $format                        &F8            ³
³                                                         ³
³ File      $file                                         ³
³                                                         ³
ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
³           &F10          &ESC             &F1            ³
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
}
    Field MENU m_pformat            $FLDBOX name format
    Field string nc 255 ncv 45 DIR   $FLDBOX name file

    $BUTTONF10 "Apply    "
    $BUTTONF8  "Options  "
    $BUTTONF1  "Help     "
    $BUTTONESC "Cancel   "
}

/*

TPAGES tp_opts
{
    TITLE "A2M Options"

    PAGE p_a2mgnl  "General"
    PAGE p_a2mgdi  "Windows printer"
    PAGE p_a2mgdi2 "ASCII file"
    PAGE p_a2mmif  "MIF"
    PAGE p_a2mrtf  "RTF"
    PAGE p_a2mhtml "HTML"
}
*/

PAGE p_a2mgnl
{
    CENTER BACK REVERSE BOX 2 PROFILE NOSHADOW NOCLOSEBOX DEF_OBJ
    TITLE "General parameters"

    WIN_SKIP 0 WIN_NLPNULL 8
/*
    SCREEN {
ÉÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ»
º                   A 2 M   G E N E R A T I O N                     º
º ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿          º
º ³ Escape character  $esc  Define character $def        ³          º
º ³ Command character $cmd  Cell separator   $sep        ³          º
º ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ          º
º ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿          º
º ³ Default paragraph $tag                               ³          º
º ³ $bl Preserve spaces                                  ³          º
º ³ $lf Preserve linefeed                                ³          º
º ³ $ap Append to existing file                          ³          º
º ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ          º
º ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿  º
º ³ Width (mm)  $gw    Background color $col     Box $box        ³  º
º ³ Height (mm) $gh    Background brush $pct                     ³  º
º ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ  º
ÈÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ¼
}
*/
    SCREEN {
 ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
 ³ Escape character  $esc  Define character $def        ³
 ³ Command character $cmd  Cell separator   $sep        ³
 ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
 ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
 ³ Default paragraph $tag                               ³
 ³ $bl Preserve spaces                                  ³
 ³ $lf Preserve linefeed                                ³
 ³ $ap Append to existing file                          ³
 ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
 ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
 ³ Width (mm)  $gw    Background color $col           Box $box        ³
 ³ Height (mm) $gh    Background brush $pct                           ³
 ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
}

    Group {
	Title "Control characters"
	ALT_C
	FIELDS {
	    FIELD NAME esc string "\\" blue
	    FIELD NAME cmd string "."
	    FIELD NAME def string "&"
	    FIELD NAME sep string "@"
	    }
	}

    Group {
	ALT_S
	TITLE "Startup options"
	FIELDS {
	    FIELD NAME tag string "par_1          "
	    FIELD NAME bl check_box
	    FIELD NAME lf check_box
	    FIELD NAME ap check_box
	    }
	}

    Group {
	ALT_G
	TITLE "Graphs"
	FIELDS {
	    FIELD NAME GW natural "160"
	    FIELD NAME GH natural "100"
	    FIELD NAME col MENU m_colors
	    FIELD NAME pct natural "   "
	    FIELD NAME box natural " "
	    }
	}

}

MENU m_a2mfont {
    Auto Back Reverse
    OPTION "Helvetica"
    OPTION "Times"
    OPTION "Courier"
    OPTION "Bookman"
    OPTION "Palatino"
    OPTION "Trebuchet MS"
}

PAGE p_a2mrtf {
    WIN_NLPNULL 8
    WIN_SKIP 0
    WIN_HLINE No WIN_VLINE No
    CENTER BACK REVERSE PROFILE  NOSHADOW NOCLOSEBOX DEF_OBJ
    Title "RTF options"
/*
    SCREEN {
ÉÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ»
º               R T F   G E N E R A T I O N                         º
º ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿       º
º ³ $nu Paragraph numbering     Font Incr       $incr       ³       º
º ³ Font Family     $fontfam    Font Size       $fontsz     ³       º
º ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ       º
º ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿       º
º ³ $tc Use color            Table Font size     $tfontsz   ³       º
º ³ $ts Split tables         Table Font family   $tfontfam  ³       º
º ³ $tb Outside Borders      Table Width (mm)    $tw        ³       º
º ³ $tv Vertical Lines       First Column Width  $tcol1     ³       º
º ³ $th Horizontal Lines     Other Columns Width $tcoln     ³       º
º ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ       º
º ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿  º
º ³ $hcw Prepare for HCW  $cmp Compress Help $toc Generate TOC   ³  º
º ³ Help Title $Title                                            ³  º
º ³ Copyright  $copyr                                            ³  º
º ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ  º
ÈÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ¼
}
*/
    SCREEN {
 ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿   ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
 ³$nu Paragraph numbers³   ³ $tc Use color          Tbl Width (mm)  $tw      ³
 ³Font Family $fontfam ³   ³ $ts Split tables       1st Col Width   $tcol1   ³
 ³Font Incr   $incr    ³   ³ $tb Outside Borders    nth Cols Width  $tcoln   ³
 ³Font Size   $fontsz  ³   ³ $tv Vertical Lines     Tbl Font Family $tfontfam³
 ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ   ³ $th Horizontal Lines   Tbl Font Size   $tfontsz ³
			   ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ

 ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
 ³Help Title $Title                                            ³
 ³Copyright  $copyr                                            ³
 ³$hcw Prepare for HCW  $cmp Compress Help $toc Generate TOC   ³
 ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
}
    GROUP g1 {
	TITLE "General"
	ALT_G
	FIELDS {
	    FIELD checkbox NAME nu blue
	    FIELD natural NAME fontsz  nc 2 "10"
	    FIELD natural NAME incr    nc 2 "2 "
	    FIELD menu m_a2mfont NAME fontfam
	    }
	}

    Group g2 {
	TITLE "Table options"
	ALT_T
	FIELDS {
	    FIELD natural NAME tw      nc 4 "165 "
	    FIELD natural NAME tcol1   nc 4 "60  "
	    FIELD natural NAME tcoln   nc 4 "15  "
	    FIELD natural NAME tfontsz nc 3 "8  "
	    FIELD menu m_a2mfont NAME tfontfam

	    FIELD check_box NAME tc
	    FIELD check_box NAME ts
	    FIELD check_box NAME tb
	    FIELD check_box NAME tv
	    FIELD check_box NAME th
	    }
	}
    Group g3 {
	TITLE "Help generation"
	ALT_T
	FIELDS {
	    FIELD checkbox NAME hcw
	    FIELD checkbox NAME cmp
	    FIELD checkbox NAME toc
	    FIELD string nc 38 NAME title
	    FIELD string nc 38 NAME copyr
	    }
	}
}

PAGE p_a2mhtml {
    WIN_NLPNULL 8
    WIN_SKIP 0
    WIN_HLINE No WIN_VLINE No
    CENTER BACK REVERSE PROFILE  NOSHADOW NOCLOSEBOX DEF_OBJ
    Title "HTML options"
/*
    SCREEN {
ÉÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ»
º               H T M L   G E N E R A T I O N                 º
ÇÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¶
ºÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿     ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿     º
º³ Font Size       $fontsz    ³     ³ $tc Use color     ³     º
º³ Font Incr       $incr      ³     ³ Border width $tb  ³     º
º³ Font Family     $fontfam   ³     ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ     º
º³ Table font size $tfontsz   ³                               º
ºÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ                               º
ºÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿º
º³ $nu Paragraph numbering                                   ³º
º³ $toc Generate Table of Contents                           ³º
º³ Body Tag $body                                            ³º
º³                                                           ³º
º³ Title    $title                                           ³º
ºÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙº
ÈÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ¼
}
*/
    SCREEN {
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿   ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³ Font Size       $fontsz    ³   ³ $tc Use color            ³
³ Font Incr       $incr      ³   ³ Border width $tb         ³
³ Font Family     $fontfam   ³   ³ Font Size    $tfontsz    ³
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ   ³ Font Family  $tfontfam   ³
				 ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ

ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³ $nu Paragraph numbering                                   ³
³ $toc Generate Table of Contents                           ³
³ Body Tag $body                                            ³
³                                                           ³
³ Title    $title                                           ³
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
}
    GROUP g1 {
	TITLE "Font"
	ALT_F
	FIELDS {
	    FIELD natural NAME fontsz  nc 2 "10" blue
	    FIELD natural NAME incr    nc 2 "2 "
	    FIELD menu m_a2mfont NAME fontfam
	    }
	}

    Group g2 {
	TITLE "Table options"
	ALT_T
	FIELDS {
	    FIELD check_box NAME tc
	    FIELD natural nc 2 NAME tb "0 "
	    FIELD natural NAME tfontsz nc 3 "8  "
	    FIELD menu m_a2mfont NAME tfontfam
	    }
	}
    Group g3 {
	TITLE "HTML generation"
	ALT_H
	FIELDS {
	    FIELD checkbox NAME nu
	    FIELD checkbox NAME toc
	    FIELD string nc 48 NAME title
	    FIELD EDITOR nc 48 nl 2 NAME body
	    }
	}
}

PAGE p_a2mmif {
    WIN_NLPNULL 8
    WIN_SKIP 0
    WIN_HLINE No WIN_VLINE No
    CENTER BACK REVERSE PROFILE  NOSHADOW NOCLOSEBOX DEF_OBJ
    Title "MIF (Frame) Options"
/*
    SCREEN {
ÉÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ»
º                    M I F   G E N E R A T I O N                      º
ÇÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¶
ºÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿ º
º³ Font Size   $fontsz        Font Increment  $incr                 ³ º
º³ Font family $fontfam       Table Font Size $tfontsz              ³ º
ºÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ º
ºÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿              º
º³                                 $tc Use color       ³              º
º³ Table Width (mm)    $tw         $ts Split tables    ³              º
º³ First Column Width  $tcol1      $tb Outside Borders ³              º
º³ Other Columns Width $tcoln      $tv Vertical Lines  ³              º
º³                                 $th Horizontal Lines³              º
ºÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ              º
ºÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿             º
º³ $img Image refs in text                              ³             º
ºÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ             º
ÈÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ¼
}
*/
    SCREEN {
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³ Font Size       $fontsz     ³
³ Font family     $fontfam    ³
³ Font Increment  $incr       ³
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³ $tc Use color        Table Width (mm)    $tw        ³
³ $ts Split tables     First Column Width  $tcol1     ³
³ $tb Outside Borders  Other Columns Width $tcoln     ³
³ $tv Vertical Lines   Table Font Size     $tfontsz   ³
³ $th Horizontal Lines Table Font Family   $tfontfam  ³
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³ $img Image refs in text                             ³
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
}
    GROUP g1 {
	TITLE "Font size"
	ALT_F
	FIELDS {
	    FIELD natural NAME fontsz  nc 2 "10" blue
	    FIELD natural NAME incr    nc 2 "2 "
	    FIELD menu m_a2mfont NAME fontfam
	    }
	}

    Group g2 {
	TITLE "Table options"
	ALT_T
	FIELDS {
	    FIELD natural NAME tw      nc 4 "165 "
	    FIELD natural NAME tcol1   nc 4 "60  "
	    FIELD natural NAME tcoln   nc 4 "15  "
	    FIELD natural NAME tfontsz nc 3 "8  "
	    FIELD menu m_a2mfont NAME tfontfam

	    FIELD check_box NAME tc
	    FIELD check_box NAME ts
	    FIELD check_box NAME tb
	    FIELD check_box NAME tv
	    FIELD check_box NAME th
	    }
	}
    Group g3 {
	TITLE "Miscellaneous"
	ALT_M
	FIELDS {
	    FIELD checkbox NAME img
	    }
	}
}

PAGE p_a2mgdi {
    Title "A2M Printer Options"
    Center back REVERSE PROFILE  NOSHADOW NOCLOSEBOX DEF_OBJ
    WIN_NLPNULL 8
    WIN_SKIP 0 WIN_HLINE No WIN_VLINE No
/*
    SCREEN {
ÉÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ»
º A2M PRINTING OPTIONS                                         º
ÇÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¶
º ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿ ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿ º
º ³$co Black/White            ³ ³$tc  Table Color            ³ º
º ³$nu Para Numbering         ³ ³$ts  Table shading          ³ º
º ³Font Size   $fontsz        ³ ³$tbr Allow break in tables  ³ º
º ³Font Incr   $incr          ³ ³$tp  New page for each table³ º
º ³Font family $fontfam       ³ ³$gp  New page for each graph³ º
º ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ ³Table Fontsize     $tfontsz ³ º
º                               ³Table Border width $tb      ³ º
º                               ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ º
º ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿     º
º ³Left Margin   $lmarg  Top Margin    $tmarg            ³     º
º ³Right Margin  $rmarg  Bottom Margin $bmarg            ³     º
º ³Page header   $PgHead                                 ³     º
º ³Page footer   $PgFoot                                 ³     º
º ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ     º
ÈÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ¼
}
*/

/*
    SCREEN {
 ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿  ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
 ³$co Black/White      ³  ³$tc  Table Color       Tbl Font Size    $tfontsz  ³
 ³$nu Para Numbering   ³  ³$ts  Table shading     Tbl Font Family  $tfontfam ³
 ³Font Size   $fontsz  ³  ³$tbr Break in tables   Tbl Border width $tb       ³
 ³Font Incr   $incr    ³  ³$tp  New page/Table                               ³
 ³Font family $fontfam ³  ³$gp  New page/Graph                               ³
 ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ  ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
 ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
 ³Left Margin    $lmarg      Top Margin    $tmarg          ³
 ³Right Margin   $rmarg      Bottom Margin $bmarg          ³
 ³$PgPrompt Prompt printer   Orientation   $orient         ³
 ³                           Recto-verso   $duplex         ³
 ³Page header    $PgHead                                   ³
 ³Page footer    $PgFoot                                   ³
 ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
}


    GROUP g1 {
	TITLE "General"  ALT_G
	FIELDS {
	    FIELD natural NAME fontsz  "10 " blue
	    FIELD natural NAME incr    "2  "
	 //   FIELD NAME color MENU {OPTION "Yes" OPTION "No"}
	    FIELD NAME co checkbox
	    FIELD NAME nu checkbox
	    FIELD menu m_a2mfont NAME fontfam
	    }
	}

    GROUP g2 {
	TITLE "Tables and Graphics" ALT_T
	FIELDS {
	    FIELD check_box NAME tc
	    FIELD natural   NAME tb       "0  "
	    FIELD natural   NAME tfontsz  "8  "
	    FIELD menu m_a2mfont NAME tfontfam
	    FIELD checkbox  NAME tbr
	    FIELD checkbox  NAME ts
	    FIELD checkbox  NAME tp
	    FIELD checkbox  NAME gp
	    }
	}

    GROUP g3 {
	TITLE "Page Setup" ALT_S
	FIELDS {
	    FIELD checkbox  NAME pgprompt
	    FIELD string nc 38 NAME pghead
	    FIELD string nc 38 NAME pgfoot
	    FIELD natural NAME lmarg   "10 "
	    FIELD natural NAME rmarg   "10 "
	    FIELD natural NAME tmarg   "10 "
	    FIELD natural NAME bmarg   "10 "
	    FIELD menu m_orient NAME orient
	    FIELD menu m_duplex NAME duplex
	    }
	}

*/

    SCREEN {
 ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿ ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
 ³$co Black/White           ³ ³$tc  Table Color       Tbl Font Size    $tfontsz    ³
 ³$nu Para Numbering        ³ ³$ts  Table shading     Tbl Font Family  $tfontfam   ³
 ³Font Size   $fontsz       ³ ³$tbr Break in tables   Tbl Border width $tb         ³
 ³Font Incr   $incr         ³ ³$tp  New page/Table                                 ³
 ³Font family $fontfam      ³ ³$gp  New page/Graph                                 ³
 ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
 ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
 ³Left Margin    $lmarg      Top Margin    $tmarg                            ³
 ³Right Margin   $rmarg      Bottom Margin $bmarg                            ³
 ³Prompt printer $PgPrompt                                                   ³
 ³Page header    $PgHead                                                     ³
 ³Page footer    $PgFoot                                                     ³
 ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
}


    GROUP g1 {
	TITLE "General"  ALT_G
	FIELDS {
	    FIELD natural NAME fontsz  "10 " blue
	    FIELD natural NAME incr    "2  "
	 /*   FIELD NAME color MENU {OPTION "Yes" OPTION "No"} */
	    FIELD NAME co checkbox
	    FIELD NAME nu checkbox
	    FIELD menu m_a2mfont NAME fontfam
	    }
	}

    GROUP g2 {
	TITLE "Tables and Graphics" ALT_T
	FIELDS {
	    FIELD check_box NAME tc
	    FIELD natural   NAME tb       "0  "
	    FIELD natural   NAME tfontsz  "8  "
	    FIELD menu m_a2mfont NAME tfontfam
	    FIELD checkbox  NAME tbr
	    FIELD checkbox  NAME ts
	    FIELD checkbox  NAME tp
	    FIELD checkbox  NAME gp
	    }
	}

    GROUP g3 {
	TITLE "Page Setup" ALT_S
	FIELDS {
	    FIELD natural NAME lmarg   "10 "
	    FIELD natural NAME rmarg   "10 "
	    FIELD natural NAME tmarg   "10 "
	    FIELD natural NAME bmarg   "10 "
	    FIELD checkbox  NAME pgprompt
	    FIELD string nc 38 NAME pghead
	    FIELD string nc 38 NAME pgfoot
	    }
	}


}

PAGE p_a2mgdi2 {
    Title "Ascii Printer Options (W/O translation)"
    Center back REVERSE PROFILE  NOSHADOW NOCLOSEBOX DEF_OBJ
    WIN_NLPNULL 8
    WIN_SKIP 0 WIN_HLINE No WIN_VLINE No
/*
    SCREEN {
ÉÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ»
º ASCII PRINTING OPTIONS                                       º
ÇÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¶
º ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿                                      º
º ³Font          $font  ³                                      º
º ³Font Size     $fontsz³                                      º
º ³Line spacingr $lnsp  ³                                      º
º ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ                                      º
º ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿     º
º ³Left Margin   $lmarg  Top Margin    $tmarg            ³     º
º ³Right Margin  $rmarg  Bottom Margin $bmarg            ³     º
º ³Page header   $PgHead                                 ³     º
º ³Page footer   $PgFoot                                 ³     º
º ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ     º
ÈÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ¼
}
*/
    SCREEN {
 ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
 ³Font          $font  ³
 ³Font Size     $fontsz³
 ³Line spacingr $lnsp  ³
 ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
 ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
 ³Left Margin   $lmarg  Top Margin    $tmarg            ³
 ³Right Margin  $rmarg  Bottom Margin $bmarg            ³
 ³Page header   $PgHead                                 ³
 ³Page footer   $PgFoot                                 ³
 ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
}
    GROUP g1 {
	TITLE "General"  ALT_G
	FIELDS {
	    FIELD string  NAME font    "Courier       " blue
	    FIELD natural NAME fontsz  "10 "
	    FIELD natural NAME lnsp    "1"
	    }
	}

    GROUP g3 {
	TITLE "Page Setup" ALT_S
	FIELDS {
	    FIELD string nc 38 NAME pghead "- Page %d -"
	    FIELD string nc 38 NAME pgfoot "IODE"
	    FIELD natural NAME lmarg   "10 "
	    FIELD natural NAME rmarg   "10 "
	    FIELD natural NAME tmarg   "10 "
	    FIELD natural NAME bmarg   "10 "
	    }
	}
}

PAGE p_a2mgif
{
    WIN_NLPNULL 8
    WIN_SKIP 0
    WIN_HLINE No WIN_VLINE No
    CENTER BACK REVERSE PROFILE  NOSHADOW NOCLOSEBOX DEF_OBJ
    Title "GIF Options"
    SCREEN {
 ÚÄColorsÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
 ³Background   $col            ³
 ³Transparent  $trs            ³
 ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
 ÚÄGIF encodingÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
 ³Transparent GIF  $trans                       ³
 ³Interlaced GIF   $inter                       ³
 ³GIF Font         $giffont                     ³
 ³Do NOT fill bars $filled                      ³
 ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
}

    Group
    {
	Title "Background color"
	ALT_C
	FIELDS
	{
	    FIELD natural NAME col MENU m_colors
	    FIELD natural NAME trs MENU m_colors
	}
    }
    FIELD check_box NAME trans
    FIELD check_box NAME inter line 13
    FIELD menu m_giffont NAME giffont "Small"
    FIELD check_box NAME filled
}


MENU m_a2mdest {
    AUTO BACK REVERSE
    OPTION  "Windows printer"
    OPTION  "Windows printer without translation"
    OPTION  "RTF file (Word, ...)"
    OPTION  "HTML file (Internet)"
    OPTION  "MIF file (Frame Maker)"
    OPTION  "Postscript file"
    OPTION  "Formatted Ascii file"
}










