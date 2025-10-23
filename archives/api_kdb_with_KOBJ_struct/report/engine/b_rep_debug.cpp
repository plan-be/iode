/**
 *  @header4iode
 * 
 *  IODE report Debug function
 *  --------------------------
 *  Function to send a message and the memory status to the file ./memdbg.txt.
 *  
 *  TODO: add logging function RP_log(char *txt,...).
 *  
 *  List of functions 
 *  -----------------
 *      void RP_debug(char* txt) : appends a message to the report debug file memdbg.txt.
 */

#include <scr4.h>
#include "api/report/engine/engine.h"

/**
 *  Sub-function of RP_debug() to create a summary message of the swap memory usage.
 *  
 *  @param [in, out] char*  txt     buffer that will receive the message
 *  @return          char*          pointer to txt  
 *          
 */
static char *RP_mem(char *txt)
{
    SWSTAT  st;

    SW_status(&st);
    sprintf(txt,"NBA=%ld NBB=%ld ALLOC=%ld FREE=%ld USED=%ld",
            st.st_nballoc - st.st_nbfree,
            st.st_nbblks,
            (st.st_convalloc)/(1024 * 1024),
            (st.st_convafree + st.st_convfree)/(1024L*1024L),
            (st.st_convalloc - st.st_convafree)/(1024L*1024L));
    return(txt);
}

/**
 *  Appends a message to the report debug file memdbg.txt.
 *  
 *  @param [in, out] char*  txt to print in memdb.txt
 */
void RP_debug(char* txt)
{
    FILE 	*fd = 0;
    char	buf[512];

    fd = fopen("memdbg.txt", "a+");
    if(fd) {
        fprintf(fd, "%s", txt);
        fprintf(fd, "\n+++%s\n", RP_mem(buf));
    }
    fclose(fd);
}
