#if defined(SCRW32) || defined(DOSW32) || defined(LINUX)
#ifdef UNIX
#include <scr4.h>
#else
#include <scr4w.h>
#endif

extern int      ISC_SOCKET;
int             ISC_COMPRESS = 1;
//int             ISC_BUF_INCR = 10242; /* JMP 13-10-01 */
int             ISC_BUF_INCR = 102400; /* BP_M 17-03-2011 13:27 */

/********** OUT **************/

extern S4C **ISC_S4C;

ISC_outbuf_add(txt, lg)
U_ch    *txt;
int     lg;
{
    int     i;

    i = ISC_check_socket(ISC_SOCKET);
    if(i < 0) return(-1);

    if(ISC_S4C[i]->outmaxlg < ISC_S4C[i]->outlg + lg) {
	ISC_S4C[i]->outbuf = SCR_realloc(ISC_S4C[i]->outbuf, 1, ISC_S4C[i]->outmaxlg, ISC_S4C[i]->outlg + lg + ISC_BUF_INCR);
	ISC_S4C[i]->outmaxlg = ISC_S4C[i]->outlg + lg + ISC_BUF_INCR;
	}

    memcpy(ISC_S4C[i]->outbuf + ISC_S4C[i]->outlg, txt, lg);
    ISC_S4C[i]->outlg += lg;
    return(0);
}

ISC_outbuf_flush()
{
    char    *outstr;
    long    outlen;
    int     rc, i;

    i = ISC_check_socket(ISC_SOCKET);
    if(i < 0) return(-1);

    if(ISC_S4C[i]->outlg == 0) return(0);
    if(ISC_COMPRESS == 0 || ISC_S4C[i]->outlg < 128) {
	/* On ne compacte pas */
	WSockBWrite(ISC_SOCKET, &(ISC_S4C[i]->outlg), sizeof(long));
	WSockBWrite(ISC_SOCKET, ISC_S4C[i]->outbuf, ISC_S4C[i]->outlg);
	rc = WSockFlush(ISC_SOCKET);
	ISC_S4C[i]->outlg = 0;
	if(rc < 0) return(rc);
	return(0);
	}
    /* Compactage */
    if(GzipEncodeStr(ISC_S4C[i]->outbuf, ISC_S4C[i]->outlg, &outstr, &outlen)) return(-1);
    outlen = -outlen;
    WSockBWrite(ISC_SOCKET, &outlen, sizeof(long));
    outlen = -outlen;
    WSockBWrite(ISC_SOCKET, outstr, outlen);
    rc = WSockFlush(ISC_SOCKET);
    SCR_free(outstr);
    ISC_S4C[i]->outlg = 0;
    if(rc < 0) return(rc);
    return(0);
}

ISC_write_rec(buf, lg)
char    *buf;
int     lg;
{
    long    llg = lg;

    ISC_outbuf_add(&llg, sizeof(long));
    ISC_outbuf_add(buf, lg);
    return(0);
/*
    int     rc;

    rc = WSockBWriteRecord(ISC_SOCKET, buf, lg);
    if(rc < 0) return(rc);
    return(0);
*/
}

ISC_write_lg_string(buf)
char    *buf;
{
    int     rc, lg;

    if(buf == 0) lg = 0;
    else lg = strlen(buf) + 1;

    ISC_write_rec(buf, lg);
    return(0);
/*
    int     rc, lg;

    if(buf == 0) lg = 0;
    else lg = strlen(buf) + 1;
    return(ISC_write_rec(buf, lg));
*/
}

ISC_send_rc(rc)
int     rc;
{
    short   src = rc;

    ISC_outbuf_add(&src, sizeof(short));
    return(0);
}

ISC_send_long(lng)
long        lng;
{
    ISC_outbuf_add(&lng, sizeof(long));
    return(0);
}

ISC_printf(format, a, b, c, d, e, f, g, h)
char    *format, *a, *b, *c, *d, *e, *f, *g, *h;
{
    char    buf[1024];
    long    llg;

    sprintf(buf, format, a, b, c, d, e, f, g, h);
    ISC_write_lg_string(buf);
    return(0);
}

/********** IN **************/


ISC_inbuf_read()
{
    long    lg, rlg;
    int     rc, i;
    extern  long last_check_live;

    /* BP_M 09-12-2010 15:43 */
    // Le contrôle du socket a été déplacé en début de fonction
    i = ISC_check_socket(ISC_SOCKET);
    if(i < 0) {
	Debug("ISC_inbuf_read: ISC_check_socket = %d error\n", i);
	return(-1);
    }

    rc = ISC_outbuf_flush();
    if(rc) {
	Debug("ISC_inbuf_read: ISC_outbuf_flush = %d\n", rc);
	return(-1);
    }
    rc = WSockReadLgBytes(ISC_SOCKET, &lg, sizeof(long));
    if(rc < 0) {
//      Debug("ISC_inbuf_read: WSockReadLgBytes = %d\n", rc);
	return(rc);
    }
//  else
//      Debug("ISC_inbuf_read: lecture de '%ld' bytes\n", lg);
    /* Replacer les data au début de INBUF */
    if(ISC_S4C[i]->inpos > 0) memcpy(ISC_S4C[i]->inbuf, ISC_S4C[i]->inbuf + ISC_S4C[i]->inpos, ISC_S4C[i]->inlg - ISC_S4C[i]->inpos);
    ISC_S4C[i]->inlg -= ISC_S4C[i]->inpos;
    ISC_S4C[i]->inpos = 0;

    /* Ajouter éventuellement l'espace dans INBUF */
    rlg = lg;
    if(rlg < 0) rlg = -lg;
    if(ISC_S4C[i]->inmaxlg < rlg + ISC_S4C[i]->inlg) {
	ISC_S4C[i]->inbuf = SCR_realloc(ISC_S4C[i]->inbuf, 1, ISC_S4C[i]->inmaxlg, ISC_S4C[i]->inlg + rlg + ISC_BUF_INCR);
	ISC_S4C[i]->inmaxlg = ISC_S4C[i]->inlg + rlg + ISC_BUF_INCR;
    }

    /* Lire les data */
    if(lg >= 0) {
	rc = WSockReadLgBytes(ISC_SOCKET, ISC_S4C[i]->inbuf + ISC_S4C[i]->inlg, (int)rlg);
	ISC_S4C[i]->inlg += rlg;
    }
    else {
	long    len;

	if(ISC_S4C[i]->incmaxlg < rlg) {
	    SCR_free(ISC_S4C[i]->inbufc);
	    ISC_S4C[i]->incmaxlg = rlg * 4;
	    ISC_S4C[i]->inbufc = SCR_malloc(ISC_S4C[i]->incmaxlg);
	}
	rc = WSockReadLgBytes(ISC_SOCKET, ISC_S4C[i]->inbufc, (int)rlg);
	len = *(long *)ISC_S4C[i]->inbufc;
	if(ISC_S4C[i]->inmaxlg < len + ISC_S4C[i]->inlg) {
	    ISC_S4C[i]->inbuf = SCR_realloc(ISC_S4C[i]->inbuf, 1, ISC_S4C[i]->inmaxlg, ISC_S4C[i]->inlg + len + ISC_BUF_INCR);
	    ISC_S4C[i]->inmaxlg = ISC_S4C[i]->inlg + len + ISC_BUF_INCR;
	}
	if(GzipDecodeStrNA(ISC_S4C[i]->inbufc, rlg, ISC_S4C[i]->inbuf + ISC_S4C[i]->inlg)) return(-1);
	ISC_S4C[i]->inlg += len;
    }

    if(rc < 0) {
//      Debug("ISC_inbuf_read: 2 - WSockReadLgBytes = %d\n", rc);
	return(rc);
    }
    last_check_live = WscrGetMS();
    return(0);
}

ISC_inbuf_read_lg_bytes(buf, lg)
char    *buf;
int     lg;
{
    int     i, rc;

    i = ISC_check_socket(ISC_SOCKET);
    if(i < 0) {
//      Debug("ISC_inbuf_read_lg_bytes: socket error %d\n", i);
	return(-1);
    }
    if(ISC_S4C[i]->inlg < ISC_S4C[i]->inpos + lg) {
	rc = ISC_inbuf_read();
	if(rc) {
//          Debug("ISC_inbuf_read_lg_bytes: ISC_inbuf_read = %d\n", rc);
	    return(rc);
	}
    }
    memcpy(buf, ISC_S4C[i]->inbuf + ISC_S4C[i]->inpos, lg);
    ISC_S4C[i]->inpos += lg;
    return(0);
}

ISC_read_rec(buf)
char    *buf;
{
    int     rc;
    long    llg;

    rc = ISC_inbuf_read_lg_bytes((char *)&llg, sizeof(long));
    if(rc < 0) return(rc);
    if(llg == 0) return(0);
    rc = ISC_inbuf_read_lg_bytes(buf, (int)llg);
    if(rc < 0) return(rc);
    return(0);
}

ISC_read_rec_alloc(buf)
char    **buf;
{
    int     rc;
    long    llg;

    *buf = 0;
    rc = ISC_inbuf_read_lg_bytes((char *)&llg, sizeof(long));
    if(rc < 0) return(rc);
    if(llg == 0) return(0);
    *buf = SCR_malloc((int)llg);
    rc = ISC_inbuf_read_lg_bytes(*buf, (int)llg);
    if(rc < 0) return(rc);
    return(0);
}

ISC_read_rc()
{
    int     rc;
    short   src;

    rc = ISC_inbuf_read_lg_bytes((char *)&src, sizeof(short));
    if(rc < 0) return(rc);
    return((int)src);
}

long ISC_read_long()
{
    int     rc;
    long    lng = 0;

    rc = ISC_inbuf_read_lg_bytes((char *)&lng, sizeof(long));
    if(rc < 0) return(rc);
    return(lng);
}

ISC_read_cmd(args)
char    *args;
{
    char    txt[1024];
    int     pos, cmd, rc;

    rc = ISC_read_rec(txt);
    if(rc < 0) return(rc);
    cmd = atoi(txt);
    args[0] = 0;
    pos = U_pos(' ', txt);
    if(pos > 0) strcpy(args, txt + pos + 1);
    return(cmd);
}

/****** IN/OUT ********/

ISC_cmd(format, a, b, c, d, e, f, g, h)
char    *format, *a, *b, *c, *d, *e, *f, *g, *h;
{
    int     rc;

    rc = ISC_printf(format, a, b, c, d, e, f, g, h);
    if(rc < 0) return(rc);
    return(ISC_read_rc());
}

#endif











