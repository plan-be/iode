#pragma once 

#include "api/constants.h"
#include "api/time/period.h"
#include "api/time/sample.h"
#include "api/objs/kdb.h"

// K_LZH : indicates if a saved WS must be compressed or not 
// -> https://en.wikipedia.org/wiki/LHA_(file_format).
inline int  K_LZH = 0; 

inline char K_LABELS[] = "KOBJS 4.02\032";  // Version 1
inline char K_LABELD[] = "KOBJS 504d\032";  // Version 2
inline char K_LABELX[] = "KOBJS 564A\032";  // Version 3 
inline char K_LABEL[]  = "KOBJS 564A\032";  // Version 0 = Current version = Version 3

/* k_objs.c */
int K_upd_eqs(char* name, char* lec, char* cmt, int method, Sample* smpl, 
              char* instr, char* blk, float* tests, int date);
int K_upd_tbl(char* name, char* arg);

/* k_objvers.c */
int K_calcvers(char *);

/* k_objfile.c */
char *K_add_ext(char* filename, char* ext);
int K_get_ext(char* filename, char* ext, int max_ext_len);
int K_has_ext(char* filename);
char *K_set_ext(char *,char *,int );
char *K_set_ext_asc(char *,char *,int );
void K_strip(char *);
int K_set_backup_on_save(int take_backup);
int K_get_backup_on_save();
int K_backup(char *);
int K_setname(char *,char *);
int K_findtype(char* filename, int type);
int K_filetype(char *,char *,int *,Sample *);
int X_findtype(char* arg);
