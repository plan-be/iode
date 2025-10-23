#pragma once

#include "api/constants.h"

/* buf.c */
char *BUF_alloc(int );
void BUF_free(void);
void BUF_lock(void);
void BUF_unlock(void);
char *BUF_memcpy(char *,int );
char *BUF_strcpy(char *);
