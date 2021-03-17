#include "iode.h"

char    **IMP_rule  = NULL;
char    **IMP_pat  = NULL;
int     IMP_trace = 0;

int IMP_readrule(filename)
char    *filename;
{
    int         nbr = 0, nbp = 0, blen;
    char        buf[257], **tbl;
    FILE        *fd = NULL;

    if(filename == NULL ) goto done;

    SCR_strip(filename);
    if(filename[0] == 0) goto done;

    fd = fopen(filename, "r");
    if (fd == NULL) {
		kerror(0, "Rule file %s does not exist", filename);
		goto done;
    }

    while(1) {
		if(fgets(buf, 256, fd) == NULL)  goto done;
		while(1) { /* strip newline, carriage-return */
		    blen = strlen(buf);
		    if(isspace(buf[blen - 1])) buf[blen - 1] = '\0';
		    else break;
		}
		tbl = SCR_vtoms(buf, " \t");
		if(SCR_tbl_size(tbl) != 2) {
		    SCR_free_tbl(tbl);
		    continue;
		}

		SCR_add_ptr(&IMP_pat, &nbp, tbl[0]);
		SCR_add_ptr(&IMP_rule, &nbr, tbl[1]);
		SCR_free_tbl(tbl);
    }

done:
    if(nbr == 0 || (nbr != nbp)) {
		SCR_free_tbl(IMP_pat);
		SCR_free_tbl(IMP_rule);
		IMP_pat  = IMP_rule = NULL;
		nbp = nbr = 0;
		SCR_add_ptr(&IMP_pat,  &nbp, "*");
		// SCR_add_ptr(&IMP_rule, &nbr, "++++++++++");
		SCR_add_ptr(&IMP_rule, &nbr, "++++++++++++++++++++"); /* JMP 11-08-11 */
    }

    if(fd) fclose(fd);
    SCR_add_ptr(&IMP_pat, &nbp, NULL);
    SCR_add_ptr(&IMP_rule, &nbr, NULL);
    return(0);
}

int IMP_grep(rule, in)
char    **rule;
char    *in;
{
    int i, rc = -1;

    for(i = 0; i < SCR_tbl_size(rule); i++) {
		rc = SCR_grep(rule[i], in, 0);
		if(rc == 0) return(i);
    }
    return(-1);
}

int IMP_change(rule, pat, in, out)
char    **rule, **pat;
char    *in, *out;
{
    int rc = -1,
	r, i, o, rlen, ilen, idone = 0;

    rc = IMP_grep(pat, in);
    if(rc < 0) return(-1);

    rlen = strlen(rule[rc]);
    ilen = strlen(in);
    for(r = i = o = 0; r < rlen && !idone && o < K_MAX_NAME ; r++) {
		switch(rule[rc][r]) {
		    case '-' :   if(i >= ilen) idone = 1;
				 else i++;
				 break;
		    case '+' :   if(i >= ilen) idone = 1;
				 else {
				    out[o] = in[i];
				    i++; o++;
				 }
				 break;
		    default  :   out[o] = rule[rc][r]; o++; break;
		}
    }

    out[o] = '\0';

    if(IMP_trace)
		W_printf("%s -> %s\t(Rule %s)\n", in, out, rule[rc]);

    return(0);
}


KDB *IMP_InterpretVar(impdef, rulefile, vecfile, smpl)
IMPDEF  *impdef;
char    *rulefile, *vecfile;
SAMPLE  *smpl;
{
    KDB     *kdb = 0;
    int     i, nb, size, pos, shift = 0, cmpt = 0;
    char    iname[256];
    ONAME   oname;
    IODE_REAL    *vector = NULL, value;
    YYFILE  *yy;

    if(smpl == NULL || IMP_readrule(rulefile) < 0) return(kdb);

    YY_CASE_SENSITIVE = 1;
    if(impdef->imp_keys != NULL) {
		size = impdef->imp_dim;
		qsort(impdef->imp_keys, size, sizeof(YYKEYS), YY_strcmp);
    }
    else size = 0;

    SCR_strip(vecfile);
    yy = YY_open(vecfile, impdef->imp_keys, size, YY_FILE);
    if(yy == 0) {
	kerror(0,"Cannot open '%s'", vecfile);
	return(kdb);
    }

    if(impdef->imp_hd_fn != NULL
	    && (*(impdef->imp_hd_fn))(yy, smpl) < 0) goto err;

    kdb = K_create(K_VAR, K_UPPER);
    memcpy((SAMPLE *) KDATA(kdb), smpl, sizeof(SAMPLE));
    nb = smpl->s_nb;

    if(impdef->imp_vec_fn != NULL) {
	vector = (IODE_REAL *) SW_nalloc(nb * sizeof(IODE_REAL));
	if(vector == NULL) goto err;

	while(1) {
	    for(i = 0; i < nb; i++) vector[i] = L_NAN;

	    if((*(impdef->imp_vec_fn))(yy, iname, nb, vector) < 0) break;

	    if(IMP_change(IMP_rule, IMP_pat, iname, oname) < 0) continue;
	    kmsg("Reading object %d : %s", ++cmpt, oname);
	    if(K_add(kdb, oname, vector, &nb) < 0)
		kerror(0, "Unable to create '%s'", oname);
	}
    }
    else {
	if(impdef->imp_elem_fn == NULL) {
	    kerror(0, "Both vector and element function unavailable");
	    goto err;
	}

	while(1) {
	    if((*(impdef->imp_elem_fn))(yy, iname, &shift, &value) < 0) break;
	    if(shift < 0 || shift > nb) continue;

	    if(IMP_change(IMP_rule, IMP_pat, iname, oname) < 0) continue;


	    pos = K_find(kdb, oname);

if(SW_BLKS[7].blk_space > 100000L) Debug("%s\n", oname);

	    if(pos < 0) {
		kmsg("Reading object %d : %s", ++cmpt, oname);
		pos = K_add(kdb, oname, NULL, &nb);
		if(pos < 0) {
		    kerror(0, "Unable to create '%s'", oname);
		    goto err;
		}
	    }
	    KV_set(kdb, pos, shift, K_LEVEL, value);
	}

    }

    if(impdef->imp_end_fn != NULL
	    && (*(impdef->imp_end_fn))() < 0) goto err;

    YY_close(yy);
    SW_nfree(vector);
    return(kdb);

err:
    K_free(kdb);
    kdb = NULL;
    kerror(0, "%s : incorrect filter (%s : offending entry)",
		YY_error(yy), yy->yy_text);
    YY_close(yy);
    SW_nfree(vector);
    return(kdb);
}

KDB *IMP_InterpretCmt(impdef, rulefile, cfile, lang)
IMPDEF  *impdef;
char    *rulefile, *cfile;
int     lang; /* 0 = English, 1 = French , 2 = Dutch */
{
    KDB     *kdb = NULL;
    int     size, cmpt = 0;
    char    iname[256],
	    *cmt = NULL;
    ONAME   oname;

    if(IMP_readrule(rulefile) < 0) return(kdb);

    YY_CASE_SENSITIVE = 1;
    if(impdef->imp_keys != NULL) {
	size = impdef->imp_dim;
	qsort(impdef->imp_keys, size, sizeof(YYKEYS), YY_strcmp);
    }
    else size = 0;

    if(impdef->imp_hd_fn != NULL
	    && (*(impdef->imp_hd_fn))(impdef, cfile, lang) < 0) goto err;

    kdb = K_create(K_CMT, K_ASIS);

    if(impdef->imp_vec_fn != NULL) {

	while(1) {
	    if((*(impdef->imp_vec_fn))(iname, &cmt) < 0) break;

	    if(IMP_change(IMP_rule, IMP_pat, iname, oname) < 0) {
		SW_nfree(cmt);
		continue;
	    }

if(SW_BLKS[7].blk_space > 100000L) Debug("CMT:%s\n", oname);
	    kmsg("Reading object %d : %s", ++cmpt, oname);
	    SCR_strip(cmt);
	    if(K_add(kdb, oname, cmt) < 0)
		kerror(0, "Unable to create '%s'", oname);
	    SW_nfree(cmt);
	}
    }

    if(impdef->imp_end_fn != NULL
	    && (*(impdef->imp_end_fn))() < 0) goto err;

    return(kdb);

err:
    K_free(kdb);
    kdb = NULL;
    return(kdb);
}

/*
main(argc, argv)
int     argc;
char    **argv;
{
    int     i, rc;
    KDB     *kdb;
    SAMPLE  *smpl = PER_atosmpl("1980Y1", "1990Y1");
    IMPDEF  *impdef;

    if(argc != 5) {
	fprintf(stderr, "Usage: a type rule ifile ofile");
	exit(9);
    }

    switch(atoi(argv[1])) {
	case 0: impdef = &IMP_ASC; break;
	case 1: impdef = &IMP_RASC; break;
	case 2: impdef = &IMP_DIF; break;
	case 3: impdef = &IMP_BST; break;
	default : exit(0);
    }

    IMP_trace = 1;
    W_dest("imptrace.log", 0);
    kdb = IMP_interpret(impdef, argv[2], argv[3], smpl);
    if(kdb != NULL) {
	K_save(kdb, argv[4]);
	if(IMP_trace) (*K_save_asc[KTYPE(kdb)])(kdb, "-");
	K_free(kdb);
    }
    W_close();
    exit(0);
}
*/




DebugSwap()
{
    if(SW_BLKS[7].blk_space > 100000L) Debug("");
}
