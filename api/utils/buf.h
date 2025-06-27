#pragma once

#include "api/constants.h"

#ifdef __cplusplus
extern "C" {
#endif

/* buf.c */
extern char *BUF_alloc(int );
extern void BUF_free(void);
extern void BUF_lock(void);
extern void BUF_unlock(void);
extern char *BUF_memcpy(char *,int );
extern char *BUF_strcpy(char *);

#ifdef __cplusplus
}
#endif
