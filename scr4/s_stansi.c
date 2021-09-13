
#include "s_strs.h"

unsigned char SCR_ANSI_TBL[] = {

    /* 80 =128 */
    /* C c */    0xC7,           // 128
    /* u " */    0xFC,   // Å
    /* e ' */    0xE9,           // 130
    /* a ^ */    0xE2,   // É
    /* a " */    0xE4,   // Ñ
    /* a \ */    0xE0,   // 5
    /* a o */    0xE5,
    /* c c */    0xE7,   //
    /* e ^ */    0xEa,
    /* e " */    0xEb,
    /* e \ */    0xE8,
    /* i " */    0xEf,  // ã
    /* i ^ */    0xEe,  // å     // 140
    /* i \ */    0xEc,  //
    /* A " */    0xc4,  //
    /* A o */    0xc5,

    /* 90 =144 */
    /* E ' */    0xc9,  //
    /* a e */    0xe6,
    /* A E */    0xC6,
    /* o ^ */    0xf4,  // ì
    /* o " */    0xf6,  // î
    /* o \ */    0xf2,  // ï
    /* u ^ */    0xfb,           // 150
    /* u \ */    0xf9,  // ó
    /* y " */    0xff,  //
    /* O " */    0xd6,  // ô
    /* U " */    0xdc,  // ö
    /* c | */    0xa2,
    /* pnd */    0xa3,  // ú
    /* Y = */    0xa5,
    /* P t */    0xa0, /**/
    /* f - */    0xa4, /**/

    /* A0 =160 */
    /* a ' */    0xe1,           // †           // 160
    /* i ' */    0xed,           // °
    /* o ' */    0xf3,           // ¢
    /* u ' */    0xfa,           // £
    /* n ~ */    0xf1,
    /* N ~ */    0xd1,
    /* a _ */    0xaa,
    /* o _ */    0xba,
    /* ? i */    0xbf,
    /* not */    0xa6, /**/
    /* nad */    0xac,                          // 170
    /* 1/2 */    0xbd,
    /* 1/4 */    0xbc,
    /* ! i */    0xa1,
    /* <<  */    0xab,
    /* >>  */    0xbb,

    /* B0 =176 */
    /* gr  */    0xa7, /**/
    /* gr  */    0xa8, /**/
    /* gr  */    0xa9, /**/
    /* gr  */    0xad, /**/
    /* gr  */    0xc0, /**/   // µ              // 180
    /* gr  */    0xc1, /**/
    /* gr  */    0xc2, /**/   //
    /* gr  */    0xc0, /**/   // ∑
    /* gr  */    0xb8, /**/
    /* gr  */    0xb9, /**/
    /* gr  */    0xbe, /**/
    /* gr  */    0xc0, /**/
    /* gr  */    0xc1, /**/
    /* gr  */    0xc2, /**/
    /* gr  */    0xc3, /**/                     // 190
    /* gr  */    0xc8, /**/


    /* C0 =192 */
    /* gr  */    0xcb, /* coin inf gauche (voir http://pythonfacile.free.fr/python/codepages.html) */
    /* gr  */    181 , /* hor bas c*/           // A aigu // avant 0xcc
    /* gr  */    0xcd, /* hor haut centre */
    /* gr  */    0xce, /* vert ga centre */
    /* gr  */    0x97, /* hor */ /* JMP 30-09-10 */
    /* gr  */    0xcf, /* ??  */ /* JMP 30-09-10 */
    /* gr  */    0xd2, /**/
    /* gr  */    0xd3, /**/
    /* gr  */    0xd4, /**/
    /* gr  */    0xd5, /**/
    /* gr  */    0xd7, /**/
    /* gr  */    0xd8, /**/
    /* gr  */    0xd9, /**/
    /* gr  */    0xAF, /* ligne hor sup */     /* JMP 30-09-10 */
    /* gr  */    0xdb, /**/
    /* gr  */    0xdd, /**/

    /* D0 =208 */
    /* gr  */    0xde, /**/
    /* gr  */    0xe3, /**/
    /* gr  */    0xCA, /**/  // “
    /* gr  */    0xCB, /**/  // ”
    /* gr  */    0xC8, /**/  // ‘
    /* gr  */    0xf7, /**/
    /* gr  */    0xCD, /**/  // ÷
    /* gr  */    0xCE, /**/  // ◊
    /* gr  */    0xCF, /**/  // ÿ
    /* gr  */    0x80, /**/
    /* gr  */    0x81, /**/
    /* gr  */    0x82, /**/
    /* gr  */    0x83, /**/
    /* gr  */    0x84, /**/
    /* gr  */    0xCC, /**/  // ﬁ
    /* U Ô */    0xDA, /**/  // È


    /* E0 =224 */
    /* O Ô   */  0xd3, // ‡
    /* beta  */  0xdf,
    /* GAMMA */  0xd4, /**/     // ‚
    /* PI    */  0xd2, // „
    /* SIGMA */  0x89, /**/
    /* sigma */  0x8a, /**/
    /* mu    */  0xb5,          // Ê
    /* gamma */  0x8b, /**/
    /* PHI   */  0x8c, /**/
    /* theta */  0xDA, /**/     // È
    /* U ^   */  0xDB, /**/     // Í
    /* delta */  0xD9, /**/     // Î
    /* y Ô   */  0xFD, /**/     // Ï
    /* Y Ô   */  0xdd, /**/     // Ì
    /* eps   */  0x92, /**/
    /* eta   */  0x93, /**/

    /* F0 =240 */
    /* ===   */  0x94, /**/
    /* +-    */  0xb1,
    /* >=    */  0x95, /**/
    /* <=    */  0x96, /**/
    /* accol1*/  0x97, /**/
    /* accol2*/  0xa7, /**/        //
    /* divis.*/  0x99, /**/
    /* +-=   */  0x9a, /**/
    /* degree*/  0xb0,
    /* . cent*/  0x9b, /**/
    /* . cent*/  0xb7,
    /* sqrt  */  0x9c, /**/
    /* n exp */  0xb3, /**/     // ¸
    /* 2 exp */  0xb2,          // ˝
    /* pave  */  0x9d, /**/
    /*       */  0x9e  /**/

};

/* ================================================================
Transforme un caractäre ASCII-PC en caractäre ANSI (Latin 1). Seuls les
caractäres supÇrieurs 5 127 sont modifiÇs.

Une bijection est dÇfinie entre les deux ensembles de caractäres (ces
ensembles sont largement distincts) de telle sorte que les fonctions
SCR_OemToAnsiChar() et SCR_AnsiToOemChar() sont rÇciproques.

&RT le caractäre traduit
&SA SCR_AnsiToOemChar(), SCR_OemToAnsi(), SCR_AnsiToOem()

=================================================================== */

int SCR_OemToAnsiChar(ch)
int     ch;
{
    if(ch < 128) return(ch);
    return(SCR_ANSI_TBL[ch - 128]);
}

/* ================================================================
Transforme une chaåne de caractäres ASCII-PC en ANSI (Latin-1). Seuls
les caractäres supÇrieurs 5 127 sont modifiÇs.

Une bijection est dÇfinie entre les deux ensembles de caractäres (ces
ensembles sont largement distincts) de telle sorte que les fonctions
SCR_OemToAnsi() et SCR_AnsiToOem() sont rÇciproques.

Les deux paramätres (ansi et oem) peuvent
une transformation "sur place".

Les pointeurs peuvent

&RT un pointeur vers la chaåne ansi rÇsultat
&SA SCR_OemToAnsiChar(), SCR_AnsiToOemChar(), SCR_AnsiToOem()

=================================================================== */

unsigned char *SCR_OemToAnsi(ansi, oem)
unsigned char *ansi, *oem;
{
    int     i;

    if(ansi == 0 || oem == 0) return(ansi);
    for(i = 0 ; oem[i] ; i++)
	ansi[i] = SCR_OemToAnsiChar(oem[i]);

    ansi[i] = 0;
    return(ansi);
}

/* ================================================================
Transforme une chaåne de caractäres ASCII-PC en ANSI (Latin-1). Seuls
les caractäres supÇrieurs 5 127 sont modifiÇs.

Une bijection est dÇfinie entre les deux ensembles de caractäres (ces
ensembles sont largement distincts) de telle sorte que les fonctions
SCR_OemToAnsi() et SCR_AnsiToOem() sont rÇciproques.

&EN unsigned char *ansi : chaåne rÇsultat
&EN unsigned char *oem : chaåne input
&EN int lg : longueur de la chaåne 5 transformer

Les deux paramätres (ansi et oem) peuvent
une transformation "sur place".

Les pointeurs peuvent

&RT un pointeur vers la chaåne ansi rÇsultat
&SA SCR_OemToAnsiChar(), SCR_AnsiToOemChar(), SCR_AnsiToOem()

=================================================================== */

unsigned char *SCR_OemToAnsiLg(ansi, oem, lg)
unsigned char   *ansi, *oem;
int             lg;
{
    int     i;

    if(ansi == 0 || oem == 0) return(ansi);
    for(i = 0 ; i < lg ; i++)
	ansi[i] = SCR_OemToAnsiChar(oem[i]);

    return(ansi);
}

/* ================================================================
Transforme un caractäre ANSI (Latin-1) en caractäre ASCII PC. Seuls les
caractäres supÇrieurs 5 127 sont modifiÇs.

Une bijection est dÇfinie entre les deux ensembles de caractäres (ces
ensembles sont largement distincts) de telle sorte que les fonctions
SCR_OemToAnsiChar() et SCR_AnsiToOemChar() sont rÇciproques.

&RT le caractäre traduit
&PROTO #include <s_strs.h>
&SA SCR_OemToAnsiChar(), SCR_OemToAnsi(), SCR_AnsiToOem()

=================================================================== */

int SCR_AnsiToOemChar(ch)
int     ch;
{
    static unsigned char    *oem_tbl = 0;
    int                     i;

    if(ch < 128) return(ch);
    if(oem_tbl == 0) {
	oem_tbl = malloc(128);
	if(oem_tbl == 0) return(ch);
	for(i = 0 ; i < 128 ; i++) {
	    oem_tbl[SCR_ANSI_TBL[i] - 128] = i + 128;
	    }
	}
    return(oem_tbl[ch - 128]);
}

/* ================================================================
Transforme une chaåne de caractäres ANSI (Latin-1) en ASCII-PC. Seuls
les caractäres supÇrieurs 5 127 sont modifiÇs.

Une bijection est dÇfinie entre les deux ensembles de caractäres (ces
ensembles sont largement distincts) de telle sorte que les fonctions
SCR_OemToAnsi() et SCR_AnsiToOem() sont rÇciproques.

Les deux paramätres (ansi et oem) peuvent
une transformation "sur place".

Les pointeurs peuvent

&RT un pointeur vers la chaåne oem rÇsultat
&PROTO #include <s_strs.h>
&SA SCR_OemToAnsiChar(), SCR_AnsiToOemChar(), SCR_AnsiToOem()

=================================================================== */

unsigned char *SCR_AnsiToOem(oem, ansi)
unsigned char *ansi, *oem;
{
    int     i;

    if(ansi == 0 || oem == 0) return(oem);
    for(i = 0 ; oem[i] ; i++)
	oem[i] = SCR_AnsiToOemChar(ansi[i]);

    oem[i] = 0;
    return(oem);
}

/* ================================================================
Transforme une chaåne de caractäres ANSI (Latin-1) en ASCII-PC. Seuls
les caractäres supÇrieurs 5 127 sont modifiÇs.

Une bijection est dÇfinie entre les deux ensembles de caractäres (ces
ensembles sont largement distincts) de telle sorte que les fonctions
SCR_OemToAnsi() et SCR_AnsiToOem() sont rÇciproques.

&EN unsigned char *oem : chaåne rÇsultat
&EN unsigned char *ansi : chaåne input
&EN int lg : longueur de la chaåne 5 transformer

Les deux paramätres (ansi et oem) peuvent
une transformation "sur place".

Les pointeurs peuvent

&RT un pointeur vers la chaåne oem rÇsultat
&PROTO #include <s_strs.h>
&SA SCR_OemToAnsiChar(), SCR_AnsiToOemChar(), SCR_AnsiToOem()

=================================================================== */

unsigned char *SCR_AnsiToOemLg(oem, ansi, lg)
unsigned char   *ansi, *oem;
int             lg;
{
    int     i;

    if(ansi == 0 || oem == 0) return(oem);
    for(i = 0 ; i < lg ; i++)
	oem[i] = SCR_AnsiToOemChar(ansi[i]);

    return(oem);
}

int SCR_OemToTextChar(int c)
{
    if(c >= 176) {
	if(U_is_in(c, "ƒ"))                 return('-');
	else if(U_is_in(c, "Õ"))            return('=');
	else if(U_is_in(c, "⁄¬ø¿¡Ÿ…ª» Àº")) return('+');
	else if(U_is_in(c, "≥∫"))           return('|');
	else return(' ');
	}
    else {
	if(c >= ' ') return(SCR_OemToAnsiChar(c));
	else return(' ');
	}
}

unsigned char *SCR_OemToTextLg(U_ch *res, U_ch *oem, int lg)
{
    int     i;

    if(res == 0 || oem == 0) return(res);
    for(i = 0 ; i < lg ; i++)
	res[i] = SCR_OemToTextChar(oem[i]);

    return(res);
}

unsigned char *SCR_OemToText(unsigned char *res, unsigned char *oem)
{
    if(res == 0) return(res);
    return(SCR_OemToTextLg(res, oem, strlen(res)));
}









