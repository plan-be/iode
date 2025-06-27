/**
 *  @header4iode
 *
 *  Function called by the Newton-Raphson sub-iteration and by the step wise algorithm 
 *  to save debugging info in a text file.
 *
 *  Function
 *  --------
 *      void L_debug(char* fmt, ...) : appends a message to the file L_DEBUG_FILENAME ("simul.dbg" by default).
 *
 *  Global variable
 *  ---------------
 *      char* L_DEBUG_FILENAME = 0;  :  name of the output file (default "simul.dbg")
 *  
 *  TODO: replace by a "super" function 
 */

#include <stdio.h>
#include <stdarg.h>

#include "api/lec/lec.h"


char    *L_DEBUG_FILENAME = 0;  // Name of the output file

/**
 *  Appends a message to the file L_DEBUG_FILENAME ("simul.dbg" by default).
 *  
 *  @param [in] char*   fmt  format of the message (1st fprintf parameter)
 *  @param [in] ...          arbitrary number of args (depends on fmt contents)
 */
void L_debug(char* fmt,...)
{
    char    *filename = L_DEBUG_FILENAME;
    FILE    *fd = 0;
    char    buf[512];
    va_list vargs;
    
    va_start(vargs, fmt);
    if(filename == 0) filename = "simul.dbg";
    fd = fopen(filename, "a+");
    if(fd) {
#ifdef _MSC_VER   
        vsnprintf_s(buf, sizeof(buf) - 1, _TRUNCATE, fmt, vargs);
#else
        vsnprintf_s(buf, sizeof(buf) - 1, fmt, vargs);
#endif    
        fprintf(fd, "%s", buf);
        fclose(fd);
    }    
    va_end(vargs);
}

// OLD VERSION
//void L_debug(char* txt, char* a1, char* a2, char* a3, char* a4, char* a5, char* a6)
//{
//    FILE *fd = 0;
//
//    fd = fopen("simul.dbg", "a+");
//    if(fd) fprintf(fd, txt, a1, a2, a3, a4, a5, a6);
//    fclose(fd);
//}


