#pragma once

#include "api/constants.h"
#include "api/objs/kdb.h"
#include "api/objs/equations.h"
#include "api/objs/identities.h"
#include "api/objs/scalars.h"
#include "api/objs/tables.h"
#include "api/objs/variables.h"

#ifdef __cplusplus
extern "C" {
#endif

/* k_cmp.c */
extern double K_CMP_EPS;
extern int K_cmp(char *,KDB *,KDB *);
extern int K_cmp_eqs(EQ* eq1, EQ* eq2, char* name);
extern int K_cmp_idt(IDT* idt1, IDT* idt2);
extern int K_cmp_scl(SCL* scl1, SCL* scl2);
extern int K_cmp_tbl(TBL* tbl1, TBL* tbl2);
extern int K_cmp_var(VAR var1, VAR var2);

#ifdef __cplusplus
}
#endif
