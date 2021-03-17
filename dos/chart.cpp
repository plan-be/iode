//----------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "chart.h"
//#include "pref.h"

#include <math.h>
#include <inifiles.hpp>
//#include <pendlg.hpp>
//----------------------------------------------------------------------------
//#pragma link "TeeComma"
//#pragma link "TeeEdit"
//#pragma link "TeeJPEG"
//#pragma link "TeeGif"
//#pragma link "TeePNG"
//#pragma link "TeeSVGCanvas"
// #pragma link "TeeMIFCanvas"
//#pragma link "TeePDFCanvas"
//#pragma link "TeeInspector"
#pragma link "VCLTee.TeeEdit"
#pragma link "VCLTee.TeeEdit"
#pragma resource "*.dfm"

#define SCR_ISAN(x)   ((x) >= (double)(-1.0e37))
#define L_NAN       (double)(-2.0e37)


extern  int B_MULTIBAR;
#define CHRT_MAXHDLS    128
//------------------------
static const TColor clLightOrange = 0x0099ff;
static const TColor clSeaGreen = 0x669933;
static const TColor clPaleBlue = 0xffcc99;
static const TColor clViolet = 0x800800;

/* template colors
static const TColor clc1 = 0x2d687e;
static const TColor clc2 = 0x292929;
static const TColor clc3 = clWhite;
static const TColor clc4 = 0x6d6d6d;
static const TColor clc5 = 0xf58220;
static const TColor clc6 = 0xa5b1be;
static const TColor clc7 = 0x6dc3d2;
static const TColor clc8 = 0xffc73b;
static const TColor clc9 = 0x1b3b5a;
*/
static const TColor clc1 = 0x005a3b1b;
static const TColor clc2 = 0x00292929;
static const TColor clc3 = 0x00ffffff;
static const TColor clc4 = 0x006d6d6d;
static const TColor clc5 = 0x002082f5;
static const TColor clc6 = 0x00beb1a5;
static const TColor clc7 = 0x00d2c36d;
static const TColor clc8 = 0x003bc7ff;
static const TColor clc9 = 0x007e682d;


//psRectangle, psCircle, psTriangle, psDownTriangle, psCross, psDiagCross, psStar, psDiamond, psSmallDot, psNothing, psLeftTriangle, psRightTriangle
//-------------------------
int CHRT_hdls[CHRT_MAXHDLS];

TList   *LIodeColors = NULL;
TList   *LIodeLines = NULL;
TList   *LIodeBars = NULL;

IodePanel	*IodeChartPanel;
IodeLine	*IodeGridLine;

TFont   *TitleFont = NULL, *LegendFont = NULL, *AxisFont = NULL;
double  ColorBandStartValue = 0.0, ColorBandEndValue = 0.0;

extern AnsiString GetArgument(AnsiString Description, AnsiString Key);

TFont   *IodeFont(AnsiString lDescription)
{
	AnsiString  Arg;
	TFont       *rFont = new TFont();
	rFont->Name = "Arial";
//	rFont->Name = "Trebuchet MS";
	rFont->Size = 8;
	rFont->Style = TFontStyles(); // TFontStyles()<< fsBold << fsUnderline;
	Arg = GetArgument(lDescription, "name");
	if(!Arg.IsEmpty()) rFont->Name = Arg;
	Arg = GetArgument(lDescription, "size");
	if(!Arg.IsEmpty()) rFont->Size = Arg.ToInt();
	Arg = GetArgument(lDescription, "style");
	if(!Arg.IsEmpty()) {
		if(Arg.Pos("bold")) rFont->Style = rFont->Style << fsBold;
		if(Arg.Pos("italic")) rFont->Style = rFont->Style << fsItalic;
	}
	Arg = GetArgument(lDescription, "color");
	if(!Arg.IsEmpty()) rFont->Color = ReadColor(Arg);
	return(rFont);
}

bool equals(double x, double y)
{
	return(fabs(x-y) <=.0000001);
}

int GetNbdec(double incr)
{
    double  x;
    int     nbdec;

    /* get nbr of significant decimals */
    x = incr - floor(incr);
    if (x == 0.0) x = incr;
	nbdec = 0;
    while(x < 1 && x != 0) {
        (nbdec)++;
        x *= 10;
        x =  x - floor(x);
    }
    return(nbdec);
}

AnsiString    GetValuesFormat(double incr)
{
	int         i;
	double      nbdec;
    AnsiString  VFmt;

	VFmt = "###0";
	nbdec = GetNbdec(incr);
    if(nbdec > 0) {
		VFmt = VFmt + ".";
	    for(i = 0; i < nbdec; i++) VFmt = VFmt + "0";
    }
    return(VFmt);
}


int ChrtScaleAxis(TChart *Chart, TChartAxis  *Axis, double ChrtYMin, double ChrtYMax, bool Y = true)
{
    double      d, ld, incr, maxi, mini, left, right;
    int         div;
	AnsiString  VFmt;

    if(Y) {
        maxi = Chart->MaxYValue(Axis);
        if(SCR_ISAN(ChrtYMax) && maxi < ChrtYMax) maxi = ChrtYMax;

		mini = Chart->MinYValue(Axis);
        if(SCR_ISAN(ChrtYMin) && mini < ChrtYMin) mini = ChrtYMin;
    }
    else {
		maxi = Chart->MaxXValue(Axis);
        if(SCR_ISAN(ChrtYMax) && maxi < ChrtYMax) maxi = ChrtYMax;

		mini = Chart->MinXValue(Axis);
        if(SCR_ISAN(ChrtYMin) && mini < ChrtYMin) mini = ChrtYMin;
    }

    d = (double) maxi - mini;
    if(d > 0) {
        div = 0;
		ld =  pow(10.0, floor(log10(d)));
	    while(d/ld  <= 3) {
			ld /= 2;
            div ++;
        }
	    incr = ld;
	    Axis->Increment = incr;

		if(equals(mini, incr * ceil(mini/incr)))
			left = mini;
		else
		    left = (ceil(mini/incr) - 1) * incr;

//          if(maxi == incr * ceil(maxi/incr))
		if(equals(maxi,incr * ceil(maxi/incr)))
		    right = maxi ;
	    else
		    right = (floor(maxi/incr) + 1) * incr;

	    if(SCR_ISAN(ChrtYMax)) right = ChrtYMax;
	    if(SCR_ISAN(ChrtYMin)) left = ChrtYMin;
	    Axis->Automatic = false;
	    Axis->SetMinMax(left, right);

        Axis->AxisValuesFormat = GetValuesFormat(Axis->Increment);
        if((right - left)/incr < 5) Axis->MinorTickCount = 4;
        else        Axis->MinorTickCount = 1;
    }
}

AnsiString GetArgument(AnsiString Description, AnsiString Key)
{
    AnsiString Argument;
	int bpos, epos;

	bpos = Description.Pos(Key+"=");
    if(bpos != 0) {
	    Argument = Description;
		Argument.Delete(1, bpos + Key.Length());
	    epos = Argument.Pos(",");
	    if(epos != 0) Argument.Delete(epos, 1 + Argument.Length() - epos);
	}

	return(Argument);
}

AnsiString  WriteColor(TColor Color)
{
	AnsiString  Description;

	switch(Color){
		case clPaleBlue : Description = "paleblue"; break;
		case clBlue: Description = "blue"; break;
		case clRed: Description = "red"; break;
		case clLtGray: Description = "gray"; break;
		case clSeaGreen:  Description = "green"; break;
		case clLightOrange: Description = "orange"; break;
		case clViolet: Description = "violet"; break;
		case clc1: Description = "c1"; break;
		case clc2: Description = "c2"; break;
		case clc3: Description = "c3"; break;
		case clc4: Description = "c4"; break;
		case clc5: Description = "c5"; break;
		case clc6: Description = "c6"; break;
		case clc7: Description = "c7"; break;
		case clc8: Description = "c8"; break;
		case clc9: Description = "c9"; break;
		default: Description = "black";
	}
	return(Description);
}

AnsiString IodePanel::Describe() {
	AnsiString  Description;

	Description = "color="+ WriteColor(Color) + ", border=" + Border + ", rounded=" + Rounded;
	return(Description);
}

AnsiString IodeBar::Describe() {
    AnsiString  Description;

    Description = "color="+ WriteColor(Color);

	Description += ", style=";
	switch(BrushStyle) {
		case bsCross: Description += "cross"; break;
		case bsDiagCross: Description += "diagcross"; break;
		case bsBDiagonal: Description += "bdiagonal"; break;
		case bsFDiagonal: Description += "fdiagonal"; break;
		case bsHorizontal: Description += "horizontal"; break;
		case bsVertical: Description += "vertical"; break;
		default: Description += "solid";
	}
	Description += ", border=" + Border;

	return(Description);
}


AnsiString IodeLine::Describe() {
	AnsiString  Description, aWidth;

    Description = "color=" + WriteColor(Color);

    aWidth = Width;
    Description += ", width=" + aWidth;

	Description += ", style=";
	switch(PenStyle) {
	    case psDot: Description += "dot"; break;
	    case psDash : Description += "dash"; break;
	    case psDashDot : Description += "dashdot"; break;
	    default: Description += "solid";
    }

    Description += ", pointer=";
    switch(PointerStyle) {
	    case psRectangle : Description += "rectangle"; break;
	    case psCircle : Description += "circle"; break;
	    case psTriangle : Description += "triangle"; break;
	    case psDiamond : Description += "diamond"; break;
	    default : Description += "nothing";
    }
    return(Description);
}

TColor ReadColor(AnsiString aColor, TColor DColor)
{
	TColor  Color = DColor;

	if(!aColor.IsEmpty()) {
		if(aColor == "white")   return(clWhite);
		if(aColor == "black")   return(clBlack);
		if(aColor == "blue")    return(clBlue);
		if(aColor == "paleblue")return(clPaleBlue);
		if(aColor == "red")     return(clRed);
		if(aColor == "gray")    return(clLtGray);
		if(aColor == "darkgray")return(clDkGray);
		if(aColor == "green")   return(clSeaGreen);
		if(aColor == "orange")  return(clLightOrange);
		if(aColor == "violet")  return(clViolet);
		if(aColor == "c1")  	return(clc1);
		if(aColor == "c2")  	return(clc2);
		if(aColor == "c3")  	return(clc3);
		if(aColor == "c4")  	return(clc4);
		if(aColor == "c5")  	return(clc5);
		if(aColor == "c6")  	return(clc6);
		if(aColor == "c7")  	return(clc7);
		if(aColor == "c8")  	return(clc8);
		if(aColor == "c9")  	return(clc9);
	}
	return(Color);
}

TColor ReadColor(AnsiString aColor)
{
	return(ReadColor(aColor, clBlack));
}

IodePanel::IodePanel(AnsiString Description) {
	AnsiString lDescription, aBorder, aRounded;

	lDescription = Description.LowerCase();
	Color = ReadColor(GetArgument(lDescription, "color"), clWhite);
	aBorder = GetArgument(lDescription, "border");
	Border = 1;
	if(!aBorder.IsEmpty()) {
		if(aBorder == "0") 	Border = 0;
		else				Border = 1;
	}

	aRounded = GetArgument(lDescription, "rounded");
	Rounded = 0;
	if(!aRounded.IsEmpty()) Rounded = aRounded.ToInt();
}


IodeBar::IodeBar(AnsiString Description) {
	AnsiString lDescription, aBrushStyle, aBorder;

	Color = clBlack;
	BrushStyle = bsSolid;

	lDescription = Description.LowerCase();

	Color = ReadColor(GetArgument(lDescription, "color"));

	aBrushStyle = GetArgument(lDescription, "style");
	if(!aBrushStyle.IsEmpty()) {
		if(aBrushStyle == "solid")  BrushStyle = bsSolid;
		if(aBrushStyle == "cross")  BrushStyle = bsCross;
		if(aBrushStyle == "diagcross")  BrushStyle = bsDiagCross;
		if(aBrushStyle == "bdiagonal")  BrushStyle = bsBDiagonal;
		if(aBrushStyle == "fdiagonal")  BrushStyle = bsFDiagonal;
		if(aBrushStyle == "horizontal")  BrushStyle = bsHorizontal;
		if(aBrushStyle == "vertical")  BrushStyle = bsVertical;
	}
	aBorder = GetArgument(lDescription, "border");
	Border = 1;
	if(!aBorder.IsEmpty()) {
		if(aBorder == "0") 	Border = 0;
		else				Border = 1;
	}
}


IodeLine::IodeLine(AnsiString Description) {
	AnsiString lDescription, aWidth, aPenStyle, aPointerStyle;

	Color = clBlack;
	Width = 2;
	PenStyle = psSolid;
	PointerStyle = psNothing;

	lDescription = Description.LowerCase();

	Color = ReadColor(GetArgument(lDescription, "color"));

	aWidth = GetArgument(lDescription, "width");
	if(!aWidth.IsEmpty()) Width = aWidth.ToInt();

	aPenStyle = GetArgument(lDescription, "style");
	if(!aPenStyle.IsEmpty()) {
//            if(aPenStyle == "solid")    PenStyle = psSolid;
		if(aPenStyle == "dash")     PenStyle = psDash;
		if(aPenStyle == "dot")      PenStyle = psDot;
		if(aPenStyle == "dashdot")  PenStyle = psDashDot;
	}

	aPointerStyle = GetArgument(lDescription, "pointer");
	if(!aPointerStyle.IsEmpty()) {
//            if(aPointerStyle == "nothing")      PointerStyle = psNothing;
		if(aPointerStyle == "rectangle")    PointerStyle = psRectangle;
		if(aPointerStyle == "circle")       PointerStyle = psCircle;
		if(aPointerStyle == "triangle")     PointerStyle = psTriangle;
		if(aPointerStyle == "diamond")      PointerStyle = psDiamond;
	}
};

IodeLine *GetLine(int i)
{
	int         nc = LIodeLines->Count;
	IodeLine   *Line = (IodeLine *) LIodeLines->Items[i%nc];

	return(Line);
}

IodeBar *GetBar(int i)
{
	int         nc = LIodeBars->Count;
	IodeBar   *Bar = (IodeBar *) LIodeBars->Items[i%nc];

	return(Bar);
}

void SaveTheme(AnsiString Filename)
{
	int         i;
	TStringList *Theme = new TStringList;

    for(i = 0; i < LIodeLines->Count; i++)
	    Theme->Add("Line " + GetLine(i)->Describe());

    for(i = 0; i < LIodeBars->Count; i++)
	    Theme->Add("Bar " + GetBar(i)->Describe());

    Theme->SaveToFile(Filename);

    delete Theme;
}

IodeSetFont(TTeeFont *tteeFont, TFont *tFont)
{
    if(tFont) {
		tteeFont->Name = tFont->Name;
		tteeFont->Size = tFont->Size;
		tteeFont->Style = tFont->Style;
		tteeFont->Color = tFont->Color;
    }
}


void ThemeInit(AnsiString Filename, bool reset = false)
{
	int             i;
	AnsiString      Description, ExeName;
	TStringList     *Theme;

	if(LIodeLines &&  LIodeBars && !reset) return;

	if(reset && LIodeLines && LIodeBars) {
		//clean up first
		for(i = 0; i < LIodeLines->Count; i++) delete GetLine(i);
		delete LIodeLines;

		for(i = 0; i < LIodeBars->Count; i++) delete GetBar(i);
		delete LIodeBars;

		delete TitleFont; TitleFont = NULL;
		delete LegendFont; LegendFont = NULL;
		delete AxisFont; AxisFont = NULL;
	}

	LIodeLines = new TList();
	LIodeBars = new TList();

	Filename = ChangeFileExt(Filename,".thm");
	if(!FileExists(Filename)) {
		ExeName = Application->ExeName;
		ExeName = ExtractFilePath(ExeName);
		Filename = ExeName + "\\" + ExtractFileName(Filename);
	}
	if(FileExists(Filename)) {
	 Theme = new TStringList;
	 Theme->LoadFromFile(Filename);
	 for(i = 0; i < Theme->Count; i++) {
		Description = Theme->Strings[i];
		if(Description.Pos("TitleFont")) TitleFont = IodeFont(Description);
		if(Description.Pos("LegendFont")) LegendFont = IodeFont(Description);
		if(Description.Pos("AxisFont")) AxisFont = IodeFont(Description);
		if(Description.Pos("Line")) LIodeLines->Add(new IodeLine(Description));
		if(Description.Pos("Bar")) LIodeBars->Add(new IodeBar(Description));
		if(Description.Pos("Panel")) IodeChartPanel = new IodePanel(Description);
		if(Description.Pos("Grid")) IodeGridLine = new IodeLine(Description);
	 }
	 if(IodeChartPanel == NULL) IodeChartPanel = new IodePanel(clWhite, 0, 0);
	 if(IodeGridLine == NULL) IodeGridLine = new IodeLine(clBlack, 1, psSolid, psNothing);
	 delete Theme;
	}
	else {
		LIodeLines->Add(new IodeLine(clBlue, 2, psSolid, psNothing));
		LIodeLines->Add(new IodeLine(clLightOrange, 2, psSolid, psNothing));
		LIodeLines->Add(new IodeLine(clSeaGreen, 2, psSolid, psNothing));
		LIodeLines->Add(new IodeLine(clRed, 2, psSolid, psRectangle));
		LIodeLines->Add(new IodeLine(clViolet, 2, psSolid, psCircle));

		LIodeBars->Add(new IodeBar(clLightOrange, bsSolid,1));
		LIodeBars->Add(new IodeBar(clPaleBlue, bsSolid,1));
		LIodeBars->Add(new IodeBar(clBlue, bsBDiagonal,1));
		LIodeBars->Add(new IodeBar(clRed, bsFDiagonal,1));

		IodeChartPanel = new IodePanel(clWhite, 0, 0);
		IodeGridLine = new IodeLine(clBlack, 1, psSolid, psNothing);
	}
}

int GetIodeName(char *iodeini)
{
	if(GetTempPath(255, iodeini) == 0) strcpy(iodeini, "iode.ini");
	else                               strcat(iodeini, "\\iode.ini");
	return(0);
}

//---------------------------------------------------------------------------

TfrmChart *frmChart[CHRT_MAXHDLS];

//---------------------------------------------------------------------------
__fastcall TfrmChart::TfrmChart(TComponent* Owner)
    : TForm(Owner)
{
	int         i, ppi;
	AnsiString  Line;
	TIniFile    *ini;
	char        tpath[256];

	GetIodeName(tpath);
	ini = new TIniFile(tpath);
//    Top = ini->ReadInteger("Chart", "Top", 20);
//    Left = ini->ReadInteger("Chart", "Left", 20);
	Width = ini->ReadInteger("Chart", "Width", 474);
	Height = ini->ReadInteger("Chart", "Height", 347);
	Theme = ini->ReadString("Chart", "Theme", "default.thm");
	ThemeInit(Theme);
	delete ini;

	// disable scroll
	Chart->AllowPanning = Forms::pmNone;
	// Add SelectorTool
	if(Selector == NULL) {
	   Selector = new TSelectorTool(NULL);
	   Selector->Brush->Color = clWhite;

	   Selector->ParentChart = Chart;
	   Selector->ShowInEditor = true;
	}

	// Chart Panel
	if(IodeChartPanel->Border) 	Chart->Border->Visible = true;
	else						Chart->Border->Visible = false;
	Chart->BorderRound = IodeChartPanel->Rounded;
	Chart->Color = IodeChartPanel->Color;

	Chart->BottomAxis->Grid->Color = IodeGridLine->Color;
	Chart->BottomAxis->Grid->Style = IodeGridLine->PenStyle;
	Chart->BottomAxis->Grid->Width = IodeGridLine->Width;
	Chart->BottomAxis->Grid->SmallDots = true;

	Chart->LeftAxis->Grid->Color = IodeGridLine->Color;
	Chart->LeftAxis->Grid->Style = IodeGridLine->PenStyle;
	Chart->LeftAxis->Grid->Width = IodeGridLine->Width;
	Chart->LeftAxis->Grid->SmallDots = true;

	Chart->RightAxis->Grid->Color = IodeGridLine->Color;
//	Chart->RightAxis->Grid->Style = IodeGridLine->PenStyle;
	Chart->RightAxis->Grid->Width = IodeGridLine->Width;
	Chart->RightAxis->Grid->SmallDots = true;
}


__fastcall TfrmChart::ChrtAddColorBand(double startvalue, double endvalue, TColor color)
{
//  TColorBandTool    *ColorBand;

	if(ColorBand == NULL) ColorBand = new TColorBandTool(NULL);

	ColorBand->Axis = Chart->BottomAxis;
	ColorBand->DrawBehindAxes = true;

	ColorBand->StartValue = startvalue;
	ColorBand->EndValue = endvalue;
	ColorBand->Color = color;
	ColorBand->Transparency = 75;
}

__fastcall TfrmChart::ChrtAdd(char *title,int axis, int type, int nobs, double *vals)
{
	int         i, Border;
	double      oval = 0.0;
	TColor      Color;
	IodeLine    *Line;
	IodeBar     *Bar;

	TPointSeries *SSeries;
	TLineSeries  *LSeries;
	TAreaSeries  *ASeries;
	TBarSeries   *BSeries;
	TChartSeries *Series;
	AnsiString	  Buf;

//    nc = Chart->SeriesCount();
	// if line chart and multibar selected enforce bar
	if(type == 'L' & B_MULTIBAR > 0) type = 'B';
	switch(type) {
	case 'S' :
		Line = GetLine(nl);
		Color = Line->Color;
		SSeries = new  TPointSeries(this);
		SSeries->LinePen->Width = Line->Width;
		SSeries->LinePen->Style = Line->PenStyle;
		if(Line->PointerStyle != psNothing) {
			SSeries->Pointer->Style = Line->PointerStyle;
			SSeries->Pointer->Size = Line->Width + 2;
			SSeries->Pointer->Pen->Visible = false;
		}
		else
			SSeries->Pointer->Style = psCircle;
		SSeries->Pointer->Visible = true;
		TypeSeries += "S";
		Series = SSeries;
		nl++;
		break;
	case 'B' :
		BSeries = new  TBarSeries(this);
		Bar = GetBar(nb);
		Color = Bar->Color;
		Border = Bar->Border;
		BSeries->BarBrush->Style = Bar->BrushStyle;
		if(Border == 0) BSeries->BarPen->Hide();
		switch(B_MULTIBAR) {
			case 0: BSeries->MultiBar = Series::mbSide; break;
			case 1: BSeries->MultiBar = mbStacked; break;
			case 2: BSeries->MultiBar = mbStacked100; break;
		}
		Series = BSeries;
		TypeSeries += "B";
		nb ++;
		break;
	case 'A' :
		ASeries = new  TAreaSeries(this);
		Bar = GetBar(nb);
		Color = Bar->Color;
		ASeries->AreaBrush = Bar->BrushStyle;
		switch(B_MULTIBAR) {
			case 0: ASeries->MultiArea = Series::maNone; break;
			case 1: ASeries->MultiArea = maStacked; break;
			case 2: ASeries->MultiArea = maStacked100; break;
		}

		Series = ASeries;
		TypeSeries += "A";
		nb++;
		break;
	default  :
		LSeries = new  TLineSeries(this);
		Line = GetLine(nl);
		Color = Line->Color;
		LSeries->LinePen->Width = Line->Width;
		LSeries->LinePen->Style = Line->PenStyle;
		LSeries->Pointer->Style = Line->PointerStyle;
		LSeries->Pointer->Size = Line->Width + 3;
		LSeries->Pointer->Pen->Visible = true;
		LSeries->Pointer->Pen->Width = Line->Width;
		LSeries->Pointer->Pen->Color = Line->Color;
		LSeries->Pointer->Brush->Color = clWhite;
		LSeries->Pointer->Visible = true;

		TypeSeries += "L";
		Series = LSeries;
		nl++;
	}

	Series->Marks->Visible = false;
	Series->ParentChart = Chart;
	if(title != NULL && title[0] != '\0') {
		Series->Title = title;
		if(Series->Title.Length() > 90)
			Series->Title = Series->Title.SubString(1, 88) + "..." ;
		nbLegend ++;
	}
	if(axis) Series->VertAxis = aRightAxis;

	/* provide a default value for NA */
	for(i = 1; i < nobs && oval == 0.0; i=i+2)
	if(SCR_ISAN(vals[i])) oval = vals[i];
	if(B_MULTIBAR >0) {
		for(i = 0; i < nobs; i++)
			if(SCR_ISAN(vals[i*2+1])) {
				Buf = IntToStr((int) vals[i*2]);
				Series->AddY(vals[i*2+1], Buf.c_str(), clTeeColor);
			}
	}
	else {
		for(i = 0; i < nobs; i++)  {
			if(SCR_ISAN(vals[i*2+1]))
				Series->AddXY(vals[i*2], vals[i*2+1], "", clTeeColor);
			else
			if(type != 'B') Series->AddNullXY(vals[i*2], oval, "");
		}
	}
	Series->SeriesColor = Color;
	return(1);
}
__fastcall TfrmChart::ChrtAddTitle(char *title)
{
	Chart->Title->Text->Add(title);
}

__fastcall TfrmChart::ChrtParams(char *title, double ymin, double ymax, double zmin, double zmax)
{
    IodeSetFont(Chart->Title->Font, TitleFont);
	IodeSetFont(Chart->Legend->Font, LegendFont);
	Chart->Legend->Symbol->Shadow->Visible = false;
    IodeSetFont(Chart->BottomAxis->LabelsFont, AxisFont);
    IodeSetFont(Chart->LeftAxis->LabelsFont, AxisFont);
    IodeSetFont(Chart->RightAxis->LabelsFont, AxisFont);

	Chart->Title->Text->Clear();
	if(!emf) {
        Chart->Title->Text->Add(title);
        Caption = title;
    }
	else Chart->Title->Visible = false;
    Chart->RemoveAllSeries();
    nl = 0;
    nb = 0;

    ChrtYMin = ymin;
    ChrtYMax = ymax;
    ChrtZMin = zmin;
	ChrtZMax = zmax;

	Chart->BottomAxis->AxisValuesFormat = "";
	return(0);
}

int  TfrmChart::ChrtScale()
{
	/* Y axis */
	if(B_MULTIBAR == 2)
		ChrtScaleAxis(Chart, Chart->LeftAxis, 0, 100);
	else
		ChrtScaleAxis(Chart, Chart->LeftAxis, ChrtYMin, ChrtYMax);
	/* Z axis */
	ChrtScaleAxis(Chart, Chart->RightAxis, ChrtZMin, ChrtZMax);

	/* X-axis */
/*    ChrtScaleAxis(Chart, Chart->BottomAxis, L_NAN, L_NAN, false);
	if(Chart->BottomAxis->CalcIncrement() < 1.0)

*/
	Chart->BottomAxis->Increment = 1.0;
	Chart->BottomAxis->MinorTicks->Visible = false;
	if(ColorBandStartValue != ColorBandEndValue)
	ChrtAddColorBand(ColorBandStartValue, ColorBandEndValue, clLtGray);
    return(1);
}


//---------------------------------------------------------------------------
void __fastcall TfrmChart::SaveAs(TObject *Sender)
{
    if(dlgSaveDialog->Execute()) {
	    SaveChartToFile(Chart, dlgSaveDialog->FileName, true, true);
    }
}

void __fastcall TfrmChart::Print(TObject *Sender)
{
    Chart->Print();
}

void __fastcall TfrmChart::OnClose(TObject *Sender, TCloseAction &Action)
{
    int         i;
    TIniFile    *ini;
    AnsiString  Line;
    char        tpath[256];

    GetIodeName(tpath);
    ini = new TIniFile(tpath);
    ini->WriteInteger("Chart", "Width", Width);
    ini->WriteInteger("Chart", "Height", Height);
	ini->WriteString("Chart", "Theme", Theme);
    delete ini;

	Chart->RemoveAllSeries();
//    delete frmChart[Chrthdl];
	frmChart[Chrthdl] = NULL;
	Action = caFree; // delete form
}

void __fastcall TfrmChart::ClickSeries(TCustomChart *Sender,
      TChartSeries *Series, int ValueIndex, TMouseButton Button,
      TShiftState Shift, int X, int Y)
{
    EditSeries(this, Series);
}


//---------------------------------------------------------------------------
int TChrtInit(bool emf, int wgoal, int hgoal)
{
    int     hdl;
    int     p, r, c, offset = 20;
    int     nc = 2, nr = 2, ppi;

    if(emf) {
        hdl = 0;
        if(frmChart[hdl] != NULL) {
	        ppi = Screen->PixelsPerInch;
	        frmChart[hdl]->Chart->Width = (wgoal*ppi)/1440;
	        frmChart[hdl]->Chart->Height = (hgoal*ppi)/1440;

            return(hdl);
        }
    }
	else
        for(hdl = 0; hdl < CHRT_MAXHDLS && frmChart[hdl] ; hdl++) ;

    if(hdl < CHRT_MAXHDLS) {
	    frmChart[hdl] = new TfrmChart(NULL);
		frmChart[hdl]->Chrthdl = hdl;
	    frmChart[hdl]->emf = emf;

    	if(emf) {
	        ppi = Screen->PixelsPerInch;
	        frmChart[hdl]->Chart->Width = (wgoal*ppi)/1440;
	        frmChart[hdl]->Chart->Height = (hgoal*ppi)/1440;
	    }
	    else {
		    if(GetArange() == "Tile") {
		        nc = Screen->Width/frmChart[hdl]->Width;
		        nr = Screen->Height/frmChart[hdl]->Height;
		        p = hdl/(nc*nr);
		        c = (hdl%(nc*nr))%nc;
		        r = (hdl%(nc*nr))/nc;
		    }
		    else {
		        p = hdl;
		        c = r = 0;
	        }

	    frmChart[hdl]->Left = offset * (1 + p) + frmChart[hdl]->Width * c;
	    frmChart[hdl]->Top =  offset * (1 + p) + frmChart[hdl]->Height * r;
	    }
    }
    else hdl = -1;

    return(hdl);
}

int TChrtEnd(int hdl)
{
	TChart  *Chart;
	if(hdl < 0 || hdl > CHRT_MAXHDLS) return(-1);

	frmChart[hdl]->ChrtScale();
	Chart = frmChart[hdl]->Chart;
	if(frmChart[hdl]->nbLegend == 0) Chart->Legend->Visible = false;
	else Chart->Legend->Visible = true;
//	Chart->Title->Text->Add(frmChart[hdl]->TypeSeries.c_str());
	frmChart[hdl]->Show();
	return(0);
}

int TChrtEnd(int hdl, char *emfFile)
{
	TChart  *Chart;

	if(hdl < 0 || hdl > CHRT_MAXHDLS) return(-1);

	frmChart[hdl]->ChrtScale();
	if(frmChart[hdl]->Selector) frmChart[hdl]->Selector->DrawHandles = false;
	Chart = frmChart[hdl]->Chart;
	if(frmChart[hdl]->nbLegend == 0) Chart->Legend->Visible = false;
	else Chart->Legend->Visible = true;
	Chart->SaveToMetafileEnh(emfFile);

	Chart->RemoveAllSeries();
	frmChart[hdl]->nl = 0;
	frmChart[hdl]->nb = 0;
	if(frmChart[hdl]->ColorBand) {
		delete frmChart[hdl]->ColorBand;
		frmChart[hdl]->ColorBand = NULL;
	}
//    delete frmChart[hdl]; werkt blijkbaar niet in C
//    frmChart[hdl] = NULL;

	return(0);
}

int TChrtAdd(int hdl, char *title, int axis, int type, int nobs, double *xy_vals)
{
	if(hdl < 0 || hdl > CHRT_MAXHDLS) return(-1);
	frmChart[hdl]->ChrtAdd(title, axis, type, nobs, xy_vals);
    return(0);
}

int TChrtAddColorBand(int hdl, double startvalue, double endvalue, TColor bcolor)
{
    if(hdl < 0 || hdl > CHRT_MAXHDLS) return(-1);
    frmChart[hdl]->ChrtAddColorBand(startvalue, endvalue, bcolor);
    return(0);

}

int TChrtAddTitle(int hdl, char *title)
{
    if(hdl < 0 || hdl > CHRT_MAXHDLS) return(-1);
	frmChart[hdl]->ChrtAddTitle(title);
    return(0);
}

int TChrtColorBand(double startvalue, double endvalue)
{
    ColorBandStartValue = startvalue;
    ColorBandEndValue = endvalue;
    return(0);
}

int TChrtParams(int hdl, char *title, double ymin, double ymax, double zmin, double zmax)
{
    if(hdl < 0 || hdl > CHRT_MAXHDLS) return(-1);
	frmChart[hdl]->ChrtParams(title, ymin, ymax, zmin, zmax);
    return(0);
}

void TChrtThemeInit(char *filename)
{
    ThemeInit(filename, true);
}

//---------------------------------------------------------------------------
extern "C" unsigned char *SCR_OemToAnsi(char *ansi, char *oem);

extern "C" int ChrtInit()
{
    return(TChrtInit(false, 0, 0));
}

extern "C" int ChrtEnd(int hdl)
{
    return(TChrtEnd(hdl));
}

extern "C" int ChrtInitEmf(int wgoal, int hgoal)
{
    return(TChrtInit(true, wgoal, hgoal));
}

extern "C" int ChrtEndEmf(int hdl)
{
	return(TChrtEnd(hdl, "_scr4.emf"));
}


extern  "C" int ChrtAdd(int hdl, char *title, int axis, int type, int nobs, double *xy_vals)
{
    SCR_OemToAnsi(title, title);
    return(TChrtAdd(hdl, title, axis, type, nobs, xy_vals));
}

extern  "C" int ChrtAddTitle(int hdl, char *title)
{
    SCR_OemToAnsi(title, title);
	return(TChrtAddTitle(hdl, title));
}

extern  "C" int ChrtColorBand(double sv, double ev)
{
    return(TChrtColorBand(sv, ev));
}

extern "C" extern unsigned char    *SCR_stracpy(unsigned char    *in);
extern "C" extern int SCR_free(void *ptr);

extern  "C" int ChrtParams(int hdl, char *title, double ymin, double ymax, double zmin, double zmax)
{
    int     rc;
    char    *atitle = SCR_stracpy(title);

    SCR_OemToAnsi(atitle, title);
    rc = TChrtParams(hdl, atitle, ymin, ymax, zmin, zmax);
    SCR_free(atitle);
	return(rc);
}

extern "C" void ChrtThemeInit(char *filename)
{
   TChrtThemeInit(filename);
}


void __fastcall TfrmChart::View3D(TObject *Sender)
{
    Chart->View3D = !(Chart->View3D);
    opt3DView->Checked = Chart->View3D;
}
//---------------------------------------------------------------------------

void __fastcall TfrmChart::Edit(TObject *Sender)
{
    edChart->Execute();
}

void __fastcall TfrmChart::frmClose(TObject *Sender)
{
    this->Close();
}
//---------------------------------------------------------------------------
void __fastcall TfrmChart::optLabelClick(TObject *Sender)
{
    int     i;
    TChartSeries    *Series;

    for(i = 0; i < Chart->SeriesList->Count; i++) {
//      Series = Chart->SeriesList->Series[i];
	Series = Chart->Series[i];
	Series->Marks->Visible = !(Series->Marks->Visible);
    }
    optLabel->Checked = Series->Marks->Visible;
}
//---------------------------------------------------------------------------
void __fastcall TfrmChart::FormKeyPress(TObject *Sender, char &Key)
{
    if(Key == VK_ESCAPE) Close();
//    if(Key == VK_CONTROL) Chart->CopyToClipboardMetafile(true);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

AnsiString GetArange()
{
    TIniFile    *ini;
    AnsiString  Arange;
    char        tpath[256];

    GetIodeName(tpath);
    ini = new TIniFile(tpath);
    Arange = ini->ReadString("Chart", "Arange", "Cascade");
    delete ini;
    return(Arange);
}

void SetArange(AnsiString Str)
{
    TIniFile    *ini;
    char        tpath[256];

	GetIodeName(tpath);
    ini = new TIniFile(tpath);
    ini->WriteString("Chart", "Arange", Str);
    delete ini;
}


void __fastcall TfrmChart::optCopyClick(TObject *Sender)
{
    if(Selector != NULL) Selector->DrawHandles = false;
    Chart->CopyToClipboardMetafile(true);
    if(Selector != NULL) Selector->DrawHandles = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmChart::ChartDblClick(TObject *Sender)
{
    if(Selector != NULL) {
	    if(Selector->Part.Part != cpNone) {
		    // show chart editor for the current selected part
		    EditChartPart(NULL, Chart, Selector->Part);
		    Chart->CancelMouse = true;
		    Selector->StopDragging();
	    }
    }
}

void __fastcall TfrmChart::optExportClick(TObject *Sender)
{
	if(Selector) Selector->DrawHandles = false;
	TeeExport(this, Chart);
	if(Selector) Selector->DrawHandles = true;

}


void __fastcall TfrmChart::LoadThemeClick(TObject *Sender)
{
	TPointSeries *SSeries;
	TLineSeries  *LSeries;
	TBarSeries   *BSeries;
	TColor        Color;
	IodeLine     *Line;
	IodeBar      *Bar;
	int          i, nc, nb, nl;
	AnsiString   CurDir;

	nb = 0; nl = 0;
	CurDir = GetCurrentDir();
	if(dlgOpenDialog->Execute()) {
		Theme = dlgOpenDialog->FileName;
		ThemeInit(Theme, true);
		SetCurrentDir(CurDir); // go back to original directory
		nc = Chart->SeriesCount();
		for (i = 0; i < nc; i++) {
			switch(this->TypeSeries[1+i]) {
			case 'B':
				Bar = GetBar(nb);
				Color = Bar->Color;
				BSeries = dynamic_cast<TBarSeries *>(Chart->Series[i]);
				/* clear Brush first */
				BSeries->BarBrush->Style =  bsSolid;
				BSeries->BarBrush->Color =  clWhite;
				BSeries->SeriesColor = Color;
				BSeries->BarBrush->Style = Bar->BrushStyle;
				nb ++;
				break;
			case 'L':
				Line = GetLine(nl);
				Color = Line->Color;
				LSeries = dynamic_cast<TLineSeries *> (Chart->Series[i]);
				LSeries->Pen->Style = Line->PenStyle;
				LSeries->Pen->Width = Line->Width;
				LSeries->Pointer->Style = Line->PointerStyle;
				LSeries->Pointer->Color = Color;
				LSeries->SeriesColor = Color;
			    nl ++;
			    break;

	        }
	    }
	    Chart->Repaint();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmChart::DefaultThemeClick(TObject *Sender)
{

    Theme = "default.thm";
    ThemeInit(Theme, true);
	IodeSetFont(Chart->Title->Font, TitleFont);
    IodeSetFont(Chart->Legend->Font, LegendFont);
    IodeSetFont(Chart->BottomAxis->LabelsFont, AxisFont);
    IodeSetFont(Chart->LeftAxis->LabelsFont, AxisFont);
    IodeSetFont(Chart->RightAxis->LabelsFont, AxisFont);
}
//---------------------------------------------------------------------------
// Copied form s_a2m
typedef struct _a2mpen_ {
	double  p_width;                /* valeur en pts */
	char    p_color[9];
	short   p_nbsegments;
	double  p_segment[8];           /* valeur en pts */
} A2MPEN;

typedef struct _a2mbrush_ {
	short   b_fill;
	char    b_color[9];
} A2MBRUSH;


extern int	A2MNBRPEN;
extern int	A2MNBRBRUSH;

A2MPEN      *A2mFrGrfdPen = NULL;
A2MBRUSH    *A2mFrGrfdBrush = NULL;
A2MPEN      A2mFrGridPen = {0.5, "Grid", 0, {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}};

char	*FrmColor(TColor Color)
{
	return(WriteColor(Color).UpperCase().c_str());
}

extern int A2M_FONTSIZE;

double dash_pattern[8]= {8.0, 2.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,};
double dot_pattern[8]= {3.0, 3.0, 3.0, 3.0, 0.0, 0.0, 0.0, 0.0,};
double dashdot_pattern[8]= {4.0, 2.0, 4.0, 0.0, 0.0, 0.0, 0.0, 0.0,};

int  A2mFrGrfPrepareCC()
{
	int         i, ppi;
	AnsiString  Theme;
	TIniFile    *ini;
	char        tpath[256];
	IodeLine   	*Line;
	IodeBar		*Bar;

	GetIodeName(tpath);
	ini = new TIniFile(tpath);
	Theme = ini->ReadString("Chart", "Theme", "default.thm");
	ThemeInit(Theme);
	delete ini;

	A2M_FONTSIZE = AxisFont->Size;

	A2MNBRPEN = 1 + LIodeLines->Count;
	A2mFrGrfdPen = (A2MPEN *) malloc(A2MNBRPEN*sizeof(A2MPEN));
	for(i = 0; i < A2MNBRPEN; i++)
		memcpy(A2mFrGrfdPen + i, &A2mFrGridPen, sizeof(A2MPEN));
	for(i = 0; i < LIodeLines->Count; i++) {
		Line = (IodeLine *) LIodeLines->Items[i];
		strcpy(A2mFrGrfdPen[i + 1].p_color, FrmColor(Line->Color));
		A2mFrGrfdPen[i + 1].p_width = 0.5 * Line->Width;
		switch(Line->PenStyle) {
		    case psDash: memcpy(&(A2mFrGrfdPen[i + 1].p_segment), dash_pattern, 8 * sizeof(double)); A2mFrGrfdPen[i + 1].p_nbsegments = 2;break;
		    case psDot: memcpy(&(A2mFrGrfdPen[i + 1].p_segment), dot_pattern, 8 * sizeof(double)); A2mFrGrfdPen[i + 1].p_nbsegments = 4;break;
		    case psDashDot: memcpy(&(A2mFrGrfdPen[i + 1].p_segment), dashdot_pattern, 8 * sizeof(double)); A2mFrGrfdPen[i + 1].p_nbsegments = 3;break;
	     	}
	}


	A2MNBRBRUSH = 1 + LIodeBars->Count;
	A2mFrGrfdBrush = (A2MBRUSH *) malloc(A2MNBRPEN*sizeof(A2MBRUSH));
	strcpy(A2mFrGrfdBrush[0].b_color, "Black");
	A2mFrGrfdBrush[0].b_fill = 15;

	for(i = 0; i < LIodeBars->Count; i++) {
		Bar = (IodeBar *) LIodeBars->Items[i];
		strcpy(A2mFrGrfdBrush[i + 1].b_color, FrmColor(Bar->Color));
		switch(Bar->BrushStyle) {
			case bsCross: A2mFrGrfdBrush[i + 1].b_fill = 2; break;
			case bsDiagCross: A2mFrGrfdBrush[i + 1].b_fill = 8; break;
			case bsBDiagonal: A2mFrGrfdBrush[i + 1].b_fill = 9; break;
			case bsFDiagonal: A2mFrGrfdBrush[i + 1].b_fill = 4; break;
			case bsHorizontal: A2mFrGrfdBrush[i + 1].b_fill = 12; break;
			case bsVertical: A2mFrGrfdBrush[i + 1].b_fill = 11; break;
			default: A2mFrGrfdBrush[i + 1].b_fill = 0;
		}
	}

	return(1);
}


extern "C" int A2mFrGrfPrepare()
{
	int         i, j, p;
	A2MPEN      *Pen;
	A2MBRUSH    *Brush;


	if(A2mFrGrfdPen != NULL && A2mFrGrfdBrush != NULL) return(0);
	if(A2mFrGrfPrepareCC()) return(0);

	A2mFrGrfdPen = (A2MPEN *) malloc(A2MNBRPEN*sizeof(A2MPEN));
	A2mFrGrfdBrush = (A2MBRUSH *) malloc(A2MNBRBRUSH*sizeof(A2MBRUSH));
	/* Default pen */
	Pen = A2mFrGrfdPen;
	Pen->p_width = 0.2;
	strcpy(Pen->p_color, "Black");
	Pen->p_nbsegments = 0;
	for(j = 0; j < 8; j++) Pen->p_segment[j] = 0.0;

	/* Pen */
	for(i = 1; i < A2MNBRPEN - 1; i++) {
		Pen = A2mFrGrfdPen + i;

		p = (i - 1)/2;
		if (i%2) Pen->p_width = 0.5;
		else     Pen->p_width = 1.0;
		strcpy(Pen->p_color, "Blue");

		Pen->p_nbsegments = 0;
		for(j = 0; j < 8; j++) Pen->p_segment[j] = 0.0;
	}

	/* Default Brush */
	Brush = A2mFrGrfdBrush;
	strcpy(Brush->b_color, "Black");
	Brush->b_fill = 15;
	/* Brush */
	for(i = 1; i < A2MNBRBRUSH; i++) {
		Brush = A2mFrGrfdBrush + i;

		strcpy(Brush->b_color, "Blue");
		Brush->b_fill = 3;
	}
	return(0);
}

