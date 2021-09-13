#ifndef _TCWC_
#define _TCWC_

#ifdef DOS
#ifdef WATCOM

#define SCR_MONO_ADR  (char *)(0xB0000)
#define SCR_COLOR_ADR (char *)(0xB8000)
#define SCR_COM_ADR   (char *)(0x400)

#define int86(a, b, c)  int386(a, b, c)

#else

#define SCR_MONO_ADR  (char *)(0xB0000000L)
#define SCR_COLOR_ADR (char *)(0xB8000000L)
#define SCR_COM_ADR   (char *)(0x00000400L)
/*#define inp(a)  inportb(a)
#define outp(a, b) outportb(a, b) */

#endif
#endif
#endif
