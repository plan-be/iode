#if defined(DOSW32) || defined(SCRW32)
#include "scr4w.h"  /* JMP 06-01-98 */
#else
#include "scr.h"
#endif

extern PR_DEF  *PR_CURRENT ;
extern char    *PR_TITLE ;


/*NH*/
/* ===+==== PR_open =============================
    Open the current printer
    =========================================== */

PR_open()
{
    PR_DEF  *pr = PR_CURRENT;

    if(pr->pr_im != 0) return(0);

    /* OPEN PRINTER OF FILE */

#ifdef PRWINDOWS
    switch(pr->pr_type) {  /* JMP 19-08-98 */
	case 'F' :
	    if(SCR_cstrncmp(pr->pr_name, "LPT", 3) == 0 || /* JMP 25-04-97 */
	       SCR_cstrncmp(pr->pr_name, "PRN", 3) == 0)   /* JMP 25-04-97 */
		    pr->pr_fd = fopen(pr->pr_name, "r+"); /* JMP 25-04-97 */
	    else
		    pr->pr_fd = fopen(pr->pr_name, "w+");

	    if(pr->pr_fd == 0) {
		PR_error(pr);
		return(-1);
		}
	    break;

	case 'D' :
	    if(WscrPrintRawInit(pr->pr_name, PR_DOCNAME)) /* JMP 13-09-98 */
		return(-1);
	    break;

	default :
	    if(WprChInit(NULL, 0, PR_DOCNAME)) { /* JMP 23-08-98 */
		PR_error(pr);
		return(-1);
		}
	    break;

	}

#else
#ifdef DOS
    pr->pr_fd = fopen(pr->pr_name, "w+");
#else
    if(pr->pr_type == 'F')
	pr->pr_fd = fopen(pr->pr_name, "w+");
    else
/*        pr->pr_fd = popen(pr->pr_name, "w+"); /* JMP 19-02-96 */
	pr->pr_fd = popen(pr->pr_name, "w");   /* JMP 19-02-96 */
#endif /* DOS */

    if(pr->pr_fd == 0) {
	PR_error(pr);
	return(-1);
	}

#endif /* PRWINDOWS */

    /* SET PAGE NB */
    pr->pr_pgnb = 1;

    /* INITIALISE IMAGE */
    pr->pr_im = SCR_alloc_image(pr->pr_nl, pr->pr_nc);
    PR_clear();

    /* SEND INIT STRING */
#ifdef PRWINDOWS
    if(U_is_in(pr->pr_type, "FD") && pr->pr_begin != 0)
	PR_print_str(pr, pr->pr_begin);
#else
    if(pr->pr_begin != 0)
	PR_print_str(pr, pr->pr_begin);
#endif

    return(0);
}

/*NH*/
PR_clear()
{
    int i;

    for(i = 0 ; i < PR_CURRENT->pr_nl ; i++)
	PR_clear_line(i, 0);

    PR_goto(0, 0);
    return(0);
}

/*NH*/
PR_clear_line(line, col)
int     line, col;
{
    int     i, lg;
    unsigned char   *cells = 0;                 /* JMP 22-06-05 */

    if(PR_CURRENT && PR_CURRENT->pr_im)         /* JMP 22-06-05 */
	cells = PR_CURRENT->pr_im->im_cells;    /* JMP 22-06-05 */
    if(cells == 0) return(-1);                  /* JMP 22-06-05 */

    col = (col + line * PR_CURRENT->pr_nc) * 2;
    lg  = (line + 1) * PR_CURRENT->pr_nc * 2;
    for(i = col ; i < lg ; i++) {
	cells[i++] = ' ';
	cells[i] = SCR_DEFAULT;
	}

    return(0);
}

/* ======================================================================
Cette fonction termine une impression. Si aucun PRINTER n'est sélectionné
ou qu'aucune instruction d'impression n'a été envoyée, cet appel n'a
aucun effet. Dans le cas contraire, le string de terminaison (END) du
PRINTER est envoyé en fin d'impression.

&IT DOS
───
En DOS, le fichier ou le device dont le nom est fourni dans la
définition du PRINTER par le mot-clé FILE ou PRINTER est ouvert dès la
première impression de la session. S'il s'agit d'une imprimante,
l'impression commence donc dès qu'une page est prête. S'il s'agit d'un
fichier, le fichier est construit page par page.

&IT Unix
────
Dans un système multi-tâche, l'imprimante n'est pas ouverte pour
l'utilisation exclusive du programme. Un "pipe" est ouvert en UNIX vers
lequel est dirigé l'output d'impression page par page.

&EX
     PR_select(pr1);
     ...
     PR_send();
&TX
&SA PR_select()
------------------------------------------------------------------------ */

PR_send()
{
    PR_DEF  *pr = PR_CURRENT;

    if(pr == 0 || pr->pr_im == 0) return(0);

    /* SEND CURRENT PAGE */
    PR_flush(pr);

    /* SEND END STRING */
#ifdef PRWINDOWS
    switch(pr->pr_type) {                          /* JMP 19-08-98 */
	case 'F' :
	    if(pr->pr_end != 0)
		PR_print_str(pr, pr->pr_end);
	    fprintf(pr->pr_fd, "\032");
	    fclose(pr->pr_fd);
	    break;
	case 'D' :                                 /* JMP 19-08-98 */
	    if(pr->pr_end != 0)                    /* JMP 19-08-98 */
		PR_print_str(pr, pr->pr_end);      /* JMP 19-08-98 */
	    /*WscrPrintRawData("\032", 1L);        /* JMP 23-08-98 */
	    WscrPrintRawEnd();                     /* JMP 19-08-98 */
	    break;                                 /* JMP 19-08-98 */
	default :
	    WprChEnd();
	    break;
	}
#else
    if(pr->pr_end != 0)
	PR_print_str(pr, pr->pr_end);

    /* SEND TO FILE OR PRINTER */
#ifdef DOS
    fprintf(pr->pr_fd, "\032");
    fclose(pr->pr_fd);
#else
    if(pr->pr_type == 'F') fclose(pr->pr_fd);
    else                   pclose(pr->pr_fd);
#endif  /* DOS */
#endif  /* PRWINDOWS */

    /* FREE PRINTER MEM PAGE */
    SCR_free_image(pr->pr_im);
    pr->pr_im = 0;
    return(0);
}

/* ======================================================================
Définit un nouveau printer courant. Cette fonction n'a d'autre effet que
ce fixer une variable globale. Aucune ouverture réelle n'a lieu à ce
stade.
&EX
    PR_select(pr1);
    ...
    PR_send();
&TX
&SA PR_send()
------------------------------------------------------------------------ */
PR_select(pr)
PR_DEF  *pr;
{
    PR_CURRENT = pr;
    return(0);
}

/*NH*/
/* ===+==== PR_print_attr =============================
    Print attribute string
	attr : IN : attribute (SCR DEF)
   ============================================== */

PR_print_attr_std(pr, attr)
PR_DEF  *pr;
int     attr;
{
    unsigned char    *ptr = 0;

    switch(attr) {
	case SCR_DEFAULT    : ptr = pr->pr_normal; break;
	case SCR_BOLD       : ptr = pr->pr_bold; break;
	case SCR_UNDERLINE  : ptr = pr->pr_underl ; break;
	case SCR_REVERSE    : ptr = pr->pr_reverse; break;
	case SCR_ENLARGED   : ptr = pr->pr_enlarged; break;
	case SCR_CONDENSED  : ptr = pr->pr_condensed; break;
	}
    if(ptr != 0)
	PR_print_str(pr, ptr);

    return(0);
}

/*NH*/
PR_print_attr(pr, attr)
PR_DEF  *pr;
int     attr;
{
#ifdef PRWINDOWS
    switch(pr->pr_type) {
	case 'F' :
	case 'D' :
	    PR_print_attr_std(pr, attr);
	    break;
	default :
	    WprSetFontType(attr);
	    break;
	}
#else
    PR_print_attr_std(pr, attr);
#endif

    return(0);
}

/*NH*/
/* ===+==== PR_flush ============================
    Print the content of the current page
   ============================================== */

PR_flush(pr)
PR_DEF  *pr;
{
    int     i, j, pos, nc;
    unsigned char    *cells;
    int     attr = 0, ch;
/*    char    buf[133]; /* JMP 28-09-96 */
    char    buf[256];   /* JMP 28-09-96 */

    if(PR_TITLE != 0 && PR_TITLE[0] != 0) {
	sprintf(buf, PR_TITLE, pr->pr_pgnb);
	PR_print_str(pr, buf);
	PR_flush_lf(pr, 2);
    }

    cells = (unsigned char *)pr->pr_im->im_cells;

    if(!SCR_DISP_MIN32) /* BP_M 07-04-1995 */
	for(i = 0 ; i < pr->pr_nl * pr->pr_nc * 2 ; i+= 2)
	    if(cells[i] < ' ') cells[i] = ' ';

    for(i = 0 ; i < pr->pr_nl ; i++) {
	pos = i * pr->pr_nc * 2;
	for(nc = pr->pr_nc - 1 ; nc >= 0 ; nc--)
	    if(cells[pos + nc * 2] != ' ')
		break;
	for(j = 0 ; j <= nc ; j++) {
	    ch = cells[pos];
	    if(attr != cells[pos + 1]) {
		attr = cells[pos + 1];
		PR_print_attr(pr, attr);
	    }
	    if(pr->pr_maps[ch] != 0)
		PR_print_str(pr, pr->pr_maps[ch]);
	    else
		PR_flush_char(pr, ch);
	    pos += 2;
	}
	if(i != pr->pr_nl - 1) PR_flush_lf(pr, 1);
    }

    PR_flush_ff(pr);
    return(0);
}

/*NH*/
PR_flush_char(pr, ch)
PR_DEF  *pr;
int     ch;
{
#ifdef PRWINDOWS
    switch(pr->pr_type) {
	case 'F' :
	    fputc(ch, pr->pr_fd); /* JMP 19-08-98 */
	    break;
	case 'D' :
	    WscrPrintRawData(&ch, 1L); /* JMP 19-08-98 */
	    break;
	default :
	    WprChPrintChar(ch);
	    break;
	}
#else
    fputc(ch, pr->pr_fd);
#endif
    return(0);
}

/*NH*/
PR_flush_lf_std(pr, n)
PR_DEF  *pr;
int     n;
{
    int     i;

    for(i = 0 ; i < n; i++)
	PR_print_str(pr, pr->pr_linefeed);

    return(0);
}

/*NH*/
PR_flush_lf(pr, n)
PR_DEF  *pr;
int     n;
{
#ifdef PRWINDOWS
    if(U_is_in(pr->pr_type, "FD")) PR_flush_lf_std(pr, n);
    else                           WprChNewLine(n);
#else
    PR_flush_lf_std(pr, n);
#endif

    return(0);
}

/*NH*/
PR_flush_ff_std(pr)
PR_DEF  *pr;
{
    PR_print_str(pr, pr->pr_formfeed);
    return(0);
}

/*NH*/
PR_flush_ff(pr)
PR_DEF  *pr;
{
#ifdef PRWINDOWS
    if(U_is_in(pr->pr_type, "FD")) PR_flush_ff_std(pr);
    else                           WprChFormFeed();
#else
    PR_flush_ff_std(pr);
#endif
    return(0);
}

/* ======================================================================
Repositionne le curseur en colonne 0 de la ligne courante.
&SA
------------------------------------------------------------------------ */
PR_cr()
{
    PR_DEF  *pr = PR_CURRENT;

    if(PR_open()) return(0);
    pr->pr_ccol = 0;
    return(0);
}


/* ======================================================================
Positionne le curseur en position absolue. Si la position nouvelle sort
de la page, elle est fixée à la dernière ligne (et/ou colonne) de la page.

Une valeur négative ne change pas la valeur courante dans le printer.

&EX
    PR_goto(10, 15);  Place en colonne 15, ligne 10
    PR_goto(-1, 20);  Place en colonne 20 sans changer la ligne
&TX
&SA PR_up(), PR_right(), PR_down(), PR_left()
------------------------------------------------------------------------ */
PR_goto(l, c)
int     l, c;
{
    PR_DEF  *pr = PR_CURRENT;

    if(PR_open()) return(0);
    if(c >= 0) pr->pr_ccol  = c;
    if(l >= 0) pr->pr_cline = l;
    if(pr->pr_nl <= pr->pr_cline) pr->pr_cline = pr->pr_nl - 1;
    if(pr->pr_nc <= pr->pr_ccol)  pr->pr_ccol  = pr->pr_nc - 1;
    return(0);
}

/* ======================================================================
Imprime un caractère à la position courante du curseur et avance
celui-ci d'un caractère à droite.

&EX
    PR_char('a');
    PR_char('\n');
&TX
&SA PR_string()
------------------------------------------------------------------------ */
PR_char(ch)
int     ch;
{
    PR_DEF          *pr = PR_CURRENT;
    unsigned char   *cells;
    int             ccol = pr->pr_ccol;

    if(PR_open()) return(0);
    switch(ch) {
	case '\n': PR_linefeed(); break;
	case '\f': PR_formfeed(); break;
	case '\t': pr->pr_ccol += 8 - ccol % 8; break;
	default  :
	    if(ccol >= pr->pr_nc) break;
	    cells = pr->pr_im->im_cells +
		       2 * (pr->pr_nc * pr->pr_cline + ccol);
	    cells[0] = ch;
	    cells[1] = pr->pr_attr;
	    pr->pr_ccol++;
	    break;
	}
    return(0);
}


/* ======================================================================
Envoie la page vers l'imprimante ou le fichier et place la curseur en
position (0, 0) de la nouvelle page vierge.
------------------------------------------------------------------------ */
PR_formfeed()
{
    PR_DEF  *pr = PR_CURRENT;

    if(PR_open()) return(0);
    PR_flush(pr);
    pr->pr_pgnb++;
    PR_clear();
    return(0);
}

/* ======================================================================
Place le curseur en colonne 0 de la ligne suivante de la page courante.
Si le curseur est en dernière ligne, génère un saut de page.

&SA PR_lf()
------------------------------------------------------------------------ */
PR_linefeed()
{
    PR_DEF  *pr = PR_CURRENT;

    if(PR_open()) return(0);
    pr->pr_cline++;
    pr->pr_ccol = 0;
    if(pr->pr_nl <= pr->pr_cline) PR_formfeed();
    return(0);
}

/* ======================================================================
Génère n linefeed(). Si un saut de page a lieu pour l'un des linefeed,
les linefeed suivants ont quand-même lieu (su rla page suivante).

&SA PR_linefeed()
------------------------------------------------------------------------ */
PR_lf(n)
{
    for( ; n > 0 ; n--) PR_linefeed();

    return(0);
}

/* ======================================================================
Vérifie qu'il reste assez de place sur la page courante d'impression
pour imprimer au moins nl lignes. Si ce n'est pas le cas, génère un saut
de page.

&EX
    PR_block(5);
    PR_utitle("Article %s - valeur %d FB", is_art_CODE, is_art_PRICE);
&TX
------------------------------------------------------------------------ */

PR_block(nl)
int     nl;             /* Number of lines in block */
{
    PR_DEF  *pr = PR_CURRENT;

    if(nl + pr->pr_cline > pr->pr_nl)
	PR_formfeed();

    return(0);
}

/* ======================================================================
Imprime un string à la position courante du curseur dans la page
d'impression et souligne ce string. Le curseur est ensuite positionné
sous la ligne de soulignement en colonne 0.

&EX
    PR_block(5);
    PR_utitle("Article %s - valeur %d FB", is_art_CODE, is_art_PRICE);

	donnera

    Article AA0001 - valeur 1230 FB
    -------------------------------
&TX
&SA PR_string(), PR_block()
------------------------------------------------------------------------ */
PR_utitle(c1, c2, c3, c4, c5, c6, c7, c8)
char    *c1, *c2, *c3, *c4, *c5, *c6, *c7, *c8;
{
    char    buf[256];

    sprintf(buf, c1, c2, c3, c4, c5, c6, c7, c8);
    PR_string(buf);
    PR_linefeed();
    memset(buf, '-', strlen(buf));
    PR_string(buf);
    PR_linefeed();
    return(0);
}

/*NH*/
PR_print_str(pr, str)                /* JMP 05-01-97 */
PR_DEF  *pr;
U_ch    *str;
{
    if(str == 0) return(0);         /* JMP 13-09-98 */
#ifdef PRWINDOWS
    switch(pr->pr_type) {
	case 'F' :
	    while(*str) fputc(*str++, pr->pr_fd);
	    break;
	case 'D' :                          /* JMP 19-08-98 */
	    WscrPrintRawString(str);        /* JMP 19-08-98 */
	    break;                          /* JMP 19-08-98 */
	default :
	    while(*str) WprChPrintChar(*str++);
	    break;
	}

#else
    while(*str) fputc(*str++, pr->pr_fd);
#endif

    return(0);
}





