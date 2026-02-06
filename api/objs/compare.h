#pragma once

#include "api/constants.h"
#include "api/objs/kdb.h"
#include "api/objs/comments.h"
#include "api/objs/equations.h"
#include "api/objs/identities.h"
#include "api/objs/lists.h"
#include "api/objs/scalars.h"
#include "api/objs/tables.h"
#include "api/objs/variables.h"

// Threshold for Variables comparison
inline double K_COMPARE_EPS = 1e-7;

/* k_cmp.c */
bool K_compare_var(const Variable& var1, const Variable& var2);
// TODO : make a method of KDBTemplate class
int K_compare(const std::string& name, KDB* kdb1, KDB* kdb2);
