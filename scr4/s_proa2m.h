#ifdef __cplusplus
extern "C" {
#endif



/* s_a2mrd.c */
extern A2MFILE *A2mOpen(char *filename,int type);
extern int A2mClose(A2MFILE *af);
extern A2MPAR *A2mNewPar(char *tag);
extern int A2mSetParTag(A2MPAR *ap,unsigned char *tag);
extern int A2mFreePar(A2MPAR *ap);
extern int A2mFreeStrs(A2MSTR **as);
extern A2MTBL *A2mNewTbl(void);
extern A2MTOP *A2mNewTopic(void);
extern int A2mFreeTopic(A2MTOP *atp);
extern A2MPAGE *A2mNewPage(void);
extern int A2mFreePage(A2MPAGE *apg);
extern A2MLINE *A2mNewLine(void);
extern int A2mFreeLine(A2MLINE *al);
extern int A2mAddTblLine(A2MTBL *at,int type,int pos);
extern int A2mSetTblNc(A2MTBL *at,int nc);
extern int A2mFreeTbl(A2MTBL *at);
extern A2MGRF *A2mNewGrf(void);
extern int A2mFreeGrids(A2MGRID **grids);
extern int A2mFreeLabs(A2MLAB **labs);
extern int A2mFreeGXY(A2MGXY **gxy);
extern int A2mFreeGrf(A2MGRF *ag);
extern A2MOBJ *A2mNewObj(int type);
extern int A2mFreeObj(A2MOBJ *ao);
extern int A2mReadVar(A2MFILE *af,unsigned char **var);
extern int A2mReadInt(A2MFILE *af);
extern int A2mDefCh(A2MFILE *af,int *escch);
extern int A2mDefPar(A2MFILE *af,char *parname);
extern A2MOBJ *A2mReadLine(A2MFILE *af,int type);
extern A2MOBJ *A2mReadPage(A2MFILE *af);
extern A2MOBJ *A2mReadTopic(A2MFILE *af);
extern A2MOBJ *A2mReadPar(A2MFILE *af);
extern int A2mSupdbl(unsigned char *txt,int ljust);
extern int A2mReadTblLine(A2MFILE *af,A2MTBL *at,int pos);
extern int A2mReadTblWidth(A2MFILE *af,A2MTBL *at);
extern int A2mReadParLine(A2MFILE *af,A2MPAR **ap);
extern A2MOBJ *A2mReadTbl(A2MFILE *af);
extern int A2mEndTbl(A2MTBL* at);
extern int A2mReadGrTxt(A2MFILE *af,unsigned char **txt);
extern double A2mRead1Dbl(A2MFILE *af);
extern int A2mReadGrGrids(A2MFILE *af,A2MGRID ***grds);
extern int A2mReadGrLabs(A2MFILE *af,A2MLAB ***labs);
extern int A2mReadGrGrid(A2MFILE *af,short *grid);
extern int A2mReadGrXY(A2MFILE *af,A2MGXY ***gxys,int axis);
extern int A2mReadAlign(A2MFILE *af);
extern int A2mReadGrTY(A2MFILE *af,A2MGXY ***gxys,int axis);
extern int A2mSkipBrackets(A2MFILE *af);
extern A2MOBJ *A2mReadGraph(A2MFILE *af);
extern A2MOBJ *A2mReadCmd(A2MFILE *af);
extern int A2mReadCmdNoObj(A2MFILE *af,char str[]);
extern int A2mReadTShading(A2MFILE *af);
extern A2MOBJ *A2mRead(A2MFILE *af);
extern int A2mAddParStrAlloc(A2MSTR ***as,int *nl);
extern int A2mAddParStr(A2MSTR ***as,int *nl,A2MSTR *as_proto);
extern A2MSTR **A2mAddParLine(A2MSTR **as,unsigned char *txt);
extern unsigned char *A2mError(A2MFILE *af);
extern int A2mFindPpr(char *partag);
extern A2MPPR *A2mCreatePpr(char *partag);
extern int A2mReadParString(A2MFILE *af,unsigned char *str);
extern int A2mReadParVal(A2MFILE *af);
extern int A2mReadParChars(A2MFILE *af,char *chars,int upper);
extern int A2mReadParChar(A2MFILE *af);
extern int A2mReadParAlias(A2MFILE *af,A2MPPR *pp);
extern int A2mReadParDef(A2MFILE *af);
extern A2MPAR *A2mTxt2Par(char *tag,char *txt);
extern int A2mReadTopics(char* a2mfile);
extern int A2mFreeTopics();
extern int A2mReadA2mIni(char *filename);
extern int A2mAnalysePeriod(char *str,double *period,double *skip);
extern int A2mHHReplace1(U_ch* res, U_ch* txt, U_ch* txtu, A2MTOP* atp);
extern int A2mFreeAnchors();
extern int A2mFindAnchor(char *ref);
extern int A2mFreeFNotes();
extern int A2mAddFNote(char* ref);

/* s_a2mseg.c */
extern int A2mCreateParCatalog(A2MFILE *af);
extern int A2mSetParProps(char *tag,A2MPPR *pp);
extern int A2mSetFontProps(unsigned char *ctag,A2MFNT *fnt);
extern int A2mSetFont(A2MPAR *ap,A2MSTR *as,A2MFNT *fnt);
extern int A2mSplitStrs(A2MPAR *ap);
extern int A2mSplitStr(A2MSTR ***pas2,int *pn,unsigned char *txt,A2MFNT *fnt);
extern int A2mCalcNumbering(int numb,unsigned char *txt);
extern int A2mFrCatFile(FILE *fdout,char *outfile,char *ext);

/* s_a2mhtm.c */
extern int A2mToHtml(char *a2mfile,char *outfile, int wh);
extern int A2mToHtmlStrip(char* a2mfile, char* outfile, int strp, int wh);
extern int A2mHtmlInit(char *outfile,int wh);
extern int A2mHtmlEnd(void);
extern int A2mHtmlPrintObj(A2MOBJ *ao, int del);
extern int A2mHtmlCalcBlc(A2MPAR *ap);
extern int A2mHtmlOpenTab(int blc);
extern int A2mHtmlPrintPar(A2MPAR *ap,int tbl,int tcol);
extern int A2mHtmlFindTag(A2MPPR *pp,char *tag);
extern int A2mHtmlCalcTagFnt(char *tag,A2MFNT *fnt);
extern int A2mHtmlPrintStr(A2MPAR *ap,int n,A2MFNT *fntt, int tbl);
extern int A2mHtmlResetFont(void);
extern int A2mHtmlPrintAttr(A2MPAR *ap,int n,A2MFNT *fntt);
extern int A2mHtmlPrintChar(FILE *fd, int ch);
extern int A2mHtmlPrintTbl(A2MTBL *at);
extern int A2mHtmlPrintGrf(A2MGRF *ag);
extern int A2mHtmlAFrame(char *filename);
extern int A2mHtmlReadIni(char *filename);
extern int A2mHtmlHhp(char* outfile);
extern int A2mHtmlPrintFNotes();
extern int A2mHtmlPrintTopicCnt(A2MTOP *atp);
extern int A2mHtmlPrintTopic(A2MTOP* atp, int alias);
extern int A2mHtmlPrintTopicKey(A2MTOP* atp);
extern int A2mHtmlAnchor(char* ref);
extern int A2mHtmlFNote(char* ref);
extern int A2mReadAnchor(A2MFILE *af, int save);

/* s_a2mmsg.c */
extern void A2mMessage(char *msg);
/* Public A2mMessage */
extern void (*A2mMessage_super)(char* msg); // if not null, replaces the default implementation A2mMessage()
extern void A2mMessage_toggle(int IsOn);

/* s_a2mcsv.c */
extern int A2mToCsv(char *a2mfile,char *outfile);
extern int A2mCsvInit(char *outfile);
extern int A2mCsvEnd(void);
extern int A2mCsvPrintObj(A2MOBJ *ao);
extern int A2mCsvPrintPar(A2MPAR *ap,int tbl,int tcol);
extern int A2mCsvPrintStr(A2MPAR *ap,int n);
extern int A2mCsvPrintChar(int ch);
extern int A2mCsvPrintTbl(A2MTBL *at);
extern int A2mCsvPrintGrf(A2MGRF *ag);
extern int A2mCsvAFrame(char *filename);
extern int A2mCsvReadIni(char *filename);

/* s_a2mrtf.c */
extern int A2mToRtf(char *a2mfile,char *outfile,int wh);
extern int A2mRtfInit(char *outfile,int wh);
extern int A2mRtfEnd(char *outfile);
extern int A2mRtfPrintObj(A2MOBJ *ao);
extern int A2mRtfHpj(char *outfile);
extern int A2mRtfPrintTopic(A2MTOP *atp);
extern int A2mRtfPrintCntTxt(unsigned char *txt);
extern int A2mRtfPrintTopicCnt(A2MTOP *atp);
extern int A2mRtfPrintPar(A2MPAR *ap,int tbl);
extern int A2mRtfSetParProps(A2MPAR *ap,int tbl);
extern int A2mRtfPrintTxt(FILE *fd,unsigned char *txt);
extern int A2mRtfPrintStr(A2MPAR *ap,int n);
extern int A2mRtfPrintStrImage(A2MPAR *ap,int n);
extern int A2mRtfPrintAttr(A2MPAR *ap,int n);
extern int A2mRtfPrintChar(FILE *fd,int ch);
extern int A2mRtfPrintTbl(A2MTBL *at);
extern int A2mRtfPrintTblLines(A2MTBL *at,int hbf);
extern int A2mRtfCellxs(A2MTBL *at,int line,int hbf,int hline);
extern int A2mRtfCalcFirstLine(A2MTBL *at,int hbf);
extern int A2mRtfCalcLastLine(A2MTBL *at,int hbf);
extern int A2mRtfPrintGrf(A2MOBJ *ao);
extern int A2mRtfAFrame(char *filename,int type);
extern int A2mRtfHeader(void);
extern int A2mRtfStyles(void);
extern int A2mRtfReadIni(char *filename);
extern void A2mRtfPrintPgHeadFoot(U_ch * txt, int type);
extern int A2mRtfPrintPage(A2MPAGE* ap, int tbl);


/* s_a2mmem.c */
extern A2MFILE *A2mMemBegin(int dest,unsigned char *outfile,int ask);
extern int A2mMemRecord(A2MFILE *af,unsigned char *str);
extern int A2mMemRecordFile(A2MFILE *af,unsigned char *filename);
extern int A2mMemFlush(A2MFILE *af);
extern int A2mMemEnd(A2MFILE *af);

/* s_a2merr.c */
extern int A2mPrintError(A2MFILE *af,unsigned char *msg);

/* s_a2mfrm.c */
extern FILE *A2mFrOpenFile(char *outfile,char *ext);
extern int A2mFrUnlinkFiles(char *outfile);
extern int A2mToMif(char *a2mfile,char *outfile);
extern int A2mFrInit(char *outfile);
extern int A2mFrPrintObj(A2MOBJ *ao);
extern int A2mFrEnd(char *outfile);
extern int A2mFrPrintPar(A2MPAR *ap,int dest,int shift,int tbl,int grf,int just, U_ch *var);
extern int A2mFrPrintStr(FILE *fd,A2MPAR *ap,int n);
extern int A2mFrPrintChar(FILE *fd,int ch);
extern int A2mFrAFrame(FILE *fd,char *filename,int type);
extern int A2mFrPrintAttr(FILE *fd,A2MPAR *ap,int n);
extern int A2mFrPrintTbl(A2MTBL *at);
extern int A2mFrCalcShading(int val);
extern char *A2mFrRuling(int line);
extern int A2mFrPrintTblFmt(FILE *fd,A2MTBL *at);
extern int A2mFrPrintTblLines(A2MTBL *at,int hbf);
extern int A2mFrFontCatalog(void);
extern char *A2mFrAlignment(int just);
extern char *A2mFrNoYes(int ny);
extern char *A2mFrPlacement(int newpage);
extern char *A2mFrFontFamily(int family);
extern int A2mFrCatalogPar(void);
extern int A2mFrCatalog1Par(FILE *fd,A2MPPR *pp);
extern int A2mFrmReadIni(char *filename);
extern int A2mFrPrintGrf(A2MGRF *agrf);
extern int A2mFrGrfInit(A2MGRF *ag);
extern int A2mFrGrfEnd(A2MGRF *ag);
extern int A2mFrGrfPen(A2MPEN *pen);
extern int A2mFrGrfBrush(A2MBRUSH *brush);
extern int A2mFrGrfLine(int axis,double x1,double y1,double x2,double y2,int props);
extern int A2mFrGrfText(int axis,double x,double y,char *string,char *align,char *color);
extern int A2mFrGrfPar(int axis,double x,double y,A2MPAR *par,char *align,char *color);
extern int A2mFrGrfGroupObj(void);
extern int A2mFrGrfGroup(void);
extern int A2mFrGrfBox(int axis,double x,double y,double w,double h,int props);
extern int A2mFrGrfPolyLine(int axis,int nobs,double *vals,int props);
extern int A2mFrGrfPolyBar(int axis,int nobs,double *vals,double width,int props);
extern int A2mFrGrfPrepare(void);
extern int A2mFrGrfGetPen(int props,A2MPEN *pen);
extern int A2mFrGrfGetBrush(int props,A2MBRUSH *brush);

/* s_a2mgrf.c */
extern int A2mGrfPrint(A2MGRF *agrf);
extern int A2mGrfInit(A2MGRF *ag);
extern int A2mGrfEnd(A2MGRF *ag);
extern int A2mGrf(A2MGRF *agrf);
extern int A2mGrfTitle(A2MGRF *agrf);
extern int A2mGrfLegend(A2MGRF *agrf);
extern int A2mGrfBarNbr(A2MGRF *agrf);
extern int A2mGrfGrid(A2MGRF *agrf);
extern int A2mGrfXy(A2MGRF *agrf);
extern int A2mGrfBeginObj(void);
extern int A2mGrfEndObj(void);
extern int A2mGrfGroup(void);
extern int A2mGrfPlot(A2MGRF *agrf);
extern int A2mGrfOrigin(double *left,double *right);
extern int A2mGrfTranslate(double x,double y,double z);
extern int A2mGrfBox(int axis,double x,double y,double w,double h,int props);
extern int A2mGrfLine(int axis,double x1,double y1,double x2,double y2,int props);
extern int A2mGrfPlotGrid(A2MGRF *agrf,double *left,double *right,double *incr,int *nbdec);
extern int A2mGrfGridLine(A2MGRF *agrf,int type,double left,double right,double span,A2MPAR *text,int dir, double maxi, int level);
extern int A2mGrfPar(int axis,double x1,double y1,A2MPAR *par,char *align,char *color);
extern int A2mGrfPlotXy(int axis,int type,int nobs,double *vals,int props);
extern int A2mGrfPolyBar(int axis,int nobs,double *vals,double width,int props);
extern int A2mGrfMins(A2MGRF *agrf,double mins[],double maxs[]);
extern int A2mGrfIncr(A2MGRF *agrf,int grid,double mini,double maxi,double *incr,int *nbdec,double *left,double *right,double *lbar);
extern int A2mGrfLabel(int nbr,int type,A2MPAR *title,int props);
extern int A2mGrfPolyLine(int axis,int nobs,double *vals,int props);

#if defined(DOSWIN) || defined(SCRW32) || defined(WINDOWS)
/* s_a2mgdi.c */
extern int A2mGdiPrinterInit(HWND hWndOwner,int Dlg,unsigned char *JobTitle);
extern int A2mGdiAllInit(void);
extern int A2mGdiPrinterEnd(void);
extern int A2mGdiAllEnd(void);
extern int A2mToGdiPrinter(HWND hWndOwner,int Dlg,unsigned char *JobTitle,unsigned char *a2mfile);
extern int A2mGdiPrintObj(A2MOBJ *ao);
extern int A2mCalcObjSize(A2MOBJ *ao,int w);
extern int A2mGdiWriteObj(A2MOBJ *ao);
extern int A2mGdiPrepPar1(A2MPAR *ap);
extern int A2mGdiPrepPar2(A2MPAR *ap,int w);
extern int A2mPtsToPix(int pts);
extern int A2mGdiWritePar(A2MPAR *ap,int x,int y,int w,int just);
extern int A2mGdiCalcLineWH(A2MSTR **as,int from,int *w,int *h);
extern int A2mGdiPrintLine(A2MSTR **as,int from,int x,int y);
extern int A2mGdiCalcRect(A2MPAR *ap,int marg);
extern int A2mGdiCalcSubRect(A2MSTR **as,int from,int to,long *w,long *h);
extern int A2mGdiSetFont(A2MFNT *fnt);
extern int A2mGdiCalcSizes(A2MPAR *ap);
extern int A2mGdiPrepTbl1(A2MTBL *at);
extern int A2mGdiPrepTblWs(A2MTBL *at,int w);
extern int A2mGdiPrepTbl2(A2MTBL *at);
extern int A2mGdiCalcTblH(A2MTBL *at);
extern int A2mGdiTblTitleH(A2MTBL *at);
extern int A2mGdiTblFootnoteH(A2MTBL *at);
extern int A2mGdiSubTblH(A2MTBL *at,int from,int nc,int hbf);
extern int A2mGdiCalcTblW(A2MTBL *at,int from,int nc);
extern int A2mWriteTblHBFwr(A2MTBL *at,int from,int nc,int hbf,int i,int wr);
extern int A2mWriteTblHBF(A2MTBL *at,int from,int nc,int hbf);
extern int A2mWriteTblCellwr(A2MTBL *at,int line,int from,int j,int nc,int *cx,int cy,int *ch,int wr);
extern int A2mGdiWriteSubTbl(A2MTBL *at,int from,int nc,int w);
extern int A2mGdiWriteTbl(A2MTBL *at);
extern int A2mFormFeed(void);
extern int A2mPageTitle(unsigned char *txt,int yt,int yr,int just);
extern int A2mPageHead(void);
extern int A2mPageFoot(void);
extern int A2mGdiReadIni(char *filename);
extern int A2mGdiPrepGrf(A2MGRF *ag);
extern int A2mGdiWriteGrf(A2MGRF *ag);
extern int A2mGdiTsf(double x1,double y1,int *gx1,int *gy1);
extern int A2mGdiGrfInit(A2MGRF *ag);
extern int A2mGdiGrfEnd(A2MGRF *ag);
extern int A2mGdiGrfLine(int axis,double x1,double y1,double x2,double y2,int props);
extern int A2mGdiGrfPar(int axis,double x,double y,A2MPAR *par,char *align,char *color);
extern int A2mGdiGrfGroupObj(void);
extern int A2mGdiGrfGroup(void);
extern int A2mGdiGrfBox(int axis,double x,double y,double w,double h,int props);
extern int A2mGdiGrfPolyLine(int axis,int nobs,double *vals,int props);
extern int A2mGdiGrfPolyBar(int axis,int nobs,double *vals,double width,int props);
extern int A2mGdiSetPen(int pen);
extern int A2mGdiGrfPrepare(void);

/* s_a2memf.c */
extern int A2mGdiEMFInit(unsigned char *filename,int w,int h);
extern int A2mGdiEMFEnd(void);
extern int A2mGdiWMFEnd(void);
extern int A2mGdiWMFInit(unsigned char *filename,int w,int h);
extern int A2mToGdiEMF(unsigned char *a2mfile,unsigned char *outfile,int w,int h);
extern int A2mGdiEMFGrf(char *a2mfile,char *outfile);

/* s_a2mswf.c */
extern int A2mSWF_HTML(FILE* fd, int w, int h, A2MGRF* grf, int cnt);
extern int A2mSWFIni();

/* s_a2mgif.c */
extern int A2mGIF_HTML(A2MGRF *go, U_ch* filename); 

/* s_a2mmd.c */
extern int A2mToMD(char* a2mfile, char* outfile);
extern int A2mMDInit(char* outfile);
extern U_ch *A2mReadFile(char *filename);
extern int A2mMDEnd(char* outfile);
extern int A2mMDPrintObj(A2MOBJ* ao, int del);
extern int A2mMDPrintTopic(A2MTOP *atp, int alias);
extern int A2mMDCalcBlc(A2MPAR* ap);
extern U_ch* A2mMDCalcResetAttrSTATIC();
extern U_ch* A2mMDCalcAttrSTATIC(A2MPAR* ap, int n, A2MFNT* fntt);
extern int A2mMDPrintAttr(A2MPAR* ap, int n, A2MFNT* fntt);
extern U_ch* A2mMDCalcCharSTATIC(int ch, int code);
extern char* A2mMDAFrame(char* filename);
extern char *A2mMDFNote(char* ref);
extern int A2mMDPrintFNotes();
extern U_ch* A2mMDCalcStr(A2MPAR* ap, int n, A2MFNT *fntt, int tbl, int code);
extern U_ch* A2mMDCalcStrs(A2MPAR* ap, A2MFNT* fnt, int tbl, int code);
extern U_ch *A2mMDCalcPar(A2MPAR* ap, int tbl, int tcol);
extern int A2mMDPrintPar(A2MPAR* ap, int tbl, int tcol);
extern int A2mMDFindTag(A2MPPR* pp, char* tag);
extern int A2mMDCalcTagFnt(char* tag, A2MFNT* fnt);
extern int A2mMDGetCenter(A2MPAR *ap);
extern int A2mMDPrintTbl(A2MTBL* at);
extern int A2mMDPrintGrf(A2MGRF* ag);
extern int A2mMDAnchor(char* ref);
extern char* A2mMDacatff(char* txt1, char* txt2);

/* s_a2mtch.c */
extern int A2mChrtInit();
extern int A2mChrtPrintObj(A2MOBJ *ao);
extern int A2mChrtEnd(int hdl);
extern int A2mChrtPrintGrf(int hdl, A2MGRF* grf);

#endif /* DOSWIN ... */

#ifdef __cplusplus
}
#endif
