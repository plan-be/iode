// CCIODEIMPL.H : Declaration of the TCCIodeImpl

#ifndef CCIodeImplH
#define CCIodeImplH

#include "IodeCom_TLB.h"


/////////////////////////////////////////////////////////////////////////////
// TCCIodeImpl     Implements ICCIode, default interface of CCIode
// ThreadingModel : Apartment
// Dual Interface : TRUE
// Event Support  : FALSE
// Default ProgID : Project1.CCIode
// Description    : 
/////////////////////////////////////////////////////////////////////////////
class ATL_NO_VTABLE TCCIodeImpl : 
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<TCCIodeImpl, &CLSID_CCIode>,
  public IDispatchImpl<ICCIode, &IID_ICCIode, &LIBID_IodeCom>
{
public:
  TCCIodeImpl()
  {
  }

  // Data used when registering Object 
  //
  DECLARE_THREADING_MODEL(otApartment);
  DECLARE_PROGID("IodeCom.CCIode");
  DECLARE_DESCRIPTION("");

  // Function invoked to (un)register object
  //
  static HRESULT WINAPI UpdateRegistry(BOOL bRegister)
  {
    TTypedComServerRegistrarT<TCCIodeImpl> 
    regObj(GetObjectCLSID(), GetProgID(), GetDescription());
    return regObj.UpdateRegistry(bRegister);
  }


BEGIN_COM_MAP(TCCIodeImpl)
  COM_INTERFACE_ENTRY(ICCIode)
  COM_INTERFACE_ENTRY2(IDispatch, ICCIode)
END_COM_MAP()

// ICCIode
public:
 
  STDMETHOD(MWsLoad(BSTR FileName, int Type, int* Status));
  STDMETHOD(MExecute(BSTR Cmd, int* Status));
  STDMETHOD(MReport(BSTR FileName, int* Status));
  STDMETHOD(MGetObject(BSTR Name, int Type, BSTR* Object));
  STDMETHOD(MGetVar(BSTR Name, int T, VARIANT* Value));
  STDMETHOD(MStatus(int Type, long* Nb, BSTR* Name));
//  STDMETHOD(MGetVector(BSTR Name, VARIANT* Value));
  STDMETHOD(MGetMaxt(long* Value));
  STDMETHOD(MGetFreq(long* Value));
  STDMETHOD(MGetBegin(long* Year, long* Sub, long* Freq));
  STDMETHOD(MGetEnd(long* Year, long* Sub, long* Freq));
  STDMETHOD(MGetBeginSub(long* Value));
  STDMETHOD(MGetBeginYear(long* Value));
  STDMETHOD(MVector(BSTR Name, long Offset, long Length, LPSAFEARRAY* Value));
  STDMETHOD(get_PSample(LPSAFEARRAY* Value));
  STDMETHOD(MContents(int Type, BSTR Pattern, LPSAFEARRAY* List));
  STDMETHOD(MTest(BSTR Name, int Formatted, int ToAnsi, long Offset, LPSAFEARRAY* Res));




  STDMETHOD(MGetTbl(BSTR Name, BSTR GSmpl, long Formatted, LPSAFEARRAY* Value));
  STDMETHOD(get_PDebug(int* Value));
  STDMETHOD(set_PDebug(int Value));
  STDMETHOD(MMatrix(BSTR List, int WithNames, long Offset, long Length, LPSAFEARRAY* Value));




  STDMETHOD(MOffset(BSTR Period, int* Offset));
  STDMETHOD(get_POEMToAnsi(int* Value));
  STDMETHOD(set_POEMToAnsi(int Value));
  STDMETHOD(get_PError(BSTR* Value));
  STDMETHOD(get_PCurrentDir(BSTR* Value));
  STDMETHOD(set_PCurrentDir(BSTR Value));
  STDMETHOD(MUpdateVars(long Offset, LPSAFEARRAY Value, long* Res));
  STDMETHOD(MUpdateObj(long ObjType, LPSAFEARRAY Value, long* Res));
  STDMETHOD(MMatrixPattern(BSTR Pattern, BSTR X, BSTR Y, BSTR Year, LPSAFEARRAY* Value));




//  STDMETHOD(get_MVersion(BSTR* Value));
  STDMETHOD(get_PVersion(BSTR* Value));
};

#endif //CCIodeImplH
