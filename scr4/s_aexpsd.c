#include <string.h>
#include "s_args.h"

#if defined(DOS) && !defined(VC) /* JMP 18-01-98 */
#include <dos.h>

/* ==================================================================
Fonction appelée par A_expandstar() lors du processus d'initialisation
des arguments (A_init() et A_initv()).

&SA A_init(), A_initv(), A_expand(), A_iexpand(), A_expandstar()
==================================================================== */

A_expandstar_dft(arg)
char    *arg;
{
    struct find_t   ffblk;
    char            fullname[65], dir[65];
    int             i;

    if(_dos_findfirst(arg, 0, &ffblk)) {
	A_cerror("A_init : '%s' not found\n", arg);
	return(0); /* JMP 14-07-96 */
	}

    strcpy(dir, arg);
    for(i = strlen(dir) - 1 ; i >= 0 ; i--) {
	if(dir[i] == '\\' || dir[i] == ':') {
	    dir[i+1] = 0;
	    break;
	    }
	}

    if(i < 0) dir[0] = 0;

    while(1) {
	sprintf(fullname, "%s%s", dir, ffblk.name);
	A_add1(fullname);
	if(_dos_findnext(&ffblk)) break;
	}
    return(0); /* JMP 14-07-96 */
}

#else
A_expandstar_dft(arg)
char    *arg;
{
    return(0);
}

#endif



