#include <scr4.h>

char *SCR_oracle_txt(tydb, txt, len)
int     tydb;
unsigned char *txt;
int     len;
{
    static  U_ch   buf2[2500];
    int     pos = 0, i, mysql = IS_MYSQL;
    U_ch    *buf;
    char    *rest;
    extern  int  SCR_TYPE_DB;  /* JMP 08-02-11 */

    if(len > 2000)
	buf = SCR_malloc(len * 2);
    else
	buf = buf2;

    // !!!!!!!!!!!!!!!!!!!!!!!
    if(tydb == ODBC) tydb = SCR_TYPE_DB;  /* JMP 08-02-11 */

    if(tydb == mysql)                               rest = "\\\"'";                  /* JMP 06-02-11 */
    else if(tydb != ODBC && tydb != CTREE4)         rest = "\\\"@#^§$*";             /* JMP 06-02-11 */
    else                                            rest = ""; // rest = "\\\"§$%";  /* JMP 06-02-11 */

    for(i = 0 ; i < len && txt[i] ; i++, pos++) {
	if(txt[i] == '\'') {    /* BP_M 26-06-2007 */
	    buf[pos] = txt[i];
	    pos++;
	}
	else if(U_is_in(txt[i], rest)) {
	    buf[pos] = '\\';
	    pos++;
	}
	buf[pos] = txt[i];
    }
    buf[pos] = 0;
    if(SCR_ISAM_ANSI) SCR_OemToAnsi(buf, buf); /* JMP 01-01-99 */
    return(buf);
}

extern int SCR_TYPE_DB; /* BP_M 24-10-2002 */

IS_create_write_buf(is, txt, buf2, type, len, sqz, noquote)
ISAM    *is;
char    *txt, *buf2;
int     len, sqz, noquote;
{
    char    fmt[20], *bufa;
    short   s;
    long    l;
    float   f;
    double  d;

    switch(type) {
	case SCR_ZSTRING:
	case SCR_STRING:
	    txt[len] = 0;
	    if(IS_STRIP_STRING) SCR_strip(txt); /* JMP 29-12-01 */
	    if(sqz) {
		SCR_compress(buf2, txt, len);
		memcpy(txt, buf2, len); txt[len] = 0;
		SCR_strip(txt);
	    }
	    bufa = SCR_oracle_txt(is->is_type_dbd, txt, len);
	    if(noquote)
		sprintf(buf2, "%s", bufa);
	    else if(is->is_type_dbd == POSTGRES)
		sprintf(buf2, "E'%s'", bufa);
	    else
		sprintf(buf2, "'%s'", bufa);
	    if(len > 2000) SCR_free(bufa);
	    break;

	case SCR_EDITOR:  /* JMP 29-12-98 */
	    if(txt == 0) {
		strcpy(buf2, "''");
		break;
		}
	    SCR_strip(txt);
	    len = strlen(txt);
	    bufa = SCR_oracle_txt(is->is_type_dbd, txt, len);
	    if(noquote)
		sprintf(buf2, "%s", bufa);
	    else if(is->is_type_dbd == POSTGRES)
		sprintf(buf2, "E'%s'", bufa);
	    else
		sprintf(buf2, "'%s'", bufa);
	    if(len > 2000) SCR_free(bufa);
	    break;

	case SCR_CHAR:
	    if(is->is_type_dbd == IS_MYSQL)
		sprintf(buf2, "%d", txt[0]);
	    else {
		if(txt[0] < ' ') { /* BP_M 23-02-1999 */
		    if(is->is_type_dbd == INFORMIX_DB || is->is_type_dbd == POSTGRES || SCR_TYPE_DB == 2) {
			if(noquote)
			    sprintf(buf2, " ");
			else
			    sprintf(buf2, "' '");
		    }
		    else
			sprintf(buf2, "%d", txt[0]);
		}
		else {
		    if(noquote)
			sprintf(buf2, "%c", txt[0]);
		    else
			sprintf(buf2, "'%c'", txt[0]);
		}
	    }
	    break;  /* BP_M 29-05-99 */

	case SCR_SHORT1:
	    sprintf(buf2, "%d", txt[0]);
	    break;

	case SCR_CHECK_BOX :
	case SCR_TYPE_RADIO:
	case SCR_MENU:
	case SCR_SHORT:
	    memcpy(&s, txt, sizeof(short));
	    sprintf(buf2, "%d", (int)s);
	    break;
	case SCR_DATE:
	    memcpy(&l, txt, sizeof(long));
	    sprintf(buf2, "%08ld", l); buf2[8] = 0;
	    break;
	case SCR_TIME:
	    l = 0;
	    memcpy(&l, txt, sizeof(long));
	    sprintf(buf2, "%04ld", l);
	    break;
	case SCR_LONG:
	case SCR_AUTO:
	case SCR_REF     :
	case SCR_VAR     :
	    memcpy(&l, txt, sizeof(long));
	    sprintf(buf2, "%ld", l);
	    break;
	case SCR_FLOAT:
	    memcpy(&f, txt, sizeof(float));
	    sprintf(buf2, "%.7e", f); /* JMP 29-12-01 */
	    break;
	case SCR_DOUBLE:
	    memcpy(&d, txt, sizeof(double));
	    sprintf(buf2, "%.14le", d); /* JMP 29-12-01 */
	    break;
    }
    return(strlen(buf2));
}


