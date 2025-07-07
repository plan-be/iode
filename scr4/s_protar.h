#ifdef __cplusplus 
extern "C" { 
#endif 


/* s_tarc.c */ 
extern int TarCopyOpen(int type,char *tarfile,int (*nextfn)());
extern int TarCopy(int type,char *tarfile,char **files,int (*nextfn)());
extern int TarUpdateOpen(int type,char *tarfile,int (*nextfn)());
extern int TarUpdate(int type,char *tarfile,char **files,int (*nextfn)());
extern int TarAddFile(char *filename);
extern int TarAddRegFile(char *filename);

/* s_tarx.c */ 
extern int TarXtractOpen(int type,char *tarfile,int (*nextfn)());
extern int TarXtract(int type,char *tarfile,char **files,int (*nextfn)());
extern int TarXtract1(TARHEAD *thead);

/* s_tart.c */ 
extern int TarTable(int type,char *tarfile,int (*nextfn)(),int (*tablefn)());
extern int TarSkipFile(TARHEAD *thead);

/* s_tario.c */ 
extern int TarOpen(int mode,int type,char *filename,int (*nextfn)());
extern int TarClose(void);
extern int TarRWBlock(char *buf,int mode);
extern int TarReadTrack(int head,int track);
extern int TarReadSect(int head,int track,int sect,char *buf);
extern int TarWriteSect(int head,int track,int sect,char *buf);
extern int TarWriteTrack(void);
extern int TarTapeFlush(void);
extern int TarRewindBlock(void);
extern int TarReadBlock(char *buf);
extern int TarWriteBlock(char *buf);
extern int TarSkipBlocks(long n);
extern FILE *Tarfopen(char *filename,char *mode);

/* s_tarfd.c */ 
extern int TarDriveType(int drive);
extern int TarDiskPresent(int drive);
extern int TarDiskType(int drive);
extern int TarBiosReadSects(int drive,int head,int track,int sect,int nsect,unsigned char *buf);
extern int TarBiosWriteSects(int drive,int head,int track,int sect,int nsect,unsigned char *buf);
extern int TarBiosdisk(int service,int drive,int head,int track,int sect,int nsect,void *buf);
extern int TarDriveReset(int drive);

/* s_tarh.c */ 
extern int TarNextHeader(TARHEAD *thead);
extern int TarCreateHeader(char *header,TARHEAD *thead);
extern int TarXtrHeader(char *header,TARHEAD *thead);
extern int TarCalcHead(char *filename,TARHEAD *thead);

/* s_tardft.c */ 
extern int TarDftNextFn(int nb);
extern int TarDftTableFn(TARHEAD *thead);

/* s_tarerr.c */ 
extern char *TarError(void);

#ifdef __cplusplus 
} 
#endif 
