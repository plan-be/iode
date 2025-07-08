#pragma once


/* ********************************************** *
 * copied/pasted from the /cli/s_iode.c directory *
 * ********************************************** */

#include <stdio.h>
#include <stdarg.h>

#include "scr4/scr4.h"
#include "scr4/scr4w.h"
#include "scr4/s_a2m.h"
#include "scr4/s_proa2m.h"
#include "scr4/s_probas.h"

#ifdef _MSC_VER
#include "scr4/s_prowin.h"

/* ==== Annulations fonction HWND ==== */
#include <windows.h>

HWND WscrGetMainWnd()
{
    // JMP 14/3/2017 pour nouvelle version BC32
    return (HWND)NULL;
}
#endif

// s_odbc

/* ==== Annulations fonctions GIF ==== */

int A2M_GIF_BGCOLOR[3];
int A2M_GIF_TRCOLOR[3];
int A2M_GIF_INTER;
int A2M_GIF_TRANS;
int A2M_GIF_FILLED;
int A2M_GIF_FONT;

int A2mGIF_HTML(A2MGRF *go, U_ch* filename)
{
    return 0;
}

/* ====  A2mMessage  ==== */

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

int SCR_confirme(char* str, ...)
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

int PG_display_error(char* fmt, ...)
{
    va_list args;
    
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    printf("\n");
    Iode_Continue();
    return 0;
}

int SCR_beep()
{
    return printf("Beep");
}

/* ==== Annulations fonctions de b_dde.c ==== */

int SCR_record_text(unsigned char* szBuffer)
{
    return 0;
}

int SCR_record_key(int key)
{
    return 0;
}

/* ==== Annulations fonctions ODE_settitle ==== */

void ODE_settitle() {}
