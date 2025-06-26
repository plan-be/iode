#ifndef _S_STRS_
#define _S_STRS_

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Alloc doc */ // JMP 28/9/2012
#include "s_alldoc.h"

#ifdef SCRCPP
extern "C" {
#endif

// #define UCHAR unsigned char /* JMP 14-10-96 */
#define SCR_NOT_AVAIL   -2.0e+37
#ifndef U_ch
#define U_ch unsigned char
#endif

/* AR */
#define AR_ACCESS_ARCH_ERR 1
#define AR_ARCH_VERS_ERR   2
#define AR_ACCESS_FILE_ERR 3
#define AR_EOF_ERR         4


typedef struct _arlist_ {
    long    ar_rsize,
	    ar_psize,
	    ar_date;
    char    ar_name[51];
} ARLIST;

/* RING */
/*tampon circulaire*/
typedef struct _m_ring_
{
    int compt;      /*nb car dans tampon */
    int debut;      /*caractére à prendre */
    int taille;     /*taille tampon */
    char *tampon;   /*adresse tampon */
} M_RING;

/* =========== FUNCTIONS Pci*() ========================= */

#define MAXFDISKS  26

typedef struct _pcfdisk_ {
    char            drive;   /* Lettre identifiant le drive */
    unsigned long   total;   /* Espace total du disque (bytes) */
    unsigned long   free;    /* Espace restant sur le disque (bytes) */
    unsigned long   KBtotal; /* Espace total du disque (Kbytes) */
    unsigned long   KBfree;  /* Espace restant sur le disque (Kbytes) */
} PCFDISK;

typedef struct _pcinfos_ {
    short   fpu;                /* 1 si un processeur math. est présent */
    short   nbfdisks;           /* Nbre de disques logiques (fixes) */
    PCFDISK fdisks[MAXFDISKS];  /* Infos sur chaque disque */
    short   lpts[3];            /* Etat de chaque imprimante paralléle */
    short   nbcoms;             /* Nombre de portes séries COMx */
} PCINFOS;

/* === ALLOC DOC === */
typedef struct _allocdoc_ { // JMP 23/8/2102
    struct _allocdoc_   *prev,
			*next;
    char                *file;
    int                 line;
    unsigned long       size;
    //int                 freed; // JMP 15/2/2013
} ALLOCDOC;

extern ALLOCDOC *SCR_ALLOCDOC_LAST;   /* JMP 23-08-2012 */
extern int SCR_ALLOC_DOC;             /* JMP 23-08-2012 */


/* ==== VARS ======*/

extern char    SCR_NAME_ERR[];
extern char    SCR_BASE_PATH[];
extern char    *DebugFilename;
extern char    PC_SEPS[], PC_BEG[];
extern int      SCR_ERR_FILE_NB;            /* JMP 16-04-95 */
extern long     SCR_ERR_FILE_POS;           /* JMP 16-04-95 */

extern int AR_BUFSIZE, AR_ERRNO;

extern int  SCR_ADD_PTR_CHUNCK;             /* JMP 13-11-2012 */

#ifdef SCRCPP
}
#endif

/* ==== FNS ====== */


#ifndef SCRPROTO
extern U_ch  *SCR_mtov(),
	     *SCR_mtovs(),
	     **SCR_vtom(),
	     **SCR_vtoms(),
	     **SCR_vtoms2(),
	     **SCR_vtoms3(),
	     **SCR_vtoms2A(),
	     **SCR_copy_tbl(),
	     **SCR_text(),
	     *SCR_strip(),
	     *SCR_stripdc(),
	     *SCR_lower(),
	     *SCR_upper(),
	     *SCR_pad(),
	     *SCR_zstrip(),
	     *SCR_pstrip(),
	     *SCR_sqz(),
	     *SCR_asqz(),
	     *SCR_stracpy(),
	     *SCR_stracat(),
	     *SCR_strafcat(),
	     *SCR_strfacpy(),
	     *U_rjust_text(),
	     *U_ljust_text(),
	     *U_sqz_text(),
	     *U_shift_text(),
	     *U_center_text(),
	     *SCR_strlcpy(),
	     *SCR_strrcpy(),
	     *SCR_replace(),
	     *SCR_replace_word(),
	     *SCR_replace_gnl(),
	     *SCR_areplace(),
	     *SCR_areplace_gnl(),
	     *SCR_AnsiToOem(), /* JMP 31-01-96 */
	     *SCR_OemToAnsi(), /* JMP 31-01-96 */
	     *SCR_AnsiToOemLg(), /* JMP 01-01-99 */
	     *SCR_OemToAnsiLg(), /* JMP 01-01-99 */
	     *SCR_std_filename(),
	     *SCR_unix_filename(),
	     *SCR_LoadRawFile(),
	     *SCR_change_char(),
         *SCR_OemToUTF8();

extern U_ch  *SCR_fmt_long(),
	     *SCR_ufmt_text(),
	     *SCR_fmt_text(),
	     *SCR_fmt_dbl(),
	     *SCR_inter_list(),
	     *SCR_union_list(),
	     *SCR_dif_list(),
	     **SCR_inter(),
	     **SCR_union(),
         **SCR_union_quick(),
	     **SCR_dif();

extern char  **PC_infos(),
	     **PC_list();

long            SCR_FindStringPos();        /* JMP 17-11-96 */
unsigned long   SCR_ufmt_long();
extern long     SCR_get_app_pos();          /* JMP 16-04-95 */
extern long     SCR_get_app_size();         /* JMP 23-12-97 */

/* FILES */
extern char     *SCR_search_path(),
		*SCR_search_file(),
		*SCR_err_txt(),
		*SCR_get_err_txt(),
		*SCR_err_txt_lang(),
		*SCR_get_err_txt_lang(),
		*SCR_err_txt_t(),
		*SCR_get_err_txt_t(),
		*SCR_err_txt_t_lang(),
		*SCR_get_err_txt_t_lang(),
		*SCR_change_ext(),
		*SCR_cat_ext(),
		*SCR_fullpath(),
		*SCR_getcwd(),
		* (*SCR_err_txt_super)(int val);

/* ALLOC */
extern char     *SCR_malloc_doc();
extern char     *SCR_realloc_doc();

extern char     *SCR_malloc_orig();
extern char     *SCR_realloc_orig();

extern char     *SCR_malloc_chk();
extern char     *SCR_realloc_chk();

extern char     *SCR_palloc();
extern char     *SCR_palloca();
extern long     SCR_free_mem();

/* AR */
extern char *AR_error();
ARLIST      **AR_list();

/* STD */
extern char *STD_unit();

/* RING */
extern M_RING *M_RING_init();
extern U_ch  M_RING_read();

#else

extern int Debug(char *, ...);
extern int DebugForce(char *, ...);
extern int DebugB(char *, ...);
extern int DebugE(char *, ...);
extern int DebugEn(char *, ...);
extern int DebugET(char *, ...);
extern int DebugHexa(char *title, unsigned char *str, int lg);
extern int DebugFile(char* filename, char* txt, char* a, char* b, char* c, char* d, char* e);
extern int DebugFileHexa(char *filename, char *title, unsigned char *str, int lg);

#include <s_prost.h>

#endif /* SCRPROTO */

#include "s_alldoc.h"

#endif /* _S_STRS_ */
