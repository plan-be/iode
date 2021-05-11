#ifndef _IODEVARG_
#define _IODEVARG_

#ifdef SCRCPP
extern "C" {
#endif

/* k_errorv.c */
extern void kerror(int ,...);
extern void kmsg(char *, ...);

/* l_debug.c */
extern void L_debug(char *,...);

/* e_errorv.c */
extern void E_msg(char *,...);
extern void E_error(char *,...);

/* b_basev.c */
extern void B_seterror(char *,...);
extern void B_seterrn(int ,...);


/* w_wrtv.c  */
extern int W_printf(char *c1,...);

/* objsv.c */
extern int K_add(KDB *kdb, ...);
	
/* o_miscv.c */
extern int ODE_error(int ,...);

#ifdef SCRCPP
}
#endif

#endif
