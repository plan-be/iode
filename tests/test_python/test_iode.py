# IODE EXTENSION FOR PYTHON
# =========================
#
#    @header4iode
#
# Unit tests for iode.pyd

import iode
from iode import *

import pytest
import re
import numpy as np
import pandas as pd
import larray as la
import logging

from pathlib import Path

# GLOBALS
SAMPLE_DATA_DIR = Path(iode.SAMPLE_DATA_DIR).absolute()
IODE_OUTPUT_DIR = (Path(__file__).parent.parent / "output").absolute()
IODE_VERBOSE = 1

if not IODE_OUTPUT_DIR.exists():
    IODE_OUTPUT_DIR.mkdir() 

# Equations
# ---------

def test_print_equation():
    iode.equations.load(f"{iode.SAMPLE_DATA_DIR}/fun.eqs")
    eq_ACAF = iode.equations["ACAF"]
    string_eq_ACAF = ("Equation(endogenous = ACAF,\n"
                      "         lec = (ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995),\n"
                      "         method = LSQ,\n"
                      "         sample = 1980Y1:1996Y1,\n"
                      "         block = ACAF,\n"
                      "         tests = corr = 1,\n"
                      "                 dw = 2.32935,\n"
                      "                 fstat = 32.2732,\n"
                      "                 loglik = 83.8075,\n"
                      "                 meany = 0.00818467,\n"
                      "                 r2 = 0.821761,\n"
                      "                 r2adj = 0.796299,\n"
                      "                 ssres = 5.19945e-05,\n"
                      "                 stderr = 0.00192715,\n"
                      "                 stderrp = 23.5458,\n"
                      "                 stdev = 0.0042699,\n"
                      "         date = 12-06-1998)")
    assert str(eq_ACAF) == string_eq_ACAF

# Tables
# ------

def test_table_language():
    table = iode.Table()
    # wrong value for languague
    with pytest.raises(ValueError, match=r"'language': Invalid value 'Spanish'. Expected one of ENGLISH, DUTCH, FRENCH."):
        table.language = "Spanish"

def test_table_content():
    iode.tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")
    # specify list of line titles and list of LEC expressions
    lines_titles = ["GOSG:", "YDTG:", "DTH:", "DTF:", "IT:", "YSSG+COTRES:", "RIDG:", "OCUG:"]
    lines_lecs = ["GOSG", "YDTG", "DTH", "DTF", "IT", "YSSG+COTRES", "RIDG", "OCUG"]
    tables["TABLE_CELL_LECS"] = {"nb_columns": 2, "table_title": "New Table", "lecs_or_vars": lines_lecs, 
                                "lines_titles": lines_titles, "mode": True, "files": True, "date": True}
    
    assert str(tables["TABLE_CELL_LECS"][0]) == "New Table"
    index = tables["TABLE_CELL_LECS"].index("YSSG+COTRES")
    assert str(tables["TABLE_CELL_LECS"][index]) == "('\"YSSG+COTRES:\"', 'YSSG+COTRES')"
    assert str(tables["TABLE_CELL_LECS"][index][0]) == '"YSSG+COTRES:"'
    assert str(tables["TABLE_CELL_LECS"][index][1]) == 'YSSG+COTRES'
    
    table = tables["TABLE_CELL_LECS"]
    assert str(table[0]) == "New Table"
    index = table.index("YSSG+COTRES")
    assert str(table[index]) == "('\"YSSG+COTRES:\"', 'YSSG+COTRES')"
    assert str(table[index][0]) == '"YSSG+COTRES:"'
    assert str(table[index][1]) == 'YSSG+COTRES'

def test_computed_table_extra_files():
    iode.tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")
    iode.variables.load(f"{SAMPLE_DATA_DIR}/fun.var")

    # too long list of files
    sample_dir = Path(SAMPLE_DATA_DIR)
    extra_files = [sample_dir / "fun.av", sample_dir / "fun2.av", 
                   sample_dir / "ref.av", sample_dir / "a.var", sample_dir / "b.var"]
    with pytest.raises(ValueError, match=r"The number of extra files cannot exceed 4"):
        computed_table = iode.tables["C8_1"].compute("2010[1;2]:5", extra_files=extra_files)

    # file does not exist
    extra_files = "file_which_not_exists.var"
    with pytest.raises(ValueError, match=fr"Cannot run 'load'\.\nThe file '.*{re.escape(extra_files)}' does not exist"):
        computed_table = iode.tables["C8_1"].compute("2010[1;2]:5", extra_files=extra_files)

def test_computed_table_nb_decimals():
    iode.tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")
    iode.variables.load(f"{SAMPLE_DATA_DIR}/fun.var")

    # nb decimals must be in range [0, 99]
    with pytest.raises(ValueError, match=r"nb_decimals must be between 0 and 99"):
        computed_table = iode.tables["C8_1"].compute("2010[1;2]:5", nb_decimals=-1)

# Variables
# ---------

def test_from_frame():
    # check that pandas nan are converted to IODE NA
    iode.variables.clear()
    assert len(iode.variables) == 0

    # create the pandas DataFrame
    vars_names = [f"{region}_{code}" for region in ["VLA", "WAL", "BXL"] for code in ["00", "01", "02"]]
    periods_list = [f"{i}Y1" for i in range(1960, 1971)]
    data = np.arange(len(vars_names) * len(periods_list), dtype=float).reshape(len(vars_names), len(periods_list))
    df = pd.DataFrame(index=vars_names, columns=periods_list, data=data)
    # set first and last value to nan
    df.loc["VLA_00", "1960Y1"] = np.nan
    df.loc["BXL_02", "1970Y1"] = np.nan

    # load into the IODE Variables database
    iode.variables.from_frame(df)
    assert len(iode.variables) == 9
    assert str(iode.variables.sample) == '1960Y1:1970Y1'

    # check some valid and NA values
    assert iode.is_NA(iode.variables["VLA_00", "1960Y1"])
    assert iode.variables["VLA_00", "1970Y1"] == 10.0
    assert iode.variables["BXL_02", "1960Y1"] == 88.0
    assert iode.is_NA(iode.variables["BXL_02", "1970Y1"])


def test_from_array():
    # check that LArray nan are converted to IODE NA
    iode.variables.clear()
    assert len(iode.variables) == 0

    regions_axis = la.Axis("region=VLA,WAL,BXL")
    code_axis = la.Axis("code=00..02")
    periods_axis = la.Axis("time=1960Y1..1970Y1")
    array = la.ndtest((regions_axis, code_axis, periods_axis), dtype=float)
    # set first and last value to nan
    array["VLA", "00", "1960Y1"] = np.nan
    array["BXL", "02", "1970Y1"] = np.nan
    
    # load the IODE Variables from the Array object
    iode.variables.from_array(array)
    assert len(iode.variables) == 9
    assert str(iode.variables.sample) == '1960Y1:1970Y1'

    # check some valid and NA values
    assert iode.is_NA(iode.variables["VLA_00", "1960Y1"])
    assert iode.variables["VLA_00", "1970Y1"] == 10.0
    assert iode.variables["BXL_02", "1960Y1"] == 88.0
    assert iode.is_NA(iode.variables["BXL_02", "1970Y1"])

# WRITE
# -----

def test_iode_wrt():

    def iode_wrt_1(filename:str="", dest: iode.WriteFileExt = iode.WriteFileExt.A2M):
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

    iode_wrt_1("", iode.WriteFileExt.DUMMY)
    iode_wrt_1(str(IODE_OUTPUT_DIR / "test_wrt.htm"), iode.WriteFileExt.HTML)
    iode_wrt_1(str(IODE_OUTPUT_DIR / "test_wrt.a2m"), iode.WriteFileExt.A2M)
    iode_wrt_1(str(IODE_OUTPUT_DIR / "test_wrt.rtf"), iode.WriteFileExt.RTF)
    iode_wrt_1(str(IODE_OUTPUT_DIR / "test_wrt.csv"), iode.WriteFileExt.CSV)

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
