#include "scr4.h"
#include "s_a2m.h"

/* ======================================================================
Crée un descripteur de fichier a2m en mémoire. Cela permet de créer des
impressions à l'aide des outils a2m sans passer par un fichier
intermédiaire.

Les destinations possibles sont :

&EN A2M_DESTGDIPRT (1) : impression sur l'imprimante Windows
&EN A2M_DESTGDIEMF (2) : impression dans un fichier Enhanced Metafile (emf)
&EN A2M_DESTGDIWMF (3) : impression dans un fichier Metafile (wmf)
&EN A2M_DESTRTF    (4) : impression dans un fichier RTF
&EN A2M_DESTHTML   (5) : impression dans un fichier HTML
&EN A2M_DESTFRM    (6) : impression dans un fichier FRM
&EN A2M_DESTA2M    (7) : impression dans un fichier A2M
&EN A2M_DESTCSV    (8) : impression dans un fichier CSV
&EN A2M_DESTDUMMY  (9) : impression supprimée
&EN A2M_DESTTCHRT (10) : affichage via TeeChart

La signification des paramétres dépend de la destination :

&EN dest = destination (voir ci-dessus)
&EN outfile = nom du fichier output. L'extension est ajoutée pour les
    fichiers emf et wmf.
&EN ask =
&EN2 A2M_DESTGDIPRT : affichage du panneau pour sélection des paramétres
                     d'impression
&EN2 A2M_DESTGDIEMF : non utilisé
&EN2 A2M_DESTGDIWMF : non utilisé
&EN2 A2M_DESTRTF    : génération pour une aide Windows (hcw)
&EN2 A2M_DESTHTML   : génération pour une aide Html Help (hhw)
&EN2 A2M_DESTFRM    : non utilisé
&EN2 A2M_DESTA2M    : append au fichier outfile au lieu de le remplacer
&EN2 A2M_DESTCSV    : non utilisé
&EN2 A2M_DESTDUMMY  : non utilisé
&EN2 A2M_DESTTCHRT  : affichage via TeeChart

&EX
    #include <s_a2m.h>

    A2MFILE *af;

    af = A2mMemBegin(A2M_DESTRTF, "tmp", 0);
    if(af == 0) return;
    A2mMemRecord(af, "Avant le fichier\n\n");
    A2mMemFlush(af);
    A2mMemRecordFile(af, "gb.a2m");
    A2mMemFlush(af);
    A2mMemRecord(af, "Aprés le fichier\n\n");
    A2mMemFlush(af);
    A2mMemEnd(af);
&TX

&RT Un pointeur vers un descripteur de fichier a2m ou 0 en cas d'erreur.

&SA A2mMemFlush(), A2mMemRecord(), A2mMemEnd()
========================================================================= */

#ifdef SCRCPP
A2MFILE *A2mMemBegin(
int     dest,
U_ch    *outfile,
int     ask
)
#else
A2MFILE *A2mMemBegin(dest, outfile, ask)
int     dest;
U_ch    *outfile;
int     ask;
#endif
{
    A2MFILE     *af = 0;

    af = A2mOpen("", 2);
    if(af == 0) return(af);
    af->af_outfile = SCR_stracpy(outfile);
    af->af_dest = dest;

    switch(dest) {
#if defined(DOSWIN) || defined(SCRW32) || defined(WINDOWS)
        case A2M_DESTGDIPRT :
            if(A2mGdiPrinterInit(0L, ask, outfile)) goto err;
            break;
#ifndef NOEMF // Suppression des EMF/WMF pour iodew
        case A2M_DESTGDIEMF :
            if(A2mGdiEMFInit(outfile, 120, 90)) goto err;
            break;
        case A2M_DESTGDIWMF :
            if(A2mGdiWMFInit(outfile, 120, 90)) goto err;
            break;
#endif
#endif            
        case A2M_DESTRTF :
            if(A2mRtfInit(outfile, ask)) goto err;
            break;
        case A2M_DESTHTML :
            if(A2mHtmlInit(outfile, ask)) goto err; /* JMP 29-04-99 */
            break;
        case A2M_DESTFRM :
            if(A2mFrInit(outfile)) goto err;
            break;
        case A2M_DESTA2M :
            if(ask)
                af->af_fda2m = fopen(outfile, "a+");
            else
                af->af_fda2m = fopen(outfile, "w+");
            if(af->af_fda2m == 0) goto err;
            break;
        case A2M_DESTCSV :
            if(A2mCsvInit(outfile)) goto err;
            break;
        case A2M_DESTTCHRT :
            if(A2mChrtInit(outfile)) goto err;
            break;
        case A2M_DESTDUMMY : break;
        default : return(af);
        }

    return(af);
err:
    A2mClose(af);
    return(af = 0);
}

/* ======================================================================
Mémorise str comme suite du texte a2m pour le descripteur af. Ce
descripteur est créé à l'aide de la fonction A2mMemBegin(). Lorsqu'un
objet est terminé, on peut utiliser la fonction A2mMemFlush() pour
interpréter le texte enregisté et envoyer vers la destination prévue.

&EX
    #include <s_a2m.h>

    A2MFILE *af;

    af = A2mMemBegin(A2M_DESTRTF, "tmp", 0);
    if(af == 0) return;
    A2mMemRecord(af, "Avant le fichier\n\n");
    A2mMemFlush(af);
    A2mMemRecordFile(af, "gb.a2m");
    A2mMemFlush(af);
    A2mMemRecord(af, "Aprés le fichier\n\n");
    A2mMemFlush(af);
    A2mMemEnd(af);
&TX

&SA A2mMemBegin(), A2mMemFlush(), A2mMemRecordFile(), A2mMemEnd()
========================================================================= */

A2mMemRecord(af, str)
A2MFILE *af;
U_ch    *str;
{
    if(af->af_dest == A2M_DESTDUMMY) return(0);

    if(af->af_dest == A2M_DESTA2M)
        fprintf(af->af_fda2m, "%s", str);
    else
        CppRecordLILO(af->af_cpp, str);

    return(0);
}

/* ======================================================================
Mémorise le contenu du fichier filename comme suite du texte a2m pour le
descripteur af. Ce descripteur est créé à l'aide de la fonction
A2mMemBegin(). Lorsqu'un objet est terminé, on peut utiliser la fonction
A2mMemFlush() pour interpréter le texte enregisté et envoyer vers la
destination prévue.

&EX
    #include <s_a2m.h>

    A2MFILE *af;

    af = A2mMemBegin(A2M_DESTRTF, "tmp", 0);
    if(af == 0) return;
    A2mMemRecord(af, "Avant le fichier\n\n");
    A2mMemFlush(af);
    A2mMemRecordFile(af, "gb.a2m");
    A2mMemFlush(af);
    A2mMemRecord(af, "Aprés le fichier\n\n");
    A2mMemFlush(af);
    A2mMemEnd(af);
&TX

&SA A2mMemBegin(), A2mMemFlush(), A2mMemRecord(), A2mMemEnd()
========================================================================= */

A2mMemRecordFile(af, filename)
A2MFILE *af;
U_ch    *filename;
{
    FILE    *fd;
    U_ch    buf[512];

    if(af->af_dest == A2M_DESTDUMMY) return(0);
    fd = fopen(filename, "r");
    if(fd == 0) return(-1);
    while(1) {
        if(SCR_read_line(fd, buf, 511) < 0) break;
        strcat(buf, "\n");
        if(af->af_dest == A2M_DESTA2M)
            fprintf(af->af_fda2m, "%s", buf);
        else
            CppRecordLILO(af->af_cpp, buf);
        }
    fclose(fd);
    return(0);
}


/* ======================================================================
Imprime les objets enregistrés à l'aide de la fonction A2mMemRecord()
sur la destination associée au descripteur af.

Il ne faut utiliser cette fonction que lorsqu'un ou plusieurs objets
sont complétement mémorisés. Si cette fonction est appelée alors qu'une
partie d'un objet est en enregistrée, soit l'objet n'est pas imprimé
(tableaux, graphiques), soit il est découpé (paragraphe).

&EX
    #include <s_a2m.h>

    A2MFILE *af;

    af = A2mMemBegin(A2M_DESTRTF, "tmp", 0);
    if(af == 0) return;
    A2mMemRecord(af, "Avant le fichier\n\n");
    A2mMemFlush(af);
    A2mMemRecordFile(af, "gb.a2m");
    A2mMemFlush(af);
    A2mMemRecord(af, "Aprés le fichier\n\n");
    A2mMemFlush(af);
    A2mMemEnd(af);
&TX

&SA A2mMemBegin(), A2mMemFlush(), A2mMemRecord(), A2mMemEnd()
========================================================================= */

A2mMemFlush(af)
A2MFILE *af;
{
    A2MOBJ  *ao;

    if(af->af_dest == A2M_DESTDUMMY) return(0);

    while(1) {
        ao = A2mRead(af);
        if(ao == 0) break;
        switch(af->af_dest) {
#if defined(DOSWIN) || defined(SCRW32) || defined(WINDOWS)
            case A2M_DESTGDIPRT :
            case A2M_DESTGDIWMF :
            case A2M_DESTGDIEMF : A2mGdiPrintObj(ao); break;
#endif
            case A2M_DESTRTF    : A2mRtfPrintObj(ao); break;
            case A2M_DESTHTML   : A2mHtmlPrintObj(ao, 1); break; /* JMP 19-10-99 */
            case A2M_DESTFRM    : A2mFrPrintObj(ao); break;
            case A2M_DESTCSV    : A2mCsvPrintObj(ao); break;
            case A2M_DESTTCHRT  : A2mChrtPrintObj(ao); break;
            default : break;
            }
        }
    return(0);
}

/* ======================================================================
Termine une impression en mémoire au format A2M. Le descripteur af est
créé à l'aide de la fonction A2mMemBegin().

Tous les objets enregistrés sont envoyés vers la destination d'impression
déterminée lors de la création du descripteur af.

&EX
    #include <s_a2m.h>

    A2MFILE *af;

    af = A2mMemBegin(A2M_DESTRTF, "tmp", 0);
    if(af == 0) return;
    A2mMemRecord(af, "Avant le fichier\n\n");
    A2mMemFlush(af);
    A2mMemRecordFile(af, "gb.a2m");
    A2mMemFlush(af);
    A2mMemRecord(af, "Aprés le fichier\n\n");
    A2mMemFlush(af);
    A2mMemEnd(af);
&TX

&SA A2mMemBegin(), A2mMemFlush(), A2mMemRecord()
========================================================================= */

A2mMemEnd(af)
A2MFILE *af;
{
    A2mMemFlush(af);
    switch(af->af_dest) {
#if defined(DOSWIN) || defined(SCRW32) || defined(WINDOWS)
        case A2M_DESTGDIPRT :
            A2mGdiPrintObj((A2MOBJ *)0);
            A2mGdiPrinterEnd();
            break;
#ifndef NOEMF // Suppression des EMF/WMF pour iodew
        case A2M_DESTGDIEMF :
            A2mGdiPrintObj((A2MOBJ *)0);
            A2mGdiEMFEnd();
            break;
        case A2M_DESTGDIWMF :
            A2mGdiPrintObj((A2MOBJ *)0);
            A2mGdiWMFEnd();
            break;
#endif
#endif
        case A2M_DESTRTF :
            A2mRtfEnd(af->af_outfile);
            break;
        case A2M_DESTHTML :
            A2mHtmlEnd();
            break;
        case A2M_DESTFRM :
            A2mFrEnd(af->af_outfile);
            break;
        case A2M_DESTA2M :
            fclose(af->af_fda2m);
            break;
        case A2M_DESTCSV :
            A2mCsvEnd();
            break;
        case A2M_DESTTCHRT :
            A2mChrtEnd(-1);
            break;
        case A2M_DESTDUMMY :
        default :
            break;
        }

    A2mClose(af);
    return(0);
}













