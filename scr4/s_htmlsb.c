#include "scr4.h"
#include "s_html.h"

#ifdef DOS
#include <io.h>
#endif

/* ===================================================================
Cette fonction balaie un ensemble de fichiers HTML, éventuellement
récursivement et remplace des portions de chacun des fichiers par le
contenu du fichier replfile.

&EN from = délimiteur de début de section
&EN to = délimiteur de fin de section
&EN replfile = fichier remplaçant la section
&EN files = fichier à balayer (avec * ou ? éventuellement). Ce nom peut
    également être celui d'un directory.

&EN Recursive = traitement des fichiers dans les sous-directories

&EN Backup = prise d'un backup pour chaque fichier avant traitement. Si
    c'est la cas, le fichier backup a pour extension .bak.

&EN Silent = 0 pour éviter un message par fichier, 1 sinon

Cette fonction est utilisée par exemple pour remplacer des barres de
navigations de façon automatique, typiquement de la façon suivante :

&CO
    HtmlReplace("<!STARTHEADER>", "<!ENDHEADER>", "head.ht1",
						"*.htm", 0, 0);
    HtmlReplace("<!STARTFOOTER>", "<!ENDFOOTER>", "foot.ht1",
						"*.htm", 0, 0);
&TX
avec pour fichier head.ht1:

&CO
    <!STARTHEADER>
    <center>
    <a href="../../index.htm"><img src="../../images/xon.gif"
	    VALIGN=TOP border=0></a>
    <a href="../../scr4/index.htm"><img src="../../images/scr42t.gif"
	    VALIGN=TOP border=0></a>
    <BR>
    <img src="../../images/rainbow.gif" VALIGN=TOP border=0
						width=500 height=4>
    </center>
    <!ENDHEADER>
&TX
et foot.ht1:
&CO
    <!STARTFOOTER>
    <center>
    <img src="../../images/rainbow.gif" VALIGN=TOP
			    border=0 width=500 height=2>
    <br>
    [<a href="../../index.htm">X-ON</a>]
    [<a href="../../scr4/index.htm">SCR4/AL1</a>]
    </font>
    </center>

    <br>
    <center>
    <font size=-1>Copyright &copy; 1997-2001Jean-Marc and Bernard PAUL
		<A HREF="mailto:bp@xon.be">bp@xon.be</A>
    </font>
    </center>
    <!ENDFOOTER>
&TX

Le traitement continue tant que cela est possible, même si certains
fichiers ne peuvent être ouverts.

&RT
&EN 0 en cas de succès,
&EN -1 en cas d'erreur(s)

&SA HtmlSplit(), HtmlToc(), HtmlMessage()
====================================================================== */

HtmlReplace(from, to, replfile, files, Recursive, Backup, Silent)
U_ch    *from, *to, *replfile, *files;
int     Recursive, Backup, Silent;
{
    char    dir[132], fname[132], buf[132];
    SCRSTAT **ss, ss1;
    int     i;

    if(!U_is_in('*', files) && !U_is_in('?', files)) {
	if(SCR_stat(files, &ss1)) {
	    sprintf(buf, "Cannot access %s\n", files);
	    HtmlMessage(buf);
	    return(-1);
	    }

	if(!(ss1.ss_mode & SCR_FTYPE_DIR)) {
	    return(JNavFile(from, to, replfile, files, ss1.ss_mode, Backup, Silent));
	    }
	}

    ss = SCR_dir(files, 1, dir, fname);
    if(ss == 0) {
	    sprintf(buf, "Cannot access %s\n", files);
	    HtmlMessage(buf);
	    return(-1);
	    }

    for(i = 0 ; ss[i] ; i++) {
	if(ss[i]->ss_mode & SCR_FTYPE_DIR) {
	    if(Recursive) {
		if(strcmp(ss[i]->ss_name, ".") == 0 ||
		   strcmp(ss[i]->ss_name, "..") == 0) continue;
#ifdef DOS
		sprintf(buf, "%s\\%s\\%s", dir, ss[i]->ss_name, fname);
#else
		sprintf(buf, "%s/%s/%s", dir, ss[i]->ss_name, fname);
#endif
		HtmlReplace(from, to, replfile, buf, Recursive, Backup, Silent);
		}
	    continue;
	    }

#ifdef DOS
	sprintf(buf, "%s\\%s", dir, ss[i]->ss_name);
#else
	sprintf(buf, "%s/%s", dir, ss[i]->ss_name);
#endif
	JNavFile(from, to, replfile, buf, ss[i]->ss_mode, Backup, Silent);
	}

    unlink("scr4_sbs.$$$");
    SCR_free_tbl((U_ch **)ss);
    return(0);
}
/*NH*/
JNavFile(from, to, replfile, buf, mode, Backup, Silent)
U_ch            *from, *to, *replfile, *buf;
unsigned int    mode, Backup, Silent;
{
    int     rc;
    char    msg[128];

    if(Silent == 0) {
	sprintf(msg, "Translating %s ...", buf);
	HtmlMessage(msg);
	}
    rc = JNavFileNoMsg(from, to, replfile, buf, mode, Backup);
    switch(rc) {
	case 0 : if(Silent == 0) strcat(msg, "Done."); break;
	case -1 : sprintf(msg, "Cannot access %s.", buf); break;
	case -2 : sprintf(msg, "Strings not found."); break;
	case -3 : sprintf(msg, "Cannot rename %s.", buf); break;
	case -4 : sprintf(msg, "Cannot open %s.$$$", buf); break;
	case -5 : sprintf(msg, "Cannot modify %s.", buf);break;
	}

    if(rc || Silent == 0) HtmlMessage(msg);
    return(rc);
}

/*NH*/
JNavFileNoMsg(from, to, replfile, file, mode, Backup)
U_ch   *from, *to, *replfile, *file;
unsigned int    mode, Backup;
{
    FILE    *fdin, *fdou;
    char    old[132];
    int     ch;
    long    lfrom, lto, i;

    if(JNavAccess(file)) return(-1);

    lfrom = SCR_FindStringPos(file, from, strlen(from));
    lto   = SCR_FindStringPos(file, to, strlen(to));
    if(lfrom < 0 || lto < 0) return(-2);

    strcpy(old, "scr4_sbs.$$$");
    if(Backup) SCR_change_ext(old, file, "bak");
    unlink(old);
    if(SCR_rename(file, old)) return(-3);

#ifdef DOS
    fdin = fopen(old, "rb");
#else
    fdin = fopen(old, "r");
#endif
    if(fdin == 0) return(-4);

#ifdef DOS
    fdou = fopen(file, "wb+");
#else
    fdou = fopen(file, "w+");
#endif
    if(fdou == 0) {
	fclose(fdin);
	return(-5);
	}

    for(i = 0L ; i < lfrom ; i++) {
	ch = getc(fdin);
	putc(ch, fdou);
	}

    JNavDump(fdou, replfile);

    fseek(fdin, lto + strlen(to), 0);
    while(1) {
	ch = getc(fdin);
	if(ch == EOF) break;
	putc(ch, fdou);
	}

    fclose(fdin);
    fclose(fdou);
    chmod(file, mode);
    return(0);
}

/*NH*/
JNavAccess(filename)
char    *filename;
{
    FILE    *fd;

    fd = fopen(filename, "r+");
    if(fd == 0) return(-1);
    fclose(fd);
    return(0);
}


/*NH*/
int JNavDump(fdou, replfile)
FILE    *fdou;
char    *replfile;
{
    FILE    *fdin;
    int     ch, cr = 0;

#ifdef DOS
    fdin = fopen(replfile, "rb");
#else
    fdin = fopen(replfile, "r");
#endif
    if(fdin == 0) return(0);
    while(1) {
	ch = getc(fdin);
	if(ch == EOF) break;
	if(cr >= 2) {
	    if(cr == 3) putc('\r', fdou);
	    putc('\n', fdou);
	    cr = 0;
	    }

	if(ch == '\r') cr = 1;
	else if(ch == '\n') cr += 2;
	else putc(ch, fdou);
	}

    fclose(fdin);
    return(0);
}

