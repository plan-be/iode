#include "scr4.h"

/*NH*/
MN_help_stack(mn)
MENU    *mn;
{
    HLP_stack_add(mn->mn_help);
    return(0);
}

/*NH*/
MN_help_stack_opt(mn)
MENU    *mn;
{
    int     choice;
    OPTION  *opt;

    choice = mn->mn_choice;
    opt = mn->mn_opts + choice;
    if(choice < 0) {
	HLP_stack_add(0L);
	HLP_stack_add(0L);
	}
    else {
	HLP_stack_add(opt->opt_text);
	HLP_stack_add(opt->opt_help);
	}
    return(0);
}


