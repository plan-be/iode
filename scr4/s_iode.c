/**
 *  Replacement functions for IODE implementation. 
 *  These fns are generally a simplified but adequate version of the full fledge scr4 implementation.
 *  
 *  
 */

#include "scr4.h"

FILE *ISC_fopen(char *filename, char *mode)
{
    return(fopen(filename, mode));
}

int ISC_fclose(FILE *fd)
{
    return(fclose(fd));
}

ISC_unlink(char *filename)
{
#ifdef __GNUC__
    return(unlink(filename));
#else
    return(_unlink(filename));
#endif
}

ISC_rename(char *oldname, char *newname)
{
    return(rename(oldname, newname));
}

ISC_read_line(fd, buf, lg)
FILE            *fd;
unsigned char   *buf;
int             lg;
{
    return(SCR_read_line(fd, buf, lg));
}

ISC_fwrite(char *buf, int lg, int nb, FILE *fd)
{
    return(fwrite(buf, lg, nb, fd));
}

ISC_getc(FILE *fd)
{
    return(getc(fd));
}

ISC_putc(int ch, FILE *fd)
{
    return(putc(ch, fd));
}
