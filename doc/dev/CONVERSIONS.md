<!-- This content was generated by scr4w_amd -->

# Table of Contents



- [IODE: Object conversions (Import / Export)](#T1)
    - [Export](#T2)
    - [Import](#T3)
    - [Rule files](#T4)
      - [Definition of "Rules"](#T5)
      - [Syntax of "Rules"](#T6)
      - [Example of rule file](#T7)
      - [Order of execution](#T8)
    - [Source files](#T9)
  - [k\_rules.c](#T10)
      - [List of functions](#T11)
  - [Export IODE objects](#T12)
    - [k\_emain.c](#T13)
      - [List of functions](#T14)
    - [k\_ecsv.c](#T15)
      - [Example of a CSV output file](#T16)
      - [Example of a rotated CSV output file](#T17)
      - [List of functions](#T18)
    - [k\_edif.c](#T19)
      - [Example of a DIF output file](#T20)
      - [List of functions](#T21)
    - [k\_etsp.c](#T22)
      - [Example of a TSP output file](#T23)
      - [List of functions](#T24)
    - [k\_ewks.c](#T25)
      - [List of functions](#T26)
    - [k\_wks.c](#T27)
      - [List of functions](#T28)
  - [Import IODE objects](#T29)
    - [k\_imain.c](#T30)
      - [Available input formats](#T31)
      - [Rule file](#T32)
      - [List of functions](#T33)
    - [k\_iasc.c](#T34)
      - [VAR format (sample is required)](#T35)
      - [CMT format (sample is required)](#T36)
      - [List of functions](#T37)
    - [k\_idif.c](#T38)
      - [DIF format viewed in Excel](#T39)
      - [List of functions](#T40)
    - [k\_ibst.c](#T41)
      - [List of functions](#T42)
    - [k\_igem.c](#T43)
      - [GEM VAR format](#T44)
      - [List of functions](#T45)
    - [k\_inis.c](#T46)
      - [NIS VAR format](#T47)
      - [List of functions](#T48)
    - [k\_iprn.c](#T49)
      - [PRN VAR format](#T50)
      - [PRN CMT format](#T51)
      - [List of functions](#T52)
    - [k\_irasc.c](#T53)
      - [VAR format (sample is required)](#T54)
      - [List of functions](#T55)
    - [k\_itxt.c](#T56)
      - [Format](#T57)
      - [List of functions](#T58)

# IODE: Object conversions (Import / Export) {#T1}

IODE offers several functions to import and export objects from and to various formats. Some formats date back to the 80s and 90s and are now obsolete, but are still available in the software and therefore kept (in a simplified version) for sake of completeness.

### Export {#T2}

For each output format, an `EXPDEF` struct (mainly a table of function pointers) must be given as argument to one of the general functions `EXP_Ws()` or `EXP_Rev_Ws()` (found in k\_emain.c). These functions, specific to each output format, are coded in one of the files **k\_e\*.c** (for example **k\_ecvs.c** for csv output).

### Import {#T3}

The same logic applies to the import of object: for each import format, the general functions `IMP_InterpretVar()` and `IMP_InterpretCmt()` (defined in k\_emain.c) are to be fed with an `IMPDEF` struct (a table of function pointers).

These functions, related to one input format, are coded in the files **k\_i\*.c** (for ex **k\_iasc.c**).

### Rule files {#T4}

To limit the output or input to a group of objetcs and optionally rename the objects in the output/input file, a set of rules can be specified. The syntax and logic of the rule file is explained below.

#### Definition of "Rules" {#T5}

Rules are defined in an external file and allow essentially two things :

- limit the variables to be im/exported
- rename the objects (if the name in the ASCII file is not appropriate for IODE)

#### Syntax of "Rules" {#T6}

Each rule is defined on one line and composed of two fields:

- the selection mask (pattern) containing a description of the names concerned by the rule. This mask is defined with the same syntax as the search in IODE objects (see Data menu), i.e. by possibly including \* and ? to specify sets.
- the name transcoding algorithm for this rule, which may contain:
- \+ : indicating that the character at this position in the source name should be included as is in the output name
- \- : any character: indicates that the character at this position in the source name should be skipped
- any other character is kept as is in the resulting name

#### Example of rule file {#T7}

```
   
    B*  C+-+            => changes B1234 to CB2, BCDEF to CBE, etc 
    *X  ++++++++++      => keeps names ending in X unchanged
```

If the rule file is empty or undefined, the default rule is used:

```
   * ++++++++++
```

#### Order of execution {#T8}

Rules are executed in the order in which they are defined in the "rule file". As soon as a rule can be applied to a name, it is used. It is therefore important to position "exceptions" at the beginning of the file and to put general rules to the end.

### Source files {#T9}

- k\_rules.c : import and export rules management
- Export IODE objects
- Import IODE objects

## k\_rules.c {#T10}

Management of the rules used to select objects to import/export and to optionally adapt their names.

#### List of functions {#T11}

|Syntax|Description|
|:---|:---|
|   int IMP\_readrule(char\* filename)|Reads a "rule file" and stores its contents in 2 global variables IMP\_rule and IMP\_pat.|
|   int IMP\_change(char\*\* rule, char\*\* pat, char\* in, char\* out)|Modifies an object name according to the rule definitions.|

## Export IODE objects {#T12}

- k\_emain.c : main export module
- k\_ecsv.c : export functions specific to CSV format
- k\_edif.c : export functions specific to DIF format
- k\_etsp.c : export functions specific to TSP format
- k\_ewks.c : export functions specific to WKS format (obsolete)
- k\_wks.c : basic functions to create a WKS (Lotus 123) files (obsolete)

### k\_emain.c {#T13}

Main functions to export (partial) KDBs into external formats like csv, reverse csv...

The two main export functions EXP\_Ws() and EXP\_Rev\_W() call virtual functions implemented in the following source files specific to each format:

- k\_edif.c
- k\_ecsv.c
- k\_etsp.c
- k\_ewks.c

For each output format, a table of function pointers must be given as argument to EXP\_Ws() or EXP\_Rev\_Ws().

A set of rules can be defined to select and/or determine the name each object is given in the output file.

#### List of functions {#T14}

|Syntax|Description|
|:---|:---|
|`void EXP_val(char* tmp, double val)`|Formats a double value on 20 positions in general format.|
|`char *EXP_addprepost(char* pre, char* post, char* src, char** tg)`|Creates an allocated string formatted as \{pre\}\{src\}\{post\}.|
|`char *EXP_addsep(char* src, char** tg)`|Creates an allocated string formatted as \{src\}\{EXP\_SEP\}.|
|`int EXP_Ws(EXPDEF* expdef, KDB* dbv, KDB* dbc, char* rulefile, char* outfile, char* na, char* sep)`|Exports a KDB of VARs (and optionally of CMTs) in the format defined by the virtual functions in EXPDEF|
|`int EXP_Rev_Ws(EXPDEF* expdef, KDB* dbv, KDB* dbc, char* rulefile, char* outfile, char* na, char* sep)`|Same as EXP\_Ws() but the output is "rotated", i.e each column is a VAR and each line a period.|
|`int EXP_RuleExport(char* trace, char* rule, char* out, char* vfile, char* cfile, char* from, char* to, char* na, char* sep, int fmt)`|Exports VAR files into an external format.|

### k\_ecsv.c {#T15}

Implementation of the CSV and rotated CSV export functions. See k\_emain.c for more information.

#### Example of a CSV output file {#T16}

```
    code;comment;1990Y1;1991Y1;1992Y1;1993Y1;...
    ACAF; Ondernemingen: ontvangen kapitaaloverdrachten.; 23.771;26.240999;30.159;...
    ACAG; Totale overheid: netto ontvangen kapitaaloverdrachten.; -28.172186;-30.934;...
    .....
```

#### Example of a rotated CSV output file {#T17}

```
    ; ACAF; ACAG; AOUC; AOUC_; AQC; BENEF; BQY; ...
    1990Y1; 23.771; -28.172186; 1; 0.93735915; 1; ...
    1991Y1; 26.240999; -30.934; 1.0244334; 0.96466659; ...
    .....
```

#### List of functions {#T18}

|Syntax|Description|
|:---|:---|
|`int EXP_hd_csv(EXPDEF *expdef, KDB* dbv, KDB* dbc, char* outfile)`|Opens and initialise a CSV export file.|
|`int EXP_end_csv(EXPDEF* expdef, KDB* dbv, KDB* dbc)`|Saves the footer and closes the CSV export files.|
|`char *EXP_code_csv(char* name, char** code)`|Variable name translation for CSV output.|
|`char *EXP_cmt_csv(KDB* dbc, char* name, char**cmt)`|Creates the CMT text \+ separator for CSV output.|
|`char *EXP_elem_csv(KDB* dbv, int nb, int t, char** vec)`|Adds one element of a VAR (KDB\[nb\]\[t\]) to the export vector in CSV format.|
|`int EXP_vec_csv(EXPDEF* expdef, char* code, char* cmt, char* vec)`|Saves one VAR in the csv export file.|
|`int EXP_hd_rcsv(EXPDEF* expdef, KDB* dbv, KDB* dbc, char*outfile)`|Opens and initialise a rotated CSV export file.|
|`char *EXP_elem_rcsv(KDB* dbv, int nb, int t, char** vec)`|Adds one element of a VAR (KDB\[nb\]\[t\]) to the export vector in rotated CSV format.|
|`int EXP_vec_rcsv(EXPDEF* expdef, char* code, char* cmt, char* vec)`|Saves one VAR in the rotated csv export file.|
|`int EXP_RuleExport(char* trace, char* rule, char* out, char* vfile, char* cfile, char* from, char* to, char* na, char* sep, int fmt)`|Exports VAR files into an external format.|

### k\_edif.c {#T19}

Export functions specific to DIF format.

#### Example of a DIF output file {#T20}

```
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
```

#### List of functions {#T21}

|Syntax|Description|
|:---|:---|
|`int EXP_hd_dif(EXPDEF *expdef, KDB* dbv, KDB* dbc, char* outfile)`|Opens and initialise a DIF export file.|
|`int EXP_end_dif(EXPDEF* expdef, KDB* dbv, KDB* dbc)`|Saves the footer and closes the DIF export files.|
|`char *EXP_code_dif(char* name, char** code)`|Variable name translation for DIF output.|
|`char *EXP_cmt_dif(KDB* dbc, char* name, char**cmt)`|Creates the CMT text \+ separator for DIF output.|
|`char *EXP_elem_dif(KDB* dbv, int nb, int t, char** vec)`|Adds one element of a VAR (KDB\[nb\]\[t\]) to the export vector in DIF format.|
|`int EXP_vec_dif(EXPDEF* expdef, char* code, char* cmt, char* vec)`|Saves one VAR in the DIF export file.|

### k\_etsp.c {#T22}

Export functions specific to TSP format.

#### Example of a TSP output file {#T23}

```
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
```

#### List of functions {#T24}

|Syntax|Description|
|:---|:---|
|`int EXP_hd_tsp(EXPDEF *expdef, KDB* dbv, KDB* dbc, char* outfile)`|Opens and initialise a tsp export file.|
|`int EXP_end_tsp(EXPDEF* expdef, KDB* dbv, KDB* dbc)`|Saves the footer and closes the tsp export files.|
|`char *EXP_code_tsp(char* name, char** code)`|Variable name translation for tsp output.|
|`char *EXP_cmt_tsp(KDB* dbc, char* name, char**cmt)`|Creates the CMT text \+ separator for tsp output.|
|`char *EXP_elem_tsp(KDB* dbv, int nb, int t, char** vec)`|Adds one element of a VAR (KDB\[nb\]\[t\]) to the export vector in tsp format.|
|`int EXP_vec_tsp(EXPDEF* expdef, char* code, char* cmt, char* vec)`|Saves one VAR in the tsp export file.|

### k\_ewks.c {#T25}

Export functions specific to WKS format (obsolete).

#### List of functions {#T26}

|Syntax|Description|
|:---|:---|
|`int EXP_hd_wks(EXPDEF *expdef, KDB* dbv, KDB* dbc, char* outfile)`|Opens and initialise a wks export file.|
|`int EXP_end_wks(EXPDEF* expdef, KDB* dbv, KDB* dbc)`|Saves the footer and closes the wks export files.|
|`char *EXP_code_wks(char* name, char** code)`|Variable name translation for wks output.|
|`char *EXP_cmt_wks(KDB* dbc, char* name, char**cmt)`|Creates the CMT text \+ separator for wks output.|
|`char *EXP_elem_wks(KDB* dbv, int nb, int t, char** vec)`|Adds one element of a VAR (KDB\[nb\]\[t\]) to the export vector in wks format.|
|`int EXP_vec_wks(EXPDEF* expdef, char* code, char* cmt, char* vec)`|Saves one VAR in the wks export file.|

### k\_wks.c {#T27}

Basic functions to create a WKS (Lotus 123) files (obsolete)

#### List of functions {#T28}

|Syntax|Description|
|:---|:---|
|`int wks_init(char* file, int c, int r)`||
|`int wks_cwidth(int c, int w)`||
|`int wks_end()`||
|`void wks_string(char* label, int c, int r)`||
|`void wks_value(double value, int c, int r)`||
|`void wks_name(char* str, int c1, int r1, int c2, int r2)`||

## Import IODE objects {#T29}

- k\_imain.c : main import module
- k\_iasc.c : import from IODE ASCII format
- k\_idif.c : import from DIF format
- k\_ibst.c : import from BISTEL format (obsolete)
- k\_igem.c : import from GEM (Chronos) format (obsolete)
- k\_inis.c : import from INS format (obsolete)
- k\_iprn.c : import from PRN (Aremos) format (obsolete)
- k\_irasc.c : import from reverse ASCII format
- k\_itxt.c : import from Belgostat TXT format (obsolete)

### k\_imain.c {#T30}

Functions to import variables and comments from files in various external formats.

For each input format, a structure IMPDEF, which is essentially a table of function pointers, must be given as argument to one of the general functions IMP\_InterpretVar() or IMP\_InterpretCmt().

#### Available input formats {#T31}

- in k\_iasc.c: IODE ASCII for comments and variables
- in k\_irasc.c: rotated ASCII variables
- in k\_ibst.c: Bistel format (obsolete)
- in k\_idif.c: DIF format (Data Interchange Format)
- in k\_igem.c: Chronos format (obsolete)
- in k\_inis.c: INS format (obsolete)
- in k\_iprn.c: Aremos prn format (obsolete)
- in k\_itxt.c: Belgostat format (Obsolete)

#### Rule file {#T32}

A set of rules can also be defined to select and/or determine (transform) the name of each read object. See k\_emain.c for details on rule files.

#### List of functions {#T33}

|Syntax|Description|
|:---|:---|
|`KDB *IMP_InterpretVar(IMPDEF* impdef, char* rulefile, char* vecfile, SAMPLE* smpl)`|Interprets a text file containing VAR definitions|
|`KDB *IMP_InterpretCmt(IMPDEF* impdef, char* rulefile, char* cfile, int lang)`|Interprets an ASCII file containing COMMENTS definitions|
|`int IMP_RuleImport(int type, char* trace, char* rule, char* ode, char* asc, char* from, char* to, int fmt, int lang)`|Imports variables or comments in various formats.|

### k\_iasc.c {#T34}

Functions to import variables and comments in ASCII format.

#### VAR format (sample is required) {#T35}

```
   sample 1980Y1 1990Y1
   A 1 2 3 na na 4 5
   B 0.1 0.2 -3.14
   .....
```

#### CMT format (sample is required) {#T36}

```
    A "Description of A" 
    B "..."
    ...
```

#### List of functions {#T37}

|Syntax|Description|
|:---|:---|
|`int IMP_hd_asc(YYFILE* yy, SAMPLE* smpl)`|Reads the sample (required) in an ASCII variable file.|
|`int IMP_vec_asc(YYFILE* yy, char* name, int dim, double* vector)`|Reads a VAR name and values in an ASCII variable file format.|
|`int IMP_hd_casc(IMPDEF* impdef, char* file, int lang)`|Opens an ASCII comment file for reading with the YY library functions.|
|`int IMP_vec_casc(char* name, char** cmt)`|Reads one comment on the open YY stream.|

### k\_idif.c {#T38}

Functions to import variables from a DIF formatted ASCII file (obsolete).

See also k\_edif.c for an example of DIF format.

#### DIF format viewed in Excel {#T39}

```
   CODE 1980Y 1981Y1 1982Y1 1983Y1 ....
   A    1     1       2      4
   B    1     1       2      5
```

#### List of functions {#T40}

|Syntax|Description|
|:---|:---|
|`int DIF_skip_to(YYFILE* yy, int skey)`|Moves forward to the keyword skey or EOF or EOD.|
|`int DIF_cell(YYFILE* yy, char** str, double* value)`|Reads the next cell which can contain a real or a string.|
|`int IMP_hd_dif(YYFILE* yy, SAMPLE* smpl)`|Reads the header of a DIF file and determines the sample of the content.|
|`int IMP_vec_dif(YYFILE* yy, char* name, int dim, double* vector)`|Reads a VAR name and values in a DIF file.|
|`int IMP_end_dif()`|Cleanup the DIF global variables.|

### k\_ibst.c {#T41}

Functions to import variables and comments from the legacy Bistel format (obsolete).

When loaded in 123, the format generated by the Belgostat program is as follows:

```
   123456789012345     1       1980      1             456900     1       1
   (series nb)       (freq)    (year)    (per in year) (value)  (status)  (free)
   ........
```

#### List of functions {#T42}

See k\_idif.c for a similar group of functions.

### k\_igem.c {#T43}

Functions to import variables from the legacy GEM (Chronos) format (obsolete).

#### GEM VAR format {#T44}

```
   nobs nser rub .....
   subrub[nser1][nobs1] subrub[nser2][nobs1] ...
   ....
   subrub[nser1][nobs2] subrub[nser1][nobs2] ...
```

#### List of functions {#T45}

See k\_idif.c for a similar group of functions.

### k\_inis.c {#T46}

Functions to import variables from the legacy INS\-NIS format (obsolete).

#### NIS VAR format {#T47}

```
    0000 XXX 91 1    one   two   three  for   five  six
    0-3  6-8 11-15   16-25 27-36 38-47 49-58 60-61 71-80
```

#### List of functions {#T48}

See k\_idif.c for a similar group of functions.

### k\_iprn.c {#T49}

Functions to import variables from the legacy PRN (Aremos) format (obsolete).

#### PRN VAR format {#T50}

```
        "Name" 1960 1961 ... 1990
        "A" 1 2 3 ... 4 5
        "B" 0.1 0.2 ... -3.14
    ...
```

#### PRN CMT format {#T51}

```
        "A" "Comment of A"
        "B" "Comment of B"
    ...
```

#### List of functions {#T52}

See k\_idif.c for a similar group of functions.

### k\_irasc.c {#T53}

Functions to import variables in rotated ASCII format.

#### VAR format (sample is required) {#T54}

```
    sample 1980Y1 1990Y1
    A B C
    1 2 3
    4.2 na 5.4
    na na 4 
    .....
```

#### List of functions {#T55}

|Syntax|Description|
|:---|:---|
|`int IMP_hd_rasc(YYFILE* yy, SAMPLE* smpl)`|Reads the sample (required) and the list of VARs in a rotated ASCII variable file.|
|`int IMP_elem_rasc(YYFILE* yy, char* name, int* shift, double* value)`|Reads one value in an ASCII variable file format.|
|`int IMP_end_rasc()`|Frees the allocated vars during the rotated ASCII file import session.|

### k\_itxt.c {#T56}

Functions to import variables and comments from the legacy Belgostat format (obsolete).

#### Format {#T57}

```
     N;0104104000023;   1;19801231;               8,3;D
     D;code;freq;date;value;status
  
     0104104000023;N;Werkloosheidsgraden (bron: Eurostat), Seizoengezuiverde gegevens;Taux de chomage (source: Eurostat), Donnees desaissonnalisees;Taux de chomage (source: Eurostat), Donnees desaissonnalisees; ;0,010;
     code;N;cmtnl;cmtfr;cmten;units1;units2
```

#### List of functions {#T58}

See k\_idif.c for a similar group of functions.
