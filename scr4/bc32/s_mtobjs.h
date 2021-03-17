
#ifndef _SCR_OBJS_H_
#define _SCR_OBJS_H_

#include <scr4_str.h>

extern FNKEYS *scr_f3f10;
extern PAGE *scr_prt;
extern PAGE *scr_conf;
extern PAGE *scr_err;
extern MENU *scr_bcm1;
extern MENU *scr_bcm2;
extern PAGE *scr_incr;
	extern OBJ scr_incr_nbl;
	extern OBJ scr_incr_nbs;
#define scr_incr_NBL	(*(long *)(scr_incr->pg_rec + 0))
#define scr_incr_vNBL	((long *)(scr_incr->pg_rec + 0))
#define scr_incr_NBS	(*(long *)(scr_incr->pg_rec + 4))
extern A_BAR *scr_abedis;
extern MENU *scr_bcie;
extern PAGE *pg_ap_isam;
extern APPL *scr_ap_isam;
extern FNKEYS *fk_esc;
extern FNKEYS *fk_f1;
extern FNKEYS *fk_f5;
extern FNKEYS *fk_f6;
extern FNKEYS *fk_f7;
extern FNKEYS *fk_f8;
extern FNKEYS *fk_f10;
extern FNKEYS *tt_enter;
extern FNKEYS *tt_kgene;
extern MENU *tt_yes;
extern MENU *tt_no;
extern PAGE *tt_add;
#define tt_add_NAME	(tt_add->pg_rec + 0)
extern PAGE *tt_chang;
#define tt_chang_FORM	(tt_chang->pg_rec + 0)
typedef char _T_I31_[31];
#define tt_chang_vFORM	((_T_I31_ *)(tt_chang->pg_rec + 0))
#define tt_chang_TO	(tt_chang->pg_rec + 31)
#define tt_chang_NBCHANGE	(tt_chang->pg_rec + 62)
#define tt_chang_CASE	(*(short *)(tt_chang->pg_rec + 66))
#define tt_chang_vCASE	((short *)(tt_chang->pg_rec + 66))
#define tt_chang_ALL	(*(short *)(tt_chang->pg_rec + 68))
#define tt_chang_vALL	((short *)(tt_chang->pg_rec + 68))
#define tt_chang_SAVE	(*(short *)(tt_chang->pg_rec + 70))
#define tt_chang_vSAVE	((short *)(tt_chang->pg_rec + 70))
#define tt_chang_CONF	(*(short *)(tt_chang->pg_rec + 72))
extern PAGE *tt_fill;
#define tt_fill_CHAR	(tt_fill->pg_rec + 0)
extern PAGE *tt_name;
#define tt_name_NAME	(tt_name->pg_rec + 0)
extern PAGE *tt_pglin;
#define tt_pglin_LINE	(*(long *)(tt_pglin->pg_rec + 0))
extern PAGE *tt_searc;
#define tt_searc_TEXT	(tt_searc->pg_rec + 0)
#define tt_searc_CASE	(*(short *)(tt_searc->pg_rec + 32))
#define tt_searc_vCASE	((short *)(tt_searc->pg_rec + 32))
#define tt_searc_ALL	(*(short *)(tt_searc->pg_rec + 34))
extern PAGE *tt_ttab;
#define tt_ttab_TAB	(*(long *)(tt_ttab->pg_rec + 0))
#define tt_ttab_vTAB	((long *)(tt_ttab->pg_rec + 0))
#define tt_ttab_INSERT	(*(short *)(tt_ttab->pg_rec + 76))
extern MENU *ttm_just;
extern MENU *ttm_del;
extern PAGE *tt_margin;
#define tt_margin_ACTIVE	(*(short *)(tt_margin->pg_rec + 0))
#define tt_margin_CURRENT	(*(long *)(tt_margin->pg_rec + 4))
#define tt_margin_JUST	(*(short *)(tt_margin->pg_rec + 8))
#define tt_margin_vJUST	((short *)(tt_margin->pg_rec + 8))
#define tt_margin_DBL	(*(short *)(tt_margin->pg_rec + 10))
#define tt_margin_SET	(*(long *)(tt_margin->pg_rec + 12))
#define tt_margin_vSET	((long *)(tt_margin->pg_rec + 12))
extern PAGE *tt_findn;
#define tt_findn_YN	(tt_findn->pg_rec + 0)
extern PAGE *tt_chn;
#define tt_chn_YN	(tt_chn->pg_rec + 0)
extern MENU *ttm_edimp;
extern ACTION *tta_imp;
extern PAGE *ttp_printer;
#define ttp_printer_NAME	(ttp_printer->pg_rec + 0)
#define ttp_printer_DEVICE	(ttp_printer->pg_rec + 21)
#define ttp_printer_SYSTEM	(*(short *)(ttp_printer->pg_rec + 32))
#define ttp_printer_VAL	(*(long *)(ttp_printer->pg_rec + 76))
#define ttp_printer_vVAL	((long *)(ttp_printer->pg_rec + 76))
extern PAGE *tt_configure;
#define tt_configure_VSCROL	(*(long *)(tt_configure->pg_rec + 0))
#define tt_configure_vVSCROL	((long *)(tt_configure->pg_rec + 0))
#define tt_configure_HSCROL	(*(long *)(tt_configure->pg_rec + 4))
#define tt_configure_CTYPE	(*(short *)(tt_configure->pg_rec + 8))
#define tt_configure_vCTYPE	((short *)(tt_configure->pg_rec + 8))
#define tt_configure_COPY	(*(short *)(tt_configure->pg_rec + 10))
#define tt_configure_SYSTEM	(tt_configure->pg_rec + 12)
typedef char _T_I41_[41];
#define tt_configure_vSYSTEM	((_T_I41_ *)(tt_configure->pg_rec + 12))
#define tt_configure_ALTF	(tt_configure->pg_rec + 53)
#define tt_configure_vALTF	((_T_I41_ *)(tt_configure->pg_rec + 53))
#define tt_configure_CLOCK	(*(short *)(tt_configure->pg_rec + 176))
#define tt_configure_vCLOCK	((short *)(tt_configure->pg_rec + 176))
#define tt_configure_JUMP	(*(short *)(tt_configure->pg_rec + 178))
#define tt_configure_vJUMP	((short *)(tt_configure->pg_rec + 178))
#define tt_configure_ALTP	(*(short *)(tt_configure->pg_rec + 180))
#define tt_configure_TBKP	(*(long *)(tt_configure->pg_rec + 184))
#define tt_configure_SBKP	(*(short *)(tt_configure->pg_rec + 188))
#define tt_configure_NBKP	(tt_configure->pg_rec + 190)
#define tt_configure_SAVETAB	(*(short *)(tt_configure->pg_rec + 194))
#define tt_configure_vSAVETAB	((short *)(tt_configure->pg_rec + 194))
#define tt_configure_POSL	(*(short *)(tt_configure->pg_rec + 196))
#define tt_configure_MTHELP	(tt_configure->pg_rec + 198)
#define tt_configure_vMTHELP	((_T_I41_ *)(tt_configure->pg_rec + 198))
#define tt_configure_USERHELP	(tt_configure->pg_rec + 239)
extern PAGE *ttp_print;
#define ttp_print_NBLP	(*(long *)(ttp_print->pg_rec + 0))
#define ttp_print_vNBLP	((long *)(ttp_print->pg_rec + 0))
#define ttp_print_NBL	(*(long *)(ttp_print->pg_rec + 4))
#define ttp_print_vNBL	((long *)(ttp_print->pg_rec + 4))
#define ttp_print_NPI	(*(long *)(ttp_print->pg_rec + 8))
extern PAGE *ttp_nbexep;
#define ttp_nbexep_FLINE	(*(long *)(ttp_nbexep->pg_rec + 0))
#define ttp_nbexep_vFLINE	((long *)(ttp_nbexep->pg_rec + 0))
#define ttp_nbexep_LLINE	(*(long *)(ttp_nbexep->pg_rec + 4))
#define ttp_nbexep_vLLINE	((long *)(ttp_nbexep->pg_rec + 4))
#define ttp_nbexep_FPAGE	(*(long *)(ttp_nbexep->pg_rec + 8))
#define ttp_nbexep_vFPAGE	((long *)(ttp_nbexep->pg_rec + 8))
#define ttp_nbexep_NB	(*(long *)(ttp_nbexep->pg_rec + 12))
#define ttp_nbexep_vNB	((long *)(ttp_nbexep->pg_rec + 12))
#define ttp_nbexep_NLP	(*(long *)(ttp_nbexep->pg_rec + 16))
#define ttp_nbexep_vNLP	((long *)(ttp_nbexep->pg_rec + 16))
#define ttp_nbexep_NCL	(*(long *)(ttp_nbexep->pg_rec + 20))
#define ttp_nbexep_vNCL	((long *)(ttp_nbexep->pg_rec + 20))
#define ttp_nbexep_NLDP	(*(long *)(ttp_nbexep->pg_rec + 24))
#define ttp_nbexep_vNLDP	((long *)(ttp_nbexep->pg_rec + 24))
#define ttp_nbexep_NLFP	(*(long *)(ttp_nbexep->pg_rec + 28))
#define ttp_nbexep_TITL	(ttp_nbexep->pg_rec + 32)
#define ttp_nbexep_vTITL	((_T_I41_ *)(ttp_nbexep->pg_rec + 32))
#define ttp_nbexep_TITC	(ttp_nbexep->pg_rec + 73)
#define ttp_nbexep_vTITC	((_T_I41_ *)(ttp_nbexep->pg_rec + 73))
#define ttp_nbexep_TITR	(ttp_nbexep->pg_rec + 114)
#define ttp_nbexep_vTITR	((_T_I41_ *)(ttp_nbexep->pg_rec + 114))
#define ttp_nbexep_FOOTL	(ttp_nbexep->pg_rec + 155)
#define ttp_nbexep_vFOOTL	((_T_I41_ *)(ttp_nbexep->pg_rec + 155))
#define ttp_nbexep_FOOTC	(ttp_nbexep->pg_rec + 196)
#define ttp_nbexep_vFOOTC	((_T_I41_ *)(ttp_nbexep->pg_rec + 196))
#define ttp_nbexep_FOOTR	(ttp_nbexep->pg_rec + 237)
extern PAGE *ttp_key;
#define ttp_key_NAME	(ttp_key->pg_rec + 0)
extern MENU *ttm_key;
extern MENU *ttm_insert;
extern MENU *ttm_decorate;
extern MENU *ttm_margins;
extern MENU *ttm_vars;
extern MENU *ttm_imps;
extern PAGE *tt_grep;
#define tt_grep_TEXT	(tt_grep->pg_rec + 0)
#define tt_grep_FILES	(tt_grep->pg_rec + 51)
#define tt_grep_CASE	(*(short *)(tt_grep->pg_rec + 102))
#define tt_grep_FOUND1	(tt_grep->pg_rec + 104)
typedef char _T_I68_[68];
#define tt_grep_vFOUND1	((_T_I68_ *)(tt_grep->pg_rec + 104))
#define tt_grep_FOUND2	(tt_grep->pg_rec + 172)
#define tt_grep_vFOUND2	((_T_I68_ *)(tt_grep->pg_rec + 172))
#define tt_grep_FOUND3	(tt_grep->pg_rec + 240)
extern PAGE *tt_titre;
	extern OBJ tt_titre_sn;
	extern OBJ tt_titre_user;
#define tt_titre_SN	(tt_titre->pg_rec + 0)
#define tt_titre_USER	(tt_titre->pg_rec + 13)
extern PAGE *tt_pcomm;
#define tt_pcomm_FILENAME	(tt_pcomm->pg_rec + 0)
#define tt_pcomm_CHAR	(tt_pcomm->pg_rec + 32)
#define tt_pcomm_C_LINE	(tt_pcomm->pg_rec + 36)
#define tt_pcomm_C_COL	(tt_pcomm->pg_rec + 42)
#define tt_pcomm_T_LINE	(tt_pcomm->pg_rec + 47)
#define tt_pcomm_MODIF	(tt_pcomm->pg_rec + 53)
#define tt_pcomm_CORE	(tt_pcomm->pg_rec + 55)
#define tt_pcomm_NB_FILE	(tt_pcomm->pg_rec + 62)
#define tt_pcomm_MODE	(tt_pcomm->pg_rec + 65)
#define tt_pcomm_MARGIN	(*(long *)(tt_pcomm->pg_rec + 72))
#define tt_pcomm_TYPE	(tt_pcomm->pg_rec + 76)
extern PAGE *tt_vline;
extern APPL *tt_amain;
extern PAGE *tt_dir;
#define tt_dir_CDIR	(tt_dir->pg_rec + 0)
extern MENU *ttm_box;

#endif /* _SCR_OBJS_H_ */
