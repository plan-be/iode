GLOBAL {
    OPT_MEM     YES
    FLD_MEM     Yes
    DRW_MEM     Yes
    DEF_OBJ     NO
}

#include "common.f"

APPL vkiap_appl  {
    line 0 col 0 nl 30 nc 100
/*    FILL 'Û'
    BACK black
*/
    FILL ' '
    BACK black  // JMP 6/9/2015

    BEGIN_FN {
	HELPFILE $HELPFILE
	C_FN {
		{
		    extern int first_page;
		    if(first_page == 0) {
			SCR_record_key(SCR_A_Z);
			//SCR_fill(' ', SCR_PAGE_SIZE[0] - 1, 0, 1, SCR_PAGE_SIZE[1]);
			ODE_adapt_size(0, 0);
			}

		}
	     }
	}
    A_BAR vkiab_ab
    FNKEY {
/*      F9      {PG_edit vkp_status} /* JMP_M 4.21 01-11-95 */
	F9      {C_FN SB_status();}  /* JMP_M 4.21 01-11-95 */
/*      S_F10   {C_FN SB_FileEdit();} */
/*        S_F10   {C_FN ODE_PlaybackMsg();} */
	CTRL_A  {C_FN SCR_config_screen();}
	ALT_S   {C_FN SB_FilePrintSetup();}
	F1      {Record {HELP}}
//        s_F1    {Record {HELP}}
//      F2      {C_FN ODE_adapt_size(1, 1);}
//      c_F2    {C_FN ODE_adapt_size(-1, -1);}
	/* F2      {C_FN TestHelp();} */
	}

      DRAW
//        FILL RED        0 0 1  200   /* JMP 03-12-10 */
	FILL REVERSE    1 0 1  200   /* JMP 03-12-10 */
//        FILL reverse    29 0 1 200   /* JMP 03-12-10 */
//      "                                                    ณ        ณ       ณ "  REVERSE 29 0
//
//        $IODE_VERSION RED 0 8 /* JMP 08-01-11 */

}

/* MENU */

MENU vkm_ny {
    auto back reverse
    OPTION "No"
    OPTION "Yes"
}

MENU vkm_obj {
    AUTO
    OPTION  "Comments"
    OPTION  "Equations"
    OPTION  "Identities"
    OPTION  "Lists"
    OPTION  "Scalars"
    OPTION  "Tables"
    OPTION  "Variables"
}

MENU vkm_method {
    auto back reverse PROFILE
    OPTION "LSQ"
    OPTION "Zellner"
    OPTION "Instrumental"
    OPTION "GLS (3SLS)"
    OPTION "Max.Likelihood"
}

MENU vkm_mode {
    auto back reverse PROFILE
    OPTION "Level"
    OPTION "Differences"
    OPTION "Growth rates"
    OPTION "YoY Diffs"
    OPTION "YoY Grt"
}

MENU vkm_axis  {
    auto back reverse PROFILE
    OPTION "Std Axis"
    OPTION "Log Axis"        OFF
    OPTION "Semi-log Axis"   OFF
    OPTION "Percent Axis"    OFF
}

MENU vkm_align {
    auto back reverse PROFILE
    OPTION "Left"
    OPTION "Middle"
    OPTION "Right"
}

MENU vkm_grids {
    auto back reverse PROFILE
    OPTION "Major thicks"
    OPTION "No grids"
    OPTION "Minor thicks"
}

MENU vkm_chart {
    auto back reverse PROFILE
    OPTION "Line chart"
    OPTION "Scatter chart"
    OPTION "Bar chart"
    OPTION "Scatter Line chart"
}

MENU vkm_laxis  {
    auto back reverse PROFILE
    OPTION "Left "
    OPTION "Right"
}

MENU vkvm_orient {
    auto back reverse PROFILE
    OPTION "Line"
    OPTION "Column"
}

MENU vkm_results {
    line 3 col 55 back reverse PROFILE
    OPTION "      RESULTS       " OFF
    OPTION "ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ" OFF
    OPTION "Coefficients"
    OPTION "Correlation matrix"
    OPTION "Tests by equation"
    OPTION "Graph of YOBS-YEST"
    OPTION "Graph of Residuals"
    OPTION "ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ" OFF
    OPTION "Print Graphs.."
    OPTION "Print Output ..."
}
MENU vkm_lang {
    OPTION "English"
    OPTION "French"
    OPTION "Dutch"
}

MENU vkm_extrap {
    TITLE "STARTING VALUES"
    OPTION "Y := Y[-1], if Y null or NA"
    OPTION "Y := Y[-1], always"
    OPTION "Y := extrapolation, if Y null or NA"
    OPTION "Y := extrapolation, always"
    OPTION "Y unchanged"
    OPTION "Y := Y[-1], if Y = NA"
    OPTION "Y := extrapolation, if Y = NA"
}

MENU vkm_what {
    OPTION "Data + Graph"
    OPTION "Data only"
    OPTION "Graph only"
}

MENU vkm_orient {
    OPTION "Portrait"
    OPTION "Landscape"
}

MENU vkm_asis {
    OPTION "As Is (condensed)"
    OPTION "Splitted if needed"
}

MENU vkm_fonts {
    Auto Back Reverse NL 8 OPT_MEM
    TITLE "POSTSCRIPT FONTS"
    OPTION "Courier"
    OPTION "Courier-Bold"
    OPTION "Courier-BoldOblique"
    OPTION "Courier-Oblique"
    OPTION "Helvetica"
    OPTION "Helvetica-Bold"
    OPTION "Helvetica-BoldOblique"
    OPTION "Helvetica-Narrow"
    OPTION "Helvetica-Narrow-Bold"
    OPTION "Helvetica-Narrow-BoldOblique"
    OPTION "Helvetica-Oblique"
    OPTION "Times"
    OPTION "Times-Bold"
    OPTION "Times-BoldItalic"
    OPTION "Times-Italic"
    OPTION "Times-Roman"
    OPTION "AvantGarde-Book"
    OPTION "AvantGarde-BookOblique"
    OPTION "AvantGarde-Demi"
    OPTION "AvantGarde-DemiOblique"
    OPTION "Bookman-Demi"
    OPTION "Bookman-DemiItalic"
    OPTION "Bookman-Light"
    OPTION "Bookman-LightItalic"
    OPTION "Trebuchet MS"
    }

A_BAR vkiab_ab {
    line 1 col 2

    OPTION "File" ALT_F COMMENT "File management"
	MENU {
	    PROFILE
/*
	    OPTION "Open"             {C_FN SB_WsLoad  ();}       COMMENT "Load new work space(s) from disk"
	    OPTION "Save"             {C_FN SB_WsSave  ();}       COMMENT "Save current work space(s) contents on disk"
	    OPTION "ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ" OFF
*/
	    OPTION "Import"                     {C_FN SB_XodeRuleImport();} COMMENT "Import files respecting certain rules to IODE"
	    OPTION "Export"                     {C_FN SB_XodeRuleExport();} COMMENT "Export files respecting certain rules to IODE"
	    OPTION "ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ" OFF
	    OPTION "Print Setup "    ALT_S      {C_FN SB_FilePrintSetup();} COMMENT "Change Print Setup"
/*            OPTION "Preferences"                {TP_EDIT TP_pref} COMMENT "Change Print options"*/
	    OPTION "ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ" OFF
/*
	    OPTION "FILE MANAGEMENT" OFF
	    OPTION "ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ" OFF
	    OPTION "List files"                 {C_FN SB_FileList();}   COMMENT "Display directory and sub-directories contents"
	    OPTION "Delete files"               {C_FN SB_FileDelete();} COMMENT "Delete files from disk"
	    OPTION "Copy files"                 {C_FN SB_FileCopy();}   COMMENT "Duplicate files on disk"
	    OPTION "Rename files"               {C_FN SB_FileRename();} COMMENT "Rename files on disk"
	    OPTION "Edit files"                 {C_FN SB_FileEdit();}   COMMENT "Edit files with MMT"
	    OPTION "อออออออออออออออ" OFF
*/
	    OPTION "P R O F I L E S" OFF
	    OPTION "ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ" OFF
	    OPTION "Load profile"               {C_FN SB_ProfileLoad();}  COMMENT "Reset profile and load a profile from disk to replace the current one"
	    OPTION "Save current"               {C_FN SB_ProfileSave();}  COMMENT "Save permanently the current profile on disk"
	    OPTION "Reset profile"              {C_FN SB_ProfileReset();} COMMENT "Clear current profile - Reset default values"
	    OPTION "ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ" OFF
	    OPTION "eXit      Alt-X"   ALT_X    {C_FN ODE_quit();}           COMMENT "Leave I.O.D.E application"
	    }

    OPTION "Workspace" ALT_W COMMENT "Work space Management"
	MENU {
	    PROFILE
	    OPTION "    WS MANAGEMENT    " OFF
	    OPTION "ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ" OFF
/*            OPTION "Display WS Status"         {C_FN SB_WsStatus();}    COMMENT "Display current status of the work space(s)" */
	    OPTION "Load Work Space"             {C_FN SB_WsLoad  ();}    COMMENT "Load new work space(s) from disk"
	    OPTION "Save Work Space"             {C_FN SB_WsSave  ();}    COMMENT "Save current work space(s) contents on disk"
	    OPTION "Clear Work Space"            {C_FN SB_WsClear ();}    COMMENT "Clear current work space(s)"
	    OPTION "ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ" OFF
	    OPTION "Copy into Work Space"        {C_FN SB_WsCopy  ();} COMMENT "Copy object list from disk into the work space"
	    OPTION "Merge into Work Space"       {C_FN SB_WsMerge ();} COMMENT "Merge file from disk into the work space"
	    OPTION "Describe Work Space"         {C_FN SB_WsDescr ();} COMMENT "Change work spaces descriptions"
	    OPTION "Set Variables Sample"        {C_FN SB_WsSample();} COMMENT "Change current variables definition sample"
	    OPTION "Extrapolate Variables"       {C_FN SB_WsExtrapolate();} COMMENT "Extrapolate variables from WS"
	    OPTION "ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ" OFF
	    OPTION "High to Low "                {C_FN SB_WsHtoL();}   COMMENT "Copy file with higher frequency into the work space"
	    OPTION "Low to High "                {C_FN SB_WsLtoH();}   COMMENT "Copy file with lower frequency into the work space"
	    OPTION "Seasonal Adjustment"         {C_FN SB_WsSeasonAdj();} COMMENT "Census II-method for Seasonal Adjustment"
	    OPTION "Trend correction"            {C_FN SB_WsTrend();}  COMMENT "Hodrick-Prescott filter for trend correction"
	   /* OPTION "Time series analysis"        {C_FN SB_WsTimeSeries();} COMMENT "Statistical Analysis of Time Series" */
	    }

    OPTION "Data" ALT_D COMMENT "Data management - Create, Modify, Delete, ..."
	MENU {
	    PROFILE
	    FNKEY {F10 {Record {ENTER HOME ENTER}}}
	    OPTION "    DATA EDIT    " OFF
	    OPTION "ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ" OFF
	    OPTION "Comments"           {
		MN_EDIT {
		    PROFILE AUTO BACK REVERSE
		    OPTION " COMMENTS    " OFF
		    OPTION "ฤฤฤฤฤฤฤฤฤฤฤฤฤ" OFF
		    OPTION "Edit WS      "   {C_FN SB_DataEditScroll(0L, K_CMT);}
		    OPTION "Edit List    "   {C_FN SB_DataEdit(0L, K_CMT);}
		}
	    } COMMENT "Update, create, delete and browse current work space comments"

	    OPTION "Equations"          {
		MN_EDIT  {
		    PROFILE AUTO BACK REVERSE
		    OPTION " EQUATIONS   " OFF
		    OPTION "ฤฤฤฤฤฤฤฤฤฤฤฤฤ" OFF
		    OPTION "Edit WS      "   {C_FN SB_DataEditScroll(0L, K_EQS);}
		    OPTION "Edit List    "   {C_FN SB_DataEdit(0L, K_EQS);}
		}
	    } COMMENT "Update, create, delete, estimate and browse current work space equations"

	    OPTION "Identities"         {
		MN_EDIT  {
		    PROFILE AUTO BACK REVERSE
		    OPTION " IDENTITIES  " OFF
		    OPTION "ฤฤฤฤฤฤฤฤฤฤฤฤฤ" OFF
		    OPTION "Edit WS      "   {C_FN SB_DataEditScroll(0L, K_IDT);}
		    OPTION "Edit List    "   {C_FN SB_DataEdit(0L, K_IDT);}
		}
	    } COMMENT "Update, create, delete and browse current work space identities"

	    OPTION "Lists"              {
		MN_EDIT  {
		    PROFILE AUTO BACK REVERSE
		    OPTION "    LISTS    " OFF
		    OPTION "ฤฤฤฤฤฤฤฤฤฤฤฤฤ" OFF
		    OPTION "Edit WS      "   {C_FN SB_DataEditScroll(0L, K_LST);} COMMENT "Update, create, delete and browse current work space lists"
		    OPTION "Edit List    "   {C_FN SB_DataEdit(0L, K_LST);}       COMMENT "Update, create, delete and browse current work space lists"
		    OPTION "ฤฤฤฤฤฤฤฤฤฤฤฤฤ" OFF
		    OPTION "Sort & Expand"   {C_FN SB_DataListSort();}            COMMENT "Expand and sort lists"
		    OPTION "List Calculus"   {C_FN SB_DataCalcLst();}             COMMENT "Execute various logical operations on lists"
		    OPTION "ฤฤฤฤฤฤฤฤฤฤฤฤฤ" OFF
		    OPTION "File Contents"   {C_FN SB_DataList();}                COMMENT "Gives the contents of a file"
		    OPTION "File Compare "   {C_FN SB_DataCompare();}             COMMENT "Compares the contents of the WS and that of a file"
		    OPTION "Search Text  "   {C_FN SB_DataSearch();}              COMMENT "Search for a text in object definitions"
		    OPTION "Scan Objects "   {C_FN SB_DataScan();}                COMMENT "Scan for variables and scalars in object definitions"
		}
	    } COMMENT "Update, create, delete and browse current work space lists"

	    OPTION "Scalars"            {
		MN_EDIT  {
		    PROFILE AUTO BACK REVERSE
		    OPTION "   SCALARS   " OFF
		    OPTION "ฤฤฤฤฤฤฤฤฤฤฤฤฤ" OFF
		    OPTION "Edit WS      "   {C_FN SB_DataEditScroll(0L, K_SCL);}
		    OPTION "Edit List    "   {C_FN SB_DataEdit(0L, K_SCL);}
		}
	    } COMMENT "Update, create, delete and browse current work space scalars"

	    OPTION "Tables"             {
		MN_EDIT  {
		    PROFILE AUTO BACK REVERSE
		    OPTION "   TABLES    " OFF
		    OPTION "ฤฤฤฤฤฤฤฤฤฤฤฤฤ" OFF
		    OPTION "Edit WS      "   {C_FN SB_DataEditScroll(0L, K_TBL);}
		    OPTION "Edit List    "   {C_FN SB_DataEdit(0L, K_TBL);}
		}
	    } COMMENT "Update, create, delete, test and browse current work space tables"

	    OPTION "Variables"          {
		MN_EDIT  {
		    PROFILE AUTO BACK REVERSE
		    OPTION "  VARIABLES  " OFF
		    OPTION "ฤฤฤฤฤฤฤฤฤฤฤฤฤ" OFF
		    OPTION "Edit WS      "   {C_FN SB_DataEditScroll(0L, K_VAR);}
		    OPTION "Edit List    "   {C_FN SB_DataEdit(0L, K_VAR);}
		    OPTION "Edit Tables  "   {C_FN SB_ViewByTbl();}
		}
	    } COMMENT "Update, create, delete, graph and browse current work space variables"

	    OPTION "อออออออออออออออออ" OFF
	    OPTION "  MISCELLANEOUS  " OFF
	    OPTION "ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ" OFF
	    OPTION "Duplicate objects"  {C_FN SB_DataDuplicate();}    COMMENT "Duplicate objects in the current work space"
	    }

    OPTION "Compute" ALT_C COMMENT "Residual check, Simulation, Estimation and identity execution"
	MENU {
	    PROFILE
	    OPTION "       M O D E L S       "  OFF
	    OPTION "ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ"  OFF
/*            OPTION "Model residual check"           {PG_edit vkp_reschk}             COMMENT "Compute the residuals on a set of equations" */
	    OPTION "Simulation - Goal Seeking"        {C_FN SB_ModelSimulate(); }      COMMENT "Simulate a model"
	    OPTION "Compile Model           "         {C_FN SB_ModelCompile();  }      COMMENT "Compile a set of equations"
	    OPTION "อออออออออออออออออออออออออ"  OFF
	    OPTION "SCC Decomposition"                {C_FN SB_ModelCalcSCC();  }      COMMENT "SCC Decomposition and reordering"
	    OPTION "SCC Simulation"                   {C_FN SB_ModelSimulateSCC();  }  COMMENT "Simulate a pre-ordered model"
	    OPTION "อออออออออออออออออออออออออ"  OFF
	    OPTION "  I D E N T I T I E S    "  OFF
	    OPTION "ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ"  OFF
	    OPTION "Execute Identities"               {C_FN SB_IdtExecute();}     COMMENT "Execute a set of identities and update current variables WorkSpace"
	    }

    OPTION "Print/Graph" ALT_P COMMENT "Print/Display objects, tables and graphics"
	MENU {
	    PROFILE
	    OPTION "      P R I N T      "  OFF
	    OPTION "ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ"  OFF
	    OPTION "Print/View Tables"      {C_FN SB_ViewPrintTbl();}  COMMENT "Compute tables and Display or Print results"
	    OPTION "Print/View Variables"   {C_FN SB_ViewPrintVar();}  COMMENT "Compare, Display or Print variables"
	    OPTION "Object Definitions"     {C_FN SB_PrintObjDef();}   COMMENT "Print object definitions"
	    OPTION "ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ"  OFF
	    OPTION "   G R A P H I C S   "  OFF
	    OPTION "ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ"  OFF
	    OPTION "Graphs from Tables"     {C_FN SB_ViewPrintGr();}   COMMENT "Compute tables and Display or Print"
	    OPTION "Graphs from Variables"  {C_FN SB_DataEditGraph();} COMMENT "Display or Print series values"
	    OPTION "ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ"  OFF
	    OPTION "       F I L E S     "  OFF
	    OPTION "ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ"  OFF
	    OPTION "Print A2M File"         {C_FN SB_A2MPrint();}       COMMENT "Print A2M File"
	    OPTION "Print Report  "         {C_FN SB_ReportPrint();}    COMMENT "Print IODE Report"
	    }

    OPTION "Report" ALT_R COMMENT "Report definition and execution"
	MENU {
	    PROFILE
	    OPTION "     R E P O R T     " OFF
	    OPTION "ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ" OFF
	    OPTION "Edit report"                  {C_FN SB_ReportEdit();}       COMMENT "Create or modify a report"
	    OPTION "Execute report"               {C_FN SB_ReportExec();}       COMMENT "Execute IODE report"
	    OPTION "Execute a report line"        {C_FN SB_ReportLine();}       COMMENT "Execute a single report command"
/*          OPTION "Edit output file"             {C_FN SB_ReportEditOutput();} COMMENT "Edit report destination file" */
	    }

    OPTION "Help" ALT_H HELP "MiscODE"  COMMENT "Iode Documentation"
	MENU {
	    PROFILE
	    OPTION "         HELP/INFOS          " OFF
	    OPTION "ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ" OFF
//            OPTION "Help                   F1"    {HELP "Introduction au logiciel"}                      COMMENT "Reference manual "
//            OPTION "Iode Manual (WinHelp)"        {C_FN ODE_set_hlp(1); HELP "Introduction au logiciel"} COMMENT "Change Help System to WinHelp format"
	    OPTION "Iode Manual"                    {C_FN ODE_set_hlp(2); HELP "Introduction au logiciel"} COMMENT "Change Help system to HtmlHelp format"
	    OPTION "Iode Home"                      {C_FN WscrShellExec("http://iode.plan.be");}           COMMENT "Go to IODE Web Site"
	    OPTION "Iode Readme"                    {C_FN ODE_readme();  }                                 COMMENT "Last changes in IODE"
	    OPTION "About Iode"               ALT_Z   {C_FN ODE_DisplayBanner();}                            COMMENT "Display introduction screen"
	    }
}

PAGE vkp_status {
    TITLE "Memory Usage"
    $PGHEAD
/*    DISPLAY_FN {C_FN ODE_status(); } /* JMP_M 4.21 01-11-95 */
    DEFAULTS {nc 10 natural rjust output}
    SCREEN {
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ                 M E M O R Y    U S A G E                   บ
ฬออออออออออออออออออออออออออัออออออออออัอออออออออออัออออออออออน
บ Storage type             ณAllocated ณFree space ณUsed spaceบ
วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤฤลฤฤฤฤฤฤฤฤฤฤถ
บ Conventional memory      ณ*convallocณ *convafreeณ*convausedบ
บ Expanded memory          ณ*emsalloc ณ *emsafree ณ*emsaused บ
บ Swap on File             ณ*fileallocณ *fileafreeณ*fileausedบ
ฬออออออออออออออออออออออออออุออออออออออฯอออออออออออฯออออออออออน
บ Number of allocations    ณ*nballoc                         บ
บ Number of blocks         ณ*nbblks                          บ
บ Number of segments       ณ*nbsegs                          บ
บ Segment size             ณ*segsize                         บ
ฬออออออออออออออออออออออออออฯอออออออออออออออออออออออออออออออออน
บ                                                            บ
บ                        &ESC                                บ
บ                                                            บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
}
    FIELD name nballoc
    FIELD name nbblks
    FIELD name nbsegs
    FIELD name segsize
    FIELD name emsalloc
    FIELD name filealloc
    FIELD name convalloc
    FIELD name emsaused
    FIELD name fileaused
    FIELD name convaused
    FIELD name emsafree
    FIELD name fileafree
    FIELD name convafree

    NODEFAULTS
    $BUTTONESC "  Continue  "
}

#include "odefile.f"
#include "odeprof.f"
#include "odews.f"
#include "odedata.f"
#include "odecomp.f"
#include "odeprint.f"
#include "oderep.f"
#include "odexode.f"

PAGE global {        /* Paramtres globaux JMP 30-11-93 */
    PROFILE
    FIELD name tw   integer "22"
    FIELD name tn   integer "-1"
    FIELD name sw   integer "10"    // Scalar cells width
    FIELD name sn   integer "3 "
    FIELD name vw   integer "9 "    // Var cells width
    FIELD name vn   integer "2 "
    FIELD name vm   integer "0 "
    FIELD name vs   integer "0 "
    FIELD name vtw  integer "8 "
    FIELD name vtn  integer "-1"
    FIELD name vt0w integer "37"
    FIELD name prec integer "8 "
    FIELD name wnl  integer " 30"
    FIELD name wnc  integer "100"
    FIELD name lc0  integer " 10" // 1st col (names) width

}


PAGE p_playback_cmt {
    $PGHEADNPR
    //END_FN {C_FN return(PB_comment());}
    SCREEN {
ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ                    Play back comment                            บ
ฬอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออน
บ Enter comment : $cmt                                            บ
บ                                                                 บ
วฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤบ
บ    &F10              &ESC                 &F1                   บ
บ                                                                 บ
ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
}

    FIELD name cmt nc 120 ncv 45 string $FLDBOX
    $BUTTONESC " Cancel "
    $BUTTONF10 "   Ok   "
    $BUTTONF1  "  Help  "
}











