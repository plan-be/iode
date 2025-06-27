/**
 *  @header4iode
 * 
 *  Functions acting on files called by the report engine (see also b_rep_syntax.c). 
 *  
 *  List of Functions
 *  -----------------
 *      int B_SysRename(char* arg)      $SysMoveFile filein fileout
 *      int B_SysCopy(char* arg)        $SysCopyFile filein fileout
 *      int B_SysAppend(char* arg)      $SysAppendFile filein fileout
 *      int B_SysDelete(char* arg)      $SysDeleteFile file1 file2 ...
 *      int B_SysOemToUTF8(char *arg)   $SysOemToUTF8 inputfile outputfile
 *      int B_SysAnsiToUTF8(char *arg)  $SysAnsiToUTF8 inputfile outputfile
 *      int B_SysAnsiToOem(char *arg)   $SysAnsiToOem inputfile outputfile    
 *      int B_SysOemToAnsi(char *arg)   $SysOemToAnsi inputfile outputfile
 */
#include "api/b_errors.h"

#include "api/report/commands/commands.h"


/**
 *  Report function to rename a file. 
 *  
 *  Syntax: $SysMoveFile filein fileout
 *  
 *  @see https://iode.plan.be/doku.php?id=sysmovefile
 *  
 *  @params See b_data.c for details
 *  @return int     0 on success, -1 on error (file not found, syntax error...)
 */

int B_SysRename(char* arg) 
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


/**
 *  Syntax: $SysCopyFile filein fileout
 *  
 *  @see https://iode.plan.be/doku.php?id=syscopyfile
 */
int B_SysCopy(char* arg) 
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


/**
 *  Syntax: $SysAppendFile filein fileout
 *  
 *  @see https://iode.plan.be/doku.php?id=sysappendfile
 */
int B_SysAppend(char* arg) 
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


/**
 *  Syntax: $SysDeleteFile file1 file2 ...
 *  
 *  @see https://iode.plan.be/doku.php?id=sysdeletefile
 */
int B_SysDelete(char* arg) 
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

    for(i = 0 ; i < ntbl ; i++) _unlink(tbl[i]);

fin:
    SCR_free_tbl(tbl);
    return(rc);
}

/**
 *  Converts an oem-850- or ansi-coded file to UTF8.
 *  
 *  @param [in] char*   arg     input and output filenames (required)
 *  @param [in] int     isansi  0 if the input file is oem-coded, 1 for ansi Windows-1252 
 *  @return     int             -1 on syntax error
 *                              return code of SCR_ConvertToUTF8() on error
 *  
 *  @see scr4/SCR_ConvertToUTF8()
 *  
 */
static int B_SysOemAnsiToUTF8(char *arg, int isansi)
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

/**
 *  Syntax: $SysOemToUTF8 inputfile outputfile
 *  
 *  @see https://iode.plan.be/doku.php?id=sysoemtoutf8
 */
int B_SysOemToUTF8(char *arg)
{
    return(B_SysOemAnsiToUTF8(arg, 0));
}


/**
 *  Syntax: $SysAnsiToUTF8 inputfile outputfile
 *  
 *  @see https://iode.plan.be/doku.php?id=sysansitoutf8
 */
int B_SysAnsiToUTF8(char *arg)
{
    return(B_SysOemAnsiToUTF8(arg, 1));
}


/**
 *  Convert an OEM-850 coded file to Windows-1252 encoding or vice-versa.
 *  
 *  @param [in] char*   arg     names of input and output files. 
 *                              Spaces are not allowed in the filenames.
 *                              The output file is replaced.
 *  @param [in] int     isansi  0 if input file is OEM-coded, 1 if it is ansi-coded
 *  @return     int             0 on success, -1 on error (syntax, file does not exist...)
 *  
 */
static int B_SysOemOrAnsiToAnsiOrOem(char *arg, int isansi)
{
    int     c;
    FILE    *fd1, *fd2;
    U_ch    **tbl = 0;
    int     ntbl = 0, rc = 0;
    
    tbl = SCR_vtom(arg, ' ');
    ntbl = SCR_tbl_size(tbl);
    if(ntbl != 2) {
        B_seterror("Input and output file required %s", arg);
        rc = -1;
        goto fin;
    }
    
#ifdef DOS
	fd1 = fopen(tbl[0], "rb");
	fd2 = fopen(tbl[1], "wb+");
#else
	fd1 = fopen(tbl[0], "r");
	fd2 = fopen(tbl[1], "w+");
#endif
	if(fd1 == 0 || fd2 == 0) {
	    B_seterror("%s or %s: access denied\n", tbl[0], tbl[1]);
	    if(fd1 != 0) fclose(fd1);
	    if(fd2 != 0) fclose(fd2);
	    rc = -1;
        goto fin;
	}

    while(!feof(fd1)) {
	    c = getc(fd1);
	    if(c == EOF) break;
        if(isansi) c = SCR_AnsiToOemChar(c);
        else       c = SCR_OemToAnsiChar(c);
        putc(c, fd2);
	}
	fclose(fd1);
	fclose(fd2);

fin:
    SCR_free_tbl(tbl);
    return(rc);
}

/**
 *  Syntax: $SysAnsiToOem inputfile outputfile
 *  
 *  @see https://iode.plan.be/doku.php?id=sysansitooem
 */
int B_SysAnsiToOem(char *arg)
{
     return(B_SysOemOrAnsiToAnsiOrOem(arg, 1));
}

/**
 *  Syntax: $SysOemToAnsi inputfile outputfile
 *  
 *  @see https://iode.plan.be/doku.php?id=sysoemtoansi
 */
int B_SysOemToAnsi(char *arg)
{
     return(B_SysOemOrAnsiToAnsiOrOem(arg, 0));
}
