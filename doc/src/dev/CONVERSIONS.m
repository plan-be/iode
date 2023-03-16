<IODE: Object conversions>
IODE: Object conversions (Import / Export)
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..sep |
..esc ~
..ignore ¯

IODE offers several functions to import and export objects from and to various formats.
Some formats date back to the 80s and 90s and are now obsolete, but are still available in the
software and therefore kept (in a simplified version) for sake of completeness.

&TI Export
¯¯¯¯¯¯¯¯¯¯¯¯
For each output format, an ~cEXPDEF~C struct (mainly a table of function pointers) must be given 
as argument to one of the general functions ~cEXP_Ws()~C or ~cEXP_Rev_Ws()~C (found in k_emain.c). 
These functions, specific to each output format, are coded in one of the files ~bk_e*.c~B 
(for example ~bk_ecvs.c~B for csv output).

&TI Import
¯¯¯¯¯¯¯¯¯¯¯¯
The same logic applies to the import of object: for each import format, 
the general functions ~cIMP_InterpretVar()~C and ~cIMP_InterpretCmt()~C (defined in k_emain.c) are to be fed with  
an ~cIMPDEF~C struct (a table of function pointers).

These functions, related to one input format, are coded in the files ~bk_i*.c~B (for ex ~bk_iasc.c~B).

&TI Rule files
¯¯¯¯¯¯¯¯¯¯¯¯¯
To limit the output or input to a group of objetcs and optionally rename the objects in the output/input file, a set 
of rules can be specified. The syntax and logic of the rule file is explained below.

&IT Definition of "Rules"
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
Rules are defined in an external file and allow essentially two things :

&EN limit the variables to be im/exported 
&EN rename the objects (if the name in the ASCII file is not appropriate for IODE)

&IT Syntax of "Rules"
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
Each rule is defined on one line and composed of two fields:

&EN the selection mask (pattern) containing a description of the names concerned by the rule. 
  This mask is defined with the same syntax as the search in IODE objects (see Data menu), i.e. by possibly including * and ? to specify sets.
&EN  the name transcoding algorithm for this rule, which may contain: 
&EN2    + : indicating that the character at this position in the source name should be included as is in the output name 
&EN2    - : any character: indicates that the character at this position in the source name should be skipped 
&EN2    any other character is kept as is in the resulting name

&IT Example of rule file
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
&CO    
    B*  C+-+            =>> changes B1234 to CB2, BCDEF to CBE, etc 
    *X  ++++++++++      =>> keeps names ending in X unchanged
&TX

If the rule file is empty or undefined, the default rule is used:
&CO
   * ++++++++++
&TX

&IT Order of execution
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
Rules are executed in the order in which they are defined in the "rule file". 
As soon as a rule can be applied to a name, it is used.
It is therefore important to position "exceptions" at the beginning of the file and to put general rules to the end.

&TI Source files
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯  
&EN <k_rules.c >: import and export rules management
&EN <Export IODE objects>
&EN <Import IODE objects>
>

<Export IODE objects>
Export IODE objects
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
&EN <k_emain.c >: main export module
&EN <k_ecsv.c  >: export functions specific to CSV format 
&EN <k_edif.c  >: export functions specific to DIF format 
&EN <k_etsp.c  >: export functions specific to TSP format 
&EN <k_ewks.c  >: export functions specific to WKS format (obsolete)
&EN <k_wks.c   >: basic functions to create a WKS (Lotus 123) files (obsolete)
>

<Import IODE objects>
Import IODE objects
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
&EN <k_imain.c >: main import module       
&EN <k_iasc.c  >: import from IODE ASCII format
&EN <k_idif.c  >: import from DIF format 
&EN <k_ibst.c  >: import from BISTEL format (obsolete)
&EN <k_igem.c  >: import from GEM (Chronos) format (obsolete)
&EN <k_inis.c  >: import from INS format (obsolete)
&EN <k_iprn.c  >: import from PRN (Aremos) format (obsolete)
&EN <k_irasc.c >: import from reverse ASCII format 
&EN <k_itxt.c  >: import from Belgostat TXT format (obsolete)
>

<k_rules.c >
k_rules.c
¯¯¯¯¯¯¯¯¯¯
Management of the rules used to select objects to import/export and to optionally adapt their names.
 
&IT List of functions 
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
| Syntax                                                           | Description    
|    int IMP_readrule(char* filename)                              | Reads a "rule file" and stores its contents in 2 global variables IMP_rule and IMP_pat.
|    int IMP_change(char** rule, char** pat, char* in, char* out)  | Modifies an object name according to the rule definitions.
..te

>

<k_emain.c>
k_emain.c 
¯¯¯¯¯¯¯¯¯¯
Main functions to export (partial) KDBs into external formats like csv, reverse csv... 

The two main export functions EXP_Ws() and EXP_Rev_W() call virtual functions 
implemented in the following source files specific to each format:
&EN k_edif.c
&EN k_ecsv.c
&EN k_etsp.c
&EN k_ewks.c

For each output format, a table of function pointers must be given as argument to 
EXP_Ws() or EXP_Rev_Ws().

A set of rules can be defined to select and/or determine the name each object is given in the output file.

&IT List of functions 
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
| Syntax                                                                                                   | Description    
| ~cvoid EXP_val(char* tmp, double val)                                                                    | Formats a double value on 20 positions in general format.  
| ~cchar *EXP_addprepost(char* pre, char* post, char* src, char** tg)                                      | Creates an allocated string formatted as {pre}{src}{post}.
| ~cchar *EXP_addsep(char* src, char** tg)                                                                 | Creates an allocated string formatted as {src}{EXP_SEP}.
| ~cint EXP_Ws(EXPDEF* expdef, KDB* dbv, KDB* dbc, char* rulefile, char* outfile, char* na, char* sep)     | Exports a KDB of VARs (and optionally of CMTs) in the format defined by the virtual functions in EXPDEF
| ~cint EXP_Rev_Ws(EXPDEF* expdef, KDB* dbv, KDB* dbc, char* rulefile, char* outfile, char* na, char* sep) | Same as EXP_Ws() but the output is "rotated", i.e each column is a VAR and each line a period.    
| ~cint EXP_RuleExport(char* trace, char* rule, char* out, char* vfile, char* cfile, char* from, char* to, char* na, char* sep, int fmt)  | Exports VAR files into an external format. 
..te
>

<k_ecsv.c  >
k_ecsv.c 
¯¯¯¯¯¯¯¯¯¯
Implementation of the CSV and rotated CSV export functions. See k_emain.c for more information.


&IT Example of a CSV output file
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
&CO 
    code;comment;1990Y1;1991Y1;1992Y1;1993Y1;...
    ACAF; Ondernemingen: ontvangen kapitaaloverdrachten.; 23.771;26.240999;30.159;...
    ACAG; Totale overheid: netto ontvangen kapitaaloverdrachten.; -28.172186;-30.934;...
    .....
&TX

&IT Example of a rotated CSV output file
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
&CO 
    ; ACAF; ACAG; AOUC; AOUC_; AQC; BENEF; BQY; ...
    1990Y1; 23.771; -28.172186; 1; 0.93735915; 1; ...
    1991Y1; 26.240999; -30.934; 1.0244334; 0.96466659; ...
    .....
&TX

&IT List of functions 
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
| Syntax                                                                  | Description    
| ~cint EXP_hd_csv(EXPDEF *expdef, KDB* dbv, KDB* dbc, char* outfile)     | Opens and initialise a CSV export file.
| ~cint EXP_end_csv(EXPDEF* expdef, KDB* dbv, KDB* dbc)                   | Saves the footer and closes the CSV export files.
| ~cchar *EXP_code_csv(char* name, char** code)                           | Variable name translation for CSV output.
| ~cchar *EXP_cmt_csv(KDB* dbc, char* name, char**cmt)                    | Creates the CMT text + separator for CSV output. 
| ~cchar *EXP_elem_csv(KDB* dbv, int nb, int t, char** vec)               | Adds one element of a VAR (KDB[nb][t]) to the export vector in CSV format.
| ~cint EXP_vec_csv(EXPDEF* expdef, char* code, char* cmt, char* vec)     | Saves one VAR in the csv export file.
| ~cint EXP_hd_rcsv(EXPDEF* expdef, KDB* dbv, KDB* dbc, char*outfile)     | Opens and initialise a rotated CSV export file.
| ~cchar *EXP_elem_rcsv(KDB* dbv, int nb, int t, char** vec)              | Adds one element of a VAR (KDB[nb][t]) to the export vector in rotated CSV format.
| ~cint EXP_vec_rcsv(EXPDEF* expdef, char* code, char* cmt, char* vec)    | Saves one VAR in the rotated csv export file.
| ~cint EXP_RuleExport(char* trace, char* rule, char* out, char* vfile, char* cfile, char* from, char* to, char* na, char* sep, int fmt) | Exports VAR files into an external format. 
 
..te
>
<k_edif.c  >
k_edif.c  
¯¯¯¯¯¯¯¯¯¯
Export functions specific to DIF format.

&IT Example of a DIF output file
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
&CO 
    TABLE
    0,1
    ""
    VECTORS
    0,13
    ""
    TUPLES
    0,395
    ""
    DATA
    0,0
    ""
    -1,0
    BOT
    1,0
    "CODE"
    1,0
    "COMMENT"
    1,0
    "1990Y1"
    ...
&TX

&IT List of functions 
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
| Syntax                                                                   | Description    
| ~cint EXP_hd_dif(EXPDEF *expdef, KDB* dbv, KDB* dbc, char* outfile)      | Opens and initialise a DIF export file.
| ~cint EXP_end_dif(EXPDEF* expdef, KDB* dbv, KDB* dbc)                    | Saves the footer and closes the DIF export files.
| ~cchar *EXP_code_dif(char* name, char** code)                            | Variable name translation for DIF output.
| ~cchar *EXP_cmt_dif(KDB* dbc, char* name, char**cmt)                     | Creates the CMT text + separator for DIF output. 
| ~cchar *EXP_elem_dif(KDB* dbv, int nb, int t, char** vec)                | Adds one element of a VAR (KDB[nb][t]) to the export vector in DIF format.
| ~cint EXP_vec_dif(EXPDEF* expdef, char* code, char* cmt, char* vec)      | Saves one VAR in the DIF export file.
..te 
>

<k_etsp.c  >
k_etsp.c  
¯¯¯¯¯¯¯¯¯¯
Export functions specific to TSP format.

&IT Example of a TSP output file
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
&CO 
        FREQ A;
        SMPL 1990 2000 ;
        LOAD ACAF ;
        
        ? Ondernemingen: ontvangen kapitaaloverdrachten.
        23.771 26.240999 30.159 34.661999 8.1610022 -13.130997 32.171001 39.935799 
        29.645657 13.530405 10.046611 
        ; 
        LOAD ACAG ;
        
        ? Totale overheid: netto ontvangen kapitaaloverdrachten.
        -28.172186 -30.934 -40.285999 -43.157997 -16.029003 -41.845993 -40.237 -32.93 
        -38.345695 -39.858174 -41.534787 
        ; 
        ...
&TX

&IT List of functions 
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
| Syntax                                                                   | Description    
| ~cint EXP_hd_tsp(EXPDEF *expdef, KDB* dbv, KDB* dbc, char* outfile)      | Opens and initialise a tsp export file.
| ~cint EXP_end_tsp(EXPDEF* expdef, KDB* dbv, KDB* dbc)                    | Saves the footer and closes the tsp export files.
| ~cchar *EXP_code_tsp(char* name, char** code)                            | Variable name translation for tsp output.
| ~cchar *EXP_cmt_tsp(KDB* dbc, char* name, char**cmt)                     | Creates the CMT text + separator for tsp output. 
| ~cchar *EXP_elem_tsp(KDB* dbv, int nb, int t, char** vec)                | Adds one element of a VAR (KDB[nb][t]) to the export vector in tsp format.
| ~cint EXP_vec_tsp(EXPDEF* expdef, char* code, char* cmt, char* vec)      | Saves one VAR in the tsp export file.
..te
>

<k_ewks.c  >
k_ewks.c  
¯¯¯¯¯¯¯¯¯¯
Export functions specific to WKS format (obsolete).

&IT List of functions 
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
| Syntax                                                                    | Description    
| ~cint EXP_hd_wks(EXPDEF *expdef, KDB* dbv, KDB* dbc, char* outfile)       | Opens and initialise a wks export file.
| ~cint EXP_end_wks(EXPDEF* expdef, KDB* dbv, KDB* dbc)                     | Saves the footer and closes the wks export files.
| ~cchar *EXP_code_wks(char* name, char** code)                             | Variable name translation for wks output.
| ~cchar *EXP_cmt_wks(KDB* dbc, char* name, char**cmt)                      | Creates the CMT text + separator for wks output. 
| ~cchar *EXP_elem_wks(KDB* dbv, int nb, int t, char** vec)                 | Adds one element of a VAR (KDB[nb][t]) to the export vector in wks format.
| ~cint EXP_vec_wks(EXPDEF* expdef, char* code, char* cmt, char* vec)       | Saves one VAR in the wks export file.
..te 
>

<k_wks.c>
k_wks.c
¯¯¯¯¯¯¯¯
Basic functions to create a WKS (Lotus 123) files (obsolete)

&IT List of functions 
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
| Syntax                                                        | Description    
| ~cint wks_init(char* file, int c, int r)                      |
| ~cint wks_cwidth(int c, int w)                                |
| ~cint wks_end()                                               |
| ~cvoid wks_string(char* label, int c, int r)                  |
| ~cvoid wks_value(double value, int c, int r)                  |
| ~cvoid wks_name(char* str, int c1, int r1, int c2, int r2)    |
..te

>
<k_imain.c >
k_imain.c 
¯¯¯¯¯¯¯¯¯¯
Functions to import variables and comments from files in various external formats.

For each input format, a structure IMPDEF, which is essentially a table of function pointers,
must be given as argument to one of the general functions IMP_InterpretVar() or IMP_InterpretCmt().

&IT Available input formats
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
&EN in k_iasc.c:  IODE ASCII for comments and variables
&EN in k_irasc.c: rotated ASCII variables
&EN in k_ibst.c:  Bistel format (obsolete)
&EN in k_idif.c:  DIF format (Data Interchange Format) 
&EN in k_igem.c:  Chronos format (obsolete)   
&EN in k_inis.c:  INS format (obsolete)
&EN in k_iprn.c:  Aremos prn format (obsolete)
&EN in k_itxt.c:  Belgostat format (Obsolete)

&IT Rule file
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
A set of rules can also be defined to select and/or determine (transform) the name of each read object.
See k_emain.c for details on rule files.
     
&IT List of functions 
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
| Syntax                                                                                | Description    
| ~cKDB *IMP_InterpretVar(IMPDEF* impdef, char* rulefile, char* vecfile, SAMPLE* smpl)  | Interprets a text file containing VAR definitions
| ~cKDB *IMP_InterpretCmt(IMPDEF* impdef, char* rulefile, char* cfile, int lang)        | Interprets an ASCII file containing COMMENTS definitions
| ~cint IMP_RuleImport(int type, char* trace, char* rule, char* ode, char* asc, char* from, char* to, int fmt, int lang) | Imports variables or comments in various formats.
..te

>

<k_iasc.c  >
k_iasc.c  
¯¯¯¯¯¯¯¯¯¯
Functions to import variables and comments in ASCII format. 

&IT VAR format (sample is required)
&CO
   sample 1980Y1 1990Y1
   A 1 2 3 na na 4 5
   B 0.1 0.2 -3.14
   .....
&TX

&IT CMT format (sample is required)
&CO
    A "Description of A" 
    B "..."
    ...
&TX
&IT List of functions 
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
| Syntax                                                                | Description    
| ~cint IMP_hd_asc(YYFILE* yy, SAMPLE* smpl)                            | Reads the sample (required) in an ASCII variable file.
| ~cint IMP_vec_asc(YYFILE* yy, char* name, int dim, IODE_REAL* vector) | Reads a VAR name and values in an ASCII variable file format. 
| ~cint IMP_hd_casc(IMPDEF* impdef, char* file, int lang)               | Opens an ASCII comment file for reading with the YY library functions. 
| ~cint IMP_vec_casc(char* name, char** cmt)                            | Reads one comment on the open YY stream.
..te
>


<k_irasc.c >
k_irasc.c 
¯¯¯¯¯¯¯¯¯¯
Functions to import variables in rotated ASCII format.

&IT VAR format (sample is required)

&CO
    sample 1980Y1 1990Y1
    A B C
    1 2 3
    4.2 na 5.4
    na na 4 
    .....
&TX
&IT List of functions 
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
| Syntax                                                                   | Description    
| ~cint IMP_hd_rasc(YYFILE* yy, SAMPLE* smpl)                              | Reads the sample (required) and the list of VARs in a rotated ASCII variable file.
| ~cint IMP_elem_rasc(YYFILE* yy, char* name, int* shift, IODE_REAL* value)| Reads one value in an ASCII variable file format. 
| ~cint IMP_end_rasc()                                                     | Frees the allocated vars during the rotated ASCII file import session.
..te
>

<k_ibst.c  >
k_ibst.c  
¯¯¯¯¯¯¯¯¯¯
Functions to import variables and comments from the legacy Bistel format (obsolete). 

When loaded in 123, the format generated by the Belgostat program is as follows:
&CO
   123456789012345     1       1980      1             456900     1       1
   (series nb)       (freq)    (year)    (per in year) (value)  (status)  (free)
   ........
&TX

&IT List of functions 
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
See k_idif.c for a similar group of functions.

>

<k_idif.c  >
k_idif.c  
¯¯¯¯¯¯¯¯¯¯
Functions to import variables from a DIF formatted ASCII file (obsolete). 

See also k_edif.c for an example of DIF format.

&IT DIF format viewed in Excel 
&CO
   CODE 1980Y 1981Y1 1982Y1 1983Y1 ....
   A    1     1       2      4
   B    1     1       2      5
&TX

&IT List of functions 
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
| Syntax                                                               | Description    
| ~cint DIF_skip_to(YYFILE* yy, int skey)                              | Moves forward to the keyword skey or EOF or EOD.
| ~cint DIF_cell(YYFILE* yy, char** str, double* value)                | Reads the next cell which can contain a real or a string.
| ~cint IMP_hd_dif(YYFILE* yy, SAMPLE* smpl)                           | Reads the header of a DIF file and determines the sample of the content.
| ~cint IMP_vec_dif(YYFILE* yy, char* name, int dim, IODE_REAL* vector)| Reads a VAR name and values in a DIF file.
| ~cint IMP_end_dif()                                                  | Cleanup the DIF global variables.
..te
>

<k_igem.c  >
k_igem.c 
¯¯¯¯¯¯¯¯¯¯
Functions to import variables from the legacy GEM (Chronos) format (obsolete). 

&IT GEM VAR format
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
&CO
   nobs nser rub .....
   subrub[nser1][nobs1] subrub[nser2][nobs1] ...
   ....
   subrub[nser1][nobs2] subrub[nser1][nobs2] ...
&TX

&IT List of functions 
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
See k_idif.c for a similar group of functions.

>

<k_inis.c  >
k_inis.c 
¯¯¯¯¯¯¯¯¯¯

Functions to import variables from the legacy INS-NIS format (obsolete). 

&IT NIS VAR format
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
&CO
    0000 XXX 91 1    one   two   three  for   five  six
    0-3  6-8 11-15   16-25 27-36 38-47 49-58 60-61 71-80
&TX

&IT List of functions 
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
See k_idif.c for a similar group of functions.
>

<k_iprn.c  >
k_iprn.c  
¯¯¯¯¯¯¯¯¯¯
Functions to import variables from the legacy PRN (Aremos) format (obsolete). 

&IT PRN VAR format
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
&CO
	"Name" 1960 1961 ... 1990
	"A" 1 2 3 ... 4 5
	"B" 0.1 0.2 ... -3.14
    ...
&TX

&IT PRN CMT format
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
&CO
	"A" "Comment of A"
	"B" "Comment of B"
    ...
&TX
&IT List of functions 
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
See k_idif.c for a similar group of functions.
>

<k_itxt.c  >
k_itxt.c
¯¯¯¯¯¯¯¯¯¯
Functions to import variables and comments from the legacy Belgostat format (obsolete). 

&IT Format
¯¯¯¯¯¯¯¯¯¯
&CO 
     N;0104104000023;   1;19801231;               8,3;D
     D;code;freq;date;value;status
  
     0104104000023;N;Werkloosheidsgraden (bron: Eurostat), Seizoengezuiverde gegevens;Taux de chomage (source: Eurostat), Donnees desaissonnalisees;Taux de chomage (source: Eurostat), Donnees desaissonnalisees; ;0,010;
     code;N;cmtnl;cmtfr;cmten;units1;units2
&TX
&IT List of functions 
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
See k_idif.c for a similar group of functions.
>
