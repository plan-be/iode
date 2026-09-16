#ifdef __cplusplus 
extern "C" { 
#endif 

/* s_cpp*.c */ 
extern CPPFILE *CppOpen(char *filename,int type);
extern int CppClose(CPPFILE *cpp);
extern int CppCloseNoDef(CPPFILE *cpp);
extern int CppRecord(CPPFILE *cpp,unsigned char *buf);
extern int CppRecordLILO(CPPFILE *cpp,unsigned char *buf);
extern int CppRecordRealloc(CPPFILE *cpp,int lg);
extern int CppUngetc(CPPFILE *cpp,int ch);
extern CPPFILE *CppCurrent(CPPFILE *cpp);
extern int CppCloseLast(CPPFILE *cpp);
extern int CppGetc(CPPFILE *cpp);
extern int CppIfdef(CPPFILE *cpp);
extern int CppElse(CPPFILE *cpp);
extern int CppEndif(CPPFILE *cpp);
extern int CppGetNext(CPPFILE *cpp);
extern int CppAddDefine(unsigned char *str);
extern int CppFindDefine(unsigned char *str);
extern int CppFreeDefines(void);
extern int CppDefine(unsigned char *name,unsigned char *txt);
extern int CppReadDefine(CPPFILE *cpp);
extern int CppReadUndef(CPPFILE *cpp);
extern int CppApplyDefine(CPPFILE *cpp,unsigned char *str);
extern int CppInclude(CPPFILE *cpp);
extern char *CppError(CPPFILE *cpp);
extern int CppPrintError(CPPFILE *cpp,char *msg);
extern int CppSkipSpaces(CPPFILE *cpp);
extern int CppSkipToChars(CPPFILE *cpp,unsigned char *chs);
extern unsigned char *CppReadToChars(CPPFILE *cpp,unsigned char *chs);
extern unsigned char *CppReadLine(CPPFILE *cpp);
extern int CppSkipToEol(CPPFILE *cpp);
extern int CppReadOnlyChars(CPPFILE *cpp,unsigned char *chs,unsigned char *str,int maxlg);
extern int CppReadWord(CPPFILE *cpp,unsigned char *str,int maxlg);
extern long CppReadLong(CPPFILE *cpp);
extern double CppReadDbl(CPPFILE *cpp);
extern int CppReadString(CPPFILE *cpp,unsigned char *str,int maxlg);
extern unsigned char *CppReadQuotedString(CPPFILE *cpp);

#ifdef __cplusplus 
} 
#endif 
