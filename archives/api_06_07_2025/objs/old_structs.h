#pragma once

#include "api/constants.h"
#include "api/objs/kdb.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef char    OONAME[OK_MAX_NAME + 1];


typedef struct  _okobj_ {
    short       o_val;
	OONAME      o_name;
    char        o_pad[3];
} OKOBJ;

typedef struct _okdb_ {
    KOBJ        *k_objs;
    long        k_nb;
    short       k_type;
    short       k_mode;
    char        k_arch[LMAGIC];
    char        k_magic[LMAGIC];
    OFNAME       k_name;
    char        k_desc[K_MAX_DESC];
    char        k_data[K_MAX_DESC];     /* Client Data */
} OKDB;

typedef struct _okdb643_ {
    KOBJ        *k_objs;
    long        k_nb;
    short       k_type;
    short       k_mode;
    char        k_arch[LMAGIC];
    char        k_magic[LMAGIC];
    OFNAME       k_name;
    char        k_desc[K_MAX_DESC];
    char        k_data[K_MAX_DESC];     /* Client Data */
    char        k_compressed;           /* IODE64K */
    char        k_reserved[63];         /* IODE64K */
} OKDB643;

typedef struct _olname_ {
    OONAME   name;
    char    pad[3];
    short   pos;
} OLNAME;

typedef struct _oclec_ {
    short   tot_lg,         /* SWHDL */
	    exec_lg,
	    nb_names;
    char    dupendo;
    char    pad;
    OLNAME  lnames[1];
} OCLEC;

#ifdef __cplusplus
}
#endif
