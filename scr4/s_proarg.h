#ifdef __cplusplus 
extern "C" { 
#endif 


/* s_ainit.c */ 
extern char **A_init(char *string);
extern char **A_initv(char **argv);
extern int A_initv1(char **argv);
extern int A_add(char *arg);
extern int A_to_be_exp(char *str);
extern int A_add1(char *arg);
extern int A_read(int type,char **buf,char *word,int max_lg);
extern int A_is_sep(int ch);
extern int A_free(unsigned char **args);
extern int A_find(char **args,char *string);
extern int A_nb(char **args,int pos);
extern int A_help(char **args,int (*fn)());

/* s_aerr.c */ 
extern int A_error(char *txt);

/* s_aexec.c */ 
extern int A_exec(char **args,ADEF *adef);
extern int A_exec1(char **args,ADEF *adef);

/* s_aexp.c */ 
extern char *A_expand(char *name);

/* s_aiexp.c */ 
extern int A_iexpand(char **args);

/* s_aexps.c */ 
extern int A_expandstar(char *arg);

/* s_acheck.c */ 
extern int A_check(char **args,ADEF *adef);
extern int A_check1(char **args,ADEF *adef);
extern int A_checkdef(ADEF *adef,char *arg);

/* s_ausage.c */ 
extern int A_usage(char *title,ADEF *adef);

/* s_achkin.c */ 
extern int A_check_int(int argc,char *argv[]);
extern int A_check_int_1(char *parm);

/* s_achkac.c */ 
extern int A_check_access(int argc,char *argv[]);

#ifdef __cplusplus 
} 
#endif 
