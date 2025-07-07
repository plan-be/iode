/**
 *  @header4iode
 * 
 *  Basic functions to create a WKS (Lotus 123) file (obsolete format).
 *  
 *  List of functions 
 *  -----------------
 *      int wks_init(char* file, int c, int r)
 *      int wks_cwidth(int c, int w)
 *      int wks_end()
 *      void wks_string(char* label, int c, int r)
 *      void wks_value(double value, int c, int r)
 *      void wks_name(char* str, int c1, int r1, int c2, int r2)
 *  
 */
#include "scr4/s_xdr.h"

#include "api/conversion/import.h"
#include "api/conversion/export.h"


FILE*            WKS_FD;
unsigned char    WKS_BOF[6]     = {0, 0, 2, 0, 4, 4};
unsigned char    WKS_CW[4]      = {8, 0, 3, 0};
unsigned char    WKS_NAN[8]     = {0, 0, 0, 0, 0, 0, 240, 255};
unsigned char    WKS_RANGE[8]   = {6, 0, 8, 0, 0, 0, 0, 0};
unsigned char    WKS_LABEL[5]   = {15, 0, 0, 0, 245};
unsigned char    WKS_NM[4]      = {11, 0, 24, 0};
unsigned char    WKS_DOUBLE[5]  = {14, 0, 13, 0, 241};
unsigned char    WKS_EOF[4]     = {1,0,0,0};


/*

main(argc, argv)
int     argc;
char    *argv[];
{
    double  a = 1, b = 2.034;

    wks_init("brol.wks", 1, 4);
    wks_cwidth(1, 9);
    wks_cwidth(2, 9);

    wks_value(a, 1, 1);
    wks_value(b, 2, 1);
    wks_string("test", 3, 1);
    wks_end();
}
*/

int wks_init(char* file, int c, int r)
{
#ifndef __GNUC__
    short     i = c - 1, j = r - 1;

    WKS_FD = fopen(file, "wb+");
    if(WKS_FD == NULL) return(-1);

    fwrite(WKS_BOF, sizeof(WKS_BOF), 1, WKS_FD);
    if(c != 0 && r != 0) {
        fwrite(WKS_RANGE, sizeof(WKS_RANGE), 1, WKS_FD);
        XDR_wshort(WKS_FD, &i, 1);
        XDR_wshort(WKS_FD, &j, 1);
    }

    return(0);
#else
    return(-1);
#endif
}

int wks_cwidth(int c, int w)
{
#ifndef __GNUC__
    short     i = c - 1;
    char    width = w;

    fwrite(WKS_CW, sizeof(WKS_CW), 1, WKS_FD);
    XDR_wshort(WKS_FD, &i, 1);
    fwrite(&width, sizeof(char), 1, WKS_FD);

    return(0);
#else
    return(-1);
#endif
}

int wks_end()
{
#ifndef __GNUC__
    fwrite(WKS_EOF, sizeof(WKS_EOF), 1, WKS_FD);
    fclose(WKS_FD);
    return(0);
#else
    return(-1);
#endif
}

void wks_string(char* label, int c, int r)
{
#ifndef __GNUC__
    short     i = c - 1, j = r - 1,
              lg;

    lg = (short)strlen(label);
    if(lg + 5 > 240) label[234] = 0;
    lg = (short)strlen(label) + 7;
    XDR_l2s_s(&lg, 1);
    memcpy(WKS_LABEL + 2, (char *) &lg, 2);
    fwrite(WKS_LABEL, sizeof(WKS_LABEL), 1, WKS_FD);

    XDR_wshort(WKS_FD, &i, 1);
    XDR_wshort(WKS_FD, &j, 1);
    fwrite("'", 1, 1, WKS_FD);
    fwrite(label, strlen(label) + 1, 1, WKS_FD);
#endif
}

void wks_value(double value, int c, int r)
{
#ifndef __GNUC__
    short     i = c - 1, j = r - 1;

    fwrite(WKS_DOUBLE, sizeof(WKS_DOUBLE), 1, WKS_FD);
    XDR_wshort(WKS_FD, &i, 1);
    XDR_wshort(WKS_FD, &j, 1);
    if(value <= -1.0e37)
        fwrite(WKS_NAN, sizeof(WKS_NAN), 1, WKS_FD);
    else
        XDR_wdouble(WKS_FD, &value, 1);
#endif
}

void wks_name(char* str, int c1, int r1, int c2, int r2)
{
#ifndef __GNUC__
    int     i;
    char    name[20];                           /* JMP 13-02-2013 */
    short   i1 = c1 - 1, j1 = r1 - 1,
            i2 = c2 - 1, j2 = r2 - 1;

    fwrite(WKS_NM, sizeof(WKS_NM), 1, WKS_FD);
    SCR_strlcpy((unsigned char*) name, (unsigned char*) str, 17);                 /* JMP 13-02-2013 */
    name[15] = 0;
    for(i = (int)strlen(name); i < 16; i++) name[i] = 0;
    fwrite(name, 16, 1, WKS_FD);
    XDR_wshort(WKS_FD, &i1, 1);
    XDR_wshort(WKS_FD, &j1, 1);
    XDR_wshort(WKS_FD, &i2, 1);
    XDR_wshort(WKS_FD, &j2, 1);
#endif
}
