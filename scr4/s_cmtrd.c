#include "scr.h"

extern FILE    *fd_cmt;

/*NH*/
SCR_open_cmt(mode)
int     mode;
{
    static int last_mode, first_call = 1;
    char    *pt;
#ifdef __cplusplus
    char    *SCR_search_file(char *);
#else
    char    *SCR_search_file();  /* JMP 07-01-98 */
#endif
    char    *open_mode;

    if(SCR_CMT_FILE[0] == 0)
	strcpy(SCR_CMT_FILE, "scr.cmt");

    if(mode == 1 && first_call) {
	first_call = 0;
	unlink(SCR_CMT_FILE);
	fd_cmt = fopen(SCR_CMT_FILE, "w+");
	if(fd_cmt != 0) fclose(fd_cmt);
	fd_cmt = 0;
    }

#ifdef DOS
    if(mode == 0) open_mode = "rb";
    else          open_mode = "r+b";
#else
    if(mode == 0) open_mode = "r";
    else          open_mode = "r+";
#endif

    if(fd_cmt != 0 && last_mode != mode) {
	fclose(fd_cmt);
	fd_cmt = 0;
    }
    if(fd_cmt == 0) {
	if(mode == 1)
	    pt = SCR_CMT_FILE;
	else
	    pt = SCR_search_file(SCR_CMT_FILE);

	if(pt == 0 || (fd_cmt = fopen(pt, open_mode)) == 0) {
	    if(ERR_TYPE == 0) SCR_mini_end(); /* JMP 04-03-92 */
	    else SCR_end();
	    if(pt == 0)
		Werror("Fatal : '%s' not found\n", SCR_CMT_FILE); /* JMP 17-01-98 */
	    else
		Werror("Fatal : '%s': found but not accessible\n", pt); /* JMP 17-01-98 */
	    exit(1);
	    }

	last_mode = mode;
    }
    if(mode == 1) fseek(fd_cmt, 0L, 2);
    else {
	if(first_call) {    /* JMP 15-10-98 */
	    SCR_CMT_FILE_POS = SCR_get_app_pos(fd_cmt, SCR_CMT_FILE_NB);
	    if(SCR_CMT_FILE_POS < 0) {      /* JMP_M 4.19 08-05-95 */
		Werror("Fatal : append error in '%s'\n", pt); /* JMP 17-01-98 */
		exit(1);
		}
	    first_call = 0; /* JMP 15-10-98 */
	    }
	}
    return(0);
}

/*NH*/
CMT_read_text(txt)
char    **txt;
{
    short    i;

    if(*txt == 0) return(0);
    fread(&i, sizeof(short), 1, fd_cmt);
    *txt = SCR_malloc(i);
    fread(*txt, i, 1, fd_cmt);
    return(0);
}

/*NH*/
CMT_read_len(txt)
char    **txt;
{
    short    i;

    fread(&i, sizeof(short), 1, fd_cmt);
    if(i == 0) return(0);
    *txt = SCR_malloc(i);
    fread(*txt, i, 1, fd_cmt);
    return(0);
}



