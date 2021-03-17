// CCIODEIMPL : Implementation of TCCIodeImpl (CoClass: CCIode, Interface: ICCIode)

#include <vcl.h>
#include <StrUtils.hpp>
#include "c:\usr\iode_src\api\iode.h"
#include "c:\usr\iode_src\api\iodeapi.h"
extern  char **KT_names;
extern  int  KT_nbnames, KT_mode[MAX_MODE];
extern  void CCIodeDebug(char *cmd, char *arg1);
extern int CCIodeDebugOn;
extern int CCIodeOEMToAnsi;
extern int CCIodeError;
extern AnsiString CCErrorBuf;
#pragma hdrstop

#include "CCIODEIMPL.H"

extern void TfrmStatus(char *buf);
extern void TfrmShow(int show);
int IodeInitDone = 0;

int     ODE_NOEXPANDSTAR;

void IodeImplInit()
{
	if(!IodeInitDone) IodeInit();
	ODE_NOEXPANDSTAR = 1;
    IodeInitDone = 1;
}


/////////////////////////////////////////////////////////////////////////////
// TCCIodeImpl
char* ConvertToAnsi(OLECHAR FAR* szW)
{
  static char achA[10240];

  WideCharToMultiByte(CP_ACP, 0, szW, -1, achA, 10240, NULL, NULL);
  return achA;
}

STDMETHODIMP TCCIodeImpl::MWsLoad(BSTR FileName, int Type, int* Status)
{
    int     rc;
    char    *f = ConvertToAnsi(FileName);

    CCIodeDebug("WsLoad", f);
    IodeImplInit();
    rc = B_WsLoad(f, Type);
//    *Status = KNB(K_WS[Type]);
    if(rc == 0) *Status = KNB(K_WS[Type]);
    else *Status = -1;
    return(S_OK);
}


STDMETHODIMP TCCIodeImpl::MExecute(BSTR Cmd, int* Status)
{
   char    *cmdline = ConvertToAnsi(Cmd);

   CCIodeDebug("Execute", cmdline);
   IodeImplInit();
   *Status = B_ReportLine(cmdline);
   if(CCIodeError) *Status = -1;
   CCIodeError = 0;
   return(S_OK);
}

STDMETHODIMP TCCIodeImpl::MReport(BSTR FileName, int* Status)
{
   char    *f = ConvertToAnsi(FileName);

   CCIodeDebug("Report", f);
   IodeImplInit();
   *Status = B_ReportExec(f);
   if(CCIodeError) *Status = -1;
   CCIodeError = 0;
   return(S_OK);
}

STDMETHODIMP TCCIodeImpl::MGetObject(BSTR Name, int Type, BSTR* Object)
{
	  AnsiString    Abuf;
	  char          *name = ConvertToAnsi(Name),
					*res1, *res2;
	  unsigned char **lst, **vlst;
	  int           objnb, i;
	  KDB           *kdb;

	  CCIodeDebug("GetObject", name);

	  IodeImplInit();
      lst = SCR_vtoms(name, " ;,");
      kdb = K_WS[Type];
      for(i = 0; i < SCR_tbl_size(lst); i++) {
          objnb = K_find(kdb, lst[i]);
          if(objnb < 0) {
            Abuf.sprintf("%s : Object unknown", name);
            *Object = WideString(Abuf.c_str()).Detach();
          }
          else {
            if(Type == K_VAR ) {
                    if(i == 0)  {
                        res1 = IodeDdeCreatePer(0);
                        Abuf.sprintf("\t%s\n", res1);
                        SCR_free(res1);
                    }
                    res2 = IodeDdeCreateSeries(objnb, 0);
                    Abuf = Abuf + "\n" + res2;

                    SCR_free(res2);
            }
            else {
				switch(Type) {
				case K_CMT : res1 = (char *)KCVAL(kdb, objnb); break;
				case K_EQS : res1 = (char *)KELEC(kdb, objnb); break;
                case K_IDT : res1 = (char *)KILEC(kdb, objnb); break;
                case K_LST : res1 = (char *)KLVAL(kdb, objnb);
                    vlst =  (unsigned char **) B_ainit_chk(res1, NULL, 0);
                    res1 = SCR_mtov(vlst, ',');
                    break;
                default    : res1 = SCR_stracpy("Not yet implemented") ; break;
                }
                if(strlen(res1) > 0) {
                    res2 = (char *) SCR_stracpy((unsigned char *)res1);
                    SCR_OemToAnsi((unsigned char *)res2, (unsigned char *)res2);
                    Abuf.sprintf("%s", (char *) res2);
                    SCR_free(res2);
               }
               else Abuf = "";
            }
        }
      }
      SCR_free_tbl(lst);

      *Object = WideString(Abuf.c_str()).Detach();
      return(S_OK);
}


STDMETHODIMP TCCIodeImpl::MGetVar(BSTR Name, int T, VARIANT* Value)
{
      AnsiString    Abuf;
      int           objnb;
      char          *name = ConvertToAnsi(Name);
      Variant        value =SCR_NAN;
      KDB           *kdb;
      SAMPLE        *smpl;
      IodeImplInit();
      kdb = K_WS[K_VAR];

      CCIodeDebug("GetVar", name);
      objnb = K_find(kdb, name);
      if(objnb >= 0)  {
        smpl = (SAMPLE *) KDATA(K_WS[K_VAR]);
        if(T < smpl->s_nb) value = *KVVAL(kdb, objnb, T);
      }

      *Value = (VARIANT) value;
      return(S_OK);
}


STDMETHODIMP TCCIodeImpl::MStatus(int Type, long* Nb, BSTR* Name)
{
    KDB           *kdb;
    char           *name;

    CCIodeDebug("Status", "");
    IodeImplInit();
    kdb = K_WS[Type];
    if(kdb == NULL) {
        *Nb = -1;
        *Name = WideString("Nothing loaded").Detach();
    }
    else {
        *Nb = KNB(kdb);
        //name = KNAME(kdb);
		name = KNAMEPTR(kdb); // JMP 13/8/2015
        *Name = WideString(name).Detach();
    }
    return(S_OK);
}

STDMETHODIMP TCCIodeImpl::MGetMaxt(long* Value)
{
    SAMPLE  *smpl;
    KDB     *kdb;

    *Value = 0;
    IodeImplInit();
    kdb = K_WS[K_VAR];

    if(kdb != NULL) {
        smpl = (SAMPLE *) KDATA(K_WS[K_VAR]);
        *Value = smpl->s_nb;
    }
    return(S_OK);
}


STDMETHODIMP TCCIodeImpl::MGetFreq(long* Value)
{
    SAMPLE  *smpl;
    KDB     *kdb;

    *Value = 0;
    IodeImplInit();
    kdb = K_WS[K_VAR];

    if(kdb != NULL) {
        smpl = (SAMPLE *) KDATA(K_WS[K_VAR]);
        *Value = PER_nb(smpl->s_p1.p_p);
    }
    return(S_OK);

}


STDMETHODIMP TCCIodeImpl::MGetBegin(long* Year, long* Sub, long* Freq)
{
    SAMPLE  *smpl;
    KDB     *kdb;

    *Year = *Sub = *Freq = 0;
    IodeImplInit();
    kdb = K_WS[K_VAR];

    if(kdb != NULL) {
        smpl = (SAMPLE *) KDATA(K_WS[K_VAR]);
        *Year = smpl->s_p1.p_y;
        *Sub = smpl->s_p1.p_s;
        *Freq = PER_nb(smpl->s_p1.p_y);
    }
    return(S_OK);
}


STDMETHODIMP TCCIodeImpl::MGetEnd(long* Year, long* Sub, long* Freq)
{
    SAMPLE  *smpl;
    KDB     *kdb;

    *Year = *Sub = *Freq = 0;
    IodeImplInit();
    kdb = K_WS[K_VAR];

    if(kdb != NULL) {
        smpl = (SAMPLE *) KDATA(K_WS[K_VAR]);
        *Year = smpl->s_p2.p_y;
        *Sub = smpl->s_p2.p_s;
        *Freq = PER_nb(smpl->s_p2.p_y);
    }
    return(S_OK);
}


STDMETHODIMP TCCIodeImpl::MGetBeginSub(long* Value)
{
    SAMPLE  *smpl;
    KDB     *kdb;

    *Value = 0;
    IodeImplInit();
    kdb = K_WS[K_VAR];

    if(kdb != NULL) {
        smpl = (SAMPLE *) KDATA(K_WS[K_VAR]);
        *Value = smpl->s_p1.p_s;
    }
    return(S_OK);
}

STDMETHODIMP TCCIodeImpl::MGetBeginYear(long* Value)
{
    SAMPLE  *smpl;
    KDB     *kdb;

    *Value =  0;
    IodeImplInit();
    kdb = K_WS[K_VAR];

    if(kdb != NULL) {
        smpl = (SAMPLE *) KDATA(K_WS[K_VAR]);
        *Value = smpl->s_p1.p_y;
    }
    return(S_OK);
}


STDMETHODIMP TCCIodeImpl::MVector(BSTR Name, long Offset, long Length, LPSAFEARRAY* Value)




{
    int           i, nobs, objnb, offset, length;
    double        *valueptr;
    char          *name = ConvertToAnsi(Name);
    SAMPLE        *smpl;
    KDB            *kdb;

     IodeImplInit();
     kdb = K_WS[K_VAR];
     if(kdb != NULL) {
        smpl = (SAMPLE *) KDATA(K_WS[K_VAR]);
        nobs = smpl->s_nb;
        offset = Offset;
        length = Length;
        if(offset > nobs) offset = nobs - 1;
        if(offset < 0) offset = nobs + offset;
        if(length == 0) length = nobs;
        if(length + offset > nobs) length = nobs - offset;
     }
     TSafeArrayDim1      dim(length);
     TSafeArrayDouble1   sa(dim);


     objnb = K_find(kdb, name);
     if(objnb < 0)
        for(i=0; i < length; i++)   sa[i] = SCR_NAN;

     else {
        valueptr = KVVAL(kdb, objnb, offset);
        for(i=0; i < length; i++) sa[i] = valueptr[i];
     }

     *Value = sa.Detach();
     return(S_OK);
}


STDMETHODIMP TCCIodeImpl::get_PSample(LPSAFEARRAY* Value)
{
  try
  {
     SAMPLE        *smpl;
     PERIOD        *per;
     KDB            *kdb;
     int            t, nobs = 0;
     char           res[20];

     IodeImplInit();
     kdb = K_WS[K_VAR];
     if(kdb != NULL) {
        smpl = (SAMPLE *) KDATA(K_WS[K_VAR]);
        nobs = smpl->s_nb;
     }

     if(nobs != 0) {
        TSafeArrayDim1      dim(nobs);
        TSafeArrayBSTR1     sa(dim);

        for(t = 0 ; t < nobs ; t++) {
            per = PER_addper(&(KSMPL(kdb)->s_p1), t);
            strcpy(res, PER_pertoa(per, 0L));
			sa[t] = WideString(res).c_bstr();;
        }
        *Value = sa.Detach();
     }
     else {
        TSafeArrayDim1      dim(1);
        TSafeArrayBSTR1     sa(dim);

		sa[0] = WideString("Empty Sample").c_bstr();;
        *Value = sa.Detach();
     }

  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_ICCIode);
  }
  return S_OK;
};



STDMETHODIMP TCCIodeImpl::MContents(int Type, BSTR Pattern, LPSAFEARRAY* List)
{
    int           i, nobj;
    char          *pattern = ConvertToAnsi(Pattern);
    unsigned char **lst = NULL;
    char          *vlst = NULL;
    KDB            *kdb;

    IodeImplInit();
    kdb = K_WS[Type];
    vlst = (unsigned char *) K_expand(Type, NULL, pattern, '*');
    lst =  (unsigned char **) B_ainit_chk(vlst, NULL, 0);

    nobj = SCR_tbl_size(lst);
    TSafeArrayDim1      dim(nobj);
    TSafeArrayBSTR1     sa(dim);

	for(i = 0; i < nobj; i++) sa[i] = WideString((char *) lst[i]).c_bstr();;
    *List = sa.Detach();

    SCR_free(vlst);
    SCR_free_tbl(lst);
    return(S_OK);
}

AnsiString  CellBuf;
double      CellVal;

int IodeCell(TCELL *cell, COL *cl, int Formatted = 0, int ToAnsi = 1)
{
    AnsiString  tmp;

    CellVal = SCR_NAN;
    CellBuf = "";
    if(cell->tc_type != 0) {
        if(cl == NULL || cell->tc_type == KT_STRING) {
            tmp = (char *) COL_text(cl, cell->tc_val, KT_nbnames);
            if(CCIodeOEMToAnsi) CellBuf = (char *) SCR_OemToAnsi(tmp.c_str(), tmp.c_str());
            else       CellBuf = tmp;

            if(Formatted) {
                tmp = "";
                if(cell->tc_attr & KT_BOLD) tmp += "B";
                if(cell->tc_attr & KT_ITALIC) tmp += "I";
                if(cell->tc_attr & KT_UNDERLINE) tmp += "U";


                if(cell->tc_attr & KT_RIGHT) tmp += "R";
                if(cell->tc_attr & KT_CENTER) tmp += "C";

                if(tmp.Length() > 0) CellBuf = tmp + "|" + CellBuf;
            }
        }
        else CellVal = cl->cl_res;
    }
    return(1);
}

STDMETHODIMP TCCIodeImpl::MTest(BSTR Name, int Formatted, int ToAnsi, long Offset,
          LPSAFEARRAY* Res)
{
    char          *name;
    int                     i, j, objnb = 0, nc, nl, d;
    TBL                     *tbl;
    COLS                    *cls;
    TLINE                   *line;
    TCELL                   *cell;

    name =  ConvertToAnsi(Name);
    tbl = IodeGetTblDefinition(name);
    nc = T_prep_cls(tbl, "1990:2", &cls);
    KT_names = T_find_files(cls);
    KT_nbnames = SCR_tbl_size((unsigned char **) KT_names);
    COL_find_mode(cls, KT_mode, 2);
    nc = nc + 1;
    nl = T_NL(tbl);

    TArrayDimT<2>          dim(nl, nc);
    TSafeArrayT<VARIANT, VT_VARIANT,2>         sa(dim);


    for(i = 0; i < nl; i++) {
        line = T_L(tbl) + i;
        switch(line->tl_type) {
            case KT_TITLE:
                cell = (TCELL *) line->tl_val;
                IodeCell(cell, NULL, Formatted);
                sa[i][0] = TVariant(WideString(CellBuf.c_str()));

                break;
            case KT_CELL :
                COL_clear(cls);
                if(COL_exec(tbl, i, cls) >= 0) {
                    for(j = 0; j < cls->cl_nb; j++) {
                        d = j % T_NC(tbl);
                        if(tbl->t_free == 0 && d == 0 && j != 0) continue;
                        cell = (TCELL *) line->tl_val;
                        IodeCell(cell + d, cls->cl_cols + j, Formatted);
                        if(CellVal == SCR_NAN) sa[i][j] = TVariant(WideString(CellBuf.c_str()));
                        else sa[i][j] = TVariant(CellVal);
                    }
                }
                break;
        }
    }
    *Res = sa.Detach();

    return(S_OK);
}


STDMETHODIMP TCCIodeImpl::MGetTbl(BSTR Name, BSTR GSmpl, long Formatted, LPSAFEARRAY* Value)




{
    char          *name, *gsmpl, date[20];
    extern char *KLG_OPERS_TEXTS[][3];    
    int           i, j, k, objnb = 0, nc, nl, d;
    TBL           *tbl;
    COLS          *cls;
    TLINE         *line;
    TCELL         *cell;

    name =  ConvertToAnsi(Name);
    CCIodeDebug("GetTbl", name);

    tbl = IodeGetTblDefinition(name);
    gsmpl = ConvertToAnsi(GSmpl);
    nc = T_prep_cls(tbl, gsmpl, &cls);
    KT_names = T_find_files(cls);
    KT_nbnames = SCR_tbl_size((unsigned char **) KT_names);
    COL_find_mode(cls, KT_mode, 2);
//  nc = nc + 1;
    nl = T_NL(tbl);

    TArrayDimT<2>          dim(nl, nc);
    TSafeArrayT<VARIANT, VT_VARIANT,2>         sa(dim);


    for(i = 0; i < nl; i++) {
        line = T_L(tbl) + i;
        switch(line->tl_type) {
            case KT_DATE  :
                CellBuf = SCR_long_to_fdate(SCR_current_date(), date, "dd/mm/yy");
                sa[i][0] = TVariant(WideString(CellBuf.c_str()));
                break;
            case KT_MODE  :
                CellBuf = "";
                for(j = 0; j < MAX_MODE; j++) {
                    if(KT_mode[j] == 0) continue;
                    if(j > 0) CellBuf += "\n";
                    sprintf(date, "(%s) ", COL_OPERS[j + 1]);
                    CellBuf += date;
                    CellBuf += KLG_OPERS_TEXTS[j + 1][B_LANG];
                }                    
                sa[i][0] = TVariant(WideString(CellBuf.c_str()));
                break;
            case KT_FILES :
                CellBuf = "";
                for(j = 0; KT_names[j]; j++) {
                    if(j > 0) CellBuf += "\n";
                    CellBuf += KT_names[j];
                }
                sa[i][0] = TVariant(WideString(CellBuf.c_str()));
                break;
            case KT_TITLE:
                cell = (TCELL *) line->tl_val;
                IodeCell(cell, NULL, Formatted);
                sa[i][0] = TVariant(WideString(CellBuf.c_str()));

                break;
            case KT_CELL :
                COL_clear(cls);
                if(COL_exec(tbl, i, cls) >= 0) {
                    for(j = 0, k = 0; j < cls->cl_nb; j++) {
                        d = j % T_NC(tbl);
                        if(tbl->t_free == 0 && d == 0 && j != 0) continue;
                        cell = (TCELL *) line->tl_val;
                        IodeCell(cell + d, cls->cl_cols + j, Formatted);
                        if(CellVal == SCR_NAN) sa[i][k] = TVariant(WideString(CellBuf.c_str()));
                        else sa[i][k] = TVariant(CellVal);
                        k++;
                    }
                }
                break;
        }
    }
    *Value = sa.Detach();

    return(S_OK);
}


STDMETHODIMP TCCIodeImpl::get_PDebug(int* Value)
{
  try
  {
      *Value = CCIodeDebugOn;
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_ICCIode);
  }
  return S_OK;
};


STDMETHODIMP TCCIodeImpl::set_PDebug(int Value)
{
  try
  {
      if(Value) {
        CCIodeDebug("ON", "");
//        TfrmShow(1);
      }
      else {
        CCIodeDebug("OFF", "");
//        TfrmShow(0);
      }
      CCIodeDebugOn = Value;
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_ICCIode);
  }
  return S_OK;
};



STDMETHODIMP TCCIodeImpl::MMatrix(BSTR List, int WithNames, long Offset, long Length,
          LPSAFEARRAY* Value)
{
// WithNames 0 = No names,  1 = Names, 2 = Names + Cmt

    int     i, j, nobj, nobs, varnb, offset, length;
    double  *valptr;
    char    *list = ConvertToAnsi(List);
    KDB     *kdb;
    SAMPLE  *smpl;
    AnsiString  tmp;

    unsigned char **lst;
    char          *vlst;

    CCIodeDebug("GetMatrix", list);
    IodeImplInit();
    kdb = K_WS[K_VAR];
    if(kdb != NULL) {
        smpl = (SAMPLE *) KDATA(kdb);
        nobs = smpl->s_nb;
        offset = Offset;
        length = Length;
        if(offset > nobs) offset = nobs - 1;
        if(offset < 0) offset = nobs + offset;
        if(length == 0) length = nobs;
        if(length + offset > nobs) length = nobs - offset;
    }

    vlst = (unsigned char *) K_expand(K_VAR, NULL, list, '*');
    lst =  (unsigned char **) B_ainit_chk(vlst, NULL, 0);
    nobj = SCR_tbl_size(lst);

    TArrayDimT<2>                       dim(nobj, length + WithNames);
    TSafeArrayT<VARIANT, VT_VARIANT,2>  sa(dim);

    nobj = SCR_tbl_size(lst);
    for(i = 0; i < nobj; i++) {
        varnb = K_find(kdb, lst[i]);
        if(WithNames) {
            sa[i][0] = TVariant(WideString((char *) lst[i]));
            if(WithNames == 2) {
                tmp = (char *) IodeGetCmt(lst[i]);
                if(CCIodeOEMToAnsi) tmp = (char *) SCR_OemToAnsi(tmp.c_str(), tmp.c_str());
                sa[i][1] = TVariant(WideString((char *) tmp.c_str()));
            }
        }
        if(varnb < 0)
            for(j = 0; j < length; j++) sa[i][j+WithNames] = TVariant(SCR_NAN);
        else {
            valptr = KVVAL(kdb, varnb, offset) ;
            for(j = 0; j < length; j++) sa[i][j+WithNames] = TVariant(valptr[j]);
        }
    }

    *Value = sa.Detach();

    SCR_free(vlst);
    SCR_free_tbl(lst);
    return(S_OK);
}


STDMETHODIMP TCCIodeImpl::MOffset(BSTR Period, int* Offset)
{
    char    *period;
    SAMPLE  *smpl;
    PERIOD  *pfrom;
    KDB     *kdb;

    period = ConvertToAnsi(Period);
    *Offset = 0;
    IodeImplInit();
    kdb = K_WS[K_VAR];

    if(kdb != NULL) {
        pfrom = PER_atoper(period);
        smpl = (SAMPLE *) KDATA(kdb);
        *Offset = PER_diff_per(pfrom, &(smpl->s_p1));
        SCR_free(pfrom);
    }
    return(S_OK);
}


STDMETHODIMP TCCIodeImpl::get_POEMToAnsi(int* Value)
{
  try
  {
    *Value = CCIodeOEMToAnsi;
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_ICCIode);
  }
  return S_OK;
};


STDMETHODIMP TCCIodeImpl::set_POEMToAnsi(int Value)
{
  try
  {
       CCIodeOEMToAnsi = Value;
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_ICCIode);
  }
  return S_OK;
};



STDMETHODIMP TCCIodeImpl::get_PError(BSTR* Value)
{
  try
  {
     CCErrorBuf = AnsiReplaceStr(CCErrorBuf, "\n", "\r\n");
      *Value = WideString(CCErrorBuf.c_str()).Detach();
      CCErrorBuf = "";
      CCIodeError = 0;
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_ICCIode);
  }
  return S_OK;
};



STDMETHODIMP TCCIodeImpl::get_PCurrentDir(BSTR* Value)
{
  try
  {
	  *Value = WideString(GetCurrentDir().c_str()).Detach();
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_ICCIode);
  }
  return S_OK;
};


STDMETHODIMP TCCIodeImpl::set_PCurrentDir(BSTR Value)
{
        char    *f;
  try
  {
    f = ConvertToAnsi(Value);
    CCIodeDebug("CurrentDir", f);
    ChDir(f);
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_ICCIode);
  }
  return S_OK;
};



STDMETHODIMP TCCIodeImpl::MUpdateVars(long Offset, LPSAFEARRAY Value, long* Res)
{
	int     i, j, nobj, nobs, varnb, offset, length;
	double  *valptr, val;
	KDB     *kdb;
	SAMPLE  *smpl;
	AnsiString  name, list;
	TVariant *v;

//	unsigned char **lst;
//	char          *vlst;

	CCIodeDebug("UpdateVars", "..");
	IodeImplInit();
	nobj = Value->rgsabound[1].cElements;
	length = Value->rgsabound[0].cElements;

	kdb = K_WS[K_VAR];
	if(kdb != NULL) {
		smpl = (SAMPLE *) KDATA(kdb);
		nobs = smpl->s_nb;
		offset = Offset;
		if(offset + length > nobs + 1) {
			*Res = -1;
			return(S_OK);
		}
	}

	list = "";
	v = (TVariant *) Value->pvData;
	for (i = 0; i < nobj; i++) {
		name = (AnsiString) v[i].operator AnsiString();
		list = list + name + ";";
//		tmp = (AnsiString) (v[i], wcslen(v[i]) );
		varnb = K_find(kdb, (char *) name.c_str());
		if (varnb < 0) B_DataCreate((char *) name.c_str(), K_VAR);
		varnb = K_find(kdb, (char *) name.c_str());
		if(varnb < 0) {
			*Res = -1;
			return(S_OK);
		}
		valptr = KVVAL(kdb, varnb, offset);
		for(j = 1; j < length; j++) {
			valptr[j - 1] = (double) v[i+j*nobj];
	   }
	}
	*Res = 0;
	CCIodeDebug("UpdateVars", list.c_str());
	return(S_OK);
}


STDMETHODIMP TCCIodeImpl::MUpdateObj(long ObjType, LPSAFEARRAY Value, long* Res)
{
	int         i, j, nobj, nobs, objnb, length, objtype = ObjType;
	AnsiString  name, value, line;
	TVariant    *v;

	unsigned char **lst;
	char          *vlst;

	CCIodeDebug("UpdateObj", "");
	IodeImplInit();
	nobj = Value->rgsabound[1].cElements;
	length = Value->rgsabound[0].cElements;

	if(objtype != K_CMT && objtype != K_LST) {
		*Res = -1;
		return(S_OK);
	}
	v = (TVariant *) Value->pvData;
	for (i = 0; i < nobj; i++) {
		//name = (AnsiString) (v[i], wcslen(v[i])) ;
		name = (AnsiString) v[i].operator AnsiString();
		//value = (AnsiString) (v[i+nobj], wcslen(v[i+nobj]));
		value = (AnsiString) v[i+nobj].operator AnsiString();
		line = name + " " + value;

		if(B_DataUpdate(line.c_str(), objtype) < 0) {
			*Res = -1;
			return(S_OK);
		}
	}
	*Res = 0;
	return(S_OK);

}

STDMETHODIMP TCCIodeImpl::MMatrixPattern(BSTR Pattern, BSTR X, BSTR Y, BSTR Year,
          LPSAFEARRAY* Value)
{
    int     i, j, nrows, ncols, ct = 0, varnb;
    double  *valptr;
    char    *pattern, *x, *y, *year;
    char    cvar[K_MAX_NAME + 1], rvar[K_MAX_NAME + 1];
    unsigned char    **xvars, **yvars;
    KDB     *kdb;
    SAMPLE  *smpl;

    pattern = SCR_stracpy(ConvertToAnsi(Pattern));
    x = SCR_stracpy(ConvertToAnsi(X));
    y = SCR_stracpy(ConvertToAnsi(Y));
    year = SCR_stracpy(ConvertToAnsi(Year));

    CCIodeDebug("GetMatrixPattern", pattern);
    IodeImplInit();
    kdb = K_WS[K_VAR];
    if(kdb != NULL) {
        smpl = (SAMPLE *) KDATA(kdb);
        ct = PER_diff_per(PER_atoper(year), &(smpl->s_p1));
        if(ct > smpl->s_nb) ct = -1;
    }

	xvars = (unsigned char **) B_ainit_chk(x, NULL, 0);
	nrows = SCR_tbl_size(xvars);

	yvars = (unsigned char **) B_ainit_chk(y, NULL, 0);
	ncols = SCR_tbl_size(yvars);

    TArrayDimT<2>                       dim(nrows, ncols);
    TSafeArrayT<VARIANT, VT_VARIANT,2>  sa(dim);

    for(i = 0; i < nrows; i++) {
	    strcpy(rvar, pattern);
	    SCR_replace(rvar, "x", xvars[i]);

	    for(j = 0; j < ncols; j++) {
    		strcpy(cvar, rvar);
	    	SCR_replace(cvar, "y", yvars[j]);

            varnb = K_find(kdb, cvar);
            if(varnb < 0 || ct < 0) sa[i][j] = TVariant(SCR_NAN);
            else {
                valptr = KVVAL(kdb, varnb, 0) ;
                sa[i][j] = TVariant(valptr[ct]);
            }
		}
	}

	*Value = sa.Detach();

	SCR_free_tbl(xvars);
	SCR_free_tbl(yvars);
	SCR_free(pattern);
	SCR_free(x);
	SCR_free(y);
	SCR_free(year);
	return(S_OK);
}


/*
wchar_t *AnsiTowchar_t(AnsiString Str)
{
  wchar_t *str = new wchar_t[Str.WideCharBufSize()];
  return Str.WideChar(str, Str.WideCharBufSize());
  delete str;
}
*/
/*
STDMETHODIMP TCCIodeImpl::get_MVersion(BSTR* Value)
{
  try
  {
	  *Value = WideString(IODE_VERSION).Detach();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ICCIode);
  }
  return S_OK;
};
*/

STDMETHODIMP TCCIodeImpl::get_PVersion(BSTR* Value)
{
  try
  {
	  *Value = WideString(IODE_VERSION).Detach();
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_ICCIode);
  }
  return S_OK;
};



