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

int ISC_unlink(char *filename)
{
#ifdef __GNUC__
    return(unlink(filename));
#else
    return(_unlink(filename));
#endif
}

int ISC_rename(char *oldname, char *newname)
{
    return(rename(oldname, newname));
}

int ISC_read_line(FILE *fd, unsigned char *buf, int lg)
{
    return(SCR_read_line(fd, buf, lg));
}

int ISC_fwrite(char *buf, int lg, int nb, FILE *fd)
{
    return(fwrite(buf, lg, nb, fd));
}

int ISC_getc(FILE *fd)
{
    return(getc(fd));
}

int ISC_putc(int ch, FILE *fd)
{
    return(putc(ch, fd));
}
