#include "scr4.h"

/*NH*/
InfRStrip0(a)
char    *a;
{
    int     i;

    for(i = strlen(a) - 1 ; i > 0 ; i--) {
	if(a[i] != '0') return(0);
	a[i] = 0;
	}
    return(0);
}

/*NH*/
InfStrip0(a)
char    *a;
{
    InfRev(a);
    InfRStrip0(a);
    InfRev(a);
    return(0);
}

/*NH*/
InfRev(a)
char    *a;
{
    int     lg = strlen(a);
    int     lg2 = lg / 2, i, ch;

    for(i = 0 ; i < lg2 ; i++) {
	ch = a[i];
	a[i] = a[lg - i - 1];
	a[lg - i - 1] = ch;
	}
    return(0);
}

