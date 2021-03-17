//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <utilcls.h>
#include <atl\atlbase.h>
#include <atl\atlmod.h>
#include "CCIodeImpl.h"

#pragma link "iodeapi.lib"
#pragma link "scr4iode.lib"
#pragma link "odbc32.lib"

#pragma link "k_errorv"
#pragma link "s_iode"
#pragma link "sb_gui"


USEFORM("frmMain.cpp", frmM);
//---------------------------------------------------------------------------
TComModule _ProjectModule(0 /*InitATLServer*/);
TComModule &_Module = _ProjectModule;

// The ATL Object map holds an array of _ATL_OBJMAP_ENTRY structures that
// described the objects of your OLE server. The MAP is handed to your
// project's CComModule-derived _Module object via the Init method.
//
BEGIN_OBJECT_MAP(ObjectMap)
  OBJECT_ENTRY(CLSID_CCIode, TCCIodeImpl)
END_OBJECT_MAP()
HINSTANCE g_hinst;
//---------------------------------------------------------------------------
AnsiString aExecName;
WINAPI WinMain(HINSTANCE, HINSTANCE hInstance, LPSTR, int)
{
    char    buffer[128];
    try
    {
         Application->Initialize();
         g_hinst = hInstance;
         GetModuleFileName(g_hinst, buffer, 127);
         aExecName= buffer;
         Application->CreateForm(__classid(TfrmM), &frmM);
		Application->ShowMainForm = false;
         Application->Run();
    }
    catch (Exception &exception)
    {
         Application->ShowException(&exception);
    }
    return 0;
}
//---------------------------------------------------------------------------
