// #if defined(DOSWIN) && !defined(WINDOWS) /* JMP 02-12-99 */
#if defined(DOSWIN) && !defined(WINDOWS) /* JMP 21-10-2012 */
#include <windows.h>
#define SCRSUPPROTO
#include "scr4.h" 
  
extern HWND hWndDOS;
extern int  W_PRINTF_USED;          /* JMP 05-06-96 */

extern int W_PRINTF_BUF; /* JMP 30-09-98 */

WprintfNl(line, col)
int     *line, *col;
{
    char    buf2[256];
    U_ch    *cl = (U_ch  *)(SCR_SCREEN->im_cells);

    if(*line >= SCR_PAGE_SIZE[0] - 1) {
        memcpy(cl, cl + 2 * SCR_PAGE_SIZE[1], (SCR_PAGE_SIZE[0] - 1) * 2 * SCR_PAGE_SIZE[1]);
        *line = SCR_PAGE_SIZE[0] - 1;
        memset(buf2, ' ', 255);
        SCR_write_scr_mem(cl, *line, 0, buf2, SCR_PAGE_SIZE[1], 1);
        InvalidateRect(hWndDOS, 0L, FALSE);
        SCR_wdispatch();
    }
    else (*line)++;
    *col = 0;
    return(0);
}

Wprintf(a, b, c, d, e, f, g, h, j, k, l, m, n, o)
char    *a, *b, *c, *d, *e, *f, *g, *h, *j, *k, *l, *m, *n, *o;
{
    char    buf[1024];                /* JMP 02-09-98 */
    static  int line = 0, col = 0;
    U_ch    *cl;
    int     i;

    if(hWndDOS == 0) return(0);

    W_PRINTF_USED = 1;          /* JMP 05-06-96 */
    if(SCR_SCREEN == 0)
        SCR_SCREEN = SCR_alloc_image(SCR_PAGE_SIZE[0], SCR_PAGE_SIZE[1]);

    cl = (U_ch  *)(SCR_SCREEN->im_cells);

    sprintf(buf, a, b, c, d, e, f, g, h, j, k, l, m, n); /* JMP 02-12-99 */
    for(i = 0 ; buf[i] ; i++) {
        if(buf[i] == '\n') {
            WprintfNl(&line, &col);
            continue;
        }

        if(col >= SCR_PAGE_SIZE[1]) WprintfNl(&line, &col);
        SCR_write_scr_mem(cl, line, col, buf + i, 1, 1);
        col++;
        if(W_PRINTF_BUF == 0) {
            InvalidateRect(hWndDOS, 0L, FALSE);
            SCR_wdispatch();
        }
    }

    if(W_PRINTF_BUF) {
        InvalidateRect(hWndDOS, 0L, FALSE);
        SCR_wdispatch();
    }

    return(0);
}

Werror(a, b, c, d, e, f, g, h, i, j, k, l, m, n)
char    *a, *b, *c, *d, *e, *f, *g, *h, *i, *j, *k, *l, *m, *n;
{
    char    buf[1024];
    int     lg;

    sprintf(buf, a, b, c, d, e, f, g, h, i, j, k, l, m, n);
    lg = strlen(buf);
    if(lg && buf[lg - 1] == '\n') {
        buf[lg - 1] = 0;
        lg--;
    }
    SCR_OemToAnsi(buf, buf);
    MessageBox(0, buf, "SCR4/AL1", MB_OK | MB_ICONSTOP | MB_APPLMODAL);
    return(0);
}

#else
#if defined(SCRW32)
#include <windows.h>
#define SCRSUPPROTO
#include "scr4.h"

Wprintf(a, b, c, d, e, f, g, h, i, j, k, l, m, n)
char    *a, *b, *c, *d, *e, *f, *g, *h, *i, *j, *k, *l, *m, *n;
{
    printf(a, b, c, d, e, f, g, h, i, j, k, l, m, n);
    return(0);
}

Werror(a, b, c, d, e, f, g, h, i, j, k, l, m, n)
char    *a, *b, *c, *d, *e, *f, *g, *h, *i, *j, *k, *l, *m, *n;
{
    Wprintf(a, b, c, d, e, f, g, h, i, j, k, l, m, n);
    return(0);
}


#else
/* Version 16 bits ou Unix */
#define SCRSUPPROTO
#include "scr4.h"
Wprintf(a, b, c, d, e, f, g, h, i, j, k, l, m, n)
char    *a, *b, *c, *d, *e, *f, *g, *h, *i, *j, *k, *l, *m, *n;
{
    printf(a, b, c, d, e, f, g, h, i, j, k, l, m, n);
}

Werror(a, b, c, d, e, f, g, h, i, j, k, l, m, n)
char    *a, *b, *c, *d, *e, *f, *g, *h, *i, *j, *k, *l, *m, *n;
{
    Wprintf(a, b, c, d, e, f, g, h, i, j, k, l, m, n);
    return(0);
}

/* JMP 05-01-03 pour Gnome
SCR_wdispatch()
{
}
*/
#endif
#endif




















