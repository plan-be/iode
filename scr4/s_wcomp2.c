#define SCRSUPPROTO
#include "scr4w.h"

SCR_confirme(text, c1, c2, c3, c4, c5, c6, c7, c8, c9, ca, cb, cc, cd)
char    *text, *c1, *c2, *c3, *c4, *c5, *c6, *c7, *c8, *c9, *ca, *cb, *cc, *cd;
{
    char    buffer[10240], title[81], *ptitle = title;  /* JMP 21-01-99 */
    HWND    hWndOwner;                                /* JMP 23-08-97 */

    if(text == 0) text = SCR_err_txt(117);   /* JMP 23-01-97 */
    sprintf(buffer, text, c1, c2, c3, c4, c5, c6, c7, c8, c9, ca, cb, cc, cd); /* BP_M 20-02-2003 15:31 */
    SCR_OemToAnsi(buffer, buffer);

    hWndOwner = GetLastActivePopup(hMainWnd);         /* JMP 23-08-97 */
    if(hWndOwner == 0) hWndOwner = hMainWnd;          /* JMP 23-08-97 */
    if(GetWindowText(hWndOwner, ptitle, 80) == 0)     /* JMP 23-08-97 */
	ptitle = WSCRMainTitle;                      /* JMP 23-08-97 */

/*    return(IDCANCEL == MessageBox(WSCR_CUR_HWND, buffer, WSCRMainTitle, MB_OKCANCEL | MB_ICONQUESTION | MB_APPLMODAL)); */
    return(IDCANCEL == MessageBox(hWndOwner, buffer, ptitle, MB_OKCANCEL | MB_ICONQUESTION | MB_APPLMODAL));
}

SCR_v3_confirme(text, c1, c2, c3, c4, c5, c6, c7, c8, c9, ca, cb, cc, cd)
char    *text, *c1, *c2, *c3, *c4, *c5, *c6, *c7, *c8, *c9, *ca, *cb, *cc, *cd;
{
    return(SCR_confirme(text, c1, c2, c3, c4, c5, c6, c7, c8, c9, ca, cb, cc, cd));
}

PG_display_error(text, c1, c2, c3, c4, c5, c6, c7, c8, c9, ca, cb, cc, cd)
char    *text, *c1, *c2, *c3, *c4, *c5, *c6, *c7, *c8, *c9, *ca, *cb, *cc, *cd;
{
    char    buffer[10240], title[81], *ptitle = title;  /* JMP 21-01-99 */
    HWND    hWndOwner;                                /* JMP 23-08-97 */

    if(text == 0) text = "";
    sprintf(buffer, text, c1, c2, c3, c4, c5, c6, c7, c8, c9, ca, cb, cc, cd);  /* BP_M 20-02-2003 15:31 */
    SCR_OemToAnsi(buffer, buffer);

    hWndOwner = GetLastActivePopup(hMainWnd);         /* JMP 23-08-97 */
    if(hWndOwner == 0) hWndOwner = hMainWnd;          /* JMP 23-08-97 */
    if(GetWindowText(hWndOwner, ptitle, 80) == 0)     /* JMP 23-08-97 */
	ptitle = WSCRMainTitle;;                      /* JMP 23-08-97 */
    MessageBox(hWndOwner, buffer, ptitle, MB_OK | MB_ICONSTOP | MB_APPLMODAL);
    return(0);
}
