# IODE EXTENSION FOR PYTHON
# =========================
#
#    @header4iode
#
# Unit tests for iode.pyd

import iode
import pytest
import larray as la
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

from iode import Comments

# MISC FUNCTIONS
# --------------

def test_iode_version():
    result = iode.version()
    assert result == "IODE Modeling Software 7.0.0 - (c) 1990-2023 Federal Planning Bureau - Brussels"


# PYIODE_OBJECTS
# --------------

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
    assert round(e_ACAF.test_r2, 5) == 0.82176


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


# EXECUTE LEC
# -----------


def test_iode_exec_lec():
    iode.Variables.load(str(IODE_DATA_DIR / "a.var"))

    v_A = iode.Variables["A"]
    vec = iode.exec_lec("1+A-1")
    assert vec == v_A

    res = iode.exec_lec("A+1", 0)
    assert  res == v_A[0] + 1


# REPORT DATA_* FUNCTIONS
# -----------------------

# DATA_UPDATE
# -----------

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


# ESTIMATION
# ----------


def test_iode_eqs_estimation():
    iode.ws_load_eqs(str(IODE_DATA_DIR / "fun.eqs"))
    iode.Variables.load(str(IODE_DATA_DIR / "fun.var"))
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
    iode.Variables.load(str(IODE_DATA_DIR / "fun.var"))
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
    iode.Variables.load(str(IODE_DATA_DIR / "fun.var"))
    iode.ws_load_scl(str(IODE_DATA_DIR / "fun.scl"))

    # Version with exchange in at least 2 equations
    # Set values of endo UY
    UY = iode.Variables["UY"]
    UY[40:43] = [650.0, 670.0, 680.0]   # 2000Y1..2002Y1
    iode.Variables["UY"] = UY

    # Simulate with exchange UY - XNATY
    iode.model_simulate("2000Y1", "2002Y1", endo_exo_list="UY-XNATY", relax=0.7)

    # Check result
    UY = iode.Variables["UY"]
    XNATY = iode.Variables["XNATY"]
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
    iode.Variables.clear()
    iode.Variables.sample = "2000Y1:2020Y1"
    
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
    iode.Variables.clear()
    iode.Variables.sample = "2010Q1:2020Q4"

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
   
    periods = ["2000Q2", "2000Q3", "2000Q4"] + [f"{y}Q{q}" for y in range(2001, 2010) for q in [1, 2, 3, 4]] + ["2010Q1"]

    # Creating a new Quaterly 3D-Array la3D 
    vars = la.Axis("vars=AA,BB,CC")
    sectors = la.Axis("sectors=S1,S2")
    time = la.Axis(periods, "time")

    la3D = la.ones([vars, sectors, time])

    # Setting values in la3D for S1, S2 in 2001Q4 and 2002Q2
    la3D["2001Q4","S1"] = [1, 2, 3]     # AA_S1[2001Q4] = 1;  BB_S1[2001Q4] = 2;  CC_S1[2001Q4] = 3;
    la3D["2002Q2","S2"] = [11, 22, 33]  # AA_S2[2001Q4] = 11; BB_S2[2001Q4] = 22; CC_S2[2001Q4] = 33;

    # Saving la3D in KV_WS, then in file testq_3D.var
    iode.Variables.clear()
    iode.Variables.from_array(la3D)

    filename = str(IODE_OUTPUT_DIR / "testq_3D")
    iode.Variables.save(filename)
 
    # Import testq_3D in yearly WS
    iode.Variables.clear() 
    iode.Variables.sample = "2000Y1:2010Y1"  
    
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
    iode.Variables.save(filename)


# PANDAS FUNCTIONS
# ----------------

def test_from_frame_timeit():
    import timeit
    import pandas as pd
    import numpy as np

    iode.Variables.clear()

    # create the pandas DataFrame
    vars_names = [f"A_{i}" for i in range(0, 10_000)]           # 10_000 variables
    periods_list = [f"{i}Y1" for i in range(1951, 2051)]        # 100 periods
    data = np.arange(len(vars_names) * len(periods_list), dtype=float).reshape(len(vars_names), len(periods_list))
    df = pd.DataFrame(index=vars_names, columns=periods_list, data=data)

    assert df.shape == (10_000, 100)
    assert df.size == 1000_000

    stmt = "iode.Variables.from_frame(df)"
    nb_times = 10
    t = timeit.timeit(stmt, globals={"iode": iode, "df": df}, number=nb_times)
    logging.info(f"{stmt} (shape {df.shape}): {t} ({nb_times} times) -> {t / nb_times} per loop")

def test_to_frame_timeit():
    import timeit
    import pandas as pd
    import numpy as np

    iode.Variables.clear()

    # set sample to 100 periods
    iode.Variables.sample = "1951Y1:2050Y1"
    assert iode.Variables.nb_periods == 100

    # create 10_000 variables
    for i in range(0, 10_000):
        iode.Variables[f"A_{i}"] = f"t + {i}"
    assert len(iode.Variables) == 10_000

    stmt = "df = iode.Variables.to_frame()"
    nb_times = 10
    t = timeit.timeit(stmt, globals={"iode": iode}, number=nb_times)
    logging.info(f"{stmt} (shape ({len(iode.Variables)}, {iode.Variables.nb_periods})): "
                 f"{t} ({nb_times} times) -> {t / nb_times} per loop")
