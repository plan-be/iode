#ifdef __cplusplus 
extern "C" { 
#endif 


/* s_fiam.c */ 
extern int FI_ascmagic(char *cmt,unsigned char *buf,int nbytes);

/* s_fiap.c */ 
extern int FI_apprentice(char **tbl);
extern unsigned long FI_signextend(struct FI_magic *m,unsigned long v);
extern int FI_parse(int nb,char *l,int *ndx);
extern int FI_getvalue(struct FI_magic *m,char **p);
extern char *FI_getstr(char *s,char *p,int plen,int *slen);
extern int FI_hextoint(int c);
extern int FI_parse_ext(int nb,char *l,int *ndx);

/* s_fiem.c */ 
extern int FI_extmagic(char *inname,char *cmt);

/* s_fifm.c */ 
extern int FI_fsmagic(char *fn,char *cmt,struct stat *sb);

/* s_file.c */ 
extern int FI_file(char **tbl,char *inname,char *cmt,int lg);
extern int FI_tryit(char *inname,char *cmt,unsigned char *buf,int nb);
extern int FI_strcat(char *cmt,char *string);

/* s_fism.c */ 
extern int FI_softmagic(char *cmt,unsigned char *buf,int nbytes);
extern int FI_match(char *cmt,unsigned char *s,int nbytes);
extern int FI_mprint(char *cmt,union VALUETYPE *p,struct FI_magic *m);
extern int FI_mconvert(union VALUETYPE *p,struct FI_magic *m);
extern int FI_mget(union VALUETYPE *p,unsigned char *s,struct FI_magic *m,int nbytes);
extern int FI_mcheck(union VALUETYPE *p,struct FI_magic *m);

#ifdef __cplusplus 
} 
#endif 
