
#define IS_INPUT(fld) ((fld)->fld_io == 0 || (fld)->fld_io == SCR_ACCESSIBLE - SCR_INPUT)
#define IS_BEST_COL   (abs(col - c_col) < abs(b_col - c_col))

#define UP      0
#define DOWN    1
#define LEFT    0
#define RIGHT   1

#define PG_up_field(pg, nb)    PG_updn_field((pg), (pg)->pg_flds[nb].fld_line, (pg)->pg_flds[nb].fld_column, UP)
#define PG_dn_field(pg, nb)    PG_updn_field((pg), (pg)->pg_flds[nb].fld_line, (pg)->pg_flds[nb].fld_column, DOWN)
#define PG_left_field(pg, nb)  PG_leftright_field((pg), (pg)->pg_flds[nb].fld_line, (pg)->pg_flds[nb].fld_column, LEFT)
#define PG_right_field(pg, nb) PG_leftright_field((pg), (pg)->pg_flds[nb].fld_line, (pg)->pg_flds[nb].fld_column, RIGHT)

