#pragma once
#include "s_dir.h"

#ifdef __cplusplus
extern "C" {
#endif

/* s_dir*.c */
extern char *SCR_getcwd(int drive,char *dir);
extern int SCR_dosgetdrive();
extern int SCR_dossetdrive(int ndrive);
extern int SCR_dosgetcwd(char* dir, int lg);
extern int SCR_dir_edit_fn(char *text,char *dirname,SCRSTAT *ss);
extern SCRSTAT *SCR_dir_edit_1(char *path,int add_dir,char *dir,char *file,int line,int col,int nl,int attr,int (*fn)());
extern SCRSTAT *SCR_dir_edit(char *path,char *dir,char *file,int line,int col,int nl,int attr,int (*fn)());
extern int SCR_dir_select(char *path,char *res,int line,int col,int nl,int attr,int (*fn)());
extern int SCR_mode_fmt_1(unsigned int mode,char *txt);
extern int SCR_mode_fmt(unsigned int mode,char *txt);
extern char *SCR_fullpath(char *name,char *res);
extern char *SCR_dir_join(char *res,char *dir,char *file);
extern int SCR_is_dir(char *dirname);
extern SCRDIR *SCR_opendir(char *dirname);
extern SCRSTAT *SCR_readdir(SCRDIR *sdir,int getstat);
extern int SCR_closedir(SCRDIR *sdir);
#ifdef DOS
extern int SCR_dos_stat(struct find_t *ff,SCRSTAT *ss);
#endif
extern char *SCR_relpath(char *dir,char *file,char *res);
extern int SCR_split_dir(char *dir,char *file);
extern int SCR_split_name(char *path,char *dir,char *file);
extern int SCR_stat(char *filename,SCRSTAT *ss);
extern int SCR_stat_ex(char *filename,SCRSTAT *ss, int crc);
extern int SCR_dir_std(char *res,char *dir);
extern SCRSTAT **SCR_dir(char *path,int add_dir,char *dir,char *filename);

#ifdef __cplusplus
}
#endif
