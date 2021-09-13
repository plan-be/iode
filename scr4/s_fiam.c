#include <ctype.h>
#include "s_filein.h"
#include "s_file.h"

/* these types are used to index the table 'types': keep em in sync! */
#define L_C     0               /* first and foremost on UNIX */
#define L_FORT  1               /* the oldest one */
#define L_MAKE  2               /* Makefiles */
#define L_PLI   3               /* PL/1 */
#define L_MACH  4               /* some kinda assembler */
#define L_ENG   5               /* English */
#define L_PAS   6               /* Pascal */
#define L_MAIL  7               /* Electronic mail */
#define L_NEWS  8               /* Usenet Netnews */

char *FI_types[] = {
	"c program text",
	"fortran program text",
	"make commands text" ,
	"pl/1 program text",
	"assembler program text",
	"English text",
	"pascal program text",
	"mail text",
	"news text",
	"can't happen error on names.h/types",
	0};

static struct names {
	char *name;
	short type;
} names[] = {
	/* These must be sorted by eye for optimal hit rate */
	/* Add to this list only after substantial meditation */
	{"/*",          L_C},   /* must preced "The", "the", etc. */
	{"#include",    L_C},
	{"char",        L_C},
	{"The",         L_ENG},
	{"the",         L_ENG},
	{"double",      L_C},
	{"extern",      L_C},
	{"float",       L_C},
	{"real",        L_C},
	{"struct",      L_C},
	{"union",       L_C},
	{"CFLAGS",      L_MAKE},
	{"LDFLAGS",     L_MAKE},
	{"all:",        L_MAKE},
	{".PRECIOUS",   L_MAKE},
/* Too many files of text have these words in them.  Find another way
 * to recognize Fortrash.
 */
#ifdef  NOTDEF
	{"subroutine",  L_FORT},
	{"function",    L_FORT},
	{"block",       L_FORT},
	{"common",      L_FORT},
	{"dimension",   L_FORT},
	{"integer",     L_FORT},
	{"data",        L_FORT},
#endif  /*NOTDEF*/
	{".ascii",      L_MACH},
	{".asciiz",     L_MACH},
	{".byte",       L_MACH},
	{".even",       L_MACH},
	{".globl",      L_MACH},
	{"clr",         L_MACH},
	{"(input,",     L_PAS},
	{"dcl",         L_PLI},
	{"Received:",   L_MAIL},
	{">From",       L_MAIL},
	{"Return-Path:",L_MAIL},
	{"Cc:",         L_MAIL},
	{"Newsgroups:", L_NEWS},
	{"Path:",       L_NEWS},
	{"Organization:",L_NEWS},
	{NULL,          0}
};
#define NNAMES ((sizeof(names)/sizeof(struct names)) - 1)

#define STREQ(a, b)     (*(a) == *(b) && strcmp((a), (b)) == 0)

int FI_ascmagic(cmt, buf, nbytes)
char    *cmt;
unsigned char *buf;
int     nbytes;     /* size actually read */
{
    int i, isblock, type;
    unsigned char *s;
    char nbuf[HOWMANY+1];   /* one extra for terminating '\0' */
    char *token;
    register struct names *p;

    /* these are easy, do them first */

    /*
     * for troff, look for . + letter + letter or .\";
     * this must be done to disambiguate tar archives' ./file
     * and other trash from real troff input.
     */
    if (*buf == '.') {
	    unsigned char *tp = buf + 1;

	    while (isascii(*tp) && isspace(*tp))
		    ++tp;   /* skip leading whitespace */
	    if ((isascii(*tp) && (isalnum(*tp) || *tp=='\\') &&
		isascii(*(tp+1)) && (isalnum(*(tp+1)) || *tp=='"'))) {
		    FI_strcat(cmt, "a2m, troff or preprocessor input text ");
		    return(type = FI_A2M);
	    }
    }
    if ((*buf == 'c' || *buf == 'C') &&
	isascii(*(buf + 1)) && isspace(*(buf + 1))) {
	    FI_strcat(cmt, "fortran program text ");
	    return(type = FI_FORTRAN);
    }

    /* look for tokens from names.h - this is expensive! */
    /* make a copy of the buffer here because strtok() will destroy it */
    s = (unsigned char*) memcpy(nbuf, buf, nbytes);
    s[nbytes] = '\0';
    while ((token = strtok((char*)s, " \t\n\r\f")) != NULL) {
	    s = NULL;       /* make strtok() keep on tokin' */
	    for (p = names; p < names + NNAMES; p++) {
		    if (STREQ(p->name, token)) {
			    FI_strcat(cmt, FI_types[p->type]);
			    return(type = FI_C - p->type);
		    }
	    }
    }

    for (i = 0; i < nbytes; i++) {
	    if (!isascii(*(buf+i))) return(FI_DATA);   /* not all ascii */
    }

    /* all else fails, but it is ascii... */
    FI_strcat(cmt, "ascii text");
    return(type = FI_ASCII);
}





