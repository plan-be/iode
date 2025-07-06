#pragma once

#include "scr4/s_prost.h"       // SCR_free, SCR_stracpy, SCR_strip
#include "scr4/s_a2m.h"

#include <stdarg.h>         // for va_list
#include "api/constants.h"

#ifdef __cplusplus
extern "C" {
#endif

extern double   A2M_GWIDTH;
extern double   A2M_GHEIGHT;
extern int      A2M_BOXWIDTH;
extern int      A2M_BACKBRUSH;
extern int      A2M_BACKCOLOR;

/* w_wrt.c (MSC) */
extern int W_dest(char *filename,int type);
// extern int W_InitParms();
// extern int W_open();
extern int W_close();
extern int W_flush();
extern int W_putc(int ch);
//extern int W_isempty(char *buf);
extern int W_record(char *str);
extern int W_InitDisplay();
extern int W_EndDisplay(char *title,int x,int y,int w,int h);
// extern int W_SavePrinterSettings();
// extern int W_ResetPrinterSettings();
// extern int W_SetPrinterSettings();
extern int W_printf(char *fmt,...);
extern int W_printfDbl(char* fmt, ...);
extern int W_printfRepl(char* fmt, ...);
extern int W_printfReplEsc(char* fmt, ...);
extern int W_printfEx(int dup, int ch1, int ch2, char *fmt, va_list args);

/* w_wrt1.c */
extern void W_print_enum(int );
extern void W_print_cmd(int );
extern void W_print_par(int );
extern void W_print_tit(int );
extern void W_print_pg_header(char* arg);
extern void W_print_pg_footer(char* arg);
extern void W_print_rtf_topic(char* arg);
extern void W_print_tb(char* title, int nc);

#ifdef __cplusplus
}
#endif
