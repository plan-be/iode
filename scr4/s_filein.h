#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>      /* for open() */
#if defined(WATCOM) || defined(VC)
#include <sys/utime.h>
#else
#if defined(INTAU) || defined(XENIX)
#include <time.h>       /* BP_M 28-01-97 */
#else
#include <utime.h>
#endif
#endif

#define HOWMANY     256           /* how much of the file to look at */
#define MAXMAGIS    100           /* max entries in /etc/magic */
#define MAXDESC     50            /* max leng of text description */
#define MAXEXT      10            /* max leng of file extension */
#define MAXstring   32            /* max leng of "string" types */

struct FI_magic {
	int   fm_nb;
	short flag;
#define INDIR   1               /* if '>(...)' appears,  */
#define UNSIGNED 2              /* comparison is unsigned */
	short cont_level;       /* level of ">" */
	struct {
		char type;      /* byte short long */
		long offset;    /* offset from indirection */
	} in;
	long offset;            /* offset to magic number */
	unsigned char reln;     /* relation (0=eq, '>'=gt, etc) */
	char type;              /* int, short, long or string. */
	char vallen;            /* length of string value, if any */
#define                         BYTE    1
#define                         SHORT   2
#define                         LONG    4
#define                         STRING  5
#define                         DATE    6
#define                         BESHORT 7
#define                         BELONG  8
#define                         BEDATE  9
#define                         LESHORT 10
#define                         LELONG  11
#define                         LEDATE  12
	union VALUETYPE {
		unsigned char b;
		unsigned short h;
		unsigned long l;
		char s[MAXstring];
		unsigned char hs[2];    /* 2 bytes of a fixed-endian "short" */
		unsigned char hl[4];    /* 2 bytes of a fixed-endian "long" */
	} value;                /* either number or string */
	unsigned long mask;     /* mask before comparison with value */
	char nospflag;          /* supress space character */
	char desc[MAXDESC];     /* description */
};

struct  FI_ext {
    int     fe_nb;
    char    fe_ext[MAXEXT];       /* file extension */
    char    fe_desc[MAXDESC];     /* description */
};

#include <stdio.h>

struct stat;
extern unsigned long FI_signextend();

extern struct FI_magic *FI_magic;              /* array of magic entries               */
extern int              FI_nmagic;              /* number of valid magic[]s             */

extern struct FI_ext   *FI_ext;
extern int              FI_next;

#ifndef MAXPATHLEN
#define MAXPATHLEN      512
#endif

