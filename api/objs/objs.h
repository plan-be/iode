#pragma once 

#include "api/constants.h"
#include "api/utils/time.h"
#include "api/objs/kdb.h"


inline char K_LABELS[] = "KOBJS 4.02\032";  // Version 1
inline char K_LABELD[] = "KOBJS 504d\032";  // Version 2
inline char K_LABELX[] = "KOBJS 564A\032";  // Version 3 
inline char K_LABEL[]  = "KOBJS 564A\032";  // Version 0 = Current version = Version 3

/* k_objs.c */
int K_key(char *,int );
int K_dup(KDB *,char *,KDB *,char *);
int K_ren(KDB *,char *,char *);
int K_add_entry(KDB *,char *);
int K_find(KDB *,char *);
int K_del(KDB *,int );
int K_del_entry(KDB *,int );
int K_del_by_name(KDB* kdb, char* name);
int K_upd_eqs(char* name, char* lec, char* cmt, int method, SAMPLE* smpl, char* instr, char* blk, float* tests, int date);
int K_upd_tbl(char* name, char* arg);

/* k_objsv.c */
int K_add(KDB *kdb, char* name,...);

/* k_objvers.c */
int K_calcvers(char *);
void K_setvers(KDB* kdb, int i, int vers);

/* k_objfile.c */
char *K_add_ext(char* filename, char* ext);
int K_get_ext(char* filename, char* ext, int max_ext_len);
int K_has_ext(char* filename);
char *K_set_ext(char *,char *,int );
char *K_set_ext_asc(char *,char *,int );
void K_strip(char *);
int K_merge(KDB *,KDB *,int );
int K_merge_del(KDB *,KDB *,int );
KDB *K_load(int ,char *,int ,char **);
KDB *K_interpret(int ,char *);
int K_copy(KDB *,int ,char **,int ,char **,SAMPLE *);
int K_cat(KDB *,char *);
int K_set_backup_on_save(int take_backup);
int K_get_backup_on_save();
int K_backup(char *);
int K_save(KDB *,char *);
int K_save_ws(KDB *);
int K_setname(char *,char *);
int K_filetype(char *,char *,int *,SAMPLE *);
