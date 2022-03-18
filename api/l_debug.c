/**
 *  @header4iode
 *
 *  Function called by Newton-Raphson sub iteration.
 *  
 *      void L_debug(char* txt, char* a1, char* a2, char* a3, char* a4, char* a5, char* a6)  Saves a message in simul.dbg.
 *  
 *  TODO: replace by a "super" function and replace "simul.dbg" by a global variable
 */

#include <stdio.h>

/**
 *  Saves a message in simul.dbg.
 *  
 *  @param [in] char*   txt  format of the message (1st fprintf parameter)
 *  @param [in] char*   a1   param 1
 *  @param [in] char*   a2   param 2
 *  @param [in] char*   a4   param 3
 *  @param [in] char*   a5   param 4
 *  @param [in] char*   a6   param 5
 *  @param [in] char*   a6   param 6 
 */
void L_debug(char* txt, char* a1, char* a2, char* a3, char* a4, char* a5, char* a6)
{
    FILE *fd = 0;

    fd = fopen("simul.dbg", "a+");
    if(fd) fprintf(fd, txt, a1, a2, a3, a4, a5, a6);
    fclose(fd);
}

