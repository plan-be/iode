#pragma once


/* ********************************************** *
 * copied/pasted from the /cmd/s_iode.c directory *
 * ********************************************** */

#include "../api/iode.h"
#include <stdio.h>

/* ==== Annulations fonction HWND ==== */
#include <windows.h>

HWND WscrGetMainWnd()
{
    // JMP 14/3/2017 pour nouvelle version BC32
    return (HWND)NULL;
}

int Wprintf(char* fmt, ...)
{
    return 0;
}

int SCR_panic()
{
    fprintf(stderr, "Memory full. exit(2)\n");
    exit(2);
}

// s_odbc
int ODebugDetail = 0;

#ifdef IODEWIN

/* ==== Annulations fonctions GIF ==== */

int A2M_GIF_BGCOLOR[3];
int A2M_GIF_TRCOLOR[3];
int A2M_GIF_INTER;
int A2M_GIF_TRANS;
int A2M_GIF_FILLED;
int A2M_GIF_FONT;

int A2mGIF_HTML()
{
    return 0;
}

/* ====  A2mMessage  ==== */

int A2mMessage(char* msg)
{
    return fprintf(stderr, "%-79.79s\n", msg);
}

int IODE_FORCEYES = 0;

void Iode_Continue()
{
    if (!IODE_FORCEYES)
    {
        printf(" -- Press Enter to continue --");
        getchar();
    }
}

/* ==== Annulations fonctions de k_sim.c ==== */

int SCR_term_vkey(int ovtime)
{
    return 0;
}

int SCR_hit_key()
{
    return 0;
}

int SCR_get_key()
{
    return 0;
}

int SCR_confirme(char* str)
{
    return printf("%s", str);
}

/* ==== Annulations fonctions de b_base.c ==== */

int SCR_MessageBox(unsigned char* str, unsigned char* v, unsigned char* buts[])
{
    int res = printf("%s\n", v);
    Iode_Continue();
    return res;
}

/* ==== Annulations fonctions de b_rep.c ==== */

int PG_display_error(char* a, char* b, char* c, char* d, char* e, char* f, char* g, char* h, char* i, char* j, char* k, char* l, char* m, char* n)
{
    printf(a, b, c, d, e, f, g, h, i, j, k, l, m, n);
    printf("\n");
    Iode_Continue();
    return 0;
}

int SCR_beep()
{
    return printf("Beep");
}

/* ==== Annulations fonctions ISC_* de s_rdini.c ==== */

FILE* ISC_fopen(char* filename, char* mode)
{
    return fopen(filename, mode);
}

int ISC_fclose(FILE* fd)
{
    return fclose(fd);
}

int ISC_unlink(char* filename)
{
    return unlink(filename);
}

int ISC_rename(char* oldname, char* newname)
{
    return rename(oldname, newname);
}

int ISC_read_line(FILE* fd, unsigned char* buf, int lg)
{
    return SCR_read_line(fd, buf, lg);
}

int ISC_fwrite(char* buf, int lg, int nb, FILE* fd)
{
    return fwrite(buf, lg, nb, fd);
}

int ISC_getc(FILE* fd)
{
    return getc(fd);
}

int ISC_putc(int ch, FILE* fd)
{
    return putc(ch, fd);
}

/* ==== Annulations fonctions de b_dde.c ==== */

int SCR_record_text(char* szBuffer)
{
    return 0;
}

int SCR_record_key(int key)
{
    return 0;
}

/* ==== Annulations fonctions ODE_settitle ==== */

void ODE_settitle() {}

#endif
