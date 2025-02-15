#include <ctype.h>
#include "s_filein.h"
#include "s_file.h"

#define EATAB {while (isascii((unsigned char) *l) && isspace((unsigned char) *l))  ++l;}


int FI_getvalue();
int FI_hextoint();
char *FI_getstr();
int FI_parse();

int maxmagic = 0, maxext = 0, FI_startext = 0;

int FI_apprentice(tbl)
char    **tbl;
{
    char line[BUFSIZ+1];
    int i, errs = 0;

    maxmagic = maxext = MAXMAGIS;
    if ((FI_magic = (struct FI_magic *) calloc(sizeof(struct FI_magic), maxmagic)) == NULL
	||
	(FI_ext = (struct FI_ext *) calloc(sizeof(struct FI_ext), maxmagic)) == NULL
	)    {
	(void) fprintf(stderr, "Out of memory.\n");
	exit(1);
    }

    /* parse it */
    for (i = 0; tbl[i] != NULL; i++) {
	strcpy(line, tbl[i]);
	if(line[0] == '.') {
	    if (FI_parse_ext(i, line, &FI_next) != 0) ++errs;
	}
	else
	    if (FI_parse(i, line, &FI_nmagic) != 0) ++errs;
    }

    return errs ? -1 : 0;
}

/*
 * extend the sign bit if the comparison is to be signed
 */
unsigned long FI_signextend(m, v)
struct FI_magic *m;
unsigned long v;
{
	if (!(m->flag & UNSIGNED))
		switch(m->type) {
		/*
		 * Do not remove the casts below.  They are
		 * vital.  When later compared with the data,
		 * the sign extension must have happened.
		 */
		case BYTE:
			v = (char) v;
			break;
		case SHORT:
		case BESHORT:
		case LESHORT:
			v = (short) v;
			break;
		case DATE:
		case BEDATE:
		case LEDATE:
		case LONG:
		case BELONG:
		case LELONG:
			v = (long) v;
			break;
		case STRING:
			break;
		default:
			return -1;
		}
	return v;
}

/*
 * parse one line from magic file, put into magic[index++] if valid
 */
int FI_parse(nb, l, ndx)
int     nb;
char    *l;
int *   ndx;
{
	int i = 0, nd = *ndx;
	struct FI_magic *m;
	char *t, *s;

	if (nd+1 >= maxmagic){
	    maxmagic += 20;
	    if ((FI_magic = (struct FI_magic *) realloc(FI_magic,
				    sizeof(struct FI_magic) *
				    maxmagic)) == NULL) {
		(void) fprintf(stderr, "Out of memory.\n");
		exit(1);
	    }
	}
	m = &FI_magic[*ndx];
	m->fm_nb = nb;
	m->flag = 0;
	m->cont_level = 0;

	while (*l == '>') {
		++l;            /* step over */
		m->cont_level++;
	}

	if (m->cont_level != 0 && *l == '(') {
		++l;            /* step over */
		m->flag |= INDIR;
	}

	/* get offset, then skip over it */
	m->offset = (int) strtol(l,&t,0);
	if (l == t)
		fprintf(stderr, "offset %s invalid", l);
	l = t;

	if (m->flag & INDIR) {
		m->in.type = LONG;
		m->in.offset = 0;
		/*
		 * read [.lbs][+-]nnnnn)
		 */
		if (*l == '.') {
			switch (*++l) {
			case 'l':
				m->in.type = LONG;
				break;
			case 's':
				m->in.type = SHORT;
				break;
			case 'b':
				m->in.type = BYTE;
				break;
			default:
				fprintf(stderr, "indirect offset type %c invalid", *l);
				break;
			}
			l++;
		}
		s = l;
		if (*l == '+' || *l == '-') l++;
		if (isdigit((unsigned char)*l)) {
			m->in.offset = strtol(l, &t, 0);
			if (*s == '-') m->in.offset = - m->in.offset;
		}
		else
			t = l;
		if (*t++ != ')')
			fprintf(stderr, "missing ')' in indirect offset");
		l = t;
	}


	while (isascii((unsigned char)*l) && isdigit((unsigned char)*l))
		++l;
	EATAB;

#define NBYTE           4
#define NSHORT          5
#define NLONG           4
#define NSTRING         6
#define NDATE           4
#define NBESHORT        7
#define NBELONG         6
#define NBEDATE         6
#define NLESHORT        7
#define NLELONG         6
#define NLEDATE         6

	if (*l == 'u') {
		++l;
		m->flag |= UNSIGNED;
	}

	/* get type, skip it */
	if (strncmp(l, "byte", NBYTE)==0) {
		m->type = BYTE;
		l += NBYTE;
	} else if (strncmp(l, "short", NSHORT)==0) {
		m->type = SHORT;
		l += NSHORT;
	} else if (strncmp(l, "long", NLONG)==0) {
		m->type = LONG;
		l += NLONG;
	} else if (strncmp(l, "string", NSTRING)==0) {
		m->type = STRING;
		l += NSTRING;
	} else if (strncmp(l, "date", NDATE)==0) {
		m->type = DATE;
		l += NDATE;
	} else if (strncmp(l, "beshort", NBESHORT)==0) {
		m->type = BESHORT;
		l += NBESHORT;
	} else if (strncmp(l, "belong", NBELONG)==0) {
		m->type = BELONG;
		l += NBELONG;
	} else if (strncmp(l, "bedate", NBEDATE)==0) {
		m->type = BEDATE;
		l += NBEDATE;
	} else if (strncmp(l, "leshort", NLESHORT)==0) {
		m->type = LESHORT;
		l += NLESHORT;
	} else if (strncmp(l, "lelong", NLELONG)==0) {
		m->type = LELONG;
		l += NLELONG;
	} else if (strncmp(l, "ledate", NLEDATE)==0) {
		m->type = LEDATE;
		l += NLEDATE;
	} else {
		fprintf(stderr, "type %s invalid", l);
		return -1;
	}
	/* New-style anding: "0 byte&0x80 =0x80 dynamically linked" */
	if (*l == '&') {
		++l;
		m->mask = FI_signextend(m, strtol(l, &l, 0));
	} else
		m->mask = ~0L;
	EATAB;

	switch (*l) {
	case '>':
	case '<':
	/* Old-style anding: "0 byte &0x80 dynamically linked" */
	case '&':
	case '^':
	case '=':
		m->reln = *l;
		++l;
		break;
	case '!':
		if (m->type != STRING) {
			m->reln = *l;
			++l;
			break;
		}
		/* FALL THROUGH */
	default:
		if (*l == 'x' && isascii((unsigned char)l[1]) &&
		    isspace((unsigned char)l[1])) {
			m->reln = *l;
			++l;
			goto GetDesc;   /* Bill The Cat */
		}
		m->reln = '=';
		break;
	}
	EATAB;

	if (FI_getvalue(m, &l))
		return -1;
	/*
	 * TODO finish this macro and start using it!
	 * #define offsetcheck {if (offset > HOWMANY-1)
	 *      magwarn("offset too big"); }
	 */

	/*
	 * now get last part - the description
	 */
GetDesc:
	EATAB;
	if (l[0] == '\b') {
		++l;
		m->nospflag = 1;
	} else if ((l[0] == '\\') && (l[1] == 'b')) {
		++l;
		++l;
		m->nospflag = 1;
	} else
		m->nospflag = 0;
	while ((m->desc[i++] = *l++) != '\0' && i<MAXDESC)
		/* NULLBODY */;

	++(*ndx);               /* make room for next */
	return 0;
}

/*
 * Read a numeric value from a pointer, into the value union of a magic
 * pointer, according to the magic type.  Update the string pointer to point
 * just after the number read.  Return 0 for success, non-zero for failure.
 */
int FI_getvalue(m, p)
struct FI_magic *m;
char **p;
{
    int slen;

    if (m->type == STRING) {
	*p = FI_getstr(*p, m->value.s, sizeof(m->value.s), &slen);
	m->vallen = slen;
    } else
	if (m->reln != 'x')
	    m->value.l = FI_signextend(m, strtol(*p, p, 0));
    return 0;
}

/*
 * Convert a string containing C character escapes.  Stop at an unescaped
 * space or tab.
 * Copy the converted version to "p", returning its length in *slen.
 * Return updated scan pointer as function result.
 */
char *FI_getstr(s, p, plen, slen)
register char   *s;
register char   *p;
int     plen, *slen;
{
    char    *origs = s, *origp = p;
    char    *pmax = p + plen - 1;
    register int    c;
    register int    val;

    while ((c = *s++) != '\0') {
	if (isspace((unsigned char) c)) break;
	if (p >= pmax) {
	    fprintf(stderr, "String too long: %s\n", origs);
	    break;
	}
	if(c == '\\') {
	    switch(c = *s++) {
		case '\0': goto out;
		default  :  *p++ = (char) c;
			    break;
		case 'n':   *p++ = '\n';
			    break;
		case 'r':   *p++ = '\r';
			    break;
		case 'b':   *p++ = '\b';
			    break;
		case 't':   *p++ = '\t';
			    break;
		case 'f':   *p++ = '\f';
			    break;

		case 'v':   *p++ = '\v';
			    break;

	       /* \ and up to 3 octal digits */
	       case '0':
	       case '1':
	       case '2':
	       case '3':
	       case '4':
	       case '5':
	       case '6':
	       case '7': val = c - '0';
			 c = *s++;  /* try for 2 */
			    if(c >= '0' && c <= '7') {
				    val = (val<<3) | (c - '0');
				    c = *s++;  /* try for 3 */
				    if(c >= '0' && c <= '7')
					    val = (val<<3) | (c-'0');
				    else
					    --s;
			    }
			    else
				    --s;
			    *p++ = (char)val;
			    break;

		    /* \x and up to 3 hex digits */
	      case 'x':
			    val = 'x';      /* Default if no digits */
			    c = FI_hextoint(*s++);     /* Get next char */
			    if (c >= 0) {
				    val = c;
				    c = FI_hextoint(*s++);
				    if (c >= 0) {
					    val = (val << 4) + c;
					    c = FI_hextoint(*s++);
					    if (c >= 0) {
						    val = (val << 4) + c;
					    } else
						    --s;
				    } else
					    --s;
			    } else
				    --s;
			    *p++ = (char)val;
			    break;
		    }
	    } else
		    *p++ = (char)c;
    }
out:
    *p = '\0';
    *slen = p - origp;
    return s;
}


/* Single hex char to int; -1 if not a hex char. */
int FI_hextoint(c)
int c;
{
	if (!isascii((unsigned char) c))        return -1;
	if (isdigit((unsigned char) c))         return c - '0';
	if ((c>='a')&&(c<='f')) return c + 10 - 'a';
	if ((c>='A')&&(c<='F')) return c + 10 - 'A';
				return -1;
}

int FI_parse_ext(nb, l, ndx)
int     nb;
char    *l;
int     *ndx;
{
    int i = 0, nd = *ndx;
    struct FI_ext *e;
    char *t, *s;

    if (nd + 1 >= maxext) {
	maxext += 20;
	if ((FI_ext = (struct FI_ext *) realloc(FI_ext,
				    sizeof(struct FI_ext) * maxext)) == NULL) {
		(void) fprintf(stderr, "Out of memory.\n");
		exit(1);
	}
    }

    e = &FI_ext[*ndx];
    e->fe_nb = nb;
    for(i = 0; i < MAXEXT && !isspace(*l); i++) {
	e->fe_ext[i] = *l;
	l++;
    }
    e->fe_ext[i] = '\0';

    while(isspace(*l)) l++;

    for(i = 0; i < MAXDESC && *l != '\0'; i++) {
	e->fe_desc[i] = *l;
	l++;
    }
    e->fe_desc[i] = '\0';

    ++(*ndx);               /* make room for next */
    return(0);
}



