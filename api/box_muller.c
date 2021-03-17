/* Copyright (c) 2012 the authors listed at the following URL, and/or
the authors of referenced articles or incorporated external code:
http://en.literateprograms.org/Box-Muller_transform_(C)?action=history&offset=20060711193108

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Retrieved from: http://en.literateprograms.org/Box-Muller_transform_(C)?oldid=7011
*/

#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdio.h>

/* Random number generator ran1 from Computers in Physics */
/* Volume 6 No. 5, 1992, 522-524, Press and Teukolsky */
/* To generate real random numbers 0.0-1.0 */
/* Should be seeded with a negative integer */
#define IA 16807
#define IM 2147483647
#define IQ 127773
#define IR 2836
#define NTAB 32
#define EPS (1.2E-07)
#define MAX(a,b) (a>b)?a:b
#define MIN(a,b) (a<b)?a:b

double ran1(idum)
int *idum;
{
    int j,k;
    static int iv[NTAB],iy=0;
    void nrerror();
    static double NDIV = 1.0/(1.0+(IM-1.0)/NTAB);
    static double RNMX = (1.0-EPS);
    static double AM = (1.0/IM);

    if((*idum <= 0) || (iy == 0)) {
        *idum = MAX(-*idum,*idum);
        for(j=NTAB+7; j>=0; j--) {
            k = *idum/IQ;
            *idum = IA*(*idum-k*IQ)-IR*k;
            if(*idum < 0) *idum += IM;
            if(j < NTAB) iv[j] = *idum;
        }
        iy = iv[0];
    }
    k = *idum/IQ;
    *idum = IA*(*idum-k*IQ)-IR*k;
    if(*idum<0) *idum += IM;
    j = iy*NDIV;
    iy = iv[j];
    iv[j] = *idum;
    return MIN(AM*iy,RNMX);
}
#undef IA
#undef IM
#undef IQ
#undef IR
#undef NTAB
#undef EPS
#undef MAX
#undef MIN

double rand2() {
    static int     idum = -12;

    return(ran1(&idum));
}


double rand_normal(double mean, double stddev) {
    static double n2 = 0.0;
    static int n2_cached = 0;
    if(!n2_cached) {
        double x, y, r;
        do {
            /*
                x = 2.0 * rand() / RAND_MAX - 1;
                y = 2.0 * rand() / RAND_MAX - 1;
            */
            x = 2.0 * rand2() - 1;
            y = 2.0 * rand2() - 1;

            r = x*x + y*y;
        }
        while(r == 0.0 || r > 1.0);
        {
            double d = sqrt(-2.0*log(r)/r);
            double n1 = x*d;
            double result;
            n2 = y*d;
            result = n1*stddev + mean;
            n2_cached = 1;
            return result;
        }
    }
    else {
        n2_cached = 0;
        return n2*stddev + mean;
    }
}




int main(int argc, char *argv[]) {
    double  val, ave = 0.0, var = 0.0;
    int i, j, n = 1000;

    if(argc > 1) n  = atoi(argv[1]);
    srand(time(NULL));

    for(i = 0; i < n; i++) {
        val  = rand_normal(0.0, 10.0);
        ave  += val;
        var  += val * val;
    }
    ave = ave / n;
    var = (var - n * ave * ave) / (n - 1);
    printf("Mean = %lf -- Stdev = %lf\n", ave, sqrt(var));
    return 0;
}


/*
int main() {
    double  m = 0.0;
    int buckets[20] = {0};
    int i, j;

    srand(time(NULL));
    for(i=0; i<500; i++) {
        double val = rand_normal(10.0, 3.0);
        int i = (int)floor(val + 0.5);

        if(i >= 0 && i < sizeof(buckets)/sizeof(*buckets))
            buckets[i]++;
    }
    for(i=0; i<sizeof(buckets)/sizeof(*buckets); i++) {
        for(j=0; j<buckets[i]; j++) {
            printf("#");
        }
        printf("\n");
    }
    return 0;
}

*/