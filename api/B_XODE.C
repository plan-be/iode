#include "iode.h"

B_FileImportCmt(arg)
char    *arg;
{
    int     rc = 0, nb_args, format, lang;
    char    **args = NULL, *trace, *rule, *infile, *oufile;

    args = B_ainit_chk(arg, NULL, 0);
    nb_args = SCR_tbl_size(args);
    if(nb_args < 5) {
        B_seterrn(67);
        rc = -1;
        goto fin;
    }

    format = B_argpos("ARDDNGPT", args[0][0]);
    rule   = args[1];
    infile = args[2];
    oufile = args[3];
    lang   = B_argpos("EFD", args[4][0]);

    if(nb_args == 6) trace = args[5];
    else             trace = "";

    rc = X_RuleImport(K_CMT, trace, rule, oufile, infile,
                      "", "", format, lang);

fin:
    A_free(args);
    return(rc);
}

B_FileImportVar(arg)
char    *arg;
{
    int     rc = 0, nb_args, format, lang;
    char    **args = NULL, *trace, *rule, *from, *to,*infile, *oufile;

    args = B_ainit_chk(arg, NULL, 0);
    nb_args = SCR_tbl_size(args);    /* JMP 16-12-93 */
    if(nb_args < 6) {
        B_seterrn(67);
        rc = -1;
        goto fin;
    }

    format = B_argpos("ARDDNGPT", args[0][0]);
    rule   = args[1];
    infile = args[2];
    oufile = args[3];
    from   = args[4];
    to     = args[5];

    if(nb_args == 7) trace = args[6];
    else             trace = "";

    rc = X_RuleImport(K_VAR, trace, rule, oufile, infile,
                      from, to, format, 0);

fin:
    A_free(args);
    return(rc);
}




X_RuleImport(type, trace, rule, ode, asc, from, to, fmt, lang)
int     fmt, type, lang;
char    *rule, *ode, *asc, *trace;
char    *from, *to;
{
    switch(type) {
        case K_CMT   :
            X_RuleImportCmt(trace, rule, ode, asc, fmt, lang);
            break;

        case K_VAR   :
            X_RuleImportVar(trace, rule, ode, asc, from, to, fmt);
            break;

        default :
            break;
    }
    K_WARN_DUP = 0;
    return(0);
}

X_RuleImportCmt(trace, rule, ode, asc, fmt, lang)
int     fmt, lang;
char    *rule, *ode, *asc, *trace;
{
    int     rc = 0;
    KDB     *kdb;
    IMPDEF  *impdef;

    SCR_strip(trace);
    SCR_strip(rule);
    SCR_strip(ode);
    SCR_strip(asc);

    if(trace[0] != 0) {
        IMP_trace = 1;
        K_WARN_DUP = 0;
        W_dest(trace, W_A2M);
    }
    else {
        IMP_trace = 0;
        K_WARN_DUP = 1;
    }
    switch(fmt) {
        case 0:
            impdef = &IMP_ASC_CMT;
            break; /* JMP 11-01-99 */
        case 1:
            impdef = NULL;
            break;
        case 2:
            impdef = NULL;
            break;
        case 3:
            impdef = &IMP_BST_CMT;
            break;
        case 4:
            impdef = NULL;
            break;
        case 5:
            impdef = NULL;
            break;
        case 6:
            impdef = &IMP_PRN_CMT;
            break;
        case 7:
            impdef = &IMP_TXT_CMT;
            break;
    }

    kdb = IMP_InterpretCmt(impdef, rule, asc, lang);
    if(kdb != NULL) {
        K_save(kdb, ode);
        K_free(kdb);
    }

    if(IMP_trace) W_close();
    return(0);
}

X_RuleImportVar(trace, rule, ode, asc, from, to, fmt)
int     fmt;
char    *rule, *ode, *asc, *trace;
char    *from, *to;
{
    int     rc = 0;
    SAMPLE  *smpl;
    KDB     *kdb;
    IMPDEF  *impdef;

    SCR_strip(trace);
    SCR_strip(rule);
    SCR_strip(ode);
    SCR_strip(asc);

    if(trace[0] != 0) {
        IMP_trace = 1;
        K_WARN_DUP = 0;
        W_dest(trace, W_A2M);
    }
    else {
        IMP_trace = 0;
        K_WARN_DUP = 1;
    }

    switch(fmt) {
        case 0:
            impdef = &IMP_ASC;
            break;
        case 1:
            impdef = &IMP_RASC;
            break;
        case 2:
            impdef = &IMP_DIF;
            break;
        case 3:
            impdef = &IMP_BST;
            break;
        case 4:
            impdef = &IMP_NIS;
            break;
        case 5:
            impdef = &IMP_GEM;
            break;
        case 6:
            impdef = &IMP_PRN;
            break;
        case 7:
            impdef = &IMP_TXT;
            break;
    }

    SCR_strip(from);
    SCR_strip(to);
    if(from[0] == 0 || to[0] == 0)
        smpl = (SAMPLE *) SW_nalloc(sizeof(SAMPLE));
    else smpl = PER_atosmpl(from, to);
    if(smpl == 0) return(-1);

    kdb = IMP_InterpretVar(impdef, rule, asc, smpl);
    if(kdb != NULL) {
        K_save(kdb, ode);
        K_free(kdb);
    }

    if(IMP_trace) W_close();
    return(0);
}
