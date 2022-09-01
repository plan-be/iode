/**
 *  @header4iode
 * 
 *  Functions acting on files called by the report engine (see b_rep_syntax.c). 
 *  
 *  List of Functions
 *  ------------------
 *      int B_FileCopy(char* arg, int type)    $FileCopy<type> source_file dest_file
 *      int B_FileRename(char* arg, int type)  $FileRename<type> source_file dest_file
 *      int B_FileDelete(char* arg, int type)  $FileDelete<type> file1 [file2...]
 */

#include "iode.h"

/**
 *  Report function to copy a file. 
 *  For IODE object files, the filename in the KDB header is modified to reflect the
 *  new filename (calls K_setname()).
 *  
 *  Syntax:
 *  
 *       $FileCopy<type> source_file dest_file
 *
 *  @see https://iode.plan.be/doku.php?id=filecopy for details
 *  
 *  @params See b_data.c for details
 *  @return int     0 on success, -1 on error (file not found, syntax error...)
 */

int B_FileCopy(char* arg, int type)
{
    char    **args;
    int     rc = 0, ch;
    char    buf_from[1024]; // JMP 15-09-2015
    char    buf_to[1024];	// JMP 15-09-2015
    FILE    *from, *to;

    args = B_ainit_chk(arg, 0L, 2);
    if(args == 0) {
        rc = -1;
        goto fin;
    }

    K_set_ext(buf_from, args[0], type);
    K_set_ext(buf_to, args[1], type);

    if((from = fopen(buf_from, "r+b")) == 0) {
        B_seterror("FileCopy: unable to open file %.80s", buf_from);
        rc = -1;
        goto fin;
    }

    if((to = fopen(buf_to, "w+b")) == 0) {
        B_seterror("FileCopy: unable to create file %.80s", buf_to);
        rc = -1;
        goto fin;
    }

    while((ch = getc(from)) != EOF)
        if(putc(ch, to) == EOF) {
            B_seterror("FileCopy: error writing %.80s", buf_to);
            rc = -1;
            goto fin;
        }
fin:
    if(from != 0) fclose(from);
    if(to != 0) {
        fclose(to);
        if(type <= K_VAR) K_setname(buf_to, buf_to);  // JMP 27/10/2021
    }
    A_free(args);
    return(rc);
}


/**
 *  Report function to rename a file. 
 *  For IODE object files, the filename in the KDB header is modified to reflect the
 *  new filename (calls K_setname()).
 *  
 *  Syntax:
 *  
 *      $FileRename<type> source_file dest_file
 *
 *  @see https://iode.plan.be/doku.php?id=filerename for details
 *  
 *  @params See b_data.c for details
 *  @return int     0 on success, -1 on error (file not found, syntax error...)
 */

int B_FileRename(char* arg, int type)
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
    else if(type <= K_VAR && K_setname(buf_to, buf_to)) rc = -1; // JMP 27/10/2021

fin:
    if(rc < 0) B_seterror("Unable to rename %s", buf_from);
    A_free(args);
    return(rc);
}


/**
 *  Sub-function of B_FileDelete() to delete one single file.
 *  
 *  @param [in] char*   arg  filename
 *  @param [in] int*    type file type (value defined in iode.h between K_CMT and K_CSV). See also K_set_ext().
 *  @return 
 */
 
static int B_unlink_1(char* arg, int *type)
{
    char    buf[512];

    /*    if(unlink(K_set_ext(buf, arg, *type)) == 0)    return(0);
        B_seterror("Cannot delete %s", buf);
        return(-1);
    /* JMP 11-11-93 */
    K_set_ext(buf, arg, *type);
    W_close();                      /* JMP 11-07-96 */
    unlink(buf);
    return(0);
}

/**
 *  Report function to delete a file(s) of a specified type. 
 *  
 *  Syntax:  
 *  
 *      $FileDelete<type> filename [filename...]
 *  
 *  @see https://iode.plan.be/doku.php?id=filedelete for details
 *  
 *  @params see b_data.c 
 *  @return int 0 always 
 */

int B_FileDelete(char* arg, int type)
{
    int     unlink();

    return(B_ainit_loop(arg, B_unlink_1, (char *)&type));
}
