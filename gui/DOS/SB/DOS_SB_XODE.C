#include <iodedos.h>

SB_XodeRuleImport() {
    PG_edit_window(xp_ruleimport);
    return(0);
}

C_XodeRuleImport() {
    return(X_RuleImport(xp_ruleimport_TYPE,
                        xp_ruleimport_TRACE, xp_ruleimport_RULE,
                        xp_ruleimport_ODE, xp_ruleimport_FILE,
                        xp_ruleimport_FROM, xp_ruleimport_TO,
                        xp_ruleimport_FORMAT, xp_ruleimport_LANG));
}



SB_XodeRuleExport() {
    PG_edit_window(xp_ruleexport);
    return(0);
}

C_XodeRuleExport() {
    return(X_RuleExport(xp_ruleexport_TRACE, xp_ruleexport_RULE,
                        xp_ruleexport_OUT, xp_ruleexport_VFILE, xp_ruleexport_CFILE,
                        xp_ruleexport_FROM, xp_ruleexport_TO,
                        xp_ruleexport_NA, xp_ruleexport_SEP,
                        xp_ruleexport_FMT));
}


X_RuleExport(trace, rule, out, vfile, cfile, from, to, na, sep, fmt)
char    *trace, *rule, *out, *vfile, *cfile, *from, *to, *na, *sep;
int     fmt;
{
    KDB     *dbv = NULL, *dbc = NULL;
    SAMPLE  *smpl;
    EXPDEF  *expdef;
    int     rc = 0;

    SCR_strip(trace);
    SCR_strip(rule);
    SCR_strip(out);
    SCR_strip(vfile);
    SCR_strip(cfile);

    SCR_strip(from);
    SCR_strip(to);
    if(from[0] == 0 || to[0] == 0) smpl = NULL;
    else smpl = PER_atosmpl(from, to);

    if(trace[0] != 0) {
        K_WARN_DUP = 0;
        W_dest(trace, W_A2M);
    }
    else {
        IMP_trace = 0;
        K_WARN_DUP = 1;
    }

    switch(fmt) {
        case 0:
            expdef = &EXPCSV;
            break;     /* JMP 28-08-98 */
        case 1:
            expdef = &EXPDIF;
            break;
        case 2:
            expdef = &EXPWKS;
            break;
        case 3:
            expdef = &EXPTSP;
            break;
        default:
            expdef = &EXPRCSV;           /* JMP 28-08-98 */
    }

    if(vfile[0] != 0) {
        dbv = K_interpret(K_VAR, vfile);
        if(dbv == 0) return(-1); // JMP 29-05-2012
        if(smpl != NULL) KV_sample(dbv, smpl);
    }

    if(cfile[0] != 0)
        dbc = K_interpret(COMMENTS, cfile);    
    if(fmt < 4)
        rc = EXP_Ws(expdef, dbv, dbc, rule, out, na, sep); /* JMP 28-08-98 */
    else
        rc = EXP_Rev_Ws(expdef, dbv, dbc, rule, out, na, sep); /* JMP 28-08-98 */


    K_free(dbv);
    K_free(dbc);
    K_WARN_DUP = 0;
    if(rc) B_display_last_error();
    return(rc);
}














