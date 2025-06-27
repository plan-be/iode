#pragma once

#include "api/constants.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------- STRUCTS ----------------------------*/

// SCL = Scalar (struct containing a scalar value, and if estimated, its relaxation parameter and its standard error).
typedef struct _scl_ {
    double    val;       // Value of the scalar
    double    relax;     // Relaxation parameter. Used only for estimation.
			    // If null, the parameter will not be estimated.
			    // If 0 < relax < 1, the convergence of the estimation will be slowed down. To use when the estimation process diverges.
    double    std;       // Standard error. Result of the estimation.
} SCL;

/*----------------------- MACROS ----------------------------*/

#define KSVAL(kdb, pos)     ((SCL *)  K_oval0(kdb, pos))

#ifdef __cplusplus
}
#endif
