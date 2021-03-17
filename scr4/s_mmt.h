#ifndef _MMT_
#define _MMT_
#define SWAP

#include <scr4_str.h>
#include <s_dir.h>

/*#define UCHAR unsigned char /* JMP 14-10-96 */

typedef struct flist {
    U_ch    file[SCR_FILE_LG + 1];
    int     line;
} FLIST;
typedef struct _staff_ {
    MMT     *file;
    int     line;
} STAFF;

extern MMT    *MMT_CURRENT;
extern MMTFN  MMT_FIELD_FNS[];
extern MMTFN  MMT_FILE_FNS[];
extern MMT    **MMT_FILES;
extern int    MMT_NB_FILES;
extern int    ED_amarg, ED_rmarg;
extern U_ch   MMT_FIND[];
extern U_ch   MMT_REPLF[];
extern U_ch   MMT_REPLT[];
extern int    MMT_GOTO;
extern int    MMT_MARG;
extern FLIST  MMT_FLIST[];
extern U_ch   MMT_LFILE[];
extern U_ch   MMT_HYPT[];
extern U_ch   MMT_FILL[];
extern STAFF  ED_staff;
extern U_ch   *MMT_HLPF10;

#define ED_CSR      (MMT_CURRENT->csr    )
#define ED_MARK     (MMT_CURRENT->mark   )
#define ED_MARKT    (MMT_CURRENT->markt  )
#define ED_CSG      (MMT_CURRENT->csg    )
#define ED_SIZE     (MMT_CURRENT->size   )
#define ED_OSIZE    (MMT_CURRENT->osize  )
#define ED_POS      (MMT_CURRENT->pos    )
#define ED_OPOS     (MMT_CURRENT->opos   )
#define ED_LIM      (MMT_CURRENT->lim    )
#define ED_ATTR     (MMT_CURRENT->attr   )
#define ED_CP       (MMT_CURRENT->cp     )
#define ED_RATTR    (MMT_CURRENT->rattr  )
#define ED_STATBAR  (MMT_CURRENT->statbar)
#define ED_BOX      (MMT_CURRENT->box    )
#define ED_MOD      (MMT_CURRENT->mod    )
#define ED_ISSBAR   (MMT_CURRENT->issbar )
#define ED_OUTPUT   (MMT_CURRENT->output )
#define ED_TXT      (MMT_CURRENT->lst    )
#define ED_IM       (MMT_CURRENT->im     )
#define ED_TMPIM    (MMT_CURRENT->tmpim  )
#define ED_SBAR     (MMT_CURRENT->sbar   )
#define ED_TITLE    (MMT_CURRENT->title  )
#define ED_MMTFNS   (MMT_CURRENT->mmtfns )
#define ED_ZOOM     (MMT_CURRENT->zoom   )
#define ED_FNKEYS   (MMT_CURRENT->fnkeys )
#define ED_CHNG     (MMT_CURRENT->chng   )
#define ED_FOCUS    (MMT_CURRENT->focus  )

#define MMT_MAX_FILES 100

extern unsigned char *ED_get_line();
extern unsigned char *ED_realloc();

/* ============ BASIC EDITING FUNCTIONS OF MMT ======================= */

extern int  ED_s_left(); /* JMP 11-10-98 */
extern int  ED_s_right(); /* JMP 11-10-98 */
extern int  ED_s_up(); /* JMP 11-10-98 */
extern int  ED_s_down(); /* JMP 11-10-98 */
extern int  ED_s_home(); /* JMP 11-10-98 */
extern int  ED_s_end(); /* JMP 11-10-98 */
extern int  ED_s_pgup(); /* JMP 11-10-98 */
extern int  ED_s_pgdn(); /* JMP 11-10-98 */
extern int  ED_clippaste(); /* JMP 11-10-98 */
extern int  ED_clipcopy(); /* JMP 11-10-98 */

extern int  ED_command();
extern int  ED_left();
extern int  ED_right();
extern int  ED_up();
extern int  ED_down();
extern int  ED_home();
extern int  ED_end();
extern int  ED_pgup();
extern int  ED_pgdn();
extern int  ED_c_home();
extern int  ED_c_end();
extern int  ED_tab();
extern int  ED_mode_insert();
extern int  ED_delete_ch();
extern int  ED_delete_ch();
extern int  ED_backspace();
extern int  ED_enter();
extern int  ED_mark_block();
extern int  ED_copy();
extern int  ED_delete();
extern int  ED_del_eol();
extern int  ED_del_bol();
extern int  ED_help();
extern int  ED_insert();
extern int  ED_join();
extern int  ED_kill();
extern int  ED_mark_line();
extern int  ED_move();
extern int  ED_new();
extern int  ED_overlay();
extern int  ED_quit();
extern int  ED_mark_text();
extern int  ED_unmark();
extern int  ED_zoom();
extern int  ED_write_file();
extern int  ED_buf_set();
extern int  ED_buf_paste();
extern int  ED_show_dos();
extern int  ED_cut();
extern int  ED_paste();
extern int  ED_show();
extern int  ED_fill();
extern int  ED_move();
extern int  ED_find();
extern int  ED_goto();
extern int  ED_repl();
extern int  MMT_cascade();
extern int  MMT_fullsize();
extern int  MMT_litlebox();
extern int  ED_underl();
extern int  ED_box();
extern int  ED_shift_left();
extern int  ED_shift_right();
extern int  ED_menu_box();
extern int  ED_about();
extern int  ED_left_del();
extern int  ED_menu_bloc();
extern int  ED_menu_misc();
extern int  ED_menu_curs();
extern int  ED_menu_txt();
extern int  ED_menu_win();
extern int  ED_copy_paste();
extern int  ED_move_box();
extern int  ED_resize_box();
extern int  ED_reflow();
extern int  ED_scr_help();
extern int  ED_prev_word();
extern int  ED_next_word();
extern int  ED_get_rmarg();
extern int  ED_upper();
extern int  ED_lower();
extern int  ED_act_marg();
extern int  ED_shell();
extern int  ED_savequit();
extern int  ED_saveall();
extern int  ED_chngfname();
extern int  ED_chngdir();
extern int  ED_printmmt();
extern int  ED_grep();
extern int  ED_switch2();
extern int  SCR_refresh();
extern int  ED_quick_help();
extern int  ED_menu_right();
extern int  ED_hypertext();
extern int  ED_set_staff();
extern int  ED_go_staff();

#endif
