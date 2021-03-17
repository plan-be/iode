#include "iode.h"
//#include <s_swap.h>

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

FIL     COL_FIL  = {0, 0, 0};
FILS    COL_FILS = {1, &COL_FIL};

char    *COL_OPERS[] = {
    "",
    "-",
    "--",
    "^",
    "/",
    "//",
    "+",
    "=",
    "<", // Faux !! Ne peut pas se produire
    ">"  // Faux !! Idem
};

#define COL_add_c(cls, n)   ((COLS *)COL_add(cls,  n, sizeof(COL)))
#define COL_add_f(fils, n)  ((FILS *)COL_add(fils, n, sizeof(FIL)))

#define COL_free_c(cls)     COL_free(cls)
#define COL_free_f(fils)    COL_free(fils)

/*
    Compile  les  d‚finitions  de  colonnes. Cette d‚fintion
    contient   une   liste   de   p‚riodes,   ‚ventuellement
    affectu‚es   d'une   op‚ration   (taux   de  croissance,
    diff‚rence,  ...)  appliqu‚es … des fichiers repr‚sent‚s
    ici  par  des  num‚ros. Chaque p‚riode est s‚par‚e de la
    suivante  par  une virgule. Une colonne ou un groupe des
    colonnes  peut  ˆtre r‚p‚t‚e : il suffit de placer aprŠs
    la  d‚finition de la colonne ou du groupe de colonnes le
    caractŠre : suivi du nombre de r‚p‚titions souhait‚.

    La d‚finition des fichiers est optionnelle et est plac‚e
    entre crochets. Elle s'applique … toute la d‚finition de
    p‚riode  qui  pr‚cŠde.  Les op‚rations possibles sur les
    fichiers sont :

	- valeur absolue                        (1)
.
	- diff‚rence                            (1-2)
.
	- diff‚rence en pourcents               (1/2)
.
	- somme                                 (1+2)
.
	- moyenne                               (1^2)

    Les op‚rations sur les p‚riodes sont :

	- valeur (75)
.
	- taux de croissance sur une ou plusieurs p‚riodes (75/74, 75/70)
.
	- taux de croissance moyen (75//70)
.
	- diff‚rence (75-74, 75-70)
.
	- diff‚rence moyenne (75--70)
.
	- moyenne (75^74)
.
	- somme de p‚riode … p‚riode cons‚cutives (70Q1+70Q4)
.
	- valeur en indice ou en base (76=70)

    La   r‚p‚tition   peut  s'effectuer  avec  un  incr‚ment
	sup‚rieur … 1 ou < 0: il suffit de placer une ‚toile suivie du
    pas  aprŠs  le  nombre  de r‚p‚titions (70:3*5 = 70, 75,
    80).

	21-05-2012 : La   r‚p‚tition   peut  s'effectuer  avec  un  incr‚ment negatif.
	Dans ce cas, l'affichage se fait a l'envers (2000:3*-1 = 2000, 1999, 1998, ...)
	
	

    Exemples :

	    70, 75, 80:6  = 70:3.5, 81:5 =
			    70, 75, 80, 81, 82, 83, 84, 85

	    70/69:2       = 70/69, 71/70

	    (70, 70-69):2 = 70, 70-69, 71, 71-70,

	    70[1,2]:2*5   = 70[1], 70[2], 75[1], 75[2]

	    (70,75){1,2-1} = 70[1],75[1],70[2-1],75[2-1]

	    (70,75,(80, 80/79):2)[1,2] = 70[1], 70[2], 75[1], 75[2],
					 80[1], 80[2], 80/79[1], 80/79[2]
					 81[1], 81[2], 81/80[1], 81/80[2]


    La fonction COL_free_c(COLS *) libŠre l'espace utilis‚.
    La fonction COL_ctoa(COL *, char) retourne un string avec le texte
    correspondant au calcul. (s : sample, f : file, t : time)

*/


COLS *COL_cc(string)
char    *string;
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
    yy = YY_open(string, COL_KEYWS,
                 sizeof(COL_KEYWS) / sizeof(YYKEYS), YY_MEM);
    if(yy == 0) return((COLS *)0);
    cls = COL_read_cols(yy);
    YY_close(yy);
    if(cls != 0)
        for(i = 0, cl = cls->cl_cols ; i < cls->cl_nb ; i++, cl++) {
            if(cl->cl_fnb[0] != 0) continue;
            cl->cl_fnb[0] = 1;
            cl->cl_fnb[1] = 0;
            cl->cl_opf = COL_NOP;
        }

    return(cls);

}
/*
    Frees the column definition structure created by COL_cc().
*/

int COL_free(cls)
COLS    *cls;
{
    if(cls == 0) return(0);
    if(cls->cl_nb != 0) SW_nfree(cls->cl_cols);
    SW_nfree(cls);
    return(0);
}

/*
    Formatte une colonne dans le format standard :
    yyyyPpp op yyyyPpp [ filenb op filenb ]

*/
COL_stripy(buf)
char    *buf;
{
    char        *ptr;

    ptr = strchr(buf, 'Y');
    if(ptr != NULL) *ptr = 0;
    return(0);
}

/*
char    *COL_ctoa(cl, ch, n)
COL     *cl;
int     ch, n;
{
    static char buf[30];
    char        buf1[20], buf2[20], buft[20], bufT[20], bufp[20], bufP[20],
		*opt, buff[20];

    if(cl->cl_opf != COL_NOP)
	sprintf(buff, "[%d%s%d]", cl->cl_fnb[0], COL_OPERS[cl->cl_opf - COL_NOP],
				  cl->cl_fnb[1]);
    else
	sprintf(buff, "[%d]", cl->cl_fnb[0]);

    PER_pertoa(&(cl->cl_per[0]), buf1);
    COL_stripy(buf1);
    if(cl->cl_opy != COL_NOP) {
	PER_pertoa(&(cl->cl_per[1]), buf2);
	COL_stripy(buf2);
	opt = COL_OPERS[cl->cl_opy - COL_NOP];
	sprintf(buft, "%s-%s(%s)", buf1, buf2, opt);
	sprintf(bufT, "%s%s%s", buf1, opt, buf2);
	strcpy(bufp, buf1);
	sprintf(bufP, "%s-%s", buf1, buf2);
	}
    else {
	strcpy(buft, buf1);
	strcpy(bufT, buf1);
	strcpy(bufp, buf1);
	strcpy(bufP, buf1);
	}

    switch(ch) {
	case 't' : strcpy(buf, buft); break;
	case 'T' : strcpy(buf, bufT); break;
	case 'p' : strcpy(buf, bufp); break;
	case 'P' : strcpy(buf, bufP); break;
	case 'f' :
	case 'F' : strcpy(buf, buff); break;
	case 'r' : sprintf(buf, "%s%s", bufp, buff); break;
	case 'R' : sprintf(buf, "%s%s", bufP, buff); break;
	case 'S' : sprintf(buf, "%s%s", bufT, buff); break;
	case 's' :
	default  : sprintf(buf, "%s%s", buft, buff); break;
	}
    return(buf);
}
*/
char    *COL_ctoa(cl, ch, n, nbf)
COL     *cl;
int     ch, n, nbf;
{
    static char res[30];
    PERIOD      *per;
    extern int  B_LANG;
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
                strcpy(res, KLG_MONTHS[per->p_s -1][B_LANG]);
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
                strcpy(res, KLG_OPERS_TEXTS[cl->cl_opy - COL_NOP][B_LANG]);
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

char    *COL_text(cl, str, nbnames)
COL     *cl;
char    *str;
int     nbnames;
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
/*NH*/
COLS *COL_add(cls, n, size)
COLS    *cls;
int     n, size;
{
    if(cls == 0) {
        cls = (COLS *) SW_nalloc(size);
    }
    if(cls->cl_nb % 20 == 0)
        cls->cl_cols = (COL *) SW_nrealloc(cls->cl_cols,
                                           size * cls->cl_nb,
                                           size * (cls->cl_nb + 25));
    cls->cl_nb += n;
    return(cls);
}


/*NH*/
// Termine la construction d'un groupe de période en les répétant (:n*m) et en appliquant les shifts

COLS *COL_construct(cls, cltmp, fils, rep, shiftdir, shiftval)
COLS    *cls, *cltmp;
FILS    *fils;
REP     *rep;
int     shiftdir, shiftval;
{
    int     i, nbtmp, j, nbfils, k;
    COL     *cl, *tmp;
    PERIOD  *per;

    if(fils == 0) fils = &COL_FILS;
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
                cls = COL_add_c(cls, 1);
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
/*NH*/
COL_apply_fil(cl, fl)
COL *cl;
FIL *fl;
{
    if(cl->cl_fnb[0] != 0) return(0);
    cl->cl_fnb[0] = fl->fl_1;
    cl->cl_fnb[1] = fl->fl_2;
    cl->cl_opf = fl->fl_op;
    return(0);
}

/*NH*/
COL_calc_now(PERIOD *per) {
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

/*NH*/
COL_calc_subper() 
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

/*NH*/
// JMP 21-05-2012 : nouvelle syntaxe avec periode<n ou periode>n
COL_read_per(yy, per)
YYFILE  *yy;
PERIOD  *per;
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
    
    // 3. <n ou >n avec possible répétition (<0>1 ou <0>N comptent pour la seule période courante)
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

    // Lit la suite des <> s'il y en a pour pouvoir écrire 2000Q5<0>1/2000Q5<0 => 2000Q2/2000Q1
    YY_skip_spaces(yy);
    ch	= YY_getc(yy); // Lit la suite
    goto nextshift;
        
    return(0);
}

/*NH*/
COLS *COL_read_y(yy)
YYFILE  *yy;
{
    PERIOD  per;
    COLS    *cls;
    int     op;

    if(COL_read_per(yy, &per)) return((COLS *)0);
    cls = COL_add_c((COLS *)0, 1);
    cls->cl_cols[0].cl_opy = COL_NOP;
    memcpy(&(cls->cl_cols[0].cl_per[0]), &per, sizeof(PERIOD));

    op = YY_lex(yy);
    if(op <= COL_NOP || op >= COL_LAST_OP) {
        YY_unread(yy);
        return(cls);
    }

    cls->cl_cols[0].cl_opy = op;

    if(COL_read_per(yy, &per)) {
        COL_free(cls);
        return((COLS *)0);
    }
    memcpy(&(cls->cl_cols[0].cl_per[1]), &per, sizeof(PERIOD));
    return(cls);
}

/*NH*/
// Lit un long ou P ou PER et retourne la valeur ou -1 en cas d'erreur
COL_read_long_per(YYFILE *yy)
{
    int     keyw = YY_lex(yy);
    
    if(keyw == YY_LONG)         return(yy->yy_long); 
    else if(keyw == COL_PER)    return(PER_nb(KSMPL(KV_WS)->s_p1.p_p));
    else if(keyw == COL_SUBPER) return(COL_calc_subper());
    else                        return(-1);
}
/*NH*/
COL_read_rep(yy, rep)
YYFILE  *yy;
REP     *rep;
{
    int	sign = 1; // JMP 21-05-2012
    int	keyw;

    rep->r_incr = 1;

    // Syntax : [:n[*[-]incr]]
    // 1. partie (optionnelle) n
    rep->r_nb = COL_read_long_per(yy);
    if(rep->r_nb < 0) return(-1);

    /*
    keyw = YY_lex(yy);
    if(keyw == YY_LONG) {
        if(yy->yy_long > 500) {
            B_seterrn(75);
            return(-1);
        }
        rep->r_nb = yy->yy_long;
    }    
    else if(keyw == COL_PER) 
        rep->r_nb = PER_nb(KSMPL(KV_WS)->s_p1.p_p);
    else    
        return(-1);
    */
    
    // 2. partie optionnelle :[-]incr
    if(YY_lex(yy) != COL_DOT) {
        YY_unread(yy);
        return(0);
    }

    
    keyw = YY_lex(yy);
    // incrément négatif si -
    if(keyw == COL_DIFF) 
        sign = -1;
    else 
        YY_unread(yy);
    
    rep->r_incr = COL_read_long_per(yy);
    if(rep->r_incr < 0) return(-1);
    rep->r_incr *= sign; 
    
    /*
    if(keyw == YY_LONG) 
        rep->r_incr = sign * yy->yy_long; 	// JMP 21-05-2012
    else if(keyw == COL_PER) 
        rep->r_incr = sign * PER_nb(KSMPL(KV_WS)->s_p1.p_p);
    else    
        return(-1);
    */
    return(0);
}

COL_read_1f(yy, fil)
YYFILE  *yy;
FIL     *fil;
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

/*NH*/
FILS *COL_read_f(yy)
YYFILE  *yy;
{
    FILS    *fils = 0, fil;

    while(1) {
        fils = COL_add_f(fils, 1);
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
    COL_free_f(fils);
    return((FILS *)0);
}

/*NH*/
COL_shift(COLS *cls, int key, int nb)
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

/*NH*/
COLS *COL_read_cols(yy)
YYFILE  *yy;
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
                COL_free_f(fils);
                COL_free_c(cltmp);
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
    COL_free_c(cls);
    COL_free_c(cltmp);
    COL_free_f(fils);
    return((COLS *)0);
}


COL_find_mode(cls, mode, type)
COLS    *cls;
int     *mode, type;
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
    return(nb); /* nb = 0 : pas d'op‚ration, = 1 : ops */
}







