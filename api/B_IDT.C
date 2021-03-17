#include "iode.h"

B_IdtExecute(arg)
char    *arg;
{
    int     lg1, lg2;
    int     rc = 0;
    char    from[16], to[16], **idts;
    SAMPLE  *smpl;

    lg1 = B_get_arg0(from, arg, 15);
    lg2 = B_get_arg0(to, arg + lg1, 15);
    smpl = PER_atosmpl(from, to);

    if(smpl == NULL) {
	B_seterror("IdtExecute: %s %s wrong sample", from, to);
	return(-1);
    }

    idts = B_ainit_chk(arg + lg1 + lg2, NULL, 0);

    rc = B_IdtExecuteIdts(smpl, idts);

    SCR_free_tbl(idts);
    SCR_free(smpl);
    return(rc);
}

B_IdtExecuteIdts(smpl, idts)
SAMPLE  *smpl;
char    **idts;
{
    KDB     *tdbi, *tkdb;

    if (idts == NULL || SCR_tbl_size(idts) == 0)
	tkdb = KI_exec(KI_WS,
		    KV_WS, SCR_tbl_size(KEXEC_VFILES), KEXEC_VFILES,
		    KS_WS, SCR_tbl_size(KEXEC_SFILES), KEXEC_SFILES,
		    smpl);
    else {
	tdbi = K_refer(KI_WS, SCR_tbl_size(idts), idts);
	tkdb = KI_exec(tdbi,
		    KV_WS, SCR_tbl_size(KEXEC_VFILES), KEXEC_VFILES,
		    KS_WS, SCR_tbl_size(KEXEC_SFILES), KEXEC_SFILES,
		    smpl);
	K_free_kdb(tdbi);
    }

    SCR_free_tbl(KEXEC_VFILES);
    KEXEC_VFILES = NULL;
    SCR_free_tbl(KEXEC_SFILES);
    KEXEC_SFILES = NULL;

    if(tkdb == NULL) return(-1);
    KV_sample(tkdb, smpl);

    if(KV_WS != NULL) KV_merge_del(KV_WS, tkdb, 1);
    else KV_WS = tkdb;

    return(0);
}

B_IdtExecuteVarFiles(arg)
char    *arg;
{
    SCR_free_tbl(KEXEC_VFILES);
    KEXEC_VFILES = B_ainit_chk(arg, NULL, 0);
    return(0);
}

B_IdtExecuteSclFiles(arg)
char    *arg;
{
    SCR_free_tbl(KEXEC_SFILES);
    KEXEC_SFILES = B_ainit_chk(arg, NULL, 0);
    return(0);
}

B_IdtExecuteTrace(arg)
char    *arg;
{
    KEXEC_TRACE = (SCR_upper_char(arg[0]) == 'Y') ? 1 : 0;  /* JMP 19-12-97 */
    return(0);
}



