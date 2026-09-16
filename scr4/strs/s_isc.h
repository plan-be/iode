#ifndef _S_ISC_H_
#define _S_ISC_H_

#ifdef __cplusplus
extern "C" {
#endif

SCRSTAT **ISC_dir(char *, int , char *, char *);
SCRSTAT **ISC_dir_ex(char *, int , char *, char *, int);

FILE *ISC_fopen(char *filename, char *mode);
int ISC_fclose(FILE *fd);
long ISC_ftell(FILE *);
int ISC_read_line(FILE* fd, unsigned char* buf, int lg);
int ISC_fread(char *buf, int lg, int nb, FILE *fd);
int ISC_fwrite(char *buf, int lg, int nb, FILE *fd);
int ISC_getc(FILE *fd);
int ISC_ungetc(int ch, FILE *fd);
int ISC_putc(int ch, FILE *fd);

unsigned long ISC_getcrc32(char *);
int ISC_unlink(char *filename);
int ISC_rename(char *oldname, char *newname);

extern int          SCR_ISC_PORT;           /* JMP 06-12-99 */
extern char         *SCR_ISC_SERVER;        /* JMP 06-12-99 */
extern int          ISC_COMPRESS;           /* JMP 03-06-00 */
extern int          ISC_NB_NEXTS;           /* JMP 08-12-00 */
extern char         SCR_ISC_SEPS[];         /* JMP 26-09-02 */
extern char         *SCR_ISC_LOGIN;         /* JMP 22-10-02 */
extern int          SCR_ISC_ERRNO;          /* JMP 26-10-02 */

#ifdef __cplusplus
}
#endif

#endif /* _S_ISC_H_ */
