#include <stdio.h>

Wprintf(a, b, c, d, e, f, g, h, i, j, k, l, m, n)
char    *a, *b, *c, *d, *e, *f, *g, *h, *i, *j, *k, *l, *m, *n;
{
    printf(a, b, c, d, e, f, g, h, i, j, k, l, m, n);
    return(0);
}

Werror(a, b, c, d, e, f, g, h, i, j, k, l, m, n)
char    *a, *b, *c, *d, *e, *f, *g, *h, *i, *j, *k, *l, *m, *n;
{
    Wprintf(a, b, c, d, e, f, g, h, i, j, k, l, m, n);
    return(0);
}

PG_display_error(a, b, c, d, e, f, g, h, i, j, k, l, m, n)
char    *a, *b, *c, *d, *e, *f, *g, *h, *i, *j, *k, *l, *m, *n;
{
    printf(a, b, c, d, e, f, g, h, i, j, k, l, m, n);
    printf("\n");
    return(0);
}

