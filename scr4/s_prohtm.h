#ifdef __cplusplus
extern "C" {
#endif

/* s_htm*.c */
extern int HtmlInit(FILE *html,char *title);
extern int HtmlEnd(FILE *html,char *footer);
extern int HtmlFormInit(FILE *html,char *action);
extern int HtmlFormEnd(FILE *html,char *submit,char *reset);
extern int HtmlFormButton(FILE *html,unsigned char *name,unsigned char *text);
extern int HtmlFormHidden(FILE *html,unsigned char *name,unsigned char *text);
extern int HtmlFormPassword(FILE *html,unsigned char *name);
extern int HtmlFormText(FILE *html,char *name,int lg,char *value);
extern int HtmlFormRadio(FILE *html,char *name,char **options,int choice);
extern int HtmlFormMenu(FILE *html,char *name,char **options,int choice);
extern int HtmlFormTextArea(FILE *html,char *name,char *value,int nl,int nc);
extern int HtmlMessage(char *msg);
extern int HtmlReplace(unsigned char *from,unsigned char *to,unsigned char *replfile,unsigned char *files,int Recursive,int Backup,int Silent);
extern int JNavFile(unsigned char *from,unsigned char *to,unsigned char *replfile,unsigned char *buf,unsigned int mode,unsigned int Backup,unsigned int Silent);
extern int JNavFileNoMsg(unsigned char *from,unsigned char *to,unsigned char *replfile,unsigned char *file,unsigned int mode,unsigned int Backup);
extern int JNavAccess(char *filename);
extern int JNavDump(FILE *fdou,char *replfile);
extern int HtmlSplit(char *htmlfile,char *resfile,int level,unsigned char *title,unsigned char *body,unsigned char *back,int prevnext,int verbose, unsigned char *style);
extern int JSplitChapter(HFILE *hfd,int level,int clevel);
extern int JResetTit(int level);
extern int JAddTit(int level,unsigned char *txt);
extern int JOpenNext(void);
extern int HtmlToc(char *htmlfile,char *resfile,int maxlevel, char *target);
extern int JTocSub(char *htmlfile,int maxlevel,int clevel);
extern YYFILE *JHYYopen(HFILE *hfd);
extern int JHYYFind(HFILE *hfd,unsigned char *tag);
extern HFILE *JHOpen(char *filename);
extern int JHClose(HFILE *hfd);
extern int JHLex(HFILE *hfd);
extern int JHRead(HFILE *hfd);
extern int JHUnread(HFILE *hfd);
extern int JHReset(HFILE *hfd);
extern int JHAdd(HFILE *hfd,int ch);
extern int JHCompare(HFILE *hfd,char *tag);
extern int JHRefFind(char *filename);
extern int JHRefFree(void);
extern int JHRefAdd(char *filename,int type,int exist);
extern int JHRefIsProto(char *filename);
extern unsigned char *JHRefFullPath(char *res,char *current,char *name);

#ifdef __cplusplus
}
#endif
