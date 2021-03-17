#include <stdio.h>
#include <string.h>
#if !defined(INTAU) && !defined(XENIX)           /* BP_M 28-01-97 */
#include <time.h>
#endif                  /* BP_M 28-01-97 */

#include "s_filein.h"
#include "s_file.h"

int FI_softmagic(cmt, buf, nbytes)
char    *cmt;
unsigned char *buf;
int nbytes;
{
	return(FI_match(cmt, buf, nbytes));
}

int FI_match(cmt, s, nbytes)
char    *cmt;
unsigned char   *s;
int nbytes;
{
    int magindex = 0,
	type = FI_UNKNOWN,
	cont_level = 0,
	need_separator = 0;
    union VALUETYPE p;

    for (magindex = 0; magindex < FI_nmagic; magindex++) {
	    /* if main entry matches, print it... */
	    if (!FI_mget(&p, s, &FI_magic[magindex], nbytes) ||
		!FI_mcheck(&p, &FI_magic[magindex])) {
			/*
			 * main entry didn't match,
			 * flush its continuations
			 */
			while (magindex < FI_nmagic &&
			       FI_magic[magindex + 1].cont_level != 0)
			       magindex++;
			continue;
	    }
	    type = FI_magic[magindex].fm_nb;
	    FI_mprint(cmt, &p, &FI_magic[magindex]);
	    /*
	     * If we printed something, we'll need to print
	     * a blank before we print something else.
	     */
	    if (FI_magic[magindex].desc[0])
		    need_separator = 1;
	    /* and any continuations that match */
	    cont_level++;
	    while (FI_magic[magindex+1].cont_level != 0 &&
	       ++magindex < FI_nmagic) {
		if (cont_level >= FI_magic[magindex].cont_level) {
		    if (cont_level > FI_magic[magindex].cont_level) {
			/*
			 * We're at the end of the level
			 * "cont_level" continuations.
			 */
			cont_level = FI_magic[magindex].cont_level;
		    }
		    if (FI_mget(&p, s, &FI_magic[magindex], nbytes) &&
			FI_mcheck(&p, &FI_magic[magindex])) {
			/*
			 * This continuation matched.
			 * Print its message, with
			 * a blank before it if
			 * the previous item printed
			 * and this item isn't empty.
			 */
			/* space if previous printed */
			if (need_separator
			   && (FI_magic[magindex].nospflag == 0)
			   && (FI_magic[magindex].desc[0] != '\0')
			   ) {
				FI_strcat(cmt, " ");
				need_separator = 0;
			}
			type = FI_magic[magindex].fm_nb;
			FI_mprint(cmt, &p, &FI_magic[magindex]);
			if (FI_magic[magindex].desc[0])
				need_separator = 1;

			/*
			 * If we see any continuations
			 * at a higher level,
			 * process them.
			 */
			cont_level++;
		    }
		}
	    }
	    return(type);           /* all through */
    }
    return(type);
}

FI_mprint(cmt, p, m)
char    *cmt;
union VALUETYPE *p;
struct FI_magic *m;
{
	char *pp, *rt;
	char    buf[81];
	unsigned long v;

	switch (m->type) {
	case BYTE:
		v = p->b;
		break;

	case SHORT:
	case BESHORT:
	case LESHORT:
		v = p->h;
		break;

	case LONG:
	case BELONG:
	case LELONG:
		v = p->l;
		break;

	case STRING:
		if (m->reln == '=')  sprintf(buf, m->desc, m->value.s);
		else  sprintf(buf, m->desc, p->s);
		FI_strcat(cmt, buf);
		return(0);

	case DATE:
	case BEDATE:
	case LEDATE:
		pp = ctime((time_t*) &p->l);
		if ((rt = strchr(pp, '\n')) != NULL)
			*rt = '\0';
		sprintf(buf, m->desc, pp);
		FI_strcat(cmt, buf);
		return(0);
	}

	v = FI_signextend(m, v) & m->mask;
	sprintf(buf, m->desc, (unsigned char) v);
	FI_strcat(cmt, buf);
	return(0);
}

/*
 * Convert the byte order of the data we are looking at
 */
int FI_mconvert(p, m)
union VALUETYPE *p;
struct FI_magic *m;
{
	char *rt;

	switch (m->type) {
	case BYTE:
	case SHORT:
	case LONG:
	case DATE:
		return 1;
	case STRING:
		/* Null terminate and eat the return */
		p->s[sizeof(p->s) - 1] = '\0';
		if ((rt = strchr(p->s, '\n')) != NULL)
			*rt = '\0';
		return 1;
	case BESHORT:
		p->h = (short)((p->hs[0]<<8)|(p->hs[1]));
		return 1;
	case BELONG:
	case BEDATE:
		p->l = (long)
		    ((p->hl[0]<<24)|(p->hl[1]<<16)|(p->hl[2]<<8)|(p->hl[3]));
		return 1;
	case LESHORT:
		p->h = (short)((p->hs[1]<<8)|(p->hs[0]));
		return 1;
	case LELONG:
	case LEDATE:
		p->l = (long)
		    ((p->hl[3]<<24)|(p->hl[2]<<16)|(p->hl[1]<<8)|(p->hl[0]));
		return 1;
	}
}

int FI_mget(p, s, m, nbytes)
union VALUETYPE* p;
unsigned char   *s;
struct FI_magic *m;
int nbytes;
{
    long offset = m->offset;

    if (offset + sizeof(union VALUETYPE) > nbytes) return 0;
    memcpy(p, s + offset, sizeof(union VALUETYPE));
    if (!FI_mconvert(p, m)) return 0;

    if (m->flag & INDIR) {

	    switch (m->in.type) {
	    case BYTE:
		    offset = p->b + m->in.offset;
		    break;
	    case SHORT:
		    offset = p->h + m->in.offset;
		    break;
	    case LONG:
		    offset = p->l + m->in.offset;
		    break;
	    }

	    if (offset + sizeof(union VALUETYPE) > nbytes) return 0;
	    memcpy(p, s + offset, sizeof(union VALUETYPE));
	    if (!FI_mconvert(p, m)) return 0;
    }
    return 1;
}

int FI_mcheck(p, m)
union VALUETYPE* p;
struct FI_magic *m;
{
    unsigned long l = m->value.l;
    unsigned long v;
    int matched;

    if ( (m->value.s[0] == 'x') && (m->value.s[1] == '\0') ) return 1;

    switch (m->type) {
    case BYTE:
	    v = p->b;
	    break;

    case SHORT:
    case BESHORT:
    case LESHORT:
	    v = p->h;
	    break;

    case LONG:
    case BELONG:
    case LELONG:
    case DATE:
    case BEDATE:
    case LEDATE:
	    v = p->l;
	    break;

    case STRING:
	    l = 0;
	    /* What we want here is:
	     * v = strncmp(m->value.s, p->s, m->vallen);
	     * but ignoring any nulls.  bcmp doesn't give -/+/0
	     * and isn't universally available anyway.
	     */
	    v = 0;
	    {
		    register unsigned char *a = (unsigned char*)m->value.s;
		    register unsigned char *b = (unsigned char*)p->s;
		    register int len = m->vallen;

		    while (--len >= 0)
			    if ((v = *b++ - *a++) != 0)
				    break;
	    }
	    break;
    }

    v = FI_signextend(m, v) & m->mask;

    switch (m->reln) {
    case 'x':
	    matched = 1;
	    break;

    case '!':
	    matched = v != l;
	    break;

    case '=':
	    matched = v == l;
	    break;

    case '>':
	    if (m->flag & UNSIGNED) matched = v > l;
	    else                    matched = (long) v > (long) l;
	    break;

    case '<':
	    if (m->flag & UNSIGNED) matched = v < l;
	    else                    matched = (long) v < (long) l;
	    break;

    case '&':
	    matched = (v & l) == l;
	    break;

    case '^':
	    matched = (v & l) != l;
	    break;

    default:
	    matched = 0;
    }

    return matched;
}








