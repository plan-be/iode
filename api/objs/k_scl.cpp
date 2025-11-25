#include "api/objs/scalars.h"
#include "api/print/print.h"

bool Scalar::print_definition() const
{    
    char tcoef[128], trelax[128], tstd[128], tttest[128];
    T_fmt_val(tcoef,  (double) value, 15, K_NBDEC);
    T_fmt_val(trelax, (double) relax, 15, -1);
    T_fmt_val(tstd,   (double) std,   15, K_NBDEC);
    
    double ttest = calculate_t_test();
    T_fmt_val(tttest, ttest, 15, K_NBDEC);
    W_printfReplEsc((char*) "%s ~i(%s, %s, %s)\n",  tcoef, trelax, tstd, tttest);

    return true;        
}

std::size_t hash_value(const Scalar& scalar)
{
    std::hash<Scalar> scl_hash;
    return scl_hash(scalar);
}
