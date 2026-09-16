#ifdef __cplusplus
extern "C" {
#endif

/* s_y*.c */
extern YYFILE *YY_open(char *filename,YYKEYS *table,int size,int type);
extern int YY_init(YYFILE *yy,char *filename,YYKEYS *table,int size,int type);
extern int YY_end(YYFILE *yy);
extern int YY_strcmp(const char *, const char *);
extern int YY_close(YYFILE *yy);
extern int YY_record(YYFILE *yy,unsigned char *txt);
extern int YY_ungetc(int ch,YYFILE *yy);
extern int YY_getc(YYFILE *yy);
extern int YY_upper(char *str);
extern int YY_debug(YYFILE *yy);
extern int YY_undef(YYFILE *yy);
extern int YY_unsave_define(YYFILE *yy,char *name);
extern int YY_define(YYFILE *yy);
extern int YY_ifdef(YYFILE *yy,int flag);
extern int YY_find_end_else(YYFILE *yy);
extern int YY_save_define(YYFILE *yy,char *name,char *def);
extern YYDEF *YY_search_def(YYDEF **tbl,int nb,char *txt);
extern int YY_find_def(YYFILE *yy);
extern char *YY_error(YYFILE *yy);
extern int YY_include(YYFILE *yy);
extern int YY_find_oper(YYFILE *yy,YYKEYS *yykeys,int nbkeys);
extern int YY_find_string(YYFILE *yy,YYKEYS *yykeys,int nbkeys);
extern int YY_lex2(YYFILE *yy,YYKEYS *yykeys,int nbkeys);
extern int YY_lex(YYFILE *yy);
extern int YY_add_to_text(YYFILE *yy,int pos,int ch);
extern int YY_read(YYFILE *yy);
extern int YY_unread(YYFILE *yy);
extern int YY_skip_spaces(YYFILE *yy);
extern int YY_skip_to_eol(YYFILE *yy);
extern int YY_skip_to_char(YYFILE *yy,int c);
extern int YY_read_to_char(YYFILE *yy,int c);
extern int YY_isin(int ch,unsigned char *chars);
extern int YY_read_to_chars(YYFILE *yy,unsigned char *chars);
extern int YY_read_to_string(YYFILE *yy,unsigned char *str);
extern int YY_read_control(YYFILE *yy);
extern int YY_read_escape(YYFILE *yy);
extern int YY_read_word(YYFILE *yy);
extern int YY_read_letter(YYFILE *yy);
extern int YY_read_num(YYFILE *yy,int dbl);
extern int YY_read_string(YYFILE *yy, int quote);
extern int U_hex_to_dec(char *buf);
extern int U_octal_to_dec(char *buf);
extern int YY_comment(YYFILE *yy);

#ifdef __cplusplus
}
#endif
