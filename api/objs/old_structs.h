#pragma once

#include "api/constants.h"
#include "api/objs/kdb.h"


typedef char OONAME[OK_MAX_NAME + 1];


struct OKOBJ 
{
    short       o_val;
	OONAME      o_name;
    char        o_pad[3];
};

struct OKDB 
{
    OKOBJ       *k_objs;
    long        k_nb;
    short       k_type;
    short       k_mode;
    char        k_arch[LMAGIC];
    char        k_magic[LMAGIC];
    OFNAME       k_name;
    char        k_desc[K_MAX_DESC];
    char        k_data[K_MAX_DESC];     /* Client Data */
};

struct OKDB643 
{
    OKOBJ       *k_objs;
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
};

struct OLNAME 
{
    OONAME  name;
    char    pad[3];
    short   pos;
};

struct OOCLEC 
{
    short   tot_lg;
	short   exec_lg;
	short   nb_names;
    char    duplicated_endo;
    char    pad;
    OLNAME  lnames[1];
};

struct LNAME 
{
    ONAME   name;   // scalar or variable name
    char    pad[3];
    long    pos;
};

struct OCLEC
{
    long    tot_lg;      
    long    exec_lg;       
    short   nb_names;   // number of scalar and variables names
    char    dupendo;    // duplicate endogenous variable in the LEC expression
    char    pad;
    LNAME   lnames[1];  // list of of scalar and variable names
};
