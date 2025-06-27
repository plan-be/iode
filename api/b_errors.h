#pragma once

#include "api/constants.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ---------------- ENUMS ---------------- */

enum IodeBErrorMsg
{
    OM_ILL_ARGS = 28,
    OM_DEL_FAILED_1,
    OM_DREN_FAILED_2,
    OM_DDUP_NO_SENSE,
    OM_DDUP_FAILED_2,
    OM_DUPD_SCL_ERR,
    OM_UPD_TBLS,
    OM_DUPD_VAR_ERR_1,
    OM_PATTERN_1,
    OM_LIST_SET_ERR_1,
    OM_NO_FILE_1,
    OM_EEST_SMPL_2,
    OM_EEST_EMPTY,
    OM_FCOPY_OPEN_1,
    OM_FCOPY_CREATE_1,
    OM_FCOPY_WRITE_1,
    OM_CANNOT_DEL_1,
    OM_FPRINT_OPEN_1,
    OM_FPRINT_WRITE_1,
    OM_CANNOT_RENAME_1,
    OM_EXEC_SMPL_2,
    OM_SIM_SMPL_2,
    OM_PRINT_ERR_1,
    OM_OBJ_NOT_FOUND_1,
    OM_CANNOT_OPEN,
    OM_NBDEC_ERR_1,
    OM_TBL_NOT_FOUND_1,
    OM_TBL_NOT_PRINTED_1,
    OM_SMPL_ERR,
    OM_PROF_CANNOT_LOAD_1,
    OM_PROF_CANNOT_SAVE_1,
    OM_TBL_NOT_VIEWED_1,
    OM_INV_ODE_FILE_1,
    OM_NO_SCL,
    OM_SMPL_SYNTAX_1,
    OM_CANNOT_PRINT_LINE_1
};

/* ---------------- GLOBALS ---------------- */

extern char** B_ERROR_MSG;

/* ---------------- FUNCS ---------------- */

extern void B_seterror(char *,...);
extern void B_seterrn(int , ...);

extern void B_add_error(char* msg);
extern char* B_get_last_error(void);
extern void B_display_last_error(void);
extern void B_print_last_error(void);
extern void B_clear_last_error(void);
//extern void B_reset_error(void);
extern char *B_msg(int );
extern int B_get1int(char *);

#ifdef __cplusplus
}
#endif
