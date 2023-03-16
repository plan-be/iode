<IODE: Table calculation>
..sep |
..esc ~
..ignore ¯

IODE: Table calculations
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
When a table is printed, the cell values are simple texts or numbers calculated from 
the LEC formulas included in the table definition on a GSAMPLE. The same is true for the line or bar charts generated from IODE TBLs.
  
Indeed, the table formulas don't give indication on the period(s), the files, nor the 
representation of the values (growth rates, differences...). 

The purpose of the GSAMPLE is to provide these informations.

&TI GSAMPLE
¯¯¯¯¯¯¯¯¯¯¯¯
A GSAMPLE (generalised printing sample) determines the source of the variables 
and the transformations applied to these values like growth rates of differences in percents.

For tables, each column of a table is printed for every (period / file) given in the GSAMPLE.

The GSAMPLE contains informations about:
&EN the periods to be printed
&EN the operations to perform on the selected periods, such as growth rates...
&EN the files where the variables are to be found

For example:
&EN "2000:10" means "10 consecutive periods beginning in 2000". 
&EN "2000Q2/2000Q1:5[1;2]" means "5 consecutive growth rates beginning in 
    2000Q2 from the current values in the workspace ([1]) and the file number [2]".
&EN etc.

The available operations on periods include :
&EN values (75;80;85)
&EN growth rates between one or more periods (75/74, 75/70)
&EN average growth rates (75//70)
&EN differences and average diffences (75-74, 75--70)
&EN means, sums, indices

The available operations on files include :
&EN level [1]
&EN differences ([1-2])
&EN differences in % ([1/2])
&EN etc

Examples and more details on GSAMPLE can be found in the IODE manual (https://iode.plan.be/doku.php?id=sample_d_impression). 

&IT Formal definition of a GSAMPLE
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
&CO
     gsample    ::= gsample_n[';'gsample_n...]
     gsample_n  ::= gsample_1[repetition['*'['-']increment]] 
     repetition ::= integer
     increment  ::= integer
 
     gsample_1  ::= period_ops [file_ops]
     period_ops ::= period_op | '(' period_op ';' period_op... ')'
     period_op  ::= period [opp period] ['>>' shift] ['<<' shift]
     period     ::= integer[periodicity period] | 
                     'EOS' | 'BOS' | 'NOW' | 'EOS1' | 'BOS1' |'NOW1'
     shift      ::= 0 | integer
     opp        ::= '-' | '--' | '/' | '//' | '+' | '^' | '~' | '=' 
 
     file_ops   ::= '[' file_op | file_op ';' file_op ']'
     file_op    ::= file [opf file] 
     file       ::= integer
     opf        ::= '+' | '-' | '/' | '+' | '^' 
&TX

&TI c_cc.c
¯¯¯¯¯¯¯¯¯¯
GSAMPLE compilation functions.

..tb
| Syntax                                               | Description
| ~cCOLS *COL_cc(char* gsample)                        | GSAMPLE compiler
| ~cint COL_free_cols(COLS* cls)                       | Frees the allocated space for a COLS structure created by COL_cc()
| ~cchar *COL_text(COL* cl, char* str, int nbnames)    | Constructs a string based on a special table cell text value containing a "##"
| ~cCOLS *COL_add_col(COLS* cls)                       | Adds a new COL struct to the COLS (list of periods in a GSAMPLE)
| ~cint COL_find_mode(COLS* cls, int* mode, int type)  | Analyses a COLS struct and set 1 in the vector mode for each found operation
..te

&TI c_calc.c
¯¯¯¯¯¯¯¯¯¯¯¯
Table calculation on a GSAMPLE.

..tb
| Syntax                                       | Description
| ~cint COL_exec(TBL* tbl, int i, COLS* cls)   | Calculates the values of all LEC formulas in one TBL line for all columns of a GSAMPLE.
| ~cint COL_resize(TBL* tbl, COLS* cls)        | Extends the number of COL's (compiled GSAMPLE) by multiplying by the number of columns in a TBL definition
| ~cvoid COL_clear(COLS* cls)                  | Resets the calculated values in a COLS structure. 
..te

&TI k_print.c
¯¯¯¯¯¯¯¯¯¯¯¯¯
Functions to generate IODE tables in A2M format based on TBL structures and GSAMPLE definition.
Includes some A2M helper functions. 

..tb
| Syntax                                                       | Description
| ~cint T_prep_cls(TBL* tbl, char* smpl, COLS** cls)           | Compiles a GSAMPLE into a COLS struct and resizes COLS according to the nb of cols in TBL
| ~cvoid T_fmt_val(char* buf, IODE_REAL val, int lg, int nd)   | Formats a double value
| ~cvoid T_print_val(IODE_REAL val)                            | Prints a IODE_REAL value using W_printf()
| ~cvoid T_open_cell(int attr, int straddle, int type)         | Prints the header of an a2m table cell
| ~cvoid T_open_attr(int attr)                                 | Opens an A2M attribute sequence.
| ~cvoid T_close_attr(int attr)                                | Closes an A2M attribute sequence.
| ~cvoid T_print_cell(TCELL* cell, COL* cl, int straddle)      | Prints a TBL cell on a specific GSAMPLE column. 
| ~cchar **T_find_files(COLS* cls)                             | Retrieves the filenames used in the COLS (from GSAMPLE) needed to print the special table line KT_FILES.
| ~cunsigned char *T_get_title(TBL* tbl)                       | Retrieves a TBL title, i.e. the contents of the first line of type KT_TITLE
| ~cint T_print_tbl(TBL* tbl, char* smpl)                      | Computes a table on a GSAMPLE and saves the result in A2M format
..te

Global variables
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
| Variable        | Default value  | Description
| ~cchar KT_sep   | ~c '&'         | Table cell separator
| ~cint  K_NBDEC  | ~c -1          | Default nb of decimals
..te

&TI k_graph.c
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
Functions to generate IODE graphs in A2M format based on a TBL structure and a GSAMPLE definition.
Contains also V_graph(), a function to print or display variables or combinations of variables on a (simple) SAMPLE.

Includes some A2M helper functions. 

..tb
| Syntax                                                                                               | Description
| ~cint T_GraphInit(double w, double h, int xgrid, int ygrid, double ymin, double ymax, double zmin, double zmax, int align, int box, int brush)  |  Initialises a graph by sending a2m commands to W_printf().
| ~cint T_GraphTest(TBL *tbl)                                                                           | Displays the table tbl as a graph (in level) on the full sample of the current WS.
| ~cint T_GraphEnd()                                                                                    | Ends a A2M graph definition by sending the a2m command ".ge" to W_printf().
| ~cint T_graph_tbl_1(TBL *tbl, char *gsmpl, int mode)                                                  | Generates one graph in A2M format from a TBL struct and a GSAMPLE.
| ~cint T_GraphTitle(char *txt)                                                                         | Defines the graph title by sending a2m command ".gtitle" to W_printf().
| ~cint T_GraphLegend(int axis, int type, char *txt, char *fileop)                                      | Adds (in A2M) graph *time* axis (.gty or .gtz, see a2m language) with its position, type and title.
| ~cint T_GraphXYLegend(int axis, int type, char *txt, char *fileop)                                    | Adds (in A2M) graph *xy* axis with its position, type and title.
| ~cint T_GraphTimeData(SAMPLE *smpl, IODE_REAL *y)                                                     | Adds numerical data on a *time* graph line or bar.
| ~cint T_GraphXYData(int nb, IODE_REAL *x, IODE_REAL *y)                                               | Adds numerical data on a *xy* graph line or bar.
| ~cint T_GraphLine(TBL *tbl, int i, COLS *cls, SAMPLE *smpl, IODE_REAL *x, IODE_REAL *y, COLS *fcls)   |  Adds graph curves from a table line definition and a calculated GSAMPLE. 
| ~cint T_find_opf(COLS *fcls, COL *cl)                                                                 | Tries to find the position in *fcls of the opf (operation on files) in cl.
| ~cint T_prep_smpl(COLS *cls, COLS **fcls, SAMPLE *smpl)                                               | Given a compiled GSAMPLE, constructs a new COLS struct with unique file ops and the minimum SAMPLE smpl containing all periods present in cls.
| ~cint V_graph(int view, int mode, int type, int xgrid, int ygrid, int axis, double ymin, double ymax, SAMPLE* smpl, char** names) | Prints or displays graph(s) from variable list(s) or combination(s) or variables.

..te


&TI Translations
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
The printed/displayed version of computed tables and graphs contain texts that can be translated. Three languages 
are available: French, Dutch and English. The translated strings are grouped in ~capi/k_lang.c~C.

&IT k_lang.c
¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
| Variables                      | Description
| ~cchar *KLG_TYPE[][3]          | Texts describing object types
| ~cchar *KLG_MONTHS[][3]        | Texts representing Months
| ~cchar *KLG_ROM[]              | Roman numerals from 1 to 12
| ~cchar *KLG_CALC[3]            | Legends for computed values line in estimation graphs
| ~cchar *KLG_OBS[3]             | Legends for observed values line in estimation graphs
| ~cchar *KLG_RES[3]             | Legends for residuals in estimation graphs
| ~cchar *KLG_MODES[5][3]        | Display modes for variables in computed Tables and Graphs    
| ~cchar *KLG_OPERS_TEXTS[][3]   | Texts representing operations on files and periods in computed tables and graphs
..te

>