// copied/pasted from the /cli directory

/************ Fonctions annul�es ou a remplacees ************************/

#include <stdio.h>

/*========================== Annulations fonction HWND =================== */
#include <windows.h>

HWND  WscrGetMainWnd()
{
    // JMP 14/3/2017 pour nouvelle version BC32
    return((HWND)NULL);
}


int Wprintf(a, b, c, d, e, f, g, h, i, j, k, l, m, n)
char* a, * b, * c, * d, * e, * f, * g, * h, * i, * j, * k, * l, * m, * n;
{
    printf(a, b, c, d, e, f, g, h, i, j, k, l, m, n);
    return(0);
}

int SCR_panic()
{
    fprintf(stderr, "Memory full. exit(2)\n");
    exit(2);
}

// s_odbc
int ODebugDetail = 0;

#ifdef IODEWIN
/*========================== Annulations fonctions GIF =========================*/

int A2M_GIF_BGCOLOR[3];
int A2M_GIF_TRCOLOR[3];
int A2M_GIF_INTER;
int A2M_GIF_TRANS;
int A2M_GIF_FILLED;
int A2M_GIF_FONT;

A2mGIF_HTML()
{
    return(0);
}


/*==========================  A2mMessage  ========================================*/
A2mMessage(char* msg)
{
    fprintf(stderr, "%-79.79s\n", msg);
}

int     IODE_FORCEYES = 0;
Iode_Continue()
{
    if (!IODE_FORCEYES) {
        printf(" -- Press Enter to continue --");
        getchar();
    }
}

/*========================== Annulations fonctions de k_sim.c ================== */

int SCR_term_vkey(ovtime)
{
}

int SCR_hit_key()
{
    return(0);
}

int SCR_get_key()
{
    return(0);
}

int SCR_confirme(char* str)
{
    printf("%s", str);
    return(1);
}


/*========================== Annulations fonctions de b_base.c =================== */


SCR_MessageBox(str, v, buts)
unsigned char* str, * v, * buts[];
{
    printf("%s\n", v);
    Iode_Continue();
}

/*========================== Annulations fonctions de b_rep.c =================== */
PG_display_error(a, b, c, d, e, f, g, h, i, j, k, l, m, n)
char* a, * b, * c, * d, * e, * f, * g, * h, * i, * j, * k, * l, * m, * n;
{
    printf(a, b, c, d, e, f, g, h, i, j, k, l, m, n);
    printf("\n");
    Iode_Continue();
    return(0);
}

SCR_beep()
{
    printf("Beep");
}

/*========================== Annulations fonctions ISC_* de s_rdini.c =================== */

FILE* ISC_fopen(char* filename, char* mode)
{
    return(fopen(filename, mode));
}

int ISC_fclose(FILE* fd)
{
    return(fclose(fd));
}
ISC_unlink(char* filename)
{
    return(unlink(filename));
}
ISC_rename(char* oldname, char* newname)
{
    return(rename(oldname, newname));
}
ISC_read_line(fd, buf, lg)
FILE* fd;
unsigned char* buf;
int             lg;
{
    return(SCR_read_line(fd, buf, lg));
}
ISC_fwrite(char* buf, int lg, int nb, FILE* fd)
{
    return(fwrite(buf, lg, nb, fd));
}

ISC_getc(FILE* fd)
{
    return(getc(fd));
}

ISC_putc(int ch, FILE* fd)
{
    return(putc(ch, fd));
}

/*========================== Annulations fonctions de b_dde.c =================== */
SCR_record_text(char* szBuffer)
{
    return(0);
}
SCR_record_key(int key)
{
    return(0);
}

/*========================== Annulations fonctions ODE_settitle =================== */

ODE_settitle()
{
}

#endif
