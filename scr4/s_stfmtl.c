#include "s_strs.h"

/* ====================================================================
Formatte un long l suivant un format fmt donné. Le résultat out est
un string transformé de longueur égale à celle de fmt.

Les caractéres reconnus dans le format sont : 'X', 'x', '9', '0'.
Ils signifient qu'aux seules positions de ces caractéres seront placés
dans leur ordre d'apparition les caractéres résultant du formattage
de l.

Seul cas particulier : le caractére '0' qui sera remplacé par une '0' si
le caractére correspondant de in est ' '.

Les longueurs des strings ne sont pas vérifiées.

&RT le pointeur vers out
&EX
    SCR_fmt_long(out, "9990-00-00", 123456L);
    SCR_fmt_long(out, "TVA : 000-000-000", 123456L);

	donnent respectivement

    "  12-34-56"
    "TVA : 000-123-456"

&TX
&SA SCR_fmt_text(), SCR_ufmt_long()
=======================================================================*/

unsigned char *SCR_fmt_long(out, fmt, l)
unsigned char *out, *fmt;
unsigned long l;
{
    int     i, j;
    char    in[30];

    sprintf(in, "%lu", l);
    i = (int)strlen(fmt) - 1;
    out[i + 1] = 0;
    j = (int)strlen(in) - 1;
    for( ; i >= 0 ; i--) {
	switch(fmt[i]) {
	    case '9' :
	    case 'x' :
	    case 'X' :
		if(j >= 0) out[i] = in[j--];
		else out[i] = ' ';
		break;
	    case '0' :
		if(j >= 0) out[i] = in[j--];
		else out[i] = '0';
		if(out[i] == ' ') out[i] = '0';
		break;
	    default :
		out[i] = fmt[i]; break;
	    }
    }
    return(out);
}



#include <stdarg.h>

/**
 *  Replacement of sprintf by a the "secure" version vsnprintf_s().
 *  Adapted to the compiler Embarcadero / CLang / VC++.
 *  
 *  @param [out] buf      char*     output buffer 
 *  @param [in]  buf_size int       max buf_size
 *  @param [in]  fmt      char*     fmt
 *  @param [in]  ...                optional arguments
 *  @return               int       output buffer length
 */
int SCR_sprintf(char* buf, int buf_size, char* fmt, ...)
{
    va_list myargs;

    va_start(myargs, fmt);
    if(fmt == 0) {
        buf[0] = 0;
    }     
    else {
        #if defined(_MSC_VER)   
            vsnprintf_s(buf, buf_size - 1, _TRUNCATE, fmt, myargs);
        #elif defined(__GNUC__)
            vsnprintf(buf, buf_size - 1, fmt, myargs);
        #else
            vsnprintf_s(buf, buf_size - 1, fmt, myargs);
        #endif    
    }    
    va_end(myargs);

    return(strlen(buf));
}
