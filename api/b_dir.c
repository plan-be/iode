#include "iode.h"

SCRSTAT **B_ScrStat;
char    B_ScrDir[SCR_FILE_LG + 1];
extern char  K_LABEL[];

char    *K_TYPES[] = {
    "Cmt",
    "Eqs",
    "Idt",
    "Lst",
    "Scl",
    "Tbl",
    "Var"
};

/*
B_FileList(path)
char *path;
{
    extern int  B_Dir_Menu();
    return(B_FileListGnl(path, B_Dir_Menu));
}
*/

C_dir_edit_fn(text, dirname, ss)
char *text, *dirname;
SCRSTAT *ss;
{
    int     type, nobjs;
    char    dt[15], tm[15], filename[K_MAX_FILE + 1], descr[81], *atype;
    extern char *K_TYPES[];

    SCR_dir_join(filename, dirname, ss->ss_name);
    type = K_filetype(filename, descr, &nobjs, 0L);
    if(type < 0) atype = "";
    else         atype = K_TYPES[type];

    SCR_mode_fmt(ss->ss_mode, text);
    SCR_long_to_fdate(ss->ss_mdate, dt, "dd/mm/yy");
    /*    SCR_long_to_ftime(ss->ss_mtime, tm, "hh:mm"); */
    sprintf(text + 10, " %s %-12.12s %4d %-3.3s %-25.25s",
            dt, ss->ss_name, nobjs, atype, descr);
    return(0);
}
/* Annul‚ JMP 22-09-11
B_FileList(path)
char *path;
{
    char    dir[SCR_FILE_LG + 1];
    char    file[SCR_FILE_LG + 1];

    SCR_dir_edit(path, dir, file, 5, 2, 10, SCR_REVERSE, C_dir_edit_fn);
    return(-1);
}
*/

/* Annul‚ JMP 22-09-11
B_FileList2(path)
char *path;
{
    extern int  B_dir_ed_prt();
    return(B_FileListGnl(path, B_dir_ed_prt));
}
*/
/*
MENU *B_dir_ed_menu(path)
char *path;
{
    MENU    *mn = 0;
    int     day, month, year, minute, hour, nb_obj, i = 0;
    char    *name, desc[K_MAX_DESC], label[50];
    long    size;

    mn = MN_create_default( 5 , 0, SCR_REVERSE, 10);
    MN_set_title(mn, "ÄÄFILE NAMEÄÄÄÄÄÄSIZEÄÄÄÄÄÄDATEÄÄÄTIMEÄÄÄNBÄÄÄÄÄÄÄÄÄÄÄÄCOMMENTÄÄÄÄÄÄÄÄÄÄÄ");

    for(i = 0 ; B_ScrStat[i] ; i++) {
	B_analyse_file(B_ScrStat[i], &name, &day, &month, &year, &minute, &hour,
			       desc, &size, &nb_obj);
	MN_add_set_option_text(mn,
	    "%-12.12s³%8ld³%2.2d/%2.2d/%4.4d³%2.2d:%2.2d³%7d³%-25s",
	    name,
	    size,
	    day, month, year,
	    hour, minute,
	    nb_obj, desc);
	}
    if(i != mn->mn_nb_opts)
	MN_set_option_text(mn, mn->mn_nb_opts - 1, "...LOW MEMORY...");
    MN_set_footnote(mn, "%d files found for mask %s", i, path);
    MN_end(mn);
    return(mn);
}

B_Dir_Menu(path)
char *path;
{
    MENU    *mn;
    int choice = 0;

    if((mn = B_dir_ed_menu(path)) == NULL) return(-2);
    choice = MN_edit(mn);
    MN_free(mn);

    return(choice);
}
*/
void B_Read_Dir(path)
char *path;
{
    char    file[SCR_FILE_LG + 1];

    B_ScrStat = SCR_dir(path, 2, B_ScrDir, file);
}

B_FileListGnl(path, fn)
char    *path;
int (*fn)();
{
    int     choice;

    B_ScrStat = NULL;
    B_Read_Dir(path);
    if(B_ScrStat == NULL) {
        B_seterror("No file found for mask %s", path);
        return(-2);
    }
    choice = (*fn)(path);
    SCR_free_tbl((U_ch **)B_ScrStat);
    return(choice);
}

void B_path_change(path)
char *path;
{
    SCR_replace(path, "\\", "/");
}

void B_analyse_file(ss, name, day, month, year, minute, hour, desc, size, nb_obj)
SCRSTAT     *ss;
char        **name, *desc;
int         *day, *month, *year, *minute, *hour, *nb_obj;
long        *size;
{
    KDB     kdb;
    FILE    *fd;
    int     ode_file = 0, lg = sizeof(KDB), rd;
    char    label[128], c, buf[SCR_FILE_LG + 1];

#ifdef DOS
    c = '\\';
#else
    c = '/';
#endif
    *nb_obj = 0;
    *size    = ss->ss_size;
    *day     = ss->ss_mdate % 100L;
    *month   = (ss->ss_mdate / 100) % 100;
    *year     = ss->ss_mdate / 10000L;
    *minute   = (ss->ss_mtime / 100) % 100;
    *hour     = ss->ss_mtime / 10000L;
    if((*name = strrchr(ss->ss_name, c)) == NULL)
        *name = ss->ss_name;
    else(*name)++;

    if((ss->ss_mode & SCR_FTYPE_DIR)) {
        strcpy(desc, "DIRECTORY");
        return;
    }
    SCR_dir_join(buf, B_ScrDir, ss->ss_name);
#ifdef DOS
    fd = fopen(buf, "rb");
#else
    fd = fopen(buf, "rb");
#endif
    if(fd == 0) {
        strcpy(desc, "Unable to open file !");
        return;
    }
    fread(label, strlen(K_LABEL), 1, fd);
    if(K_calcvers(label) < 0) {                          /* JMP 09-04-98 */
        /*    if(memcmp(label, K_LABEL, strlen(K_LABEL)) == 0) { /* JMP 09-04-98 */
        ode_file = 1;
        rd = fread((char *) &kdb, 1, lg, fd);
        if(rd != lg) ode_file = 0;
    }
    fclose(fd);

    if(ode_file == 0) strcpy(desc, "Not ODE file");
    else {
        strcpy(desc, kdb.k_desc);
        SCR_strip(desc);
        if(desc[0] == 0) strcpy(desc, "ODE file without comment");
        *nb_obj = kdb.k_nb;
    }
}


B_dir_ed_prt(path)
char *path;
{
    int     day, month, year, minute, hour, i = 0, nb_obj;
    char    desc[K_MAX_DESC], label[50], *name;
    long    size;

    B_path_change(path);
    W_printf(".tb 6\n");
    W_printf(".sep &\n");
    W_printf("&6LFile list for path: %s\n", path);
    W_printf("&1LFILE NAME&1LSIZE&1LDATE&1LTIME&1LNB&1LCOMMENT\n");
    W_printf(".tl\n");

    for(i = 0 ; B_ScrStat[i] ; i++) {
        B_analyse_file(B_ScrStat[i], &name, &day, &month, &year, &minute, &hour,
                       desc, &size, &nb_obj);
        W_printf(
            "&1L%s&1L%ld&1L%2.2d/%2.2d/%4.4d&1L%2.2d:%2.2d&1L%d&1L%s\n",
            name,
            size,
            day, month, year,
            hour, minute,
            nb_obj, desc);
    }
    W_printf("&6L%d files found.\n", i);
    W_printf(".te\n");
    return(0);
}






