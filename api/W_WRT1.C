#include "iode.h"

void W_print_enum(n)
int     n;
{
    W_printf(".par1 enum_%d\n", n);
}

void W_print_cmd(n)                       /* JMP 13-07-96 */
int     n;
{
    if(n >= 0)
	W_printf(".lf_on\n.bl_on\n.par cmd_%d\n", n);
    else
	W_printf(".lf_off\n.bl_off\n.par par_1\n");
}

void W_print_par(n)
int     n;
{
    W_printf(".par1 par_%d\n", n);
}

void W_print_tit(n)
int     n;
{
    W_printf(".par1 tit_%d\n", n);
}

