#include "s_strs.h"

/* ====================================================================
Copie un string dans un autre en limitant le nombre de caractŠres … copier.
Si in est plus grand que le nombre de caractŠres sp‚cifi‚s, seuls les n
premiers sont copi‚s dans out. out[n] est fix‚ … 0.

Si in est nul, out[0] est fix‚ … 0.

&RT le pointeur vers le string out.
&EX
    printf("'%s'\n", SCR_strlcpy(buf, "123456", 3));
    printf("'%s'\n", SCR_strlcpy(buf, "123456", 6));
    printf("'%s'\n", SCR_strlcpy(buf, "123456", 10));

	donnent respectivement

    '123'
    '123456'
    '123456'
&TX
&SA SCR_pad()
=======================================================================*/

unsigned char *SCR_strlcpy(out, in, n)
unsigned char *out, *in;
int           n;
{
    int i;

    out[0] = 0;
    if(in) {
	for(i = 0; i < n && in[i] ; i++)
	    out[i] = in[i];
	out[i] = out[n] = 0;
	}
    return(out);
}
