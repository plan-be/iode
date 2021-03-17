#include "iode.h"

#define KV_SMPL     1
#define KV_NA       2
#define KV_MINUS    3

/* Read ascii file and add to DB */


YYKEYS KV_TABLE[] = {
    "sample",   KV_SMPL
    };


KDB *KV_load_yy(yy, ask)
YYFILE  *yy;
int ask;
{
    KDB     *kdb = 0;
    int     nb, nobs, cmpt = 0;
    ONAME   name;
    PERIOD  one, two;
    SAMPLE  *smpl, *K_ask_smpl();

    /* READ FILE */
    kdb = K_create(K_VAR, K_UPPER);

    /* SAMPLE IN CLIENT DATA STRUCTURE OF OBJ */
    if (YY_lex(yy) != KV_SMPL) {
	if(!ask) {
	    kerror(0,"%s Expected sample definition", YY_error(yy));
	    goto err;
	}
	else {
	    YY_unread(yy);
	    smpl = K_ask_smpl();
	}
    }
    else smpl = K_read_smpl(yy);

    if(smpl == NULL) goto err;
    memcpy((SAMPLE *) KDATA(kdb), smpl, sizeof(SAMPLE));

    while(1) {
	switch(YY_lex(yy)) {
	    case YY_EOF :
		SW_nfree(smpl);
		return(kdb);

	    case YY_WORD :
		if(smpl->s_nb == 0) {
		    kerror(0, "%s : undefined sample", YY_error(yy));
		    goto err;
		    }
		yy->yy_text[K_MAX_NAME] = 0;
		strcpy(name, yy->yy_text);
		KV_read_vec(kdb, yy, name);
		kmsg("Reading object %d : %s", ++cmpt, name);
		break;

	    default :
		kerror(0, "Incorrect entry : %s", YY_error(yy));
		break;
	    }
	}

err:
    SW_nfree(smpl);
    K_free(kdb);
    return((KDB *)0);
}


KDB *KV_load_asc_type_ask(filename, type, ask)
char    *filename;
int 	type; // 0 = file, 1 = string
int		ask; // 0
{
	static  int sorted;
	KDB     *kdb = 0;
	YYFILE  *yy;
	int		yytype;

	if(type == 0) {
		SCR_strip(filename);
		yytype = (!K_ISFILE(filename)) ? YY_STDIN : YY_FILE;
	}
	else {
		yytype = YY_MEM;
	}

	/* INIT YY READ */
	YY_CASE_SENSITIVE = 1;

	if(sorted == 0) {
		qsort(KV_TABLE, sizeof(KV_TABLE) / sizeof(YYKEYS), sizeof(YYKEYS), YY_strcmp);
		sorted = 1;
	}

	yy = YY_open(filename, KV_TABLE, sizeof(KV_TABLE) / sizeof(YYKEYS), yytype);

	if(yy == 0) {
		if(!type) kerror(0,"Cannot open '%s'", filename);
		else      kerror(0,"Cannot open string");
		return(kdb);
	}

	kdb = KV_load_yy(yy, ask);
	YY_close(yy);
	return(kdb);
}

KDB *KV_load_asc(filename)
char    *filename;
{
	return(KV_load_asc_type_ask(filename, 0, 0));
}


KV_read_vec(kdb, yy, name)
KDB   *kdb;
YYFILE  *yy;
char    *name;
{
    int     i, keyw, pos, nb;
    IODE_REAL    *vec;
    SAMPLE  *smpl;

    smpl = (SAMPLE *) KDATA(kdb);
    vec = (IODE_REAL *) SW_nalloc(smpl->s_nb * sizeof(IODE_REAL));

    /* READ AT MOST nobs OBSERVATIONS */
    for(i = 0; i < smpl->s_nb; i++)  vec[i] = K_read_real(yy);

    /* CONTINUE READING UNTIL END OF VALUES */
    while(1) {
	keyw = YY_lex(yy);
	if((keyw == YY_WORD && strcmp(yy->yy_text, "na") ) || keyw == YY_EOF) break;
    }
    YY_unread(yy);

    nb = smpl->s_nb;
    pos = K_add(kdb, name, vec, &nb);
    if(pos < 0) {
	kerror(0, "%s : unable to create %s", YY_error(yy), name);
	SW_nfree(vec);
	return(-1);
	}

    SW_nfree(vec);
    return(0);
}

KV_save_asc(kdb, filename)
KDB   *kdb;
char  *filename;
{
    FILE    *fd;
    int     i, j;
    IODE_REAL    *val;
    SAMPLE  *smpl;

    if(filename[0] == '-') fd = stdout;
    else {
	fd = fopen(filename, "w+");
	if(fd == 0) {
	    kerror(0, "Cannot create '%s'", filename);
	    return(-1);
	    }
	}

    smpl = (SAMPLE *) KDATA(kdb);
    fprintf(fd, "sample %s ", PER_pertoa(&(smpl->s_p1), 0L));
    fprintf(fd, "%s\n", PER_pertoa(&(smpl->s_p2), 0L));

    for(i = 0 ; i < KNB(kdb); i++) {
	fprintf(fd, "%s ", KONAME(kdb, i));
	val = KVVAL(kdb, i, 0);
	for(j = 0 ; j < smpl->s_nb; j++, val++) KV_print_val(fd, *val);
	fprintf(fd, "\n");
	}

    if(filename[0] != '-') fclose(fd);
    return(0);
}


KV_print_val(fd, val)
FILE    *fd; IODE_REAL    val;
{
    if(L_ISAN(val))
#ifdef REALD
	fprintf(fd, "%.15lg ", (double)(val)); /* JMP 09-04-98 */
#else
	fprintf(fd, "%.8lg ", (double)(val)); /* JMP 09-04-98 */
#endif
    else
	fprintf(fd, "na ");

    return(0);
}

/* CSV Files */

char *KV_CSV_SEP = 0;
char *KV_CSV_DEC = 0;
char *KV_CSV_NAN = 0;
char *KV_CSV_AXES = 0;
int  KV_CSV_NBDEC = 15;

int KC_save_csv(KDB *kdb, char *filename) {return(-1);}
int KE_save_csv(KDB *kdb, char *filename) {return(-1);}
int KI_save_csv(KDB *kdb, char *filename) {return(-1);}
int KL_save_csv(KDB *kdb, char *filename) {return(-1);}
int KS_save_csv(KDB *kdb, char *filename) {return(-1);}
int KT_save_csv(KDB *kdb, char *filename) {return(-1);}

int KV_save_csv(KDB *kdb, char *filename, SAMPLE *smpl, char **varlist)
{
    FILE        *fd;
    int         i, j, nb, pos;
    char        fmt[80], buf[256], *sep, *dec, *nan, *axes, **lst;
    IODE_REAL   *val;
    PERIOD      *per;

    // Parameters : sep, dec, nbdec, nan, 
    sep = KV_CSV_SEP;
    dec = KV_CSV_DEC;
    nan = KV_CSV_NAN;
    if(sep == 0 || sep[0] == 0) {
        sep = ",";
        sep[0] = SCR_sLIST; // JMP 27/1/2017
    }    
    if(dec == 0 || dec[0] == 0) {
        dec = ".";
        dec[0] = SCR_sDECIMAL; // JMP 27/1/2017
    }    
    if(KV_CSV_NBDEC < 0 || KV_CSV_NBDEC > 15)
        strcpy(fmt, "%lg");
    else
        sprintf(fmt, "%%.%dlg", KV_CSV_NBDEC);
    if(nan == 0) nan = "";

    axes = KV_CSV_AXES;
    if(axes == 0 || axes[0] == 0) axes = "var";

    // sample : if KV_CSV_SAMPLE == NULL => tout le sample
    if(smpl == 0)
        smpl = (SAMPLE *) KDATA(kdb);

    // Liste
    lst = varlist;
    if(varlist == 0) {
        lst = 0;
        nb = 0;
        for(i = 0; i < KNB(kdb); i++)
            SCR_add_ptr(&lst, &nb, KONAME(kdb, i));
        SCR_add_ptr(&lst, &nb, 0L);
    }

    // Open file
    if(filename[0] == '-') fd = stdout;
    else {
        fd = fopen(filename, "w+");
        if(fd == 0) {
            kerror(0, "Cannot create '%s'", filename);
            return(-1);
        }
	}

    // Ligne 1
    fprintf(fd, "%s\\time", axes);
    for(i = 0 ; i < smpl->s_nb ; i++) {
        if(smpl->s_p1.p_p == 'Y')
            fprintf(fd, "%s%d", sep, smpl->s_p1.p_y + i);
        else {
            per = PER_addper(&(smpl->s_p1), i);
            fprintf(fd, "%s%s", sep, PER_pertoa(per, 0L));
        }
    }
    fprintf(fd, "\n");

    // Lignes suivantes
    for(i = 0 ; lst[i] ; i++) {
        SCR_upper(lst[i]);
        fprintf(fd, "%s", lst[i]);
        pos = K_find(kdb, lst[i]);
        if(pos >= 0) {
            val = KVVAL(kdb, pos, 0);
            for(j = 0 ; j < smpl->s_nb; j++, val++) {
                if(L_ISAN(*val)) {
                    sprintf(buf, fmt, (double)(*val));
                    if(dec[0] != '.') SCR_replace(buf, ".", dec);
                }
                else {
                    strcpy(buf, nan);
                }
                fprintf(fd, "%s%s", sep, buf);
            }
        }
        fprintf(fd, "\n");
    }
    if(varlist == 0) SCR_free_tbl(lst);
    if(filename[0] != '-') fclose(fd);
    return(0);
}

