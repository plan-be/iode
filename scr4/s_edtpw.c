#ifdef SCRW32
#include "scr4w.h"

TP_edit_window(tp)
TPAGES  *tp;
{
    return(WscrEditTabbedPages(0L, tp)); /* JMP 23-08-97 */
}

#endif





