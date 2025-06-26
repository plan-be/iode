#include "s_a2m.h"

/******** Catalogue de paragraphes *********************/

int A2M_FONTSIZE = 10, A2M_FONTINCR = 2, A2M_TFONTSIZE = 8,
    A2M_FONTFAMILY = 'H', A2M_TFONTFAMILY = 'H',
    A2M_DFT_LMARGIN = 0; /* JMP 23-03-98 */

//int A2M_MAXPARLEVEL = 3;
int A2M_MAXPARLEVEL = 7; // JMP 11/1/2017

int A2mCreateParCatalog(af)
A2MFILE *af;
{
    CPPFILE *cpp = af->af_cpp;
    U_ch    buf[512];
    int     size, i;

    if(A2M_FONTSIZE < 6) A2M_FONTSIZE = 6;
    if(A2M_TFONTSIZE < 4) A2M_TFONTSIZE = 4;
    if(A2M_FONTINCR < 2) A2M_FONTINCR = 2;

    // Titres
    for(i = A2M_MAXPARLEVEL ; i >= 3 ; i--) {
	sprintf(buf,".pardef tit_%d copy=tit_3 html=%d number=%d\n",  i, i + 1, i + 1);
	CppRecord(cpp, buf);
	}
    size = A2M_FONTSIZE + A2M_FONTINCR;
    sprintf(buf, ".pardef tit_2 copy=tit_1 html=3 spaceb=%d spacea=%d size=%d number=3\n", size, size/2, size);
    CppRecord(cpp, buf);
    size = A2M_FONTSIZE + 2 * A2M_FONTINCR;
    sprintf(buf, ".pardef tit_1 html=2 bold=y spaceb=%d spacea=%d keepwn=y size=%d number=2 lmargin=%d family=%c\n", size, size/2, size, A2M_DFT_LMARGIN, A2M_FONTFAMILY);
    CppRecord(cpp, buf);
    size = A2M_FONTSIZE + 4 * A2M_FONTINCR;
    sprintf(buf, ".pardef tit_0 html=1 spaceb=%d spacea=%d bold=y keepwn=y size=%d number=1 justify=center newpage=y family=%c\n", size, size/2, size, A2M_FONTFAMILY);
    CppRecord(cpp, buf);

    // Enums
    for(i = A2M_MAXPARLEVEL ; i > 0 ; i--) {
	sprintf(buf, ".pardef enum_%d copy=enum_0 lmargin=%d\n", i, 24 + 8 * i);
	CppRecord(cpp, buf);
	}
    CppRecord(cpp, ".pardef enum_0 copy=par_0 lmargin=24 bullet=y\n");

    // Courrier avec shift
    for(i = A2M_MAXPARLEVEL ; i >= 0 ; i--) {
	sprintf(buf, ".pardef cmde_%d copy=cmd_0 lmargin=%d\n", i, A2M_DFT_LMARGIN + 16 + 8 * i);
	CppRecord(cpp, buf);
	}
    // Courrier à la marge
    for(i = A2M_MAXPARLEVEL ; i > 0 ; i--) {
	sprintf(buf, ".pardef cmd_%d copy=cmd_0\n", i);
	CppRecord(cpp, buf);
	}

    size = A2M_FONTSIZE;
    sprintf(buf, ".pardef cmd_0 spaceb=2 spacea=4 size=%d color=green lmargin=%d family=courier bold=y family=C\n", size, A2M_DFT_LMARGIN);
    CppRecord(cpp, buf);

    // Notes, pari, pari2, parb, parb2
    size = A2M_FONTSIZE;
    sprintf(buf, ".pardef note spaceb=6 spacea=6 size=%d color=red lmargin=%d family=%c\n", size, A2M_DFT_LMARGIN, A2M_FONTFAMILY);
    CppRecord(cpp, buf);

    size = A2M_FONTSIZE; /* JMP 29-10-97 */
    sprintf(buf, ".pardef pari2 copy=pari spaceb=%d spacea=%d size=%d\n", size, size/2, size); /* JMP 29-10-97 */
    CppRecord(cpp, buf);

    size = A2M_FONTSIZE + A2M_FONTINCR;

    sprintf(buf, ".pardef paru2 copy=paru size=%d\n", size - 1);
    CppRecord(cpp, buf);
    sprintf(buf, ".pardef paru copy=par_0 italic=n bold=n underline=y keepwn=y spaceb=%d spacea=%d size=%d\n", size, size/2, size);
    CppRecord(cpp, buf);
    sprintf(buf, ".pardef pari2 copy=pari size=%d\n", size - 1);
    CppRecord(cpp, buf);
    sprintf(buf, ".pardef pari copy=par_0 italic=y bold=yes underline=n keepwn=y spaceb=%d spacea=%d size=%d\n", size, size/2, size);
    CppRecord(cpp, buf);
    sprintf(buf, ".pardef parb2 copy=parb size=%d\n", size - 1);
    CppRecord(cpp, buf);
    sprintf(buf, ".pardef parb copy=par_0 bold=y keepwn=y spaceb=%d spacea=%d size=%d\n", size, size/2, size);
    CppRecord(cpp, buf);

    size = A2M_FONTSIZE - A2M_FONTINCR;
    sprintf(buf, ".pardef PageNum italic=y lmargin=0 size=8 spacea=0 spaceb=0 family=Times\n");
    CppRecord(cpp, buf);

    sprintf(buf, ".pardef GraphFooter spaceb=0 spacea=0 lmargin=0 size=8 family=%c\n", A2M_TFONTFAMILY);
    CppRecord(cpp, buf);
    sprintf(buf, ".pardef GraphHeader spaceb=0 spacea=0 lmargin=0 size=8 bold=yes family=%c\n", A2M_TFONTFAMILY);
    CppRecord(cpp, buf);
    sprintf(buf, ".pardef GraphLegend spaceb=0 spacea=0 lmargin=0 size=8 italic=yes family=%c\n", A2M_TFONTFAMILY);
    CppRecord(cpp, buf);
    sprintf(buf, ".pardef GraphLabel spaceb=0 spacea=0 lmargin=0 size=8 family=%c\n", A2M_TFONTFAMILY);
    CppRecord(cpp, buf);
    sprintf(buf, ".pardef GraphTitle copy=TableTitle justify=center\n");
    CppRecord(cpp, buf);
    sprintf(buf, ".pardef GraphFootnote copy=TableFootnote\n");
    CppRecord(cpp, buf);

    sprintf(buf, ".pardef Table copy=TableTitle justify=left\n");
    CppRecord(cpp, buf);
    size = A2M_FONTSIZE;
    sprintf(buf, ".pardef TableTitle bold=y lmargin=0 size=%d spacea=%d spaceb=%d family=%c\n", size, size, size, A2M_FONTFAMILY); /* JMP 26-02-98 */
    CppRecord(cpp, buf);

    size = A2M_FONTSIZE;
    sprintf(buf, ".pardef TableFootnote italic=y lmargin=0 size=%d spacea=%d spaceb=%d family=%c\n", size, size, size/2, A2M_FONTFAMILY);
    CppRecord(cpp, buf);

    CppRecord(cpp, ".pardef HeaderLeft   copy=TableHeader justify=Left\n");
    CppRecord(cpp, ".pardef HeaderCenter copy=TableHeader justify=Center\n");
    CppRecord(cpp, ".pardef HeaderRight  copy=TableHeader justify=Right\n");
    CppRecord(cpp, ".pardef HeaderDec    copy=TableHeader justify=Decimal\n");
    CppRecord(cpp, ".pardef HeaderJust   copy=TableHeader justify=Justify\n");

    CppRecord(cpp, ".pardef FooterLeft   copy=TableFooter justify=Left\n");
    CppRecord(cpp, ".pardef FooterCenter copy=TableFooter justify=Center\n");
    CppRecord(cpp, ".pardef FooterRight  copy=TableFooter justify=Right\n");
    CppRecord(cpp, ".pardef FooterDec    copy=TableFooter justify=Decimal\n");
    CppRecord(cpp, ".pardef FooterJust   copy=TableFooter justify=Justify\n");

    CppRecord(cpp, ".pardef CellLeft   copy=TableCell justify=Left\n");
    CppRecord(cpp, ".pardef CellCenter copy=TableCell justify=Center\n");
    CppRecord(cpp, ".pardef CellRight  copy=TableCell justify=Right\n");
    CppRecord(cpp, ".pardef CellDecimal copy=TableCell justify=Decimal\n");
    CppRecord(cpp, ".pardef CellJust   copy=TableCell justify=Justify\n");

    CppRecord(cpp, ".pardef TableFooter copy=TableCell bold=y italic=yes\n");
    CppRecord(cpp, ".pardef TableHeader copy=TableCell bold=y\n");


    size = A2M_TFONTSIZE;
    sprintf(buf, ".pardef TableCell lmargin=0 size=%d spacea=0 spaceb=0 family=%c\n", size, A2M_TFONTFAMILY); /* JMP 26-02-98 */
    CppRecord(cpp, buf);

    // Para pour enum
    for(i = A2M_MAXPARLEVEL ; i >= 0 ; i--) {
	sprintf(buf, ".pardef pare_%d copy=par_0 lmargin=%d\n", i, A2M_DFT_LMARGIN + 16 + i * 8);
	CppRecord(cpp, buf);
	}

    // Para std : ts alignés sur la marge
    for(i = A2M_MAXPARLEVEL ; i > 0 ; i--) {
	sprintf(buf, ".pardef par_%d copy=par_0\n", i);
	CppRecord(cpp, buf);
	}

    size = A2M_FONTSIZE;
    sprintf(buf, ".pardef par_0 spaceb=2 spacea=4 size=%d lmargin=%d family=%c justify=justify\n",
		    size, A2M_DFT_LMARGIN, A2M_FONTFAMILY); /* JMP 12-02-99 */
    CppRecord(cpp, buf);

    // Newpage
    CppRecord(cpp, ".pardef newpage lmargin=0 size=2 spacea=0 spaceb=0 newpage=y\n"); /* JMP 17-01-02 */

    // Para pour Header et Footer
    sprintf(buf, ".pardef Header copy=footer\n");
    CppRecord(cpp, buf);
    sprintf(buf, ".pardef Footer family=%c size=%d justify=center\n", A2M_FONTFAMILY, A2M_FONTSIZE - 2);
    CppRecord(cpp, buf);

    return(0);
}

A2mSetParProps(tag, pp)
char    *tag;
A2MPPR  *pp;
{
    int     pprnb;

    if(tag == 0) tag = "";
    pprnb = A2mFindPpr(tag);
    if(pprnb < 0) pprnb = A2mFindPpr("par_1");
    if(pprnb < 0)
	memset(pp, 0, sizeof(A2MPPR));
    else
	memcpy(pp, A2M_PPRS + pprnb, sizeof(A2MPPR));

    return(0);
}

int A2mSetFontProps(ctag, fnt)
U_ch    *ctag;
A2MFNT  *fnt;
{
    return(0);
}

int A2mSetFont(ap, as, fnt)
A2MPAR  *ap;
A2MSTR  *as;
A2MFNT  *fnt;
{
    A2MFNT  *asfnt;

    memcpy(fnt, &(ap->ap_ppr.pp_fnt), sizeof(A2MFNT));
    if(as == 0) return(0);

    asfnt = &(as->as_fnt);
    A2mSetFontProps(as->as_ctag, fnt);

    fnt->af_size += asfnt->af_asize;
    if(asfnt->af_color    ) fnt->af_color    = asfnt->af_color;
    if(asfnt->af_family   ) fnt->af_family   = asfnt->af_family   ;
    if(asfnt->af_italic   ) fnt->af_italic   = asfnt->af_italic   ;
    if(asfnt->af_bold     ) fnt->af_bold     = asfnt->af_bold     ;
    if(asfnt->af_underline) fnt->af_underline= asfnt->af_underline;
    if(asfnt->af_strike   ) fnt->af_strike   = asfnt->af_strike   ;
    if(asfnt->af_pos      ) fnt->af_pos      = asfnt->af_pos      ;
    if(asfnt->af_caps     ) fnt->af_caps     = asfnt->af_caps     ;

    return(0);
}


/* Génère des segments de strings avec des séparateurs
    uniquement où c'est permis et adaptation des fonts */

int A2mSplitStrs(ap)
A2MPAR  *ap;
{
    A2MSTR  **as2 = 0, *as;
    A2MFNT  fnt;
    int     i, n = 0;
    U_ch    buf[64];

    A2mSetParProps(ap->ap_tag, &(ap->ap_ppr));

    if(A2M_NUMBERS && ap->ap_ppr.pp_number) {
	A2mCalcNumbering(ap->ap_ppr.pp_number - 1, buf);
	A2mSetFont(ap, ap->ap_strs[0], &fnt);
	A2mSplitStr(&as2, &n, buf, &fnt);
	}

    for(i = 0 ; ap->ap_strs[i] ; i++) {
	as = ap->ap_strs[i];
	A2mSetFont(ap, as, &fnt);
	switch(as->as_type) {
	    case A2M_NEWLINE :
		A2mAddParStrAlloc(&as2, &n);
		as2[n - 1]->as_type = A2M_NEWLINE;
		break;
	    case A2M_IMAGE   :
	    case A2M_IMAGEF  :
	    case A2M_IMAGER  :
	    case A2M_IMAGEFR :
		A2mAddParStrAlloc(&as2, &n);
		as2[n - 1]->as_type = A2M_IMAGE;
		as2[n - 1]->as_txt  = SCR_stracpy(as->as_txt);
		break;
	    case A2M_TAB     :
		A2mSplitStr(&as2, &n, " ", &fnt);
		break;
	    case A2M_TEXT    :
		A2mSplitStr(&as2, &n, as->as_txt, &fnt);
		break;
	    }
	}

    A2mAddParStrAlloc(&as2, &n);
    as2[n - 1]->as_type = A2M_SEP;
    A2mAddParStr(&as2, &n, 0L);
    ap->ap_as2 = as2;
    return(0);
}

int A2mSplitStr(pas2, pn, txt, fnt)
A2MSTR  ***pas2;
int     *pn;
U_ch    *txt;
A2MFNT  *fnt;
{
    A2MSTR  *as;
    int     i = 0;

    while(1) {
	if(txt[i] == 0) {
	    if(i == 0) return(0);
	    A2mAddParStrAlloc(pas2, pn);
	    as = (*pas2)[*pn - 1];
	    as->as_txt = SCR_stracpy(txt);
	    as->as_type = A2M_TEXT;
	    memcpy(&(as->as_fnt), fnt, sizeof(A2MFNT));
	    return(0);
	    }
	else if(U_is_in(txt[i], " ,+;*")) {
	    A2mAddParStrAlloc(pas2, pn);
	    as = (*pas2)[*pn - 1];
	    as->as_txt = SCR_malloc(i + 2);
	    memcpy(as->as_txt, txt, i + 1);
	    as->as_type = A2M_TEXT;
	    memcpy(&(as->as_fnt), fnt, sizeof(A2MFNT));
	    txt += i + 1;
	    i = 0;

	    A2mAddParStrAlloc(pas2, pn);
	    as = (*pas2)[*pn - 1];
	    as->as_type = A2M_SEP;
	    }
	else i++;
	}
}

int A2M_NUMB[10];
int A2M_NUMBERS = 1;

int A2mCalcNumbering(numb, txt)
int     numb;
U_ch    *txt;
{
    U_ch    buf[20];
    int     i;

    txt[0] = 0;
    for(i = numb ; i < sizeof(A2M_NUMB) / sizeof(A2M_NUMB[0]); i++)
	A2M_NUMB[i] = 0;
    if(numb == 0) return(0);

    A2M_NUMB[numb - 1]++;

    if(numb == 1)
	sprintf(txt, "%d. ", A2M_NUMB[0]);
    else {
	for(i = 0 ; i < numb - 1; i++) {
	    sprintf(buf, "%d.", A2M_NUMB[i]);
	    strcat(txt, buf);
	    }
	sprintf(buf, "%d ", A2M_NUMB[numb - 1]);
	strcat(txt, buf);
	}

    return(0);
}

int A2mFrCatFile(fdout, outfile, ext)
FILE    *fdout;
char    *outfile, *ext;
{
    char    buf[256];
    FILE    *fd;
    int     ch;

    SCR_change_ext(buf, outfile, ext);
    fd = fopen(buf, "r");
    if(fd == 0) return(-1);
    while(1) {
	ch = getc(fd);
	if(ch == EOF) break;
	putc(ch, fdout);
	}
    fclose(fd);
    return(0);
}






