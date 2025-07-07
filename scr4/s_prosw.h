#ifdef SCRPROTO

#ifdef __cplusplus
extern "C" {
#endif

/* s_sw*.c */
extern int SW_abort(int code,char *text);
extern SWHDL SW_alloc(SWSIZE size);
extern int SW_find_free_blk(SWSIZE size);
extern int SW_restore(int blk_nb);
extern int SW_create_blk(SWSIZE size);
extern SWHDL SW_find_free_item(void);
extern int SW_free(SWHDL item);
extern SWHDL SW_realloc(SWSIZE item, SWSIZE size);
extern char *SW_getptr(SWHDL);

extern char *SW_nalloc_orig(unsigned int size);
extern char *SW_nrealloc_orig(void *old_ptr,unsigned int old_count,unsigned int new_count);
extern char *SW_nalloc_doc(unsigned int size, char *, int);
//extern char *SW_nrealloc_doc(void *old_ptr,unsigned int old_count,unsigned int new_count, char *file, int line);
extern void *SW_nrealloc_doc(void *old_ptr,unsigned int old_count,unsigned int new_count, char *file, int line);
extern void SW_Debug(int size);

//extern int SW_nfree(char *ptr);
extern int SW_nfree(void *ptr);
extern int SW_open(void);
extern int SW_close(void);
extern int SW_append_file(char *ptr);
extern int SW_swap(char *ptr,int ref);
extern int SW_init(int ems);
extern void SW_end(void);
extern int SW_panic(int code);
extern int SW_init_blk(int blk_nb, SWSIZE size);
extern int SW_compress(int blk_nb);
extern long SW_coreleft(void);
extern int SW_status(SWSTAT *st);
extern int SW_set_item(SWHDL item, int blk, SWSIZE pos);
extern SWSIZE SW_get_pos(SWHDL item);
extern int SW_get_blk(SWHDL item);
extern int SW_add_item(void);
extern int SW_nb_items(void);
extern SWHDL SW_get_free(SWHDL item);
extern int SW_add_free(SWHDL item);
extern int SW_nb_frees(void);
extern char *SW_alloc_seg(SWSIZE size);
extern int SW_create_seg(SWSIZE size);
extern int SW_del_seg(void);
extern int SW_oldest_seg(void);
extern void SW_swap_ems(int seg_nb);

#ifdef __cplusplus
}
#endif

#endif // SCRPROTO
