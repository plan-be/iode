
<IODE: functions by group>
IODE: functions by group
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..sep |
..esc ~
..ignore ¯


&EN <Function names in IODE>
&EN <Memory allocations in IODE>
&EN <SCR4 library>
&EN <Group "Global Utilities"            >
&EN <Group "Pseudo-virtual functions"    >
&EN <Group "IODE Version"                >
&EN <Group "KDB management"              >
&EN <Group "Object management"           >
&EN <Group "IODE file management"        >
&EN <Group "IODE big- and little-endian conversion">
&EN <Group "Iode object ascii formats"   >
&EN <Group "LEC language"                >
&EN <Group "Basic functions"             >
&EN <Group "Execution of identities"     >
&EN <Group "Model Estimation"            >
&EN <Group "Model Simulation"            >
&EN <Group "Table Calculation"           >
&EN <Group "Iode Printing"               >
&EN <Group "IODE object conversions"     >
&EN <Group "Iode Reports"                >
&EN <Group "Iode high-level API"         >
&EN <Group "DDE communcations"           >
>

<Function names in IODE>
Function names in IODE
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
As C does not allow "namespacing", we always give prefixes to function names, the prefix giving
an indication of the group the function belongs to. For example, P*() is the group of 
function for "packing" (aka serializing) IODE objects, PER*() for manipulating 
PERIOD, L_* () for LEC compilation / execution, etc.

Most filenames follow the same principle: each group of files has a specific prefix which normally gives 
an indication of the group they belong to.

&EN k_*: core functions (the prefix "k_" is a legacy of the "Kaa" software)
&EN b_*: building *B*locks for IODE reports
&EN ... to be continued...
>

<SCR4 library>
SCR4 library
¯¯¯¯¯¯¯¯¯¯¯¯
Most functions in IODE massively use the SCR4 library functions which extends the standard "libc" libray 
for numerous topics: 
&EN memory management
&EN list management
&EN matrix calculus
&EN ini file management
&EN html, rtf, md, csv file generation
&EN printing management
&EN odbc interface
&EN etc

The web page http://www.xon.be/scr4/libs1/libs1.htm contains a list of all available functions.

>

<Memory allocations in IODE>
Memory allocations in IODE
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
IODE uses 2 distinct groups of functions for memory allocations. 

The ~bfirst group~B is based on the "standard" memory allocation functions ~cmalloc()~C and ~cfree()~C. 

The main functions in this group are ~cSW_nalloc()~C, ~cSW_nrealloc()~C and ~cSW_nfree()~C. 
An older group from ~bscr4~B has the same properties: ~cSCR_malloc()~C, ~cSCR_realloc()~C and ~cSCR_free()~C.

The ~bsecond group~B has been created specifically to avoid memory segmentation when possible. 
The main idea is to allocate large memory buffers 
on the heap (called "segments") and to fill them with serialized objects, called "packed" objects, 
like "packed" equations or variables. 
Each allocated object is identified by a "handle" and, when it is 
reclaimed by the program, a trivial indirection translates its handle to the real pointer 
which is simply a shift from the beginning of its segment.

The main functions in this second group are ~cSW_init()~C, ~cSW_alloc()~C, ~cSW_realloc()~C, ~cSW_free()~C, ~cSW_getptr()~C and ~cSW_end~C.

More information can be found at http://xon.be/scr4/libs1/libs1236.htm. 
>

<Group "Pseudo-virtual functions">
Group "Pseudo-virtual functions"
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
Some IODE functions may differ according to the context. For example, in the context of a console app, error messages will be 
printed in the console. In the DOS-Win32 context, error messages are displayed in the status bar, in the Qt context in a different way...

For each of these functions, a standard implementation (= virtual function) is provided in ~ciode_c_api.lib~C, generally using the stdio functions like ~cprintf~C. 
These functions are grouped in k_super.c.

At the same time, an optional "super function" pointer may point to an alternative function. If that pointer is not null, 
the alternative function replaces (superseeds) the original implementation (= virtual function). In that way, each context has possibly its own implementation
without interfering with the others.

&TI k_super.c
¯¯¯¯¯¯¯¯¯¯¯¯¯¯
&IT List of functions that can be superseeded
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
Functions used in any context of IODE (GUI or not-GUI).

..tb
| Syntax                                        | Description  
| ~cvoid kerror(int level, char* fmt, ...)      | Displays an error message and optionally exits the program.
| ~cvoid kpause()                               | Displays the message "Press ENTER to continue" and waits for the user to press ENTER.
| ~cvoid kwarning(char* fmt, ...)               | Displays a message and optionally asks the user to press ENTER before continuing.
| ~cvoid kmsg(char* fmt, ...)                   | Displays a message.
| ~cvoid kmsg_toggle(int IsOn)                  | Suppresses or restores the message output by replacing kmsg_super()
| ~cint kconfirm(char *fmt,...)                 | Displays a message and optionally asks confirmation before continuing.
| ~cint kmsgbox(unsigned char *str, unsigned char *v, unsigned char **buts) | Displays a message box with optional buttons. 
| ~cvoid krecordkey(int key)                    | Records a key in the keyboard buffer. 
| ~cint Wprintf(char* fmt, ...)                 | Displays a message.
| ~cint SCR_panic()                             | Exits the program (normally on a "memory full" event).
..te

Functions used only in a GUI context.

..tb
| Syntax                                        | Description  
| ~cvoid ksettitle()                            | Set the window title (GUI only).
| ~cint ktermvkey(int vkey)                     | Defines the interval to wait between two checks on the keyboard buffer length (GUI only).
| ~cint khitkey()                               | Checks whether the keyboard buffer is not empty (GUI only).
| ~cint kgetkey()                               | Reads the next character in the keyboard buffer (GUI only).
| ~cvoid kbeep()                                | Plays a sound (GUI only).
| ~cSAMPLE *kasksmpl()                          | Asks the user to give a SAMPLE (GUI only).
| ~cint kexecsystem()                           | Calls the fonction system().
| ~cint kshellexec()                            | Call the Win32 function ShellExecuteEx().

..te

&IT List of function pointers that can replace the standard implementations  
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
| Syntax                                      
| ~cvoid (*kerror_super)(int level, char*msg);
| ~cvoid (*kpause_super)();
| ~cint   kpause_continue = 0;
| ~cvoid (*kwarning_super)(char* msg);
| ~cvoid (*kmsg_super)(char* msg);
| ~cint  (*kwprintf_super)(char* msg);
| ~cvoid (*kpanic_super)(void);
| ~cint  (*kconfirm_super)(char* msg);
| ~cint  (*kmsgbox_super)(unsigned char *str, unsigned char *v, unsigned char **buts);
| ~cint   kmsgbox_continue = 0;
| ~cvoid (*krecordkey_super)(int ch);
| ~cvoid (*ksettitle_super)(void);
| ~cint  (*ktermvkey_super)(int vkey);
| ~cint  (*khitkey_super)();
| ~cint  (*kgetkey_super)();
| ~cvoid (*kbeep_super)(void);
| ~cSAMPLE *(*kasksmpl_super)(void);
| `cint kexecsystem_super(char*);
| `cint kshellexec_super(char*);    
..te
>

<Group "Global Utilities">
Group "Global Utilities"
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
| Filename    | Object
| buf.c       | shares a large allocated buffer in different parts of the application
| pack.c      | packs (serialize) and unpacks (deserialize) objects.
| per.c       | functions for manipulating PERIOD and SAMPLE in IODE.
| yy.c        | helper functions for reading and writing IODE ascii files.
| b_iodeini.c | reading and writing parameters in the iode.ini file
..te

&TI buf.c
¯¯¯¯¯¯¯¯¯
Functions to share and reuse a large allocated buffer in different parts of the application. 

..tb
| Syntax                               | Description  
| ~cchar *BUF_alloc(int len)             | allocates or extends a global buffer of at least len bytes. 
| ~cvoid BUF_free()                      | frees the buffer
| ~cvoid BUF_lock()                      | reserves the buffer utilisation
| ~cvoid BUF_unlock()                    | unlocks the buffer
| ~cchar *BUF_memcpy(char *ptr, int lg)  | copies the the first lg bytes following address ptr to the buffer BUF_DATA
| ~cchar *BUF_strcpy(char *ptr)          | copies a null terminated string to the buffer
|
| ~cchar *BUF_DATA                       | NULL or pointer to the allocated buffer
..te

&TI pack.c
¯¯¯¯¯¯¯¯¯¯¯¯
Packing (serialize) and unpacking (deserialize) objects. 

..tb
| Syntax                                        |Description  
| ~cvoid *P_create()                              | creates (allocated) a new pack object
| ~cint P_free(char *ptr)                         | frees a pack object
| ~cvoid *P_add(void *vptr1, void *vptr2, int lg) | appends to the pack pointed to by vptr1 the content pointed to by vptr2 which has a length of lg bytes. 
| ~cvoid *P_get_ptr(void *vptr, int i)            | retrieves the pointer to the i'th element of a pack.
| ~cOSIZE P_get_len(void *vptr, int i)            | retrieves the length (in bytes) of the i'th element of a pack or the length of the full pack if i << 0.
| ~cvoid *P_alloc_get_ptr(void *ptr, int p)       | allocates space for the p'th element of a pack. Copies the pth element in the allocated space.
| ~cint P_nb(char *ptr)                           | retrieves the number of elements in the pack pointed to by ptr.
..te

&TI per.c
¯¯¯¯¯¯¯¯¯¯¯¯
Functions for manipulating PERIOD and SAMPLE in IODE.
  
&IT Calculation functions
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
| Syntax                                                        | Description  
| ~cint PER_diff_per(PERIOD *p1, PERIOD *p2)                    | calculates the number of sub periods between two periods. 
| ~cPERIOD *PER_addper(PERIOD *period, int shift)               | adds (sub-)periods to period.
| ~cint PER_common_smpl(SAMPLE* smp1, SAMPLE* smp2, SAMPLE* res)| calculates the intersection between 2 SAMPLEs.
..te 

&IT Conversion functions
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
| Syntax                                         | Description  
| ~cchar *PER_pertoa(per, text)                  | creates a text representation of a PERIOD.
| ~cPERIOD *PER_atoper(char *text)               | translates a text in a PERIOD.
| ~cchar *PER_smpltoa(SAMPLE* smpl, char* text)  | writes a SAMPLE in a string.
| ~cSAMPLE *PER_atosmpl(char* a1, char* a2)      | creates a SAMPLE based on two strings containing PERIODs.
| ~cSAMPLE *PER_pertosmpl(PERIOD* p1, PERIOD* p2)| returns a new allocated SAMPLE build on two given PERIOD.
..te

&IT Miscellaneous functions 
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
| Syntax                                        | Description  
| ~cint PER_nb(int ch)                          | gives the nbr of periods in one year for the periodicity ch
| ~cint PER_nbper(PERIOD* period)               | retrieves the number of periods in one year in a period
| ~cdouble PER_per2real(PERIOD* from, int i) | adds a number of sub-periods to a PERIOD and returns a         numerical representation of the result (used mainly for tables and graphs formatting).
..te

&TI yy.c
¯¯¯¯¯¯¯¯¯
Helper functions for reading and writing IODE ascii files.

..tb
| Syntax                                          | Description  
| ~cdouble K_read_real(YYFILE *yy)             | reads a token on the YY stream and interprets the token as a double (double) if possible.
| ~clong K_read_long(YYFILE* yy)                  | reads the next token on the YY stream and returns a long. 
| ~cchar* K_read_str(YYFILE* yy)                  | reads the next token on the YY stream. If it is a string, returns an allocated copy of the string.  
| ~cPERIOD *K_read_per(YYFILE* yy)                | reads the next tokens on the YY stream and tries to interpret them as a PERIOD definition ({long}{char}{long}).
| ~cSAMPLE *K_read_smpl(YYFILE* yy)               | reads the next tokens on the YY stream and tries to interpret them as a SAMPLE.
| ~cint K_read_align(YYFILE* yy)                  | reads the next token on the YY stream: LEFT, RIGHT or CENTER.
|                                            
| ~cvoid K_stracpy(char** to, char* from)         | allocates and copies a null terminated string. 
| ~cint K_wrdef(FILE* fd, YYKEYS* table, int def) | searches the position of a integer in a table of YYKEYS and writes the corresponding token onto fd. 
| ~cint K_compare(YYKEYS* yy1, YYKEYS* yy2)       | helper function passed as parameter to qsort to sort a table of YYKEYS.
| ~cchar *K_wrap(char *in, int lg)                | wraps a string (by inserting \n) to limit each line to lg characters.
..te

&TI b_iodeini.c
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
Functions to read and write parameters in iode.ini. 

..tb
| Syntax                                                                           | Description  
| ~cvoid B_IodeIniFile()                                                           | Retrieves the path to the iode.ini file           
| ~cint B_IniReadText(char* section, char* parm, char* res, int maxlen, char* dft) | Reads a text parameter in the current iode.ini file.
| ~cint B_IniReadChar(char* section, char* parm, char dft)                         | Reads a character parameter in the current iode.ini file.
| ~cint B_IniReadNum(char* section, char* parm, int dft)                           | Reads a integer parameter in the current iode.ini file. 
| ~cint B_IniReadYN(char* section, char* parm, int dft)                            | Reads a Y/N parameter in the current iode.ini file. 
| ~cint B_IniWriteText(char* section, char* parm, char* val)                       | Saves a text parameter in the current iode.ini file
| ~cint B_IniWriteChar(char* section, char* parm, char val)                        | Saves a char parameter in the current iode.ini file
| ~cint B_IniWriteNum(char* section, char* parm, int val)                          | Saves a integer parameter in the current iode.ini file
| ~cint B_IniWriteYN(char* section, char* parm, int val)                           | Saves a boolean parameter in the current iode.ini file    
..te
>

<Group "IODE Version">
Group "IODE Version"
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
&TI k_vers.c
¯¯¯¯¯¯¯¯¯¯¯¯
Functions to retrieve the current IODE version.

..tb
| Syntax                  | Description  
| ~cchar *K_LastVersion()   | returns in an allocated string the current version of IODE. 
| ~cchar *K_CurrentVersion()| returns in an allocated string the IODE version of the current executable.
..te
>

<Group "KDB management">
Group "KDB management"
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
&TI k_kdb.c
¯¯¯¯¯¯¯¯¯¯¯¯
Function to manage KDB, i.e. IODE object groups.
..tb
| Syntax                                                 | Description  
| ~cKDB *K_init_kdb(int ,char *)                         | allocates and initialises a KDB struct
| ~c*char *K_get_kdb_nameptr(KDB *kdb)                   | gets the KDB filename pointer stored in ~Ckdb->>k_nameptr~C
| ~cvoid K_set_kdb_name(KDB *kdb, U_ch *filename);       | changes the filename in a KDB
| ~cKDB *K_create(int type, int mode)                    | allocates and initialises a KDB object.    
| ~cint K_free_kdb(KDB* kdb)                             | frees a KDB but leaves its contents untouched.
| ~cint K_free(KDB* kdb)                                 | frees a KDB and its contents.
| ~cint K_clear(KDB* kdb)                                | deletes all objects in a KDB, reset the SAMPLE and replaces the filename by I_DEFAULT_FILENAME (default "ws"). 
| ~cKDB *K_refer(KDB* kdb, int nb, char* names[])        | creates a new kdb containing the references to the objects of the list names.
| ~cKDB *K_quick_refer(KDB *kdb, char *names[])          | same as K_refer() but more efficient for large databases.
| ~cint K_merge(KDB* kdb1, KDB* kdb2, int replace)       | merges two databases : kdb1 <<- kdb1 + kdb2. 
| ~cint K_merge_del(KDB* kdb1, KDB* kdb2, int replace)   | merges two databases : kdb1 <<- kdb1 + kdb2 then deletes kdb2. 
..te

&TI  k_ws.c
¯¯¯¯¯¯¯¯¯¯¯¯
Variables and functions for initializing and cleaning up the "in memory" workspaces.
..tb
| Syntax                                    | Description  
| ~cKDB *K_WS[7]                            | Table with pointers to the 7 KDB in memory, 1 per object type (CEILSTV)
| ~cvoid K_init_ws(int ws)                  | Initialises the "in mem" KDB structures and optionaly loads the ws.* files
| ~cvoid K_end_ws(int ws)                   | Deletes the current workspaces defined in K_WS[] and their content after having optionaly saved their content in ws.* files.
| ~cint K_load_RWS(int ref, char *filename) | Load a VAR file for use in GSAMPLE (print tables and graphs)
..te

&TI k_wsvar.c
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
Functions acting on workspaces of variables.
..tb
| Syntax                                                                  | Description  
| ~cint KV_sample(KDB *kdb, SAMPLE *nsmpl)                                | Changes the SAMPLE of a KDB of variables.
| ~cint KV_merge(KDB *kdb1, KDB* kdb2, int replace)                       | Merges two KDB of variables: kdb1 <<- kdb1 + kdb2.            
| ~cvoid KV_merge_del(KDB *kdb1, KDB *kdb2, int replace)                  | Merges 2 KDB of variables, then deletes the second one.
| ~cint KV_add(char* varname)                                             | Adds a new variable in KV_WS. Fills it with IODE_NAN.
| ~cdouble KV_get(KDB *kdb, int pos, int t, int mode)                     | Gets VAR[t]  where VAR is the series in position pos in kdb. 
| ~cvoid KV_set(KDB *kdb, int pos, int t, int mode, double new)        | Sets VAR[t], where VAR is the series in position pos in kdb. 
| ~cint KV_extrapolate(KDB *dbv, int method, SAMPLE *smpl, char **vars)   | Extrapolates variables on a selected SAMPLE according to one of the available methods.
| ~cKDB *KV_aggregate(KDB *dbv, int method, char *pattern, char *filename)| Creates a new KDB with variables created by aggregation based on variable names._
| ~cvoid KV_init_values_1(double* val, int t, int method)              | Extrapolates 1 value val[t] based on val[t], val[t-1] and a selected method.
| ~cdouble KV_get_at_t(char*varname, int t)                               | Retrieves the value of varname[t] 
| ~cdouble KV_get_at_per(char*varname, PERIOD* per)                       | Retrieves the value of varname[per] 
| ~cdouble KV_get_at_aper(char*varname, char* aper)                       | Retrieves the value of varname[aper]
| ~cint KV_set_at_t(char*varname, int t, double val)                      | Replaces the value of varname[t] by val.
| ~cint KV_set_at_per(char*varname, PERIOD* per, double val)              | Replaces the value of varname[per] by val.
| ~cint KV_set_at_aper(char*varname, char* aper, double val)              | Replaces the value of varname[aper] by val.
| ~cint KV_per_pos(PERIOD* per2)                                          | Retrieves the position of a PERIOD in the current KV_WS sample.
| ~cint KV_aper_pos(char* aper2)                                          | Retrieves the position of a period in text format in the current KV_WS sample.  
..te
>

<Group "Object management">
Group "Object management"
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
&EN k_objsv.c: function to create an IODE object of any type.
&EN k_objs.c: functions to manipulate IODE objects.
&EN k_objvers.c: functions to detect IODE object file version and to convert an object to the current IODE version._
&EN k_pack.c: functions for "packing" and "unpacking" IODE objects.
&EN k_val.c: functions to retrieve object data based on their position or name in the kdb.
&EN k_eqs.c: functions to manipulate equation expressions and objects.
&EN k_lst.c: functions to manipulate and create lists.
&EN k_tbl.c: functions to manage TBL objects.
&EN k_cmp.c: function to compare two IODE objects.
&EN k_grep.c: functions to search strings in KDB objects.

&TI k_objsv.c
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
Function to create an IODE object and to record it in a KDB.

..tb
| Syntax                                                                  | Description  
| ~cint K_add(KDB* kdb, char* name, char* a1, char* a2, char* a3, char* a4, char* a5, char* a6, char* a7, char* a8, char* a9)| adds an object to a KDB. The number of arguments depends on object type.
..te

How to create IODE objects using K_add():

..tb
| Object type | Syntax 
| Comments    | K_add(KDB* kdb, char* name, CMT cmt)
| Equations   | K_add(KDB* kdb, char* name, EQ* eq, char* endo) [where endo = name]
| Identities  | K_add(KDB* kdb, char* name, char* lec)
| Lists       | K_add(KDB* kdb, char* name, char* list)
| Scalars     | K_add(KDB* kdb, char* name, SCL* scalar)
| Tables      | K_add(KDB* kdb, char* name, TBL *tbl) 
| Variables   | K_add(KDB* kdb, char* name, double* var, int nb_obs) [nb_obs = kdb SAMPLE size]
..te

Note: the name of an equation MUST be the name of its endogenous variable.

&TI k_objs.c
¯¯¯¯¯¯¯¯¯¯¯¯
Functions to manipulate IODE objects.
..tb
| Syntax                                                         | Description  
| ~cint K_key(char* name, int mode):                             | Checks the validity of an object name and modify its "case" according to the value of mode.
| ~cint K_dup(KDB* kdb1, char* name1, KDB* kdb2, char* name2):   | Duplicates an IODE object. Copies name1 of kdb1 to name2 in kdb2.
| ~cint K_ren(KDB* kdb, char* name1, char* name2):               | Renames the object name1 in kdb into name2.
| ~cint K_add_entry(KDB* kdb, char* name):                       | Adds the entry name in kdb and returns its position in the kdb. 
| ~cint K_find(KDB* kdb, char* name):                            | Searches the position of an object name in a KDB.
| ~cint K_del_entry(KDB* kdb, int pos):                          | Deletes an entry in a KDB __without__ deleting the referenced object. 
| ~cint K_del(KDB* kdb, int pos):                                | Deletes an object (and its data) from a KDB.
| ~cint K_del_by_name(KDB* kdb, char* name):                     | Deletes an object identified by its name from a KDB. 
| ~cint K_upd_eqs(char* name, char* lec, char* cmt, int method, SAMPLE* smpl, char* instr, char* blk, float* tests, int date) | Updates equation field(s). Creates the equation if it doesn't exist.
| ~cint K_upd_tbl(char* name, char* arg)                         | Creates a basic table with an optional TITLE and optional variable names and/or lec formulas separated by semi-colons.
..te

&TI k_objvers.c
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
Functions to detect IODE object file version and to convert an object to the current IODE version._

..tb
| Syntax                                      | Description  
| ~cint K_calcvers(char* label):              | returns the current object version (0-2) from an IODE file header. 
| ~cvoid K_setvers(KDB* kdb, int i, int vers) | converts an IODE object from IODE objects version 1 or 2 to the current version (0).
..te

&TI  k_pack.c
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
Functions for "packing" and "unpacking" IODE objects.

&IT Packing functions  
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
| Syntax                                               | Description  
| ~cint K_cpack(char **pack, char *a1)                 | Packs an IODE CMT object 
| ~cint K_epack(char **pack, char *a1, char *a2)       | Packs an IODE EQ object 
| ~cint K_ipack(char **pack, char *a1)                 | Packs an IODE IDT object 
| ~cint K_lpack(char** pack, char* a1)                 | Packs an IODE LST object 
| ~cint K_spack(char **pack, char *a1)                 | Packs an IODE SCL object 
| ~cint K_tpack(char** pack, char* a1)                 | Packs an IODE TBL object 
| ~cint K_vpack(char **pack, double *a1, int *a2)   | Packs an IODE VAR object. 
| ~cint K_opack(char** pack, char* a1, int* a2)        | Reserved for future new objects
..te

&IT Unpacking functions (for TBL and EQ only ?)
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
| Syntax | Description  
| ~cTBL* K_tunpack(char *pack) |  Creates a TBL struct from a packed TBL
| ~cEQ* K_eunpack(char *pack)  |  Creates an EQ struct from a packed EQ
| ~cIDT* K_iunpack(char *pack) |  Creates an IDT struct from a packed IDT
..te
s
&IT Allocation functions (SCL & VAR only)
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯

..tb
| Syntax| Description           
| ~cint KS_alloc_scl()          | Allocates space for a new SCL (0.9, 1.0, NaN) in the the "swap area". Returns the "swap" handle.
| ~cint KV_alloc_var(int nb)    | Allocates space for a new VAR of length nb in the swap area, initialises it to IODE_NAN and returns the "swap" handle.
..te

&TI k_val.c
¯¯¯¯¯¯¯¯¯¯¯
Basic functions to retrieve object data based on their position or name in the kdb.
If the object is packed (EQ, TBL...) the position (n) of the element in the pack must be given.

List of functions 
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
| Syntax                                            | Meaning  
| ~cchar *K_oval(KDB* kdb, int pos, int n)          | ~ kdb[pos][n]
| ~cchar* K_optr(KDB *kdb, char* name, int n)       | ~ kdb[name][n]
| ~cchar *K_oval0(KDB* kdb, int pos)                | ~ kdb[pos][0]
| ~cchar* K_optr0(KDB *kdb, char* name)             | ~ kdb[name][0]
| ~cchar *K_oval1(KDB* kdb, int pos)                | ~ kdb[pos][1]
| ~cchar* K_optr1(KDB *kdb, char* name)             | ~ kdb[name][1]
| ~cdouble *K_vval(KDB* kdb, int pos, int t)     | ~ kdb[pos][t]
| ~cdouble *K_vptr(KDB* kdb, char* name, int t)  | ~ kdb[name][t]
| ~cEQ* K_eptr(KDB* kdb, char* name)                | ~ kdb[name]
| ~cTBL* K_tptr(KDB* kdb, char* name)               | ~ kdb[name]
| ~bEquation tests          
| ~cdouble K_etest(KDB* kdb, char*name, int test_nb)| Retrieves a statistical test stored the equation whose endo is name.
| ~cdouble K_e_stdev (KDB* kdb, char*name)          | Returns stdev calculated during the last estimation of equation name
| ~cdouble K_e_meany (KDB* kdb, char*name)          | Returns meany calculated during the last estimation of equation name       
| ~cdouble K_e_ssres (KDB* kdb, char*name)          | Returns ssres calculated during the last estimation of equation name
| ~cdouble K_e_stderr(KDB* kdb, char*name)          | Returns stderr calculated during the last estimation of equation name
| ~cdouble K_e_fstat (KDB* kdb, char*name)          | Returns fstat calculated during the last estimation of equation name
| ~cdouble K_e_r2    (KDB* kdb, char*name)          | Returns r2    calculated during the last estimation of equation name
| ~cdouble K_e_r2adj (KDB* kdb, char*name)          | Returns r2adj calculated during the last estimation of equation name
| ~cdouble K_e_dw    (KDB* kdb, char*name)          | Returns dw    calculated during the last estimation of equation name
| ~cdouble K_e_loglik(KDB* kdb, char*name)          | Returns loglik calculated during the last estimation of equation name
| ~bScalar data
| ~cdouble K_s_get_info(KDB* kdb, char*name, int info_nb)                | Retrieves a SCL info
| ~cdouble K_s_get_value (KDB* kdb, char*name)                           | Retrieves a SCL value
| ~cdouble K_s_get_relax (KDB* kdb, char*name)                           | Retrieves a SCL relax
| ~cdouble K_s_get_stderr(KDB* kdb, char*name)                           | Retrieves a SCL stderr
| ~cdouble K_s_get_ttest (KDB* kdb, char*name)                           | Retrieves a SCL ttest
| ~cdouble K_s_set_info(KDB* kdb, char*name, int info_nb, double val)    | Sets a SCL info
| ~cdouble K_s_set_value (KDB* kdb, char*name)                           | Sets a SCL value
| ~cdouble K_s_set_relax (KDB* kdb, char*name)                           | Sets a SCL relax
| ~cdouble K_s_set_stderr(KDB* kdb, char*name)                           | Sets a SCL stderr
..te


&TI k_eqs.c
¯¯¯¯¯¯¯¯¯¯¯¯
Functions to manipulate equation expressions and objects.

..tb
| ~cvoid E_free(EQ* eq)                                                   | Frees an EQ struct and all its elements
| ~cint E_split_eq(char* lec, char** lhs, char** rhs)                     | Extracts the left and right sides of a lec equation
| ~cint E_dynadj(int method, char* lec, char* c1, char* c2, char** adjlec)| Transforms a LEC equation to add dynamic adjustment
| ~cE_DynamicAdjustment(int method, char** eqs, char*c1, char*c2)         | Transforms a LEC equation "in place" to add dynamic adjustment
..te


&TI k_lst.c
¯¯¯¯¯¯¯¯¯¯¯¯
Basic functions to manipulate lists and to extract lists of VARs and SCLs from IODE objects.

..tb
| ~cSyntax                                             | Description
| ~cint K_scan(KDB* kdb, char* l_var, char* l_scal)    | Analyses a KDB content and creates 2 lists with all VAR and all SCL found in the kdb objects (limited to IDT, EQ or TBL).
| ~cvoid KE_scan(KDB* dbe, int i, KDB* exo, KDB* scal) | Analyses object i from a KDB of EQs and extracts all VARs and all SCLs from the CLEC struct.
| ~cvoid KI_scan(KDB* dbi, int i, KDB* exo, KDB* scal) | Analyses object i from a KDB dbi of IDTs and extracts all VARs and all SCLs from the LEC expression.
| ~cvoid KT_scan(KDB* dbt, int i, KDB* exo, KDB* scal) | Analyses object i from a KDB of TBLs and extracts all VARs and all SCLs from the LEC expressions found in the TCELLs.
| ~cint KL_lst(char* name, char** lst, int chunck)     | Creates a list from a table of strings. The elements in the new list are separated by semi-colons.
| ~cunsigned char **KL_expand(char *str)               | Replaces recursively list names in a string. Returns a table containing all terms in the string after replacement.
..te

&TI k_tbl.c
¯¯¯¯¯¯¯¯¯¯¯¯
Functions to manage TBL objects.
..tb
| ~cSyntax                                            | Description
| ~cTBL *T_create(int dim)                                                                     | Creates a new TBL objects.
| ~cvoid T_free(TBL* tbl)                                                                      | Frees a TBL object
| ~cvoid T_free_line(TLINE* line, int dim)                                                     | Frees a TLINE struct and all its TCELL.
| ~cvoid T_free_cell(TCELL* cell)                                                              | Frees a TCELL struct.
| ~cint T_add_line(TBL* tbl)                                                                   | Extents a TBL by adding at least one line.
| ~cTCELL *T_create_cell(TBL* tbl, TLINE* line)                                                | Initialises a TLINE of the type KT_CELL. 
| ~cTCELL *T_create_title(TBL* tbl, TLINE* line)                                               | Initialises a TLINE of the type KT_TITLE. 
| ~cchar* T_cell_cont(TCELL* cell, int mode)                                                   | Returns the formated contents of a TCELL.
| ~cint T_insert_line(TBL* tbl, int nbr, int type, int where)                                  | Inserts a TLINE in a TBL.
| ~cint T_set_lec_cell(TCELL* cell, unsigned char* lec)                                        | Assigns a LEC expression to a TCELL. Checks the syntax.
| ~cvoid T_set_string_cell(TCELL* cell, unsigned char* txt)                                    | Assigns a TEXT to a TCELL.
| ~cvoid T_set_cell_attr(TBL* tbl, int i, int j, int attr)                                     | Assigns justification (KT_CENTER...) and typographic (KT_BOLD...) attributes to a TCELL.
| ~cint T_default(TBL* tbl, char*titg, char**titls, char**lecs, int mode, int files, int date) | Fills a TBL with some basic data: a title, line titles and LEC expressions.
| ~cvoid T_auto(TBL* tbl, char* def, char** vars, int mode, int files, int date)               | Fills a TBL with a list of variables and their CMT. 
..te

&TI k_cmp.c
¯¯¯¯¯¯¯¯¯¯¯
Function to compare two IODE objects.

..tb
| Syntax                                            | Description
| ~cint K_cmp(char* name, KDB* kdb1, KDB* kdb2)     | Compares IODE objects having the same name in two KDB.
..te

&TI k_grep.c
¯¯¯¯¯¯¯¯¯¯¯
Functions to search strings in KDB objects.

..tb
| Syntax    | Description
| ~cchar **K_grep(KDB* kdb, char* pattern, int ecase, int names, int forms, int texts, int all)   | Creates a list of all objects in a KDB having a specific pattern in their names or LEC expression, comment...
| ~cchar *K_expand(int type, char* file, char* pattern, int all)                                  | Retrieves all object names matching one or more patterns in a workspace or an object file.
| ~cint K_aggr(char* pattern, char* ename, char* nname) *                                         | Transforms a variable name based on an "aggregation" pattern.
..te

>

<Group "IODE file management">
Group "IODE file management"
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
&TI k_objfile.c
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
Functions to manipulate IODE object files.

..tb
| Syntax                                                                          | Description
| ~cint K_get_ext(char* filename, char* ext, int max_ext_len)                     | gets a filename extension.
| ~cint K_has_ext(char* filename)                                                 | indicates if a filename contains an extension. 
| ~cchar *K_set_ext(char* res, char* fname, int type)                             | deletes left and right spaces in a filename and changes its extension according to the given type.
| ~cchar *K_set_ext_asc(char* res, char* fname, int type)                         | trims a filename then changes its extension to the ascii extension according to the given type).
| ~cvoid K_strip(char* filename)                                                  | deletes left and right spaces in a filename. Keeps the space inside the filename.
| ~cKDB  *K_load(int ftype, FNAME fname, int no, char** objs)                     | loads a IODE object file. 
| ~cint K_filetype(char* filename, char* descr, int* nobjs, SAMPLE* smpl)         | retrieves infos on an IODE file: type, number of objects, SAMPLE
| ~cKDB *K_interpret(int type, char* filename): generalisation of K_load()        | interprets the content of a file, ascii files includes, and try to load ist content into a KDB.
| ~cint K_copy(KDB* kdb, int nf, char** files, int no, char** objs, SAMPLE* smpl) | reads a list of objects from a list of IODE object files and adds them to an existing KDB.
| `cint K_cat(KDB* ikdb, char* filename)                                          | concatenates the content of a file to an existing kdb.
| `cint K_set_backup_on_save(int take_backup)                                     | sets the backup choice before saving a kdb. 
| `cint K_get_backup_on_save()                                                    | indicates if a backup must be taken before saving a kdb. 
| ~cint K_backup(char* filename)                                                  | takes a backup of a file by renaming the file: filename.xxx =>> filename.xx$.
| ~cint K_save(KDB* kdb, FNAME fname)                                             | saves a KDB in an IODE object file. The extension of fname is replaced by the standard one (.cmt, .eqs...).
| ~cint K_save_ws(KDB* kdb)                                                       | saves a KDB in an IODE object file called "ws.<<ext>>" where <<ext>> is one of (.cmt, .eqs...).
| ~cint K_setname(char* from, char* to)                                           | replaces KNAMEPTR(kdb) in an IODE object file.
..te
>

<Group "IODE big- and little-endian conversion">
Group "IODE big- and little-endian conversion"
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
&TI k_xdr.c
¯¯¯¯¯¯¯¯¯¯¯¯
Functions to convert big-endian data, used by processors like RISC,... into little-endian format (x86,...) and vice-versa.

..tb
| Syntax                                  | Description
| ~cvoid K_xdrPINT(unsigned char* a)      | Converts un short int from l-e to b-e and vice-versa
| ~cvoid K_xdrPLONG(unsigned char* a)     | Converts un long int from l-e to b-e and vice-versa
| ~cvoid K_xdrKDB(KDB* ikdb, KDB** okdb)  | Converts a KDB t from l-e to b-e and vice-versa
|                                         |
| ~cint (*K_xdrobj[])()                   | Table of function pointers, one function for each object type, for converting
..te                                          | big-endian to little-endian and vice-versa
> 



<Group "IODE object ascii formats">
Group "IODE object ascii formats" 
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
Functions to load and save files in IODE ascii format and LArray csv format.

&TI Filename extensions
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
|Type             |Binary extension|Ascii extension
| comments        |.cmt            |.ac 
| equations       |.eqs            |.ae 
| identities      |.idt            |.ai 
| lists           |.lst            |.al 
| scalars         |.scl            |.as 
| tables          |.tbl            |.at 
| variables       |.var            |.av 
| LArray variables|-               |.csv
..te

&TI k_ccall.c
¯¯¯¯¯¯¯¯¯¯¯¯
Tables of pointers to functions for reading and writing IODE objects in ASCII and CSV formats.

..tb
| Syntax                                         
| ~cKDB  *(*K_load_asc[])()
| ~cint (*K_save_asc[])()
| ~cint (*K_save_csv[])()
..te

&TI k_cccmt.c
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
Loading and saving IODE ascii comment files.

..tb
| Syntax                                      | Remark      
| ~cKDB *KC_load_asc(char* filename)          |
| ~cint KC_save_asc(KDB* kdb, char* filename) |
| ~cint KC_save_csv(KDB *kdb, char *filename) | Not implemented
..te



&TI k_cceqs.c
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
Loading and saving IODE ascii equation files.

..tb
| Syntax                                      | Remark      
| ~cKDB *KE_load_asc(char* filename)          |
| ~cint KE_save_asc(KDB* kdb, char* filename) |
| ~cint KE_save_csv(KDB *kdb, char *filename) | Not implemented
..te

&TI k_ccidt.c 
¯¯¯¯¯¯¯¯¯¯¯¯
Loading and saving IODE ascii identity files.

..tb
| Syntax                                      | Remark      
| ~cKDB *KI_load_asc(char* filename)          |
| ~cint KI_save_asc(KDB* kdb, char* filename) |
| ~cint KI_save_csv(KDB *kdb, char *filename) | Not implemented
..te

&TI k_cclst.c
¯¯¯¯¯¯¯¯¯¯¯¯
Loading and saving IODE ascii list files.

..tb
| Syntax                                      | Remark      
| ~cKDB *KL_load_asc(char* filename)          |
| ~cint KL_save_asc(KDB* kdb, char* filename) |
| ~cint KL_save_csv(KDB *kdb, char *filename) | Not implemented
..te

&TI k_ccscl.c
¯¯¯¯¯¯¯¯¯¯¯¯
Loading and saving IODE ascii scalar files.

..tb
| Syntax                                         | Remark   
| ~cKDB *KS_load_asc(char* filename)             |
| ~cint KS_save_asc(KDB* kdb, char* filename)    |
| ~cint KS_save_csv(KDB *kdb, char *filename)    | Not implemented
..te

&TI k_cctbl.c
¯¯¯¯¯¯¯¯¯¯¯¯
Loading and saving IODE ascii table files.

..tb
| Syntax                                      | Remark   
| ~cKDB *KT_load_asc(char* filename)          |
| ~cint KT_save_asc(KDB* kdb, char* filename) |
| ~cint KT_save_csv(KDB *kdb, char *filename) | Not implemented
..te

&TI k_ccvar.c
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
Functions to import and export IODE files to/from ascii and LArray-csv format.

..tb
| Syntax                                         
| ~cKDB *KV_load_asc(char *filename)
| ~cKV_save_asc(KDB* kdb, char* filename)
| ~cint KV_save_csv(KDB *kdb, char *filename, SAMPLE *smpl, char **varlist)
..te
>

<Group "LEC language">
Group "LEC language"
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
For the LEC implementation, see ~l"LEC.md"LEC.md~L.

&TI k_lec.c
¯¯¯¯¯¯¯¯¯¯¯¯
Implemention of the LEC library virtual functions for SCL and VAR references.

..tb
| Syntax                                    | Description    
| ~cdouble *L_getvar(KDB* kdb, int pos)  | Retrieves a pointer to the first element of a VAR.  
| ~cdouble L_getscl(KDB* kdb, int pos)   | Retrieves a scalar value.
| ~cSAMPLE *L_getsmpl(KDB* kdb)             | Retrieves the sample of a KDB.
| ~cint L_findscl(KDB* kdb, char *name)     | Retrieves a scalar position.
| ~cint L_findvar(KDB* kdb, char* name)     | Retrieves a variable position.
..te
>

<Group "Basic functions">
Group "Basic functions"
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯

&TI b_args.c
¯¯¯¯¯¯¯¯¯¯¯¯¯¯
Basic functions for managing function and report arguments.

..tb
| Syntax                                                    | Description
| ~cchar **B_ainit_chk(char* arg, ADEF* adef, int nb)       | expands an argument by replacing @filename and $listname by their contents
| ~cchar **B_vtom_chk(char* arg, int nb)                    | splits a string (generally a function argument) into a table of strings. 
| ~cint B_loop(char *argv[], int (*fn)(), char* client)     | executes the function fn(char*, char*) for each string in the table of strings argv.
| ~cint B_ainit_loop(char* arg, int (*fn)(), char* client)  | calls B_ainit_check() to expand arg, then calls B_loop() on the resulting table of strings.
| ~cint B_get_arg0(char* arg0, char*arg, int lg)            | computes arg0, the first arg ('word') of max lg bytes, in the string arg. 
| ~cint B_argpos(char* str, int ch)                         | returns the position of a char in a string. 
..te

&TI b_errors.c
¯¯¯¯¯¯¯¯¯¯¯¯¯¯
Basic functions for managing error messages.

..tb
| Syntax                            | Description
| ~cchar *B_msg(int n)              | Returns a static buffer containing the message n from file iode.msg. 
| ~cvoid B_seterror(char* fmt, ...) | Formats an error message and adds the text of the message to the global table of last errors.
| ~cvoid B_seterrn(int n, ...)      | Formats a message found in iode.msg and adds the result to the list of last errors.
| ~cvoid B_display_last_error()     | Displays the last recorded errors (in B_ERROR_MSG) using kmsgbox().
| ~cvoid B_print_last_error()       | Displays or prints the last recorded errors (in B_ERROR_MSG) using W_printf().
| ~cvoid B_clear_last_error()       | Resets the list of last messages (B_ERROR_MSG and B_ERROR_NB).
..te


>

<Group "Model Simulation"            >
Group "Model Simulation"            
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
See ~l"simul.md"SIMUL.md~L.
>

<Group "Table Calculation"           >
Group "Table Calculation"           
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
See ~l"tbl_calc.md"TBL_CALC.md~L.
>

<Group "Execution of identities"     >
Group "Execution of identities"    
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
See ~l"identities.md"IDENTITIES.md~L.
>

<Group "Model Estimation" >
Group "Model Estimation"
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
See ~l"ESTIM.md"ESTIM.md~L.
>

<Group "IODE Printing">
Group "IODE Printing"
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
See ~l"PRINTING.md"PRINTING.md~L.
>

<Group "Iode Reports">
Group "Iode Reports"
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯

See ~l"REPORTS.md"REPORTS.md~L.
>

<Group "IODE object conversions"      >
Group "IODE object conversions"
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
See ~l"CONVERSIONS.md"CONVERSIONS.md~L.

>

<Group "Iode high-level API"         >
Group "Iode high-level API"         
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯

&TI b_api.c
¯¯¯¯¯¯¯¯¯¯¯¯

Set of high-level functions essentially developed for the creation of the Cython implementation of IODE.
Most of these functions are (almost) aliases to other API functions but with a different and more
coherent naming convention.

..tb
| Syntax                                          | Description
| ~cint IodeInit()                                | Initialise an IODE session.
| ~cint IodeEnd()                                 | Terminate an IODE session.
| ~cchar *IodeVersion()                           | Return the IODE version in a const string.
| ~bWS related functions
| ~c  int IodeLoad(char *name, int type)            | Return the IODE version in a const string.
| ~c  int IodeSave(char *name, int type)            | Load an IODE workspace file.
| ~c  int IodeClearWs(int type)                     | Clear a workspace.
| ~c  int IodeClearAll()                            | Clear all workspaces.
| ~c  char **IodeContents(char *pattern, int type)  | Returns a table of object names corresponding to the specified pattern.
| ~bSAMPLE Functions
| ~c  int IodeGetSampleLength()                     | Returns the current sample length (0 if undefined)
| ~c  int IodeIsSampleSet()                         | Indicates if the VAR sample is defined
| ~c  char *IodeGetSampleAsString()                 | Returns current sample in an ALLOCATED string in the form "per1 per2".
| ~c  char **IodeGetSampleAsPeriods()               | Return all periods of the current KV_WS sample in a table of strings.
| ~c  char **IodeCreateSampleAsPeriods(char* aper_from, char* aper_to) | Return all periods from aper_from to aper_to in a table of strings.
| ~c  double *IodeGetSampleAsDoubles(int *lg)                          | Return a sample as a list of doubles.
| ~c  int IodeSetSampleStr(char* str_from, char* str_to)               | Set the WS sample from periods as strings
| ~c  int IodeSetSample(int from, int to)                              | Set the WS sample from integers (yearly only).
| ~bOBJECTS MANIPULATION FUNCTIONS
| ~c  int IodeDeleteObj(char* obj_name, int obj_type)                  | Delete the object obj_name of type obj_type.
| ~c  char *IodeGetCmt(char *name)                                     | Return a pointer to the comment "name". The pointer cannot be freed.
| ~c  int IodeSetCmt(char *name, char *cmt)                            | Save a the comment cmt under the name "name".
| ~c  int IodeGetEqs(char *name, char**lec, int *method, char*sample_from, char* sample_to, char**blk, char**instr, float *tests) | Retrieve an equation and its elements.
| ~c  char *IodeGetEqsLec(char *name)                                  | Retrieve the (non allocated) LEC form of the equation "name".
| ~c  int IodeSetEqs(char *name, char *eqlec)                          | Change the LEC form of an equation.
| ~c  char *IodeGetIdt(char *name)                                     | Return the (non allocated) pointer to an identity.
| ~c  int IodeSetIdt(char *name, char *idt)                            | Set the LEC form of an identity.
| ~c  char *IodeGetLst(char *name)                                     | Return the (non allocated) pointer to a list.
| ~c  int IodeSetLst(char *name, char *lst)                            | Set a list value.
| ~c  int IodeGetScl(char *name, double* value, double *relax, double *std_err) | Change the values of an existing scalar. Return -1 if the scalar does not exist.
| ~c  int IodeSetScl(char *name, double value, double relax, double std) | Set the values of a scalar. The scalar is created if it does not exist.
| ~c  char* IodeGetTbl(char *name, char *gsmpl)                        | Compute a table on the GSAMPLE gsmpl and return a string containing the result.  
| ~c  char* IodeGetTblTitle(char *name)                                | Return a table (first) title in an not allocated string.
| ~c  TBL* IodeGetTblDefinition(char *name)                            | Return a table struct
| ~c  int IodeSetTblFile(int ref, char *filename)                      | Load the var file filename and set its reference number to ref.
| ~c  double IodeGetVarT(char *name, int t, int mode)                  | Get the value of the variable name at position t where t is the position in the KV_WS SAMPLE.
| ~c  int IodeSetVarT(char *name, int t, int mode, double value)       | Set the value of the variable name at position t, possibly after recalulation based on value and mode.
| ~c  double *IodeGetVector(char *name, int *lg)                       | Returns a pointer to the first value of the VAR name. 
| ~c  int IodeCalcSamplePosition(char *str_pyper_from, char* str_pyper_to, int *py_pos, int *ws_pos, int *py_lg) |
| ~c  int IodeSetVector(char *la_name, double *la_values, int la_pos, int ws_pos, int la_lg) | Determines the position to copy from (python object), where to copy to (KV_WS) and the nb of elements to copy 
| ~bESTIMATION
| ~c  int IodeEstimate(char* veqs, char* afrom, char* ato)             | Estimate an equation of a given sample. 
| ~bSIMULATION
| ~c  int IodeModelSimulate(char *per_from, char *per_to, char *eqs_list, char *endo_exo_list, double eps, double relax, int maxit, int init_values, int sort_algo, int nb_passes, int debug, double newton_eps, int newton_maxit, int newton_debug) | Simulate of model.
| ~bREPORTS
| ~c  int IodeExecArgs(char *filename, char **args)                    | Execute a report with optionnal parameters.
| ~c  int IodeExec(char *filename)                                     | Execute a report with no parameters.
| ~bLEC DIRECT EXECUTION
| ~c  static CLEC* IodeLinkLec(char* lec)                              | Compile and link a LEC expression with the KV_WS and KS_WS.
| ~c  double IodeExecLecT(char* lec, int t)                            | Calc a LEC expression in t.
| ~c  double *IodeExecLec(char* lec)                                   | Calculate a LEC expression on the full KV_WS sample.
| ~bMESSAGES
| ~c  void IodeSuppressMsgs()                                          | Suppress all messages from the A2M interpretor and from the IODE functions.
| ~c  void IodeResetMsgs()                                             | Reset the messages from the A2M interpretor and from the IODE functions.
| ~bMISCELLANEOUS
| ~c  int IodeSetNbDec(int nbdec)                                      | Define the number of decimals in the tables and variables to be printed.
| ~c  int IodeGetNbDec()                                               |  Returns the number of decimals currently set for the printing of tables and variables.
..te

>

<Group "DDE communcations">
Group "DDE communcations"
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
&TI b_dde.c
¯¯¯¯¯¯¯¯¯¯¯

This file contains 2 groups of functions: the implementation of the IODE DDE server, allowing access to IODE functions from inside any program able to 
interact with a DDE server, like Excel or Word and functions to access the Excel DDE server. 

Some of IODE report commands line $ExcelGet are implemented here.

..tb
| Syntax                                          | Description
| ~cint IodeDdeLocale(char *buf)
| ~cint IodeDdeUnLocale(char *buf)
| ~cint IodeDdeType(char *szTopic)
| ~cchar *IodeDdeGetWS(char *szItem)
| ~cchar *IodeDdeCreateSeries(int objnb, int bt)
| ~cchar *IodeDdeCreatePer(int bt)
| ~cchar *ToBase26(int num)
| ~cchar *IodeDdeXlsCell(char *offset, int i, int j, int lg, int hg)
| ~cchar *IodeTblCell(TCELL *cell, COL *cl, int nbdec)
| ~cchar *IodeDdeCreateTbl(int objnb, char *ismpl, int *nc, int *nl, int nbdec)
| ~cchar *IodeDdeCreateObj(int objnb, int type, int *nc, int *nl)
| ~cchar *IodeDdeGetReportRC(char *szItem)
| ~cchar *IodeDdeGetXObj(char *szItem, int type)
| ~cchar *IodeDdeGetItem(char *szTopic, char *szItem)
| ~cint IodeDdeSetWS(char *szItem, char *szBuffer)
| ~cint IodeDdePlay(char *szItem, char *szBuffer)
| ~cint DdeTsfKey(char *key)
| ~cint IodeDdeSetItem(char *szTopic, char *szItem, char *szBuffer)
| ~cchar *B_ExcelGetItem(char *arg)
| ~cint B_ExcelSetItem(char *ddeitem, char *ptr, int nc, int nl)
| ~cint B_ExcelDecimal(char *arg)
| ~cint B_ExcelThousand(char *arg)
| ~cint B_ExcelCurrency(char *arg)
| ~cint B_ExcelLang(char *arg)
| ~cint B_ExcelGet(char *arg, int type)
| ~cint B_ExcelSet(char *arg, int type)
| ~cint B_ExcelExecute(char *arg)
| ~cint B_ExcelCmd(char *cmd, char *arg)
| ~cint B_DDEGet(char *arg)
| ~cint B_ExcelWrite(char *ptr)
| ~cint B_DDEGet(char *arg)
| ~cchar *B_ExcelGetItem(char *arg)
| ~cint B_ExcelGet(char *arg, int type)
| ~cint B_ExcelSet(char *arg, int type)
| ~cint B_ExcelExecute(char *arg)
| ~cint B_ExcelCmd(char *cmd, char *arg)
| ~cint B_ExcelWrite(char *ptr)
| ~cint B_ExcelOpen(char *arg)
| ~cint B_ExcelClose(char *arg)
| ~cint B_ExcelPrint(char *arg)
| ~cint B_ExcelSave(char *arg)
| ~cint B_ExcelSaveAs(char *arg)
| ~cint B_ExcelNew(char *arg)
| ~cint IodeFmtVal(char *buf, double val)
..te

>