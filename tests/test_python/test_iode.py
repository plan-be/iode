# IODE EXTENSION FOR PYTHON
# =========================
#
#    @header4iode
#
# Unit tests for iode.pyd

import iode
import pytest
import larray as la
import numpy as np
import pandas as pd
import logging

from pathlib import Path

# GLOBALS
IODE_DATA_DIR = Path("../data").absolute()
IODE_OUTPUT_DIR = Path("../output").absolute()
IODE_VERBOSE = 1

nanobind = False
try:
    from iode import TEST_DATA_DIR
    IODE_DATA_DIR = TEST_DATA_DIR
    nanobind = True
except:
    pass


# MISC FUNCTIONS
# --------------

def test_iode_version():
    result = iode.version()
    assert result == "IODE Modeling Software 7.0.0 - (c) 1990-2023 Federal Planning Bureau - Brussels"

# WS FUNCTIONS
# ------------
def test_iode_ws_content():
    iode.ws_load_cmt(str(IODE_DATA_DIR / "fun.cmt"))
    result = iode.ws_content_cmt("ACA*")
    assert result == ["ACAF", "ACAG"]

def test_iode_ws_clear_var():
    varfile = str(IODE_DATA_DIR / "a.var")
    nbvars = iode.ws_load_var(varfile)
    print(f"{nbvars} variables have been read")

    iode.ws_clear_var()
    result = iode.ws_content_var("*")
    assert isinstance(result, list) and len(result) == 0 

def test_iode_ws_load_var():
    # Loading var varfile in IODE memory (KV_WS): iode.ws_load_var(filename)
    varfile = str(IODE_DATA_DIR / "a.var")
    nbvars = iode.ws_load_var(varfile)
    assert nbvars == 433

def test_iode_ws_load_all():
    # Comments
    nbvars = iode.ws_load(str(IODE_DATA_DIR / "fun"), 0)
    assert nbvars == 317
    # Equations
    nbvars = iode.ws_load(str(IODE_DATA_DIR / "fun"), 1)
    assert nbvars == 274
    # Identities
    nbvars = iode.ws_load(str(IODE_DATA_DIR / "fun"), 2)
    assert nbvars == 48
    # Lists
    nbvars = iode.ws_load(str(IODE_DATA_DIR / "fun"), 3)
    assert nbvars == 17
    # Scalars
    nbvars = iode.ws_load(str(IODE_DATA_DIR / "fun"), 4)
    assert nbvars == 161
    # Tables
    nbvars = iode.ws_load(str(IODE_DATA_DIR / "fun"), 5)
    assert nbvars == 46
    # Variables
    nbvars = iode.ws_load(str(IODE_DATA_DIR / "fun"), 6)
    assert nbvars == 394

def test_iode_ws_sample():

    varfile = str(IODE_DATA_DIR / "a.var")
    iode.ws_load_var(varfile)
    sample = iode.ws_sample_get()
    assert sample == ('1990Y1', '2020Y1')
    print(f"Sample of {varfile} = {sample}")

    sample = iode.ws_sample_set("1995Y1", "")
    assert sample ==  ('1995Y1', '2020Y1')

    sample = iode.ws_sample_set("", "2030Y1")
    assert sample ==  ('1995Y1', '2030Y1')

    sample = iode.ws_sample_set("2000Y1", "2002Y1")
    assert sample ==  ('2000Y1', '2002Y1')

    nobs = iode.ws_sample_nb_periods()
    assert nobs == 3
    print(f"Nobs = {nobs}")

    str1 = iode.ws_sample_to_string()
    assert str1 == "2000Y1 2002Y1"

    lst = iode.ws_sample_to_list()
    assert lst == ["2000Y1", "2001Y1", "2002Y1"]

    ax2 = iode.ws_sample_to_larray_axis()
    assert repr(ax2) == repr(la.Axis(["2000Y1", "2001Y1", "2002Y1"], 'time'))


# PYIODE_OBJECTS
# --------------


# IODE COMMENTS <-> PYTHON STRINGS
# --------------------------------

def test_iode_get_cmt():

    iode.ws_load_cmt(str(IODE_DATA_DIR / "fun.cmt"))
    c_ACAF = iode.get_cmt("ACAF")
    c_EFM = iode.get_cmt("EFM")
    c_XPWMAB = iode.get_cmt("XPWMAB")

    print(f"ACAF   = '{c_ACAF}'")
    print(f"EFM    = '{c_EFM}'")
    print(f"XPWMAB = '{c_XPWMAB}'")

    assert c_ACAF == "Ondernemingen: ontvangen kapitaaloverdrachten."
    assert c_EFM == "Vreemde grensarbeiders in België (30 juni)."
    assert c_XPWMAB == "Croissance des prix des biens importés"


def test_iode_set_cmt():

    # Clear CMT before creating new comments
    iode.ws_clear_cmt()

    # Create comment as py strings
    pycmt1 = "New comment with éîâëéàçèï"
    pycmt2 = "Other comment"

    # Save them in KC_WS + check
    rc = iode.set_cmt("MYCMT1", pycmt1)
    rc = iode.set_cmt("MYCMT2", pycmt2)

    ## Reload to check values
    c_MYCMT1 = iode.get_cmt("MYCMT1")
    c_MYCMT2 = iode.get_cmt("MYCMT2")

    print(f"MYCMT1 = '{c_MYCMT1}'")
    print(f"orig   = '{pycmt1}'")
    print(f"MYCMT2 = '{c_MYCMT2}'")
    print(f"orig   = '{pycmt2}'")

    assert c_MYCMT1 == pycmt1
    assert c_MYCMT2 == pycmt2


# IODE EQUATIONS <-> PYTHON STRINGS
# ---------------------------------

def test_iode_get_eqs_lec():

    iode.ws_load_eqs(str(IODE_DATA_DIR / "fun.eqs"))
    lec_BVY = iode.get_eqs_lec("BVY")
    print(f"lec_BVY = '{lec_BVY}'")
    assert lec_BVY == "BVY:=YN+YK"


def test_iode_get_eqs():

    iode.ws_load_eqs(str(IODE_DATA_DIR / "fun.eqs"))
    e_BVY = iode.get_eqs("BVY")
    print(e_BVY)
    assert e_BVY.lec == "BVY:=YN+YK"

    e_ACAF = iode.get_eqs("ACAF")
    print(f"txt:  e_ACAF = '{e_ACAF}'")
    print(f"repr: e_ACAF = '{repr(e_ACAF)}'")
    assert round(e_ACAF.e_r2, 5) == 0.82176


def test_iode_set_eqs():

    # Clear EQS WS before creating new object
    iode.ws_clear_eqs()

    # Create eq as py string
    py_A = "grt A := grt B"

    # Save py_GA as GA in KE_WS + check
    iode.set_eqs("A", py_A)
    iode_A  = iode.get_eqs_lec("A")
    assert iode_A == py_A

    # Test error
    py_A = "(grt A"
    with pytest.raises(RuntimeError):
        iode.set_eqs("A", py_A)


# IODE IDENTITIES <-> PYTHON STRINGS
# ----------------------------------

def test_iode_get_idt():

    iode.ws_load_idt(str(IODE_DATA_DIR / "fun.idt"))
    i_XEX = iode.get_idt("XEX")
    assert i_XEX == "grt EX"


def test_iode_set_idt():

    # Clear IDT before creating new object
    iode.ws_clear_idt()

    # Create comment as py strings
    pyxex = "1 * (grt EX)"

    # Save pyxex as XEX in KI_WS + check
    iode.set_idt("XEX", pyxex)
    i_XEX = iode.get_idt("XEX")
    assert i_XEX == pyxex

    # Test error
    pyxex = "1 * (grt EX) - "
    with pytest.raises(RuntimeError):
        iode.set_idt("XEX", pyxex)
    

# IODE LISTS <-> PYTHON STRINGS
# -----------------------------

def test_iode_get_lst():

    iode.ws_load_lst(str(IODE_DATA_DIR / "fun"))
    i_MYLIST = iode.get_lst("MYLIST")
    assert i_MYLIST == "A02_075_H_BRU A02_100_H_VLA"


def test_iode_set_lst():

    # Clear LST before creating new object
    iode.ws_clear_lst()

    # Create list as py strings
    py_list1 = "A;B;C"
    name = "LIST1"

    # Save py_list1 as LIST1 in KL_WS + check
    iode.set_lst(name, py_list1)
    i_LIST1  = iode.get_lst(name)
    assert i_LIST1 == py_list1


# IODE SCALARS <-> PYTHON
# -----------------------

def test_iode_get_scl():

    iode.ws_load_scl(str(IODE_DATA_DIR / "fun"))
    name = "acaf1"
    i_acaf1 = iode.get_scl(name)
    assert round(i_acaf1.value, 7) == 0.0157684
    assert round(i_acaf1.relax, 7) == 1.0
    assert round(i_acaf1.std, 7) == 0.0013687


def test_iode_set_scl():

    # Clear SCL before creating new object
    iode.ws_clear_scl()

    # Create SCl
    name = "myscl"
    py_myscl = iode.Scalar(1.2345, 1.0, 0.0)

    # Save py_scl as myscl in KS_WS + reread and check
    iode.set_scl(name, py_myscl)
    i_myscl = iode.get_scl(name)
    assert i_myscl == py_myscl


# VARIABLES IODE <-> PYTHON LISTS AND NDARRAYS
# --------------------------------------------

def test_iode_get_var_as_ndarray():
    '''
    Getting and setting vectors of doubles from KV_WS
    x points to KV_WS["A"] because get_var_as_ndarray(..., 1)

    KV_WS["A"] is only modified via x[2] = ...
    '''

    varfile = str(IODE_DATA_DIR / "a.var")
    nbvars = iode.ws_load_var(varfile)

    name = "A"

    # x points to KV_WS["A"] because get_var_as_ndarray(..., 0)
    x = iode.get_var_as_ndarray(name, 0)
    x[2] = 22222 # Modifies the variable A in KV_WS
    A = iode.get_var("A")
    assert A[2] == 22222.0

    # y is a deep copy of KV_WS["A"] because get_var_as_ndarray(..., 0) => memory leak ?
    y = iode.get_var_as_ndarray(name, 1)
    y[2] = 33333 # Does not modify the variable A in KV_WS
    A = iode.get_var("A")
    assert A[2] == 22222.0

    # XYY does not exist in KV_WS => new allocation ? 
    # TODO: check this + does XYY exist in KV_WS ?
    z = iode.get_var_as_ndarray("XYY", 1)
    XYY = iode.get_var_as_ndarray("XYY", 1)
    print("XYY : ", XYY)

    # Saving a copy of the modified KV_WS
    iode.ws_save_var(str(IODE_OUTPUT_DIR / "a_mod.var"))


def test_iode_get_var():

    varfile = str(IODE_DATA_DIR / "a.var")
    iode.ws_load_var(varfile)
    iode.ws_sample_set("1990Y1", "1992Y1")

    A = iode.get_var("A")
    B = [0.0, 1.0, 2]
    assert A == B


def test_iode_set_var():

    nbvars = iode.ws_load_var(str(IODE_DATA_DIR / "a.var"))

    AA = [11, 22, 33]
    AB = np.array([0.0, 111.0, 112.5], np.double)

    iode.set_var("AA", AA)
    iode.set_var("AB", AB)
    iode.ws_save_var(str(IODE_OUTPUT_DIR / "a_mod2.var"))

    iode.ws_load_var(str(IODE_OUTPUT_DIR / "a_mod2.var"))
    new_AA = iode.get_var("AA")
    new_AB = iode.get_var("AB")

    assert new_AA[2] == AA[2] # 1993Y1
    assert new_AB[1] == AB[1] # 1992Y1


def test_iode_exec_lec():
    nbvars = iode.ws_load_var(str(IODE_DATA_DIR / "a.var"))

    v_A = iode.get_var("A")
    vec = iode.exec_lec("1+A-1")
    assert vec == v_A

    res = iode.exec_lec("A+1", 0)
    assert  res == v_A[0] + 1


# REPORT DATA_* FUNCTIONS
# -----------------------

# DATA_DELETE
# -----------


def test_iode_delete_objects():    
    # Load IDT ws and delete group of idts based on name pattern
    iode.ws_load_idt(str(IODE_DATA_DIR / "fun.idt"))
    nbobjs = len(iode.ws_content_idt("*"))

    nbobjs_pattern = len(iode.ws_content_idt("KLL"))
    with pytest.raises(RuntimeError):
        iode.delete_objects("KLL", 2)
    nbobjs_after_delete = len(iode.ws_content_idt("*"))
    assert nbobjs_after_delete == nbobjs - nbobjs_pattern
    
    iode.ws_load_idt(str(IODE_DATA_DIR / "fun.idt"))
    nbobjs_pattern = len(iode.ws_content_idt("X*"))
    iode.delete_objects("X*", 2)
    nbobjs_after_delete = len(iode.ws_content_idt("*"))
    assert nbobjs_after_delete == nbobjs - nbobjs_pattern


# DATA_UPDATE
# -----------


def test_iode_data_update_cmt():

    A = "Comment of A"
    iode.data_update_cmt("A", A)
    new_A = iode.get_cmt("A")
    assert new_A == A


def test_iode_data_update_eqs():

    # Make a LEC error and check return code
    A = "A := ln t +++ "
    with pytest.raises(RuntimeError):
        iode.data_update_eqs("A", A)

    # Save a new equation A and check the return code
    A = "A := ln t + 1"
    iode.data_update_eqs("A", A)

    # Get the lec value and compare with A
    new_A = iode.get_eqs_lec("A")
    assert new_A == A


def test_iode_data_update_var():

    varfile = str(IODE_DATA_DIR / "a.var")
    nbvars = iode.ws_load_var(varfile)

    A = iode.get_var("A")
    A[2] = 123
    B = [100, 101, 102] # 1992Y1 1993Y1 ...
    iode.data_update_var("A", A)
    iode.data_update_var("B", B, operation="L", per_from="1992Y1")  

    new_A = iode.get_var("A")
    new_B = iode.get_var("B")
    assert new_A[2] == A[2] # 1992Y1
    assert new_B[3] == B[1] # 1993Y1
    assert new_B[4] == B[2] # 1994Y1


# Miscellaneous functions
# -----------------------

# def test_iode_ws_load_varpy():
# 
#     print('iode.ws_load_varpy("split.var")')
#     print(iode.ws_load_varpy("split.var"))
# 
#     print('iode.ws_load_varpy("split.var", axis_names=["a", "b"], regex=r"(\w{2})(\w{2})")')
#     print(iode.ws_load_varpy("split.var", axis_names=["a", "b"], regex=r"(\w{2})(\w{2})"))
# 
#     print('print(iode.ws_load_varpy("split.var", axis_names="a,b", regex=r"(\w{2})(\w{2})"))')
#     print(iode.ws_load_varpy("split.var", axis_names="a,b".split(","), regex=r"(\w{2})(\w{2})"))
# 
#     print('iode.ws_load_varpy("split.var", varsaxis="a_b", axis_names=["a", "b"], regex=r"(\w{2})(\w{2})")')
#     print(iode.ws_load_varpy("split.var", varsaxis="a_b", axis_names=["a", "b"], regex=r"(\w{2})(\w{2})"))


# ESTIMATION
# ----------


def test_iode_eqs_estimation():
    iode.ws_load_eqs(str(IODE_DATA_DIR / "fun.eqs"))
    iode.ws_load_var(str(IODE_DATA_DIR / "fun.var"))
    iode.ws_load_scl(str(IODE_DATA_DIR / "fun.scl"))

    name = "ACAF"
    iode.eqs_estimate(name, "1980Y1", "1996Y1")

    # Check acaf1 value after estimation
    name = "acaf1"
    i_acaf1 = iode.get_scl(name)
    assert round(i_acaf1.value, 8) == 0.0157705

    # Check _YCALC[1980Y1]
    res = iode.exec_lec("_YRES0[1980Y1]", 0)
    assert round(res, 8) == -0.00115008


# SIMULATION
# ----------


def test_iode_model_simulate():
    iode.ws_load_eqs(str(IODE_DATA_DIR / "fun.eqs"))
    iode.ws_load_var(str(IODE_DATA_DIR / "fun.var"))
    iode.ws_load_scl(str(IODE_DATA_DIR / "fun.scl"))

    # Test non convergence
    iode.suppress_msgs()
    with pytest.raises(RuntimeError):
        iode.model_simulate("2000Y1", "2002Y1", "", eps=0.0001, relax=0.7, maxit=2)

    # Test convergence
    iode.model_simulate("2000Y1", "2002Y1", "", relax=0.7)
   

    # Check some result after simulation (values obatined with the BORLAND compiler)
    # ACAF [2000..2002] before simulation = [10.0466107922005, 2.86792273645546, -0.929212509051645]
    # ACAF [2000..2002] after simulation  = [10.0466107922005, 2.62379276852768, -1.27462319299379]
    res = iode.exec_lec("ACAF[2002Y1]", 0)
    assert round(res, 12) == -1.274623192994

    iode.reset_msgs()


def test_iode_model_simulate_exchange():

    iode.suppress_msgs()
    iode.ws_load_eqs(str(IODE_DATA_DIR / "fun.eqs"))
    iode.ws_load_var(str(IODE_DATA_DIR / "fun.var"))
    iode.ws_load_scl(str(IODE_DATA_DIR / "fun.scl"))

    # Version with exchange in at least 2 equations
    # Set values of endo UY
    UY = iode.get_var("UY")
    UY[40:43] = [650.0, 670.0, 680.0] # 2000Y1..2002Y1
    iode.set_var("UY", UY)

    # Simulate with exchange UY - XNATY
    iode.model_simulate("2000Y1", "2002Y1", endo_exo_list="UY-XNATY", relax=0.7)

    # Check result
    UY = iode.get_var("UY")
    XNATY = iode.get_var("XNATY")
    assert iode.exec_lec("UY[2000Y1]")[0] == 650.0
    assert round(iode.exec_lec("XNATY[2000Y1]")[0], 7) == 0.8006734

    iode.reset_msgs()


def test_iode_wrt():

    def iode_wrt_1(filename:str="", dest:int=iode.W_A2M):
        iode.w_dest(filename, dest)
        
        iode.w_print(".sep |\n")
        iode.w_print(".esc ~\n")
        
        iode.w_print_pg_header("IODE")
        iode.w_print_pg_footer("- page %d - ")
        
        iode.w_print_tit(1, "Mon titre")
        
        iode.w_print_enum(1)
        iode.w_print("This is an enum paragraph\n\n")
        iode.w_print("This is a normal paragraph\n\n")
        
        iode.w_print(".tb\n")
        iode.w_print(".tborder 1\n")
        iode.w_print(".theader\n")
        iode.w_print("| Syntax | Description\n")
        iode.w_print(".tbody\n")
        iode.w_print("| ~cint L_errno       | Last error number during LEC compilation\n")
        iode.w_print("| ~cchar* L_error()   | Returns a static buffer containing the last LEC compilation error message.\n")
        iode.w_print(".te\n")
            
        iode.w_print(".gb 16.00 10.00\n")
        iode.w_print(".ggrid TMM\n")
        iode.w_print(".galign L\n")
        iode.w_print(".gbox 0 b 0\n")
        iode.w_print(".gtitle Equation ACAF : observed and fitted values\n")
        iode.w_print(".gty L \"(ACAF/VAF[-1])  : observed\" 1980Y1  0.011412042  0.016028202  0.011903394  0.012051363  0.010215556  0.010582964  0.0090882893  0.009277778  0.0082268494  0.0051589358  0.0066405193  0.0068489061  0.0075258742  0.0082727193  0.0019340143  -0.0029850522  0.0069569681 \n")
        iode.w_print(".gty L \"(ACAF/VAF[-1])  : fitted\" 1980Y1  0.012562124  0.012491075  0.012526504  0.011687035  0.011060337  0.010403968  0.0095320575  0.0090522427  0.008714914  0.0076539375  0.0065561227  0.006355248  0.0064942167  0.0062763884  0.0062678674  -0.0029850522  0.0044903364 \n")
        iode.w_print(".ge\n")
        
        iode.w_print_tit(1, "Mon titre 1")
        iode.w_print_par(1, "Mon para level 1 (éàç)")
        iode.w_print(f"Second para 1\n\n")
        iode.w_print_par(2, "Mon para level 2 (âêî)\n")
        iode.w_print(f"Second para level 2\n")
        iode.w_print_tit(2, "Title 2")
        iode.w_print_enum(2, "Mon enum level 2 (âêî)")
        iode.w_print_enum(2, "enum2 suite")
        iode.w_print_enum(2, "enum2 suite")
        
        iode.w_close()

    iode_wrt_1("", iode.W_DUMMY)
    iode_wrt_1(str(IODE_OUTPUT_DIR / "test_wrt.htm"), iode.W_HTML)
    iode_wrt_1(str(IODE_OUTPUT_DIR / "test_wrt.a2m"), iode.W_A2M)
    iode_wrt_1(str(IODE_OUTPUT_DIR / "test_wrt.rtf"), iode.W_RTF)
    iode_wrt_1(str(IODE_OUTPUT_DIR / "test_wrt.csv"), iode.W_CSV)


def test_iode_htol():
    #Read quaterly data and convert it to the current WS sample (yearly)
   
    # define a yearly sample
    iode.ws_clear_var()
    iode.ws_sample_set("2000Y1", "2020Y1")
    
    # input filename
    filename = str(IODE_DATA_DIR / "fun_q.var")
    
    # Last Obs in year
    varname = "ACAF"
    iode.ws_htol_last(filename, varname)
    res = iode.exec_lec(f"{varname}[2014Y1]", 0)
    assert round(res, 6) == -83.340625

    # mean of year
    varname = "ACAG"
    iode.ws_htol_mean(filename, varname)
    res = iode.exec_lec(f"{varname}[2014Y1]", 0)
    assert round(res, 7) == 8.1050747

    # sum
    varname = "AOUC"
    iode.ws_htol_sum(filename, varname)
    res = iode.exec_lec(f"{varname}[2014Y1]", 0)
    assert round(res, 6) == 1.423714


def test_iode_ltoh():
    #Read yearly data data and convert it to the current WS sample (quaterly)
   
    # define a yearly sample
    iode.ws_clear_var()
    iode.ws_sample_set("2010Q1", "2020Q4")

    # input filename
    filename = str(IODE_DATA_DIR / "fun.var")
    
    # Linear interpolation / stock
    varname = "ACAF"
    iode.ws_ltoh_stock(filename, varname, iode.LTOH_LIN)
    res = iode.exec_lec(f"{varname}[2014Q3]", 0)
    assert round(res, 6) == -79.729132
    
    # Linear interpolation / flow
    varname = "ACAG"
    iode.ws_ltoh_flow(filename, varname, iode.LTOH_LIN)
    res = iode.exec_lec(f"{varname}[2014Q3]", 0)
    assert round(res, 6) == 8.105075

    # Cubic splines / stock
    varname = "ACAF"
    iode.ws_ltoh_stock(filename, varname, iode.LTOH_CS)
    res = iode.exec_lec(f"{varname}[2012Q3]", 0)
    assert round(res, 6) == -52.805666

    # Cubic splines / flow
    varname = "ACAG"
    iode.ws_ltoh_flow(filename, varname, iode.LTOH_CS)
    res = iode.exec_lec(f"{varname}[2012Q3]", 0)
    assert round(res, 7) == 7.6135768

    # Step / stock
    varname = "ACAF"
    iode.ws_ltoh_stock(filename, varname, iode.LTOH_STEP)
    res = iode.exec_lec(f"{varname}[2014Q3]", 0)
    assert round(res, 5) == -83.34063

    # Step / flow
    varname = "ACAG"
    iode.ws_ltoh_flow(filename, varname, iode.LTOH_STEP)
    res = iode.exec_lec(f"{varname}[2014Q3]", 0)
    assert round(res, 7) == 8.1050747


def test_iode_htol_la():
    # Creates a quaterly larray and convert it to a yearly one
   
    # Creating a new Quaterly 3D-Array la3D 
    vars = la.Axis("vars=AA,BB,CC")
    sectors = la.Axis("sectors=S1,S2")
    time = iode.ws_sample_to_larray_axis('time', "2000Q2", "2010Q1")

    la3D = la.ones([vars, sectors, time])

    # Setting values in la3D for S1, S2 in 2001Q4 and 2002Q2
    la3D["2001Q4","S1"] = [1, 2, 3]     # AA_S1[2001Q4] = 1;  BB_S1[2001Q4] = 2;  CC_S1[2001Q4] = 3;
    la3D["2002Q2","S2"] = [11, 22, 33]  # AA_S2[2001Q4] = 11; BB_S2[2001Q4] = 22; CC_S2[2001Q4] = 33;

    # Saving la3D in KV_WS, then in file testq_3D.var
    iode.ws_clear_var()
    iode.larray_to_ws(la3D)
    filename = str(IODE_OUTPUT_DIR / "testq_3D")
    iode.ws_save_var(filename)
 
    # Import testq_3D in yearly WS
    iode.ws_clear_var()                 # Clear the ws before setting a new sample 
    iode.ws_sample_set("2000Y1", "2010Y1")  
    
    # Last Obs in year
    varname = "BB_S1"
    iode.ws_htol_last(filename, varname)
    res = iode.exec_lec(f"{varname}[2001Y1]", 0)
    assert res == 2.0

    # mean of year
    varname = "BB_S1"
    iode.ws_htol_mean(filename, varname)
    res = iode.exec_lec(f"{varname}[2001Y1]", 0)
    assert res == 5.0/4.0

    # sum
    varname = "BB_S1"
    iode.ws_htol_sum(filename, "AA_S1 BB_S1") # CC_S1 AA_S2 BB_S2 CC_S2")
    res = iode.exec_lec(f"{varname}[2001Y1]", 0)
    assert res == 5.0

    # save the new yearly ws
    filename = str(IODE_OUTPUT_DIR / "testy_3D")
    iode.ws_save_var(filename)
    
    # read in la
    la3Dy = iode.ws_load_var_to_larray(filename, split_axis_names=["vars", "sectors"], split_sep="_")
    print(la3Dy)


# PANDAS FUNCTIONS
# ----------------

def test_iode_df_to_ws():
    # Clear the WS
    iode.ws_clear_all()

    # Creating a new simple dataframe df
    data = {"1991Y1": [0, 0.5, 1], "1992Y1": [2, 2.5, 3], "1993Y1": [4, 4.5, 5]}
    index = ["AA", "BB", "CC"]
    df = pd.DataFrame(data=data, index=index)
    df.index.name = "vars"
    df.columns.name = "time"

    # Copying la1 to KV_WS
    iode.df_to_ws(df)

    # Check nb of objects
    nbvars = len(iode.ws_content_var("*"))
    assert nbvars == 3

    # Check values
    AA = iode.get_var("AA")
    assert AA[1] ==  2.0


def test_iode_df_to_ws_timeit():
    import timeit

    # Clear the WS
    iode.ws_clear_all()

    # Creating a new simple dataframe df
    data = {"1991Y1": [0, 0.5, 1], "1992Y1": [2, 2.5, 3], "1993Y1": [4, 4.5, 5]}
    index = ["AA", "BB", "CC"]
    df = pd.DataFrame(data=data, index=index)
    df.index.name = "vars"
    df.columns.name = "time"

    stmt = "iode.df_to_ws(df)"
    t = timeit.timeit(stmt, globals={"iode": iode, "df": df}, number=10000)
    logging.info(f"{stmt}: {t}")

    if nanobind:
        var_names = df.index.to_list()
        periods = df.columns.to_list()

        stmt = "df.to_numpy(dtype=float, copy=False, na_value=iode.nan)"
        t = timeit.timeit(stmt, globals={"iode": iode, "df": df}, number=10000)
        logging.info(f"{stmt}: {t}")

        # na_value=nan converts Numpy NaN to IODE NaN values
        data = df.to_numpy(dtype=float, copy=False, na_value=iode.nan)

        import iode.iode_python_api as cpp_api

        assert data.flags['F_CONTIGUOUS']
        stmt = "cpp_api.__set_vars_from_ndarray(var_names, periods[0], periods[-1], data)"
        t = timeit.timeit(stmt, globals={"cpp_api": cpp_api, "var_names": var_names, "periods": periods, "data": data}, number=10000)
        logging.info(f"{stmt} [F CONTIGUOUS]: {t}")

        stmt = "np.ascontiguousarray(data)"
        t = timeit.timeit(stmt, globals={"data": data, "np": np}, number=10000)
        logging.info(f"{stmt}: {t}")

        data = np.ascontiguousarray(data)

        assert data.flags['C_CONTIGUOUS']
        stmt = "cpp_api.__set_vars_from_ndarray_contiguous(var_names, periods[0], periods[-1], data)"
        t = timeit.timeit(stmt, globals={"cpp_api": cpp_api, "var_names": var_names, "periods": periods, "data": data}, number=10000)
        logging.info(f"{stmt} [C CONTIGUOUS]: {t}")

        cpp_api.__set_vars_from_ndarray_contiguous(var_names, periods[0], periods[-1], data)


def test_iode_ws_to_df():
    # Clear the WS
    iode.ws_load_var(str(IODE_DATA_DIR / "fun.var"))
    df = iode.ws_to_df()
    
    # Check nb of objects
    nbvars = len(df.index)
    assert nbvars == 394

    # Check values
    value = df.loc["ACAF", "1990Y1"]
    assert value == 23.771


# LARRAY FUNCTIONS
# ----------------

def test_iode_larray_to_ws_2D():
    # Clear the WS
    iode.ws_clear_all()

    # Creating a new simple 2D-Array la1
    vars = la.Axis("vars=AA,BB,CC")
    time = la.Axis("time=1991Y1,1992Y1,1993Y1")
    la1 = la.ones([vars, time])

    # Setting values in la1 for AA, BB and CC in 1990 and 1991
    la1["1992Y1"] = [2, 3, 4]
    la1["1993Y1"] = [22, 33, 44]

    # Copying la1 to KV_WS
    iode.larray_to_ws(la1)

    # Check nb of objects
    nbvars = len(iode.ws_content_var("*"))
    assert nbvars == 3

    # Check values
    AA = iode.get_var("AA")
    assert AA[1] == 2.0

    # Saving KV_WS to la1/var
    iode.ws_save_var(str(IODE_OUTPUT_DIR / "la1.var"))


# Working with a 3D-Array la3D (vars, sector, time)
# -------------------------------------------------

def test_iode_larray_to_ws_3D():

    # Clear the WS
    iode.ws_clear_all()

    # Creating a new 3D-Array la3D
    sectors = la.Axis("sectors=S1,S2")
    vars = la.Axis("vars=AA,BB,CC")
    time = la.Axis("time=1991Y1,1992Y1,1993Y1")

    # Setting values in la3D for S1, S2 in 1900 and 1991
    la3D = la.zeros([vars, sectors, time])
    la3D["1991Y1","S1"] = [1, 2, 3]
    la3D["1992Y1","S2"] = [11, 22, 33]

    assert la3D.data.data.c_contiguous

    # Saving la3D in KV_WS
    iode.larray_to_ws(la3D)

    # Check nb of objects
    nbvars = len(iode.ws_content_var("*"))
    assert nbvars == 6

    # Check values
    BB_S2 = iode.get_var("BB_S2")
    assert BB_S2[1] == 22.0

    # Save KV_WS to la3d.var
    iode.ws_save_var(str(IODE_OUTPUT_DIR / "la3D.var"))


def test_iode_larray_to_ws_3D_R():

    # Clear the WS
    iode.ws_clear_all()

    # Creating a new 3D-Array la3D
    sectors = la.Axis("sectors=S1,S2")
    vars = la.Axis("vars=AA,BB,CC")
    time = la.Axis("time=1991Y1,1992Y1,1993Y1")

    # Setting values in la3D for S1, S2 in 1900 and 1991
    la3D = la.zeros([time, vars, sectors])
    la3D["1991Y1","S1"]= [1, 2, 3]
    la3D["1992Y1","S2"]=[11, 22, 33]

    # Saving la3D in KV_WS
    iode.larray_to_ws(la3D)

    # Check nb of objects
    nbvars = len(iode.ws_content_var("*"))
    assert nbvars == 6

    # Check values
    BB_S2 = iode.get_var("BB_S2")
    assert BB_S2[1] == 22.0

    # Save KV_WS to file
    iode.ws_save_var(str(IODE_OUTPUT_DIR / "la3D_R.var"))


def test_iode_larray_to_ws_long_sample():

    # Clear the WS
    iode.ws_clear_all()

    # Set IODE sample
    iode.ws_sample_set("1990Y1", "2000Y1")

    # Creating a new 3D-Array la3D_R
    sectors = la.Axis("sectors=S1,S2")
    vars = la.Axis("vars=AA,BB,CC")
    time = la.Axis("time=1991Y1,1992Y1,1993Y1")

    # Setting values in la3D for S1, S2 in 1900 and 1991
    la3D_R = la.zeros([time, vars, sectors])
    la3D_R["1991Y1","S1"] = [7, 8, 9]
    la3D_R["1992Y1","S2"] = (99, 88, 77)

    # Saving la3D in KV_WS
    iode.larray_to_ws(la3D_R)

    # Check nb of objects
    expected_nbvars = 6
    nbvars = len(iode.ws_content_var("*"))
    assert nbvars == 6

    # Check values
    BB_S2 = iode.get_var("BB_S2")
    assert BB_S2[2] == 88.0

    # Save KV_WS to la3d.var
    iode.ws_save_var(str(IODE_OUTPUT_DIR / "la3D_R2.var"))


def test_iode_larray_to_ws_short_sample():

    # Clear the WS
    iode.ws_clear_all()

    # Set IODE sample
    iode.ws_sample_set("1992Y1", "1994Y1")

    # Creating a new 3D-Array la3D_R
    sectors = la.Axis("sectors=S1,S2")
    vars = la.Axis("vars=AA,BB,CC")
    time = la.Axis("time=1991Y1,1992Y1,1993Y1,1994Y1,1995Y1")

    # Setting values in la3D for S1, S2 in 1900 and 1991
    la3D_R = la.zeros([time, vars, sectors])
    la3D_R["1991Y1","S1"] = [5, 6, 7]
    la3D_R["1992Y1","S2"] = (99, 88, 77)

    # Saving la3D in KV_WS
    iode.larray_to_ws(la3D_R)

    # Save KV_WS
    iode.ws_save_var(str(IODE_OUTPUT_DIR / "la3D_R3.var"))


def test_iode_larray_to_ws_out_sample():

    # Clear the WS
    iode.ws_clear_all()

    # Set IODE sample
    iode.ws_sample_set("2000Y1", "2005Y1")

    # Creating a new 3D-Array la3D_R
    sectors = la.Axis("sectors=S1,S2")
    vars = la.Axis("vars=AA,BB,CC")
    time = la.Axis("time=1991Y1,1992Y1,1993Y1,1994Y1,1995Y1")

    # Setting values in la3D for S1, S2 in 1900 and 1991
    la3D_R = la.zeros([time, vars, sectors])
    la3D_R["1991Y1","S1"] = [5, 6, 7]
    la3D_R["1992Y1","S2"] = (99, 88, 77)

    # Saving la3D in KV_WS
    iode.larray_to_ws(la3D_R)

    # Save KV_WS to la3d.var
    iode.ws_save_var(str(IODE_OUTPUT_DIR / "la3D_R4.var"))


def test_iode_larray_to_ws_big_la():

    # Clear the WS
    iode.ws_clear_all()

    # Set IODE sample
    iode.ws_sample_set("1992Y1", "1994Y1")

    # Creating Axes
    sectors = la.Axis("sectors=S0,S1,S2,S3,S4,S5,S6,S7,S8,S9")
    dim2 = la.Axis("dim2=A1,B1,C1,D1,E1,F1,G1,H1,I1, J1")
    dim3 = la.Axis("dim3=A2,B2,C2,D2,E2,F2,G2,H2,I2, J2")
    dim4 = la.Axis("dim4=A3,B3,C3,D3,E3,F3,G3,H3,I3, J3")
    vars = la.Axis("vars=AA,BB,CC,DD,EE,FF,GG,HH,II,JJ")
    time = la.Axis("time=1991Y1,1992Y1,1993Y1,1994Y1,1995Y1")

    # Create a new LArray with time in the last position
    la3D_R5 = la.zeros([vars, sectors, dim2, dim3, dim4, time])

    # Setting values in la3D for S1, S2 in 1900 and 1991
    la3D_R5["1992Y1","S1", "A1", "A2", "A3"] = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
    la3D_R5["1994Y1","S2", "A1", "A2", "A3"] = [11, 22, 33, 44, 55, 66, 77, 88, 99, 100.4]

    # Saving la3D in KV_WS
    iode.larray_to_ws(la3D_R5)

    # Creating a new LArray with time in the middle
    la3D_R5 = la.zeros([vars, sectors, time, dim2, dim3, dim4])

    # Setting values in la
    la3D_R5["1992Y1", "S1", "A1", "A2", "A3"] = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
    la3D_R5["1994Y1", "S2", "A1", "A2", "A3"] = [11, 22, 33, 44, 55, 66, 77, 88, 99, 100.4]

    # Saving la3D in KV_WS
    iode.larray_to_ws(la3D_R5)

    # Save KV_WS
    iode.ws_save_var(str(IODE_OUTPUT_DIR / "la3D_R5.var"))
