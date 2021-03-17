#include "iode.h"

B_unlink_1(arg, type)
char *arg;
int  *type;
{
    char    buf[128];

/*    if(unlink(K_set_ext(buf, arg, *type)) == 0)    return(0);
    B_seterror("Cannot delete %s", buf);
    return(-1);
/* JMP 11-11-93 */
    K_set_ext(buf, arg, *type);
    W_close();                      /* JMP 11-07-96 */
    unlink(buf);
    return(0);
}


B_FileDelete(arg, type)
char    *arg;
int     type;
{
    int     unlink();

    return(B_ainit_loop(arg, B_unlink_1, &type));
}









