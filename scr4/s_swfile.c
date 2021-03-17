#include <string.h>
#include <stdio.h>
#include <s_swap.h>
#ifdef DOS
#include <io.h>
#include <sys\stat.h>
#else
#include <sys/types.h>
#include <sys/stat.h>

#ifdef POWERPC
#include <fcntl.h>
#else
#include <sys/fcntl.h>
#endif

#endif

#include <stdlib.h>

int     SW_HANDLE = -1;

extern  char    *getenv();

/*NH*/
/*  Cr‚ation du fichier de swap : la varaible d'environnement SWAPFILE
    contient le nom du disque et du fichier … utiliser.
*/

SW_open()
{
    char    *ptr;
    int     o_fmode, ext;

    SW_panic(1); /* JMP 23-05-00 */

    if(SW_HANDLE >= 0) return(0);

    if(SW_FILE[0] == 0) {
	ptr = getenv("SWAPFILE");
	if(ptr == 0) strcpy(SW_FILE, "swapfile.swa");
	else         strcpy(SW_FILE, ptr);
	}

    for(ext = 'a' ; ext < 'z' ; ext++) {            /* JMP 06-12-93 */
	    if(access(SW_FILE, 0)) break;
	    SW_FILE[strlen(SW_FILE) - 1] = ext;
	    }

#ifdef DOS
    o_fmode = _fmode;
    _fmode = O_BINARY;
#endif
    SW_HANDLE = creat(SW_FILE, S_IREAD | S_IWRITE);
#ifdef DOS
    _fmode = o_fmode;
#endif
    SW_FILE_POS = 0;

    if(SW_HANDLE < 0) SW_panic(1);
    close(SW_HANDLE);
#ifdef DOS
    SW_HANDLE = open(SW_FILE, O_RDWR | O_BINARY);
#else
    SW_HANDLE = open(SW_FILE, O_RDWR);
#endif
    if(SW_HANDLE < 0) SW_panic(1);
    return(0);
}
/*NH*/
int SW_close()
{
    if(SW_HANDLE >= 0) {
	close(SW_HANDLE);
	unlink(SW_FILE);
	}
    SW_HANDLE = -1;
    return(0);
}

/*NH*/
int SW_append_file(ptr)
char    *ptr;
{
/*    extern long lseek(); cfr stdio.h /* JMP 03-01-98 */
    long        pos;

    if(SW_open()) return(-1);
    pos = lseek(SW_HANDLE, 0L, 2);
    if(pos < 0) SW_panic(2);
    if(write(SW_HANDLE, ptr, SW_SEG_SIZE) != SW_SEG_SIZE) SW_panic(3);
    SW_FILE_POS++;
    return(0);
}

#define SW_BUF_SIZE SW_SEG_SIZE

/*NH*/
int SW_swap(ptr, ref)
char    *ptr;
int     ref;
{
    char   *buf;
    long   pos, i, chk;
/*  long   lseek(); cfr stdio.h /* JMP 03-01-98 */

    //buf = (char *)malloc(SW_BUF_SIZE);
    buf = (char *)SCR_malloc(SW_BUF_SIZE); /* JMP 22-08-2012 */
    if(buf == 0) SW_panic(5);
    pos = (long) ref * (long) SW_SEG_SIZE;
    for(i = 0 ; i < SW_SEG_SIZE ; i += SW_BUF_SIZE) {
	chk = lseek(SW_HANDLE, pos + i, 0);
	if(chk < 0)                                     SW_panic(2);
	chk = read(SW_HANDLE, buf, SW_BUF_SIZE);
	if(chk != SW_BUF_SIZE)                          SW_panic(4);
	chk = lseek(SW_HANDLE, pos + i, 0);
	if(chk < 0)                                     SW_panic(2);
	chk = write(SW_HANDLE, ptr + i, SW_BUF_SIZE);
	if(chk != SW_BUF_SIZE)                          SW_panic(3);
	memcpy(ptr + i, buf, SW_BUF_SIZE);
	}
    //free(buf);
    SCR_free(buf); /* JMP 22-08-2012 */
    return(0);
}




