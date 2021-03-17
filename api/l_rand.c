#include <math.h>
#include <stdlib.h>

#define u_rand() ((double) rand()/(1.0+(double) RAND_MAX))
double  z2 =0.0;
int  use_z2 = 1;

double randBoxMuller(double rv_mean, double rv_sd) {
    double  z1, x, y, w;
    if(use_z2 && z2 != 0.0) {
        z1 = z2;
        z2 = 0.0;
    }
    else {
        do {
            /* choose x,y in uniform square (-1,-1) to (+1,+1) */
            x = -1.0 + 2.0 * u_rand();
            y = -1.0 + 2.0 * u_rand();
            /* see if it is in the unit circle */
            w = x * x + y * y;
        }
        while(w > 1.0 || w == 0);
        /* Box-Muller transform */
        w = sqrt(-2.0 * log(w)/w);
        z2 = x * w;
        z1 = y * w;
    }
    return(rv_mean + rv_sd * z1);
}


/* Gamma function in double precision */
double dgamma(double x) {
    int k, n;
    double w, y;
    n = x < 1.5 ? -((int)(2.5 - x)) : (int)(x - 1.5);
    w = x - (n + 2);
    y = ((((((((((((-1.99542863674e-7 * w + 1.337767384067e-6) * w -
                   2.591225267689e-6) * w - 1.7545539395205e-5) * w +
                 1.45596568617526e-4) * w - 3.60837876648255e-4) * w -
               8.04329819255744e-4) * w + 0.008023273027855346) * w -
             0.017645244547851414) * w - 0.024552490005641278) * w +
           0.19109110138763841) * w - 0.233093736421782878) * w -
         0.422784335098466784) * w + 0.99999999999999999;
    if(n > 0) {
        w = x - 1;
        for(k = 2; k <= n; k++) {
            w *= x - k;
        }
    }
    else {
        w = 1;
        for(k = 0; k > n; k--) {
            y *= x - k;
        }
    }
    return w / y;
}





