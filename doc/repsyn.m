START
#include "macros.h"

<Syntaxe des rapports>
    Syntaxe des rapports
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Les mots-cl‚s commencent par une majuscule (Label, NoPrint, ...). Les
paramŠtres qui ne sont pas des mots-cl‚s commencent par une minuscule
(labelname, question_text, command, ...).

&TI Commandes sp‚cifiques aux rapports
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Toutes ces commandes sont pr‚fix‚es indiff‚remment par $ ou ##.

&CO
    $Ask        labelname question_text
    $Beep
    $Goto       labelname [{$cmd} | {LEC-formula} | 0 | 1}]
    $Label      labelname
    $OnError    {Ignore | Return | Abort | QuitOde
		 | Print | NoPrint | Display}
    $Return
    $Abort
    $Quit
    $Show       free_text
    $Msg        free_text
    ##Prompt     macroname question
    $System     command;command;...
    $Define     macroname text
    $SetTime    period
    $IncrTime   [step]
    $shift
&TX

&TI D‚finition des outputs
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&CO
    $PrintDest  [filename [{A2M|RTF|MIF|CSV|HTML}]]
    $PrintDestNew  [filename [{A2M|RTF|MIF|CSV|HTML}]]
    $PrintNbDec nn (-1 pour une adaptation automatique … la valeur)
    $PrintLang  {Fr | Nl | En}

    $PrintA2mAppend [NO|Yes]
    $PrintFont      Time|Helvetica|Courier|Bookman|Palatino [size [incr]]
    $PrintTableFont Time|Helvetica|Courier|Bookman|Palatino [size]
    $PrintTableBox  n
    $PrintTableColor [NO|Yes]
    $PrintTableWidth width [col1 [coln]]
    $PrintTableBreak [NO|Yes]
    $PrintTablePage  [NO|Yes]

    $PrintBackground Black|Blue|Magenta|Cyan|Red|Green|Yellow|White
    $PrintGraphBox   n
    $PrintGraphBrush pct|Yes
    $PrintGraphSize  width_mm [height_mm [fontsize_pts]]
    $PrintGraphPage  [NO|Yes]

    $PrintRtfHelp    [YES|No]
    $PrintRtfTopic   titre du topic
    $PrintRtfLevel   [+|-|n]
    $PrintRtfTitle   titre du help
    $PrintRtfCopyright copyright text
    $PrintParanum    [NO|Yes]
    $PrintPageHeader titre des pages suivantes
    $PrintPageFooter  footnote des pages suivantes
&TX

&TI Fonctions de fichiers
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Les fonctions ayant A2m comme suffixe existent pour toutes les extensions
suivantes :

&CO
    Cmt,    Eqs,    Idt,    Lst,    Scl,    Tbl,    Var,    Ooo,
    Asc,    A2m,    Dif,    Rep,    Log,    Prf,    Mif,    Rtf,    Agl
&TX

&IT Non Fullscreen

&CO
    $FileList       mask
    $FileEdit       filename
    $FileDeleteA2m  filename ...
    $FileRenameA2m  filename newname
    $FileCopyA2m    filename newname
&TX

&IT Fullscreen

&CO
    ##FileList
    ##FileEdit
    ##FileDelete
    ##FileRename
    ##FileCopy
&TX

&TI Fonctions de Workspaces
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Les fonctions ayant Cmt comme suffixe existent pour toutes les extensions
suivantes :

&CO
    Cmt,    Eqs,    Idt,    Lst,    Scl,    Tbl,    Var
&TX

&IT Non Fullscreen

&CO
    $WsLoadCmt      filename
    $WsSaveCmt      filename
    $WsDescrCmt     newdescription
    $WsClearCmt
    $WsCopyCmt      file1,file2,... name1 name2 ...
    $WsCopyVar      file1,file2,... [perfrom perto] name name ...
    $WsMergeCmt     file
    $WsSample       perfrom perto
    $WsExtrapolate  [method] perfrom perto [name name ...]
&TX

&IT Fullscreen

&CO
    ##WsLoad
    ##WsCopy
    ##WsMerge
    ##WsSave
    ##WsDescr
    ##WsClear
    ##WsSample
    ##WsExtrapolate
&TX

&TI Fonctions de Donn‚es
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Les fonctions ayant Cmt comme suffixe existent pour toutes les extensions
suivantes, sauf mention du contraire :

&CO
    Cmt,    Eqs,    Idt,    Lst,    Scl,    Tbl,    Var
&TX

&IT Non Fullscreen

&CO
    $DataUpdateCmt name value
    $DataUpdateEqs name value
    $DataUpdateIdt name value
    $DataUpdateLst name value
    $DataUpdateScl name [value [relax]]
    $DataUpdateTbl name lec;lec;lec...
    $DataUpdateVar name [{Level | Grt | Diff}] perfrom val1 val2 ...

    $DataCreateCmt      name
    $DataExistCmt       name
    $DataDeleteCmt      name
    $DataRenameCmt      oldname newname /* Not for Eqs */
    $DataDuplicateCmt   oldname newname /* Not for Eqs */
    $DataAppendCmt      name text
    $DataAppendIdt      name text

    $DataSearchCmt      text word ecase names lec text outlistname
			where word, ecase, names, lec, text are {0 | 1}
    $DataListSort       inputlistname outputlistname
    $DataScanIdt        [name1 name2 ...]
    $DataScanTbl        [name1 name2 ...]
    $DataScanEqs        [name1 name2 ...]

    $DataEditCnf        {Level | Grt | Dif} nbdec
    $DataCalcVar        varname lec_expression
    $DataDisplayGraph   {Level | Diff | Grt}
			{Line | Scatter | Bar | Mixt}
			{NoXGrids | MinorXGrids | J(MajorXGrids)}
			{NoYGrids | MinorYGrids | J(MajorYGrids)}
			{Level | G(Log) | Semi-Log | Percent}
			{ymin | --} {ymax | --}
			perfrom perto varname1 varname2 ...

    $DataSaveGraph      aglfilename
			{Level | Diff | Grt}
			{Line | Scatter | Bar | Mixt}
			{NoXGrids | MinorXGrids | J(MajorXGrids)}
			{NoYGrids | MinorYGrids | J(MajorYGrids)}
			{Level | G(Log) | Semi-Log | Percent}
			{ymin | --} {ymax | --}
			perfrom perto varname1 varname2 ...
&TX


&IT Fullscreen

&CO
    ##DataEditCmt       [name1 name2 ...]
    ##DataSearch
    ##DataDuplicate
    ##DataListSort
    ##DataDisplayGraph
    ##DataSaveGraph
    ##DataEditCnf
&TX


&TI Fonctions d'impression
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Les fonctions ayant Cmt comme suffixe existent pour toutes les extensions
suivantes, sauf mention du contraire :

&CO
    Cmt,    Eqs,    Idt,    Lst,    Scl,    Tbl,    Var
&TX


&IT Non Fullscreen

&CO
    $PrintObjDefCmt     [name ...] (dft = all)
    $PrintTblFile       2-5 varfilename
    $PrintTbl           gsample tblname1[+tblname2+..] ...
    $PrintVar           gsample lec1 lec2 ...

    $ViewTblFile        2-5 varfilename
    $ViewTbl            gsample tblname1[+tblname2+..] ...
    $ViewVar            gsample lec1 lec2 ...

    $ViewGr             gsample tblname1[+tblname2+..] ...
    $PrintGrAll         gsample tblname1[+tblname2+..] ...
    $PrintGrData        gsample tblname1[+tblname2+..] ...
    $PrintGrWin         gsample tblname1[+tblname2+..] ...
&TX

&IT Fullscreen

&CO
    ##PrintObjDefCmt
    ##PrintTbl
    ##ViewTbl
    ##ViewGr
    ##PrintGr
&TX


&TI Simulation
ÄÄÄÄÄÄÄÄÄÄ

&IT Non Fullscreen

&CO
    $ModelCalcSCC       nbtri lstpre lstinter lstpost
    $ModelSimulate      perfrom perto [eqname1 eqname2 ...]
    $ModelSimulateSCC   perfrom perto prelist interlist postlist
    $ModelSimulateParms eps relax maxit
			{Connex | Triang | None}
			0-4                         (starting values)
			{Yes | No}                  (debug)
			{Yes | No}                  (debug sous-it‚ration)
			nbtri
    $ModelExchange      eqname1-varname1,eqname2-varname2,...
    $ModelCompile       [eqname1 eqname2,...]
&TX

&IT Fullscreen

&CO
    ##ModelSimulate
    ##ModelCompile
&TX

&TI Ex‚cution d'identit‚s
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&IT Non Fullscreen

&CO
    $IdtExecute             perfrom perto [idtname1 ...]
    $IdtExecuteVarFiles     varfilename1 ...
    $IdtExecuteSclFiles     sclfilename1 ...
    $IdtExecuteTrace        {Yes | No}
&TX

&IT Fullscreen

&CO
    ##IdtExecute
&TX


&TI Estimation
ÄÄÄÄÄÄÄÄÄÄ

&IT Non Fullscreen

&CO
    $EqsSetMethod   {0|1|2|3|4} eqname1 eqname2 ...
    $EqsSetSample   from to   eqname1 eqname2 ...
    $EqsSetBloc     eq1 eq2 eq3 ...
    $EqsSetInstrs   eqname instr1;intr2;instr3;...
    $EqsSetCmt      eqname comment
    $EqsEstimate    perfrom perto eqname1 ...
&TX

&IT Fullscreen

&CO
    ##EqsEstimate
&TX

&TI Rapports
ÄÄÄÄÄÄÄÄ

&IT Non Fullscreen

&CO
    $ReportExec     reportfilename1 ...
    $ReportEdit     reportfilename
&TX

&IT Fullscreen

&CO
    ##ReportExec
    ##ReportEdit
&TX
>










