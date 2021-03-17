#include "iode.h"

B_WsLoad(arg, type)
char    *arg;
int     type;
{
    KDB     *kdb = NULL;
    int     pos = K_PWS[type];
	char    buf[K_MAX_FILE + 1];

    SCR_strlcpy(buf, arg, K_MAX_FILE);
    K_strip(buf);   /* JMP 19-04-98 */
    if(buf[0] == 0) return(0);

    kdb = K_interpret(type, buf);
    if(kdb == NULL) return(-1);

    K_free(K_RWS[type][pos]);
    K_WS[type] = K_RWS[type][pos] = kdb;

    return(0);
}



/* Retourne le type de fichier
  - 0 -> 6 = .cmt, ..., .var (binary format)
  - 10 -> 16 = ac, ... av
  - 26 = csv
  - 22 = rep ??? 
  - autres formats (rep, a2m, ..., csv)
  - -1 sinon
*/
X_findtype(filename)
char    *filename;
{
    int         i, lg = strlen(filename);
    extern char k_ext[][4];
    char        buf[5];

    // Check std extensions .cmt => .var
    if(lg > 4) {
        for(i = 0 ; i < 7 ; i++) {
            if(filename[lg - 4] == '.' &&
               SCR_cstrcmp(k_ext[i], filename + lg - 3) == 0) return(i);
        }       
 	}
    
    // Check ascii extensions .ac => .av
    if(lg > 3) {
        strcpy(buf, ".ac");
        for(i = 0 ; i < 7 ; i++) {
            buf[2] = k_ext[i][0];
            if(SCR_cstrcmp(filename + lg - 3, buf) == 0) return(10 + i);
        }
	}
    
    // Other extensions
    // 
    //if(lg > 4 && SCR_cstrcmp(filename + lg - 4, ".csv") == 0) return(21); // Correction JMP 16/1/2019 
    //if(lg > 4 && SCR_cstrcmp(filename + lg - 4, ".rep") == 0) return(22); // Correction JMP 16/1/2019 
    if(lg > 4 && SCR_cstrcmp(filename + lg - 4, ".csv") == 0) return(K_CSV); // Correction JMP 25/3/2019 
    if(lg > 4 && SCR_cstrcmp(filename + lg - 4, ".rep") == 0) return(22); // ??? pas très cohérent...
    
    // Sais plus a quoi ca peut servir... => a supprimer
    for(i = 16 ; strcmp(k_ext[i], "xxx") !=0 ; i++) {
        if(lg > 4 && SCR_cstrcmp(filename + lg - 4, k_ext[i]) == 0) return(i); // Correction JMP 16/1/2019 : lg - 4 au lieu de -3 
    }

    return(-1);
}

int B_WsDump(kdb, filename)
KDB     *kdb;
char    *filename;
{
    int     rc = -1, ftype, type = KTYPE(kdb);

    kmsg("Saving %s", filename);
    ftype = X_findtype(filename);

    if(ftype >= 10 && ftype <= 17) 
        rc = (*K_save_asc[type])(kdb, filename);
    //else if(ftype >= 0 && ftype <= 6)
    else if(ftype <= 6)
        rc = K_save(kdb, filename);
    else if(ftype == K_CSV)
        rc = (*K_save_csv[type])(kdb, filename, NULL, NULL);

    return(rc);
}

B_WsSave(arg, type)
char    *arg;
int     type;
{
	char    buf[K_MAX_FILE + 1];

    SCR_strip(arg);
    if(strlen(arg) >= sizeof(FNAME)) {
	B_seterror(B_msg(256));   /* File name too long */
	return(-1);
	}
    SCR_strlcpy(buf, arg, K_MAX_FILE); /* JMP 18-04-98 */
    SCR_strip(buf);
    if(buf[0] == 0) return(0);
    return(B_WsDump(K_WS[type], buf));
}

B_WsSaveCmp(arg, type)
char    *arg;
int     type;
{
	char        buf[K_MAX_FILE + 1];
    extern int  K_LZH;                  /* JMP 28-05-00 */
    int         rc = 0, klzh;

    SCR_strlcpy(buf, arg, K_MAX_FILE); /* JMP 18-04-98 */
    SCR_strip(buf);
    if(buf[0] == 0) return(0);
    klzh = K_LZH;
    K_LZH = 2;
    rc = B_WsDump(K_WS[type], buf);
    K_LZH = klzh;
    return(rc);
}

/*=============  CSV ============= */

/*
B_WsSaveCsvOld(arg, type)
char    *arg;
int     type;
{
	char        file[K_MAX_FILE + 1];
    int         rc = 0, lg;

    // Filename
    lg = B_get_arg0(file, arg, K_MAX_FILE);
    SCR_strip(file);
    if(file[0] == 0) return(0); // no arg
    K_set_ext(file, file, K_CSV);
    rc = (*K_save_csv[type])(K_WS[type], file);
    return(rc);
}
*/

/* 
   Syntax : $CsvSaveVar file [sample] [vars]
*/
B_CsvSave(arg, type)
char    *arg;
int     type;
{
    int     lg, shift = 0, rc = 0;
	char    file[K_MAX_FILE + 1];
    char    **data0, *K_expand(), *lst;
    SAMPLE  *smpl = NULL;

    // filename 
    lg = B_get_arg0(file, arg, K_MAX_FILE);
    K_set_ext(file, file, K_CSV);
    
    // [sample] [vars]
    lst = K_expand(type, NULL, arg + lg, '*'); 
    data0 = B_ainit_chk(lst, NULL, 0);
    SCR_free(lst);
    if(SCR_tbl_size(data0) == 0) {
        SCR_free_tbl(data0);
        data0 = 0;
    }

    if(data0 && type == K_VAR && SCR_tbl_size(data0) >= 2) {
        if(SCR_is_num(data0[0][0]) && SCR_is_num(data0[1][0])) {
            shift = 2;
            smpl = PER_atosmpl(data0[0], data0[1]);
            B_clear_last_error();
        }    
        if(SCR_tbl_size(data0) <= shift) {
            SCR_free_tbl(data0);
            data0 = 0;
            shift = 0;
        }
    }
    
    rc = (*K_save_csv[type])(K_WS[type], file, smpl, data0 + shift);
    SCR_free_tbl(data0);
    SCR_free(smpl);

    if(rc < 0) return(rc);
    else return(0);
}

B_CsvNbDec(char *nbdec)
{
    extern int KV_CSV_NBDEC;
    
    KV_CSV_NBDEC = atoi(nbdec);
    if(KV_CSV_NBDEC > 99 || (KV_CSV_NBDEC < 0 && KV_CSV_NBDEC != -1)) {
        B_seterrn(53, nbdec);
        KV_CSV_NBDEC = 10;
        return(-1);
    }
    return(0);
}

B_CsvSep(char *sep)
{
    extern char  *KV_CSV_SEP;
    
    SCR_free(KV_CSV_SEP);
    KV_CSV_SEP = SCR_stracpy(sep);
    return(0);
}

B_CsvNaN(char *nan)
{
    extern char  *KV_CSV_NAN;
    
    SCR_free(KV_CSV_NAN);
    KV_CSV_NAN = SCR_stracpy(nan);
    return(0);
}


B_CsvAxes(char *var)
{
    extern char  *KV_CSV_AXES;

    SCR_free(KV_CSV_AXES);
    KV_CSV_AXES = SCR_stracpy(var);
    return(0);
}


B_CsvDec(char *dec)
{
    extern char  *KV_CSV_DEC;
    
    SCR_free(KV_CSV_DEC);
    KV_CSV_DEC = SCR_stracpy(dec);
    return(0);
}

/* =========== Fin CSV ============= */

B_WsExport(arg, type)
char    *arg;
int     type;
{

    return((*K_save_asc[type])(K_WS[type], arg));
}

B_WsImport(arg, type)
char    *arg;
int     type;
{
    return(B_WsLoad(arg, type));
}

B_WsSample(arg)
char    *arg;
{
    char    **args;
    SAMPLE  *new = NULL;
    KDB     *kdb = K_WS[K_VAR];

    args = B_ainit_chk(arg, NULL, 2);
    if(args == NULL) goto err;

    new = PER_atosmpl(args[0], args[1]);
    if(new == 0 || new->s_nb <= 0) goto err; /* JMP 25-05-92 */
    if(KV_sample(kdb, new) < 0) goto err;

    SW_nfree(new);
    A_free(args);
    return(0);

err:
    SW_nfree(new); /* JMP 25-05-92 */
    A_free(args);
    B_seterror(B_msg(4));   /* New sample invalid */
    return(-1);
}

B_WsClear(arg, type)
char    *arg;
int     type;
{
    B_WsDescr("", type);
    B_WsName("", type);
    return(K_clear(K_WS[type]));
}

B_WsClearAll(arg)
char    *arg;
{
    int i;

    for(i = K_CMT; i <= K_VAR; i++) B_WsClear("", i);
    return(0);
}


B_WsDescr(arg, type)
char    *arg;
int     type;
{
    SCR_strlcpy(KDESC(K_WS[type]), arg, 50);
    return(0);
}

B_WsName(arg, type)
char    *arg;
int     type;
{
	//char	dir[K_MAX_FILE], file[K_MAX_FILE];				// JMP 3/6/2015
	
	// Save filename only (not path) in KDB
	//SCR_split_name(arg, dir, file)						// JMP 3/6/2015
	//SCR_strlcpy(KNAME(K_WS[type]), file, K_MAX_FILE - 1);	// JMP 3/6/2015
	
	// Save full name in K_WSNAME
	K_set_kdb_name(K_WS[type], arg);  // JMP 3/6/2015
    return(0);
}

B_WsCopy(arg, type)
char    *arg;
int     type;
{
    int     lg, shift = 0, rc = 0;
	char    file[K_MAX_FILE + 1], **files;
    char    **data, **data0, *K_expand(), *lst;
    U_ch    **SCR_inter();
    SAMPLE  *smpl = NULL;

    lg = B_get_arg0(file, arg, K_MAX_FILE);
    files = B_ainit_chk(file, NULL, 0);
    if(files == 0) return(-1);

    lst = K_expand(type, files[0], arg + lg, '*');
    data0 = B_ainit_chk(lst, NULL, 0);
    SCR_free(lst);
/*    data0 = B_ainit_chk(arg + lg, NULL, 0); */
    if(data0 == 0 || data0[0] == 0) {   /* JMP 24-06-98 */
	B_seterror(B_msg(134));
	SCR_free_tbl(files);
	SCR_free_tbl(data0);
	return(-1);
	}

    if(type == K_VAR && SCR_tbl_size(data0) >= 2) {
	smpl = PER_atosmpl(data0[0], data0[1]);
	if(smpl != NULL) shift = 2;
	else B_clear_last_error();
	}

    if(data0 + shift == NULL) {
	B_seterror(B_msg(135));
	rc = -1;
	}
    else {
	data = SCR_inter(data0 + shift, data0 + shift);
	rc = K_copy(K_WS[type],
		    SCR_tbl_size(files), files,
		    SCR_tbl_size(data), data,
		    smpl);
	SCR_free_tbl(data);
	}
    SCR_free_tbl(data0);
    SCR_free_tbl(files);
    SCR_free(smpl);

    if(rc < 0) return(-1);
    else return(0);
}

B_WsMerge(arg, type)
char    *arg;
int     type;
{
    int     lg, shift = 0, rc;
	char    file[K_MAX_FILE + 1];

    lg = B_get_arg0(file, arg, K_MAX_FILE);
    rc = K_cat(K_WS[type], file);
    return(rc);
}

B_WsExtrapolate(arg)
char    *arg;
{
    int     nb_args, p = 0, method = 0, rc = -1;
    char    **args, **vars;
    SAMPLE  *smpl;
    KDB     *kdb = K_WS[K_VAR];

    args = B_ainit_chk(arg, NULL, 0);
    nb_args = SCR_tbl_size(args);
    if (nb_args < 2) {
	B_seterror(B_msg(130));
	goto done;
    }
    else {
	method = atoi(args[0]);
	if(method < 0 || method > 6) method = 0;
	else p = 1;
    }

    smpl = PER_atosmpl(args[p], args[p + 1]);
    if(smpl == 0 || smpl->s_nb <= 0) {
	B_seterror(B_msg(131));
	goto done;
    }

    vars = args + p + 2;
    rc = KV_extrapolate(kdb, method, smpl, vars);

done:
    SW_nfree(smpl);
    SCR_free_tbl(args);
    return(rc);
}


B_WsAggrChar(arg)
char    *arg;
{
    if(arg == NULL || arg[0] == 0) K_AggrChar = ' ';
    else K_AggrChar = arg[0];
    return(0);
}

B_WsAggrSum(arg)
char    *arg;
{
    return(B_WsAggr(0, arg));
}

B_WsAggrProd(arg)
char    *arg;
{
    return(B_WsAggr(1, arg));
}

B_WsAggrMean(arg)
char    *arg;
{
    return(B_WsAggr(2, arg));
}

B_WsAggr(method, arg)
int     method;
char    *arg;
{
    int     nb_args, rc = -1;
    char    **args, *pattern = NULL;
    KDB     *kdb = K_WS[K_VAR], *nkdb = NULL;

    args = B_ainit_chk(arg, NULL, 0);
    nb_args = SCR_tbl_size(args);
    if (nb_args < 1) {
	B_seterror(B_msg(136));
	goto done;
    }
    pattern = SCR_stracpy(args[0]);
    nkdb = KV_aggregate(kdb, method, pattern, args[1]);
    KV_merge_del(kdb, nkdb, 1);
    rc = 0;

done:
    SCR_free(pattern);
    SCR_free_tbl(args);
    return(rc);
}

 IODE_REAL *B_StatUnitRoot_1(arg, print)
char    *arg;
int     print;
{
    int     lg, drift, trend, order, rc = -1;
    char    name[80], buf[1024];
    IODE_REAL    *df = NULL;
    extern  IODE_REAL *E_UnitRoot();

    lg = B_get_arg0(name, arg, K_MAX_NAME + 1);
    drift = atoi(name);

    lg = B_get_arg0(name, arg + lg + 1, K_MAX_NAME + 1) + lg;
    trend = atoi(name);

    lg = B_get_arg0(name, arg + lg + 1, K_MAX_NAME + 1) + lg;
    order = atoi(name);

    sprintf(buf, "_DF %s", arg + lg + 1);
    rc = B_DataCalcVar(buf);
    if(rc) return(NULL);

    df = E_UnitRoot("_DF", drift, trend, order);
    if(print && df != NULL) E_PrintDF(arg + lg + 1, df, drift, trend, order);

    B_DataDelete("_DF", K_VAR);

    if(df) {
	E_GetLecName(arg + lg + 1, name);
	sprintf(buf, "df_%s %lf", name, df[2]);
	B_DataUpdate(buf, K_SCL);
	}
    return(df);
}

B_StatUnitRoot(arg)
char    *arg;
{
    int     rc = -1;
    IODE_REAL    *df;

    df = B_StatUnitRoot_1(arg, 1);
    if(df) rc = 0;

    SCR_free(df);
    return(rc);
}


IODE_REAL    C_evallec(lec, t)
char    *lec;
{
    CLEC    *clec;
    IODE_REAL    x = L_NAN;

    SCR_strip(lec);
    if(lec[0]) {
	clec = L_cc(lec);
	if(clec == NULL) {
	    B_seterror("Syntax error %.80s", L_error());
	    return(x);
	}
	if(clec != 0 && !L_link(K_WS[K_VAR], K_WS[K_SCL], clec))
	    x = L_exec(K_WS[K_VAR], K_WS[K_SCL], clec, t);
	SW_nfree(clec);
    }

    return(x);
}










