//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "frmMain.h"
#include "c:\usr\iode_src\api\iode.h"
#include "c:\usr\iode_src\api\iodeapi.h"



//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

TfrmM *frmM;
int CCIodeDebugOn = 0;
int CCIodeOEMToAnsi = 1;
int CCIodeError = 0;
AnsiString  CCErrorBuf;
//---------------------------------------------------------------------------
__fastcall TfrmM::TfrmM(TComponent* Owner)
	: TForm(Owner)
{
	State = 0;
}
void TfrmStatus(char *buf)
{
	AnsiString  ABuf;

	ABuf.sprintf("%s %s", ">>", buf);
	frmM->mLog->Lines->Add(buf);
}

void TfrmShow(int show)
{
    if(show) frmM->Show();
    else     frmM->Hide();
}
/* A2mMessage */
int A2mMessage(char * msg)
{
	if(CCIodeDebugOn) {
		AnsiString  ABuf;

		ABuf.sprintf("DEBUG:%s", msg);
		frmM->mLog->Lines->Add(ABuf.c_str());
		frmM->ToggleState();
	}
}


void CCIodeDebug(char *cmd, char *arg1)
{
	if(CCIodeDebugOn) {
		AnsiString  ABuf;

		ABuf.sprintf("DEBUG:%s %s", cmd, arg1);
		frmM->mLog->Lines->Add(ABuf.c_str());
		frmM->ToggleState();
	}
}

extern "C" FrmStatus(char *buf)
{
    TfrmStatus(buf);
}

void TfrmError(char *buf)
{
    CCIodeError = 1;
    CCErrorBuf.sprintf("IODEERROR: %s", buf);
    frmM->mError->Lines->Add(CCErrorBuf);
}

extern "C" FrmError(char *buf)
{
	TfrmError(buf);
}

extern "C" MyShowMessage(char * buf)
{
//    ShowMessage(buf);
}
//---------------------------------------------------------------------------
void __fastcall TfrmM::FormClose(TObject *Sender, TCloseAction &Action)
{
    AnsiString  aTmp;
    char    tpath[256];

    GetTempPath(255, tpath);
    aTmp = tpath;
    mLog->Lines->Add("Closing ...");
    mLog->Lines->SaveToFile(aTmp + "lastlog.txt");
    mError->Lines->SaveToFile(aTmp + "lasterror.txt");
   	TrayMessage(NIM_DELETE);
}
//---------------------------------------------------------------------------



void __fastcall TfrmM::mnoShutdownClick(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool __fastcall TfrmM::TrayMessage(DWORD dwMessage)
{
   NOTIFYICONDATA tnd;
   PSTR pszTip;

//   pszTip = "IodeComServer";
   pszTip = IODE_VERSION;

   tnd.cbSize          = sizeof(NOTIFYICONDATA);
   tnd.hWnd            = Handle;
   tnd.uID             = IDC_MYICON;
   tnd.uFlags          = NIF_MESSAGE | NIF_ICON | NIF_TIP;
   tnd.uCallbackMessage	= MYWM_NOTIFY;

   if (dwMessage == NIM_MODIFY)
    {
        tnd.hIcon		= (HICON)IconHandle();
        if (pszTip)
           lstrcpyn(tnd.szTip, pszTip, sizeof(tnd.szTip));
	    else
        tnd.szTip[0] = '\0';
    }
   else
    {
        tnd.hIcon = NULL;
        tnd.szTip[0] = '\0';
    }

   return (Shell_NotifyIcon(dwMessage, &tnd));
}
//---------------------------------------------------------------------------
HICON __fastcall TfrmM::IconHandle(void)
{
    HICON   HIcon;

    if(State) HIcon = imgIcon_1->Picture->Icon->Handle;
    else      HIcon = imgIcon_2->Picture->Icon->Handle;

    State = !State;
    return (HIcon);
}

void __fastcall TfrmM::FormDestroy(TObject *Sender)
{
    TrayMessage(NIM_DELETE);
}
//---------------------------------------------------------------------------
void __fastcall TfrmM::MyNotify(TMessage& Msg)
{
    POINT MousePos;

    switch(Msg.LParam)
    {
        case WM_RBUTTONUP:
            if (GetCursorPos(&MousePos))
            {
                PopupMenu->PopupComponent = frmM;
                SetForegroundWindow(Handle);
                PopupMenu->Popup(MousePos.x, MousePos.y);
            }
            else
                Show();
            break;
        case WM_LBUTTONDBLCLK:
            Show();
            break;
        default:
            break;
    }
    TForm::Dispatch(&Msg);
}

extern AnsiString aExecName;

void __fastcall TfrmM::FormCreate(TObject *Sender)
{
    char    buffer[128];
    TrayMessage(NIM_ADD);
    TrayMessage(NIM_MODIFY);
	mLog->Lines->Add("IodeComServer v6.63 30/09/2020");
	mLog->Lines->Add(IODE_VERSION);
	mLog->Lines->Add("Working Directory " + GetCurrentDir());

/*
	strcpy(SCR_CMT_FILE, "iode");
	SCR_CMT_FILE_NB = 1;
	strcpy(SCR_NAME_ERR, "iode");
	SCR_ERR_FILE_NB = 2;
	strcpy(SCR_NAME_SCR, "iode");
	SCR_SCR_FILE_NB = 3;
*/

/*	strcpy(SCR_NAME_ERR, aExecName.c_str());
	SCR_ERR_FILE_NB = 1;
*/

	strcpy(SCR_NAME_ERR, "iode.msg");

}
//---------------------------------------------------------------------------

void __fastcall TfrmM::mnoPropertiesClick(TObject *Sender)
{
	int             i;
	AnsiString      ABuf;
	KDB             *kdb;

	for(i = 0; i < 7; i++) {
		kdb = K_WS[i];
		if(kdb != NULL)
			//ABuf.sprintf("%s [%d objects]", KNAME(kdb), KNB(kdb));
			ABuf.sprintf("%s [%d objects]", KNAMEPTR(kdb), KNB(kdb));  // JMP 13/8/2015
		else
			ABuf.sprintf("Type %d [0 objects]", i);
		mLog->Lines->Add(ABuf);
	}
    if(!Visible) Show();
}
//---------------------------------------------------------------------------

void __fastcall TfrmM::mnoHideClick(TObject *Sender)
{
    if(Visible) Hide();
    else        Show();
}
//---------------------------------------------------------------------------

void __fastcall TfrmM::ToggleState(void)
{
    TrayMessage(NIM_MODIFY);
}

LRESULT IconDrawItem(LPDRAWITEMSTRUCT lpdi)
{
	HICON hIcon;

	hIcon = (HICON)LoadImage(g_hinst, MAKEINTRESOURCE(lpdi->CtlID), IMAGE_ICON,
		16, 16, 0);
	if (!hIcon)
		return(FALSE);

	DrawIconEx(lpdi->hDC, lpdi->rcItem.left, lpdi->rcItem.top, hIcon,
		16, 16, 0, NULL, DI_NORMAL);

	return(TRUE);
}

void __fastcall TfrmM::DrawItem(TMessage& Msg)
{
     IconDrawItem((LPDRAWITEMSTRUCT)Msg.LParam);
     TForm::Dispatch(&Msg);
}



void __fastcall TfrmM::ResetFields1Click(TObject *Sender)
{
    mLog->Lines->Clear();
    mError->Lines->Clear();
}
//---------------------------------------------------------------------------

