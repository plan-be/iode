# IODE functions by source file

## Utilities

### buf.c
Allocates or extends a global buffer of at least len bytes. 

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
 
###### Representation functions

    char *PER_pertoa(per, text): creates a text representation of a PERIOD.
    PERIOD *PER_atoper(char *text): translates a text in a PERIOD.
    char *PER_smpltoa(SAMPLE* smpl, char* text)
    SAMPLE *PER_atosmpl(char* a1, char* a2): creates a SAMPLE based on two given PERIOD strings.
    SAMPLE *PER_pertosmpl(PERIOD* p1, PERIOD* p2):

###### Miscellaneous functions 

     int PER_nb(int ch): gives the nbr of periods in one year for the periodicity ch
     int PER_nbper(PERIOD* period)
     IODE_REAL PER_per2real(PERIOD* from, int i)

## WS and Objects management

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

###### Unpacking functions

     TBL* K_tunpack(char *pack)
     EQ* K_eunpack(char *pack)

###### Allocation functions (VAR & SCL only)

     int KS_alloc_scl()
     int KV_alloc_var(int nb)

### objs.c

###  k_ws.c

### k_cc*.c
Function to import / export IODE files from/to ascii and csv format.

## LEC 

## Estimation

## Simulation

## Reports

## 