//---------------------------------------------------------------------------
#ifndef debugH
#define debugH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Menus.hpp>
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
class TfrmDebug : public TForm
{
__published:	// IDE-managed Components
    TMemo *mmDebug;
    TMainMenu *mnDebug;
    TMenuItem *File1;
    TMenuItem *Edit1;
    TMenuItem *Exit1;
    TMenuItem *Clear1;
    TMenuItem *SaveAs1;
    TSaveDialog *SaveDialog;
    void __fastcall Clear1Click(TObject *Sender);
    void __fastcall Exit1Click(TObject *Sender);
    void __fastcall OnClose(TObject *Sender, TCloseAction &Action);
    void __fastcall SaveAs1Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall void Add(char *str);
    __fastcall TfrmDebug(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmDebug *frmDebug;
//---------------------------------------------------------------------------
#endif
