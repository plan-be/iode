#include "scr4w.h"
#include <commdlg.h>


/* ==================================================================
Ouvre un écran de sélection de couleurs.

&EN hOwner est le handle de la fenêtre appelante. Si on ignore
    cette valeur, on peut passer 0 comme paramètre. La dernière
    fenêtre active de l'application deviendra owner.

&EN dftcol définit la couleur par défaut (ou la couleur courante
    dans votre application)

&EN custcols est un pointeur optionnel vers un tableau de 16 couleurs
    personnalisées (COLORREF *). Si ce pointeur est nul, la définition
    des couleurs personnalisées est rendu inactive.

&RT
La fonction retourne la couleur sélectionnée. Si la boîte de dialogue a
été quittée par CANCEL, la couleur d'origine dftcol est retournée.

Le tableau custcols est également modifié s'il est défini.

===================================================================== */

COLORREF WscrChooseColor(HWND hOwner, COLORREF dftcol, COLORREF *custcols)
{
    CHOOSECOLOR cc;
    COLORREF clr[16];
    int         rc;

    memset(&cc, 0, sizeof(CHOOSECOLOR));
    cc.lStructSize    = sizeof (CHOOSECOLOR) ;

    if(hOwner)        cc.hwndOwner = hOwner;
    else if(hMainWnd) cc.hwndOwner = GetLastActivePopup(hMainWnd);
    else              cc.hwndOwner = 0;

    cc.hInstance      = NULL ;
    cc.rgbResult      = dftcol;
    cc.Flags          = CC_RGBINIT;
    cc.lCustData      = 0L ;
    cc.lpfnHook       = NULL ;
    cc.lpTemplateName = NULL ;

    if(custcols == 0) cc.lpCustColors = clr;
    else              cc.lpCustColors = custcols;
    if(custcols == 0) cc.Flags |= CC_PREVENTFULLOPEN ;

    if(ChooseColor(&cc) == TRUE) return(cc.rgbResult);
    else                         return(dftcol);
}


/* ==================================================================
Ouvre un écran de sélection de fichier. Cette fonction est utilisée
pour les champs DIR des pages.

&EN hOwner est le handle de la fenêtre appelante. Si on ignore
    cette valeur, on peut passer 0 comme paramètre. La dernière
    fenêtre active de l'application deviendra owner.

&EN title définit le titre de la fenêtre ("Ouverture de fichier" par ex.)
    Si title est NULL, un titre type est proposé.

&EN opensave indique s'il s'agit d'un fichier à ouvrir ou à sauver:
    0 indique open, 1 indique save. Dans le cas d'une ouverture, le
    fichier doit exister, dans le cas d'un sauvetage, seul le
    directory doit exister. D'autre part, le texte des boutons de
    confirmation et des titres par défaut sont différents : Ouverture ou
    Sauvetage selon le cas.

&EN infile contient le nom par défaut du fichier. Le directory initial
    pour la sélection sera automatiquement placé sur celui de ce fichier.

&EN outfile contient le nom sélectionné. Si aucune sélection n'a été opérée,
    le nom par défaut (infile) est copié dans outfile. Ce buffer doit
    pouvoir contenir 255 caractères!

&EN ftypes contient les types de fichiers acceptés dans un string
    séparé par des pipes (|). Chaque type est présenté comme une paire de
    strings séparés par des |, dont le premier indique le texte à faire
    apparaître dans le Combobox et le second le ou les types de fichiers
    correspondants. Si ftypes est NULL, tous les fichiers sont admis
    et le texte All Files est affiché.
    Si plus d'un type correspond à un texte, les types doivent être
    séparés par des points-virgules (;).

&NO
    ftypes DOIT se terminer par un |, sans quoi il est ignoré.

&EX
    char outfile[256];

    WscrGetFilename(0L, "Mon titre", "toto", outfile,
			    "Text files|*.txt;*.asc|C sources|*.c|");
&TX

&RT
    La fonction retourne -1 si la sélection ne s'est pas opérée
    (Cancel, ...).

===================================================================== */

int WscrGetFilename(hOwner, opensave, title, infile, outfile, ftypes)
HWND    hOwner;
int     opensave;
U_ch    *title, *infile, *outfile, *ftypes;
{
    OPENFILENAME    ofn;
    U_ch            dirname[256], *buf;
    int             i;

    memset(&ofn, 0, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);

    if(hOwner)        ofn.hwndOwner = hOwner;
    else if(hMainWnd) ofn.hwndOwner = GetLastActivePopup(hMainWnd);
    else              ofn.hwndOwner = 0;

    if(ftypes == 0 || ftypes[strlen(ftypes)-1] != '|')
	    buf = SCR_stracpy("All Files|*.*|");
    else    buf = SCR_stracpy(ftypes);
    for(i = 0 ; buf[i] ; i++) if(buf[i] == '|') buf[i] = 0;
    ofn.lpstrFilter = buf;
    ofn.nFilterIndex = 1;

    ofn.lpstrTitle = title;
    if(title) SCR_OemToAnsi(title, title);

    strcpy(dirname, infile);
    SCR_strip(dirname);
    SCR_split_dir(dirname, outfile);
    ofn.lpstrFile = outfile;
    ofn.nMaxFile = 255;
    ofn.lpstrInitialDir = dirname;
    ofn.Flags = OFN_SHOWHELP | OFN_PATHMUSTEXIST |
		OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
    if(opensave == 0) ofn.Flags |= OFN_FILEMUSTEXIST;
    else              ofn.Flags |= OFN_PATHMUSTEXIST;

    if(opensave) i = GetSaveFileName(&ofn);
    else         i = GetOpenFileName(&ofn);

    if(title) SCR_OemToAnsi(title, title);
    SCR_free(buf);

    if(i == FALSE) {
	strcpy(outfile, infile);
	return(-1);
	}

    return(0);
}

/* ==================================================================
Ouvre un écran de sélection de polices de caractères.

&EN hOwner est le handle de la fenêtre appelante. Si on ignore
    cette valeur, on peut passer 0 comme paramètre. La dernière
    fenêtre active de l'application deviendra owner.

&EN logfont est un pointeur vers une structure LOGFONT qui définit
    une police. Cette structure sert à initialiser les valeurs proposées
    à l'écran et reçoit en fin de traitement les nouvelles valeurs
    sélectionnées.

&RT
    La fonction retourne -1 si la sélection ne s'est pas opérée
    (Cancel, ...).

===================================================================== */

int WscrChooseFont(HWND hOwner, LOGFONT *logfont)
{
    CHOOSEFONT    cf;

    memset(&cf, 0, sizeof(CHOOSEFONT));
    cf.lStructSize = sizeof(CHOOSEFONT);

    if(hOwner)        cf.hwndOwner = hOwner;
    else if(hMainWnd) cf.hwndOwner = GetLastActivePopup(hMainWnd);
    else              cf.hwndOwner = 0;

    cf.lpLogFont = logfont;
    cf.Flags = CF_SCREENFONTS | CF_EFFECTS;
/*    cf.Flags = CF_SCREENFONTS | CF_EFFECTS; | CF_INITTOLOGFONTSTRUCT; */
    cf.nFontType = SIMULATED_FONTTYPE;
    cf.rgbColors = RGB(0, 0, 255);

    ChooseFont(&cf);
    return(0);
}
/*
TestFont()
{
    LOGFONT logfont;

    memset(&logfont, 0, sizeof(LOGFONT));
    logfont.lfHeight  = 12;
    strcpy(logfont.lfFaceName, "Arial");

    WscrChooseFont(0L, &logfont);
}
*/



