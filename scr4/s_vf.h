#ifndef _S_VF_
#define _S_VF_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define s_long      sizeof(long)

typedef struct _vfile_ {
    FILE    *vf_vdt;
    FILE    *vf_vps;
    long    vf_blksize;
    long    vf_nbb;
    long    vf_nb;
    long    vf_nbh;
    long    vf_nbf;
    long    vf_onbb;
    long    vf_onb;
    long    vf_onbh;
    long    vf_onbf;
} VFILE;

/* FUNCTIONS */
#include <s_provf.h>

#endif /* _S_VF_ */
