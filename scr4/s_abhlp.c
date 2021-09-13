#include "scr4.h"

/*NH*/
AB_help_stack(ab, choice)
A_BAR   *ab;
int     choice;
{
    A_OPT   *ao;

    ao = ab->ab_opts + choice;
    HLP_stack_add(ab->ab_help);
    if(choice < 0) {
	HLP_stack_add(0L);
	HLP_stack_add(0L);
	}
    else {
	HLP_stack_add(ao->ao_text);
	HLP_stack_add(ao->ao_help);
	}
    return(0);
}

