#include "s_strs.h"

#define lettre(x)   (SCR_is_anum(x) || U_is_in(x, chars))

/*NH*/
unsigned char *SCR_replace_gnl2(txt, repl, by)
unsigned char   *txt, *repl, *by;
{
    int     lgrepl = (int)strlen(repl),
            lgby = (int)strlen(by);
    U_ch    *txt1 = txt, *txt2 = txt;

    if(txt == 0) return(txt);

    while(1) {
        if(memcmp(txt2, repl, lgrepl)) {
            *txt1 = *txt2;
            if(*txt1 == 0) break;
            txt1++;
            txt2++;
            continue;
        }
        memcpy(txt1, by, lgby);
        txt1 += lgby;
        txt2 += lgrepl;
    }
    return(txt);
}

/*NH*/
unsigned char *SCR_replace_gnl(txt, repl, by, chars)
unsigned char   *txt, *repl, *by, *chars;
{
    int     pos, shift = 0,
                 lgrepl = (int)strlen(repl),
                 lgby = (int)strlen(by),
                 lgtxt;

    if(txt == 0) return(txt); /* JMP 13-03-03 */

    if(lgrepl > lgby && chars == 0)
        return(SCR_replace_gnl2(txt, repl, by)); /* JMP 03-10-2003 */

    lgtxt = (int)strlen(txt);
    while(1) {
        pos = U_index(txt + shift, repl);
        if(pos < 0) return(txt);
        shift += pos;
        if(chars) {
            if((shift > 0 && lettre(txt[shift - 1])) ||  /* JMP 18-05-99 */
                    lettre(txt[shift + lgrepl])) {
                shift += lgrepl;
                continue;
            }
        }

        if(lgrepl >= lgby) {
            //strcpy(txt + shift + lgby, txt + shift + lgrepl);
            lgtxt -= pos + lgrepl;
            memcpy(txt + shift + lgby, txt + shift + lgrepl, lgtxt);
            txt[shift + lgtxt + lgby] = 0;
        }
        else
            SCR_strrcpy(txt + shift + lgby, txt + shift + lgrepl);
        memcpy(txt + shift, by, lgby);
        shift += lgby;
    }
}

/* ====================================================================
Calcule la longueur du string résultant du remplacement de toutes les
occurences du string repl par le string by dans txt.

&RT longueur nécessaire

&SA U_index(), SCR_areplace()
=======================================================================*/

SCR_replace_len(txt, repl, by, chars)
unsigned char   *txt, *repl, *by, *chars;
{
    int     pos, shift = 0, add = (int)strlen(txt),
                 lgrepl = (int)strlen(repl),
                 lgby = (int)strlen(by);

    while(1) {
        pos = U_index(txt + shift, repl);
        if(pos < 0) break;
        shift += pos;
        if(chars && shift > 0 &&
                (lettre(txt[shift - 1]) || lettre(txt[shift + lgrepl]))) {
            shift += lgrepl;
            continue;
        }
        add += lgby - lgrepl;
        shift += lgrepl;
    }
    return(add);
}

/*NH*/
unsigned char *SCR_areplace_gnl(txt, repl, by, chars)
unsigned char   *txt, *repl, *by, *chars;
{
    int     len, lgtxt;
    U_ch    *res;

    lgtxt = (int)strlen(txt);
    len = SCR_replace_len(txt, repl, by, chars);
    if(len < lgtxt) len = lgtxt;
    res = SCR_malloc(len + 1);
    strcpy(res, txt);
    return(SCR_replace_gnl(res, repl, by, chars));
}

/* ====================================================================
Remplace toutes les occurences du string repl par le string by dans
txt. Le résultat est alloué dans la fonction. txt est inchangé.

&RT le pointeur vers le string alloué.

&EX
    char    buf[100];

    txt = SCR_areplace("AAAAA", "AA", "B");
    printf("%s\n", txt);
    SCR_free(txt);

    donne :

    "BBA"
&TX
&SA U_index(), SCR_replace_name()
=======================================================================*/
unsigned char *SCR_areplace(txt, repl, by)
unsigned char   *txt, *repl, *by;
{
    return(SCR_areplace_gnl(txt, repl, by, 0L));
}

/* ====================================================================
Remplace toutes les occurences du mot repl par le string by dans
txt. Le résultat est alloué dans la fonction. txt est inchangé.

&RT le pointeur vers le string alloué.

&EX
    char    buf[100];

    txt = SCR_areplace_word("AA+B+ln(AA)", "AA", "X");
    printf("%s\n", txt);
    SCR_free(txt);

    donne :

    "X+B+ln(X)"
&TX
&SA U_index(), SCR_areplace()
=======================================================================*/
unsigned char *SCR_areplace_word(txt, repl, by)
unsigned char   *txt, *repl, *by;
{
    return(SCR_areplace_gnl(txt, repl, by, "_"));
}

/* ====================================================================
Remplace toutes les occurences du mot repl par le string by dans le
string txt. txt doit contenir assez d'espace pour les modifications
apportées par le remplacement.

Les seuls replacements sont ceux pour lesquels repl est entourés de
caractères autres que les lettres, les chiffres et le caractère '_'.

&RT le pointeur vers le string txt

&EX
    char    buf[100];

    strcpy(buf, "AAAA");
    SCR_replace_word(buf, "AA", "B");
    printf("%s\n", buf);
    strcpy(buf, "AA");
    SCR_replace_word(buf, "AA", "A");
    printf("%s\n", buf);
    strcpy(buf, "AB+ABC+AB_C+AB/2");
    SCR_replace_word(buf, "AB", "XXXX");
    printf("%s\n", buf);

    donne :

    "AAAA"
    "A"
    "XXXX+ABC+AB_C+XXXX/2");
&TX
&SA U_index(), SCR_replace()
=======================================================================*/

unsigned char *SCR_replace_word(txt, repl, by)
unsigned char   *txt, *repl, *by;
{
    return(SCR_replace_gnl(txt, repl, by, "_"));
}

/* ====================================================================
Remplace toutes les occurences du string repl par le string by dans le
string str. str doit contenir assez d'espace pour les modifications
apportées par le remplacement.

&RT le pointeur vers le string str.

&EX
    char    buf[100];

    strcpy(buf, "AAAA");
    SCR_replace(buf, "AA", "B");
    printf("%s\n", buf);
    strcpy(buf, "AAAA");
    SCR_replace(buf, "AA", "A");
    printf("%s\n", buf);

    donne :

    "BB"
    "AA"
&TX
&SA U_index(), SCR_replace_name()
=======================================================================*/
unsigned char *SCR_replace(txt, repl, by)
unsigned char   *txt, *repl, *by;
{
    return(SCR_replace_gnl(txt, repl, by, 0L));
}





