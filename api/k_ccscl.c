#include "iode.h"

extern  IODE_REAL    KS_read_val();
extern  IODE_REAL    K_read_real();

/* Read ascii file and add to DB */

KDB *KS_load_asc(filename)
char    *filename;
{
    int     cmpt = 0;
    KDB     *kdb = 0;
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
    kdb = K_create(K_SCL, K_LOWER);
    while(1) {
        switch(YY_lex(yy)) {
            case YY_EOF :
                YY_close(yy);
                return(kdb);

            case YY_WORD :
                yy->yy_text[K_MAX_NAME] = 0;
                strcpy(name, yy->yy_text);
                KS_read_scl(kdb, yy, name);
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

KS_read_scl(kdb, yy, name)
KDB   *kdb;
YYFILE  *yy;
char    *name;
{
    int     i, keyw, pos;
    SCL     scl;

    /* READ AT MOST 3 REALS */
    scl.val = KS_read_val(yy);
    if(!L_ISAN(scl.val)) scl.val = 0.9;
    scl.relax = KS_read_val(yy);
    if(!L_ISAN(scl.relax)) scl.relax = 1.0;
    scl.std = KS_read_val(yy);

    /* CONTINUE READING UNTIL END OF VALUES */
    while(1) {
        keyw = YY_lex(yy);
        if(keyw == YY_WORD || keyw == YY_EOF) break;
    }
    YY_unread(yy);

    pos = K_add(kdb, name, &scl);
    if(pos < 0) {
        kerror(0, "%s : unable to create %s", YY_error(yy), name);
        return(-1);
    }

    return(0);
}

IODE_REAL KS_read_val(yy)
YYFILE  *yy;
{
    return(K_read_real(yy));
}

KS_save_asc(kdb, filename)
KDB   *kdb;
char  *filename;
{
    FILE    *fd;
    int     i, j;
    SCL     *scl;

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
        scl = KSVAL(kdb, i);
        KS_print_scl(fd, scl);
        fprintf(fd, "\n");
    }

    if(filename[0] != '-') fclose(fd);
    return(0);
}


KS_print_scl(fd, scl)
FILE    *fd;
SCL     *scl;
{
    if(L_ISAN(scl->val)) fprintf(fd, "%.15lg ", (double)(scl->val)); /* JMP 09-04-98 */
    else fprintf(fd, "na ");

    if(L_ISAN(scl->relax)) fprintf(fd, "%.15lg ", (double)(scl->relax)); /* JMP 09-04-98 */
    else fprintf(fd, "na ");

    if(L_ISAN(scl->std)) fprintf(fd, "%.15lg ", (double)(scl->std));     /* JMP 09-04-98 */
    else fprintf(fd, "na ");

    return(0);
}


int KS_save_csv(KDB *kdb, char *filename)
{
    return(-1);
}
