#include "iode.h"

#include <s_swap.h>
#include <s_args.h>

/**
 * Initialize an iode session.
 * - define the "swap" memory segment size
 * - initialize the "swap" engine
 * - create 7 empty ws
 * - define list seps (?)
 */
IodeInit()
{
    SW_MIN_MEM = 120 * 1024L;
    SW_MIN_SEGS = 2;
    SW_init(1);
    K_init_ws(0);

    B_LANG = 0;
    B_NBDEC = -1;
    A_SEPS = " ,;\n\t\r";
    A_NO_EXPANDSTAR_DFT = 1; 

    return(0);
}


/**
 * Close an iode session.
 * 
 */
IodeEnd()
{
    SW_end();           // Close swap engine and free allocated mem segments
    W_close();          // Close current output session (if any)
    K_end_ws(0);        // Free 7 ws
    L_free_anames();    // Free Lec compiler memory
    BUF_free();         // Free allocated general buffers
    RP_proc_free_all(); // Free allocated report memory
    RP_free_bufs();     //  id.
    B_reset_error();    // 
}
