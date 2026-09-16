#include "s_strs.h"

/* ====================================================================
Alloue un string de la même longueur que in et copie in dans le nouveau
string.

Si in est nul, retourne NULL.

&RT le pointeur vers le string alloué. NULL en cas de mémoire
insuffisante ou si in est nul.
&EX
    new = SCR_stracpy("test");
&TX
=======================================================================*/

unsigned char *SCR_stracpy(in)
unsigned char    *in;
{
    unsigned char    *ptr;

    if(in == 0) return((unsigned char *)0);
    ptr = (unsigned char *) SCR_malloc((int)strlen(in) + 1); /* JMP 21-08-2012 */
    if(ptr != 0) strcpy(ptr, in);
    return(ptr);
}


/* ====================================================================
Free *old et le réalloue pour y stocker in. Utilise SCR_stracpy() à
cette fin.

&RT le pointeur vers le string alloué. NULL en cas de mémoire
insuffisante.
&EX
    char    *ptr = NULL;

    SCR_strfacpy(&ptr, "test");
&TX
=======================================================================*/

unsigned char *SCR_strfacpy(old, cnew)
unsigned char    **old, *cnew;
{
    if(*old) SCR_free(*old);
    *old = SCR_stracpy(cnew);
    return(*old);
}

/* ====================================================================
Concatène deux strings et alloue le résultat.

&RT le pointeur vers le string alloué. NULL en cas de mémoire
insuffisante ou si in et add sont vides ou nuls.
&EX
    new = SCR_stracat("Coucou", "test");
    ...
    SCR_free(new);
&TX
=======================================================================*/

unsigned char *SCR_stracat(in, add)
unsigned char    *in, *add;
{
    unsigned char   *ptr = 0;
    int             lg = 0;

    if(add) lg += (int)strlen(add);
    if(in) lg += (int)strlen(in);
    if(lg == 0) return(ptr);
    ptr = (unsigned char *) SCR_malloc(lg + 1);
    if(ptr == 0) return(ptr);
    ptr[0] = 0;
    if(in)  strcpy(ptr, in);
    if(add) strcat(ptr, add);
    return(ptr);
}

/* ====================================================================
Ajoute au string alloué in le string add. Le résultat est alloué.
Le string in est libéré.

&RT le pointeur vers le string alloué. NULL en cas de mémoire
insuffisante ou si in et add sont nuls.
&EX
    in = SCR_strapcy("coucou");
    new = SCR_stracat(in, "test");
    // ne pas faire free(in) car déjà libéré dans SCR_stracat().
&TX
=======================================================================*/

unsigned char *SCR_strafcat(in, add)
unsigned char    *in, *add;
{
    unsigned char   *ptr;
    int             lg;

    if(add == 0) return(in);
    lg = (int)strlen(add);
    if(in) lg += (int)strlen(in);
    ptr = (unsigned char *) SCR_malloc(lg + 1);
    if(ptr == 0) return(in);
    ptr[0] = 0;
    if(in) {
	strcpy(ptr, in);
	SCR_free(in);
	}
    strcat(ptr, add);
    return(ptr);
}



