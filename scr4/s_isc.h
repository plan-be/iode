#ifndef _S_ISC_H_
#define _S_ISC_H_

SCRSTAT **ISC_dir(char *, int , char *, char *);
SCRSTAT **ISC_dir_ex(char *, int , char *, char *, int);
FILE *ISC_fopen(char *, char *);
unsigned long ISC_getcrc32(char *);
long ISC_ftell(FILE *);

extern int          SCR_ISC_PORT;           /* JMP 06-12-99 */
extern char         *SCR_ISC_SERVER;        /* JMP 06-12-99 */
extern int          ISC_COMPRESS;           /* JMP 03-06-00 */
extern int          ISC_NB_NEXTS;           /* JMP 08-12-00 */
extern char         SCR_ISC_SEPS[];         /* JMP 26-09-02 */
extern char         *SCR_ISC_LOGIN;         /* JMP 22-10-02 */
extern int          SCR_ISC_ERRNO;          /* JMP 26-10-02 */

#endif /* _S_ISC_H_ */
