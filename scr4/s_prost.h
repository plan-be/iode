#pragma once
#include <stdio.h>
#include "s_strs.h"

#ifdef __cplusplus
extern "C" {
#endif

/* s_st*.c */
extern unsigned char *SCR_stracpy(unsigned char *in);
extern unsigned char *SCR_strfacpy(unsigned char **old,unsigned char *cnew);
extern unsigned char *SCR_strafcat(unsigned char *old,unsigned char *cnew);
extern int SCR_add_ptr_lg_0(unsigned char ***tbl,int *ntbl,unsigned char *txt,int lg,int z);
extern int SCR_add_ptr_lg(unsigned char ***tbl,int *ntbl,unsigned char *txt,int lg);
extern int SCR_add_ptr(unsigned char ***tbl,int *ntbl,unsigned char *txt);
extern int SCR_free_tbl(unsigned char **tbl);
extern int SCR_tbl_size(unsigned char **tbl);
extern unsigned char **SCR_copy_tbl(unsigned char **opts);
extern int SCR_OemToAnsiChar(int ch);
extern unsigned char *SCR_OemToAnsi(unsigned char *ansi,unsigned char *oem);
extern unsigned char *SCR_OemToAnsiLg(unsigned char *ansi,unsigned char *oem, int lg);
extern int SCR_AnsiToOemChar(int ch);
extern unsigned char *SCR_AnsiToOem(unsigned char *oem,unsigned char *ansi);
extern unsigned char *SCR_AnsiToOemLg(unsigned char *oem,unsigned char *ansi, int lg);
extern unsigned char *SCR_change_char(unsigned char *str,int ch1,int ch2);
extern char *SCR_change_ext(char *res,char *filename,char *ext);
extern int SCR_check_bank(char *ccp);
extern unsigned char *SCR_compress(unsigned char *res,unsigned char *str,int lg);
extern int SCR_crypt(unsigned char *buf,int len,long key);
extern int SCR_decrypt(unsigned char *buf,int len,long key);
extern char *SCR_cat_ext(char *res,char *filename,char *ext);
extern long SCR_cvt_time(long t);
extern long SCR_cvt_date(long t);
extern char *STD_unit(int disk,int type);
extern int STD_doscpout(char *from,char *to,int unit,int type);
extern int STD_doscpin(char *from,char *to,int unit,int type);
extern int STD_dosformat(int unit,int type);
extern int STD_subshell(void);
extern unsigned char **SCR_dif(unsigned char **tbl1,unsigned char **tbl2);
extern unsigned char *SCR_dif_list(unsigned char *str1,unsigned char *str2,unsigned char *seps);
extern void SCR_close_err_txt(void);
extern char *SCR_get_err_txt_t_lang(FILE *fd_msg,unsigned char *keyw,int curlang);
extern char *SCR_get_err_txt_t(FILE *fd_msg,unsigned char *keyw);
extern char *SCR_err_txt_t_lang(unsigned char *keyw,int lang);
extern char *SCR_err_txt_t(unsigned char *keyw);
extern char *SCR_get_err_txt_lang(FILE *fd_msg,int val,int lang);
extern char *SCR_err_txt_lang(int val,int lang);
extern char *SCR_get_err_txt(FILE *fd_msg,int val);
extern char* (*SCR_err_txt_super)(int val);
extern char *SCR_err_txt(int val);
extern unsigned char *SCR_fmt_dbl(double d,unsigned char *txt,int size,int nb_dec);
extern unsigned char *SCR_fmt_long(unsigned char *out,unsigned char *fmt,unsigned long l);
extern unsigned char *SCR_fmt_text(unsigned char *out,unsigned char *fmt,unsigned char *in);
extern int SCR_sprintf(char* buf, int buf_size, char* fmt, ...);
extern unsigned char *SCR_unix_filename(unsigned char *filename);
extern unsigned char *SCR_std_filename(unsigned char *filename);
extern int SCR_rename(char *fold,char *fnew);
extern long SCR_FindStringPos(unsigned char *filename,unsigned char *str,int lg);
extern long SCR_get_app_pos(FILE *fd,int n);
extern long SCR_get_app_size(FILE *fd,int n);
extern int SCR_grep(char *pattern,char *string,int ecase);
extern int SCR_grep_1(char *pattern,char *string,int ecase,int nbeg,int diese);
extern int SCR_grep_2(char *pattern,char *string,int ecase);
extern int SCR_grep_gnl(char* pattern, char* string, int ecase, int all);
extern unsigned char **SCR_inter(unsigned char *tbl1[],unsigned char *tbl2[]);
extern unsigned char *SCR_inter_list(unsigned char *str1,unsigned char *str2,unsigned char *seps);
extern int SCR_is_alpha(int ch);
extern int SCR_is_num(int ch);
extern int SCR_is_anum(int ch);
extern unsigned char *SCR_strlcpy(unsigned char *out,unsigned char *in,int n);
extern int SCR_fmt_lg(char *fmt);
extern unsigned char *U_ljust_text(unsigned char *text);
extern int SCR_is_lower(int ch);
extern int SCR_lower_char(int ch);
extern unsigned char *SCR_lower(unsigned char *str);
extern unsigned char *SCR_mtov(unsigned char **tbl,int sep);
extern unsigned char *SCR_mtovs(unsigned char **tbl,unsigned char *seps);
extern int SCR_LoadString(FILE *fd,unsigned char **txt);
extern int SCR_LoadShort(FILE *fd,short *a);
extern int SCR_DumpString(FILE *fd,unsigned char *txt);
extern int SCR_DumpShort(FILE *fd,int a);
extern unsigned char *SCR_pad(unsigned char *str,int lg);
extern char *AR_error(void);
extern FILE *AR_open(char *arname,int mode);
extern int AR_readnext(FILE *fd,ARLIST *arl);
extern int AR_add(char *arname,char **files,int n);
extern int AR_add_1(FILE *fdar,char *filename);
extern int AR_series(FILE *fd,int ch);
extern int AR_dump(FILE *fd,FILE *fd2,long *rsize,long *psize);
extern ARLIST **AR_list(char *arname);
extern int AR_extract(char *arname,char **files,int n,int (*fn)());
extern int AR_uncat(FILE *fdar,char *filename,long lg,int (*fn)());
extern int PC_open(char *filename);
extern int PC_getline(char *prname,char *buf);
extern int PC_exec(char *filename,char *prname,char *arg1,char *arg2,char *arg3);
extern char **PC_infos(char *filename,char *prname);
extern char **PC_list(char *filename,int fld_nb,int fld_select,char *fld_vals,int maxparms);
extern int PS_labels(char *ifl,char *ofl,int nvl,int nhl,char *font,double size,double margv,double margh,double spacing);
extern int PS_next_label(void);
extern int PS_eat_label(void);
extern int PS_lab_nl(void);
extern int PS_new_label(void);
extern int PS_init(char *ifl,char *ofl,char *font,double size,double margv,double margh,double spacing,int landsc);
extern int PS_close(void);
extern int PS_text(char *ifl,char *ofl,char *font,double size,double margv,double margh,double spacing,int landsc);
extern int PS_new_string(void);
extern int PS_show(void);
extern float PS_y(void);
extern float PS_x(void);
extern int PS_moveto(void);
extern int PS_new_page(void);
extern int PS_new_line(void);
extern int PS_mif_char(int c);
extern int PS_profile(char *font,double size);
extern int PS_vert_graph(int ch);
extern int PS_graph(int ch);
extern int PS_hsegm(int pos,int size);
extern int PS_hdraw(void);
extern int PS_vsegm(int col,int pos,int size);
extern int PS_vdraw(int col);
extern int SCR_pstrip_lg(unsigned char *buf,unsigned char *punct);
extern unsigned char *SCR_pstrip(unsigned char *buf,unsigned char *punct);
extern int SCR_cstrcmp(unsigned char *str1,unsigned char *str2);
extern int SCR_cstrncmp(unsigned char *str1,unsigned char *str2,int lg);
extern unsigned char *SCR_strrcpy(unsigned char *out,unsigned char *in);
extern unsigned char *SCR_LoadRawFile(char *filename,long *count);
extern int SCR_read_line(FILE *fd,unsigned char *buf,int lg);
extern unsigned char *SCR_replace_gnl(unsigned char *txt,unsigned char *repl,unsigned char *by,unsigned char *chars);
extern unsigned char *SCR_replace_word(unsigned char *txt,unsigned char *repl,unsigned char *by);
extern unsigned char *SCR_replace(unsigned char *txt,unsigned char *repl,unsigned char *by);
extern unsigned char *SCR_areplace(unsigned char *txt,unsigned char *repl,unsigned char *by);
extern M_RING *M_RING_init(int size);
extern int M_RING_free(M_RING *t);
extern unsigned char M_RING_read(M_RING *t,int *detect);
extern int M_RING_write(M_RING *t,int c,int *detect);
extern int M_RING_reset(M_RING *t);
extern unsigned char *U_rjust_text(unsigned char *text);
extern int SCR_check_nb(char *rn);
extern unsigned char *SCR_asqz(unsigned char *text,unsigned char *chars);
extern unsigned char *SCR_sqz(unsigned char *text);
extern char *SCR_search_path(char *path,char *file);
extern char *SCR_search_file(char *file);
extern short SCR_str_add(char *txt,int len);
extern short SCR_str_find_str(char *txt,int len);
extern char *SCR_str_find(int nb);
extern unsigned char *SCR_stripdc(unsigned char *txt,int ch,unsigned char *str);
extern unsigned char *SCR_strip(unsigned char *str);
extern unsigned char **SCR_text(unsigned char *str,unsigned char *seps,int lg);
extern int SCR_text_title(unsigned char *title,unsigned char *str,unsigned char *seps,int lg,int (*fn)());
extern unsigned char *U_center_text(unsigned char *text);
extern unsigned long SCR_ufmt_long(char *fmt,char *in);
extern unsigned char *SCR_ufmt_text(unsigned char *out,unsigned char *fmt,unsigned char *in);
extern int U_index(unsigned char *str,unsigned char *sstr);
extern int U_is_in(int ch,char *string);
extern int SCR_add_tbl(unsigned char ***tbl,int *nb,unsigned char *str);
extern unsigned char **SCR_union(unsigned char **tbl1,unsigned char **tbl2);
extern unsigned char **SCR_union_quick(unsigned char **tbl1,unsigned char **tbl2);
extern unsigned char *SCR_union_list(unsigned char *str1,unsigned char *str2,unsigned char *seps);
extern int U_pos(int ch,unsigned char *str);
extern int SCR_is_upper(int ch);
extern int SCR_upper_char(int ch);
extern unsigned char *SCR_upper(unsigned char *str);
extern int SCR_is_space(int ch);
extern unsigned char *U_shift_text(unsigned char *text,int n);
extern unsigned char *U_sqz_text(unsigned char *text);
extern unsigned char **SCR_vtoms(unsigned char *str,unsigned char *seps);
extern unsigned char **SCR_vtoms2(unsigned char *str,unsigned char *seps, int supdbl);
extern unsigned char **SCR_vtoms2A(unsigned char *str,unsigned char *seps, int supdbl); /* JMP 27-10-2012 */
extern unsigned char **SCR_vtoms3(unsigned char *str,unsigned char *seps, int supdbl); // JMP 17/3/2017 
extern unsigned char **SCR_vtom(unsigned char *str,int sep);
extern unsigned char *SCR_zstrip(unsigned char *ptr,int len);
extern int SCR_dumphex(char* filein, char* fileout, int nl);
extern int SCR_dumphexfd(char* filein, FILE* fdout, int nl);
extern int SCR_fprintf_esc(FILE *fd, char *str, int addquotes);



/* s_allc.c  */
extern int AllocDocLoop();
extern char *SCR_malloc_chk(unsigned int lg, int panic);
extern char *SCR_realloc_chk(void *old_ptr,unsigned int el_size,unsigned int old_count,unsigned int new_count, int panic);
extern char *SCR_malloc_doc(unsigned int lg, char *file, int ligne);
extern char *SCR_realloc_doc(void *old_ptr,unsigned int el_size,unsigned int old_count,unsigned int new_count, char *file, int line);
 
extern int SCR_alloc_chunck(int nb);
extern int SCR_free(void *ptr);
extern char *SCR_palloca(unsigned int len,unsigned int a);
extern char *SCR_palloc(unsigned int len);

/* s_pcipar.c */
extern int PciParalInt86(int fn,int nb);
extern int PciParalInit(int nb);
extern int PciParalTest(int nb);
extern int PciTestLPT(char *name);

/* s_pcinfo.c */
extern int PciGetDriveInfo(int disk,unsigned long *total,unsigned long *free);
extern int PciGetInfos(PCINFOS *pci);

/* s_panic.c */
extern int SCR_panic(void);

/* s_copy.c */
extern int SCR_copy_file(char *in,char *out);
extern int SCR_append_file(char* in, char* out);

/* s_infmis.c */
extern int InfRStrip0(char *a);
extern int InfStrip0(char *a);
extern int InfRev(char *a);

/* s_infadd.c */
extern int InfAdd(char *a,char *b,char *res);
extern int InfRAdd(char *a,char *b,char *res);

/* s_infsub.c */
extern int InfDiff(char *a,char *b,char *res);
extern int InfRDiff(char *a,char *b,char *res);

/* s_infmul.c */
extern int InfRMult1(char *a,int ib,char *res);
extern int InfRMult(char *a,char *b,char *res);
extern int InfMult1(char *a,int b,char *res);
extern int InfMult(char *a,char *b,char *res);

/* s_infdiv.c */
extern int InfCmp(char *a,char *b);
extern int InfDiv(char *a,char *b,char *res,char *reste);

/* s_memavl.c */
extern long SCR_free_mem(void);

/* s_dirfp.c */
//extern char *SCR_fullpath(char *name,char *res); // JMP 10/5/2021 double emploi avec s_prodir.h

/* s_dircwd.c */
// extern char *SCR_getcwd(int drive,char *dir); // JMP 10/5/2021 double emploi avec s_prodir.h

/* s_stutf8.c */
extern unsigned char *SCR_OemToUTF8Char(int ch);
extern unsigned char *SCR_AnsiToUTF8Char(int ch);
int SCR_OemToUTF8Length(unsigned char *str);
int SCR_AnsiToUTF8Length(unsigned char *str);
int SCR_OemAnsiToUTF8Length(unsigned char *str, int oemansi);
extern unsigned char *SCR_OemToUTF8(unsigned char *, unsigned char *);
extern unsigned char *SCR_AnsiToUTF8(unsigned char *, unsigned char *);
extern int SCR_ConvertToUTF8(char *filein, char *fileout, int isbom, int isansi);
extern int SCR_ConvertOemToUTF8(char *filein, char *fileout, int isbom);
extern int SCR_ConvertAnsiToUTF8(char *filein, char *fileout, int isbom);

/* s_gzip.c */
extern int GzipEncodeStr(unsigned char* instr, unsigned long inlen, unsigned char** outstr, unsigned long* outlen);
extern int GzipDecodeStr(unsigned char* instr, unsigned long inlen, unsigned char** outstr, unsigned long* outlen);

#ifdef __cplusplus
}
#endif
