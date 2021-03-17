#include "iode.h"

char    K_LABELS[] = "KOBJS 4.02\032";
char    K_LABELD[] = "KOBJS 504d\032";
char    K_LABELX[] = "KOBJS 564A\032";  /* IODE64K */
//char    K_LABELY[] = "KOBJS 6.44\032";  /* IODE64K + FNAME = 512 */
char    K_LABEL[] = "KOBJS 564A\032";  // Current version

/*
#ifdef REALD
char    K_LABEL[]  = "KOBJS 504d\032";
#else
char    K_LABEL[] = "KOBJS 4.02\032";
#endif
IODE64K
*/


char    *K_TYPES[] = {
    "Cmt",
    "Eqs",
    "Idt",
    "Lst",
    "Scl",
    "Tbl",
    "Var"
};

K_filetype(filename, descr, nobjs, smpl)
char    *filename, *descr;
int     *nobjs;
SAMPLE  *smpl;
{
    FILE    *fd;
    int     i, vers;
    KDB     kdb;
    FNAME   file;
    char    label[80];

    if(descr) descr[0] = 0;
    if(nobjs) *nobjs = 0;
    if(smpl)  memset(smpl, 0, sizeof(SAMPLE));
    if(filename == 0 ||
	    filename[0] == 0 || filename[0] == '-') return(-1);

    strcpy(file, filename);
    K_strip(file); /* JMP 19-04-98 */
#ifdef DOS
    fd = fopen(file, "rb");
#else
    fd = fopen(file, "rb");
#endif
    if(fd == 0) return(-2);

    fread(label, strlen(K_LABEL), 1, fd);
    vers = K_calcvers(label);
	if(vers < 0 || vers > 3) {
		fclose(fd);
		return(-1);
    }

	K_read_kdb(&kdb, fd, vers);
    fclose(fd);

    if(descr) strcpy(descr, kdb.k_desc);
    if(nobjs) *nobjs = KNB(&kdb);
    if(smpl && kdb.k_type == K_VAR)
	memcpy(smpl, KSMPL(&kdb), sizeof(SAMPLE));

    return(kdb.k_type);
}








