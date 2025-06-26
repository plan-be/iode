#include "scr4.h"
#include "s_html.h"

U_ch    *JTitle = 0, *JInputFile, *JTits[8], *JStyle = 0;
char    JResFile[128], *JBody = 0, *JBodyHtml = 0, *JBack = 0;
FILE    *JFdOut, *JFdMain;
int     JCurSection = 0, JRefs = 1, JPrevNext = 0;
int     JTitFiles[8], JVerbose = 0, JLevel = 2;

/* ===================================================================
Cette fonction balaie le fichier HTML htmlfile et le découpe sur base des
tags <H*>. Les fichiers générés ont pour noms:

&EN resfile pour le fichier principal
&EN resfile1...n pour les fichiers secondaires

Par exemple si resfile vaut myres, on aura myres.htm, myres1.htm, etc.

&IT Arguments
─────────
&EN htmlfile : nom du fichier à analyser
&EN resfile : racine des noms de fichiers extraits (sans extension)
&EN level : niveau Hn de découpe en sous-fichiers
&EN title : titre de chaque fichier HTML
&EN body : image background des fichiers extraits
&EN back : image de l'icône back précédant le titre
&EN prevnext : indique s'il faut une barre de navigation automatique
&EN verbose : affiche l'état d'avancement
&EN style : fichier contenant les styles à incorporer

A chaque <H*> rencontré de niveau inférieur ou égal à level, le fichier
principal contient le texte du titre lié par un hyperlien au
sous-fichier correspondant. Ce sous-fichier contient tout le texte entre
les tags Hn de même niveau.

Les fichiers extraits sont des fichiers HTML avec:

&EN un titre si title est défini

&EN pour le tag Body, si body est défini, l'image correspondante en
    background, sinon le tag Body du fichier htmlfile

&EN les tags STARTHEADER et ENDHEADER

&EN si prevnext est non nul, des références vers le fichier précédent,
    le suivant et le fichier principal entre les tags STARTCMT et ENDCMT

&EN si title est défini, le titre précédé de l'image dont le nom se
    trouve dans back si back est défini, avec un hyperlien vers le
    fichier principal

&EN une ligne par titre de niveau inférieur avec hyperlien vers le fichier
    correspondant à chacun

&EN le titre du fichier courant (contenu du tag Hn qui a donné lieu à
    l'extraction

&EN le texte du fichier principal jusqu'au tag Hn suivant de même niveau

&EN les tags STARTFOOTER et ENDFOOTER

Dans les fichiers générés, chaque fois qu'un tag Hn est rencontré, il
est automatiquement précédé d'un tag NAME, pour pourvoir être référencé
avec plus de précision lors de l'extraction de la table des matières.

Les fichiers générés contiennent des tags spéciaux :

&EN !STARTHEADER ... !ENDHEADER en début de fichier
&EN !STARTCMT ... !ENDCMT en début de fichier
&EN !STARTFOOTER ... !ENDFOOTER en fin de fichier

Ces tags pourront par la suite servir d'indication au programme scr4_sbs
ou à la fonction HtmlReplace().

Cette fonction est exploitée par l'utilitaire scr4_spl.

&RT
&EN 0 en cas de succès,
&EN -1 si le fichier resfile n'a pu être créé
&EN -2 si un des fichiers référencés n'a pû être ouvert

&SA HtmlToc(), HtmlReplace(), HtmlMessage()
====================================================================== */

HtmlSplit(htmlfile, resfile, level, title, body, back, prevnext, verbose, style)
char    *htmlfile, *resfile;
U_ch    *title, *body, *back, *style;
int     level, prevnext, verbose;
{
    unsigned char   buf[132];
    int             type;
    HFILE           *hfd;

    hfd = JHOpen(htmlfile);
    if(hfd == 0) return(-1);
    sprintf(buf, "%s.htm", resfile);
    JFdMain = fopen(buf, "w+");
    if(JFdMain == 0) {
	JHClose(hfd);
	return(-2);
	}
    strcpy(JResFile, resfile);
    JLevel = level;
    JTitle = SCR_stracpy(title);
    JStyle = SCR_stracpy(style); /* JMP 02-05-00 */
    JBody = SCR_stracpy(body);
    JBack = SCR_stracpy(back);
    JPrevNext = prevnext;
    JVerbose = verbose;
    JCurSection = 0;


    fprintf(JFdMain, "<HTML><HEAD>\n");
    fprintf(JFdMain, "\t<META NAME=\"GENERATOR\" CONTENT=\"SCR4/AL1\">\n");
    fprintf(JFdMain, "\t<TITLE>");
    if(JTitle && JTitle[0]) fprintf(JFdMain, "%s", JTitle);
    fprintf(JFdMain, "</TITLE>\n");
    if(JStyle) {
       //fprintf(JFdMain, "<STYLE>\n<!--\n"); /* JMP 21-10-01 */
       SCR_dumpf_file(JFdMain, JStyle);
       //fprintf(JFdMain, "\n-->\n</STYLE>\n"); /* JMP 21-10-01 */
       }

    fprintf(JFdMain, "</HEAD>\n");
    if(JBody && JBody[0]) fprintf(JFdMain, "<BODY BACKGROUND=\"%s\">\n", JBody);
    /*else                  fprintf(JFdMain, "<BODY>\n"); /* JMP 27-04-99 */
    else                  fprintf(JFdMain, "<BODY TEXT=\"#000000\" BGCOLOR=\"#FFFFC0\" LINK=\"#0000FF\" ALINK=\"#FF00FF\" VLINK=\"#800080\">\n"); /* JMP 27-04-99 */

    fprintf(JFdMain, "<!STARTHEADER>\n<!ENDHEADER>\n");
    fprintf(JFdMain, "<!STARTTOC>\n<!ENDTOC>\n");

    if(JTitle && JTitle[0]) {
      /*  fprintf(JFdMain, "<P>");
	fprintf(JFdMain, "<FONT SIZE=+2>%s</FONT></P>\n", JTitle);
      /* JMP 21-10-01 */
	fprintf(JFdMain, "<P><B>%s</B></P>\n", JTitle); /* JMP 21-10-01 */
	}

    while(1) {
	type = JHLex(hfd);
	switch(type) {
	    case JH_BODY :
		if(JBody && JBody[0]) break;
		JBodyHtml = SCR_stracpy(hfd->h_buf);
		fprintf(JFdMain, "%s", hfd->h_buf);
		break;

	    case JH_H1 : case JH_H2 : case JH_H3 :
	    case JH_H4 : case JH_H5 : case JH_H6 :
		if(level >= type - JH_H1 + 1)
		    JSplitChapter(hfd, level, type - JH_H1 + 1);
		else
		    fprintf(JFdMain, "%s", hfd->h_buf);
		break;

	    case -JH_BODY :
	    case -JH_HTML : break;
	    case JH_EOF :
		fprintf(JFdMain, "<!STARTFOOTER>\n<!ENDFOOTER>\n");
		fprintf(JFdMain, "</BODY>\n</HTML>\n");
		fclose(JFdMain);
		JHClose(hfd);
		SCR_free(JBody);
		SCR_free(JTitle);
		SCR_free(JBack);
		JResetTit(0);
		return(0);

	    default :
		fprintf(JFdMain, "%s", hfd->h_buf);
		break;

	    }
	}
}
/*NH*/
JSplitChapter(hfd, level, clevel)
HFILE   *hfd;
int     level, clevel;
{
    char    buf[20], msg[256];
    int     type, titnb = 1, i, font;
    FILE    *JFdTmp;

    if(JOpenNext()) {
	HtmlMessage("Cannot open sub file.\n");
	return(-2);
	}

/*    if(JRefs) {
	fprintf(JFdOut, "<P ALIGN=CENTER><A HREF=\"%s.htm\">[Up one level]</A>\n", JResFile);
	if(JCurSection > 1) fprintf(JFdOut, " <A HREF=\"%s%d.htm\">[Previous]</A>\n", JResFile, JCurSection - 1);
	fprintf(JFdOut, " <A HREF=\"%s%d.htm\">[Next]</A>", JResFile, JCurSection + 1);
	fprintf(JFdOut, "</P>\n");
	}
*/
    JResetTit(clevel);
    while(1) {
	switch(JHLex(hfd)) {
	    case -JH_H1 : case -JH_H2 : case -JH_H3 :
	    case -JH_H4 : case -JH_H5 : case -JH_H6 :
		goto cont;

	    case JH_HR :
		break;

	    case -JH_BODY :
	    case -JH_HTML : break;

	    case JH_EOF :
		HtmlMessage("Error : </Hn> not found.\n");
		fprintf(JFdOut, "<!STARTFOOTER>\n<!ENDFOOTER>\n");
		fprintf(JFdOut, "</BODY>\n</HTML>\n");
		fclose(JFdOut);
		return(-1);

	    default :
		JAddTit(clevel, hfd->h_buf);
		break;
	    }
	}

cont:
    fprintf(JFdOut, "<HTML><HEAD>\n");
    fprintf(JFdOut, "\t<META NAME=\"GENERATOR\" CONTENT=\"SCR4/AL1\">\n");
    fprintf(JFdOut, "\t<TITLE>");
    if(JTitle && JTitle[0]) fprintf(JFdOut, "%s", JTitle);
    if(JTits[clevel]) fprintf(JFdOut, " - %s", JTits[clevel]);
    fprintf(JFdOut, "</TITLE>\n");
    if(JStyle) {
       //fprintf(JFdOut, "<STYLE>\n<!--\n"); /* JMP 21-10-01 */
       SCR_dumpf_file(JFdOut, JStyle);
       //fprintf(JFdOut, "\n-->\n</STYLE>\n"); /* JMP 21-10-01 */
       }

    fprintf(JFdOut, "</HEAD>\n");
    if(JBody && JBody[0]) fprintf(JFdOut, "<BODY BACKGROUND=\"%s\">\n", JBody);
    else if(JBodyHtml) fprintf(JFdOut, "%s\n", JBodyHtml); /* JMP 17-02-97 */
    else               fprintf(JFdOut, "<BODY>\n");
    fprintf(JFdOut, "<!STARTHEADER>\n<!ENDHEADER>\n");

    if(JPrevNext) {
	fprintf(JFdOut, "<!STARTCMT>\n<P><CENTER>\n");
	if(JCurSection > 1)
	    fprintf(JFdOut, "<A HREF=\"%s%d.htm\">[Previous]</A>", JResFile, JCurSection - 1); /* JMP 24-02-97 */
	fprintf(JFdOut, " <A HREF=\"%s%d.htm\">[Next]</A>\n", JResFile, JCurSection + 1); /* JMP 24-02-97 */
	if(clevel > 2) fprintf(JFdOut, " <A HREF=\"%s%d.htm\">[Up]</A>\n", JResFile, JTitFiles[clevel - 1]); /* JMP 24-02-97 */
	fprintf(JFdOut, " <A HREF=\"%s.htm\">[Toc]</A>\n", JResFile); /* JMP 24-02-97 */
	fprintf(JFdOut, "</Center></P>\n<!ENDCMT>\n");
	}

    if(JTitle && JTitle[0]) {
	// fprintf(JFdOut, "<P>"); /* JMP 21-10-01 */
	if(JBack && JBack[0]) fprintf(JFdOut, "<A HREF=\"%s.htm\"><IMG ALT=\"Up\" BORDER=0 HSPACE=5 SRC=\"%s\"></A> ",
				JResFile, JBack);
	/*fprintf(JFdOut, "<FONT SIZE=+2><A HREF=\"%s.htm\">%s</A></FONT></P>\n",
				JResFile, JTitle);
	/* JMP 21-10-01 */
	fprintf(JFdOut, "<P class=h1><A HREF=\"%s.htm\">%s</A></P>\n",
				JResFile, JTitle);
	}
    for(i = 1 ; i < clevel ; i++) {
	font = 3 - i;
	if(font < 1) font = 1;
	if(JTits[i])
	    // fprintf(JFdOut, "<P><FONT SIZE=+%d><A HREF=\"%s%d.htm\">%s</A></FONT></P>\n",
	    //                font, JResFile, JTitFiles[i], JTits[i]);
	    fprintf(JFdOut, "<p class=h%d><A HREF=\"%s%d.htm\">%s</A></p>\n",
			    clevel, JResFile, JTitFiles[i], JTits[i]);
	}
    if(JTits[clevel])
	fprintf(JFdOut, "<H%d>%s</H%d>\n", clevel, JTits[clevel], clevel);


    fprintf(JFdMain, "<P class=LI><A HREF=\"%s%d.htm\">%s</A></P>\n", JResFile, JCurSection, JTits[clevel]);
    if(JVerbose) {
	memset(msg, ' ', clevel * 2);
	sprintf(msg + clevel * 2, "%s%d.htm : %s\n", JResFile, JCurSection, JTits[clevel]);
	HtmlMessage(msg);
	}

    while(1) {
	type = JHLex(hfd);
	switch(type) {
	    case JH_H1 : case JH_H2 : case JH_H3 :
	    case JH_H4 : case JH_H5 : case JH_H6 :
		if(level >= type - JH_H1 + 1) {
		    if(clevel >= type - JH_H1 + 1) {
			fprintf(JFdOut, "<!STARTFOOTER>\n<!ENDFOOTER>\n");
			fprintf(JFdOut, "</BODY>\n</HTML>\n");
			fclose(JFdOut);
			JHUnread(hfd);
			return(0);
			}
		    else {
			JFdTmp = JFdMain;
			JFdMain = JFdOut;
			JSplitChapter(hfd, level, type - JH_H1 + 1);
			JFdOut = JFdMain;
			JFdMain = JFdTmp;
			}
		    }
		else
		    fprintf(JFdOut, "<A NAME=\"TIT%d\"></A>%s", titnb++, hfd->h_buf);
		break;

	    default :
		fprintf(JFdOut, "%s", hfd->h_buf);
		break;

	    case -JH_BODY :
	    case -JH_HTML : break;

	    case JH_EOF :
		fprintf(JFdOut, "<!STARTFOOTER>\n<!ENDFOOTER>\n");
		fprintf(JFdOut, "</BODY>\n</HTML>\n");
		fclose(JFdOut);
		return(-1);
	    }
	}
}

/*NH*/
int JResetTit(level)
int     level;
{
    int     i;

    for(i = level ; i < 8 ; i++) {
	SCR_free(JTits[i]);
	JTits[i] = 0;
	}
    return(0);
}
/*NH*/
int JAddTit(level, txt)
int     level;
U_ch    *txt;
{
    int     lentxt, lentit = 0;

    if(txt == 0 || level >= 8) return(0);
    lentxt = strlen(txt);
    if(JTits[level]) lentit = strlen(JTits[level]);
    JTits[level] = SCR_realloc(JTits[level], 1, lentit, lentxt + lentit + 1);
    strcat(JTits[level], txt);

    JTitFiles[level] = JCurSection;
    return(0);
}
/*NH*/
JOpenNext()
{
    char    buf[132];

    JCurSection++;
    sprintf(buf, "%s%d.htm", JResFile, JCurSection);
    JFdOut = fopen(buf, "w+");
    if(JFdOut == 0) return(-1);
    return(0);
}



