//---------------------------------------------------------------------------

#ifndef prefH
#define prefH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>
#include "chart.h"
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmPreferences : public TForm
{
__published:	// IDE-managed Components
    TStringGrid *sgPreferences;
    TComboBox *cbColor;
    TComboBox *cbStyle;
    TLabel *Color;
    TLabel *Style;
    TComboBox *cbWidth;
    TLabel *Label1;
    TButton *btnApply;
    TComboBox *cbArange;
    TLabel *Arange;
    TImage *imPreview;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall cbColorChange(TObject *Sender);
    void __fastcall sgPreferencesSelectCell(TObject *Sender, int ACol,
          int ARow, bool &CanSelect);
    void __fastcall cbWidthChange(TObject *Sender);
    void __fastcall cbStyleChange(TObject *Sender);
    void __fastcall btnApplyClick(TObject *Sender);
//    void __fastcall dgPreferencesDrawCell(TObject *Sender, int ACol,
//          int ARow, TRect &Rect, TGridDrawState State);
private:	// User declarations
    int Row, Col;
public:		// User declarations
    __fastcall TfrmPreferences(TComponent* Owner);
    RefreshImage(int Row);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmPreferences *frmPreferences;
//---------------------------------------------------------------------------
#endif
