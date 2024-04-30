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
SAMPLE_DATA_DIR = Path(iode.SAMPLE_DATA_DIR).absolute()
IODE_OUTPUT_DIR = (Path(__file__).parent.parent / "output").absolute()
IODE_VERBOSE = 1

if not IODE_OUTPUT_DIR.exists():
    IODE_OUTPUT_DIR.mkdir()


# EXECUTE LEC
# -----------

def test_iode_exec_lec():
    iode.variables.load(f"{SAMPLE_DATA_DIR}/a.var")

    v_A = iode.variables["A"]
    vec = iode.exec_lec("1+A-1")
    assert vec == v_A

    res = iode.exec_lec("A+1", 0)
    assert  res == v_A[0] + 1

# ESTIMATION
# ----------

def test_iode_eqs_estimation():
    iode.equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")
    iode.variables.load(f"{SAMPLE_DATA_DIR}/fun.var")
    iode.scalars.load(f"{SAMPLE_DATA_DIR}/fun.scl")

    name = "ACAF"
    iode.eqs_estimate(name, "1980Y1", "1996Y1")

    # Check acaf1 value after estimation
    name = "acaf1"
    i_acaf1 = iode.scalars[name]
    assert round(i_acaf1.value, 8) == 0.0157705

    # Check _YCALC[1980Y1]
    res = iode.exec_lec("_YRES0[1980Y1]", 0)
    assert round(res, 8) == -0.00115008

# SIMULATION
# ----------

def test_iode_model_simulate():
    iode.equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")
    iode.variables.load(f"{SAMPLE_DATA_DIR}/fun.var")
    iode.scalars.load(f"{SAMPLE_DATA_DIR}/fun.scl")

    # Test non convergence
    iode.suppress_msgs()
    with pytest.raises(RuntimeError):
        iode.model_simulate("2000Y1", "2002Y1", "", eps=0.0001, relax=0.7, maxit=2)

    # Test convergence
    iode.model_simulate("2000Y1", "2002Y1", "", relax=0.7)
   
    # Check some result after simulation (values obtained with the BORLAND compiler)
    # ACAF [2000..2002] before simulation = [10.0466107922005, 2.86792273645546, -0.929212509051645]
    # ACAF [2000..2002] after simulation  = [10.0466107922005, 2.62379276852768, -1.27462319299379]
    res = iode.exec_lec("ACAF[2002Y1]", 0)
    assert round(res, 12) == -1.274623192994

    iode.reset_msgs()

def test_iode_model_simulate_exchange():

    iode.suppress_msgs()
    iode.equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")
    iode.variables.load(f"{SAMPLE_DATA_DIR}/fun.var")
    iode.scalars.load(f"{SAMPLE_DATA_DIR}/fun.scl")

    # Version with exchange in at least 2 equations
    # Set values of endo UY
    UY = iode.variables["UY"]
    UY[40:43] = [650.0, 670.0, 680.0]   # 2000Y1..2002Y1
    iode.variables["UY"] = UY

    # Simulate with exchange UY - XNATY
    iode.model_simulate("2000Y1", "2002Y1", endo_exo_list="UY-XNATY", relax=0.7)

    # Check result
    UY = iode.variables["UY"]
    XNATY = iode.variables["XNATY"]
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

# PANDAS FUNCTIONS
# ----------------

def test_from_frame_timeit():
    import timeit
    import pandas as pd
    import numpy as np

    iode.variables.clear()

    # create the pandas DataFrame
    vars_names = [f"A_{i}" for i in range(0, 10_000)]           # 10_000 variables
    periods_list = [f"{i}Y1" for i in range(1951, 2051)]        # 100 periods
    data = np.arange(len(vars_names) * len(periods_list), dtype=float).reshape(len(vars_names), len(periods_list))
    df = pd.DataFrame(index=vars_names, columns=periods_list, data=data)

    assert df.shape == (10_000, 100)
    assert df.size == 1000_000

    stmt = "iode.variables.from_frame(df)"
    nb_times = 10
    t = timeit.timeit(stmt, globals={"iode": iode, "df": df, "iode.variables": iode.variables}, number=nb_times)
    logging.info(f"{stmt} (shape {df.shape}): {t} ({nb_times} times) -> {t / nb_times} per loop")

def test_to_frame_timeit():
    import timeit
    import pandas as pd
    import numpy as np

    iode.variables.clear()

    # set sample to 100 periods
    iode.variables.sample = "1951Y1:2050Y1"
    assert iode.variables.nb_periods == 100

    # create 10_000 variables
    for i in range(0, 10_000):
        iode.variables[f"A_{i}"] = f"t + {i}"
    assert len(iode.variables) == 10_000

    stmt = "df = iode.variables.to_frame()"
    nb_times = 10
    t = timeit.timeit(stmt, globals={"iode": iode, "iode.variables": iode.variables}, number=nb_times)
    logging.info(f"{stmt} (shape ({len(iode.variables)}, {iode.variables.nb_periods})): "
                 f"{t} ({nb_times} times) -> {t / nb_times} per loop")
