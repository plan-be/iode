#include "s_ydvlp.h"

/* =+====== YY_include =================================================
    Read the keyword include, the name of the include file and opens it.


    include"filename" === include'filename' = relative to
    the current file path

&EN    return 0 or YY_ERROR and set error number
&EN yy : IO : YYFILE pointer
===================================================================== */

YY_include(yy)
YYFILE  *yy;
{
    int     ch;
    YYFILE  yy_buf, *yy_new;
    char filename[1024], dir[1024];

    YY_skip_spaces(yy);
    ch = YY_getc(yy);
    if(ch != YY_STRING_DELIMITER && ch != YY_CHAR_DELIMITER)
	return(YY_ERROR); /* JMP 15-09-10 */
    YY_read_string(yy, ch);/* JMP 15-09-10 */
    if(yy->yy_lg == 0) return(YY_ERROR);
    YY_skip_to_eol(yy);

    // Choice of filename
    strcpy(dir, yy->yy_name);
    SCR_split_dir(dir, filename);
#ifdef DOS
    if(dir[0] != 0 && dir[strlen(dir) - 1] != '\\') strcat(dir, "\\");
#else
    if(dir[0] != 0 && dir[strlen(dir) - 1] != '/') strcat(dir, "/"); // JMP 31/10/2012 !!!
#endif
    strcat(dir, yy->yy_text);

    yy_new = YY_open(dir, yy->yy_keys, yy->yy_nb_keys, YY_FILE);
    if(yy_new == 0) return(YY_ERROR);
    /* COPY NEW IN BUFFER, OLD IN NEW AND BUFFER IN OLD */
    memcpy((char *)&yy_buf, (char *)yy_new,  sizeof(YYFILE));
    memcpy((char *)yy_new,  (char *)yy,      sizeof(YYFILE));
    memcpy((char *)yy,      (char *)&yy_buf, sizeof(YYFILE));
    yy->yy_include = yy_new;

    /* SAVE DEFINES */
    yy->yy_nb_stdefs  = yy_new->yy_nb_stdefs;
    yy->yy_nb_dyndefs = yy_new->yy_nb_dyndefs;
    yy->yy_stdefs  =    yy_new->yy_stdefs ;
    yy->yy_dyndefs =    yy_new->yy_dyndefs;

    return(0);
}
