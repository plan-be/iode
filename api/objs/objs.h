#pragma once 

#include "api/constants.h"
#include "api/utils/time.h"
#include "api/objs/kdb.h"

#ifdef __cplusplus
extern "C" {
#endif

/* k_objs.c */
extern int K_key(char *,int );
extern int K_dup(KDB *,char *,KDB *,char *);
extern int K_ren(KDB *,char *,char *);
extern int K_add_entry(KDB *,char *);
extern int K_find(KDB *,char *);
extern int K_del(KDB *,int );
extern int K_del_entry(KDB *,int );
extern int K_del_by_name(KDB* kdb, char* name);
extern int K_upd_eqs(char* name, char* lec, char* cmt, int method, SAMPLE* smpl, char* instr, char* blk, float* tests, int date);
extern int K_upd_tbl(char* name, char* arg);

/* k_objsv.c */
extern int K_add(KDB *kdb, char* name,...);

/* k_objvers.c */
extern int K_calcvers(char *);
extern void K_setvers(KDB* kdb, int i, int vers);

/* k_objfile.c */
extern char *K_add_ext(char* filename, char* ext);
extern int K_get_ext(char* filename, char* ext, int max_ext_len);
extern int K_has_ext(char* filename);
extern char *K_set_ext(char *,char *,int );
extern char *K_set_ext_asc(char *,char *,int );
extern void K_strip(char *);
extern int K_merge(KDB *,KDB *,int );
extern int K_merge_del(KDB *,KDB *,int );
extern KDB *K_load(int ,char *,int ,char **);
extern KDB *K_interpret(int ,char *);
extern int K_copy(KDB *,int ,char **,int ,char **,SAMPLE *);
extern int K_cat(KDB *,char *);
extern int K_set_backup_on_save(int take_backup);
extern int K_get_backup_on_save();
extern int K_backup(char *);
extern int K_save(KDB *,char *);
extern int K_save_ws(KDB *);
extern int K_setname(char *,char *);
extern int K_filetype(char *,char *,int *,SAMPLE *);

#ifdef __cplusplus
}
#endif
