#ifdef __cplusplus
extern "C" {
#endif

/* s_lzh*.c */
extern void LzhInitTree(void);
extern void LzhInsertNode(int r);
extern void LzhDeleteNode(int p);
extern short LzhGetBit(void);
extern short LzhGetByte(void);
extern int LzhPutCode(short l,unsigned short c);
extern void LzhStartHuff(void);
extern void LzhReconst(void);
extern void LzhUpdate(int c);
extern int LzhEncodeChar(unsigned short c);
extern int LzhEncodePosition(unsigned short c);
extern int LzhEncodeEnd(void);
extern short LzhDecodeChar(void);
extern short LzhDecodePosition(void);
extern int LzhEncode(void);
extern int LzhDecode(void);
extern int LzhPutChar(int ch);
extern int LzhGetChar(void);
extern long LzhGetSize(void);
extern void LzhFreeGnl(void);
extern int LzhAllocGnl(void);
extern int LzhEncodeGnl(int (*getfn)(),int (*putfn)(),long (*sizfn)());
extern int LzhDecodeGnl(int (*getfn)(),int (*putfn)());
extern int LzhGetFile(void);
extern int LzhPutFile(int c);
extern long LzhSizFile(void);
extern int LzhOpenFiles(char *filein,char *fileoutt,int op,int pos);
extern int LzhEncodeFile(char *filein,char *fileout);
extern int LzhDecodeFile(char *filein,char *fileout);
extern int LzhDecodeAppFile(char *filein,char *fileout,int pos);
extern int LzhGetStr(void);
extern int LzhPutStr(int c);
extern long LzhSizStr(void);
extern int LzhEncodeStr(unsigned char *instr,unsigned int inlen,unsigned char **outstr,unsigned int *outlen);
extern int LzhDecodeStr(unsigned char *instr,unsigned int inlen,unsigned char **outstr,unsigned int *outlen);

#ifdef __cplusplus
}
#endif
