#include "s_strs.h"

/* ======================================================================
Formatte un double d en prenant en compte la taille du string de
r�sultat (size) et le nombre de d�cimales (nb_dec). Le r�sultat est
plac� dans txt.

nb_dec peut valoir -1 pour indiquer que le format est libre. La fonction
utilise alors le formattage standard (%lg) qui permet d'optimiser le
nombre de chiffres significatifs suivant la valeur absolue du nombre.

Si la valeur est inf�rieure � SCR_NOT_AVAIL (voir s_strs.h), deux tirets
sont plac�s dans txt.

Cette fonction est utilis�e pour l'affichage des doubles dans les champs
des �crans SCR/AL1.

&TI Algorithme
����������

&EN Si la valeur est inf�rieure � SCR_NOT_AVAIL, '--' est plac� dans txt
&EN Si le nombre de d�cimales est inf�rieur � 0, le format utilis� est
    le format par d�faut (%lg), qui donne un nombre de d�cimales optimal
    est fonction de la valeur du nombre
&EN Si le nombre de d�cimales est sup�rieure ou �gal � 0, le format %lf
    est utilis�, avec le nombre de d�cimales demand�. Si ce format
    d�passe size caract�res, le format standard est utilis�.
&EN Si, apr�s ces essais, le nombre de caract�res est plus grand que
    size et que size est plus grand ou �gal � 7, le format E est utilis�
    avec comme nombre de d�cimales (size - 7). Si size est inf�rieur �
    7, des �toiles sont plac�es dans txt.

&EX
    printf("'%s'", SCR_fmt_dbl(12.3456789, buffer, 6, 2);
	vaut
    '12.34 '
    printf("'%s'", SCR_fmt_dbl(-12.3456789E10, buffer, 8, 2);
	vaut
    '-1.2E10 '
&TX
&RT Le pointeur vers le buffer pass� comme argument
------------------------------------------------------------------------ */

unsigned char *SCR_fmt_dbl(d, txt, size, nb_dec)
double          d;
unsigned char   *txt;
int             size, nb_dec;
{
    U_ch    buf[80], buf1[80];
    int     lg;

    if(d <= -1.0e37) {          /* JMP38 01-10-92 */
	strcpy(buf1, "--");
	goto fin;
	}

    if(d > 1E20 || d < -1E20 || nb_dec < 0) {
	sprintf(buf, "%%%d.%dlg", size, ((8<size)?8:size)); /* JMP 09-04-98 */
	sprintf(buf1, buf, d);
	}
    else {
	sprintf(buf, "%%%d.%dlf", size, nb_dec);
	sprintf(buf1, buf, d);
	if((int)strlen(buf1) > size) {
	    if(nb_dec > 0) {                                      /* JMP 27-05-01 */
		while(1) {                                        /* JMP 27-05-01 */
		    lg = (int)strlen(buf1);                            /* JMP 27-05-01 */
		    if(lg <= size || buf1[lg - 1] != '0') break;  /* JMP 27-05-01 */
		    buf1[lg - 1] = 0;                             /* JMP 27-05-01 */
		    }                                             /* JMP 27-05-01 */
		}                                                 /* JMP 27-05-01 */
	    if((int)strlen(buf1) > size && buf1[(int)strlen(buf1) - 1] == '.') /* JMP 27-05-01 */
		buf1[(int)strlen(buf1) - 1] = 0;                       /* JMP 27-05-01 */
	    if((int)strlen(buf1) > size) sprintf(buf1, "%lg", d);      /* JMP 27-05-01 */
	    }                                                     /* JMP 27-05-01 */
	}

    if((int)strlen(buf1) > size) {
	if(size >= 8) {
	    sprintf(buf, "%%%d.%dle", size, size - 8);
	    sprintf(buf1, buf, d);
	    }
	else {
	    sprintf(buf1, "%lg", d);
	    if((int)strlen(buf1) > size) {
		memset(buf1, '*', size);
		buf1[size] = 0;
		}
/*            memset(buf1, '*', size);
	    buf1[size] = 0; */
	    }
	}
fin:
    SCR_pad(buf1, size);
    strcpy(txt, buf1);
    return(txt);
}




