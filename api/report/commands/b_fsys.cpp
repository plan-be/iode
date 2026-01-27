/**
 *  @header4iode
 * 
 *  Functions acting on files called by the report engine (see also b_rep_syntax.c). 
 *  
 *  List of Functions
 *  -----------------
 *      int B_SysRename(char* arg, int unused)      $SysMoveFile filein fileout
 *      int B_SysCopy(char* arg, int unused)        $SysCopyFile filein fileout
 *      int B_SysAppend(char* arg, int unused)      $SysAppendFile filein fileout
 *      int B_SysDelete(char* arg, int unused)      $SysDeleteFile file1 file2 ...
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
 *
 *  
 *  @params See b_data.c for details
 *  @return int     0 on success, -1 on error (file not found, syntax error...)
 */

int B_SysRename(char* arg, int unused) 
{
    U_ch    **tbl = 0;
    int     ntbl = 0, rc = 0;

    tbl = SCR_vtom((unsigned char*) arg, (int) ' ');
    ntbl = SCR_tbl_size(tbl);
    if(ntbl != 2) {
        error_manager.append_error("Failed to rename file due to syntax error: '" + std::string(arg) + "'");
        rc = -1;
        goto fin;
    }

    rc = SCR_rename((char*) tbl[0], (char*) tbl[1]);
    if(rc) 
        error_manager.append_error("Failed to rename file: '" + std::string(arg) + "'");

fin:
    SCR_free_tbl(tbl);
    return(rc);
}


/**
 *  Syntax: $SysCopyFile filein fileout
 *  
 *
 */
int B_SysCopy(char* arg, int unused) 
{
    U_ch    **tbl = 0;
    int     ntbl = 0, rc = 0;

    tbl = SCR_vtom((unsigned char*) arg, (int) ' ');
    ntbl = SCR_tbl_size(tbl);
    if(ntbl != 2) {
        error_manager.append_error("Failed to copy file dut to syntax error: '" + std::string(arg) + "'");
        rc = -1;
        goto fin;
    }

    rc = SCR_copy_file((char*) tbl[0], (char*) tbl[1]);
    if(rc) 
        error_manager.append_error("Failed to copy file: '" + std::string(arg) + "'");

fin:
    SCR_free_tbl(tbl);
    return(rc);
}


/**
 *  Syntax: $SysAppendFile filein fileout
 *  
 *
 */
int B_SysAppend(char* arg, int unused) 
{
    U_ch    **tbl = 0;
    int     ntbl = 0, rc = 0;

    tbl = SCR_vtom((unsigned char*) arg, (int) ' ');
    ntbl = SCR_tbl_size(tbl);
    if(ntbl != 2) {
        error_manager.append_error("Failed to append file due to syntax error: '" + std::string(arg) + "'");
        rc = -1;
        goto fin;
    }

    rc = SCR_append_file((char*) tbl[0], (char*) tbl[1]);
    if(rc) 
        error_manager.append_error("Failed to append file: '" + std::string(arg) + "'");

fin:
    SCR_free_tbl(tbl);
    return(rc);
}


/**
 *  Syntax: $SysDeleteFile file1 file2 ...
 *  
 *
 */
int B_SysDelete(char* arg, int unused) 
{
    U_ch    **tbl = 0;
    int     ntbl = 0, rc = 0, i;

    tbl = SCR_vtom((unsigned char*) arg, (int) ' ');
    ntbl = SCR_tbl_size(tbl);
    if(ntbl < 1) {
        error_manager.append_error("Failed to delete file(s) due to syntax error: '" + std::string(arg) + "'");
        rc = -1;
        goto fin;
    }

    for(i = 0 ; i < ntbl ; i++) 
        _unlink((char*) tbl[i]);

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

    tbl = SCR_vtom((unsigned char*) arg, (int) ' ');
    ntbl = SCR_tbl_size(tbl);
    if(ntbl != 2) {
        std::string error_msg = "Failed to convert to UTF-8. Input and output file required: '";
        error_msg += std::string(arg) + "'";
        error_manager.append_error(error_msg);
        rc = -1;
        goto fin;
    }

    if(isansi) 
        rc = SCR_ConvertToUTF8((char*) tbl[0], (char*) tbl[1], 1, 1);
    else 
        rc = SCR_ConvertToUTF8((char*) tbl[0], (char*) tbl[1], 1, 0);

    if(rc) 
        error_manager.append_error("Failed to convert to UTF-8: '" + std::string(arg) + "'");

fin:
    SCR_free_tbl(tbl);
    return(rc);
}

/**
 *  Syntax: $SysOemToUTF8 inputfile outputfile
 *  
 *
 */
int B_SysOemToUTF8(char *arg, int unused)
{
    return(B_SysOemAnsiToUTF8(arg, 0));
}


/**
 *  Syntax: $SysAnsiToUTF8 inputfile outputfile
 *  
 *
 */
int B_SysAnsiToUTF8(char *arg, int unused)
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
    
    tbl = SCR_vtom((unsigned char*) arg, (int) ' ');
    ntbl = SCR_tbl_size(tbl);
    if(ntbl != 2) {
        error_manager.append_error("Input and output file required: '" + std::string(arg) + "'");
        rc = -1;
        goto fin;
    }
    
#ifdef DOS
	fd1 = fopen((char*) tbl[0], "rb");
	fd2 = fopen((char*) tbl[1], "wb+");
#else
	fd1 = fopen((char*) tbl[0], "r");
	fd2 = fopen((char*) tbl[1], "w+");
#endif
	if(fd1 == 0 || fd2 == 0) {
        std::string error_msg = std::string((char*) tbl[0]) + " or " + std::string((char*) tbl[1]);
        error_msg += ": access denied";
	    error_manager.append_error(error_msg);
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
 *
 */
int B_SysAnsiToOem(char *arg, int unused)
{
     return(B_SysOemOrAnsiToAnsiOrOem(arg, 1));
}

/**
 *  Syntax: $SysOemToAnsi inputfile outputfile
 *  
 *
 */
int B_SysOemToAnsi(char *arg, int unused)
{
     return(B_SysOemOrAnsiToAnsiOrOem(arg, 0));
}
