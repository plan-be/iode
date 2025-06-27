#pragma once

#include "api/constants.h"
#include "api/objs/objs.h"

#ifdef __cplusplus
extern "C" {
#endif

extern KDB *KI_exec(KDB *, KDB *, int , char **, KDB *, int , char **, SAMPLE *);

#ifdef __cplusplus
}
#endif
