/**
 * @author Jean-Marc PAUL 
 * @author Geert BRYON
 * 
 * Tables of pointers to functions for reading and writing IODE objects in ASCII and CSV format.
 *
 *    KDB  *(*K_load_asc[])() : array of 7 functions to load files containing ASCII definitions of IODE objects.
 *  
 *    int (*K_save_asc[])(): array of 7 functions to save IODE objects in ascii files.
 * 
 *    int (*K_save_csv[])(): array of 7 functions to save IODE objects in csv format.
 */

#include "api/ascii/ascii.h"

KDB  *(*K_load_asc[])() = {
    KC_load_asc,
    KE_load_asc,
    KI_load_asc,
    KL_load_asc,
    KS_load_asc,
    KT_load_asc,
    KV_load_asc
};

int (*K_save_asc[])() = {
    KC_save_asc,
    KE_save_asc,
    KI_save_asc,
    KL_save_asc,
    KS_save_asc,
    KT_save_asc,
    KV_save_asc
};

int (*K_save_csv[])() = {
    KC_save_csv,
    KE_save_csv,
    KI_save_csv,
    KL_save_csv,
    KS_save_csv,
    KT_save_csv,
    KV_save_csv
};

