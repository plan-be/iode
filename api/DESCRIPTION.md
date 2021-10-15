# IODE functions by source file

## Global Utilities

 - buf.c : share a large allocated buffer in different parts of the application
 - pack.c : packing (serialize) and unpacking (deserialize) objects.
 - per.c : functions for manipulating PERIOD and SAMPLE in IODE.
 - yy.c : helper functions for reading and writing IODE ascii files.

### buf.c
Functions to share and reuse a large allocated buffer in different parts of the application. 

    char *BUF_alloc(int len) :  allocates or extends a global buffer of at least len bytes. 
    void BUF_free() : free the buffer
    void BUF_lock() : reserve the buffer utilisation
    void BUF_unlock() : unlock the buffer
    char *BUF_memcpy(char *ptr, int lg) : copy the the first lg bytes following address ptr to the buffer
    char *BUF_strcpy(char *ptr) : copy a null terminated string to the buffer
    char *BUF_DATA : NULL or pointer to the allocated buffer

### pack.c
Packing (serialize) and unpacking (deserialize) objects. 

    void *P_create()
    int P_free(char *ptr)
    void *P_add(void *vptr1, void *vptr2, int lg)
    void *P_get_ptr(void *vptr, int i)
    OSIZE P_get_len(void *vptr, int i)
    void *P_alloc_get_ptr(void *ptr, int p)
    int P_nb(char *ptr)

### per.c
Functions for manipulating PERIOD and SAMPLE in IODE.
  
###### Calculation functions

    int PER_diff_per(PERIOD *p1, PERIOD *p2): calculates the number of sub periods between two periods. 
    PERIOD *PER_addper(PERIOD *period, int shift): adds (sub-)periods to period.
    int PER_common_smpl(SAMPLE* smp1, SAMPLE* smp2, SAMPLE* res): calculates the intersection between 2 SAMPLEs.
 
###### Conversion functions

    char *PER_pertoa(per, text): creates a text representation of a PERIOD.
    PERIOD *PER_atoper(char *text): translates a text in a PERIOD.
    char *PER_smpltoa(SAMPLE* smpl, char* text)
    SAMPLE *PER_atosmpl(char* a1, char* a2): creates a SAMPLE based on two given PERIOD strings.
    SAMPLE *PER_pertosmpl(PERIOD* p1, PERIOD* p2):

###### Miscellaneous functions 

    int PER_nb(int ch): gives the nbr of periods in one year for the periodicity ch
    int PER_nbper(PERIOD* period)
    IODE_REAL PER_per2real(PERIOD* from, int i)

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


## IODE Version
### k_vers.c
Functions to retrieve the current IODE versions.

    char *K_LastVersion() 
    char *K_CurrentVersion()_

## Objects management

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

###### Allocation functions (VAR & SCL only)

     int KS_alloc_scl()
     int KV_alloc_var(int nb)

## WS management

### objs.c
Function to manipulate workspaces

###  k_ws.c


## IODE ascii format
Functions to load and save files in IODE ascii format and LArray csv format.

**Ascii filenames**
 * .ac: comments
 * .ae: equations
 * .ai: identities
 * .al: lists
 * .as: scalars
 * .at: tables
 * .av: variables
 * .csv: LArray variables

### k_ccall.c
Tables of pointers to functions for reading and writing IODE objects in ASCII and CSV format.

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


## LEC 

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


- k_ws 
- k_cmp 
- k_vers 
- k_lec 
- objs 
- objsv  
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
- k_var 
- k_val
- k_lst 
- k_eqs 
- k_tbl 
- k_est 
- k_ccvar 
- k_ccall
- k_cccmt 
- k_ccscl 
- k_ccidt 
- k_cclst 
- k_cceqs
- k_cctbl 
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
