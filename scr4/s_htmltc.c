#include "scr4.h"
#include "s_html.h"

FILE    *JFdToc;
char    JInputDir[128];
char    *JTarget = 0;

/* ===================================================================
Cette fonction balaie un fichier HTML et les sous-fichiers référencés et
génère une table des matières avec les références vers les
sous-fichiers sur base des tags <Hn> (qui indiquent les niveaux de
chapitres dans les fichiers HTML).

Le titre de chaque chapitre est repris avec le lien vers le
sous-fichier et si un tag NAME est présent, vers la position
dans ce sous-fichier.


&EN htmlfile = fichier input HTML
&EN resfile = fichier HTML contenant la Table des matières
&EN maxlevel = niveau de titre maximum pour la table des matières
&EN target = fenêtre target pour chaque topic
&EN

Le fichier résultat n'est pas un fichier HTML complet dans la mesure où
il est destiné à être par la suite intégré dans un fichier plus grand.

Le fichier débute par un tag spécial !STARTTOC et se termine par
un autre: !ENDTOC. Ces deux tags pourront par la suite servir
d'indication au programme scr4_sbs ou à la fonction HtmlReplace().

Le fichier résultat contient la table des matières dans le format
suivant:

&CO
    <!STARTTOC>
    <UL><UL><LI><A HREF="exes1.htm">1. Introduction</A></LI>
    <UL><LI><A HREF="exes1.htm#TIT1">1.1. Les programmes</A></LI>
    <LI><A HREF="exes1.htm#TIT2">1.2. Outils de d&#233;veloppement</A></LI>
    </UL><LI><A HREF="exes2.htm">2. SCR4 : outils de prototypage</A></LI>
    ...
    </UL></UL></UL>
    <!ENDTOC>
&TX

Il est ensuite aisé de remplacer par le contenu de ce fichier une
section préalablement créée dans un fichier html (par exemple à l'aide
de la fonction HtmlReplace().

Cette fonction est exploitée par l'utilitaire scr4_toc.

&RT
&EN 0 en cas de succès,
&EN -1 si le fichier resfile n'a pu être créé
&EN -2 si un des fichiers référencés n'a pû être ouvert

&SA HtmlSplit(), HtmlReplace(), HtmlMessage()
====================================================================== */

HtmlToc(htmlfile, resfile, maxlevel, target)
char    *htmlfile, *resfile, *target;
int     maxlevel;
{
    int     i, clevel;
    char    fname[128], buf[65];

    JFdToc = fopen(resfile, "w+");
    if(JFdToc == 0) {
	HtmlMessage("Cannot create output file");
	return(-1);
	}
    JTarget = target; /* JMP 27-04-99 */
    fprintf(JFdToc, "<!STARTTOC>\n");
    fprintf(JFdToc, "<TABLE WIDTH=500 BORDER=0 CELLSPACING=0 CELLPADDING=0>\n");
    SCR_fullpath(htmlfile, fname);
/*#ifdef DOS
    strcpy(JInputDir, fname + 2);
    SCR_split_dir(JInputDir, buf);
    clevel = JTocSub(fname + 2, maxlevel, 0);
#else
*/
    strcpy(JInputDir, fname);
    SCR_split_dir(JInputDir, buf);
    clevel = JTocSub(fname, maxlevel, 0);
/*#endif*/
/*    for(i = 0; i < clevel; i++) fprintf(JFdToc, "</UL>"); */
    fprintf(JFdToc, "</TABLE>\n");
    fprintf(JFdToc, "<!ENDTOC>\n");
    fclose(JFdToc);
    SCR_add_ptr(&JHRefs, &JHRefsNb, 0L);
    JHRefFree();

    if(clevel < 0) return(-2);
    return(0);
}

/*NH*/
JTocSub(htmlfile, maxlevel, clevel)
char    *htmlfile;
int     maxlevel, clevel;
{
    unsigned char   *hname = 0, subhtml[132], relhtml[132];
    int             type, dump = 0, level, i, cmt = 0;
    HFILE           *hfd;

    if(JHRefFind(htmlfile) >= 0) return(0);
    sprintf(relhtml, "Opening %s", htmlfile);
    HtmlMessage(relhtml);
    hfd = JHOpen(htmlfile);

    if(hfd == 0) {
	JHRefAdd(htmlfile, 0, 0);
	sprintf(relhtml, "Cannot open %s", htmlfile);
	HtmlMessage(relhtml);
	return(-2);
	}
    JHRefAdd(htmlfile, 0, 1);

    while(1) {
	type = JHLex(hfd);
	switch(type) {
	    case JH_TEXT :
		if(dump) fprintf(JFdToc, "%s", hfd->h_buf);
		break;

	    case JH_H1 :  case JH_H2 : case JH_H3 :
	    case JH_H4 :  case JH_H5 : case JH_H6 :
		level = type - JH_H1 + 1;
		if(level <= maxlevel) {
		  /*  if(level < clevel)
			for(i = level; i < clevel; i++) fprintf(JFdToc, "</UL>");
		    if(level > clevel)
			for(i = clevel; i < level; i++) fprintf(JFdToc, "<UL>");
		  */
		    fprintf(JFdToc, "<TR>");
		    for(i = 0; i < level - 2; i++) fprintf(JFdToc, "<TD><IMG SRC=\"vertline.gif\"></TD>");
		    if(level > 1) fprintf(JFdToc, "<TD VALIGN=\"TOP\"><IMG SRC=\"node.gif\"></TD>");
		    clevel = level;
		    SCR_relpath(JInputDir, htmlfile, relhtml);
		    SCR_strip(relhtml);
		    /*fprintf(JFdToc, "<LI><A HREF=\"%s", relhtml); */
		    fprintf(JFdToc, "<TD COLSPAN=%d><FONT FACE=\"Arial\" SIZE=-2><A HREF=\"%s", 1 + maxlevel - level, relhtml); /* JMP 21-10-01 */
		    if(hname) fprintf(JFdToc, "#%s", hname);
		    fprintf(JFdToc, "\"");
		    if(JTarget) fprintf(JFdToc, " target=\"%s\"", JTarget);
		    fprintf(JFdToc, ">");
		    dump = 1;
		    }
		break;
	    case -JH_H1 : case -JH_H2 : case -JH_H3 :
	    case -JH_H4 : case -JH_H5 : case -JH_H6 :
		if(dump) {
		    fprintf(JFdToc, "</A></FONT></TD></TR>\n");
		    dump = 0;
		    }
		break;

	    case JH_SCMT : cmt = 1; break;
	    case JH_ECMT : cmt = 0; break;
	    case JH_A:
		if(cmt == 0 && JHYYFind(hfd, "HREF") == 0) {
		    U_ljust_text(hfd->h_val);
		    SCR_strip(hfd->h_val);
		    if(JHRefIsProto(hfd->h_val)) {
			JHRefAdd(hfd->h_val, 0, 0);
			break;
			}
		    JHRefFullPath(subhtml, htmlfile, hfd->h_val);
		    if(subhtml[0] && JHRefFind(subhtml) < 0) {
			clevel = JTocSub(subhtml, maxlevel, clevel);
			}
		    }
		else if(JHYYFind(hfd, "NAME") == 0) {
		    if(hname) SCR_free(hname);
		    hname = SCR_stracpy(hfd->h_val);
		    SCR_strip(hname);
		    }
		break;

	    case JH_EOF :
		JHClose(hfd);
		return(clevel);

	    default : break;
	    }
	}
}


