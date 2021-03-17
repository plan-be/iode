#include <iode.h>

extern int K_SCR, K_MSG;

int kclmsg(key, status)
int key, status;
{
    char    buf[80];

    if(K_MSG && K_SCR)  {
	buf[0] = K_MSG = 0;
	kwrmsg(buf);
    }
    return(0);
}

#if !defined(SCRW32) && !defined(VC) /* JMP 21-01-98 */
void kwrmsg(msg)          /* JMP 28-04-97 */
char    *msg;
{
    SCR_pad(msg, 48);
    SCR_set_attr(SCR_REVERSE);
    SCR_write_text(SCR_PAGE_SIZE[0] - 1, 0, msg, 48); /* JMP 12-01-11 */
#ifdef DOSW32
    SCR_wdispatch();  /* JMP 24-06-98 */
#endif
}
#endif




