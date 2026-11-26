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
 *      void RP_debug(const std::string& txt) : appends a message to the report debug file memdbg.txt.
 */

#include <scr4.h>
#include "api/report/engine/engine.h"

/**
 *  Sub-function of RP_debug() to create a summary message of the swap memory usage.
 *
 *  Example: RP_mem("+++") => "+++NBA=12 NBB=3 ALLOC=1 FREE=0 USED=1"
 *
 *  @param [in] const std::string&  txt     prefix of the message
 *  @return     std::string                 txt followed by the memory usage summary
 *
 */
static std::string RP_mem(const std::string& txt)
{
    SWSTAT  st;
    const long MB = 1024L * 1024L;

    SW_status(&st);
    return txt + "NBA=" + std::to_string(st.st_nballoc - st.st_nbfree)
               + " NBB=" + std::to_string(st.st_nbblks)
               + " ALLOC=" + std::to_string(st.st_convalloc / MB)
               + " FREE=" + std::to_string((st.st_convafree + st.st_convfree) / MB)
               + " USED=" + std::to_string((st.st_convalloc - st.st_convafree) / MB);
}

/**
 *  Appends a message, followed by the memory usage summary, to the report debug file memdbg.txt.
 *
 *  @param [in] const std::string&  txt     message to print in memdbg.txt
 */
void RP_debug(const std::string& txt)
{
    std::ofstream fd("memdbg.txt", std::ios::app);
    if(!fd)
        return;
    fd << txt << "\n" << RP_mem("+++") << "\n";
}
