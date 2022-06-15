#ifndef _IODEVARG_
#define _IODEVARG_

#ifdef SCRCPP
extern "C" {
#endif

/* k_errorv.c */
//extern void kerror(int level, char* msg, ...);
//extern void kmsg(char *msg, ...);


/* e_errorv.c */
//extern void E_msg(char *,...);
//extern void E_error(char *,...);

/* b_basev.c */
//extern void B_seterror(char *,...);
//extern void B_seterrn(int ,...);


	
/* o_miscv.c */
extern int ODE_error(int ,...);

#ifdef SCRCPP
}
#endif

#endif
