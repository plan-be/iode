#include "iode.h"

/* global buffer to do allocating in */
char    *BUF_DATA = NULL;
int     BUF_LEN = 0;
int     BUF_LCK = 0;

/*
    Allocates  a  global  buffer of at least len bytes. Real
    length is LBUF. The buffer is BUF. Warning : the buffer
    is not reset to zero at each call.
*/

char *BUF_alloc(len)
int len;
{
    if(BUF_LCK) {
	fprintf(stderr, "Internal BUFFER LOCKED (BUF_alloc)");
	exit(100);
    }

    if(BUF_LEN < len) {
	SW_nfree(BUF_DATA);
	BUF_DATA = SW_nalloc(len + 10240);
	BUF_LEN = len + 10240;
    }

    if (BUF_DATA == NULL) {
	fprintf(stderr, "Internal allocation error (BUF_alloc)");
	exit(100);
    }
    return(BUF_DATA);
}

/* JMP 14-11-2012 */
void BUF_free()
{
    SW_nfree(BUF_DATA);
    BUF_DATA = 0;
    BUF_LEN = 0;
}

void BUF_lock()
{
    BUF_LCK = 1;
}

void BUF_unlock()
{
    BUF_LCK = 0;
}

char *BUF_memcpy(ptr, lg)
char    *ptr;
{
    char    *buf;

    buf = BUF_alloc(lg);
    memcpy(buf, ptr, lg);
    return(buf);
}

char *BUF_strcpy(ptr)
char    *ptr;
{
    return(BUF_memcpy(ptr, strlen(ptr) + 1));
}









