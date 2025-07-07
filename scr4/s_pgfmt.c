#include "s_strs.h"

unsigned char *PG_fmt_text(buf, fmt, txt)
char    *buf, *fmt, *txt;
{
    return(SCR_fmt_text(buf, fmt, txt));
}

unsigned char *PG_ufmt_text(buf, fmt, txt)
char    *buf, *fmt, *txt;
{
    return(SCR_ufmt_text(buf, fmt, txt));
}

unsigned char *PG_fmt_long(buf, fmt, l)
unsigned char    *buf, *fmt;
unsigned long    l;
{
    return(SCR_fmt_long(buf, fmt, l));
}

unsigned long PG_ufmt_long(fmt, txt)
char    *fmt, *txt;
{
#ifndef __cplusplus
    extern unsigned long SCR_ufmt_long();
#endif

    return(SCR_ufmt_long(fmt, txt));
}


/*NH*/
PG_is_fmt_empty(text, fmt)  /* BP_M 01-10-97 */
char    *text, *fmt;
{
    int     j, i;

    for(j = 0, i = 0 ; fmt[j] != 0 ; j++)
	switch(fmt[j]) {
	    case '9':
	    case '0':
		      if(text[i] == 0) return(1); /* BP_M 09-10-1999 */
		      if(text[i] != ' ' && text[i] != 0 && text[i] != '0') return(0);
		      i++;
		      break;
	    case 'x':
	    case 'X':
		      if(text[i] == 0) return(1); /* BP_M 09-10-1999 */
		      if(text[i] > ' ' && text[i] != fmt[i]) return(0);
		      i++;
		      break;
	    default:  break;
	    }

    return(1);
}




