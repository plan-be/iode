#pragma once

/**
 * Global buffer manipulation.
 * Goal : avoiding multiple malloc / realloc / free to limit memory fragmentation.
 */

extern char*	BUF_DATA;
extern char*	BUF_alloc(int len);
extern void		BUF_free();
extern void		BUF_lock();
extern void		BUF_unlock();
extern char*	BUF_memcpy(char* ptr, int lg);
extern char*	BUF_strcpy(char* ptr);