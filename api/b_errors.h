#pragma once

#ifdef __cplusplus
extern "C" {
#endif

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