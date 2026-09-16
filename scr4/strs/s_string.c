#include <stdlib.h>
#include <stdio.h>
#include "s_strs.h"

/********************************************************************/
/* BUFFER CIRCULAIRE FIFO*/

#ifdef __cplusplus
M_RING *M_RING_init(
int size
)
#else
M_RING *M_RING_init(size)
int size;
#endif
{
    M_RING    *t;

    t = malloc(sizeof(M_RING));
    if(t == NULL) goto err;
    memset(t, 0, sizeof(M_RING));
    t->taille = size;
    t->tampon = malloc(size * sizeof(U_ch ));
    if(t->tampon == NULL)
    {
	free(t);
	goto err;
    }
    memset(t->tampon, 0, size);
    return(t);
err:
    return(NULL);
}


int M_RING_free(t)
M_RING *t;
{
    if(t->tampon != NULL) free(t->tampon);
    if(t != NULL) free(t);
    return(0);
}


/*
detect: si 0 < compteur  < 3/4 taille : 0 (remplir)
	si compteur < 1/4 taille : -1 (envoyer XON)
*/

U_ch  M_RING_read(t, detect)
M_RING *t;
int *detect;
{
    U_ch  c;

    if(t->compt == 0)
    {
	*detect = -1;
	return(0); /*vide */
    }

    c = t->tampon[t->debut];
    t->debut++;
    if(t->debut >= t->taille) t->debut = 0;
    t->compt --;
    if(t->compt < t->taille / 4) *detect = -1;
    return(c);
}


/*
retour: si compteur > 3/4 taille : 1 (envoyer XOFF)
	si 0 < compteur  < 3/4 taille : 0 (remplir)
	si compteur < 1/4 taille : -1 (envoyer XON)
*/

int M_RING_write(t, c, detect)
M_RING  *t;
int     c; /* JMP 18-01-98 */
int     *detect;
{
    int decal;

    decal = (t->debut + t->compt) % (t->taille);
    t->tampon[decal] = c;
    if( t->compt >= t->taille)
    {
	t->debut++;
	if (t->debut >= t->taille) t->debut -= t->taille;
    }
    else    t->compt ++;
    if(t->compt > 3 * t->taille / 4) *detect = 1;
    return(0);
}

int M_RING_reset(t)
M_RING *t;
{
    t->compt = 0;
    return(0);
}



