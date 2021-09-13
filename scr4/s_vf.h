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

#ifndef SCRPROTO
extern FILE *VF_hdl_open();
extern long VF_hdl_get();
extern long VF_hdl_find_free();
extern FILE *VF_blk_open();
extern long VF_blk_find_free();
extern VFILE *VF_open();
extern long VF_write();
extern char *VF_read();
extern long VF_read_long();

#else /* SCRPROTO */
#include <s_provf.h>
#endif /* SCRPROTO */

#endif /* _S_VF_ */
