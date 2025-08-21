/**
 * @header4iode
 * 
 * Functions to load and save ascii definitions of IODE LST objects.
 *
 *      KDB *load_asc(char* filename)
 *      int save_asc(KDB* kdb, char* filename)
 *      int save_csv(KDB *kdb, char *filename)
 *
 */
#include "api/b_errors.h"
#include "api/k_super.h"
#include "api/objs/objs.h"
#include "api/objs/lists.h"
#include "api/ascii/ascii.h"


/**
 *  Reads on an open YY stream (file or string) the ascii definition of an IODE LST and adds the new LST to kdb. 
 *  Subfn of KC_load_asc().
 *  
 *  Reads the next token on yy. 
 *      If a string (between "") is found, its content becomes the new content of the LST named *name*.
 *      If not, "" is stored in LST *name*
 *  The content of the list is split in max 60-chars lines.
 *  
 *  All other tokens found between the definition and the next name are ignored.
 *  
 *  @param [in, out ]   kdb     KDB*    KDB of LST
 *  @param [in, out]    yy      YYFILE* stream (file or string) to be read from
 *  @param [in]         name    char*   LST name
 *  @return                     int     0 if the LST is read and saved, 
 *                                      -1 if the LST can't be created. (call to kerror() in that case)
 *  
 */
static int read_lst(KDB* kdb, YYFILE* yy, char* name)
{
    int     keyw, pos;
    char    *lst;

    /* READ A STRING */
    keyw = YY_lex(yy);
    if(keyw == YY_STRING)  lst = K_wrap((char*) yy->yy_text, 60);
    else {
        lst = SW_nalloc(1);
        lst[0] = '\0';
        YY_unread(yy);
    }

    /* CONTINUE READING UNTIL END OF VALUES */
    while(1) {
        keyw = YY_lex(yy);
        if(keyw == YY_WORD || keyw == YY_EOF) break;
    }
    YY_unread(yy);

    pos = K_add(kdb, name, lst);
    if(pos < 0) {
        kerror(0, "%s : unable to create %s", YY_error(yy), name);
        SW_nfree(lst);
        return(-1);
    }

    SW_nfree(lst);
    return(0);
}

/**
 *  Loads LSTs from an ASCII file into a new KDB.
 *  
 *  Syntax of LST ascii definitions 
 *  -------------------------------
 *      LSTNAME "list of values separated by space, comma or semi-colon"
 *  
 *  Example
 *  -------
 *      COPY "$COPY0;$COPY1;"
 *      COPY0 "ACAF;ACAG;AOUC;AQC;BENEF;BQY;BVY;CGU;COEFON;COTRES;DPU;
 *               DPUF;DPUG;DPUGO;DPUH;DPUU;DTF;DTH;DTH1;DTH1C;EXC;EXCC;FLF;
 *               FLG;GAP;GOSF;GOSG;GOSH;GOSH_;IDF;IDG;IDH;IFU;IHU;IT;ITCEE"
 *      COPY1 "VMAB;VME;VMK;VMN;VMS;VMT;VS;VS_;VX;VXAB;VXB;VXE;VXK;VXN;VXS;
 *               VXT;WBF;WBF_;WBG;WBGO;WBGP;WBU;WBU_;WCF;WCF_;WDOM;WG;WIND"
 *
 *  Error messages are sent to the function kerror().
 *  
 *  For each read LST, kmsg() is called to send a message to the user. 
 *  
 *  The implementations of kerror() and kmsg() depend on the context (GUI, command line...).
 *  
 *  @param [in] filename    char*   name of the ascii file to be read or 
 *                                  string containing the definition of the identities
 *  @return                 KDB*    new KDB of LST or NULL on error
 *  
 *  TODO: what if read_lst returns an error code ?
 *  
 */
KDB* AsciiLists::load_asc(char* filename)
{
    int     cmpt = 0;
    KDB     *kdb = 0;
    YYFILE  *yy;
    ONAME   name;

    /* INIT YY READ */
    YY_CASE_SENSITIVE = 1;

    SCR_strip((unsigned char *) filename);
    yy = YY_open(filename, NULL, 0, (!K_ISFILE(filename)) ? YY_STDIN : YY_FILE);

    if(yy   == 0) {
        kerror(0,"Cannot open '%s'", filename);
        return(kdb);
    }

    /* READ FILE */
    kdb = K_create(LISTS, UPPER_CASE);
    K_set_kdb_fullpath(kdb, (U_ch*)filename); // JMP 28/11/2022
    
    while(1) {
        switch(YY_lex(yy)) {
            case YY_EOF :
                if(cmpt) {
                    char    asc_filename[1024];
                    K_set_ext_asc(asc_filename, filename, LISTS);
                    K_set_kdb_fullpath(kdb, (U_ch*)asc_filename); // JMP 03/12/2022
                }
                YY_close(yy);
                return(kdb);

            case YY_WORD :
                yy->yy_text[K_MAX_NAME] = 0;
                strcpy(name, (char*) yy->yy_text);
                if(read_lst(kdb, yy, name) == 0) cmpt++;
                kmsg("Reading object %d : %s", cmpt, name);
                break;

            default :
                kerror(0, "Incorrect entry : %s", YY_error(yy));
                break;
        }
    }

    YY_close(yy);
    K_free(kdb);
    return((KDB *)0);
}

/**
 *  Saves a KDB of LSTs in an ascii file (.al) or to the stdout.
 *  
 *  @see load_asc() for the syntax. 
 *  
 *  @param [in] kdb         KDB*    KDB of LSTs
 *  @param [in] filename    char*   name of the output file or "-" to write the result on the stdout.
 *  @return                 int     0 on success, -1 if the file cannot be written.
 *  
 */
int AsciiLists::save_asc(KDB* kdb, char* filename)
{
    FILE    *fd;
    int     i;
    // LIS     *lst;
    LIS     lst;        // JMP 30/9/2021

    if(filename[0] == '-') fd = stdout;
    else {
        fd = fopen(filename, "w+");
        if(fd == 0) {
            kerror(0, "Cannot create '%s'", filename);
            return(-1);
        }
    }

    for(i = 0 ; i < KNB(kdb); i++) {
        fprintf(fd, "%s ", KONAME(kdb, i));
        lst = KLVAL(kdb, i);
        SCR_fprintf_esc(fd, lst, 1);
        fprintf(fd, "\n");
    }

    if(filename[0] != '-') fclose(fd);
    return(0);
}

/* 
 * Save a KDB of LSTs in a .csv file.
 * NOT IMPLEMENTED.
 */

int AsciiLists::save_csv(KDB *kdb, char *filename, Sample* sample, char** varlist)
{
    return(-1);
}
