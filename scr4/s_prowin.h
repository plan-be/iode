#ifdef __cplusplus
extern "C" {
#endif


/* s_win.c */
extern unsigned char **WscrCreateArgv(unsigned char *CmdLine);
extern int WscrDOSInit(void);
extern int WscrDOSCreateWin(void);
extern int WscrDOSEnd(void);
extern int WscrDOSCreateBrushes(void);
extern int WscrDOSSetFont(int n);
extern int WscrDOSSysMenu(HWND hWnd);
extern int WscrDOSFont(int n);
extern int WscrDosInitWH(HWND hWnd);
extern int WscrDOSMetrics(HWND hWnd,int *cx,int *cy);
extern int WscrDOSMove(HWND hWnd);
extern int WscrInvalidateCsr(int line,int col);
extern int WscrSetCursor(void);
extern int WscrDosText(HDC hDC,int line,int col,unsigned char *txt,int len,int attr);
extern int WscrDosPaint(HWND hWnd);
extern int WscrDosChar(int wParam);
extern int WscrDosSysChar(int wParam);
extern int WscrDosKeyUp(int wParam);
extern int WscrDosSysKeyUp(int wParam);
//extern long __stdcall WscrDOSProc(HWND hWnd,UINT wMsg,WPARAM wParam,LPARAM lParam);
extern LONG PASCAL WscrDOSProc(HWND hWnd,UINT wMsg,WPARAM wParam,LPARAM lParam);
extern int SCR_term_vkey(int val);
extern int SCR_init_term(void);
extern int SCR_end_term(void);
extern int SCR_clear_screen(void);
extern int SCR_force_attr(int attr);
extern int SCR_set_font(int font);
extern int SCR_set_color(int color);
extern int SCR_cursor_on(void);
extern int SCR_cursor_off(void);
extern int SCR_cursor_move(int line,int col);
extern int SCR_cursor_size(int size);
extern int SCR_map_box_2(void);
extern int SCR_print_text(char *text,int lg);
extern int SCR_get_no_echo(void);
extern int SCR_hit_key(void);
extern int SCR_set_alarm(int sec, void (*fn)());
extern int SCR_stop_alarm(void);
extern int SCR_restart_alarm(void);
extern int MS_sleep(long msecs);
extern int MS_getevent(void);
extern int MS_show_attr(void);
extern int MS_hide_attr(void);
extern int MS_hide(void);
extern int MS_show(void);
extern int WscrBeep(void);
extern int WscrEndPrintf(void);
extern int SCR_sleep(int ms);

/* s_wmain.c */
extern unsigned char **WscrCreateArgv(unsigned char *CmdLine);
extern int CALLBACK WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow);
extern int MainInit(int argc,unsigned char **argv);
extern HWND WscrCreateWscr(HMENU hMenu);
extern int MainEnd(void);

/* s_wdlg.c */
extern int WscrEditPage(PAGE *pg);
extern HWND WscrObjDisplay(PAGE *pg,int objnb,int status);
extern HWND WscrPgDisplay(PAGE *pg,int status);
extern int WscrObjUndisplay(PAGE *pg);
extern int WscrPgUndisplay(PAGE *pg);
extern int WpgEditPage(PAGE *pg);
extern int WscrDlgCommand(HWND hWnd,UINT iId,HWND hCtl,UINT nNotify);
extern int WscrActExec(int act_nu);
extern int WscrSendRecord(HWND hWnd);
extern int WscrDlgFnkey(HWND hWnd,PAGE *pg,int fld_nb,int winkey);
extern LONG __stdcall WscrDlgEditChild(HWND hWnd,UINT wMsg,WPARAM wParam,LPARAM lParam);
extern int WscrDlgInit(HWND hWnd,PAGE *pg,int props);
extern int WscrDlgSysCommand(HWND hWnd,WORD wP,int x,int y);
extern BOOL __stdcall WscrDlgProc(HWND hWnd,UINT wMsg,WPARAM wParam,LPARAM lParam);
extern int WscrEditDirFld(HWND hWnd,PAGE *pg,int fld_nb);
extern int WpgEnableFlds(HWND hWnd,PAGE *apg,int iScroll,BOOL status);

/* s_wproc.c */
extern BOOL wmClose(HWND hWnd,int ask);
extern int WscrGetMenuComment(HMENU hMenu,UINT uItem,UINT uFlags);
extern int WscrGetApplComment(HMENU hMenu,UINT uItem,UINT uFlags);
extern int WscrGetPgIsamComment(HMENU hMenu,UINT uItem,UINT uFlags);
extern int WscrGetSysMenuComment(HMENU hMenu,UINT uItem,UINT uFlags);
extern int WscrMenuSelect(HWND hWnd,HMENU hMenu,UINT uItem,UINT uFlags);
extern int WscrMenuChar(HWND hWnd,HMENU hMenu,UINT ch,UINT uFlags);
extern int WscrProcKeydown(HWND hWnd,int ch);
extern int WscrResizeCtrl(HWND hWnd,WPARAM wParam,LPARAM lParam);
extern int wmHelp(HWND hWnd);
extern long __stdcall WscrMainProc(HWND hWnd,UINT wMsg,WPARAM wParam,LPARAM lParam);

/* s_wcomp.c */
extern int SCR_MessageBox(unsigned char *title,unsigned char *msg,unsigned char *buts[]);
extern int SCR_MessageBox1(unsigned char *title,unsigned char *msg,unsigned char *buts[],int ncurb,int required);
extern int MMT_edit_file(char *file,int line,int col,int nl,int nc);
extern int MMT_show_file(char *file,int line,int col,int nl,int nc);
extern int PG_display_file(char *file);
extern int WscrInit(void);
extern int PG_display_field(PAGE *pg,int fld_nb);
extern int PG_display_flds_no_cmp(PAGE *pg);
extern int PG_display_flds(PAGE *pg);
extern int AP_run(APPL *ap);
extern int MN_edit(MENU *mn);
extern int SCR_comment(unsigned char *msg);
extern int PG_edit(PAGE *pg);
extern int WscrPG_edit(PAGE *pg,int req);
extern int PG_display_im(PAGE *pg);
extern int PG_undisplay(PAGE *pg);
extern int WscrMenuEnable(int objnb,BOOL stat);
extern APPL *WapFindCurrentAp(void);
extern int APP_key(int key);
extern int SCR_beep(void);

/* s_winit1.c */
extern int WscrStart1(int argc,unsigned char **argv);

/* s_wend.c */
extern int WscrEnd(int argc,unsigned char **argv);

/* s_wemf.c */
extern ATOM WscrEMFRegister(void);
extern HWND WscrEMFWMFCreate(int type,HWND hParent,char *filename,HANDLE hemf,int del);
extern HWND WscrEMFCreate(HWND hParent,char *filename,HANDLE hemf,int del);
extern HWND WscrWMFCreate(HWND hParent,char *filename,HANDLE hemf,int del);
extern LONG __stdcall WscrEMFProc(HWND hWnd,UINT wMsg,WPARAM wParam,LPARAM lParam);
extern int WscrEMFDestroy(HWND hWnd);
extern int WscrEMFWMFPaint(HWND hWnd);
extern int WscrEMFPaint(HWND hWnd);
extern int WscrWMFPaint(HWND hWnd);

/* s_wprts.c */
#if defined(SCRW32) || defined(DOSW32)
extern char **WscrListPrinters(void);
extern int WscrGetDefaultPrinter(char *PrinterName);
extern int WscrSetDefaultPrinter(char *PrinterName);
extern int WscrPrinterProperties(char *PrinterName);
extern int WscrSetPrinterProps(char *PrinterName,int prop,int val);
extern int WscrSetPrinterOrientation(char *PrinterName,int orient);
extern int WscrSetPrinterPaperSize(char *PrinterName,int psize);
extern int WscrSetPrinterScale(char *PrinterName,int scale);
extern int WscrSetPrinterCopies(char *PrinterName,int copies);
extern int WscrSetPrinterQuality(char *PrinterName,int quality);
extern int WscrSetPrinterColor(char *PrinterName,int color);
extern int WscrSetPrinterDuplex(char *PrinterName,int duplex);
extern int WscrSetPrinterCollate(char *PrinterName,int collate);
#endif

/* s_wap.c */
extern int WapTransOptText(unsigned char *ansi,unsigned char *ascii,int key, int prfx);
extern MENU *WapFindActMenu(int act_nu);
extern HMENU WapCreateMenu(MENU *mn,int owner);
extern HMENU WapCreateApplMenu(APPL *ap);
extern HWND WapEditAppl(APPL *ap);
extern int WscrTrackMenu(HWND hWnd,unsigned char **txt,int x,int y);

/* s_wscrol.c */
extern ATOM WscrScrollRegister(void);
extern HWND WscrScrollCreate(HWND hParent,int id,int x,int y,int w,int h);
extern int WscrScrollCreateHeader(HWND hWndParent);
extern int WscrScrollCreateEdit(HWND hWndParent);
extern LONG __stdcall WscrScrollProc(HWND hWnd,UINT wMsg,WPARAM wParam,LPARAM lParam);
extern int WscrScrollRMouse(HWND hWnd,WORD x,WORD y);
extern int WscrScrollSetEditText(HWND hWnd);
extern int WscrScrollKeyboard(HWND hWnd,WPARAM wParam);
extern int WscrScrollScroll(HWND hWnd,UINT wMsg,WPARAM wParam,LPARAM lParam);
extern int WscrlInvalidate(WSCROLL *wscrl);
extern int WscrScrollCalcPos(WSCROLL *wscrl);
extern int WscrScrollDisplayEdit(HWND hWnd);
extern int WscrScrollRedisplay(HWND hWnd,WSCROLL *wscrl);
extern int WscrScrollMouse(HWND hWnd,UINT wMsg,WPARAM wParam,LPARAM lParam);
extern int WscrScrollMouseToKeyb(HWND hWnd,WSCROLL *wscrl,int i,int j,int shift);
extern int WscrScrollPaintHeader(HWND hWnd);
extern int WscrScrollGetColWidth(WSCROLL *wscrl,int width);
extern int WscrScrollCalcColWidth(HWND hWnd,WSCROLL *wscrl);
extern int WscrScrollPaint(HWND hWnd);
extern int WscrScrollIsSelected(WSCROLL *wscrl,int i,int j);
extern int WscrScrollPaintCell(HWND hWnd,HDC hDC,WSCROLL *wscrl,int i,int j,int x,int y,int w,int h);
extern int WscrScrollCommand(HWND hWnd,WPARAM wParam);
extern int WscrScrollGetWidth(WSCROLL *wscrl,int j);
extern int WscrScrollGetHeight(WSCROLL *wscrl,int i);
extern int WscrScrollGetNl(WSCROLL *wscrl);
extern int WscrScrollGetNc(WSCROLL *wscrl);
extern int WscrScrollCCoord(WSCROLL *wscrl,int line,int col,int *x,int *y,int *w,int *h);
extern int WscrScrollInvalidateEdit(HWND hWnd);
extern int WscrScrollSaveEditText(HWND hWnd);
extern LONG __stdcall WscrScrollEditFn(HWND hWnd,UINT wMsg,WPARAM wParam,LPARAM lParam);
extern int W2scrlBegin(WSCROLL *wscrl,int *i,int *j);
extern int W2scrlEnd(WSCROLL *wscrl,int i,int j);
extern int W2scrlGetNl(WSCROLL *wscrl);
extern int W2scrlGetNc(WSCROLL *wscrl);
extern int W2scrlGetWidth(WSCROLL *wscrl,int col);
extern int W2scrlGetHeight(WSCROLL *wscrl,int line);
extern unsigned char *W2scrlGetText(WSCROLL *wscrl,int i,int j);
extern int W2scrlFnkey(WSCROLL *wscrl,int i,int j,int scrkey);
extern int W2scrlCommand(WSCROLL *wscrl,int i,int j,int wParam);
extern HMENU W2scrlCreateMenu(SCROLL *scrl);
extern WSCROLL *WscrSCROLLToWSCROLL(SCROLL *scrl);
extern int WscrScrollSetCurSel(HWND hWndScroll,int line,int col);
extern int WscrScrollCellChanged(HWND hWndScroll,int line,int col);
extern int WscrScrollSetSelect(HWND hWndScroll,int fl,int tl,int fc,int tc);
extern int WscrScrollEdit(HWND hWndParent,WSCROLL *wscrl);

/* s_wreg.c */
extern int WscrRegisterObj(char *ptr,PAGE *pg,int act,HMENU hMenu,int (*fn)(HWND __p1,PAGE *__p2,int __p3));
extern int WscrRegisterHwnd(char *ptr,HWND hWnd);
extern int WscrRegisterPgHwnd(PAGE *pg,HWND hWnd);
extern int WscrUnRegisterObjPg(char *ptr,PAGE *pg);
extern int WscrUnRegisterObj(char *ptr);
extern int WscrUnRegisterMenu(HMENU hMenu);
extern int WscrUnRegisterSubMn(HMENU hMenu);
extern int WscrFindObjNb(HWND hWnd);
extern int WscrFindPgNb(PAGE *pg);
extern int WscrFindPtrNb(char *ptr);
extern int WscrFindBcApplNb(BC *bc);
extern PAGE *WscrFindPage(HWND hWnd);
extern char *WscrFindObjPtr(HWND hWnd);
extern int WscrFindStatus(HWND hWnd);
extern int WscrFindApMn(HMENU hMenu,int act);
extern MENU *WscrFindMenu(HMENU hMenu);
extern APPL *WscrFindAppl(HMENU hMenu);
extern int WRegCleanup(void);

/* s_wmisc.c */
extern unsigned char *WscrOemVsAnsi(unsigned char *txt,int type);
extern unsigned char *WscrOemToAnsi(unsigned char *txt);
extern unsigned char *WscrAnsiToOem(unsigned char *txt);
extern int WscrSetWindowTextOem(HWND hWnd,unsigned char *txt);
extern unsigned char *WscrGetWindowTextOem(HWND hWnd);
extern int WscrTextOutOem(HDC hDC,int x,int y,unsigned char *txt);
extern int WscrIsShift(void);
extern int WscrIsCtrl(void);
extern int WscrIsAlt(void);
extern int WscrKeyWinToScr(int windef);
extern int WscrKeyScrToWin(int scrdef);
extern HMENU WscrCreateMenu(unsigned char *clst,unsigned char sep,int from);
extern int WscrSetSubMenu(HMENU hMenu,HMENU hSubMenu,int pos);
extern int WscrRecordKey(HWND hWnd,int key);
extern HANDLE WscrGetFont(int attr);
extern int WscrGetFldPos(PAGE *pg,int fld_nb,int *x,int *y,int *w,int *h);
extern int WscrPixelsToDlg(int *w,int *h);
extern int WscrPixelsToPgDlg(PAGE *pg,int *x,int *y,int *w,int *h);

/* s_wprt.c */
extern int WprPrinterInit(HWND hWndOwner,int dlg,char *docname);
extern int WprPrinterInitStd(HWND hWndOwner, int dlg, char *docname);
extern int WprAllInit(void);
extern int WprPrinterEnd(void);
extern int WprEndAll(void);
extern int WprPrinterEndFF(int ff);
extern int WprPrinterEndFFStd(int ff);
extern int WprRegisterFont(int family,int size,int bold,int italic,int underl,HANDLE hFont,int w,int h);
extern int WprSelectFont(int family,int size,int bold,int italic,int underl);
extern int WprSetCurFont(int i);
extern int WprDeleteFonts(void);
extern int WprSetFont(int family,int pty,int bold,int italic,int underl);
extern int WprFormFeed(void);
extern int WprFormFeedStd();
extern int WprPrintStringNoAlign(int x,int y,unsigned char *txt);
extern int WprPrintString(int x,int y,unsigned char *txt);
extern int WprHCenterLeft(void);
extern int WprHCenterRight(void);
extern int WprHCenterCenter(void);
extern int WprVCenterTop(void);
extern int WprVCenterBottom(void);
extern int WprVCenterBaseline(void);
extern int WprGetStringWidth(unsigned char *txt,long *w,long *h);
extern int WprRectangle(int x,int y,int w,int h);
extern int WprPolyline(long *pts,int npts);
extern int WprArc(int left,int top,int right,int bot,int x0,int y0,int x1,int y1);
extern int WprLine(int x0,int y0,int x1,int y1);
extern int WprSetColor(int i);
extern int WprSetBrush(int brush);
extern HBRUSH WprCreateBrush(WORD *brdef);
extern int WprCreateBrushes(void);
extern int WprDeleteBrushes(void);
extern int WprRegisterPen(int type,int width,int color,HPEN hPen);
extern int WprDeletePens(void);
extern int WprSetCurPen(int i);
extern int WprFindPen(int type,int width,int color);
extern int WprSetPen(int type,int width,int color);

/* s_wprt2.c */
extern int WprChInit(HWND hWndOwner,int dlg,char *docname);
extern int WprChEnd(void);
extern int WprChNewLine(int n);
extern int WprChFormFeed(void);
extern int WprChFlushBuf(void);
extern int WprChPrintString(unsigned char *txt);
extern int WprChPrintChar(int ch);
extern int WprChPrintGr(int ch);
extern int WprSetFontType(int attr);

/* s_wstat.c */
extern HWND WscrStatusCreate(HWND hWnd);
extern int WscrStatusSetText(HWND hWnd,unsigned char *text);

/* s_wcomdl.c */
extern COLORREF WscrChooseColor(HWND hOwner,COLORREF dftcol,COLORREF *custcols);
extern int WscrGetFilename(HWND hOwner,int opensave,unsigned char *title,unsigned char *infile,unsigned char *outfile,unsigned char *ftypes);
extern int WscrChooseFont(HWND hOwner,LOGFONT *logfont);

/* s_wddlg.c */
extern HANDLE WDDlgGetFont(int attr);
extern int WDDlgSetFont(WDDLG *wddlg,int fld_nb,int font);
extern int WDDlgSetFont1(WDDLG *wddlg,int fld_nb,int font);
extern int WDDlgSetIo(WDDLG *wddlg,int fld_nb,int io);
extern int WDDlgSetIo1(WDDLG *wddlg,int fld_nb,int io);
extern int WDDlgSetTabs(WDDLG *wddlg,int i,int ntabs,int *tabs);
extern WDDLG *WDDlgGetDlg(HWND hWnd);
extern int WDDlgDisplayField(WDDLG *wddlg,int fld_nb);
extern int WDDlgDisplayFlds(WDDLG *wddlg);
extern int WDDlgReadFlds(WDDLG *wddlg);
extern int __stdcall WDDlgStaticProc(HWND hWnd,UINT wMsg,WPARAM wParam,LPARAM lParam);
extern int WDDlgInit(HWND hWnd,WDDLG *wddlg);
extern int WDDlgCommand(HWND hWnd,UINT iId,HWND hCtl,UINT nNotif);
extern BOOL __stdcall WDDlgProc(HWND hWnd,UINT wMsg,WPARAM wParam,LPARAM lParam);
extern WDDLG *WDDlgCreate(int x,int y,int cx,int cy,unsigned char *title);
extern int WDDlgAddChild(WDDLG *wddlg,WORD type,DWORD lStyle,int x,int y,int cx,int cy,char *ptr,char *ext,int (*fn)(WDDLG *__p1,int __p2,char *__p3,int __p4),int attr);
extern int WDDlgAddButton(WDDLG *wddlg,int x,int y,int cx,int cy,unsigned char *text,int (*fn)(WDDLG *__p1,int __p2,char *__p3,int __p4),int attr);
extern int WDDlgAddListbox(WDDLG *wddlg,int x,int y,int cx,int cy,unsigned char **opts,char *sel,int (*fn)(WDDLG *__p1,int __p2,char *__p3,int __p4),int attr);
extern int WDDlgAddStatic(WDDLG *wddlg,int x,int y,int cx,int cy,unsigned char *text);
extern int WDDlgAddIcon(WDDLG *wddlg,int x,int y,int res,int (*fn)(WDDLG *__p1,int __p2,char *__p3,int __p4));
extern int WDDlgAddBmp(WDDLG *wddlg,int x,int y,int res,int (*fn)(WDDLG *__p1,int __p2,char *__p3,int __p4));
extern int WDDlgAddGroupbox(WDDLG *wddlg,int x,int y,int cx,int cy,unsigned char *title);
extern int WDDlgAddEdit(WDDLG *wddlg,int x,int y,int cx,int cy,unsigned char *text,int maxlen,int (*fn)(WDDLG *__p1,int __p2,char *__p3,int __p4),int attr);
extern int WDDlgAddHSlider(WDDLG *wddlg,int x,int y,int cx,int cy,int *pos,int nMin,int nMax);
extern int WDDlgAddProgress(WDDLG *wddlg,int x,int y,int cx,int cy,int *pos,int nMin,int nMax);
extern int WDDlgAddCombobox(WDDLG *wddlg,int x,int y,int cx,int cy,unsigned char **opts,unsigned char *sel,int (*fn)(WDDLG *__p1,int __p2,char *__p3,int __p4),int attr);
extern int WDDlgEdit(WDDLG *wddlg);
extern int WDDlgFree(WDDLG *wddlg);
extern int WDDlgDblclkFn(WDDLG *wddlg,int fld_nb,char *txt,int nNotif);
extern int WDDlgOkFn(WDDLG *wddlg,int i,char *a,int j);
extern int WDDlgEscFn(WDDLG *wddlg,int i,char *a,int j);
extern int WDDlgSelectAllFn(WDDLG *wddlg,int i,char *a,int k);
extern int WDDlgSelect(unsigned char *title,unsigned char **opts,char *sels,int x,int y,int cx,int cy,int ntabs,int *tabs);
extern int WDDlgLVSelect(unsigned char *title,unsigned char **opts,char *sels,int x,int y,int cx,int cy);
extern int WDDlgAddListview(WDDLG *wddlg,int x,int y,int cx,int cy,unsigned char **opts,char *sel,int (*fn)(WDDLG *__p1,int __p2,char *__p3,int __p4),int attr);
extern int WDDlgInitListView(WDDLG *wddlg,int fld_nb);
extern int WDDlgLVCalcNc(WDDLG *wddlg,int fld_nb);
extern int WDDlgSetLVTabs(WDDLG *wddlg,int fld_nb,int ntabs,int *tabs);
extern int WDDlgDisplayLVTabs(WDDLG *wddlg,int fld_nb);
extern int WDDlgNotifyHandler(HWND hWnd,WPARAM wParam,LPARAM lParam);
extern int WDDlgHideField(WDDLG *wddlg,int fld_nb);
extern int WDDlgShowField(WDDLG *wddlg,int fld_nb);
extern int WDDlgSetHwndVisible(WDDLG *wddlg,int fld_nb,int show);
extern int WDDlgResize(WDDLG *wddlg,int x,int y,int cx,int cy);
extern int WDDlgAsk(unsigned char *title,unsigned char *qu,unsigned char *rep,int lg);
extern int WDDlgSetEscFn(WDDLG *wddlg,int (*fn)(WDDLG *__p1,int __p2,char *__p3,int __p4));
extern int WDDlgSetOkFn(WDDLG *wddlg,int (*fn)(WDDLG *__p1,int __p2,char *__p3,int __p4));
extern int WDDlgSetBeginFn(WDDLG *wddlg,int (*fn)(WDDLG *__p1,int __p2,char *__p3,int __p4));
extern int WDDlgSetEndFn(WDDLG *wddlg,int (*fn)(WDDLG *__p1,int __p2,char *__p3,int __p4));
extern int WDDlgGetCurSel(WDDLG *wddlg,int fld_nb);
extern int WDDlgSetCurSel(WDDLG *wddlg,int fld_nb,int opt_nb);
extern int WDDlgLBChangeText(WDDLG *wddlg,int fld_nb,int opt_nb,unsigned char *txt);
extern int WDDlgSetFocus(WDDLG *wddlg,int fld_nb);
extern WDDLG *WDDlgCreateInfoBox(unsigned char *title, unsigned char *msg);

/* s_wddecl.c */
extern int WscrDdeSetTimeOut(int msecs, int nbtries);
extern int WscrDdeExecute(char *Service,char *Topic,unsigned char *Cmd);
extern int WscrDdeExecuteCmd(HCONV hConv,unsigned char *cmd);
extern int WscrDdeEnd(HCONV hConv);
extern unsigned char *WscrDdeGetItem(HCONV hConv,char *szItem);
extern int WscrDdeSetItem(HCONV hConv,char *szItem,unsigned char *ptr);
extern unsigned char *WscrDdeGet(char *szService,char *szTopic,char *szItem);
extern int WscrDdeSet(char *szService,char *szTopic,char *szItem,unsigned char *ptr);
extern HCONV WscrDdeConnect(char *szService,char *szTopic);
extern HDDEDATA __stdcall DdeCallback(UINT iType,UINT iFmt,HCONV hConv,HSZ hsz1,HSZ hsz2,HDDEDATA hData,DWORD dwData1,DWORD dwData2);

/* s_wpgdlg.c */
extern unsigned char *WpgAddAlt(unsigned char *buf,int qkey);
extern int WpgFld2Dlg(HWND hWnd,PAGE *pg,int fld_nb);
extern int WpgDlg2Fld(HWND hWnd,PAGE *pg,int fld_nb);
extern int Wpg2Dlg(HWND hWnd,PAGE *pg);
extern int WpgDlg2Pg(HWND hWnd,PAGE *pg);
extern int WscrCalcRadioNb(PAGE *pg,int fld_nb);
extern int WscrIsFldRadioChecked(PAGE *pg,int nfld);
extern int WscrDlg2FldRadio(PAGE *pg,int fld_nb,int v);

/* s_whead.c */
extern HWND WscrCreateHeader(HWND hWndParent,int x,int y,int w,int h);
extern int WscrHeaderSetPos(HWND hWnd,int x,int y,int w,int h);
extern int WscrInsertHeaderItem(HWND hWndHeader,int iInsertAfter,int nWidth,LPSTR lpsz);
extern int WscrDeleteHeaderItem(HWND hWndHeader,int iItem);
extern int WscrSetHeaderText(HWND hWndHeader,int iItem,unsigned char *txt,int just);
extern int WscrSetHeaderWH(HWND hWndHeader,int iItem,int wh,int type);
extern int WscrSetHeaderHeight(HWND hWndHeader,int iItem,int height);
extern int WscrSetHeaderWidth(HWND hWndHeader,int iItem,int width);
extern int WscrGetHeaderWH(HWND hWndHeader,int iItem,int type);
extern int WscrGetHeaderHeight(HWND hWndHeader,int iItem);
extern int WscrGetHeaderWidth(HWND hWndHeader,int iItem);
extern int WscrGetHeaderNc(HWND hWndHeader);
extern int WscrHeaderNotify(NMHDR *nmh,int *colnb,int *size);

/* s_wprops.c */
extern BOOL __stdcall WscrPropsDlgProc(HWND hWnd,UINT wMsg,WPARAM wParam,LPARAM lParam);
extern int WscrEditTabbedPages(HWND hwndOwner,TPAGES *tpgs);

/* s_wcmt.c */
extern int WscrDisplayCmt(HWND hWnd);

/* s_wtoolb.c */
extern int WscrInitToolbar(WTOOLBAR *wtb);
extern HWND WscrCreateToolbar(HWND hWndParent,WTOOLBAR *wtb);
extern unsigned char *WscrToolbarFindHelp(WTOOLBAR *wt,int act_nu);
extern int WscrToolbarNotify(HWND hWnd,WPARAM wParam,LPARAM lParam);

/* s_wsup.c */
extern int AP_undisplay(APPL *ap);
extern int AP_edit(APPL *ap);
extern int AP_display(APPL *ap);
extern int PG_display_slide(PAGE *pg);
extern int PG_display(PAGE *pg);
extern IMAGE *PG_save(PAGE *pg);
extern int PG_edit_field(PAGE *pg,int fld_nb);
extern int SCR_term_vkey(int val);
extern int SCR_check_key(void);
extern int SCR_app_key(void);
// extern int STD_doscpout(char *from,char *to,int unit,int type);  // JMP 6/5/2021 pour cpp double def
//extern int STD_subshell(void);									// JMP 6/5/2021 pour cpp double def
//extern int STD_dosformat(int unit,int type);						// JMP 6/5/2021 pour cpp double def
//extern int STD_doscpin(char *from,char *to,int unit,int type);	// JMP 6/5/2021 pour cpp double def
extern int SCR_config_screen(void);
extern int SCR_cursor_off(void);
extern int SCR_cursor_on(void);
extern int DT_calendar(long date,int bline,int bcol);
extern int SCR_print_screen(void);
extern int SCR_stop_alarm(void);
extern int SCR_mini_init(void);
extern int SCR_end(void);
extern int ACT_exec_system(char *cmd);
extern int SCR_clear_key(void);
extern int SCR_get_key(void);
extern int SCR_hit_key(void);

/* s_wemfcr.c */
extern HWND WprGethWnd(void);
extern HDC WprEMFCreatehDC(unsigned char *filename,int w,int h,unsigned char *pictname);
extern int WprEMFInit(char *filename,int w,int h,unsigned char *pictname);
extern int WprWMFInit(char *filename,int w,int h);
extern HENHMETAFILE WprEMFEnd(void);
extern HMETAFILE WprWMFEnd(void);

/* s_wtoolt.c */
extern BOOL WDDlgSet1Tooltip(HWND hwndCtrl,LPARAM lParam);
extern LRESULT __stdcall WDDlgGetMsgProc(int nCode,WPARAM wParam,LPARAM lParam);
extern int WDDlgClearHook(void);
extern int WDDlgCreateTooltip(WDDLG *wddlg);

/* s_wsys.c */
extern int WscrAddGroupItem(unsigned char *groupname,unsigned char *cmd,unsigned char *title,unsigned char *startdir,int show);
extern int WscrDeleteGroup(unsigned char *groupname);
extern int WscrGetOSVersion(char *version);
extern int WscrRegisterFileType(char *ext,char *code,char *descr,char *icon,char *opencmd,char *newcmd,char *printcmd);
extern int WscrRegisterDeleteFileType(char *ext,char *code);
extern int WscrRegisterUninstall(char *code,char *title,char *exe);
extern int WscrRegisterDeleteUninstall(char *code);
extern int WscrRegisterSetApplData(char *ApplCode,char *ApplVar,char *VarValue);
extern int WscrRegisterGetApplData(char *ApplCode,char *ApplVar,char *VarValue);
extern int WscrRegisterDeleteApplData(char *ApplCode);
extern HWND WscrGetMainWnd(void);

/* s_wsys2.c */
extern int WscrGetPid();
extern int WscrTempFilenameInDir(char *dirname, char *filename);
extern int WscrTempFilename(char *filename);
extern int WscrTempPath(char *dirbuf);
extern int WscrSetFileTime(char *filename, long actime, long modtime);
extern int WscrSetFileTimeWin(char *filename, long cretime, long acttime, long modtime);

/* s_whook.c */
extern LRESULT __stdcall WscrKeyboardHookProc(int nCode,WPARAM wParam,LPARAM lParam);
extern int WscrSetKeyboardHook(void);
extern int WscrClearKeyboardHook(void);

/* s_wvars.c */

/* s_wobjs.c */
extern int WscrCreateFonts(void);
extern int WscrCreateBrushes(void);
extern int WscrCreatePens(void);
extern int WscrCreateGDIObjects(void);
extern int WscrDeleteGDIObjects(void);

/* s_whwnd.c */
extern int WscrCenterWindow(HWND hWnd,int w_x,int w_y);
extern int WscrCalcDlgRect(HWND hWnd,RECT *rect);
extern int WscrMoveDlgChildren(HWND hWnd,int x,int y);
extern int WscrResizeDlgChildren(HWND hWnd);

/* s_wprraw.c */
extern int WscrPrintRawInit(char *PrinterName,char *JobName);
extern int WscrPrintRawData(unsigned char *data,long count);
extern int WscrPrintRawString(unsigned char *txt);
extern int WscrPrintRawEnd(void);
extern int WscrPrintRawFile(char *filename);

/* s_whlp.c */
extern int HLP_edit(char *topic);
extern int HLP_stack_edit(void);

/* s_wddesr.c */
extern int WscrDdeServer(char *service,char **topic,char *(*Request)(char *__p1,char *__p2),int (*Poke)(char *__p1,char *__p2,char *__p3));
extern int WscrDdeServerEnd(void);
extern HDDEDATA __stdcall DdeSrvCallback(UINT iType,UINT iFmt,HCONV hConv,HSZ hsz1,HSZ hsz2,HDDEDATA hData,DWORD dwData1,DWORD dwData2);

/* s_wclipb.c */
extern int WscrClipSet(unsigned char *txt);
extern int WscrClipIsData(void);
extern unsigned char *WscrClipGet(void);

#ifdef __cplusplus
}
#endif
