/**
 *  @header4iode
 *  
 *  Functions specific to the iode - python interface.
 *  If used in the cython code, these functions must be declared in iode.pyx as (for example):
 *       cdef int PyIodePrint(char* name, double *x, int lg)
 *          ...
 *
 */


#include <stdio.h>
#include <windows.h>

int PyIodePrint(char* name, double *x, int lg)
{
    int     i;
    
    printf("%s=", name);
    for(i = 0; i < lg; i++)
        printf("%g ", x[i]);
    printf("\n");
    return(0);
}

// "Super"-functions that superseeds default API functions 

//void A2mMessage_super_PYIODE(char* msg)
//{
//    fprintf(stderr, "%-79.79s\n", msg);
//}
//
//int kconfirm_super_PYIODE(char *fmt,...)
//{
//    char    buf[256];
//    va_list myargs;
//
//    va_start(myargs, fmt);
//    printf(fmt, myargs);
//    va_end(myargs);
//
//    gets_s(buf, sizeof(buf) - 1);
//    SCR_sqz(buf);
//    return(!U_is_in(buf[0], "OoYyJj1"));
//}

// Function to call in the "main" part of iode.pxi
void ODE_assign_super_PYIODE() 
{
   // IODE_assign_super_API(); // Not needed because already called by IodeInit()
   //A2mMessage_super    = A2mMessage_super_PYIODE;
   //kconfirm_super      = kconfirm_super_PYIODE;
}


// Function used by A2M HTML generator (does not work in 64bits)
#include "iode.h" 

int A2mGIF_HTML(A2MGRF *go, U_ch* filename) 
{
    return(0);
} 