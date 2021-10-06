#include "iode.h"

/* Read ascii file and add to DB */

KDB *KC_load_asc(filename)
char    *filename;
{
    KDB     *kdb = 0;
    int     cmpt = 0;
    YYFILE  *yy;
    ONAME   name;

    /* INIT YY READ */
    YY_CASE_SENSITIVE = 1;

    SCR_strip(filename);
    yy = YY_open(filename, NULL, 0, (!K_ISFILE(filename)) ? YY_STDIN : YY_FILE);

    if(yy   == 0) {
        kerror(0,"Cannot open '%s'", filename);
        return(kdb);
    }

    /* READ FILE */
    kdb = K_create(K_CMT, K_ASIS);
    while(1) {
        switch(YY_lex(yy)) {
            case YY_EOF :
                YY_close(yy);
                return(kdb);

            case YY_WORD :
                yy->yy_text[K_MAX_NAME] = 0;
                strcpy(name, yy->yy_text);
                KC_read_cmt(kdb, yy, name);
                kmsg("Reading object %d : %s", ++cmpt, name);
                break;

            default :
                kerror(0, "Incorrect entry : %s", YY_error(yy));
                break;
        }
    }

err:
    YY_close(yy);
    K_free(kdb);
    return((KDB *)0);
}

KC_read_cmt(kdb, yy, name)
KDB   *kdb;
YYFILE  *yy;
char    *name;
{
    int     i, keyw, pos;
    char    *cmt;

    /* READ A STRING */
    keyw = YY_lex(yy);
    if(keyw == YY_STRING) cmt = K_wrap(yy->yy_text, 60);
    else {
        cmt = SW_nalloc(1);
        cmt[0] = '\0';
        YY_unread(yy);
    }

    /* CONTINUE READING UNTIL END OF VALUES */
    while(1) {
        keyw = YY_lex(yy);
        if(keyw == YY_WORD || keyw == YY_EOF) break;
    }
    YY_unread(yy);

    pos = K_add(kdb, name, cmt);
    if(pos < 0) {
        kerror(0, "%s : unable to create %s", YY_error(yy), name);
        SW_nfree(cmt);
        return(-1);
    }

    SW_nfree(cmt);
    return(0);
}

KC_save_asc(kdb, filename)
KDB   *kdb;
char  *filename;
{
    FILE    *fd;
    int     i, j;
    //CMT     *cmt;
    CMT     cmt; // JMP 30/9/2021

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
        cmt = KCVAL(kdb, i);
        SCR_replace(cmt, "\n", " ");  /* JMP 31-10-96 */
        SCR_fprintf_esc(fd, cmt, 1);
        fprintf(fd, "\n");
    }

    if(filename[0] != '-') fclose(fd);
    return(0);
}

int KC_save_csv(KDB *kdb, char *filename)
{
    return(-1);
}
