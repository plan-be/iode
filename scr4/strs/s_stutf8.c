#include "s_strs.h"

// Ansi position => UTF8 C3 group  
// unsigned char SCR_UTF8_TBL[] = {

    // /* 80 =128 */
    // /* C c */    0x87, Ã
    // /* u " */    0xbc,
    // /* e ' */    0xa9,
    // /* a ^ */    0xa2,
    // /* a " */    0xa4,
    // /* a \ */    0xa0,
    // /* a o */    0xa5,
    // /* c c */    0xa7,
    // /* e ^ */    0xaa,
    // /* e " */    0xab,
    // /* e \ */    0xa8,
    // /* i " */    0xaf,
    // /* i ^ */    0xae,
    // /* i \ */    0xac,
    // /* A " */    0x84,
    // /* A o */    0x85,

    // /* 90 =144 */
    // /* E ' */    0x89,
    // /* a e */    0xa6,
    // /* A E */    0x86,
    // /* o ^ */    0xb4,
    // /* o " */    0xb6,
    // /* o \ */    0xf2,
    // /* u ^ */    0xbb,
    // /* u \ */    0xb9,
    // /* y " */    0xbf,
    // /* O " */    0x96,
    // /* U " */    0x9c,
    // /* c | */    0xa2,  // C2+
    // /* pnd */    0xa3,  // C2+ A3
    // /* Y = */    0xa5,  // C2+
    // /* P t */    0xa0,  // C2+
    // /* f - */    0xa4,  // C2+

    // /* A0 =160 */
    // /* a ' */    0xa1,
    // /* i ' */    0xad,
    // /* o ' */    0xb3,
    // /* u ' */    0xba,
    // /* n ~ */    0xb1,
    // /* N ~ */    0x91,
    // /* a _ */    0, // ??
    // /* ░   */    0,
    // /* ? inv */    0,
    // /* not */    0xac,  // C2
    // /* nad */    0,
    // /* 1/2 */    0xbd,  // C2
    // /* 1/4 */    0xbc,  // C2
    // /* ! inv */    0xa1,  // C2
    // /* <<  */    0xab,  // C2
    // /* >>  */    0xbb,  // C2

    // /* B0 =176 */
    // /* gr  */    0, /**/
    // /* gr  */    0, /**/
    // /* gr  */    0, /**/
    // /* gr  */    0, /**/
    // /* gr  */    0, /**/
    // /* gr  */    0, /**/
    // /* A ^ */    0x82, /**/
    // /* gr  */    0, /**/
    // /* gr  */    0, /**/
    // /* gr  */    0, /**/
    // /* gr  */    0, /**/
    // /* gr  */    0, /**/
    // /* gr  */    0, /**/
    // /* gr  */    0, /**/
    // /* gr  */    0, /**/
    // /* gr  */    0, /**/


    // /* C0 =192 */
    // /* gr  */    0, /* coin inf gauche (voir http://pythonfacile.free.fr/python/codepages.html) */
    // /* gr  */    0, /* hor bas c*/
    // /* gr  */    0, /* hor haut centre */
    // /* gr  */    0, /* vert ga centre */
    // /* gr  */    0, /* hor */ /* JMP 30-09-10 */
    // /* gr  */    0, /* ??  */ /* JMP 30-09-10 */
    // /* gr  */    0, /**/
    // /* gr  */    0, /**/
    // /* gr  */    0, /**/
    // /* gr  */    0, /**/
    // /* gr  */    0, /**/
    // /* gr  */    0, /**/
    // /* gr  */    0, /**/
    // /* gr  */    0, /* ligne hor sup */     /* JMP 30-09-10 */
    // /* gr  */    0, /**/
    // /* gr  */    0, /**/

    // /* D0 =208 */
    // /* gr  */    0, /**/
    // /* gr  */    0, /**/
    // /* E^  */    0x8A, /**/
    // /* E"  */    0x8B, /**/
    // /* gr  */    0, /**/
    // /* gr  */    0, /**/
    // /* gr  */    0, /**/
    // /* I^  */    0x8E, /**/
    // /* I"  */    0x8F, /**/
    // /* gr  */    0, /**/
    // /* gr  */    0, /**/
    // /* gr  */    0, /**/
    // /* gr  */    0, /**/
    // /* gr  */    0, /**/
    // /* gr  */    0, /**/
    // /* gr  */    0, /**/


    // /* E0 =224 */
    // /* alpha */  0, /**/
    // /* beta  */  0,
    // /* O^    */  0x94, /**/
    // /* PI    */  0,
    // /* SIGMA */  0, /**/
    // /* sigma */  0, /**/
    // /* mu    */  0,  // C2 B5
    // /* gamma */  0, /**/
    // /* PHI   */  0, /**/
    // /* theta */  0, /**/
    // /* U^    */  0x9B, /**/
    // /* delta */  0, /**/
    // /* infini*/  0, /**/
    // /* inf. /*/  0, /**/
    // /* eps   */  0, /**/
    // /* eta   */  0, /**/

    // /* F0 =240 */
    // /* ===   */  0,
    // /* +-    */  0xb1, // C2
    // /* >=    */  0, /**/
    // /* <=    */  0, /**/
    // /* accol1*/  0, /**/
    // /* accol2*/  0, /**/
    // /* divis.*/  0, /**/
    // /* +-=   */  0, /**/
    // /* degree*/  0,
    // /* . cent*/  0, /**/
    // /* . cent*/  0,
    // /* sqrt  */  0, /**/
    // /* n exp */  0, /**/
    // /* 2 exp */  0,
    // /* pave  */  0, /**/
    // /*       */  0  /**/

// };

unsigned char SCR_OEM_UTF8_TBL[][4] = {

    /* 80 =128 */
    /* C c */    {0xC3, 0x87, 0},
    /* u " */    {0xC3, 0xbc, 0},
    /* e ' */    {0xC3, 0xa9, 0},
    /* a ^ */    {0xC3, 0xa2, 0},
    /* a " */    {0xC3, 0xa4, 0},
    /* a \ */    {0xC3, 0xa0, 0},
    /* a o */    {0xC3, 0xa5, 0},
    /* c c */    {0xC3, 0xa7, 0},
    /* e ^ */    {0xC3, 0xaa, 0},
    /* e " */    {0xC3, 0xab, 0},
    /* e \ */    {0xC3, 0xa8, 0},
    /* i " */    {0xC3, 0xaf, 0},
    /* i ^ */    {0xC3, 0xae, 0},
    /* i \ */    {0xC3, 0xac, 0},
    /* A " */    {0xC3, 0x84, 0},
    /* A o */    {0xC3, 0x85, 0},

    /* 90 =144 */
    /* E ' */    {0xC3, 0x89, 0},
    /* a e */    {0xC3, 0xa6, 0},
    /* A E */    {0xC3, 0x86, 0},
    /* o ^ */    {0xC3, 0xb4, 0},
    /* o " */    {0xC3, 0xb6, 0},
    /* o ` */    {0xC3, 0xb2, 0},
    /* u ^ */    {0xC3, 0xbb, 0},
    /* u \ */    {0xC3, 0xb9, 0},
    /* y " */    {0xC3, 0xbf, 0},
    /* O " */    {0xC3, 0x96, 0},
    /* U " */    {0xC3, 0x9c, 0},
    /* c | */    {0xC2, 0xa2, 0},
    /* pnd */    {0xC2, 0xa3, 0},
    /* Y = */    {0xC2, 0xa5, 0},
    /* P t */    {0xC2, 0xa0, 0},
    /* f - */    {0xC2, 0xa4, 0},

    /* A0 =160 */
    /* a ' */    {0xC3, 0xa1, 0},
    /* i ' */    {0xC3, 0xad, 0},
    /* o ' */    {0xC3, 0xb3, 0},
    /* u ' */    {0xC3, 0xba, 0},
    /* n ~ */    {0xC3, 0xb1, 0},
    /* N ~ */    {0xC3, 0x91, 0},
    /* º   */    {0xC2, 0xA7, 0},
    /* ░   */    {0xC2, 0xB0, 0},
    /* ? inv */  {0xC2, 0xBF, 0},
    /* not */    {0xC2, 0xAC, 0},
    /* nad */    {0},
    /* 1/2 */    {0xC2, 0xBD, 0},
    /* 1/4 */    {0xC2, 0xBC, 0},
    /* ! inv */  {0xC2, 0xA1, 0},
    /* <<  */    {0xC2, 0xAB, 0},
    /* >>  */    {0xC2, 0xBB, 0},

    /* B0 =176 => ansi car ascii = coins tableau */
    /* gr  */    {0xC2, 0xB0, 0},
    /* gr  */    {0xC2, 0xB1, 0},
    /* gr  */    {0xC2, 0xB2, 0},
    /* gr  */    {0xC2, 0xB3, 0},
    /* gr  */    {0xC2, 0xB4, 0},
    /* A┤  */    {0xC3, 0x81, 0},
    /* A^  */    {0xC3, 0x82, 0},
    /* A`  */    {0xC3, 0x80, 0},
    /* gr  */    {0xC2, 0xB8, 0},
    /* gr  */    {0xC2, 0xB9, 0},
    /* gr  */    {0xC2, 0xBA, 0},
    /* gr  */    {0xC2, 0xBB, 0},
    /* gr  */    {0xC2, 0xBC, 0},
    /* gr  */    {0xC2, 0xBD, 0},
    /* gr  */    {0xC2, 0xBE, 0},
    /* gr  */    {0xC2, 0xBF, 0},


    /* C0 =192 => ansi car ascii = coins tableau*/
    /* gr  */    {0xC3, 0x80, 0},
    /* gr  */    {0xC3, 0x81, 0},
    /* gr  */    {0xC3, 0x82, 0},
    /* gr  */    {0xC3, 0x83, 0},
    /* gr  */    {0xC3, 0x84, 0},
    /* Á  */     {0xC3, 0x85, 0},
    /* pi  */    {0xC3, 0x86, 0},
    /* gr  */    {0xC3, 0x87, 0},
    /* gr  */    {0xC3, 0x88, 0},
    /* gr  */    {0xC3, 0x89, 0},
    /* gr  */    {0xC3, 0x8A, 0},
    /* gr  */    {0xC3, 0x8B, 0},
    /* gr  */    {0xC3, 0x8C, 0},
    /* gr  */    {0xC3, 0x8D, 0},
    /* gr  */    {0xC3, 0x8E, 0},
    /* gr  */    {0xC3, 0x8F, 0},


    /* D0 = 208 => ansi car ascii = coins tableau */
    /* gr  */    {0xC3, 0x90, 0},
    /* gr  */    {0xC3, 0x91, 0},
    /* E ^ */    {0xC3, 0x8A, 0},
    /* E¿  */    {0xC3, 0x8B, 0},
    /* E`  */    {0xC3, 0x88, 0},
    /* gr  */    {0xC3, 0x95, 0},
    /* I┤  */    {0xC3, 0x8D, 0},
    /* I^  */    {0xC3, 0x8E, 0},
    /* I¿  */    {0xC3, 0x8F, 0},
    /* gr  */    {0xC3, 0x99, 0},
    /* gr  */    {0xC3, 0x9A, 0},
    /* gr  */    {0xC3, 0x9B, 0},
    /* gr  */    {0xC3, 0x9C, 0},
    /* gr  */    {0xC3, 0x9D, 0},
    /* I`  */    {0xC3, 0x8C, 0},
    /* gr  */    {0xC3, 0x9F, 0},


    /* E0 =224 */
    /* O┤ */  {0xC3, 0x93, 0},
    /* beta  */  {0xCE, 0xB2, 0},
    /* I^    */  {0xC3, 0x94, 0},
    /* O`   */  {0xC3, 0x92, 0},
    /* SIGMA */  {0xCE, 0xA3, 0},
    /* sigma */  {0xCF, 0x83, 0},
    /* mu    */  {0xCE, 0xBC, 0},
    /* gamma */  {0xCE, 0xB3, 0},
    /* PHI   */  {0xCE, 0xA6, 0},
    /* U┤ */    {0xC3, 0x9A, 0},
    /* U^    */  {0xC3, 0x9B, 0},
    /* U` */   {0xC3, 0x99, 0},
    /* infini*/  {0xE2, 0x88, 0x9E, 0},
    /* infini/*/ {0xE2, 0x88, 0x9E, 0},
    /* inclut */ {0xE2, 0x88, 0x88, 0},
    /* ┤  */    {0xC2, 0xB4, 0},

    /* F0 =240 */
    /* ==     x */  {0xE2, 0x89, 0xA1, 0},
    /* +-    */     {0xE2, 0x88, 0xA9, 0},
    /* >=     x */  {0xE2, 0x89, 0xA5, 0},
    /* <=     x */  {0xE2, 0x89, 0xA4, 0},
    /* paragraph*/  {0xC2, 0xA7, 0},
    /* paragr */   {0xC2, 0xA7, 0},
    /* divis. x */  {0xC3, 0xB7, 0},
    /* +-=    x */  {0xE2, 0x89, 0x88, 0},
    /* degree x */  {0xC2, 0xB0, 0},
    /* ¿ */      {0xC2, 0xA8, 0},
    /* . cent*/   {0xE2, 0x88, 0xA9, 0},
    /* sqrt   x */  {0xE2, 0x88, 0x9A, 0},
    /* n exp */  {0xE2, 0x88, 0xA9, 0},
    /* 2 exp  x */  {0xC2, 0xB2, 0},
    /* QED    x */  {0xE2, 0x88, 0x8E, 0},
    /*       */  {0}
};


unsigned char SCR_ANSI_UTF8_TBL[][4] = {

    /* 80 =128 */
    /* QED */    {0xE2, 0x88, 0x8E, 0},
    /* QED */    {0xE2, 0x88, 0x8E, 0},
    /* e ' */    {0xC3, 0xa9, 0},
    /* a ^ */    {0xC3, 0xa2, 0},
    /* a " */    {0xC3, 0xa4, 0},
    /* a \ */    {0xC3, 0xa0, 0},
    /* a o */    {0xC3, 0xa5, 0},
    /* c c */    {0xC3, 0xa7, 0},
    /* e ^ */    {0xC3, 0xaa, 0},
    /* e " */    {0xC3, 0xab, 0},
    /* e \ */    {0xC3, 0xa8, 0},
    /* i " */    {0xC3, 0xaf, 0},
    /* i ^ */    {0xC3, 0xae, 0},
    /* i \ */    {0xC3, 0xac, 0},
    /* A " */    {0xC3, 0x84, 0},
    /* A o */    {0xC3, 0x85, 0},

    /* 90 =144 */
    /* E ' */    {0xC3, 0x89, 0},
    /* a e */    {0xC3, 0xa6, 0},
    /* A E */    {0xC3, 0x86, 0},
    /* o ^ */    {0xC3, 0xb4, 0},
    /* o " */    {0xC3, 0xb6, 0},
    /* o \ */    {0xC3, 0xf2, 0},
    /* u ^ */    {0xC3, 0xbb, 0},
    /* u \ */    {0xC3, 0xb9, 0},
    /* y " */    {0xC3, 0xbf, 0},
    /* O " */    {0xC3, 0x96, 0},
    /* U " */    {0xC3, 0x9c, 0},
    /* c | */    {0xC2, 0xa2, 0},
    /* pnd */    {0xC2, 0xa3, 0},
    /* Y = */    {0xC2, 0xa5, 0},
    /* P t */    {0xC2, 0xa0, 0},
    /* f - */    {0xC2, 0xa4, 0},

    /* A0 =160 */
    /* a ' */    {0xC3, 0xa1, 0},
    /* i ' */    {0xC3, 0xad, 0},
    /* o ' */    {0xC3, 0xb3, 0},
    /* u ' */    {0xC3, 0xba, 0},
    /* n ~ */    {0xC3, 0xb1, 0},
    /* N ~ */    {0xC3, 0x91, 0},
    /* º   */    {0xC2, 0xA7, 0},
    /* ░   */    {0xC2, 0xB0, 0},
    /* ? inv */  {0xC2, 0xBF, 0},
    /* not */    {0xC2, 0xAC, 0},
    /* nad */    {0},
    /* 1/2 */    {0xC2, 0xBD, 0},
    /* 1/4 */    {0xC2, 0xBC, 0},
    /* ! inv */  {0xC2, 0xA1, 0},
    /* <<  */    {0xC2, 0xAB, 0},
    /* >>  */    {0xC2, 0xBB, 0},

    /* B0 =176 => ansi car ascii = coins tableau */
    /* gr  */    {0xC2, 0xB0, 0},
    /* gr  */    {0xC2, 0xB1, 0},
    /* gr  */    {0xC2, 0xB2, 0},
    /* gr  */    {0xC2, 0xB3, 0},
    /* gr  */    {0xC2, 0xB4, 0},
    /* Á  */     {0xC2, 0xB5, 0},
    /* pi  */    {0xC2, 0xB6, 0},
    /* gr  */    {0xC2, 0xB7, 0},
    /* gr  */    {0xC2, 0xB8, 0},
    /* gr  */    {0xC2, 0xB9, 0},
    /* gr  */    {0xC2, 0xBA, 0},
    /* gr  */    {0xC2, 0xBB, 0},
    /* gr  */    {0xC2, 0xBC, 0},
    /* gr  */    {0xC2, 0xBD, 0},
    /* gr  */    {0xC2, 0xBE, 0},
    /* gr  */    {0xC2, 0xBF, 0},


    /* C0 =192 => ansi car ascii = coins tableau*/
    /* gr  */    {0xC3, 0x80, 0},
    /* gr  */    {0xC3, 0x81, 0},
    /* gr  */    {0xC3, 0x82, 0},
    /* gr  */    {0xC3, 0x83, 0},
    /* gr  */    {0xC3, 0x84, 0},
    /* Á  */     {0xC3, 0x85, 0},
    /* pi  */    {0xC3, 0x86, 0},
    /* gr  */    {0xC3, 0x87, 0},
    /* gr  */    {0xC3, 0x88, 0},
    /* gr  */    {0xC3, 0x89, 0},
    /* gr  */    {0xC3, 0x8A, 0},
    /* gr  */    {0xC3, 0x8B, 0},
    /* gr  */    {0xC3, 0x8C, 0},
    /* gr  */    {0xC3, 0x8D, 0},
    /* gr  */    {0xC3, 0x8E, 0},
    /* gr  */    {0xC3, 0x8F, 0},


    /* D0 = 208 => ansi car ascii = coins tableau */
    /* gr  */    {0xC3, 0x90, 0},
    /* gr  */    {0xC3, 0x91, 0},
    /* gr  */    {0xC3, 0x92, 0},
    /* gr  */    {0xC3, 0x93, 0},
    /* gr  */    {0xC3, 0x94, 0},
    /* gr  */    {0xC3, 0x95, 0},
    /* pi  */    {0xC3, 0x96, 0},
    /* gr  */    {0xC3, 0x97, 0},
    /* gr  */    {0xC3, 0x98, 0},
    /* gr  */    {0xC3, 0x99, 0},
    /* gr  */    {0xC3, 0x9A, 0},
    /* gr  */    {0xC3, 0x9B, 0},
    /* gr  */    {0xC3, 0x9C, 0},
    /* gr  */    {0xC3, 0x9D, 0},
    /* gr  */    {0xC3, 0x9E, 0},
    /* gr  */    {0xC3, 0x9F, 0},


    /* E0 =224 */
    /* gr  */    {0xC3, 0xA0, 0},
    /* gr  */    {0xC3, 0xA1, 0},
    /* gr  */    {0xC3, 0xA2, 0},
    /* gr  */    {0xC3, 0xA3, 0},
    /* gr  */    {0xC3, 0xA4, 0},
    /* gr  */    {0xC3, 0xA5, 0},
    /* pi  */    {0xC3, 0xA6, 0},
    /* gr  */    {0xC3, 0xA7, 0},
    /* gr  */    {0xC3, 0xA8, 0},
    /* gr  */    {0xC3, 0xA9, 0},
    /* gr  */    {0xC3, 0xAA, 0},
    /* gr  */    {0xC3, 0xAB, 0},
    /* gr  */    {0xC3, 0xAC, 0},
    /* gr  */    {0xC3, 0xAD, 0},
    /* gr  */    {0xC3, 0xAE, 0},
    /* gr  */    {0xC3, 0xAF, 0},

    /* F0 =240 */
    /* gr  */    {0xC3, 0xB0, 0},
    /* gr  */    {0xC3, 0xB1, 0},
    /* gr  */    {0xC3, 0xB2, 0},
    /* gr  */    {0xC3, 0xB3, 0},
    /* gr  */    {0xC3, 0xB4, 0},
    /* gr  */    {0xC3, 0xB5, 0},
    /* pi  */    {0xC3, 0xB6, 0},
    /* gr  */    {0xC3, 0xB7, 0},
    /* gr  */    {0xC3, 0xB8, 0},
    /* gr  */    {0xC3, 0xB9, 0},
    /* gr  */    {0xC3, 0xBA, 0},
    /* gr  */    {0xC3, 0xBB, 0},
    /* gr  */    {0xC3, 0xBC, 0},
    /* gr  */    {0xC3, 0xBD, 0},
    /* gr  */    {0xC3, 0xBE, 0},
    /* gr  */    {0xC3, 0xBF, 0},
};



/* ================================================================
Transforme un caractère ASCII-PC en caractère UTF-8. 
Le résultat est stocké dans un buffer statique retourné par la fonction.

&RT le caractère traduit
&SA SCR_AnsiToUTF8Char(), SCR_AnsiToOemChar(), SCR_OemToAnsiChar(),
SCR_OemToAnsi(), SCR_AnsiToOem(), SCR_AnsiToUTF8(), SCR_OemToUTF8()

=================================================================== */

unsigned char *SCR_OemToUTF8Char(int ch)
{
    static unsigned char res[6];

    if(ch >= 128) return(SCR_OEM_UTF8_TBL[ch - 128]);
    res[0] = ch;
    res[1] = 0;
    return(res);
}

/* ================================================================
Transforme un caractère ASCII-PC en caractère UTF-8. 
Le résultat est stocké dans un buffer statique retourné par la fonction.

&RT le caractère traduit
&SA SCR_OemToUTF8Char(), SCR_AnsiToOemChar(), SCR_OemToAnsiChar(),
SCR_OemToAnsi(), SCR_AnsiToOem(), SCR_AnsiToUTF8(), SCR_OemToUTF8()

=================================================================== */

unsigned char *SCR_AnsiToUTF8Char(int ch)
{
    static unsigned char res[6];

    if(ch >= 128) return(SCR_ANSI_UTF8_TBL[ch - 128]);
    res[0] = ch;
    res[1] = 0;   
    return(res);
}

/* ================================================================
Calcule la longueur en bytes d'une ch¯ne OEM traduite en UTF8.

&RT la longueyur nécessaire pour stocker le résultat de la translation.
=================================================================== */

int SCR_OemToUTF8Length(unsigned char *str)
{
    return(SCR_OemAnsiToUTF8Length(str, 0));
}

int SCR_AnsiToUTF8Length(unsigned char *str)
{
    return(SCR_OemAnsiToUTF8Length(str, 1));
}


/* ================================================================
Calcule la longueur en bytes d'une ch¯ne OEM traduite en UTF8.

&RT la longueyur nécessaire pour stocker le résultat de la translation.
=================================================================== */

int SCR_OemAnsiToUTF8Length(unsigned char *str, int oemansi)
{
    int         i, lg = 0;
        
    if(str == 0) return(0);
    
    for(i = 0 ; str[i] ; i++) {
        if(oemansi == 0) lg += (int)strlen(SCR_OemToUTF8Char(str[i]));
        else             lg += (int)strlen(SCR_AnsiToUTF8Char(str[i]));
    }
    return(lg);
}

/*NH*/
unsigned char *SCR_OemAnsiToUTF8(unsigned char *str, unsigned char *res, int oemansi)
{
    int         i, lg = 0;
        
    if(str == 0 || res == 0) return(0);
    
    res[0] = 0;
    for(i = 0 ; str[i] ; i++) {
        if(oemansi == 0) strcat(res, SCR_OemToUTF8Char(str[i]));
        else             strcat(res, SCR_AnsiToUTF8Char(str[i]));
    }
    return(res);
}

unsigned char *SCR_OemToUTF8(unsigned char *str, unsigned char *res)
{
    return(SCR_OemAnsiToUTF8(str, res, 0));
}

unsigned char *SCR_AnsiToUTF8(unsigned char *str, unsigned char *res)
{
    return(SCR_OemAnsiToUTF8(str, res, 1));
}

int SCR_ConvertToUTF8(char *filein, char *fileout, int isbom, int isansi)
{
    char    *rb, *wb;
    FILE    *fd1, *fd2;
    int     c;
    unsigned char *str;
    unsigned char bom[] = {0357, 0273, 0277};
    
    #ifdef DOS
        rb = "rb";
        wb = "wb+";
    #else
        rb = "r";
        wb = "w+";
    #endif
    
    fd1 = stdin;
    fd2 = stdout;
    if(filein)  fd1 = fopen(filein, rb);
    if(fd1 == 0) return(-1);
    if(fileout)  fd2 = fopen(fileout, wb);
    if(fd2 == 0) {
        fclose(fd1);
        return(-2);
    }    
        
    if(isbom) fwrite(bom, sizeof(bom), 1,  fd2);    
    while(!feof(fd1)) {
        c = getc(fd1);
        if(c == EOF) break;
        if(isansi) str = SCR_AnsiToUTF8Char(c);
        else       str = SCR_OemToUTF8Char(c);
        fwrite(str, strlen(str), 1,  fd2);
    }

    fclose(fd1);
    fclose(fd2);
    return(0);
}    

int SCR_ConvertOemToUTF8(char *filein, char *fileout, int isbom)
{
    return(SCR_ConvertToUTF8(filein, fileout, isbom, 0));
}

int SCR_ConvertAnsiToUTF8(char *filein, char *fileout, int isbom)
{
    return(SCR_ConvertToUTF8(filein, fileout, isbom, 1));
}
