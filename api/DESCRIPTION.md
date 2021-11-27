# IODE functions by group

Note on function names: as C does not allow "namespacing", we always give prefixes to function names, the prefix giving
an indication of the group the function belongs to. For example, P*() is the group of function for "packing" (aka serializing) 
IODE objects, PER*() for manipulating PERIOD, L_* () for LEC compilation / execution, etc.

Most filenames follow the same principle: each group of files has a specific prefix which normally gives an indication of the group they belong to.

 - k_*: core functions (the prefix "k_" is a legacy of the "Kaa" software)
 - b_*: building *B*locks for IODE reports
 - ... to be continued...

## TOC
- A note on memory allocations in IODE
- Group "Global Utilities"
- Group "IODE Version"
- Group "KDB management"
- Group "Object management"
- Group "IODE file management"
- Group "IODE ascii format reading and writing"
- Group "LEC language"


## A note on memory allocations in IODE
IODE uses 2 distinct groups of functions for memory allocations. 

The **first group** is based on the "standard" memory allocation functions malloc() and free(). 
The main functions in this group are SW_nalloc(), SW_nrealloc() and SW_nfree(). An older group from *scr4* has the same properties: SCR_malloc(), SCR_realloc() and SCR_free()_

The **second group** has been created specifically to avoid memory segmentation when possible. The main idea is to allocate large memory buffers 
on the heap (called "segments") and to fill them with serialized objects, like "packed" equations or variables. 
Each allocated object is represented by an identifier (a "handle") and, when it is 
reclaim by the program, a trivial indirection translates its handle to the real pointer which is simply a shift from the beginning of its segment.

The main functions in this second group are **SW_init()**, **SW_alloc()**, **SW_realloc()**, **SW_free()**, **SW_getptr()** and **SW_end()**.

More information can be found at http://xon.be/scr4/libs1/libs1236.htm. 

## Group "Global Utilities"


 - buf.c : share a large allocated buffer in different parts of the application
 - pack.c : packing (serialize) and unpacking (deserialize) objects.
 - per.c : functions for manipulating PERIOD and SAMPLE in IODE.
 - yy.c : helper functions for reading and writing IODE ascii files.

### buf.c
Functions to share and reuse a large allocated buffer in different parts of the application. 

    char *BUF_alloc(int len) :  allocates or extends a global buffer of at least len bytes. 
    void BUF_free() : frees the buffer
    void BUF_lock() : reserves the buffer utilisation
    void BUF_unlock() : unlocks the buffer
    char *BUF_memcpy(char *ptr, int lg) : copies the the first lg bytes following address ptr to the buffer
    char *BUF_strcpy(char *ptr) : copies a null terminated string to the buffer

    char *BUF_DATA : NULL or pointer to the allocated buffer

### pack.c
Packing (serialize) and unpacking (deserialize) objects. 

    void *P_create() : creates (allocated) a new pack object
    int P_free(char *ptr) : frees a pack object
    void *P_add(void *vptr1, void *vptr2, int lg) : appends to the pack pointed to by vptr1 the content pointed to by vptr2 which has a length of lg bytes. 
    void *P_get_ptr(void *vptr, int i) : retrieves the pointer to the i'th element of a pack.
    OSIZE P_get_len(void *vptr, int i) : retrieves the length (in bytes) of the i'th element of a pack or the length of the full pack if i < 0.
    void *P_alloc_get_ptr(void *ptr, int p) : allocates space for the p'th element of a pack. Copies the pth element in the allocated space.
    int P_nb(char *ptr) : retrieves the number of elements in the pack pointed to by ptr.

### per.c
Functions for manipulating PERIOD and SAMPLE in IODE.
  
###### Calculation functions

    int PER_diff_per(PERIOD *p1, PERIOD *p2): calculates the number of sub periods between two periods. 
    PERIOD *PER_addper(PERIOD *period, int shift): adds (sub-)periods to period.
    int PER_common_smpl(SAMPLE* smp1, SAMPLE* smp2, SAMPLE* res): calculates the intersection between 2 SAMPLEs.
 
###### Conversion functions

    char *PER_pertoa(per, text): creates a text representation of a PERIOD.
    PERIOD *PER_atoper(char *text): translates a text in a PERIOD.
    char *PER_smpltoa(SAMPLE* smpl, char* text): writes a SAMPLE in a string.
    SAMPLE *PER_atosmpl(char* a1, char* a2): creates a SAMPLE based on two strings containing PERIODs.
    SAMPLE *PER_pertosmpl(PERIOD* p1, PERIOD* p2): returns a new allocated SAMPLE build on two given PERIOD.

###### Miscellaneous functions 

    int PER_nb(int ch): gives the nbr of periods in one year for the periodicity ch
    int PER_nbper(PERIOD* period): retrieves the number of periods in one year in a period
    IODE_REAL PER_per2real(PERIOD* from, int i): adds a number of sub-periods to a PERIOD and returns a 
        numerical representation of the result (used mainly for tables and graphs formatting).

### yy.c
Helper functions for reading and writing IODE ascii files.

    IODE_REAL K_read_real(YYFILE *yy)
    long K_read_long(YYFILE* yy)
    char* K_read_str(YYFILE* yy)
    PERIOD *K_read_per(YYFILE* yy)
    SAMPLE *K_read_smpl(YYFILE* yy)
    int K_read_align(YYFILE* yy)

    void K_stracpy(char** to, char* from)
    int K_wrdef(FILE* fd, YYKEYS* table, int def)
    int K_compare(YYKEYS* yy1, YYKEYS* yy2)
    char *K_wrap(char *in, int lg)


## Group "IODE Version"
### k_vers.c
Functions to retrieve the current IODE version.

    char *K_LastVersion(): returns in an allocated string the current version of IODE. 
    char *K_CurrentVersion(): returns in an allocated string the IODE version of the current executable.

## Group "KDB management"

### k_kdb.c
Function to manage KDB, i.e. IODE object groups.

      KDB *K_init_kdb(int ,char *);                        allocates and initialises a KDB struct
      void K_set_kdb_name(KDB *kdb, U_ch *filename);       changes the filename in a KDB
      KDB *K_create(int type, int mode)                    allocates and initialises a KDB object.    
      int K_free_kdb(KDB* kdb)                             frees a KDB but leaves its contents untouched.
      int K_free(KDB* kdb)                                 frees a KDB and its contents.
      int K_clear(KDB* kdb)                                deletes all objects in a KDB, reset the SAMPLE and replaces the filename by "ws". 
      KDB *K_refer(KDB* kdb, int nb, char* names[])        creates a new kdb containing the references to the objects of the list names.
      KDB *K_quick_refer(KDB *kdb, char *names[])          same as K_refer() but more efficient for large databases.
      int K_merge(KDB* kdb1, KDB* kdb2, int replace)       merges two databases : kdb1 <- kdb1 + kdb2. 
      int K_merge_del(KDB* kdb1, KDB* kdb2, int replace)   merges two databases : kdb1 <- kdb1 + kdb2 then deletes kdb2. 

###  k_ws.c
Variables and functions for initializing and cleaning up the "in memory" workspaces.

    KDB *K_WS[7]                  Table with pointers to the 7 KDB in memory, 1 per object type (CEILSTV)
    void K_init_ws(int ws)        Initialises the "in mem" KDB structures and optionnaly loads the ws.* files
    void K_end_ws(int ws)         Deletes the current workspaces defined in K_WS[] and their content after having optionnaly saved their content in ws.* files.

### k_wsvar.c
Functions acting on workspaces of variables.

    int KV_sample(KDB *kdb, SAMPLE *nsmpl)                                  Changes the SAMPLE of a KDB of variables.
    int KV_merge(KDB *kdb1, KDB* kdb2, int replace)                         Merges two KDB of variables: kdb1 <- kdb1 + kdb2.            
    void KV_merge_del(KDB *kdb1, KDB *kdb2, int replace)                    Merges 2 KDB of variables, then deletes the second one.
    double KV_get(KDB *kdb, int pos, int t, int mode)                       Gets VAR[t]  where VAR is the series in position pos in kdb. 
    void KV_set(KDB *kdb, int pos, int t, int mode, IODE_REAL new)          Sets VAR[t], where VAR is the series in position pos in kdb. 
    int KV_extrapolate(KDB *dbv, int method, SAMPLE *smpl, char **vars)     Extrapolates variables on a selected SAMPLE according to one of the available methods.
    KDB *KV_aggregate(KDB *dbv, int method, char *pattern, char *filename)  Creates a new KDB with variables created by aggregation based on variable names._

## Group "Object management"

- k_objsv.c: function to create an IODE object of any type.
- k_objs.c: functions to manipulate IODE objects.
- k_objvers.c: functions to detect IODE object file version and to convert an object to the current IODE version._
- k_pack.c: functions for "packing" and "unpacking" IODE objects.

### k_objsv.c
Function to create an IODE object and to record it in a KDB.

     int K_add(KDB* kdb, char* name, char* a1, char* a2, char* a3, char* a4, char* a5, char* a6, char* a7, char* a8, char* a9): adds an 
        object to a KDB. The number of arguments depends on object type.

How to create IODE objects using K_add():
 
    - Comments    K_add(KDB* kdb, char* name, CMT cmt)
    - Equations   K_add(KDB* kdb, char* name, EQ* eq, char* endo) [where endo = name]
    - Identities  K_add(KDB* kdb, char* name, char* lec)
    - Lists       K_add(KDB* kdb, char* name, char* list)
    - Scalars     K_add(KDB* kdb, char* name, SCL* scalar)
    - Tables      K_add(KDB* kdb, char* name, TBL *tbl) 
    - Variables   K_add(KDB* kdb, char* name, IODE_REAL* var, int nb_obs) [nb_obs = kdb SAMPLE size]
 
 Note: the name of an equation MUST be the name of its endogenous variable

### k_objs.c
Functions to manipulate IODE objects.

    int K_key(char* name, int mode):                             Checks the validity of an object name and modify its "case" according to the value of mode.
    int K_dup(KDB* kdb1, char* name1, KDB* kdb2, char* name2):   Duplicates an IODE object. Copies name1 of kdb1 to name2 in kdb2.
    int K_ren(KDB* kdb, char* name1, char* name2):               Renames the object name1 in kdb into name2.
    int K_add_entry(KDB* kdb, char* name):                       Adds the entry name in kdb and returns its position in the kdb. 
    int K_find(KDB* kdb, char* name):                            Searches the position of an object name in a KDB.
    int K_del_entry(KDB* kdb, int pos):                          Deletes an entry in a KDB __without__ deleting the referenced object. 
    int K_del(KDB* kdb, int pos):                                Deletes an object (and its data) from a KDB.
    int K_del_by_name(KDB* kdb, char* name):                     Deletes an object identified by its name from a KDB. 

### k_objvers.c
Functions to detect IODE object file version and to convert an object to the current IODE version._

     int K_calcvers(char* label):               returns the current object version (0-2) from an IODE file header. 
     void K_setvers(KDB* kdb, int i, int vers): converts an IODE object from IODE objects version 1 or 2 to the current version (0).

###  k_pack.c
Functions for "packing" and "unpacking" IODE objects.

###### Packing functions  

     int K_cpack(char **pack, char *a1)
     int K_epack(char **pack, char *a1, char *a2)
     int K_ipack(char **pack, char *a1)
     int K_lpack(char** pack, char* a1)
     int K_spack(char **pack, char *a1)
     int K_tpack(char** pack, char* a1)
     int K_vpack(char **pack, IODE_REAL *a1, int *a2)
     int K_opack(char** pack, char* a1, int* a2)

###### Unpacking functions (for TBL and EQ only ?)

     TBL* K_tunpack(char *pack)
     EQ* K_eunpack(char *pack)

###### Allocation functions (SCL & VAR only)

     int KS_alloc_scl()
     int KV_alloc_var(int nb)

### k_cmp.c
Function to compare two IODE objects.

    int K_cmp(char* name, KDB* kdb1, KDB* kdb2):            Compares IODE objects having the same name in two KDB.

## Group "IODE file management"

### k_objfile.c
Functions to manipulate IODE object files.

    char *K_set_ext(char* res, char* fname, int type):                               deletes left and right spaces in a filename and changes its extension according to the given type.
    void K_strip(char* filename):                                                    deletes left and right spaces in a filename. Keeps the space inside the filename.
    KDB  *K_load(int ftype, FNAME fname, int no, char** objs):                       loads a IODE object file. 
    int K_filetype(char* filename, char* descr, int* nobjs, SAMPLE* smpl):           retrieves infos on an IODE file: type, number of objects, SAMPLE
    KDB *K_interpret(int type, char* filename): generalisation of K_load() :         interprets the content of a file, ascii files includes, and try to load ist content into a KDB.
    int K_copy(KDB* kdb, int nf, char** files, int no, char** objs, SAMPLE* smpl):   reads a list of objects from a list of IODE object files and adds them to an existing KDB.
    int K_backup(char* filename):                                                    takes a backup of a file by renaming the file: filename.xxx => filename.xx$.
    int K_save(KDB* kdb, FNAME fname):                                               saves a KDB in an IODE object file. The extension of fname is replaced by the standard one (.cmt, .eqs...).
    int K_save_ws(KDB* kdb):                                                         saves a KDB in an IODE object file called "ws.<ext>" where <ext> is one of (.cmt, .eqs...).
    int K_setname(char* from, char* to):                                             replaces KNAMEPTR(kdb) in an IODE object file.

## Group "IODE big- and little-endian conversion"
Functions to convert big-endian data, used by processors like RISC,... into little-endian format (x86,...) and vice-versa.

    void K_xdrPINT(unsigned char* a)           Converts un short int from l-e to b-e and vice-versa
    void K_xdrPLONG(unsigned char* a)          Converts un long int from l-e to b-e and vice-versa
    void K_xdrKDB(KDB* ikdb, KDB** okdb)       Converts a KDB t from l-e to b-e and vice-versa

    int (*K_xdrobj[])()                        Table of function pointers, one function for each object type, for converting
                                               big-endian to little-endian and vice-versa
 
## Group "IODE ascii format (reading and writing)"
Functions to load and save files in IODE ascii format and LArray csv format.

**Filename extensions**

 |Type             |Binary extension|Ascii extension|
 |----             |----------------|---------------|
 | comments        |.cmt            |.ac            |
 | equations       |.eqs            |.ae            |
 | identities      |.idt            |.ai            |
 | lists           |.lst            |.al            |
 | scalars         |.scl            |.as            |
 | tables          |.tbl            |.at            |
 | variables       |.var            |.av            |
 | LArray variables|-               |.csv           |

### k_ccall.c
Tables of pointers to functions for reading and writing IODE objects in ASCII and CSV formats.

    KDB  *(*K_load_asc[])()
    int (*K_save_asc[])()
    int (*K_save_csv[])()

### k_cceqs.c
Loading and saving IODE ascii equation files.

    KDB *KE_load_asc(char* filename)
    int KE_save_asc(KDB* kdb, char* filename)
    int KE_save_csv(KDB *kdb, char *filename) : not implemented 

### k_ccidt.c 
Loading and saving IODE ascii identity files.

    KDB *KI_load_asc(char* filename)
    int KI_save_asc(KDB* kdb, char* filename)
    int KI_save_csv(KDB *kdb, char *filename)

### k_cclst.c
Loading and saving IODE ascii list files.

    KDB *KL_load_asc(char* filename)
    int KL_save_asc(KDB* kdb, char* filename)
    int KL_save_csv(KDB *kdb, char *filename)

### k_ccscl.c
Loading and saving IODE ascii scalar files.

    KDB *KS_load_asc(char* filename)
    int KS_save_asc(KDB* kdb, char* filename)
    int KS_save_csv(KDB *kdb, char *filename)

### k_cctbl.c_
Loading and saving IODE ascii table files.

    KDB *KT_load_asc(char* filename)
    int KT_save_asc(KDB* kdb, char* filename)
    int KT_save_csv(KDB *kdb, char *filename)

### k_ccvar.c
Functions to import and export IODE files to/from ascii and LArray-csv format.

    KDB *KV_load_asc(char *filename)
    KV_save_asc(KDB* kdb, char* filename)
    int KV_save_csv(KDB *kdb, char *filename, SAMPLE *smpl, char **varlist)


## Group "LEC language"

### k_lec.c
Implemention of the LEC library virtual functions for SCL and VAR references.

    IODE_REAL *L_getvar(KDB* kdb, int pos)
    IODE_REAL L_getscl(KDB* kdb, int pos)
    SAMPLE *L_getsmpl(KDB* kdb)
    int L_findscl(KDB* kdb, char *name)
    int L_findvar(KDB* kdb, char* name)

## Estimation

## Simulation

## Reports

## Remaining source files 


- k_xdr 

- l_token 
- l_cc1 
- l_alloc 
- l_err 
- l_vars
- l_cc2 
- l_link 
- l_exec 
- l_eqs 
- l_newton 
- l_debug
- l_secant 

- e_est 
- e_step 
- e_tests 
- e_prep
- e_error 
- e_errorv 
- e_print 
- e_stat 
- 
- k_var 
- k_val
- k_lst 
- k_eqs 
- k_tbl 
- k_est 
- k_iprn 
- k_imain 
- k_iasc 
- k_lang
- k_irasc 
- k_idif 
- k_ibst 
- k_inis 
- k_itxt 
- k_igem
- k_emain 
- k_wks 
- k_ecsv 
- k_edif 
- k_etsp
- k_ewks 

- c_cc 
- c_calc 

- b_base 
- b_dir
- b_file b_fdel b_fcopy b_fren
- b_rep b_rep2 b_proc b_defs b_data b_ws b_prof b_ras b_eviews
- b_readme b_pdest b_pnogui b_model b_print b_view
- b_idt b_est b_htol b_ltoh b_xode b_api
- b_season b_trend k_sim 
- k_exec 
- k_print
- k_graph 
- k_grep 
- b_dde 
- b_basev 
- b_sql 
- b_ds
- w_wrt 
- w_wrtv 
- w_wrt1
- odelinux 
- l_rand 
- m_debug 
- nochart
