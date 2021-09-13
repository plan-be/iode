#include "scr.h"


/*NH*/
/* =+======== SCR_ISF_TO_FLD ======================================
    Copy an ISAM field to a page field (formatted)
        pg     : IO : pointer to the page
        fld_nb : IN : field number
        is     : IN : pointer to the isam selected
        isf_nb : IN : isam field pointer
        return : 0 or error code if field not linked
    =============================================================== */

SCR_isf_to_fld(pg, fld_nb, is, isf_nb)
PAGE    *pg;
int     fld_nb;
ISAM    *is;
int     isf_nb;
{
    FIELD   *fld;
    ISFIELD *isf;
    long    l;
    double  d;
    short   s;
    float   f;
    unsigned char    c;
    MENU    *mn;
    char    **ptr, *ptr2, *ptr3;

    PG_read_flds(pg);
    fld = pg->pg_flds + fld_nb;
    isf = is->is_flds + isf_nb;

    if(isf->isf_var) {
        switch(fld->fld_type) {
            case SCR_EDITOR:
                IS_get_vfield(is, isf_nb, &ptr2);
                if(ptr2 != 0)
                    ptr3 = SCR_stracpy(ptr2);
                else
                    ptr3 = SCR_stracpy("\n");
                SCR_set_field_editor(pg, fld_nb, ptr3);
                break;

            default:
                IS_get_vfield(is, isf_nb, &ptr2);
                if(ptr2 != 0L)
                    PG_set_rec(pg, fld_nb, ptr2);
                else
                    PG_set_dft_field(pg, fld_nb);
                break;
        }
        return(0);
    }
    switch(isf->isf_type) {
        case SCR_ZSTRING :
        case SCR_STRING :
        case SCR_DOUBLE :
        case SCR_TYPE_RADIO: /* BP_M 21-01-97 */
        case SCR_CHECK_BOX : /* BP_M 19-01-97 */
str:
                PG_set_rec(pg, fld_nb, is->is_rec + isf->isf_start);
                break ;

        case SCR_DATE :
        case SCR_TIME :
        case SCR_AUTO :
        case SCR_REF  :
        case SCR_LONG :
                IS_get_field(is, isf_nb, (char *)&l);
                switch(fld->fld_type) {
                    case SCR_TYPE_MENU:
                    case SCR_TYPE_RADIO : /* JMP 25-10-01 */
                    case SCR_CHECK_BOX :  /* JMP 25-10-01 */
                        SCR_set_field_short(pg, fld_nb, (short)l);
                        break;
                    default:
                        SCR_set_field_long(pg, fld_nb, l);
                        break;
                }
                break;

        case SCR_SHORT :
                IS_get_field(is, isf_nb, (char *)&s);
                switch(fld->fld_type) {
                    case SCR_TYPE_MENU:
                    case SCR_TYPE_RADIO : /* JMP 25-10-01 */
                    case SCR_CHECK_BOX :  /* JMP 25-10-01 */
                        SCR_set_field_short(pg, fld_nb, s);
                        break;
                    default:
                        SCR_set_field_long(pg, fld_nb, (long)s);
                        break;
                }
                break;

        case SCR_CHAR   :
        case SCR_SHORT1 :
                IS_get_field(is, isf_nb, &c);
                l = c;
                s = c;
                switch(fld->fld_type) {
                    case SCR_NATURAL:
                    case SCR_INTEGER:
                        PG_set_rec(pg, fld_nb, (char *)&l);
                        break;
                    case SCR_TYPE_MENU:
                        PG_set_rec(pg, fld_nb, (char *)&s);
                        break;
                    default:
                        goto str;
                }
                break;

        case SCR_FLOAT :
                memcpy(&f, is->is_rec + isf->isf_start, sizeof(float));
                d = (double)f;
                PG_set_rec(pg, fld_nb, (char *)&d);
                break ;

        }

    return(0);
}
/*NH
/* =+======== SCR_FLD_TO_ISF ======================================
    Copy a PAGE field to an ISAM field
        pg     : IO : pointer to the page
        fld_nb : IN : field number
        is     : IN : pointer to the isam selected
        isf_nb : IN : isam field pointer
        return : 0 or error code if field not linked
    =============================================================== */

SCR_fld_to_isf(pg, fld_nb, is, isf_nb)
PAGE    *pg;
int     fld_nb;
ISAM    *is;
int     isf_nb;
{
    short   bufs;
    long    bufl;
    float   buff;
    double  bufd;
    ISFIELD *isf;
    FIELD   *fld;
    char    text[SCR_MAX_FIELD_LENGTH + 1], **vptr, *txt;
    unsigned char c;
    short   s;

    PG_read_flds(pg);
    isf = is->is_flds + isf_nb;
    fld = pg->pg_flds + fld_nb;

    if(isf->isf_var) {
        switch(fld->fld_type) {
            case SCR_EDITOR:
                vptr = (char **)PG_get_text(pg, fld_nb);
                bufl = 0;  /* JMP 29-12-98 */
                txt = 0;   /* JMP 29-12-98 */
                if(vptr) { /* JMP 29-12-98 */
                    txt = vptr[0];                  /* JMP 28-12-98 */
                    if(txt) bufl = strlen(txt) + 1 ;/* JMP 29-12-98 */
                    }                               /* JMP 29-12-98 */
                IS_set_vfield(is, isf_nb, txt, bufl); /* BP_M 26-10-95 */
                break;
            case SCR_STRING:
            case SCR_ZSTRING:
                txt = SCR_malloc(isf->isf_length + 1);
                memcpy(txt, PG_get_text(pg, fld_nb), isf->isf_length);
                txt[isf->isf_length] = 0;
                SCR_strip(txt);
                if(strlen(txt) == 0) break;
                IS_set_vfield(is, isf_nb, txt, (long)(strlen(txt) + 1));
                break;
            default:
                txt = PG_get_text(pg, fld_nb);
                IS_set_vfield(is, isf_nb, txt, (long)FLD_len_in_rec(pg->pg_flds + fld_nb));
                break;
        }
        return(0);
    }

    switch(isf->isf_type) {
        case SCR_TYPE_RADIO:
        case SCR_CHECK_BOX :
        case SCR_DOUBLE :
        case SCR_STRING :
        case SCR_ZSTRING :
            IS_set_field(is, isf_nb, PG_get_text(pg, fld_nb));
            break ;

        case SCR_CHAR:
        case SCR_SHORT1:
            switch(fld->fld_type) {
                case SCR_NATURAL:
                case SCR_INTEGER:
                    c = (unsigned char)SCR_get_field_long(pg, fld_nb);
                    IS_set_field(is, isf_nb, &c);
                    break;
                case SCR_TYPE_MENU:
                    c = SCR_get_field_short(pg, fld_nb);
                    IS_set_field(is, isf_nb, &c);
                    break;
                default:
                    IS_set_field(is, isf_nb, PG_get_text(pg, fld_nb));
                    break;
            }
            break;

        case SCR_DATE :
        case SCR_TIME :
        case SCR_AUTO   :
        case SCR_REF    :
        case SCR_LONG   :
            switch(fld->fld_type) {
                case SCR_TYPE_MENU:
                case SCR_TYPE_RADIO: /* JMP 25-10-01 */
                case SCR_CHECK_BOX : /* JMP 25-10-01 */
                    bufl = SCR_get_field_short(pg, fld_nb);
                    IS_set_field(is, isf_nb, (char *)&bufl);
                    break;
                default:
                    IS_set_field(is, isf_nb, PG_get_text(pg, fld_nb));
                    break;
            }
            break ;

        case SCR_SHORT :
            switch(fld->fld_type) {
                case SCR_NATURAL:
                case SCR_INTEGER:
                    s = SCR_get_field_long(pg, fld_nb);
                    IS_set_field(is, isf_nb, (char *)&s);
                    break;
                case SCR_TYPE_MENU:
                    s = SCR_get_field_short(pg, fld_nb);
                    IS_set_field(is, isf_nb, (char *)&s);
                    break;
                default:
                    IS_set_field(is, isf_nb, PG_get_text(pg, fld_nb));
                    break;
            }
            break ;

        case SCR_FLOAT :
            buff = (float)SCR_get_field_double(pg, fld_nb);
            IS_set_field(is, isf_nb, (char *)&buff);
            break ;

        }
    return(0);
}

/* ======================================================================
Copie dans les champs d'une PAGE ou des PAGES d'une TPAGES pg li‚s aux
champs de l'ISAM is les valeurs du record courant de l'ISAM.

Si pg est une TPAGES, toutes les sous-pages sont trait‚es.

Contrairement … la fonction SCR_isam_to_page(), cette fonction
n'applique pas r‚cursivement les CODEISAM d‚finis dans la PAGE ou les
TPAGES.

&SA SCR_isam_to_page()

------------------------------------------------------------------------ */

SCR_rec_to_page(pg, is)
PAGE    *pg;
ISAM    *is;
{
    TPAGES  *tpg;
    int     i;

    if(SCR_PTR_TYPE(pg) == SCR_TYPE_TPAGES) {
        tpg = (TPAGES *)pg;
        for(i = 0 ; i < tpg->tp_nb ; i++)
            SCR_rec_to_page_1((PAGE *)SCR_PTR(tpg->tp_pgsnbs[i]), is, 1);
        }
    else
        SCR_rec_to_page_1(pg, is, 1);

    return(0);
}

/*NH*/
SCR_rec_to_page_1(pg, is, flag_join)
PAGE    *pg;
ISAM    *is;
int     flag_join;
{
    FIELD   *fld;
    ISFLD   *isf;
    ISAM    *is2;
    int     i;
    int     j;
    extern  int     SCR_PG2IS_INPUT;

    PG_read_flds(pg);
    fld = pg->pg_flds;
    for(i = 0 ; i < pg->pg_nb_flds ; i++, fld++) {
        isf = fld->fld_isam;
        if(SCR_PG2IS_INPUT && fld->fld_io != 0) continue;
        for(j = 0 ; j < fld->fld_nb_isam ; j++, isf++) {
            is2 = (ISAM *)SCR_PTR(isf->if_isam);
            if(is2 == is)
                SCR_isf_to_fld(pg, i, is, isf->if_isam_nb);
        }
    }
    if(flag_join)
        for(i = 0 ; i < is->is_nb_join ; i++) { /* BP_M 29-02-2008 */
            is2 = (ISAM *)SCR_PTR(is->is_join[i].is);
            SCR_rec_to_page_1(pg, is2, 0);
        }

    return(0);
}

/*NH BP_M 17-06-2001 17:29 */
SCR_rec_to_page_1fld(pg, is, nfld)
PAGE    *pg;
ISAM    *is;
int     nfld;
{
    FIELD   *fld;
    ISFLD   *isf;
    ISAM    *is2;
    int     j;
    extern  int     SCR_PG2IS_INPUT;

    PG_read_flds(pg);
    fld = pg->pg_flds + nfld;
    isf = fld->fld_isam;
    if(SCR_PG2IS_INPUT && fld->fld_io != 0) return;
    for(j = 0 ; j < fld->fld_nb_isam ; j++, isf++) {
        is2 = (ISAM *)SCR_PTR(isf->if_isam);
        if(is2 == is)
            SCR_isf_to_fld(pg, nfld, is, isf->if_isam_nb);
    }

    return(0);
}


/* ======================================================================
Copie dans les champs d'une PAGE pg li‚s aux champs de l'ISAM is les
valeurs du record courant de l'ISAM.

Les CODES (ou CODEISAM) d‚finis dans la PAGE sont appliqu‚s
r‚cursivement, de fa‡on … remplir correctement tous les champs de la
PAGE. Ainsi, la copie de l'ISAM is_client dans la PAGE pg_client qui
contient un champ du type :

&CO
    FIELD Prompt "Code postal : " ISAM is_client post CODE is_post code
    FIELD Prompt "Commune     : " ISAM is_post commune output
&TX

remplira le champ code postal, puis recherchera dans is_post le code en
question (CODE is_post code). L'ayant trouv‚, il effectuera une copie de
la commune dans le champ suivant (ISAM is_post commune).

&SA SCR_rec_to_page(), SCR_page_to_isam()
------------------------------------------------------------------------ */

SCR_isam_to_page(pg, is)
PAGE    *pg;
ISAM    *is;
{
    SCR_rec_to_page(pg, is);
    SCR_apply_all_cmp(pg);
    return(0);
}

/* ======================================================================
Copie dans le record de l'ISAM is tous les champs de la PAGE pg qui sont
li‚s … cet ISAM. Cette fonction peut ˆtre utilis‚e avant IS_write() ou
IS_rewrite().

&SA SCR_isam_to_page()
------------------------------------------------------------------------ */
SCR_page_to_isam(pg, is)
PAGE    *pg;
ISAM    *is;
{
    TPAGES  *tpg;
    int     i;

    if(SCR_PTR_TYPE(pg) == SCR_TYPE_TPAGES) {
        tpg = (TPAGES *)pg;
        for(i = 0 ; i < tpg->tp_nb ; i++)
            SCR_page_to_isam_1((PAGE *)SCR_PTR(tpg->tp_pgsnbs[i]), is);
        }
    else
        SCR_page_to_isam_1(pg, is);

    return(0);
}

/*NH*/
SCR_page_to_isam_1(pg, is)
PAGE    *pg;
ISAM    *is;
{
    FIELD   *fld;
    ISFLD   *isf;
    ISAM    *is2;
    int     i;
    int     j;

    PG_read_flds(pg);
    fld = pg->pg_flds;
    for(i = 0 ; i < pg->pg_nb_flds ; i++, fld++) {
        isf = fld->fld_isam;
        for(j = 0 ; j < fld->fld_nb_isam ; j++, isf++) {
            is2 = (ISAM *)SCR_PTR(isf->if_isam);
            if(is2 == is)
                SCR_fld_to_isf(pg, i, is, isf->if_isam_nb);
        }
    }
    return(0);
}


/* ======================================================================
Change dans le champ d'une PAGE le num‚ro du champ de l'ISAM qui lui est
li‚. Cette fonction est … utiliser avec pr‚caution. Notamment, la
PAGE doit garder ses champs en m‚moire pour que cette op‚ration puisse
fonctionner correctement (FLD_MEM).

Elle est par exemple utile si on veut faire d‚filer plusieurs ‚crans de
valeurs en se servant de la mˆme PAGE.
&EX
    ISAM is_client {
        ...

        FIELD float     NAME chaff0
        FIELD float     NAME chaff1
        FIELD float     NAME chaff2
        FIELD float     NAME chaff3
        FIELD float     NAME chaff4
        FIELD float     NAME chaff5
        FIELD float     NAME chaff6
        FIELD float     NAME chaff7
        FIELD float     NAME chaff8
        FIELD float     NAME chaff9
        ...
        }

    PAGE pg_client {
        ...
        FIELD ISAM is_client chaff0
        FIELD ISAM is_client chaff1
        FIELD ISAM is_client chaff2
        FIELD ISAM is_client chaff3
        FIELD ISAM is_client chaff4
        ....
        }

    En C :

        Shift(n)
        int n;
        {
            int i;

            for(i = 0 ; i < 5 ; i++) {
                if(n == 0)
                    FLD_change_isf(pg_client, i, 0, i);
                else
                    FLD_change_isf(pg_client, i, 0, i + 5);
                }
        }
&TX
&SA FLD_set_isam(), PG_set_isam()
------------------------------------------------------------------------ */

FLD_change_isf(pg, nb_fld, nb_isf, new_val)
PAGE    *pg;
int     nb_fld, nb_isf, new_val;
{
    FIELD   *fld;

    PG_read_flds(pg);
    fld = pg->pg_flds + nb_fld;
    if(fld->fld_nb_isam <= nb_isf) return(0);
    fld->fld_isam[nb_isf].if_isam_nb = new_val;
    return(0);
}

/* ======================================================================
Replace toutes les occurences de l'ISAM is1 par l'ISAM is2 dans la PAGE pg:
tous les champs li‚s … is1 le sont dor‚navant … is2.

Cette fonction est … utiliser avec pr‚caution. Notamment, la
PAGE doit garder ses champs en m‚moire pour que cette op‚ration puisse
fonctionner correctement (FLD_MEM). Sans cela, aprŠs utilisation de la
PAGE, les nouvelles valeurs sont perdues. Cependant, si cette op‚ration
a lieu dans la BEGIN_FN ou la DISPLAY_FN de la PAGE, cela fonctionne
correctement.

Il est clair que les 2 ISAM doivent ˆtre semblables, voire identiques au
niveau de leur structure et des types des champs qui les composent.

&SA FLD_set_isam()
------------------------------------------------------------------------ */

PG_set_isam(pg, is1, is2)
PAGE    *pg;
ISAM    *is1, *is2;
{
     /* PG_set_isam : BP_M 02-11-93 08:25 */
    int     i, j, isam1, isam2;
    FIELD   *fld;

    PG_read_flds(pg);
    fld = pg->pg_flds;
    isam1 = SCR_NU(is1);
    isam2 = SCR_NU(is2);

    if(pg->pg_isam == isam1) pg->pg_isam = isam2;
    for(i = 0 ; i < pg->pg_nb_flds ; i++, fld++)
        for(j = 0 ; j < fld->fld_nb_isam ; j++)
            if(fld->fld_isam[j].if_isam == isam1)
                fld->fld_isam[j].if_isam = isam2;

    return(0);
}

/* ==================================================================
V‚rifie que les champs de la PAGE pg correspondent … ceux du
record courant de l'ISAM is. Cette fonction permet de savoir si
lors de l'‚dition d'un record, celui-ci a ‚t‚ modifi‚.

&EX
    En C :
        AD_adr_quit()
        {
            if(SCR_LKEY != SCR_ESCAPE) return(0);
            if(SCR_page_eq_isam(p_client, is_client) == 0) return(0);
            return(SCR_confirme("Quitter sans sauver (O/N) ?"));
        }


    Utilisation :
        PAGE p_client {
            END_FN {C_FN return(AD_adr_quit());}
            DEFAULTS {ISAM is_client}
            ...
            }
&TX

&SA Action PAGE_EQ_ISAM

===================================================================== */

SCR_page_eq_isam(pg, is)
PAGE    *pg;
ISAM    *is;
{
    SCR_page_to_isam(pg, is);
    if(memcmp(is->is_rrec, is->is_rec, is->is_rec_len)) return(0);
    return(1);
}


/*NH*/
/* =+========== SCR_IS_TO_PG ========================================
    Store a record in a page. Execute all CODEISAM defined in the fields.
        pg     : IO : pointer to the page
        is     : IN : ISAM *
    Returns : error code or 0
    ===================================================================

SCR_is_to_pg(pg, is)
PAGE    *pg;
ISAM    *is;
{
    SCR_rec_to_page(pg, is);
    return(SCR_apply_all_cmp(pg));
}
JMP_M 4.19 25-05-95 */

// Num‚ro de l'isam de base de la page
int         SCR_BASE_IS_PAGE;
