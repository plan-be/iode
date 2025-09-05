/* =+======== SCR.H =================================================== */

#ifndef _SCR4_H_
#ifndef _SCR4_D_H_
#define _SCR4_D_H_

// WARNING: using min and max as macro names makes conflicts with the GNU implementation 
//          of the C++ standard library
#ifndef _max_
	#define _max_(x,y)        ((x)<(y)?(y):(x))
#endif
#ifndef _min_
	#define _min_(x,y)        ((x)<(y)?(x):(y))
#endif

#define SCR_NULL        ((char *)0)
#define SCR_NOT_AVAIL   -2.0e+37

#include <stdio.h>
//#include <ctype.h>
#include <s_yy.h>
double  atof();


#define SCR_REVERSE     1  /* LIGHT GREY BLACK */
#define SCR_UNDERLINE   2  /* LIGHT GREY BLUE  */
#define SCR_BOLD        3  /* BLACK RED */
#define SCR_BLINKING    4  /* BLACK LIGHT RED */
#define SCR_INVISIBLE   5  /* BLACK BLACK */
#define SCR_DIM         6  /* BLACK RED  */
#define SCR_CONDENSED   7  /* BLACK RED  */
#define SCR_ENLARGED    8  /* BLACK RED  */
#define SCR_DEFAULT     9  /* BLACK CYAN */

/* =+============= COLORS ======================================== */

#define SCR_BLACK       20  /* BLACK LIGHT GREY */
#define SCR_BLUE        21  /* BLUE  WHITE      */
#define SCR_GREEN       22  /* GREEN LIGHT GREY */
#define SCR_CYAN        23  /* CYAN  YELLOW     */
#define SCR_RED         24  /* RED   LIGHT GREY */
#define SCR_MAGENTA     25  /* MAGENTA white */
#define SCR_WHITE       26  /* LIGHT GREY , RED */
#define SCR_YELLOW      27  /* BROWN YELLOW */

/* =+============= IO ============================================ */

#define SCR_INPUT       7001
#define SCR_OUTPUT      7002
#define SCR_HIDDEN      7003
#define SCR_ACCESSIBLE  7004

#define SCR_WRITE           6071
#define SCR_READ            6072
#define SCR_UPDATE          6073

#define SCR_LOCK            6074
#define SCR_NO_LOCK         6075

#define SCR_EQ              6076
#define SCR_LE              6077
#define SCR_GE              6078
#define SCR_GT              6079
#define SCR_LT              6080
#define SCR_FIRST           6081
#define SCR_LAST            6082
#define SCR_CIRC            6083
#define SCR_DB              6084

#include <scr4_str.h>

/* ==== FUNCTIONS TYPE ================================= */

extern char             *U_control_to_string(/* unsigned char */) ;
extern struct image     *PG_save(/* struct page **/) ;
extern char             SCR_graphic_char(/* int ,int ,int ,int */) ;
extern MENU             *MN_create();
extern struct option    *MN_add_option(/* struct menu **/) ;
extern long             SCR_current_date(/* void*/) ;
extern long             SCR_date_to_long(/* char **/) ;
extern long             SCR_current_auto_num(/* char **/) ;
extern long             SCR_auto_num(/* char **/) ;
extern char             *SCR_find_filename(/* void **/) ;
extern struct status    *SCR_save_status(/* void*/) ;
extern long             SCR_free_mem();
extern char             *IS_error_text(/* void*/) ;
extern struct fileinfo  *DIR_read(/*char **/);
extern char             **SCR_read_file(/*char **/);
extern char             *SCR_strip(/* char * */);
extern long             PG_get_long();
extern double           PG_get_double();
extern char             *SCR_get_text();
extern short            SCR_get_short();
extern long             SCR_get_long();
extern float            SCR_get_float();
extern double           SCR_get_double();
extern char             *SCR_rget_text();
extern short            SCR_rget_short();
extern long             SCR_rget_long();
extern float            SCR_rget_float();
extern double           SCR_rget_double();
extern char             *SCR_get_vtext();
extern long             SCR_get_vlong();
extern short            SCR_get_vshort();
extern double           SCR_get_vdouble();
extern float            SCR_get_vfloat();
extern char             *SCR_rget_vtext();
extern long             SCR_rget_vlong();
extern short            SCR_rget_vshort();
extern double           SCR_rget_vdouble();
extern float            SCR_rget_vfloat();
extern char             **SCR_get_editor();
extern FIELD            *PG_add_field();
extern long             SCR_ufmt_long();
extern unsigned char    *SCR_stracpy();
extern char             *PG_get_text();
extern char             *_SCR_find_buf();
extern char             *PG_get_rec_txt();
extern char             *PG_aget_rec_txt();

extern long             SCR_current_time();
extern long             SCR_ftime_to_long();
extern long             SCR_fdate_to_long();
extern long             SCR_get_field_date();
extern long             SCR_get_field_time();

extern long             SC_scan_page();
extern long             SC_scan_isam();
extern unsigned char    *PG_text(),
			*PG_vtext();

extern MENU             *MN_auto_create();
extern char             *IS_text();
extern char             *SCR_search_path();
extern MENU             *MN_create_default();
extern char             *SCR_err_txt();
extern char             *(*SCR_err_txt_super)(int val);

#include <scr4_rd.h>

/* ==== EXTERN VARS DECLARATIONS ================================= */

extern char             SCR_TERM[] ;

extern FILE_TABLE       SCR_READ_FILES;
extern int              SCR_BC_OPERATION;
extern int              SCR_PG_OPERATION;
extern char             **ARG_VALUES;
extern int              ARG_COUNT;
extern char             DIR_SELECTED[];
extern FILEINFO         DIR_SEL_FILE;
extern int              SCR_BREAK_ON;
extern int              CNF_EXEC_MENU;
extern int              CNF_NOTEPAD;
extern int              CNF_REVERSE_TITLE;
extern int              SCR_LKEY;
extern int              SCR_NB_DEC;
extern int              SCR_RIGHT_MENU;
extern int              SCR_MAP_EXEC;
extern FILE             *HLP_FD;
extern char             HLP_NAME[SCR_MAX_FILE_LG + 1];
extern HELPS            HLPS;
extern int              IS_DEF_SQZ;
extern int              SCR_CHECK_DELETE;
extern int              SCR_EDIT_KILL;
extern char             SCR_NAME_SCR[SCR_MAX_FILE_LG_MAX + 1];
extern char             SCR_NAME_ERR[SCR_MAX_FILE_LG_MAX + 1];
extern int              (*SCR_FN_KEY)();
extern int              (*SCR_HIT_KEY_FN)(); /* JMP 10-06-94 */
extern int              SCR_CLOCK_POS[];
extern int              SCR_MEM_POS[];
extern char             SCR_LANGUAGE[] ;
extern int              SCR_CONFIRME_ON;
extern long             SCR_END_TIME_1, SCR_END_TIME_2;
extern int              CT_SYNC;
extern int              SCR_PAGE_SIZE[2];
extern PR_DEF           *PR_CURRENT;

#define  SCR_write(a, b, c, d) (SCR_TMP_IMAGE != 0) ? SCR_write_im(a, b, c, d) : \
						      SCR_write_text(a, b, c, d)

#define  MN_get_choice(mn)  (((MENU *)(mn))->mn_choice)
#define  MN_text(mn, i)     (((MENU *)mn)->mn_opts[i].opt_text)

#define  CONFIRME           (SCR_confirme((char *)0) != 0)
#define  YYTEXT             (SCR_YY->yy_text)
#define  YYLONG             (SCR_YY->yy_long)
#define  YYREAD             (SCR_YY->yy_type)

#define SCR_set_attr(attr)  (SCR_CURRENT_ATTR = attr)
#define SCR_PTR(i)          (SCR_READ_FILES.tbl_fl[i]->fl_ptr)
#define SCR_NU(ptr)             (SCR_find_ptr_nu(ptr))

#endif /* _SCR4_D_H_ */
#endif /* _SCR4_H_ */
