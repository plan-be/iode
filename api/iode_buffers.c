#include "iode_buffers.h"
#include <s_swap.h>

/**
 * Global buffer manipulation.
 * Goal : avoiding multiple malloc / realloc / free to limit memory fragmentation.
 */

char* BUF_DATA = NULL;
int   BUF_LEN = 0;
int   BUF_LCK = 0;

/**
 * Allocates or reallocate the global buffer BUF_DATA to a min size of len bytes. 
 * The content of the buffer is lost after this function.
 * The buffer can be locked (against reallocation) by BUF_lock().
 * 
 * @param len minimum length of the buffer
 * @return char *BUF_LEN reallocated if len < current BUF_LEN
 */

char* BUF_alloc(int len)
{
	if (BUF_LCK) {
		fprintf(stderr, "Internal BUFFER LOCKED (BUF_alloc)");
		exit(100);
	}

	if (BUF_LEN < len) {
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

char* BUF_memcpy(char* ptr, int lg)
{
	char* buf;

	buf = BUF_alloc(lg);
	memcpy(buf, ptr, lg);
	return(buf);
}

char* BUF_strcpy(char* ptr)
{
	return(BUF_memcpy(ptr, strlen(ptr) + 1));
}
