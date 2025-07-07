#ifdef __cplusplus 
extern "C" { 
#endif 


/* s_vf.c */ 
extern VFILE *VF_open(char *filename);
extern int VF_close(VFILE *vfd);
extern long VF_write(VFILE *vfd,char *ptr,int size);
extern int VF_rewrite(VFILE *vfd,long hdl,char *ptr,int size);
extern char *VF_read(VFILE *vfd,long hdl);
extern int VF_delete(VFILE *vfd,long hdl);
extern int VF_get_infos(VFILE *vfd,int parm);
extern int VF_set_infos(VFILE *vfd);

/* s_vfhdl.c */ 
extern FILE *VF_hdl_open(char *filename);
extern int VF_hdl_seek(VFILE *vfd,long hdl);
extern int VF_hdl_set(VFILE *vfd,long nb,long pos);
extern long VF_hdl_get(VFILE *vfd,long nb);
extern long VF_hdl_find_free(VFILE *vfd);
extern int VF_hdl_del(VFILE *vfd,long hdl);
extern int VF_hdl_get_infos(VFILE *vfd);
extern int VF_hdl_set_infos(VFILE *vfd);

/* s_vfblk.c */ 
extern FILE *VF_blk_open(char *filename);
extern int VF_blk_seek(VFILE *vfd,long blk_nb);
extern int VF_blk_set(VFILE *vfd,long blk_nb,long cfl[]);
extern int VF_blk_get(VFILE *vfd,long blk_nb,long cfl[]);
extern int VF_blk_get_hdl(VFILE *vfd,long hdl,long *lg,long *hdl1);
extern int VF_blk_set_hdl(VFILE *vfd,long hdl,long lg,long hdl1);
extern long VF_blk_find_free(VFILE *vfd,long size);
extern int VF_blk_add(VFILE *vfd);
extern int VF_blk_compress(VFILE *vfd,long blk_nb);
extern int VF_blk_write_ptr(VFILE *vfd,long blk_nb,long hdl,char *ptr,long size);
extern int VF_blk_del_hdl(VFILE *vfd,long pos);
extern int VF_blk_get_infos(VFILE *vfd);
extern int VF_blk_set_infos(VFILE *vfd);

/* s_vferr.c */ 
extern int VF_error(char *txt);

/* s_vfanal.c */ 
extern int VF_db_analyse(char *filename);
extern int VF_blk_anal_1(VFILE *vfd,long i);

/* s_vfdel.c */ 
extern int VF_db_delete(char *filename);

/* s_vfcrea.c */ 
extern int VF_db_create(char *filename,long blksize);
extern int VF_blk_create_db(char *filename,long blksize);
extern int VF_hdl_create_db(char *filename);

/* s_vfmisc.c */ 
extern int VF_write_long(FILE *fd,long l);
extern long VF_read_long(FILE *fd);
extern int VF_fcpy(FILE *fd,long pos_from,long pos_to,long lg);

/* s_vflock.c */ 
extern int VF_lock(VFILE *vfd,int level);

/* s_vfreb.c */ 
extern int VF_db_rebuild(char *filename);

#ifdef __cplusplus 
} 
#endif 
