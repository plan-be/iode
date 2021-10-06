#include "iode.h"


KDB     *K_WS[7];
KDB     *K_RWS[7][5];
int     K_PWS[7] = { 0, 0, 0, 0, 0, 0, 0 };

extern  int K_AUTO_WS;

/* General WS functions */
KDB *K_init_kdb(type, filename)
int     type;
char    *filename;
{
    int     mode;
    KDB     *kdb;

    switch(type) {
        case K_CMT :
            mode = K_ASIS;
            break;
        case K_EQS :
        case K_IDT :
        case K_LST :
        case K_TBL :
        case K_VAR :
            mode = K_UPPER;
            break;
        case K_SCL :
            mode = K_LOWER;
            break;
    }
    kdb = K_create(type, mode);
    if(kdb == NULL) return(kdb);
    //strcpy(KNAME(kdb), filename);
    K_set_kdb_name(kdb, filename); // JMP 3/6/2015
    return(kdb);
}

K_set_kdb_name(KDB *kdb, U_ch *filename) // JMP 3/6/2015
{
    if(kdb) {
        SCR_free(KNAMEPTR(kdb));
        KNAMEPTR(kdb) = SCR_stracpy(filename);
    }
    return(0);
}

/*
    Initialises all the WS structures
*/
void K_init_ws(ws)
int ws;
{
    int     i;

    memset(K_RWS, 0, sizeof(K_RWS));
    for(i = 0 ; i < 7 ; i++) {
        K_WS[i] = K_RWS[i][0] = K_init_kdb(i, "ws");
        if(ws) K_cat(K_WS[i], "ws");
    }
}
/*
    Deletes all WS structures
*/
void K_end_ws(ws)
int     ws;
{
    int i, j;

    for(i = 0; i < 7; i++) {
        if(ws) K_save_ws(K_WS[i]);
        for(j = 0; K_RWS[i][j]; j ++) {
            K_free(K_RWS[i][j]);
            K_RWS[i][j] = 0; // JMP 9/11/2012
        }
    }
}

/*
    Catenates the contents of a file to a given db
    If the file is an ascii file the fn_ascii function provided
    will interpret this file into a valid db
*/
K_cat(ikdb, filename)
KDB     *ikdb;
char    *filename;
{
    KDB     *kdb;
    int     ftype;

    kdb = K_interpret(KTYPE(ikdb), filename);
    if(kdb == NULL) return(-1);

    if(KNB(ikdb) == 0) {
        memcpy(KDESC(ikdb), KDESC(kdb), K_MAX_DESC);
        //strcpy(KNAME(ikdb), KNAME(kdb));
        K_set_kdb_name(ikdb, KNAMEPTR(kdb)); // JMP 3/6/2015
    }
    if(KTYPE(ikdb) == K_VAR) KV_merge_del(ikdb, kdb, 1);
    else K_merge_del(ikdb, kdb, 1);

    return(0);
}


