#ifdef __cplusplus 
extern "C" { 
#endif 

/* s_xdr*.c */ 
extern void XDR_rev(unsigned char *buf,int n,int width);
extern int XDR_write(FILE *fd,unsigned char *in,int n,int width);
extern int XDR_read(FILE *fd,unsigned char *out,int n,int width);

#ifdef __cplusplus 
} 
#endif 
