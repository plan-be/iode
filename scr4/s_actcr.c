#include "scr.h"


/*NH*/
long ACT_find_lg(aa, i)
A_ACTION    *aa;
int         i;
{
    char    *buf;
    long    l;

    if(aa->aa_nb_lg == 0) return(0L);
    buf = (char *)aa;
    memcpy(&l, buf + sizeof(A_ACTION) + sizeof(long) * i, sizeof(long)); /* JMP38 31-08-92 */
    return(l);
}

/*NH*/
double ACT_find_db(aa, i)
A_ACTION    *aa;
int         i;
{
    double  d;

    if(aa->aa_nb_db == 0) return(0.0);
    memcpy(&d,  (char *)aa + sizeof(A_ACTION) +         /* JMP38 31-08-92 */
		       sizeof(long) * aa->aa_nb_lg +
		       sizeof(double) * i, sizeof(double));
    return(d);                                          /* JMP38 31-08-92 */
}

/*NH*/
char *ACT_find_str(aa, i)
A_ACTION    *aa;
int         i;
{
    char    *str;

    if(aa->aa_nb_str == 0) return((char *)0);
    str = (char *)aa + sizeof(A_ACTION) +
		       sizeof(long)   * aa->aa_nb_lg +
		       sizeof(double) * aa->aa_nb_db;
    for( ; i > 0 ; i--)
	str += *str + 1;
    return(str + 1);
}
