/**
 *  @header4iode
 * 
 *  GSAMPLE compiler.
 *  
 *  A GSAMPLE (or generalised SAMPLE) defines groups of periods and files on which 
 *  the tables formulas are to be calculated. 
 *  
 *  See https://iode.plan.be/doku.php?id=sample_d_impression for more details.
 *  
 *  Formal definition of a GSAMPLE (tentative)
 *  ------------------------------------------
 *      gsample    ::= gsample_n[';'gsample_n...]
 *      gsample_n  ::= gsample_1[repetition['*'['-']increment]] 
 *      repetition ::= integer
 *      increment  ::= integer
 *  
 *      gsample_1  ::= period_ops [file_ops]
 *      period_ops ::= period_op | '(' period_op ';' period_op... ')'
 *      period_op  ::= period [opp period] ['>' shift] ['<' shift]
 *      period     ::= integer[periodicity period] | 
 *                      'EOS' | 'BOS' | 'NOW' | 'EOS1' | 'BOS1' |'NOW1'
 *      shift      ::= 0 | integer
 *      opp        ::= '-' | '--' | '/' | '//' | '+' | '^' | '~' | '=' 
 *  
 *      file_ops   ::= '[' file_op | file_op ';' file_op ']'
 *      file_op    ::= file [opf file] 
 *      file       ::= integer
 *      opf        ::= '+' | '-' | '/' | '+' | '^' 
 *  
 *  Examples
 *  --------
 *  Various examples can be found here: https://iode.plan.be/doku.php?id=sample_d_impression or in TBL_CALC.md. 
 *  
 *  
 *  List of functions 
 *  -----------------
 *      COLS *COL_cc(char* gsample)                         GSAMPLE compiler
 *      int COL_free_cols(COLS* cls)                        Frees the allocated space for a COLS structure created by COL_cc()
 *      char *COL_text(COL* cl, char* str, int nbnames)     Constructs a string based on a special table cell text value containing a "#"
 *      COLS *COL_add_col(COLS* cls)                        Adds a new COL struct to the COLS (list of periods in a GSAMPLE)
 *      int COL_find_mode(COLS* cls, int* mode, int type)   Analyses a COLS struct and set 1 in the vector mode for each found operation
 *
 */

#include "iode.h"

// GSAMPLE tokens -- may be expanded
YYKEYS COL_KEYWS[] = {
    "[",    COL_OBRACK,
    "]",    COL_CBRACK,
    "(",    COL_OPAR,
    ")",    COL_CPAR,
    ":",    COL_COLON,
    ";",    COL_COMMA,
    "*",    COL_DOT,
    "<",    COL_SHIFTL,
    ">",    COL_SHIFTR,

    "-",    COL_DIFF,
    "--",   COL_MDIFF,
    "~",    COL_MEAN,  /* ^ */
    "^",    COL_MEAN,  /* ^ */
    "+",    COL_ADD,
    "=",    COL_BASE,
    "/",    COL_GRT,
    "//",   COL_MGRT,

    "EOS",   COL_EOS,
    "EOSM",  COL_EOS,
    "EOSQ",  COL_EOS,
    "EOSW",  COL_EOS,

    "EOS1",   COL_EOS1,
    "EOSM1",  COL_EOS1,
    "EOSQ1",  COL_EOS1,
    "EOSW1",  COL_EOS1,

    "BOS",   COL_BOS,
    "BOSM",  COL_BOS,
    "BOSQ",  COL_BOS,
    "BOSW",  COL_BOS,

    "BOS1",   COL_BOS1,
    "BOSM1",  COL_BOS1,
    "BOSQ1",  COL_BOS1,
    "BOSW1",  COL_BOS1,

    "NOW",   COL_NOW,
    "NOWM",  COL_NOW,
    "NOWQ",  COL_NOW,
    "NOWW",  COL_NOW,

    "NOW1",   COL_NOW1,
    "NOWM1",  COL_NOW1,
    "NOWQ1",  COL_NOW1,
    "NOWW1",  COL_NOW1,

    "PER",  COL_PER,
    "P",    COL_PER,

    "S",    COL_SUBPER,
    "SUB",  COL_SUBPER

};

// Operator representations used when printing (only valid for period operations)
// TODO: distinguish bw operations between periods and between files ?
char    *COL_OPERS[] = {
    "",     // COL_NOP
    "-",    // COL_DIFF,
    "--",   // COL_MDIFF, 
    "^",    // COL_MEAN, 
    "/",    // COL_GRT,
    "//",   // COL_MGRT,
    "+",    // COL_ADD,
    "="     // COL_BASE,
//    "<",    // Faux !! Ne peut pas se produire
//    ">"     // Faux !! Idem
};


/*
    Compile un GSAMPLE (d�finitions des colonnes � imprimer). 
    
    Cette d�finition contient une liste de p�riodes, �ventuellement affect�es 
    d'une op�ration (taux de croissance, diff�rence...)  appliqu�es � des fichiers repr�sent�s
    ici  par  leurs  num�ros d'ordre. 
    
    Chaque p�riode est s�par�e de la suivante par une virgule ou un point-virgule. 
    Une colonne ou un groupe de colonnes peut �tre r�p�t�e : il suffit de placer apr�s
    la  d�finition de la colonne ou du groupe de colonnes un double-point (":") 
    suivi du nombre de r�p�titions souhait�.

    La d�finition des fichiers est optionnelle et est plac�e
    entre crochets. Elle s'applique � toute la d�finition de
    p�riode  qui  pr�c�de.  Les op�rations possibles sur les
    fichiers sont :

	- valeur absolue                        (1)
	- diff�rence                            (1-2)
	- diff�rence en pourcents               (1/2)
	- somme                                 (1+2)
	- moyenne                               (1^2)

    Les op�rations sur les p�riodes sont :

	- valeur (75)
	- taux de croissance sur une ou plusieurs p�riodes (75/74, 75/70)
	- taux de croissance moyen (75//70)
	- diff�rence (75-74, 75-70)
	- diff�rence moyenne (75--70)
	- moyenne (75^74)
	- somme de p�riode � p�riode cons�cutives (70Q1+70Q4)
	- valeur en indice ou en base (76=70)

    La   r�p�tition   peut  s'effectuer  avec  un  incr�ment
	sup�rieur � 1 ou < 0: il suffit de placer une �toile suivie du
    pas  apr�s  le  nombre  de r�p�titions (70:3*5 = 70, 75,
    80).

	21-05-2012 : La   r�p�tition   peut  s'effectuer  avec  un  incr�ment negatif.
	Dans ce cas, l'affichage se fait a l'envers (2000:3*-1 = 2000, 1999, 1998, ...)

    Exemples :
	    70, 75, 80:6  = 70:3*5, 81:5 =
			    70, 75, 80, 81, 82, 83, 84, 85

	    70/69:2       = 70/69, 71/70

	    (70, 70-69):2 = 70, 70-69, 71, 71-70,

	    70[1,2]:2*5   = 70[1], 70[2], 75[1], 75[2]

	    (70,75){1,2-1} = 70[1],75[1],70[2-1],75[2-1]

	    (70,75,(80, 80/79):2)[1,2] = 70[1], 70[2], 75[1], 75[2],
					 80[1], 80[2], 80/79[1], 80/79[2]
					 81[1], 81[2], 81/80[1], 81/80[2]

*/




/**
 *  Frees the allocated space for a FILS structure created by COL_cc().
 *  
 *  @param [in] FILS*   fils    FILS to free
 *  
 */
 
static void COL_free_fils(FILS* fils)
{
    if(fils == 0) return;
    if(fils->fl_nb != 0) SW_nfree(fils->fl_fils);
    SW_nfree(fils);
}


/**
 *  Constructs a string based on a special table cell text value containing a "#".
 *  
 *  Returns a static string constructed from an atomic element 
 *  of a table text cell containing the '#' character and adapted 
 *  to the contents of a specific COL (periods and files). 
 *  
 *  COL_ctoa() is a sub function of COL_text().
 *  
 *  Examples
 *  --------
 *  Cell contents   ch   n  periods      files      Result
 *  #s              s    1  2020/2019    [1]        20/19[1]
 *  #T              T    1  2020/2019    [1]        2020/2019
 *  #y1             y    1  2020/2019    1          20
 *  #O              O    1  2020/2019    1          Taux de croissance
 *  
 *  More details and the full syntax can be found in the IODE manual: see    
 *  https://iode.plan.be/doku.php?id=tables#interpetation_des_cellules_de_texte
 *  
 *  
 *  @param [in] COL*    cl      COL struct containing the current periods, files and operations 
 *  @param [in] int     ch      character following # in the cell definition (ex 'y' for "#y")
 *  @param [in] int     n       position of the period to print (#y1 => n=1, #y2 => n=2 )
 *  @param [in] int     nbf     number of compared files in the current GSAMPLE
 *  @return     char*           static buffer with the formatting of the TBL text cell
 *  
 */

char *COL_ctoa(COL* cl, int ch, int n, int nbf)
{
    static char res[30];
    PERIOD      *per;
    // extern int  B_LANG;
    extern int  K_LANG;
    extern char *KLG_MONTHS[][3], *KLG_ROM[], *KLG_OPERS_TEXTS[][3];

    res[0] = res[1] = 0;

    if(U_is_in(ch, "yYpPrRnNmMoO")) {
        if(cl->cl_opy == COL_NOP && n == 1) return(res);
        per = &(cl->cl_per[n]);
        if(U_is_in(ch, "PpMmRrNn") && per->p_p == 'Y') return(res);
        switch(ch) {
            case 'y' :
                sprintf(res, "%02ld", (per->p_y) % 100L);
                break;
            case 'Y' :
                sprintf(res, "%04ld", per->p_y);
                break;
            case 'p' :
                res[0] = SCR_lower_char(per->p_p);
                break;
            case 'P' :
                res[0] = per->p_p;
                break;
            case 'M' :
            case 'm' :
                if(per->p_p != 'M') goto Rom;
                //strcpy(res, KLG_MONTHS[per->p_s -1][B_LANG]);
                strcpy(res, KLG_MONTHS[per->p_s -1][K_LANG]);
                if(ch == 'm') res[3] = 0;
                break;
Rom:
            case 'r' :
            case 'R' :
                if(!U_is_in(per->p_p, "QM")) goto Num;
                strcpy(res, KLG_ROM[per->p_s - 1]);
                if(SCR_is_lower(ch)) SCR_lower(res);
                break;
Num:
            case 'n' :
            case 'N' :
                sprintf(res, "%ld", per->p_s);
                break;
            case 'o' :
                if(cl->cl_opy == COL_NOP) return(res);
                strcpy(res, COL_OPERS[cl->cl_opy - COL_NOP]);
                break;
            case 'O' :
                if(cl->cl_opy == COL_NOP) return(res);
                //strcpy(res, KLG_OPERS_TEXTS[cl->cl_opy - COL_NOP][B_LANG]);
                strcpy(res, KLG_OPERS_TEXTS[cl->cl_opy - COL_NOP][K_LANG]);
                break;
        }
        return(res);
    }

    if(U_is_in(ch, "fF")) {
        if(nbf < 2) return(res);
        if(cl->cl_opf == COL_NOP) sprintf(res, "[%d]", cl->cl_fnb[0]);
        else    sprintf(res, "[%d%s%d]", cl->cl_fnb[0],
                            COL_OPERS[cl->cl_opf - COL_NOP],
                            cl->cl_fnb[1]);

        return(res);
    }
    return(res);
}

/**
 *  The recursive function COL_text() returns an allocated string 
 *  constructed from a TBL text cell containing the '#' character. The result
 *  depends on the contents of the specific COL (periods and files) passed as argument. 
 *  
 *  
 *  @param [in] COL*    cl      COL struct containing the current periods, files and operations 
 *  @param [in] char*   str     contents of a table cell of type "text"
 *  @param [in] int     nbnames number of files compared in the current GSAMPLE
 *  @return     char*           allocated string    
 *  
 *  TODO: check the allocated length (40+3+..) that could be too small!
 */
 
char *COL_text(COL* cl, char* str, int nbnames)
{
    int     i, j, op, n, lg;
    char    *res, *txt, *tmp;

    if(str == NULL) return(NULL);
    lg = strlen(str) + 40;
    res = SW_nalloc(lg + 3);

    for(i = j = 0 ; j < lg && str[i] ; i++) {
        if(str[i] != '#') {
            res[j++] = str[i];
            continue;
        }
        op = str[i + 1];
        n = 0;
        if(!U_is_in(op, "yYpPrRnNmMfFoOtTsS")) continue;
        i++;
        if(U_is_in(str[i + 1], "12")) {
            n = str[i + 1] - '1';
            i++;
        }
        if(U_is_in(op, "sStT")) {
            if(op == 's')       tmp = "#t#f";
            else if(op == 'S')  tmp = "#T#F";
            else if(op == 't')  tmp = "#y1#P1#n1#o1#y2#P2#n2";
            else if(op == 'T')  tmp = "#Y1#P1#n1#o1#Y2#P2#n2";
            txt = COL_text(cl, tmp, nbnames);
            if(j + strlen(txt) < lg) {
                strcat(res, txt);
                SCR_free(txt);
            }
            else break;
        }
        else {
            txt = COL_ctoa(cl, op, n, nbnames);
            if(j + strlen(txt) >= lg) break;
            strcat(res, txt);
        }
        j = strlen(res);
    }
    return(res);
}


/**
 *  Adds a new COL struct to the COLS (list of periods in a GSAMPLE).
 *  
 *  @param [in, out] COLS*  cls     current COLS (before adding a new COL) or NULL 
 *  @return          COLS*          if cols == NULL : newly allocated COLS structure
 *  
 */

COLS *COL_add_col(COLS* cls)
{
    if(cls == 0) {
        cls = (COLS *) SW_nalloc(sizeof(COLS));     // BUG corrected: was sizeof(COL)
    }
    if(cls->cl_nb % 20 == 0) // enlarges by chunks of 20
        cls->cl_cols = (COL *) SW_nrealloc(cls->cl_cols,
                                           sizeof(COL) * cls->cl_nb,
                                           sizeof(COL) * (cls->cl_nb + 20));
    cls->cl_nb ++;
    return(cls);
}


/**
 *  Adds a new FIL struct to the FILS (list of files in a GSAMPLE).
 *  
 *  @param [in, out] FILS*  fils    current FILS (before adding a new FIL) or NULL 
 *  @return          FILS*          if fils == NULL : newly allocated FILS structure
 *  
 */
 
static FILS* COL_add_fil(FILS* fils)
{
    if(fils == 0) {
        fils = (FILS *) SW_nalloc(sizeof(FILS)); // BUG in previous versions: 
                                                 // was SW_nalloc(sizeof(FIL)) instead of SW_nalloc(sizeof(FILS))
    }
    if(fils->fl_nb % 5 == 0)   // enlarges by chunks of 5
        fils->fl_fils = (FIL *) SW_nrealloc(fils->fl_fils,
                                           sizeof(FIL) * fils->fl_nb,
                                           sizeof(FIL) * (fils->fl_nb + 5));
    fils->fl_nb++;
    return(fils);
}


/**
 *  Copies the content of a FIL struct into a COL struct.
 *  
 *  @param [in, out]  COL*  cl  COL to update
 *  @param [in]       FIL*  fl  FIL to copy to cl
 *  @return 
 *  
 */

static void COL_apply_fil(COL* cl, FIL* fl)
{
    if(cl->cl_fnb[0] != 0) return;
    cl->cl_fnb[0] = fl->fl_1;
    cl->cl_fnb[1] = fl->fl_2;
    cl->cl_opf = fl->fl_op;
}


/**
 *  Constructs the PERIOD corresponding to TODAY in the current workspace periodicity.
 *  
 *  For example, if the periodicity is Q and the date of the day is 15/4/2022, the
 *  result will be equivalent to "2002Q2".
 *  
 *  @param [in, out] PERIOD*    per   constructed PERIOD (not allocated in the function, must exist)
 *  
 */
 
static void COL_calc_now(PERIOD *per)
{
    memcpy(per, &(KSMPL(KV_WS)->s_p1), sizeof(PERIOD));
    per->p_y = SCR_current_date() / 10000L;
    switch(per->p_p) {
        case 'M' :
            per->p_s = (SCR_current_date() / 100) % 100;
            break;
        case 'Q' :
            per->p_s = 1 + ((SCR_current_date() / 100) % 100 - 1) / 3;
            break;
        case 'W' :
            per->p_s = DT_week_number(SCR_current_date());
            if(per->p_s == 0) {
                per->p_y--;
                per->p_s = 53;
            }
            break;
        case 'Y' :
        default :
            break;
    }
}


/**
 *  Reads a GSAMPLE valid period on the YY stream yy.
 *  
 *  Syntax:  PERIOD[Pppp][<n|>n]
 *  where 
 *      PERIOD ::= {yyyy | yy | EOS | EOS1 | BOS | BOS1 | NOW | NOW1} (year or position in sample)
 *      P      ::= {Y|Q|M|W}  (periodicity)
 *      ppp    ::= integer   (sub period)
 *      n      ::= integer   (shift) 
 *  
 *  @param [in] YYFILE* yy      open YY stream
 *  @param [in] PERIOD* per     pointer to the resulting period 
 *  @return     int             0 on success, -1 on error
 *  
 */

static int COL_read_per(YYFILE* yy, PERIOD* per)
{
    int     ch, ch1, pos, sign = 1, keyw, shiftval;
    PERIOD	*pertmp;

    per->p_p = 'Y';
    per->p_y = 1970;
    per->p_s = 1;

    // Syntaxe : year[Pper][<n] ex 2000Y1<5  == 1995Y1
    //    ou     year[Pper][>n] ex 2000Y1>5  == 2005Y1

    // 1. year
    keyw = YY_lex(yy);
    switch(keyw) {
        case YY_LONG :
            per->p_y = yy->yy_long;
            if(per->p_y < 50) per->p_y += 2000;
            else if(per->p_y < 200) per->p_y += 1900;
            break;

        case COL_EOS :
            memcpy(per, &(KSMPL(KV_WS)->s_p2), sizeof(PERIOD));
            break;

        case COL_EOS1 :
            memcpy(per, &(KSMPL(KV_WS)->s_p2), sizeof(PERIOD));
            per->p_s = 1;
            break;

        case COL_BOS :
            memcpy(per, &(KSMPL(KV_WS)->s_p1), sizeof(PERIOD));
            break;

        case COL_BOS1 :
            memcpy(per, &(KSMPL(KV_WS)->s_p1), sizeof(PERIOD));
            per->p_s = 1;
            break;

        case COL_NOW :
            COL_calc_now(per);
            break;

        case COL_NOW1 :
            COL_calc_now(per);
            per->p_s = 1;
            break;

        default :
            return(-1);
    }

    // 2. Pper
    ch1 = ch = YY_getc(yy);
    if(ch1 >= 'a' && ch1 <= 'z')  ch1 += 'A' - 'a'; // JMP 21-05-2012
    pos = L_pos(L_PERIOD_CH, ch1);
    if(pos >= 0) {
        per->p_p = ch1;
        if(YY_lex(yy) != YY_LONG) return(-1);
        per->p_s = yy->yy_long;
        if(per->p_s < 1 || per->p_s > L_PERIOD_NB[pos]) return(-1);
        YY_skip_spaces(yy);
        ch	= YY_getc(yy); // Lit la suite
    }

    // 3. <n ou >n avec possible r�p�tition (<0>1 ou <0>N comptent pour la seule p�riode courante)
nextshift:
    if(!U_is_in(ch, "<>")) {				// JMP 21-05-2012
        YY_ungetc(ch, yy);
        return(0);
    }

    if(ch == '<') sign = -1;			   	// JMP 21-05-2012
    else          sign = 1;
    shiftval = COL_read_long_per(yy);
    if(shiftval < 0) return(-1);

    /*
    if(YY_lex(yy) != YY_LONG) return(-1);
    */
    if(shiftval == 0) {
        if(sign > 0) per->p_s = PER_nb(per->p_p);
        else         per->p_s = 1;
    }
    else {
        pertmp = PER_addper(per, sign * shiftval);
        memcpy(per, pertmp, sizeof(PERIOD));
    }

    // Lit la suite des <> s'il y en a pour pouvoir �crire 2000Q5<0>1/2000Q5<0 => 2000Q2/2000Q1
    YY_skip_spaces(yy);
    ch	= YY_getc(yy); // Lit la suite
    goto nextshift;

    return(0);
}


/**
 *  Reads a period or an operation on 2 periods on an YY stream.
 *  
 *  Syntax:  per1 [op per2]
 *  where 
 *      per1 and per2 : see COL_read_per()
 *      op : "" or one of "-" "--" "~" "^" "+" "=" "/" "//" (see iode manual)
 *  
 *  @param [in, out] YYFILE*    yy  YY stream
 *  @return          COLS*          compiled column
 *  
 */
 
static COLS *COL_read_y(YYFILE* yy)
{
    PERIOD  per;
    COLS    *cls;
    int     op;

    if(COL_read_per(yy, &per)) return((COLS *)0);
    cls = COL_add_col((COLS *)0);
    cls->cl_cols[0].cl_opy = COL_NOP;
    memcpy(&(cls->cl_cols[0].cl_per[0]), &per, sizeof(PERIOD));

    op = YY_lex(yy);
    if(op <= COL_NOP || op >= COL_LAST_OP) {
        YY_unread(yy);
        return(cls);
    }

    cls->cl_cols[0].cl_opy = op;

    if(COL_read_per(yy, &per)) {
        COL_free_cols(cls);
        return((COLS *)0);
    }
    memcpy(&(cls->cl_cols[0].cl_per[1]), &per, sizeof(PERIOD));
    return(cls);
}


/**
 *  Calculates the sub period corresponding to the current date in the 
 *  workspace periodicity.
 *  
 *  @return int    current sub period
 *  
 */
 
static int COL_calc_subper()
{
    PERIOD  *per;
    int     w, p;

    per = &(KSMPL(KV_WS)->s_p1);

    switch(per->p_p) {
        case 'M' :
            return((SCR_current_date() / 100) % 100);
        case 'Q' :
            return(1 + ((SCR_current_date() / 100) % 100 - 1) / 3);
        case 'W' :
            p = DT_week_number(SCR_current_date());
            if(p == 0) p = 53;
            return(p);
        case 'Y' :
        default :
            return(1);
    }
}


// Lit un long ou P ou PER et retourne la valeur ou -1 en cas d'erreur

/**
 *  Reads "P", "PER", "S", "SUB" or a number on a YY stream and returns 
 *  the sub period corresponding position.
 *  
 *      P or PER -> number of sub periods in the current periodicity
 *      S or SUB -> sub period of the current date in the current periodicity
 *      number   -> indicates a sub period position 
 *  
 *  @param [in] YYFILE*  yy     YY stream
 *  @return     int             sub period position             
 *  
 */
 
static int COL_read_long_per(YYFILE *yy)
{
    int     keyw = YY_lex(yy);

    if(keyw == YY_LONG)         return(yy->yy_long);
    else if(keyw == COL_PER)    return(PER_nb(KSMPL(KV_WS)->s_p1.p_p));
    else if(keyw == COL_SUBPER) return(COL_calc_subper());
    else                        return(-1);
}


/**
 *  Reads an optional repetition factor in a GSAMPLE.
 *  
 *  Syntax : [:n[*[-]incr]]
 *  
 *  @param [in]  YYFILE* yy     YY stream
 *  @param [out] REP*    rep    number of repetitions (n) and increment (incr)
 *  @return      int            0 on success, -1 on syntax error  
 *  
 */
 
static int COL_read_rep(YYFILE* yy, REP* rep)
{
    int	sign = 1; // JMP 21-05-2012
    int	keyw;

    rep->r_incr = 1;

    // Syntax : [:n[*[-]incr]]
    // 1. partie (optionnelle) n
    rep->r_nb = COL_read_long_per(yy);
    if(rep->r_nb < 0) return(-1);

    // 2. partie optionnelle :[-]incr
    if(YY_lex(yy) != COL_DOT) {
        YY_unread(yy);
        return(0);
    }

    keyw = YY_lex(yy);
    // incr�ment n�gatif si -
    if(keyw == COL_DIFF)
        sign = -1;
    else
        YY_unread(yy);

    rep->r_incr = COL_read_long_per(yy);
    if(rep->r_incr < 0) return(-1);
    rep->r_incr *= sign;

    return(0);
}


/**
 *  Reads one FILE operation in a GSAMPLE (containing optionnaly an operator and a second file).
 *  
 *  Syntax: 
 *      file_op ::= file1 [op file2]
 *      where 
 *           op is +, -, /, ~, ^ or =
 *           file1 and file2 are integers
 *  
 *  Example:  1/2
 *  
 *  @param [in] YYFILE*     yy   YY stream
 *  @param [in] FIL*        fil  struct containing the file numbers and an opt. operator   
 *  @return     int         0 on success, -1 on syntax error    
 *  
 */
 
static int COL_read_1f(YYFILE* yy, FIL* fil)
{
    int     op;

    fil->fl_op = COL_NOP;
    fil->fl_2  = 0;
    if(YY_lex(yy) != YY_LONG) return(-1);
    if(yy->yy_long < 1 || yy->yy_long > 10) return(-1);
    fil->fl_1 = yy->yy_long;
    op = YY_lex(yy);
    /*GB    if(op != COL_ADD && op != COL_DIFF && op != COL_GRT && op != COL_MEAN) { */
    if(op != COL_ADD && op != COL_DIFF && op != COL_GRT && op != COL_MEAN && op != COL_BASE) {
        YY_unread(yy);
        return(0);
    }
    fil->fl_op = op;
    if(YY_lex(yy) != YY_LONG) return(-1);
    fil->fl_2 = yy->yy_long;
    return(0);
}


/**
 *  Reads in a GSAMPLE a list of FILE operations surrounded by brackets [].
 *  
 *  Syntax: 
 *      file_op_1;file_op_2...]
 *      where 
 *           file_op_1 is defined in COL_read_1f()
 *  
 *  Example:  [1/2;1;2]
 *  
 *  @note The opening bracket is read before the call but the closing bracket 
 *  is read by the function.
 *  
 *  @param [in] YYFILE*     yy   YY stream
 *  @return     FILS*            group of FIL struct containing the file references 
 *                               and the operator for each file operation 
 *                               NULL on error
 */
 
static FILS *COL_read_f(YYFILE* yy)
{
    FILS    *fils = 0, fil;

    while(1) {
        fils = (FILS *)COL_add_fil(fils);
        if(COL_read_1f(yy, fils->fl_fils + fils->fl_nb - 1)) goto err;
        switch(YY_lex(yy)) {
            case COL_COMMA :
                break;
            case COL_CBRACK :
                return(fils);
            default :
                goto err;
        }
    }

err:
    COL_free_fils(fils);
    return((FILS *)0);
}


/**
 *  Modifies a partial GSAMPLE by shifting all periods by nb positions
 *  to the left (COL_SHIFTL)or the right (COL_SHIFTR) according to the value of key.
 *  
 *  Examples
 *      2000Q3/2000Q1<2              === 2000Q1/1999Q3 
 *      2000Q3/2000Q1>1              === 2000Q4/2000Q2
 *      2000Q1<0 or 2000Q3<0 or ...  === 2000Q1
 *      2000Q3>0 or 2000Q4>0 or ...  === 2000Q4
 *  
 *  @param [in, out] COLS*   cls    partial compiled GSAMPLE 
 *  @param [in]      int     key    period shift direction: COL_SHIFTL or COL_SHIFTR 
 *  @param [in]      in      nb     if not 0: nb of period to shift 
 *                                  if 0 and key == COL_SHIFTL: shift to the 1st period of the year
 *                                  if 0 and key == COL_SHIFTR: shift to the last period of the year
 *  
 */
 
static void COL_shift(COLS *cls, int key, int nb)
{
    COL   	*cl;
    PERIOD  *per;
    int    	i;

    for(i = 0; i < cls->cl_nb; i++) {
        cl = cls->cl_cols + i;
        if(nb == 0) {
            if(key == COL_SHIFTR)
                cl->cl_per[0].p_s = cl->cl_per[1].p_s = PER_nb(cl->cl_per[0].p_p);
            else
                cl->cl_per[0].p_s = cl->cl_per[1].p_s = 1;
        }
        else {
            if(key == COL_SHIFTL) nb = -nb;
            per = PER_addper(cl->cl_per, nb);
            memcpy(cl->cl_per, per, sizeof(PERIOD));
            if(cl->cl_opy != COL_NOP) {
                per = PER_addper(cl->cl_per + 1, nb);
                memcpy(cl->cl_per + 1, per, sizeof(PERIOD));
            }
        }
    }
}


// Default FILS for COL_construct()
static FIL     COL_FIL  = {0, 0, 0};           
static FILS    COL_FILS = {1, &COL_FIL};


/**
 *  Completes the construction of a GSAMPLE compiled form.
 *  
 *  Given a COLS struct cltmp (result of a partial GSAMPLE compilation, 
 *  for ex. "2000:5[1;2]" in "2000:5[1;2];2000/1999:5"), adds it to the global GSAMPLE 
 *  compiled struct cls.
 *  
 *  The cltmp is added to cols rep->r_nb times * fils->fl_nb times. 
 *  If cltmp comes from "2000:5[1;2]", adds 5 * 2 the COLS in cltmp to the global cols.
 *  
 *  See more examples in COL_read_cols().
 *  
 *  @param [in, out] COLS*  cls         resulting columns
 *  @param [in]      COLS*  cltmp       compiled GSAMPLE to be added to cls rep->r_nb times
 *  @param [in]      FILS*  fils        FILS on which to cltmp must be applied
 *  @param [in]      REP*   rep         repetition format (n * m)
 *  @param [in]      int    shiftdir    unused ? Replaced by COL_shift() ?
 *  @param [in]      int    shiftval    unused ? id.
 *  @return          COLS*              cls (same pointer but content modified)
 *  
 */

static COLS *COL_construct(COLS* cls, COLS* cltmp, FILS* fils, REP* rep, int shiftdir, int shiftval)
{
    int     i, nbtmp, j, nbfils, k;
    COL     *cl, *tmp;
    PERIOD  *per;

    if(fils == 0) fils = &COL_FILS; // if no fils, use a default  value
    nbfils = fils->fl_nb;

    if(rep->r_nb == 0) {
        rep->r_nb   = 1;
        rep->r_incr = 1;
    }

    nbtmp = cltmp->cl_nb;
    tmp   = cltmp->cl_cols;

    // Repetitions
    for(i = 0 ; i < rep->r_nb ; i++) {
        for(j = 0 ; j < nbtmp ; j++) {
            for(k = 0 ; k < nbfils ; k++) {
                cls = COL_add_col(cls);
                cl = cls->cl_cols + cls->cl_nb - 1;
                memcpy(cl, tmp + j, sizeof(COL));
                memcpy(&(cl->cl_per[0]),
                       PER_addper(&(tmp[j].cl_per[0]), i * rep->r_incr),
                       sizeof(PERIOD));
                if(cl->cl_opy != COL_NOP && cl->cl_opy != COL_BASE)
                    memcpy(&(cl->cl_per[1]),
                           PER_addper(&(tmp[j].cl_per[1]), i * rep->r_incr),
                           sizeof(PERIOD));
                if(cl->cl_fnb[0] == 0)
                    COL_apply_fil(cl, fils->fl_fils + k);
                else
                    break;
            }
        }
    }

    // Shift
    if(shiftdir) {
        for(i = 0; i < cls->cl_nb; i++) {
            cl = cls->cl_cols + i;
            if(shiftval == 0) {
                if(shiftdir == COL_SHIFTR)
                    cl->cl_per[0].p_s = cl->cl_per[1].p_s = PER_nb(cl->cl_per[0].p_p);
                else
                    cl->cl_per[0].p_s = cl->cl_per[1].p_s = 1;
            }
            else {
                if(shiftdir == COL_SHIFTL) shiftval = -shiftval;
                per = PER_addper(cl->cl_per, shiftval);
                memcpy(cl->cl_per, per, sizeof(PERIOD));
                if(cl->cl_opy != COL_NOP) {
                    per = PER_addper(cl->cl_per + 1, shiftval);
                    memcpy(cl->cl_per + 1, per, sizeof(PERIOD));
                }
            }
        }
    }
    return(cls);
}


/**
 *  Recursive function to compile a GSAMPLE.
 *  Returns a COLS* struct containing the column definitions.
 *  
 *  @param [in, out] YYFILE*    yy  YY stream
 *  @return          COLS*          compiled columns
 *
 *  Example 1
 *  ---------
 *  Compile the GSAMPLE 
 *      "2000:5[1;2];2000/1999:2"
 *  
 *  There are 2 sub GSAMPLE's separated by a ';'
 *      "2000:5[1;2]" 
 *      "2000/1999:2"
 *  
 *  First, COL_read_cols() will read the string until reaching ';' or the end of the string, in this case "2000:5[1;2]".
 *  It will create during the process:
 *      - cltmp : period(s) + operation on period
 *      - fils  : file(s) + operation on files
 *      - rep   : repetition factor + increment
 *   
 *  Next, fils and rep having been read, COL_construct() is called to add 
 *  cltmp to the final COLS cls (which is still NULL a this moment). 
 *  In this case, cltmp is repeated 5 times (rep.r_nb) x 2 (fils->fl_nb) 
 *  and these columns are stored in cls.
 *  
 *  Next, the remaining string "2000/1999;5" is compiled and added to cls the same way.
 *  This time however, there are only 2 new columns added to cltmp (1 file x 2 periods). 
 *       
 *  The final result contains 7 columns.
 *  
 *  Example 2
 *  ---------
 *  GSAMPLE : "(99Y1;2000Y1;2000Y1/1999Y1)[1;2]:2"
 *  
 *  In this case, COL_read_cols() is called recursively and the total number 
 *  of columns will be 3 x 2 x 2 = 12.
 *  
 *  Example 3
 *  ---------
 *  GSAMPLE : "(99:3;2000/99:3)[1;2]:2"
 *  
 *  Again, COL_read_cols() is called recursively and the total number 
 *  of columns will be (3 + 3) x 2 x 2 = 24.
 *  
 */
 
static COLS *COL_read_cols(YYFILE* yy)
{
    COLS    *cls = 0, *cltmp = 0;
    int     key, sign = 1, shiftdir = 0, shiftval;
    FILS    *fils;
    REP     rep;

    cltmp = cls = 0;
    rep.r_nb = 0;
    fils = 0;

    while(1) {
        key = YY_lex(yy);
        switch(key) {
            case YY_LONG :
            case COL_EOS:   // JMP 21-05-2012
            case COL_BOS:   // JMP 21-05-2012
            case COL_NOW:   // JMP 22-05-2012
            case COL_EOS1:   // JMP 21-05-2012
            case COL_BOS1:   // JMP 21-05-2012
            case COL_NOW1:   // JMP 22-05-2012
                if(cltmp != 0) goto err;
                YY_unread(yy);
                cltmp = COL_read_y(yy);
                if(cltmp == 0) goto err;
                break;

            case COL_OPAR :
                if(cltmp != 0) goto err;
                cltmp = COL_read_cols(yy);
                if(cltmp == 0) goto err;
                break;

            case COL_COLON :
                if(cltmp == 0) goto err;
                if(rep.r_nb > 0) goto err;
                if(COL_read_rep(yy, &rep)) goto err;
                break;

            case COL_OBRACK :
                if(cltmp == 0) goto err;
                if(fils != 0) goto err;
                fils = COL_read_f(yy);
                if(fils == 0) goto err;
                break;

            case COL_COMMA :
            case COL_CPAR :
            case YY_EOF :
                if(cltmp == 0) goto err;
                //cls = COL_construct(cls, cltmp, fils, &rep, shiftdir, shiftval);
                cls = COL_construct(cls, cltmp, fils, &rep, 0, 0);
                COL_free_fils(fils);
                COL_free_cols(cltmp);
                fils = 0;
                rep.r_nb = 0;
                cltmp = 0;
                shiftdir = 0;
                if(key != COL_COMMA) return(cls);
                break;

            case COL_SHIFTL:
            case COL_SHIFTR:
                if(cltmp == 0) goto err;
                shiftval = COL_read_long_per(yy);
                if(shiftval < 0) goto err;
                shiftdir = key;
                COL_shift(cltmp, shiftdir, shiftval);
                break;

            default :
                goto err;
        }
    }

err:
    COL_free_cols(cls);
    COL_free_cols(cltmp);
    COL_free_fils(fils);
    return((COLS *)0);
}


/**
 *  GSAMPLE compiler.
 *  
 *  @param [in]     char*   gsample text representing a GSAMPLE 
 *  @return         COLS*           group of COL struct result of the compilation of gsample
 *  
 */
 
COLS *COL_cc(char* gsample)
{
    COLS    *cls, *COL_read_cols();
    COL     *cl;
    YYFILE  *yy;
    static  int sorted = 0;
    int     i;

    YY_CASE_SENSITIVE = 1;
    if(sorted == 0) {
        qsort(COL_KEYWS, sizeof(COL_KEYWS) / sizeof(YYKEYS), sizeof(YYKEYS), YY_strcmp);
        sorted = 1;
    }
    yy = YY_open(gsample, COL_KEYWS,
                 sizeof(COL_KEYWS) / sizeof(YYKEYS), YY_MEM);
    if(yy == 0) return((COLS *)0);
    cls = COL_read_cols(yy);
    YY_close(yy);
    
    // When no file is specified in a COL, force file nb 1 (i.e. workspace) and no operation on files
    if(cls != 0)
        for(i = 0, cl = cls->cl_cols ; i < cls->cl_nb ; i++, cl++) {
            if(cl->cl_fnb[0] != 0) continue;
            cl->cl_fnb[0] = 1;
            cl->cl_fnb[1] = 0;
            cl->cl_opf = COL_NOP;
        }

    return(cls);
}


/**
 *  Frees the allocated space for a COLS structure created by COL_cc().
 *  
 *  @param [in] COLS*   cls     COLS to free
 *  @return     int             0
 *  
 */
 
int COL_free_cols(COLS* cls)
{
    if(cls == 0) return(0);
    if(cls->cl_nb != 0) SW_nfree(cls->cl_cols);
    SW_nfree(cls);
    return(0);
}


/**
 *  Analyses the COLS struct cls and set 1 in the vector mode (normally KT_mode) 
 *  for each operation found in cls. Only the operations between COL_DIFF and COL_BASE 
 *  are saved. 
 *  
 *  The aim is to prepare the texts to be printed below a table in the "MODE" TLINE's.
 *  
 *  @note The same vector is used for the operations on periods and on files (TODO: check why)
 *  
 *  @param [in]      COLS*   cls     compiled GSAMPLE
 *  @param [in, out] int*    mode    boolean vector containing a 1 for each operation found in cls (COL_DIFF, COL_GRT...)
 *  @param [in]      int     type    0: search in period operations, 
 *                                   1: search in file operations
 *                                   2: search in both
 *  @return          int             0 if no operation found, 1 else
 *  
 */
 
int COL_find_mode(COLS* cls, int* mode, int type)
{
    COL   *cl;
    int    i, nb = 0;

    memset(mode, 0,  MAX_MODE * sizeof(int));
    for(i = 0; i < cls->cl_nb; i++) {
        cl = cls->cl_cols + i;
        if(type % 2 == 0) { /* 0 ou 2 ==> years */
            if(cl->cl_opy >= COL_DIFF && cl->cl_opy <= COL_BASE)
                mode[cl->cl_opy - COL_DIFF] = 1;
            nb = 1;
        }

        if(type > 0) { /* 1 ou 2 ==> files */
            if(cl->cl_opf >= COL_DIFF && cl->cl_opf <= COL_BASE)
                mode[cl->cl_opf - COL_DIFF] = 1;
            nb = 1;
        }
    }
    return(nb); /* nb = 0 : pas d'op�ration, = 1 : ops */
}
