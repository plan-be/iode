/* MACROS */

&&TI /* TITRE */
..par1 parb
;&
&&

&&TI2 /* TITRE */
..par1 parb2
;&
&&

&&IT /* TITRE ITALIC */
..par1 pari
;&
&&

&&NI /* FONCTION NON INTERACTIVE */
..par1 fnnint
    ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
    ³Fonction non interactive³
    ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ

&&

&&IN /* FONCTION INTERACTIVE */
..par1 fnint
      ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
      ³Fonction interactive³
      ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ

&&

&&DE
..par1 parb
Description
ÄÄÄÄÄÄÄÄÄÄÄ
&&

&&CO /* COURRIER */
..par cmd_1
..lf_on
..bl_on
&&

&&CO2 /* COURRIER */
..par cmd_2
..lf_on
..bl_on
&&

&&TX  /* TEXTE */
..lf_off
..bl_off
..par par_1
&&

&&EN /* ENUM */
..par1 enum_1
;  - &
&&

&&EN2 /* ENUM2 */
..par1 enum_2
;      * &
&&

&&SY /* SYNTAX */
&TI Syntaxe
ÄÄÄÄÄÄÄ
&CO
&&

&&SY2 /* SYNTAX ITALIC*/
&IT Syntaxe
ÄÄÄÄÄÄÄ
&CO
&&

&&FC /* FUNCTION C */
&IT Fonction C
ÄÄÄÄÄÄÄÄÄÄ
&CO
&&

&&REM /* REMARQUE */
&TI Remarque
ÄÄÄÄÄÄÄÄ
&&

&&RT /* RETURN VALUE */
&TI Valeur retourn‚e
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
&&

&&NO /* Note */
..par1 note
;NOTE
;ÄÄÄÄ
&&

&&PO /* Portabilit‚ */
&TI Portabilit‚
ÄÄÄÄÄÄÄÄÄÄÄ
&&

&&SA  /* SEE ALSO */
&TI Voir ‚galement
ÄÄÄÄ
&&

&&EX /* EXEMPLE */
&TI Exemple
ÄÄÄÄÄÄÄ
&CO
&&

&&EX2 /* EXEMPLE ITALIC */
&IT Exemple
ÄÄÄÄÄÄÄ
&CO
&&

&&EEX /* FIN D'EXEMPLE */
&TX
&&

&&NFS /* FONCTION NON INTERACTIVE */
&TI Fonction non interactive
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
&&

&&FS /* FONCTION INTERACTIVE */
&TI Fonction interactive
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
&&

&&MU
Cette fonction est d‚crite dans le "Manuel de l'utilisateur".
&&

&&NEQ
Il n'y a pas de fonction ‚quivalente interactive.
&&

&&EXTWS
Xxx prend l'une des valeurs :
&CO
    cmt | eqs | idt | lst | scl | tbl | var
&TX
&&

&&EXTF
Xxx prend l'une des valeurs :
&CO
    cmt,    eqs,    idt,    lst,    scl,    tbl,    var,
    ac,     ae,     ai,     al,     as,     at,     av,
    rep,    a2m,    agl,    prf,    dif,    mif,    rtf,
    ps,     asc,    txt,
&TX
&&

&&ALIAS /* .topicalias */
..topicalias &
&&

&&TCASE /* .topiclink case=no */
..topiclink case=no
&&

&&TCASEY /* .topiclink case=no */
..topiclink case=yes
&&

&&TPART /* .topiclink partial=yes */
..topiclink partial=yes
&&





