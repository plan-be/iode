#ifdef __cplusplus
extern "C" {
#endif


/* s_wmp2.c */
extern int WscrGetLpgFld(PAGE *lpg,int col);
extern int WscrGetNc(PAGE *lpg);
extern int WmpGetNc(WSCROLL *wscrl);
extern U_ch *Wmptxt(WSCROLL *wscrl,int i,int j);
extern int Wmpcolor(WSCROLL *wscrl,int i,int j);
extern int Wmpfont(WSCROLL *wscrl,int i,int j);
extern int Wmpwidth(WSCROLL *wscrl,int i);
extern int Wmpgetnl(WSCROLL *wscrl);
extern int WmpSettext(WSCROLL *wscrl,int line,int col,unsigned char *txt);
extern int Wmpheight(WSCROLL *wscrl,int i);
extern int WmpFnkey(WSCROLL *wscrl,int cline,int ccol,int key);
extern int WmpRightMouse(WSCROLL *wscrl,int x,int y);
extern int WmpSetScroll(HWND hWnd,MPAGE *mp);

/* s_wbc.c */
extern HMENU WbcCreateMenuIndex(BC *bc);
extern HMENU WbcCreateMenu(BC *bc);
extern int WbcEnableMenu(int type);
extern int WbcMenuCommand(HWND hWnd,PAGE *pg,int iId);
extern int WscrBcNbFlds(HWND hWnd,BC *bc);
extern int WscrBcEditIsam(BC *bc);
extern int WbcEnableAll(HWND hWnd,BC *bc);
extern int WbcDisableAll(HWND hWnd,BC *bc);
extern int WbcCreate(HWND hWnd,BC *bc);
extern int WbcEdit(HWND hWnd,BC *bc);
extern int WbcSearch(HWND hWnd,BC *bc);
extern int WbcNext(HWND hWnd,BC *bc);
extern int WbcPrev(HWND hWnd,BC *bc);
extern int WbcDelete(HWND hWnd,BC *bc);
extern int WbcSave(HWND hWnd,BC *bc);

/* s_wbc2.c */
extern int WbcGetNc(WSCROLL *wscrl);
extern int WbcGetNl(WSCROLL *wscrl);
extern U_ch *WbcTxt(WSCROLL *wscrl,int i,int j);
extern int WbcColor(WSCROLL *wscrl,int i,int j);
extern int WbcFont(WSCROLL *wscrl,int i,int j);
extern int WbcWidth(WSCROLL *wscrl,int i);
extern int WbcSettext(WSCROLL *wscrl,int line,int col,unsigned char *txt);
extern int WbcHeight(WSCROLL *wscrl,int i);
extern int WbcFnkey(WSCROLL *wscrl,int cline,int ccol,int key);
extern int WbcSetScroll(HWND hWnd,BC *bc);

/* s_wisam.c */
extern int WisMenuCommand(HWND hWnd,PAGE *pg,int iId);
extern HMENU WisCreateMenu(PAGE *pg,ISAM *is);
extern int PG_edit_isam(PAGE *pg);
extern int IS_trans_write(ISAM *is);
extern int IS_trans_rewrite(ISAM *is);
extern int IS_trans_delete(ISAM *is);

/* s_wmp.c */
extern HMENU WmpCreateMenu1(MPAGE *mp);
extern HMENU WmpCreateMenu2(MPAGE *mp);
extern int WmpMenuCommand1(HWND hWnd,PAGE *pg,int iId);
extern int WmpMenuCommand2(HWND hWnd,MPAGE *mp,int iId);
extern int WmpEdit(HWND hWnd,MPAGE *mp);
extern int WmpEndEdit(HWND hWnd,MPAGE *mp);
extern int WscrMpNbFlds(HWND hWnd,MPAGE *mp);
extern int WscrMpEditIsam(MPAGE *mp);

#ifdef __cplusplus
}
#endif
