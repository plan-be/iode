#pragma once

#include "api/constants.h"


/*----------------------- STRUCTS ----------------------------*/

// SCL = Scalar (struct containing a scalar value, and if estimated, its relaxation parameter and its standard error).
struct SCL 
{
    double    val;       // Value of the scalar
    double    relax;     // Relaxation parameter. Used only for estimation.
			    // If null, the parameter will not be estimated.
			    // If 0 < relax < 1, the convergence of the estimation will be slowed down. To use when the estimation process diverges.
    double    std;       // Standard error. Result of the estimation.
};

/*----------------------- MACROS ----------------------------*/

#define KSVAL(kdb, pos)     ((SCL *)  K_oval0(kdb, pos))
