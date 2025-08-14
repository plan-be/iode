#pragma once

#include "scr4/s_prost.h"       // SCR_free, SCR_stracpy, SCR_strip
#include "scr4/s_a2m.h"

#include <stdarg.h>             // for va_list
#include "api/constants.h"


inline double   A2M_GWIDTH = 9.5;     // Default graph width in cm
inline double   A2M_GHEIGHT = 6.5;    // Default graph height in cm
inline int      A2M_BOXWIDTH = 1;     // Width of the box around the graphs (0 for no box)
inline int      A2M_BACKBRUSH = 50;   // Intensity of the graph background brushes
inline int      A2M_BACKCOLOR = 'b';  // Color of the graph background: one of "Bwrgbcmy"

/* w_wrt.c (MSC) */
int W_dest(char *filename,int type);
// int W_InitParms();
// int W_open();
int W_close();
int W_flush();
int W_putc(int ch);
//int W_isempty(char *buf);
int W_record(char *str);
int W_InitDisplay();
int W_EndDisplay(char *title,int x,int y,int w,int h);
// int W_SavePrinterSettings();
// int W_ResetPrinterSettings();
// int W_SetPrinterSettings();
int W_printf(char *fmt,...);
int W_printfDbl(char* fmt, ...);
int W_printfRepl(char* fmt, ...);
int W_printfReplEsc(char* fmt, ...);
int W_printfEx(int dup, int ch1, int ch2, char *fmt, va_list args);

/* w_wrt1.c */
void W_print_enum(int );
void W_print_cmd(int );
void W_print_par(int );
void W_print_tit(int );
void W_print_pg_header(char* arg);
void W_print_pg_footer(char* arg);
void W_print_rtf_topic(char* arg);
void W_print_tb(char* title, int nc);
