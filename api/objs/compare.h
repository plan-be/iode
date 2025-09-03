#pragma once

#include "api/constants.h"
#include "api/objs/kdb.h"
#include "api/objs/equations.h"
#include "api/objs/identities.h"
#include "api/objs/scalars.h"
#include "api/objs/tables.h"
#include "api/objs/variables.h"

// Threshold for VAR comparisons 
inline double K_CMP_EPS = 1e-7;            // Threshold for VAR comparisons 

/* k_cmp.c */
int K_cmp(char *,KDB *,KDB *);
int K_cmp_eqs(Equation* eq1, Equation* eq2, char* name);
int K_cmp_scl(Scalar* scl1, Scalar* scl2);
int K_cmp_tbl(TBL* tbl1, TBL* tbl2);
int K_cmp_var(VAR var1, VAR var2);
