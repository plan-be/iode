//---------------------------------------------------------------------------

#ifndef frmMainH
#define frmMainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Menus.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>

#define MYWM_NOTIFY     (WM_APP+100)
#define IDC_MYICON      1006
extern HINSTANCE g_hinst;
LRESULT IconDrawItem(LPDRAWITEMSTRUCT lpdi);

//---------------------------------------------------------------------------
class TfrmM : public TForm
{
__published:	// IDE-managed Components
    TMemo *mLog;
    TPopupMenu *PopupMenu;
    TMenuItem *mnoShutdown;
    TImage *imgIcon_1;
    TMenuItem *mnoHide;
    TImage *imgIcon_2;
    TMenuItem *mnoProperties;
    TMemo *mError;
    TMenuItem *ResetFields1;
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall mnoShutdownClick(TObject *Sender);
    void __fastcall FormDestroy(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall mnoPropertiesClick(TObject *Sender);
    void __fastcall mnoHideClick(TObject *Sender);
    void __fastcall ToggleState(void);
    void __fastcall ResetFields1Click(TObject *Sender);
private:	// User declarations
    int    State;
    void __fastcall MyNotify(TMessage& Msg);
    void __fastcall DrawItem(TMessage& Msg);
    bool __fastcall TrayMessage(DWORD dwMessage);
    HICON __fastcall IconHandle(void);
public:
    __fastcall TfrmM(TComponent* Owner);
BEGIN_MESSAGE_MAP
VCL_MESSAGE_HANDLER(WM_DRAWITEM,TMessage,DrawItem)
VCL_MESSAGE_HANDLER(MYWM_NOTIFY,TMessage,MyNotify)
END_MESSAGE_MAP(TForm)
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmM *frmM;
//---------------------------------------------------------------------------
#endif
