#pragma once

#include "api/constants.h"

/* buf.c */
char *BUF_alloc(int len);
void BUF_free();
void BUF_lock();
void BUF_unlock();
char *BUF_memcpy(char *ptr, int lg);
char *BUF_strcpy(char *ptr);
