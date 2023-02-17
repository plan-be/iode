# IODE EXTENSION FOR PYTHON
# =========================
#
#    @header4iode
#
# Unit tests for iode.pyd

import iode
import larray as la
import numpy as np

# GLOBALS
IODE_DATA_DIR = "../tests/data/"
IODE_OUTPUT_DIR = "output/"
IODE_VERBOSE = 1

# Utility functions
# -----------------
def test_gnl(title, success):
    if not success:
        raise RuntimeError(f"{title}")
    elif IODE_VERBOSE:
        print(f"{title}: passed")


def test_eq_float(v1:float, v2:float):

    if abs(v2) > 1e-8:
        diff = abs((v2 - v1) / v2)
    else:
        diff = abs(v2 - v1)

    return diff < 1e-5;


def test_eq(title, expected, result):
    if type(expected) == float:
        eq = test_eq_float(expected, result)
    else:
        eq = expected == result

    if not eq:
        raise RuntimeError(f"{title}: error.\n\tResult   = '{result}'\n\tExpected = '{expected}'")
    elif IODE_VERBOSE:
        print(f"{title}: passed.\n\tResult   = '{result}'\n\tExpected = '{expected}'")


from datetime import datetime

def cpu(func):
    '''Log the elapsed time of a function call'''

    def wrapper(*args, **kwargs):
        iode.cpu_init()
        #txt = f'{datetime.today().strftime("%Y-%m-%d %H:%M:%S")} {func.__name__}'
        # txt = txt + "\n" + len(txt) * '-'
        txt = f'{func.__name__}'
        txt = txt + "\n" + len(txt) * '-'
        print(txt)
        func(*args, **kwargs)
        print(iode.cpu_end_msg() + "\n")

    return wrapper

def test_message(txt):
    print(f"\n{txt}")
    print(len(txt) * "-")


# iode.pyd unit tests
# -------------------

# MISC FUNCTIONS
# --------------
@cpu
def test_iode_version():
    result = iode.version()
    expected = "IODE Modeling Software 6.64 - (c) 1990-2023 Planning Office - Brussels"
    test_eq("iode.version()", expected, result)


# WS FUNCTIONS
# ------------
@cpu
def test_iode_ws_content():
    iode.ws_load_cmt(f"{IODE_DATA_DIR}fun.cmt")
    result = iode.ws_content_cmt("ACA*")
    expected = ["ACAF", "ACAG"]
    test_eq("iode.ws_content_cmt()", expected, result)

@cpu
def test_iode_ws_clear_var():
    varfile = f"{IODE_DATA_DIR}a.var"
    nbvars = iode.ws_load_var(varfile)
    #print(f"{nbvars} variables have been read")

    iode.ws_clear_var()
    result = iode.ws_content_var("*")
    test_eq("iode.ws_clear_var", [], result)

@cpu
def test_iode_ws_load_var():
    # Loading var varfile in IODE memory (KV_WS): iode.ws_load_var(filename)
    varfile = f"{IODE_DATA_DIR}a.var"
    nbvars = iode.ws_load_var(varfile)
    #print(f"{nbvars} variables have been read")
    test_eq("iode.ws_load_var", nbvars, 433)


def u_test_ws_load_(filename:str, filetype:int, expected_nb_objects:int):
    fullfilename = f"{IODE_DATA_DIR}{filename}"
    nbvars = iode.ws_load_(fullfilename, filetype)
    test_eq(f"iode.ws_load_{filetype}('{fullfilename}')", expected_nb_objects, nbvars)


@cpu
def test_iode_ws_load_all():
    # Loading var varfile in IODE memory (KV_WS): iode.ws_load_var(filename)

    filename = "fun"
    u_test_ws_load_(filename, 0, 317);
    u_test_ws_load_(filename, 1, 274);
    u_test_ws_load_(filename, 2, 48);
    u_test_ws_load_(filename, 3, 17);
    u_test_ws_load_(filename, 4, 161);
    u_test_ws_load_(filename, 5, 46);
    u_test_ws_load_(filename, 6, 394);

@cpu
def test_iode_ws_sample():

    varfile = f"{IODE_DATA_DIR}a"
    iode.ws_load_var(varfile)
    sample = iode.ws_sample()
    test_eq("ws_sample()", ['1990Y1', '2020Y1'], sample)
    print(f"Sample of {varfile} = {sample}")

    sample = iode.ws_sample("1995Y1")
    test_eq("ws_sample()", ['1995Y1', '2020Y1'], sample)
    #print(f"New sample = {sample}")

    sample = iode.ws_sample(None, "2030Y1")
    test_eq("ws_sample()", ['1995Y1', '2030Y1'], sample)
    #print(f"New sample = {sample}")

    sample = iode.ws_sample("2000Y1", "2002Y1")
    test_eq("ws_sample()", ['2000Y1', '2002Y1'], sample)
    #print(f"New sample = {sample}")

    nobs = iode.ws_sample_length()
    test_eq("ws_samplelength()", 3, nobs)
    #print(f"Nobs = {nobs}")

    str1 = iode.ws_sample_as_string()
    test_eq("ws_sample_as_string()", "2000Y1 2002Y1", str1)
    #print("string = ", repr(str1))

    lst = iode.ws_sample_as_list()
    test_eq("ws_sample_as_list()", ["2000Y1", "2001Y1", "2002Y1"], lst)
    #print("list = ", repr(lst))

    ax2 = iode.ws_sample_as_axis()
    test_eq("ws_sample_as_axis()", repr(la.Axis(["2000Y1", "2001Y1", "2002Y1"], 'time')), repr(ax2))
    #print("axis = ", repr(ax2))


# LARRAY FUNCTIONS
# ----------------
@cpu
def test_iode_larray_to_ws_2D():
    # Clear the WS
    iode.ws_clear_all()

    # Creating a new simple 2D-Array la1
    vars = la.Axis("vars=AA,BB,CC")
    time = la.Axis("time=1991Y1,1992Y1,1993Y1")
    la1 = la.ones([vars, time])

    # Setting values in la1 for AA, BB and CC in 1990 and 1991
    la1["1992Y1"]=[2,3,4]
    la1["1993Y1"]=(22,33,44)

    # Copying la1 to KV_WS
    iode.larray_to_ws(la1)

    # Check nb of objects
    expected_nbvars = 3
    nbvars = len(iode.ws_content_var("*"))
    test_eq(f"iode.larray_to_ws()", expected_nbvars, nbvars)

    # Check values
    AA = iode.var_to_py("AA")
    test_eq(f"iode.larray_to_ws() - 2D", 2.0, AA[1])

    # Saving KV_WS to la1/var
    iode.ws_save_var(f"{IODE_OUTPUT_DIR}la1.var")


# Working with a 3D-Array la3D (vars, sector, time)
# -------------------------------------------------
@cpu
def test_iode_larray_to_ws_3D():

    # Clear the WS
    iode.ws_clear_all()

    # Creating a new 3D-Array la3D
    sectors = la.Axis("sectors=S1,S2")
    vars = la.Axis("vars=AA,BB,CC")
    time = la.Axis("time=1991Y1,1992Y1,1993Y1")

    # Setting values in la3D for S1, S2 in 1900 and 1991
    la3D = la.zeros([vars, sectors, time])
    la3D["1991Y1","S1"]=[1,2,3]
    la3D["1992Y1","S2"]=(11,22,33)
    print("c_contiguous", la3D.data.data.c_contiguous)

    # Saving la3D in KV_WS
    iode.larray_to_ws(la3D)

    # Check nb of objects
    expected_nbvars = 6
    nbvars = len(iode.ws_content_var("*"))
    test_eq(f"iode.larray_to_ws() - 3D", expected_nbvars, nbvars)

    # Check values
    BB_S2 = iode.var_to_py("BB_S2")
    test_eq(f"iode.larray_to_ws() - 3D", 22.0, BB_S2[1])


    # Save KV_WS to la3d.var
    iode.ws_save_var(f"{IODE_OUTPUT_DIR}la3D.var")


@cpu
def test_iode_larray_to_ws_3D_R():

    # Clear the WS
    iode.ws_clear_all()

    # Creating a new 3D-Array la3D
    sectors = la.Axis("sectors=S1,S2")
    vars = la.Axis("vars=AA,BB,CC")
    time = la.Axis("time=1991Y1,1992Y1,1993Y1")

    # Setting values in la3D for S1, S2 in 1900 and 1991
    la3D = la.zeros([time, vars, sectors])
    la3D["1991Y1","S1"]=[1,2,3]
    la3D["1992Y1","S2"]=(11,22,33)

    # Saving la3D in KV_WS
    iode.larray_to_ws(la3D)

    # Check nb of objects
    expected_nbvars = 6
    nbvars = len(iode.ws_content_var("*"))
    test_eq(f"iode.larray_to_ws() - 3D_R", expected_nbvars, nbvars)

    # Check values
    BB_S2 = iode.var_to_py("BB_S2")
    test_eq(f"iode.larray_to_ws() - 3D_R", 22.0, BB_S2[1])

    # Save KV_WS to file
    iode.ws_save_var(f"{IODE_OUTPUT_DIR}la3D_R.var")

@cpu
def test_iode_larray_to_ws_long_sample():

    # Clear the WS
    iode.ws_clear_all()

    # Set IODE sample
    iode.ws_sample("1990Y1", "2000Y1")

    # Creating a new 3D-Array la3D_R
    sectors = la.Axis("sectors=S1,S2")
    vars = la.Axis("vars=AA,BB,CC")
    time = la.Axis("time=1991Y1,1992Y1,1993Y1")

    # Setting values in la3D for S1, S2 in 1900 and 1991
    la3D_R = la.zeros([time, vars, sectors])
    la3D_R["1991Y1","S1"]=[7, 8, 9]
    la3D_R["1992Y1","S2"]=(99, 88, 77)

    # Saving la3D in KV_WS
    iode.larray_to_ws(la3D_R)

    # Check nb of objects
    expected_nbvars = 6
    nbvars = len(iode.ws_content_var("*"))
    test_eq(f"iode.larray_to_ws() - 3D_R", expected_nbvars, nbvars)

    # Check values
    BB_S2 = iode.var_to_py("BB_S2")
    test_eq(f"iode.larray_to_ws() - 3D_R", 88.0, BB_S2[2])

    # Save KV_WS to la3d.var
    iode.ws_save_var(f"{IODE_OUTPUT_DIR}la3D_R2.var")


@cpu
def test_iode_larray_to_ws_short_sample():

    # Clear the WS
    iode.ws_clear_all()

    # Set IODE sample
    iode.ws_sample("1992Y1", "1994Y1")

    # Creating a new 3D-Array la3D_R
    sectors = la.Axis("sectors=S1,S2")
    vars = la.Axis("vars=AA,BB,CC")
    time = la.Axis("time=1991Y1,1992Y1,1993Y1,1994Y1,1995Y1")

    # Setting values in la3D for S1, S2 in 1900 and 1991
    la3D_R = la.zeros([time, vars, sectors])
    la3D_R["1991Y1","S1"]=[5, 6, 7]
    la3D_R["1992Y1","S2"]=(99, 88, 77)

    # Saving la3D in KV_WS
    iode.larray_to_ws(la3D_R)

    # Save KV_WS
    iode.ws_save_var(f"{IODE_OUTPUT_DIR}la3D_R3.var")


@cpu
def test_iode_larray_to_ws_out_sample():

    # Clear the WS
    iode.ws_clear_all()

    # Set IODE sample
    iode.ws_sample("2000Y1", "2005Y1")

    # Creating a new 3D-Array la3D_R
    sectors = la.Axis("sectors=S1,S2")
    vars = la.Axis("vars=AA,BB,CC")
    time = la.Axis("time=1991Y1,1992Y1,1993Y1,1994Y1,1995Y1")

    # Setting values in la3D for S1, S2 in 1900 and 1991
    la3D_R = la.zeros([time, vars, sectors])
    la3D_R["1991Y1","S1"]=[5, 6, 7]
    la3D_R["1992Y1","S2"]=(99, 88, 77)

    # Saving la3D in KV_WS
    iode.larray_to_ws(la3D_R)

    # Save KV_WS to la3d.var
    iode.ws_save_var(f"{IODE_OUTPUT_DIR}la3D_R4.var")


@cpu
def test_iode_larray_to_ws_big_la():

    # Clear the WS
    iode.ws_clear_all()

    # Set IODE sample
    iode.ws_sample("1992Y1", "1994Y1")

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
    la3D_R5["1992Y1","S1", "A1", "A2", "A3"]=[1,2,3,4,5,6,7,8,9,10]
    la3D_R5["1994Y1","S2", "A1", "A2", "A3"]=(11,22,33,44,55,66,77,88,99,100.4)

    # Saving la3D in KV_WS
    iode.cpu_init()
    iode.larray_to_ws(la3D_R5)
    iode.cpu_end("larray_to_ws() - time last position")

    # Creating a new LArray with time in the middle
    iode.cpu_init()
    la3D_R5 = la.zeros([vars, sectors, time, dim2, dim3, dim4])
    iode.cpu_end("Creation of the 6D array")

    # Setting values in la
    la3D_R5["1992Y1", "S1", "A1", "A2", "A3"] = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
    la3D_R5["1994Y1", "S2", "A1", "A2", "A3"] = (11, 22, 33, 44, 55, 66, 77, 88, 99, 100.4)

    # Saving la3D in KV_WS
    iode.cpu_init()
    iode.larray_to_ws(la3D_R5)
    iode.cpu_end("larray_to_ws() - time in the middle")

    # Save KV_WS
    iode.cpu_init()
    iode.ws_save_var(f"{IODE_OUTPUT_DIR}la3D_R5.var")
    iode.cpu_end("Save WS" )


# PYIODE_OBJECTS
# --------------


# IODE COMMENTS <-> PYTHON STRINGS
# --------------------------------
@cpu
def test_iode_cmt_to_py():

    iode.ws_load_cmt(f"{IODE_DATA_DIR}fun.cmt")
    c_ACAF = iode.cmt_to_py("ACAF")
    c_EFM = iode.cmt_to_py("EFM")
    print(f"ACAF = '{c_ACAF}'")
    print(f"EFM = '{c_EFM}'")
    test_eq("iode.cmt_to_py()", "Ondernemingen: ontvangen kapitaaloverdrachten.", c_ACAF)
    test_eq("iode.cmt_to_py()", 'Vreemde grensarbeiders in België (30 juni).', c_EFM)


@cpu
def test_iode_py_to_cmt():

    # Clear CMT before creating new comments
    iode.ws_clear_cmt()

    # Create comment as py strings
    pycmt1 = "New comment with éîâëéàçèï"
    pycmt2 = "Other comment"

    # Save them in KC_WS + check
    rc = iode.py_to_cmt("MYCMT1", pycmt1)
    #print(f"iode.py_to_cmt = {rc}")
    rc = iode.py_to_cmt("MYCMT2", pycmt2)
    #print(f"iode.py_to_cmt = {rc}")
    #iode.ws_save_cmt(f"{IODE_OUTPUT_DIR}funtest.cmt")

    ## Reload to check values
    #iode.ws_load_cmt("funtest.cmt")
    c_MYCMT1 = iode.cmt_to_py("MYCMT1")
    c_MYCMT2 = iode.cmt_to_py("MYCMT2")
    print(f"MYCMT1 = '{c_MYCMT1}'")
    print(f"orig   = '{pycmt1}'")
    print(f"MYCMT2 = '{c_MYCMT2}'")
    print(f"orig   = '{pycmt2}'")

    test_eq(f"iode.py_to_cmt()", pycmt1, c_MYCMT1)
    test_eq(f"iode.py_to_cmt()", pycmt2, c_MYCMT2)


# IODE EQUATIONS <-> PYTHON STRINGS
# ---------------------------------
@cpu
def test_iode_eqs_lec_to_py():

    iode.ws_load_eqs(f"{IODE_DATA_DIR}fun")
    lec_BVY = iode.eqs_lec_to_py("BVY")
    print(f"lec_BVY = '{lec_BVY}'")
    test_eq(f"iode.eqs_lec_to_py()", "BVY:=YN+YK", lec_BVY)


@cpu
def test_iode_eqs_to_py():

    iode.ws_load_eqs(f"{IODE_DATA_DIR}fun")
    e_BVY = iode.eqs_to_py("BVY")
    print(e_BVY)
    test_eq(f"iode.eqs_to_py()", "BVY:=YN+YK", e_BVY.lec)

    e_ACAF = iode.eqs_to_py("ACAF")
    print(f"e_ACAF = '{repr(e_ACAF)}'")
    test_eq(f"iode.eqs_to_py('ACAF.e_r2')", 0.82176, e_ACAF.e_r2)


@cpu
def test_iode_py_to_eqs():

    # Clear EQS WS before creating new object
    iode.ws_clear_eqs()

    # Create eq as py string
    py_A = "grt A := grt B"

    # Save py_GA as GA in KE_WS + check
    rc = iode.py_to_eqs("A", py_A)
    test_eq(f"iode.py_to_eqs('A', '{py_A}')", 0, rc)
    iode_A  = iode.eqs_lec_to_py("A")
    test_eq(f"iode.eqs_lec_to_py('A')", py_A, iode_A)

    # Test error
    py_A = "(grt A"
    rc = iode.py_to_eqs("A", py_A)
    test_eq(f"iode.py_to_eqs('{py_A}')", -1, rc)


# IODE IDENTITIES <-> PYTHON STRINGS
# ----------------------------------
@cpu
def test_iode_idt_to_py():

    iode.ws_load_idt(f"{IODE_DATA_DIR}fun.idt")
    i_XEX = iode.idt_to_py("XEX")
    #print(f"XEX = '{i_XEX}'")
    test_eq(f"iode.idt_to_py()", "grt EX", i_XEX)


@cpu
def test_iode_py_to_idt():

    # Clear IDT before creating new object
    iode.ws_clear_idt()

    # Create comment as py strings
    pyxex = "1 * (grt EX)"

    # Save pyxex as XEX in KI_WS + check
    rc = iode.py_to_idt("XEX", pyxex)
    i_XEX  = iode.idt_to_py("XEX")
    test_eq(f"iode.py_to_idt('{pyxex}')", pyxex, i_XEX)

    # Test error
    pyxex = "1 * (grt EX) - "
    rc = iode.py_to_idt("XEX", pyxex)
    test_eq(f"iode.py_to_idt('{pyxex}')", -1, rc)

# IODE LISTS <-> PYTHON STRINGS
# -----------------------------
@cpu
def test_iode_lst_to_py():

    iode.ws_load_lst(f"{IODE_DATA_DIR}fun")
    i_MYLIST = iode.lst_to_py("MYLIST")
    #print(f"XEX = '{i_XEX}'")
    test_eq(f"iode.lst_to_py('MYLIST')", "A02_075_H_BRU A02_100_H_VLA", i_MYLIST)


@cpu
def test_iode_py_to_lst():

    # Clear LST before creating new object
    iode.ws_clear_lst()

    # Create list as py strings
    py_list1 = "A;B;C"
    name = "LIST1"

    # Save py_list1 as LIST1 in KL_WS + check
    rc = iode.py_to_lst(name, py_list1)
    i_LIST1  = iode.lst_to_py(name)
    test_eq(f"iode.py_to_lst('{name}', '{py_list1}')", py_list1, i_LIST1)


# IODE SCALARS <-> PYTHON
# -----------------------
@cpu
def test_iode_scl_to_py():

    iode.ws_load_scl(f"{IODE_DATA_DIR}fun")
    name = "acaf1"
    i_acaf1 = iode.scl_to_py(name)
    #print(f"'{name}:{i_acaf1.val}, {i_acaf1.relax}, {i_acaf1.std}'")
    test_eq(f"iode.scl_to_py('{name}')", 0.0157684, i_acaf1.value)
    #print(i_acaf1)


@cpu
def test_iode_py_to_scl():

    # Clear SCL before creating new object
    iode.ws_clear_scl()

    # Create SCl
    name = "myscl"
    py_myscl = iode.scl("myscl", 1.2345, 1.0, 0.0)
    #print(py_myscl)

    # Save py_scl as myscl in KS_WS + reread and check
    rc = iode.py_to_scl(py_myscl)
    test_eq(f"iode.py_to_scl('{name}')", 0, rc)

    i_myscl = iode.scl_to_py(name)
    #print(i_myscl)
    #print(repr(py_myscl))

    test_eq(f"iode.py_to_scl('{name}') = {i_myscl.value}", repr(py_myscl), repr(i_myscl))


# VARIABLES IODE <-> PYTHON LISTS AND NDARRAYS
# --------------------------------------------
@cpu
def test_iode_var_to_ndarray():
    '''
    Getting and setting vectors of doubles from KV_WS
    x points to KV_WS["A"] because var_to_ndarray(..., 1)

    KV_WS["A"] is only modified via x[2] = ...
    '''

    varfile = f"{IODE_DATA_DIR}a"
    nbvars = iode.ws_load_var(varfile)

    name = "A"

    # x points to KV_WS["A"] because var_to_ndarray(..., 0)
    x = iode.var_to_ndarray(name, 0)
    x[2] = 22222 # Modifies the variable A in KV_WS
    A = iode.var_to_py("A")
    test_eq(f"iode.var_to_ndarray('{name}', 0) ", 22222.0, A[2])

    # y is a deep copy of KV_WS["A"] because var_to_ndarray(..., 0) => memory leak ?
    y = iode.var_to_ndarray(name, 1)
    y[2] = 33333 # Does not modify the variable A in KV_WS
    A = iode.var_to_py("A")
    test_eq(f"iode.var_to_ndarray('{name}', 1) ", 22222.0, A[2])

    # KV_WS["A"] after 2 changes
    # z = iode.var_to_ndarray(name, 1)
    # print(f"A = {z}")

    # XYY does not exist in KV_WS => new allocation ? TODO: check this + does XYY exist in KV_WS ?
    z = iode.var_to_ndarray("XYY", 1)
    #z[3] = 33333
    #print("z : ", z)
    XYY = iode.var_to_ndarray("XYY", 1)
    print("XYY : ", XYY)

    # Saving a copy of the modified KV_WS
    iode.ws_save_var(f"{IODE_OUTPUT_DIR}a_mod.var")


@cpu
def test_iode_var_to_py():

    varfile = f"{IODE_DATA_DIR}a"
    iode.ws_load_var(varfile)
    iode.ws_sample("1990Y1", "1992Y1")

    A = iode.var_to_py("A")
    B = [0.0, 1.0, 2]
    test_eq("iode.var_to_py(A)", A, B)


@cpu
def test_iode_py_to_var():

    nbvars = iode.ws_load_var(f"{IODE_DATA_DIR}a")

    AA = [11, 22, 33]
    AB = np.array([0.0, 111.0, 112.5], np.double)

    iode.py_to_var("AA", AA)
    iode.py_to_var("AB", AB)
    iode.ws_save_var(f"{IODE_OUTPUT_DIR}a_mod2.var")

    iode.ws_load_var("a_mod2.var")
    new_AA = iode.var_to_py("AA")
    new_AB = iode.var_to_py("AB")

    test_eq("py_to_var()", AA[2], new_AA[2]) # 1993Y1
    test_eq("py_to_var()", AB[1], new_AB[1]) # 1992Y1


@cpu
def test_iode_exec_lec():
    nbvars = iode.ws_load_var("a")

    v_A = iode.var_to_py("A")

    vec = iode.exec_lec("1+A-1")
    test_eq("exec_lec('1+A-1') == A", vec, v_A)

    res = iode.exec_lec("A+1", 0)
    test_eq("exec_lec('A+1', 0) == A[0]+1", res, v_A[0]+1)


# REPORT DATA_* FUNCTIONS
# -----------------------

# DATA_DELETE
# -----------

def test_iode_delete_idts(pattern:str):
    nbobjs = len(iode.ws_content_idt("*"))
    nbobjs_pattern = len(iode.ws_content_idt(pattern))
    rc = iode.delete_objects(pattern, 2)
    nbobjs2 = len(iode.ws_content_idt("*"))
    test_eq(f"iode.delete_objects('{pattern}', 2)", nbobjs - nbobjs_pattern, nbobjs2)


@cpu
def test_iode_delete_objects():

    # Load IDT ws and delete group of idts based on name pattern
    iode.ws_load_idt(f"{IODE_DATA_DIR}fun.idt")
    test_iode_delete_idts("KLL")
    iode.ws_load_idt(f"{IODE_DATA_DIR}fun.idt")
    test_iode_delete_idts("X*")


# DATA_UPDATE
# -----------

@cpu
def test_iode_data_update_cmt():

    A = "Comment of A"
    iode.data_update_cmt("A", A)
    new_A = iode.cmt_to_py("A")
    test_eq(f"data_update_cmt()", A, new_A)

@cpu
def test_iode_data_update_eqs():

    # Make a LEC error and check return code
    A = "A := ln t +++ "
    rc = iode.data_update_eqs("A", A)
    test_eq(f"data_update_eqs('{A}')", -1, rc)

    # Save a new equation A and check the return code
    A = "A := ln t + 1"
    rc = iode.data_update_eqs("A", A)
    test_eq(f"data_update_eqs('{A}')", 0, rc)

    # Get the lec value and compare with A
    new_A = iode.eqs_lec_to_py("A")
    test_eq(f"data_update_eqs('{A}')", A, new_A)

@cpu
def test_iode_data_update_var():

    varfile = f"{IODE_DATA_DIR}a"
    nbvars = iode.ws_load_var(varfile)

    A = iode.var_to_py("A")
    A[2] = 123
    B = [100, 101, 102] # 1992Y1 1993Y1 ...
    iode.data_update_var("A", A)
    iode.data_update_var("B", B, operation="L", per_from="1992Y1")
    #iode.ws_save_var(f"{IODE_OUTPUT_DIR}a_mod.var")
    #iode.ws_load_var(f"{IODE_OUTPUT_DIR}a_mod.var")  
    new_A = iode.var_to_py("A")
    new_B = iode.var_to_py("B")
    test_eq("data_update_var()", A[2], new_A[2]) # 1992Y1
    test_eq("data_update_var()", B[1], new_B[3]) # 1993Y1
    test_eq("data_update_var()", B[2], new_B[4]) # 1994Y1

def test_iode_data_update():
    test_iode_data_update_cmt()
    test_iode_data_update_eqs()
    #test_iode_data_update_idt()
    #test_iode_data_update_lst()
    #test_iode_data_update_scl()
    #test_iode_data_update_tbl()
    test_iode_data_update_var()


# Miscellaneous functions
# -----------------------
@cpu
def test_iode_ws_load_varpy():

    print('iode.ws_load_varpy("split.var")')
    print(iode.ws_load_varpy("split.var"))

    print('iode.ws_load_varpy("split.var", axis_names=["a", "b"], regex=r"(\w{2})(\w{2})")')
    print(iode.ws_load_varpy("split.var", axis_names=["a", "b"], regex=r"(\w{2})(\w{2})"))

    print('print(iode.ws_load_varpy("split.var", axis_names="a,b", regex=r"(\w{2})(\w{2})"))')
    print(iode.ws_load_varpy("split.var", axis_names="a,b".split(","), regex=r"(\w{2})(\w{2})"))

    print('iode.ws_load_varpy("split.var", varsaxis="a_b", axis_names=["a", "b"], regex=r"(\w{2})(\w{2})")')
    print(iode.ws_load_varpy("split.var", varsaxis="a_b", axis_names=["a", "b"], regex=r"(\w{2})(\w{2})"))


# ESTIMATION
# ----------

@cpu
def test_iode_eqs_estimation():
    iode.ws_load_eqs(f"{IODE_DATA_DIR}fun")
    iode.ws_load_var(f"{IODE_DATA_DIR}fun")
    iode.ws_load_scl(f"{IODE_DATA_DIR}fun")

    name = "ACAF"
    rc = iode.eqs_estimate(name, "1980Y1", "1996Y1")
    test_eq(f"iode.eqs_estimate('{name}')", 0, rc)

    # Check acaf1 value after estimation
    name = "acaf1"
    i_acaf1 = iode.scl_to_py(name)
    #print(f"'{name}:{i_acaf1.val}, {i_acaf1.relax}, {i_acaf1.std}'")
    test_eq(f"iode.scl_to_py('{name}')", 0.01577049, i_acaf1.value)

    # Check _YCALC[1980Y1]
    res = iode.exec_lec("_YRES[1980Y1]", 0)
    test_eq(f"iode.eqs_estimate('{name}') => _YRES[1980Y1]", -0.00115008, res)



# SIMULATION
# ----------

@cpu
def test_iode_model_simulate():
    iode.ws_load_eqs(f"{IODE_DATA_DIR}fun")
    iode.ws_load_var(f"{IODE_DATA_DIR}fun")
    iode.ws_load_scl(f"{IODE_DATA_DIR}fun")

    # Test non convergence
    iode.suppress_msgs()
    rc = iode.model_simulate("2000Y1", "2002Y1", "", eps=0.0001, relax=0.7, maxit=2)
    test_eq(f"iode.model_simulate()", -1, rc)

    # Test convergence
    rc = iode.model_simulate("2000Y1", "2002Y1", "", relax=0.7)
    test_eq(f"iode.model_simulate()", 0, rc)

    # Check some result after simulation
    # ACAF [2000..2002] before simulation = [10.0466107922005, 2.86792273645546, -0.929212509051645]
    # ACAF [2000..2002] after simulation  = [10.0466107922005, 2.62379276852768, -1.27462319299379]
    res = iode.exec_lec("ACAF[2002Y1]", 0)
    test_eq(f"iode.model_simulate : ACAF[2002Y1]", -1.27462319299379, res)

    iode.reset_msgs()

@cpu
def test_iode_model_simulate_exchange():

    iode.suppress_msgs()
    iode.ws_load_eqs(f"{IODE_DATA_DIR}fun")
    iode.ws_load_var(f"{IODE_DATA_DIR}fun")
    iode.ws_load_scl(f"{IODE_DATA_DIR}fun")

    # Version with exchange in at least 2 equations
    # Set values of endo UY
    UY = iode.var_to_py("UY");
    UY[40:43] = [650.0, 670.0, 680.0] # 2000Y1..2002Y1
    iode.py_to_var("UY", UY)

    # Simulate with exchange UY - XNATY
    rc = iode.model_simulate("2000Y1", "2002Y1", endo_exo_list="UY-XNATY", relax=0.7)
    test_eq(f"iode.model_simulate() with endo_exo", 0, rc)

    # Check result
    UY = iode.var_to_py("UY");
    XNATY = iode.var_to_py("XNATY")
    #print("UY:",UY)
    #print("XNATY:",XNATY)
    test_eq(f"UY[2000Y1]", 650.0, iode.exec_lec("UY[2000Y1]", 0))
    test_eq(f"XNATY[2000Y1]", 0.8006766, iode.exec_lec("XNATY[2000Y1]", 0))
    iode.reset_msgs()


def test_iode_wrt_1(filename:str="", dest:int=iode.W_A2M):

    iode.w_dest(filename, dest);
    
    iode.w_print(".sep |\n")
    iode.w_print(".esc ~\n")
    
    iode.w_print_pg_header("IODE")
    iode.w_print_pg_footer("- page %d - ")
    
    iode.w_print_tit(1, "Mon titre")
    
    iode.w_print_enum(1);
    iode.w_print("This is an enum paragraph\n\n");
    iode.w_print("This is a normal paragraph\n\n");
    
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

@cpu
def test_iode_wrt():
    #iode.suppress_msgs()
    test_iode_wrt_1("", iode.W_DUMMY)
    # test_iode_wrt_1("", iode.W_GDI) => doesn't work (win32 fn PrintDlg returns NULL)
    test_iode_wrt_1(f"{IODE_OUTPUT_DIR}test_wrt.htm", iode.W_HTML)
    test_iode_wrt_1(f"{IODE_OUTPUT_DIR}test_wrt.a2m", iode.W_A2M)
    test_iode_wrt_1(f"{IODE_OUTPUT_DIR}test_wrt.rtf", iode.W_RTF)
    test_iode_wrt_1(f"{IODE_OUTPUT_DIR}test_wrt.csv", iode.W_CSV)
    iode.reset_msgs()

# MAIN Program: calls to test_*
# -----------------------------

# iode.suppress_msgs()

test_iode_version()
test_iode_ws_content()
test_iode_ws_clear_var()
test_iode_ws_load_var()
test_iode_ws_load_all()
test_iode_larray_to_ws_2D()
test_iode_larray_to_ws_3D()
test_iode_larray_to_ws_3D_R()
test_iode_larray_to_ws_long_sample()
test_iode_larray_to_ws_short_sample()
test_iode_larray_to_ws_out_sample()
test_iode_larray_to_ws_big_la()

test_iode_ws_sample()

test_iode_cmt_to_py()
test_iode_py_to_cmt()

test_iode_eqs_lec_to_py()
test_iode_eqs_to_py()
test_iode_py_to_eqs()

test_iode_idt_to_py()
test_iode_py_to_idt()

test_iode_lst_to_py()
test_iode_py_to_lst()

test_iode_scl_to_py()
test_iode_py_to_scl()

test_iode_var_to_ndarray()
test_iode_var_to_py()
test_iode_py_to_var()

test_iode_exec_lec()

test_iode_delete_objects()

# Reports
test_iode_data_update()

# LArray
# test_iode_ws_load_varpy()

test_iode_eqs_estimation()
test_iode_model_simulate()
test_iode_model_simulate_exchange()

# write (wrt_*) functions
test_iode_wrt()


print("...Finito!")



