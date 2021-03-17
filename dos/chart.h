//----------------------------------------------------------------------------
#ifndef chartH
#define chartH
//----------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//#include <TeeEngine.hpp>
#include "TeeProcs.hpp"
#include <Chart.hpp>
#include <ExtCtrls.hpp>
#include <Series.hpp>
#include <Dialogs.hpp>
#include <Graphics.hpp>
#include "TeeComma.hpp"
#include "TeeEdit.hpp"
#include "TeeJPEG.hpp"
#include "TeeGIF.hpp"
#include "TeExport.hpp"
#include <Menus.hpp>
#include <TeEngine.hpp>
#include <TeeProcs.hpp>
#include <TeeThemes.hpp>
#include <TeeThemeEditor.hpp>
#include <TeeSelectorTool.hpp>
#include <EditChar.hpp>
#include <Grids.hpp>
#include <TeeProcs.hpp>
#include <TeeStore.hpp>

extern  void TAddDebug(char *str);

//----------------------------------------------------------------------------
class TfrmChart : public TForm
{
__published:
    TChart  *Chart;
    TBarSeries *Series1;
    TChartEditor *edChart;
    TMainMenu *mnMain;
    TMenuItem *optFile;
    TMenuItem *optView;
    TMenuItem *optPrint;
    TMenuItem *optClose;
    TMenuItem *optSaveAs;
    TMenuItem *opt3DView;
    TMenuItem *optLabel;
    TMenuItem *optEdit;
    TMenuItem *mnoEdit;
    TMenuItem *optCopy;
    TMenuItem *optThemes;
    TMenuItem *optExport;
    TSaveDialog *dlgSaveDialog;
    TMenuItem *LoadTheme;
    TOpenDialog *dlgOpenDialog;

    void __fastcall SaveAs(TObject *Sender);
    void __fastcall Print(TObject *Sender);
    void __fastcall OnClose(TObject *Sender, TCloseAction &Action);
    void __fastcall ClickSeries(TCustomChart *Sender, TChartSeries *Series,
	  int ValueIndex, TMouseButton Button, TShiftState Shift, int X,
	  int Y);
    void __fastcall View3D(TObject *Sender);
    void __fastcall Edit(TObject *Sender);
    void __fastcall frmClose(TObject *Sender);
    void __fastcall optLabelClick(TObject *Sender);
    void __fastcall FormKeyPress(TObject *Sender, char &Key);
//    void __fastcall optEditClick(TObject *Sender);
//    void __fastcall mnoPreferencesClick(TObject *Sender);
    void __fastcall optCopyClick(TObject *Sender);
//    void __fastcall optRefreshClick(TObject *Sender);
    void __fastcall ChartDblClick(TObject *Sender);
    void __fastcall optExportClick(TObject *Sender);
    void __fastcall LoadThemeClick(TObject *Sender);
    void __fastcall DefaultThemeClick(TObject *Sender);
private:
    double          ChrtYMin, ChrtYMax, ChrtZMin, ChrtZMax;

public:
  __fastcall TfrmChart::TfrmChart(TComponent* Owner);
  __fastcall TfrmChart::ChrtAdd(char *title, int axis, int type, int nobs, double *xy_vals);
  __fastcall TfrmChart::ChrtAddTitle(char *title);
  __fastcall TfrmChart::ChrtAddColorBand(double startvalue, double endvalue, TColor color);
  __fastcall TfrmChart::ChrtParams(char *title, double ymin, double ymax, double zmin, double zmax);
  __fastcall TfrmChart::ChrtSeriesMarks();
    int     TfrmChart::ChrtScale();

    TSelectorTool    *Selector;
    TColorBandTool   *ColorBand;
//    TColorBandTool    *ColorBand;
    int             Chrthdl, nb, nl;
    int             nbLegend;
    bool            emf;
    AnsiString      Theme;
    AnsiString      TypeSeries;
//    AnsiString  ChrtArange;
//    TPen *GetPen(int i );
//    TPen    SeriesPen[10];
};

extern AnsiString  WriteColor(TColor Color);
extern TColor  ReadColor(AnsiString Str);
extern AnsiString WriteStyle(TPenStyle Style);
extern TPenStyle ReadStyle(AnsiString Str);
extern TPen *GetPen(int i);
extern void SetPen(int i, TPen *Pen);
extern void WritePen(int i, AnsiString Color, AnsiString Width, AnsiString Style);
extern AnsiString GetArange();
extern void SetArange(AnsiString Str);

//extern TPen    SeriesPen[10];

class IodeLine {
public:
    int             Width;
    TColor          Color;
    TPenStyle       PenStyle;
    TSeriesPointerStyle  PointerStyle;

    IodeLine():Color(clBlack), Width(2), PenStyle(psSolid), PointerStyle(psNothing) {};
    IodeLine(TColor iColor, int iWidth, TPenStyle iPenStyle,TSeriesPointerStyle iPointerStyle) {
        Color = iColor;
        Width = iWidth;
        PenStyle = iPenStyle;
        PointerStyle = iPointerStyle;
    };
    IodeLine(AnsiString Description);

    AnsiString Describe();
};
class IodeBar {
public:
	TColor          Color;
	TBrushStyle 	BrushStyle;
	int             Border;

	IodeBar(): Color(clBlue), BrushStyle(bsSolid), Border(1){};
	IodeBar(TColor iColor, TBrushStyle iBrushStyle, int iBorder) {
		Color = iColor;
		BrushStyle = iBrushStyle;
		Border = iBorder;
	};
	IodeBar(AnsiString Description);

	AnsiString Describe();
};

class IodePanel {
public:
	TColor          Color;
	int             Border;
	int				Rounded;

	IodePanel(): Color(clWhite), Border(1), Rounded(0){};
	IodePanel(TColor iColor, int iBorder, int iRounded) {
		Color = iColor;
		Border = iBorder;
		Rounded = iRounded;
	};
	IodePanel(AnsiString Description);

	AnsiString Describe();
};

#endif



