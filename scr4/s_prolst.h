#ifdef __cplusplus 
extern "C" { 
#endif 

/* s_lst*.c */ 
extern LST *LST_create(int tbl_size,int el_size);
extern int LST_free(LST *lst);
extern int LST_add_1(LST *lst);
extern int LST_add(LST *lst,long after,long n);
extern int LST_del(LST *lst,long from,long n);
extern int LST_move(LST *lst,long from,long after,long n);
extern void *LST_get(LST *lst,long n);
extern int LST_set(LST *lst,long n,void *ptr);
extern int LST_strip(LST *lst);
extern int LST_nb(LST *lst);
extern int LST_set_buf(int ch,int pos);
extern LST *LST_read_in(int (*fn_next)(),FILE *fd,unsigned char *seps);
extern int LST_nextch(FILE *fdvec);
extern LST *LST_vtol(unsigned char *vec,unsigned char *seps);
extern unsigned char *LST_ltov(LST *lst,unsigned char *seps);
extern int LST_free_text(LST *lst);
extern LST *LST_read_file(char *filename);
extern int LST_save_file(char *filename,LST *lst);
extern int LST_fwrite(FILE *fd,unsigned char *ptr);

#ifdef __cplusplus 
} 
#endif 
