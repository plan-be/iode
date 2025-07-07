#ifndef _SWAP_
#define _SWAP_

#ifdef UNIX
#define NOEMS
#endif

#if !defined(NOEMS) && (defined(WINDOWS) || defined(DOSWIN) || defined(SCRW32) || defined(VC)) /* JMP 12-02-2017 */
#define NOEMS
#endif

#ifdef WATCOM
#define NOEMS
#endif

#include <stdio.h>
#include <stdlib.h>
#include <s_lst.h>
#include <s_alldoc.h>

//  extern char *getenv(); /* JMP 03-01-98 */

#ifdef DOS
#include <fcntl.h>
#endif

#ifndef NOEMS

/* ======================= EMS MANAGEMENT ========================== */

typedef struct _emsdef_ {
    int         nb_pages,
		nb_free,
		nb_alloc,
		swerrno,
		ver,
		log_pgs[4];
    char        *ptr;
    unsigned    handle;
} EMSDEF;

#define EMS_SEG(pg)         (EMS_DEF.ptr + (long)pg * (long) SW_SEG_SIZE)

extern EMSDEF   EMS_DEF;
extern int      SW_NB_EMS;

#endif /* EMS */

/* ======================= DEFINES =============================== */

#ifdef DOS
#define SW_ALIGN            2
// #define SNGL_TASK /* JMP 11-03-07 */
#if defined(DOSWIN) || defined(SCRW32)          /* JMP 12-11-97 */
// #define SW_SEG_SIZE         32500 /* JMP 27-11-07 */
#endif   /* DOSWIN ... */
#endif   /* DOS */

#ifdef WATCOM
#define SW_ALIGN            2
#define MULT_TASK
#endif /* WATCOM */

#ifdef XENIX
#define SW_ALIGN            2
#endif

#ifdef SUN3
#define SW_ALIGN            2
#endif

#ifdef SUN4
#define SW_ALIGN            4
#endif

#ifdef HP
#define SW_ALIGN            4
#endif
#ifndef SW_ALIGN
#define SW_ALIGN            2
#endif

/*
#ifndef SW_SEG_SIZE
//#define SW_SEG_SIZE         16384
#define SW_SEG_SIZE         (16384*16)
#endif
*/
/* JMP 27-11-07 */

#define AGE(seg_nb)         (SW_SEGS[seg_nb].seg_age)
#define PTR(blk_nb)         (SW_SEGS[SW_BLKS[blk_nb].blk_seg].seg_ptr)

#define BLKSIZE(blk_nb)     (SW_BLKS[blk_nb].blk_size)
#define FREESP(blk_nb)      (SW_BLKS[blk_nb].blk_free)
#define SPACE(blk_nb)       (SW_BLKS[blk_nb].blk_space)
#define COMPRESS(blk_nb)    (SW_BLKS[blk_nb].blk_compress)
#define SEG(blk_nb)         (SW_BLKS[blk_nb].blk_seg)
#define REF(blk_nb)         (SW_BLKS[blk_nb].blk_ref)
#define BLKAGE(blk_nb)      (SW_BLKS[blk_nb].blk_age)

/*#define SWHDL               short
#define s_short             sizeof(short)
#define s_hdl               sizeof(SWHDL)
#define h_size              (s_short + s_hdl)
*/
/*
#define SWHDL               unsigned short
#define SWSIZE              unsigned short
*/
typedef unsigned long SWSIZE;
typedef unsigned long SWHDL;
/*
typedef unsigned short SWSIZE;
typedef unsigned short SWHDL;
*/
#define s_size              sizeof(SWSIZE)
#define s_hdl               sizeof(SWHDL)
#define h_size              (s_size + s_hdl)

/* ======================= STRUCTS =============================== */
typedef struct _sw_blk_ {
    int     blk_seg;            /* MEM SEGMENT NUMB (-1 if OUT) */
    SWSIZE  blk_space;          /* space left */
    int     blk_ref;            /* SWAP INFO (FILE BLK NB) */
    SWSIZE  blk_free;           /* FIRST FREE BYTE */
    char    blk_compress;       /* IS BLK COMPRESSED */
    long    blk_age;            /* BLOCK AGE */
    long    blk_size;           /* BLOCK SIZE */ /* JMP 23-05-00 */
}   SWBLK;

typedef struct _sw_seg_ {
    char    *seg_ptr;           /* MEMORY PTR */
    long    seg_age;            /* AGE */
    int     seg_blk;            /* BLK NUMBER */
#ifndef NOEMS
    char    seg_type;           /* SEG TYPE  (1:Heap, 0:EMS) */
    char    seg_physp;          /* EMS PHYS PAGE (-1 if out of window) */
#endif
}   SWSEG;

typedef struct _swstat_ {
    int     st_nballoc;     /* Number of allocations        */
    int     st_nbfree;      /* Free allocations             */
    int     st_nbblks;      /* Number of blocks             */
    int     st_nbsegs;      /* Number of segments           */
    int     st_segsize;     /* Segment size                 */
    long    st_emsalloc;    /* Total EMS allocation         */
    long    st_filealloc;   /* Total FILE allocation        */
    long    st_convalloc;   /* Total Conv. allocation       */
    long    st_emsafree;    /* Not used allocated EMS       */
    long    st_fileafree;   /* Not used allocated File      */
    long    st_convafree;   /* Not used allocated Conv      */
    long    st_convfree;    /* Free conventional memory     */
} SWSTAT;

/* ======================= VARS =============================== */

#ifdef __cplusplus 
// JMP 12/05/2021. Pour utiliser les vars en C++, il faut des d├®clarer extern "C"  
extern "C" {
#endif 


extern int      SW_ACTIVE;
extern long     SW_AGE, SW_MIN_MEM;
extern SWBLK    *SW_BLKS;
extern int      SW_NB_BLKS;
extern SWSEG    *SW_SEGS;
extern int      SW_NB_SEGS, SW_MIN_SEGS;
extern LST      *SW_ITEMS;
extern LST      *SW_FREES;
extern int      SW_FILE_POS;
/* extern char     *SW_BUF;*/
extern char     SW_FILE[];
extern int      SW_SEG_SIZE; /* JMP 27-11-07 */

#ifdef __cplusplus
}
#endif 

/* =================== FNS PROTOTYPES =========================== */

#ifndef SCRPROTO
extern char     *EMS_error_text();
extern int      SW_init();
extern void     SW_end();
extern SWHDL    SW_alloc();
extern SWHDL    SW_realloc();
extern char     *SW_getptr();
extern char     *SW_nalloc_doc();
extern char     *SW_nrealloc_doc();
extern char     *SW_nalloc_orig();
extern char     *SW_nrealloc_orig();
extern SWHDL    SW_find_free_item();
extern long     SW_coreleft();
extern SWSIZE   SW_get_pos();
extern SWHDL    SW_get_free();
extern void     SW_Debug();
#else

#include <s_prosw.h>

#ifdef __cplusplus
extern "C" {
    int Wprintf(char *, ...);
}
#endif /* __cplusplus */

#endif /* SCRPROTO */


#endif /* _SWAP_ */

// WARNING: the min and max macro make conflicts with the GNU implementation 
//          of the C++ standard library
#if !defined(__GNUC__) && !defined(__cplusplus)
    #ifndef min
        #define min(a, b)   (((a) < (b)) ? (a) : (b))
    #endif
    #ifndef max
        #define max(a, b)   (((a) > (b)) ? (a) : (b))
    #endif
#endif





