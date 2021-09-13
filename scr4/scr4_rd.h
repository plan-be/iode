#ifndef _SCR4_RD_H_
#define _SCR4_RD_H_

#ifndef SCR_DTYPE      /* BP_MODIF2 29-11-1993 11:54 */
#define SCR_DTYPE

#define SCR_TYPE_PAGE   9500
#define SCR_TYPE_MENU   9501
#define SCR_TYPE_ISAM   9502
#define SCR_TYPE_MPAGE  9503
#define SCR_TYPE_BC     9504
#define SCR_TYPE_ACT    9505
#define SCR_TYPE_APPS   9506
#define SCR_TYPE_PRINTER 9507
#define SCR_TYPE_FNKEYS 9508
#define SCR_TYPE_ABAR   9509
#define SCR_TYPE_APPL   9510
#define SCR_TYPE_MENUI  9511
#define SCR_TYPE_REPORT 9512
#define SCR_TYPE_SMPAGE 9513
#define SCR_TYPE_TPAGES 9514
#define SCR_TYPE_WTOOLB 9515        /* BP_M 18-04-1997 */

#endif

extern PAGE     *PG_read(/* char **/) ;
extern MENU     *MN_read(/* char **/) ;
extern ISAM     *IS_read(/* char **/) ;
extern ACTION   *ACT_read();
extern FNKEYS   *FNK_read();
extern PR_DEF   *PR_read(/* char *, FILE * */);
extern A_BAR    *AB_read(/* char **/) ;
extern APPL     *AP_read(/* char **/) ;
extern BC       *BC_read(/* char **/) ;
extern MPAGE    *MP_read(/* char **/) ;
extern TPAGES   *TP_read(/* char **/) ;
extern WTOOLBAR *TB_read(/* char **/) ;     /* BP_M 18-04-1997 */

#endif /* _SCR4_RD_H_ */

