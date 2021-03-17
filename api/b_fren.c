#include "iode.h"


B_FileRename(arg, type)
char *arg;
int  type;
{
    char    **args;
    int     rc = 0;
	char    buf_from[1024]; 	// JMP 15-09-2015
    char    buf_to[1024];		// JMP 15-09-2015


    args = B_ainit_chk(arg, 0L, 2);
    if(args == 0) {
	rc = -1;
	goto fin;
    }

    K_set_ext(buf_from, args[0], type);
    K_set_ext(buf_to, args[1], type);
    if(SCR_rename(buf_from, buf_to))  rc = -1;
    else
	if(type <= K_VAR && K_setname(type, buf_to, buf_to)) rc = -1;

fin:
    if(rc < 0) B_seterror("Unable to rename %s", buf_from);
    A_free(args);
    return(rc);
}


B_SysRename(arg) /* JMP 07-08-98 */
char    *arg;
{
    U_ch    **tbl = 0;
    int     ntbl = 0, rc = 0;

    tbl = SCR_vtom(arg, ' ');
    ntbl = SCR_tbl_size(tbl);
    if(ntbl != 2) {
	B_seterror("Syntax error %s", arg);
	rc = -1;
	goto fin;
	}

    rc = SCR_rename(tbl[0], tbl[1]);
    if(rc) B_seterror("Rename failed (%s)", arg);

fin:
    SCR_free_tbl(tbl);
    return(rc);
}

B_SysCopy(arg) /* JMP 07-08-98 */
char    *arg;
{
    U_ch    **tbl = 0;
    int     ntbl = 0, rc = 0;

    tbl = SCR_vtom(arg, ' ');
    ntbl = SCR_tbl_size(tbl);
    if(ntbl != 2) {
	B_seterror("Syntax error %s", arg);
	rc = -1;
	goto fin;
	}

    rc = SCR_copy_file(tbl[0], tbl[1]);
    if(rc) B_seterror("Copy failed (%s)", arg);

fin:
    SCR_free_tbl(tbl);
    return(rc);
}

B_SysAppend(arg) /* JMP 07-08-98 */
char    *arg;
{
    U_ch    **tbl = 0;
    int     ntbl = 0, rc = 0;

    tbl = SCR_vtom(arg, ' ');
    ntbl = SCR_tbl_size(tbl);
    if(ntbl != 2) {
	B_seterror("Syntax error %s", arg);
	rc = -1;
	goto fin;
	}

    rc = SCR_append_file(tbl[0], tbl[1]);
    if(rc) B_seterror("Copy failed (%s)", arg);

fin:
    SCR_free_tbl(tbl);
    return(rc);
}

B_SysDelete(arg) /* JMP 07-08-98 */
char    *arg;
{
    U_ch    **tbl = 0;
    int     ntbl = 0, rc = 0, i;

    tbl = SCR_vtom(arg, ' ');
    ntbl = SCR_tbl_size(tbl);
    if(ntbl < 1) {
	B_seterror("Syntax error %s", arg);
	rc = -1;
	goto fin;
	}

    for(i = 0 ; i < ntbl ; i++) unlink(tbl[i]);

fin:
    SCR_free_tbl(tbl);
    return(rc);
}

B_SysOemAnsiToUTF8(char *arg, int isansi) 
{
    U_ch    **tbl = 0;
    int     ntbl = 0, rc = 0;

    tbl = SCR_vtom(arg, ' ');
    ntbl = SCR_tbl_size(tbl);
    if(ntbl != 2) {
        B_seterror("Input and output file required %s", arg);
        rc = -1;
        goto fin;
	}

    if(isansi) rc = SCR_ConvertToUTF8(tbl[0], tbl[1], 1, 1);
    else       rc = SCR_ConvertToUTF8(tbl[0], tbl[1], 1, 0);
    
    if(rc) B_seterror("Conversion to UTF8 failed (%s)", arg);

fin:
    SCR_free_tbl(tbl);
    return(rc);
}

B_SysOemToUTF8(char *arg) 
{
    return(B_SysOemAnsiToUTF8(arg, 0));
}
B_SysAnsiToUTF8(char *arg)
{
    return(B_SysOemAnsiToUTF8(arg, 1));
}
B_SysOemToAnsi(char *arg)
{
}
B_SysAnsiToOem(char *arg)
{
}
