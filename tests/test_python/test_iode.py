# IODE EXTENSION FOR PYTHON
# =========================
#
#    @header4iode
#
# Unit tests for pyd

from iode import *

import pytest
import re
import numpy as np
import pandas as pd
import larray as la
import logging

from pathlib import Path

# GLOBALS
SAMPLE_DATA_DIR = Path(SAMPLE_DATA_DIR).absolute()
IODE_OUTPUT_DIR = (Path(__file__).parent.parent / "output").absolute()
IODE_VERBOSE = 1

if not IODE_OUTPUT_DIR.exists():
    IODE_OUTPUT_DIR.mkdir() 

# Iode Databases
# --------------

def test_check_same_names():
    comments.clear()
    comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")

    update_cmt = {"ACAF": "updated ACAF", "AOUC": "updated AOUC"}
    # works fine
    comments["ACAF, AOUC"] = update_cmt

    # missing comments in the right hand side
    with pytest.raises(KeyError, match=r"Missing value for the comments: 'ACAG, AQC'"):
        comments["ACAF, ACAG, AOUC, AQC"] = update_cmt

    # too many items in the right hand side
    update_cmt = {"ACAF": "updated ACAF", "ACAG": "updated ACAG", "AOUC": "updated AOUC", "AQC": "updated AQC"}
    with pytest.raises(KeyError, match=r"Unexpected comments in the right-hand side: 'ACAF, AOUC'"):
        comments["ACAG, AQC"] = update_cmt

    update_cmt = ["updated ACAF", "updated ACAG", "updated AOUC", "updated AQC"]
    with pytest.raises(ValueError, match=r"Cannot add/update values for comments for the selection key 'ACAF, ACAG, AOUC'.\n"
                                         r"4 values has been passed while the selection key 'ACAF, ACAG, AOUC' "
                                         r"represents 3 comments."):
        comments["ACAF, ACAG, AOUC"] = update_cmt

def test_subset_type():
    comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")
    equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")
    identities.load(f"{SAMPLE_DATA_DIR}/fun.idt")
    lists.load(f"{SAMPLE_DATA_DIR}/fun.lst")
    scalars.load(f"{SAMPLE_DATA_DIR}/fun.scl")
    tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")
    variables.load(f"{SAMPLE_DATA_DIR}/fun.var")

    # check that the subset type is correct
    assert isinstance(comments["A*"], Comments)
    assert isinstance(equations["A*"], Equations)
    assert isinstance(identities["A*"], Identities)
    assert isinstance(lists["C*"], Lists)
    assert isinstance(scalars["a*"], Scalars)
    assert isinstance(tables["C*"], Tables)
    assert isinstance(variables["A*"], Variables)

def test_type_database_copy():
    comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")
    equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")
    identities.load(f"{SAMPLE_DATA_DIR}/fun.idt")
    lists.load(f"{SAMPLE_DATA_DIR}/fun.lst")
    scalars.load(f"{SAMPLE_DATA_DIR}/fun.scl")
    tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")
    variables.load(f"{SAMPLE_DATA_DIR}/fun.var")

    assert isinstance(comments["A*"].copy(), Comments)
    assert isinstance(equations["A*"].copy(), Equations)
    assert isinstance(identities["A*"].copy(), Identities)
    assert isinstance(lists["C*"].copy(), Lists)
    assert isinstance(scalars["a*"].copy(), Scalars)
    assert isinstance(tables["C*"].copy(), Tables)
    assert isinstance(variables["A*"].copy(), Variables)

def test_database_getitem_returned_type():
    comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")
    equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")
    identities.load(f"{SAMPLE_DATA_DIR}/fun.idt")
    lists.load(f"{SAMPLE_DATA_DIR}/fun.lst")
    scalars.load(f"{SAMPLE_DATA_DIR}/fun.scl")
    tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")
    variables.load(f"{SAMPLE_DATA_DIR}/fun.var")

    assert isinstance(comments["ACAF"], str)
    assert isinstance(equations["ACAF"], Equation)
    assert isinstance(identities["AOUC"], Identity)
    assert isinstance(lists["COPY"], list)
    assert isinstance(scalars["acaf1"], Scalar)
    assert isinstance(tables["C8_1"], Table)
    assert isinstance(variables["ACAF"], Variables) 
    assert isinstance(variables["ACAF", "1990Y1:2000Y1"], Variables) 
    assert isinstance(variables["ACAF", "2000Y1"], float) 

def test_type_copy_iode_objects():
    equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")
    identities.load(f"{SAMPLE_DATA_DIR}/fun.idt")
    scalars.load(f"{SAMPLE_DATA_DIR}/fun.scl")
    tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")

    eq_ACAF = equations["ACAF"]
    assert isinstance(eq_ACAF.copy(), Equation)
    idt_AOUC = identities["AOUC"]
    assert isinstance(idt_AOUC.copy(), Identity)
    scl_acaf1 = scalars["acaf1"]
    assert isinstance(scl_acaf1.copy(), Scalar)
    table_C8_1 = tables["C8_1"]
    assert isinstance(table_C8_1.copy(), Table)

# Equations
# ---------

def test_print_equation():
    equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")
    eq_ACAF = equations["ACAF"]
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
    table = Table()
    # wrong value for languague
    with pytest.raises(ValueError, match=r"'language': Invalid value 'Spanish'. Expected one of ENGLISH, DUTCH, FRENCH."):
        table.language = "Spanish"

def test_table_content():
    tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")
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
    tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")
    variables.load(f"{SAMPLE_DATA_DIR}/fun.var")

    # too long list of files
    sample_dir = Path(SAMPLE_DATA_DIR)
    extra_files = [sample_dir / "fun.av", sample_dir / "fun2.av", 
                   sample_dir / "ref.av", sample_dir / "a.var", sample_dir / "b.var"]
    with pytest.raises(ValueError, match=r"The number of extra files cannot exceed 4"):
        computed_table = tables["C8_1"].compute("2010[1;2]:5", extra_files=extra_files)

    # file does not exist
    extra_files = "file_which_not_exists.var"
    with pytest.raises(ValueError, match=fr"Cannot run 'load'\.\nThe file '.*{re.escape(extra_files)}' does not exist"):
        computed_table = tables["C8_1"].compute("2010[1;2]:5", extra_files=extra_files)

def test_computed_table_nb_decimals():
    tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")
    variables.load(f"{SAMPLE_DATA_DIR}/fun.var")

    # nb decimals must be in range [0, 99]
    with pytest.raises(ValueError, match=r"nb_decimals must be between 0 and 99"):
        computed_table = tables["C8_1"].compute("2010[1;2]:5", nb_decimals=-1)

def test_computed_table_NA_values():
    tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")
    variables.load(f"{SAMPLE_DATA_DIR}/fun.var")
    computed_table = tables["C8_1"].compute("(1960;1961/1960):5")
    col_names = computed_table.columns
    line_names = computed_table.lines

    # ---- getitem ----
    assert line_names[1] == "Stock de capital"
    assert col_names[0] == "60"
    assert np.isnan(computed_table[1, 0])
    assert col_names[1] == "61/60"
    assert np.isnan(computed_table[1, 1])
    assert col_names[2] == "61"
    assert round(computed_table[1, 2], 4) == 1965.3674
    assert col_names[3] == "62/61"
    assert round(computed_table[1, 3], 4) == 4.722

    # ---- setitem ----
    assert line_names[0] == "Output potentiel"
    assert round(computed_table[0, 2], 4) == 1873.7079
    # check that np.nan is converted to IODE NA when setting a value in the table
    with pytest.raises(ValueError, match=r"An IODE NA \(NaN\) value is not accepted to edit a cell of a computed table"):
        computed_table[0, 2] = np.nan

# Tables
# ------

def test_table_getitem_returned_type():
    from iode.objects.table import TableLine, TableCell
    
    tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")
    table = tables["ANAPRIX"]

    assert isinstance(table.divider, TableLine)
    assert isinstance(table[4], TableLine)
    assert isinstance(table[4][0], TableCell)

# Variables
# ---------

def test_variables_setitem():
    # ==== select periods as 'first_period:' and 'last_period:' ====
    variables.clear()
    variables.load(f"{SAMPLE_DATA_DIR}/fun.var")

    vars_subset = variables["A*;*_", "1990Y1:"]
    assert str(vars_subset.sample) == "1990Y1:2015Y1"
    vars_subset = variables["A*;*_", ":2000Y1"]
    assert str(vars_subset.sample) == "1960Y1:2000Y1"

    # ==== setitem  ====
    names = ["AOUC", "AOUC_", "AQC"]
    other_names = ["BENEF", "BQY", "BRUGP"]
    
    # ==== 1) periods = unique period ====
    variables.clear()
    variables.load(f"{SAMPLE_DATA_DIR}/fun.var")

    vars_subset = variables["A*", "1990Y1":"1992Y1"].copy()

    # **** 1.a) values = float ****
    variables[names, "1991Y1"] = 0.0
    assert variables["AOUC", "1991Y1"] == 0.0
    assert variables["AOUC_", "1991Y1"] == 0.0
    assert variables["AQC", "1991Y1"] == 0.0

    assert variables["ACAF", "1990Y1"] == vars_subset["ACAF", "1990Y1"]
    assert variables["ACAF", "1992Y1"] == vars_subset["ACAF", "1992Y1"]
    assert variables["AOUC", "1990Y1"] == vars_subset["AOUC", "1990Y1"]
    assert variables["AOUC", "1992Y1"] == vars_subset["AOUC", "1992Y1"]
    assert variables["AOUC_", "1990Y1"] == vars_subset["AOUC_", "1990Y1"]
    assert variables["AOUC_", "1992Y1"] == vars_subset["AOUC_", "1992Y1"]
    assert variables["AQC", "1990Y1"] == vars_subset["AQC", "1990Y1"]
    assert variables["AQC", "1992Y1"] == vars_subset["AQC", "1992Y1"]

    # **** 1.b) values = LEC expression ****
    variables[names, "1991Y1"] = "t + 10"
    assert variables["AOUC", "1991Y1"] == 41.0
    assert variables["AOUC_", "1991Y1"] == 41.0
    assert variables["AQC", "1991Y1"] == 41.0

    assert variables["ACAF", "1990Y1"] == vars_subset["ACAF", "1990Y1"]
    assert variables["ACAF", "1992Y1"] == vars_subset["ACAF", "1992Y1"]
    assert variables["AOUC", "1990Y1"] == vars_subset["AOUC", "1990Y1"]
    assert variables["AOUC", "1992Y1"] == vars_subset["AOUC", "1992Y1"]
    assert variables["AOUC_", "1990Y1"] == vars_subset["AOUC_", "1990Y1"]
    assert variables["AOUC_", "1992Y1"] == vars_subset["AOUC_", "1992Y1"]
    assert variables["AQC", "1990Y1"] == vars_subset["AQC", "1990Y1"]
    assert variables["AQC", "1992Y1"] == vars_subset["AQC", "1992Y1"]

    # **** 1.c) values = list(float) ****
    variables[names, "1991Y1"] = [1.0, 2.0, 3.0]
    assert variables["AOUC", "1991Y1"] == 1.0
    assert variables["AOUC_", "1991Y1"] == 2.0
    assert variables["AQC", "1991Y1"] == 3.0

    assert variables["ACAF", "1990Y1"] == vars_subset["ACAF", "1990Y1"]
    assert variables["ACAF", "1992Y1"] == vars_subset["ACAF", "1992Y1"]
    assert variables["AOUC", "1990Y1"] == vars_subset["AOUC", "1990Y1"]
    assert variables["AOUC", "1992Y1"] == vars_subset["AOUC", "1992Y1"]
    assert variables["AOUC_", "1990Y1"] == vars_subset["AOUC_", "1990Y1"]
    assert variables["AOUC_", "1992Y1"] == vars_subset["AOUC_", "1992Y1"]
    assert variables["AQC", "1990Y1"] == vars_subset["AQC", "1990Y1"]
    assert variables["AQC", "1992Y1"] == vars_subset["AQC", "1992Y1"]

    # **** 1.d) values = numpy array ****
    variables[names, "1991Y1"] = np.asarray([4.0, 5.0, 6.0])
    assert variables["AOUC", "1991Y1"] == 4.0
    assert variables["AOUC_", "1991Y1"] == 5.0
    assert variables["AQC", "1991Y1"] == 6.0

    assert variables["ACAF", "1990Y1"] == vars_subset["ACAF", "1990Y1"]
    assert variables["ACAF", "1992Y1"] == vars_subset["ACAF", "1992Y1"]
    assert variables["AOUC", "1990Y1"] == vars_subset["AOUC", "1990Y1"]
    assert variables["AOUC", "1992Y1"] == vars_subset["AOUC", "1992Y1"]
    assert variables["AOUC_", "1990Y1"] == vars_subset["AOUC_", "1990Y1"]
    assert variables["AOUC_", "1992Y1"] == vars_subset["AOUC_", "1992Y1"]
    assert variables["AQC", "1990Y1"] == vars_subset["AQC", "1990Y1"]
    assert variables["AQC", "1992Y1"] == vars_subset["AQC", "1992Y1"]

    # **** 1.e) values = Variables object ****
    values = variables[other_names, "1991Y1"]
    for name, other_name in zip(names, other_names):
        variables[name, "1991Y1"] = values[other_name, "1991Y1"]
    assert variables["AOUC", "1991Y1"] == variables["BENEF", "1991Y1"]
    assert variables["AOUC_", "1991Y1"] == variables["BQY", "1991Y1"]
    assert variables["AQC", "1991Y1"] == variables["BRUGP", "1991Y1"]

    assert variables["ACAF", "1990Y1"] == vars_subset["ACAF", "1990Y1"]
    assert variables["ACAF", "1992Y1"] == vars_subset["ACAF", "1992Y1"]
    assert variables["AOUC", "1990Y1"] == vars_subset["AOUC", "1990Y1"]
    assert variables["AOUC", "1992Y1"] == vars_subset["AOUC", "1992Y1"]
    assert variables["AOUC_", "1990Y1"] == vars_subset["AOUC_", "1990Y1"]
    assert variables["AOUC_", "1992Y1"] == vars_subset["AOUC_", "1992Y1"]
    assert variables["AQC", "1990Y1"] == vars_subset["AQC", "1990Y1"]
    assert variables["AQC", "1992Y1"] == vars_subset["AQC", "1992Y1"]

    # ==== 2) periods = tuple(from_period, to_period) ====
    variables.clear()
    variables.load(f"{SAMPLE_DATA_DIR}/fun.var")
    vars_subset = variables["A*", "1990Y1":"1996Y1"].copy()
    periods = ("1991Y1", "1995Y1")

    # **** 2.a) values = float ****
    variables[names, periods] = 0.0
    assert variables["AOUC", "1991Y1"] == 0.0
    assert variables["AOUC", "1993Y1"] == 0.0
    assert variables["AOUC", "1995Y1"] == 0.0
    assert variables["AOUC_", "1991Y1"] == 0.0
    assert variables["AOUC_", "1993Y1"] == 0.0
    assert variables["AOUC_", "1995Y1"] == 0.0
    assert variables["AQC", "1991Y1"] == 0.0
    assert variables["AQC", "1993Y1"] == 0.0
    assert variables["AQC", "1995Y1"] == 0.0
    
    assert variables["ACAF", "1990Y1"] == vars_subset["ACAF", "1990Y1"]
    assert variables["ACAF", "1996Y1"] == vars_subset["ACAF", "1996Y1"]
    assert variables["ACAG", "1990Y1"] == vars_subset["ACAG", "1990Y1"]
    assert variables["ACAG", "1996Y1"] == vars_subset["ACAG", "1996Y1"]
    assert variables["AOUC", "1990Y1"] == vars_subset["AOUC", "1990Y1"]
    assert variables["AOUC", "1996Y1"] == vars_subset["AOUC", "1996Y1"]
    assert variables["AOUC_", "1990Y1"] == vars_subset["AOUC_", "1990Y1"]
    assert variables["AOUC_", "1996Y1"] == vars_subset["AOUC_", "1996Y1"]
    assert variables["AQC", "1990Y1"] == vars_subset["AQC", "1990Y1"]
    assert variables["AQC", "1996Y1"] == vars_subset["AQC", "1996Y1"]

    # **** 2.b) values = LEC expression ****
    variables[names, periods] = "t + 10"
    assert variables["AOUC", "1991Y1"] == 41.0
    assert variables["AOUC", "1993Y1"] == 43.0
    assert variables["AOUC", "1995Y1"] == 45.0
    assert variables["AOUC_", "1991Y1"] == 41.0
    assert variables["AOUC_", "1993Y1"] == 43.0
    assert variables["AOUC_", "1995Y1"] == 45.0
    assert variables["AQC", "1991Y1"] == 41.0
    assert variables["AQC", "1993Y1"] == 43.0
    assert variables["AQC", "1995Y1"] == 45.0

    assert variables["ACAF", "1990Y1"] == vars_subset["ACAF", "1990Y1"]
    assert variables["ACAF", "1996Y1"] == vars_subset["ACAF", "1996Y1"]
    assert variables["ACAG", "1990Y1"] == vars_subset["ACAG", "1990Y1"]
    assert variables["ACAG", "1996Y1"] == vars_subset["ACAG", "1996Y1"]
    assert variables["AOUC", "1990Y1"] == vars_subset["AOUC", "1990Y1"]
    assert variables["AOUC", "1996Y1"] == vars_subset["AOUC", "1996Y1"]
    assert variables["AOUC_", "1990Y1"] == vars_subset["AOUC_", "1990Y1"]
    assert variables["AOUC_", "1996Y1"] == vars_subset["AOUC_", "1996Y1"]
    assert variables["AQC", "1990Y1"] == vars_subset["AQC", "1990Y1"]
    assert variables["AQC", "1996Y1"] == vars_subset["AQC", "1996Y1"]

    # **** 2.c) values = list(list(float)) ****
    values = [[1.0, 2.0, 3.0, 4.0, 5.0], 
              [10.0, 20.0, 30.0, 40.0, 50.0], 
              [100.0, 200.0, 300.0, 400.0, 500.0]]
    variables[names, periods] = values
    assert variables["AOUC", "1991Y1"] == 1.0
    assert variables["AOUC", "1993Y1"] == 3.0
    assert variables["AOUC", "1995Y1"] == 5.0
    assert variables["AOUC_", "1991Y1"] == 10.0
    assert variables["AOUC_", "1993Y1"] == 30.0
    assert variables["AOUC_", "1995Y1"] == 50.0
    assert variables["AQC", "1991Y1"] == 100.0
    assert variables["AQC", "1993Y1"] == 300.0
    assert variables["AQC", "1995Y1"] == 500.0
    
    assert variables["ACAF", "1990Y1"] == vars_subset["ACAF", "1990Y1"]
    assert variables["ACAF", "1996Y1"] == vars_subset["ACAF", "1996Y1"]
    assert variables["ACAG", "1990Y1"] == vars_subset["ACAG", "1990Y1"]
    assert variables["ACAG", "1996Y1"] == vars_subset["ACAG", "1996Y1"]
    assert variables["AOUC", "1990Y1"] == vars_subset["AOUC", "1990Y1"]
    assert variables["AOUC", "1996Y1"] == vars_subset["AOUC", "1996Y1"]
    assert variables["AOUC_", "1990Y1"] == vars_subset["AOUC_", "1990Y1"]
    assert variables["AOUC_", "1996Y1"] == vars_subset["AOUC_", "1996Y1"]
    assert variables["AQC", "1990Y1"] == vars_subset["AQC", "1990Y1"]
    assert variables["AQC", "1996Y1"] == vars_subset["AQC", "1996Y1"]

    # **** 2.d) values = numpy array ****
    values = np.asarray(values) * 10.0
    variables[names, periods] = values
    assert variables["AOUC", "1991Y1"] == 10.0
    assert variables["AOUC", "1993Y1"] == 30.0
    assert variables["AOUC", "1995Y1"] == 50.0
    assert variables["AOUC_", "1991Y1"] == 100.0
    assert variables["AOUC_", "1993Y1"] == 300.0
    assert variables["AOUC_", "1995Y1"] == 500.0
    assert variables["AQC", "1991Y1"] == 1000.0
    assert variables["AQC", "1993Y1"] == 3000.0
    assert variables["AQC", "1995Y1"] == 5000.0
    
    assert variables["ACAF", "1990Y1"] == vars_subset["ACAF", "1990Y1"]
    assert variables["ACAF", "1996Y1"] == vars_subset["ACAF", "1996Y1"]
    assert variables["ACAG", "1990Y1"] == vars_subset["ACAG", "1990Y1"]
    assert variables["ACAG", "1996Y1"] == vars_subset["ACAG", "1996Y1"]
    assert variables["AOUC", "1990Y1"] == vars_subset["AOUC", "1990Y1"]
    assert variables["AOUC", "1996Y1"] == vars_subset["AOUC", "1996Y1"]
    assert variables["AOUC_", "1990Y1"] == vars_subset["AOUC_", "1990Y1"]
    assert variables["AOUC_", "1996Y1"] == vars_subset["AOUC_", "1996Y1"]
    assert variables["AQC", "1990Y1"] == vars_subset["AQC", "1990Y1"]
    assert variables["AQC", "1996Y1"] == vars_subset["AQC", "1996Y1"]

    # **** 2.e) values = Variables object ****
    values = variables[other_names, periods]
    for name, other_name in zip(names, other_names):
        variables[name, periods] = values[other_name, periods]
    assert variables["AOUC", "1991Y1"] == variables["BENEF", "1991Y1"]
    assert variables["AOUC", "1993Y1"] == variables["BENEF", "1993Y1"]
    assert variables["AOUC", "1995Y1"] == variables["BENEF", "1995Y1"]
    assert variables["AOUC_", "1991Y1"] == variables["BQY", "1991Y1"]
    assert variables["AOUC_", "1993Y1"] == variables["BQY", "1993Y1"]
    assert variables["AOUC_", "1995Y1"] == variables["BQY", "1995Y1"]
    assert variables["AQC", "1991Y1"] == variables["BRUGP", "1991Y1"]
    assert variables["AQC", "1993Y1"] == variables["BRUGP", "1993Y1"]
    assert variables["AQC", "1995Y1"] == variables["BRUGP", "1995Y1"]
    
    assert variables["ACAF", "1990Y1"] == vars_subset["ACAF", "1990Y1"]
    assert variables["ACAF", "1996Y1"] == vars_subset["ACAF", "1996Y1"]
    assert variables["ACAG", "1990Y1"] == vars_subset["ACAG", "1990Y1"]
    assert variables["ACAG", "1996Y1"] == vars_subset["ACAG", "1996Y1"]
    assert variables["AOUC", "1990Y1"] == vars_subset["AOUC", "1990Y1"]
    assert variables["AOUC", "1996Y1"] == vars_subset["AOUC", "1996Y1"]
    assert variables["AOUC_", "1990Y1"] == vars_subset["AOUC_", "1990Y1"]
    assert variables["AOUC_", "1996Y1"] == vars_subset["AOUC_", "1996Y1"]
    assert variables["AQC", "1990Y1"] == vars_subset["AQC", "1990Y1"]
    assert variables["AQC", "1996Y1"] == vars_subset["AQC", "1996Y1"]

    # ==== 3) periods = slice ====
    variables.clear()
    variables.load(f"{SAMPLE_DATA_DIR}/fun.var")
    vars_subset = variables["A*", "1990Y1":"1996Y1"].copy()

    # **** 3.a) values = float **** 
    variables[names, "1991Y1":"1995Y1"] = 0.0
    assert variables["AOUC", "1991Y1"] == 0.0
    assert variables["AOUC", "1993Y1"] == 0.0
    assert variables["AOUC", "1995Y1"] == 0.0
    assert variables["AOUC_", "1991Y1"] == 0.0
    assert variables["AOUC_", "1993Y1"] == 0.0
    assert variables["AOUC_", "1995Y1"] == 0.0
    assert variables["AQC", "1991Y1"] == 0.0
    assert variables["AQC", "1993Y1"] == 0.0
    assert variables["AQC", "1995Y1"] == 0.0

    assert variables["ACAF", "1990Y1"] == vars_subset["ACAF", "1990Y1"]
    assert variables["ACAF", "1996Y1"] == vars_subset["ACAF", "1996Y1"]
    assert variables["ACAG", "1990Y1"] == vars_subset["ACAG", "1990Y1"]
    assert variables["ACAG", "1996Y1"] == vars_subset["ACAG", "1996Y1"]
    assert variables["AOUC", "1990Y1"] == vars_subset["AOUC", "1990Y1"]
    assert variables["AOUC", "1996Y1"] == vars_subset["AOUC", "1996Y1"]
    assert variables["AOUC_", "1990Y1"] == vars_subset["AOUC_", "1990Y1"]
    assert variables["AOUC_", "1996Y1"] == vars_subset["AOUC_", "1996Y1"]
    assert variables["AQC", "1990Y1"] == vars_subset["AQC", "1990Y1"]
    assert variables["AQC", "1996Y1"] == vars_subset["AQC", "1996Y1"]

    # **** 3.b) values = LEC expression ****
    variables[names, "1991Y1":"1995Y1"] = "t + 10"
    assert variables["AOUC", "1991Y1"] == 41.0
    assert variables["AOUC", "1993Y1"] == 43.0
    assert variables["AOUC", "1995Y1"] == 45.0
    assert variables["AOUC_", "1991Y1"] == 41.0
    assert variables["AOUC_", "1993Y1"] == 43.0
    assert variables["AOUC_", "1995Y1"] == 45.0
    assert variables["AQC", "1991Y1"] == 41.0
    assert variables["AQC", "1993Y1"] == 43.0
    assert variables["AQC", "1995Y1"] == 45.0

    assert variables["ACAF", "1990Y1"] == vars_subset["ACAF", "1990Y1"]
    assert variables["ACAF", "1996Y1"] == vars_subset["ACAF", "1996Y1"]
    assert variables["ACAG", "1990Y1"] == vars_subset["ACAG", "1990Y1"]
    assert variables["ACAG", "1996Y1"] == vars_subset["ACAG", "1996Y1"]
    assert variables["AOUC", "1990Y1"] == vars_subset["AOUC", "1990Y1"]
    assert variables["AOUC", "1996Y1"] == vars_subset["AOUC", "1996Y1"]
    assert variables["AOUC_", "1990Y1"] == vars_subset["AOUC_", "1990Y1"]
    assert variables["AOUC_", "1996Y1"] == vars_subset["AOUC_", "1996Y1"]
    assert variables["AQC", "1990Y1"] == vars_subset["AQC", "1990Y1"]
    assert variables["AQC", "1996Y1"] == vars_subset["AQC", "1996Y1"]

    # **** 3.c) values = list(list(float)) ****
    values = [[1.0, 2.0, 3.0, 4.0, 5.0], 
              [10.0, 20.0, 30.0, 40.0, 50.0], 
              [100.0, 200.0, 300.0, 400.0, 500.0]]
    variables[names, "1991Y1":"1995Y1"] = values
    assert variables["AOUC", "1991Y1"] == 1.0
    assert variables["AOUC", "1992Y1"] == 2.0
    assert variables["AOUC", "1993Y1"] == 3.0
    assert variables["AOUC", "1994Y1"] == 4.0
    assert variables["AOUC", "1995Y1"] == 5.0
    assert variables["AOUC_", "1991Y1"] == 10.0
    assert variables["AOUC_", "1992Y1"] == 20.0
    assert variables["AOUC_", "1993Y1"] == 30.0
    assert variables["AOUC_", "1994Y1"] == 40.0
    assert variables["AOUC_", "1995Y1"] == 50.0
    assert variables["AQC", "1991Y1"] == 100.0
    assert variables["AQC", "1992Y1"] == 200.0
    assert variables["AQC", "1993Y1"] == 300.0
    assert variables["AQC", "1994Y1"] == 400.0
    assert variables["AQC", "1995Y1"] == 500.0

    assert variables["ACAF", "1990Y1"] == vars_subset["ACAF", "1990Y1"]
    assert variables["ACAF", "1996Y1"] == vars_subset["ACAF", "1996Y1"]
    assert variables["ACAG", "1990Y1"] == vars_subset["ACAG", "1990Y1"]
    assert variables["ACAG", "1996Y1"] == vars_subset["ACAG", "1996Y1"]
    assert variables["AOUC", "1990Y1"] == vars_subset["AOUC", "1990Y1"]
    assert variables["AOUC", "1996Y1"] == vars_subset["AOUC", "1996Y1"]
    assert variables["AOUC_", "1990Y1"] == vars_subset["AOUC_", "1990Y1"]
    assert variables["AOUC_", "1996Y1"] == vars_subset["AOUC_", "1996Y1"]
    assert variables["AQC", "1990Y1"] == vars_subset["AQC", "1990Y1"]
    assert variables["AQC", "1996Y1"] == vars_subset["AQC", "1996Y1"]

    # **** 3.d) values = numpy array
    values = np.asarray(values) * 10.0
    variables[names, "1991Y1":"1995Y1"] = values
    assert variables["AOUC", "1991Y1"] == 10.0
    assert variables["AOUC", "1992Y1"] == 20.0
    assert variables["AOUC", "1993Y1"] == 30.0
    assert variables["AOUC", "1994Y1"] == 40.0
    assert variables["AOUC", "1995Y1"] == 50.0
    assert variables["AOUC_", "1991Y1"] == 100.0
    assert variables["AOUC_", "1992Y1"] == 200.0
    assert variables["AOUC_", "1993Y1"] == 300.0
    assert variables["AOUC_", "1994Y1"] == 400.0
    assert variables["AOUC_", "1995Y1"] == 500.0
    assert variables["AQC", "1991Y1"] == 1000.0
    assert variables["AQC", "1992Y1"] == 2000.0
    assert variables["AQC", "1993Y1"] == 3000.0
    assert variables["AQC", "1994Y1"] == 4000.0
    assert variables["AQC", "1995Y1"] == 5000.0

    assert variables["ACAF", "1990Y1"] == vars_subset["ACAF", "1990Y1"]
    assert variables["ACAF", "1996Y1"] == vars_subset["ACAF", "1996Y1"]
    assert variables["ACAG", "1990Y1"] == vars_subset["ACAG", "1990Y1"]
    assert variables["ACAG", "1996Y1"] == vars_subset["ACAG", "1996Y1"]
    assert variables["AOUC", "1990Y1"] == vars_subset["AOUC", "1990Y1"]
    assert variables["AOUC", "1996Y1"] == vars_subset["AOUC", "1996Y1"]
    assert variables["AOUC_", "1990Y1"] == vars_subset["AOUC_", "1990Y1"]
    assert variables["AOUC_", "1996Y1"] == vars_subset["AOUC_", "1996Y1"]
    assert variables["AQC", "1990Y1"] == vars_subset["AQC", "1990Y1"]
    assert variables["AQC", "1996Y1"] == vars_subset["AQC", "1996Y1"]

    # **** 3.e) values = Variables object ****
    values = variables[other_names, "1991Y1":"1995Y1"]
    for name, other_name in zip(names, other_names):
        variables[name, "1991Y1":"1995Y1"] = values[other_name, "1991Y1":"1995Y1"]
    assert variables["AOUC", "1991Y1"] == variables["BENEF", "1991Y1"]
    assert variables["AOUC", "1992Y1"] == variables["BENEF", "1992Y1"]
    assert variables["AOUC", "1993Y1"] == variables["BENEF", "1993Y1"]
    assert variables["AOUC", "1994Y1"] == variables["BENEF", "1994Y1"]
    assert variables["AOUC", "1995Y1"] == variables["BENEF", "1995Y1"]
    assert variables["AOUC_", "1991Y1"] == variables["BQY", "1991Y1"]
    assert variables["AOUC_", "1992Y1"] == variables["BQY", "1992Y1"]
    assert variables["AOUC_", "1993Y1"] == variables["BQY", "1993Y1"]
    assert variables["AOUC_", "1994Y1"] == variables["BQY", "1994Y1"]
    assert variables["AOUC_", "1995Y1"] == variables["BQY", "1995Y1"]
    assert variables["AQC", "1991Y1"] == variables["BRUGP", "1991Y1"]
    assert variables["AQC", "1992Y1"] == variables["BRUGP", "1992Y1"]
    assert variables["AQC", "1993Y1"] == variables["BRUGP", "1993Y1"]
    assert variables["AQC", "1994Y1"] == variables["BRUGP", "1994Y1"]
    assert variables["AQC", "1995Y1"] == variables["BRUGP", "1995Y1"]

    assert variables["ACAF", "1990Y1"] == vars_subset["ACAF", "1990Y1"]
    assert variables["ACAF", "1996Y1"] == vars_subset["ACAF", "1996Y1"]
    assert variables["ACAG", "1990Y1"] == vars_subset["ACAG", "1990Y1"]
    assert variables["ACAG", "1996Y1"] == vars_subset["ACAG", "1996Y1"]
    assert variables["AOUC", "1990Y1"] == vars_subset["AOUC", "1990Y1"]
    assert variables["AOUC", "1996Y1"] == vars_subset["AOUC", "1996Y1"]
    assert variables["AOUC_", "1990Y1"] == vars_subset["AOUC_", "1990Y1"]
    assert variables["AOUC_", "1996Y1"] == vars_subset["AOUC_", "1996Y1"]
    assert variables["AQC", "1990Y1"] == vars_subset["AQC", "1990Y1"]
    assert variables["AQC", "1996Y1"] == vars_subset["AQC", "1996Y1"]

    # ==== make subset of a subset and test if modifications propagate ====
    variables.clear()
    variables.load(f"{SAMPLE_DATA_DIR}/fun.var")

    # a) create subset of a subset
    copy_ACAG = variables["ACAG"].copy()
    vars_subset = variables["A*;*_", "1990Y1:2010Y1"]
    assert str(vars_subset.sample) == "1990Y1:2010Y1"
    assert vars_subset["ACAG", "1990Y1"] == copy_ACAG["ACAG", "1990Y1"]
    assert vars_subset["ACAG", "2000Y1"] == copy_ACAG["ACAG", "2000Y1"]
    assert vars_subset["ACAG", "2010Y1"] == copy_ACAG["ACAG", "2010Y1"]
    vars_subset_subset = vars_subset["A*", "2000Y1:"]
    assert str(vars_subset_subset.sample) == "2000Y1:2010Y1"
    assert vars_subset_subset["ACAG", "2000Y1"] == copy_ACAG["ACAG", "2000Y1"]
    assert vars_subset_subset["ACAG", "2010Y1"] == copy_ACAG["ACAG", "2010Y1"]

    # b) modify subset of a subset and check if modifications propagate
    vars_subset_subset["ACAG", "2000Y1"] = 100.0
    assert variables["ACAG", "2000Y1"] == 100.0
    assert vars_subset["ACAG", "2000Y1"] == 100.0
    assert vars_subset_subset["ACAG", "2000Y1"] == 100.0
    vars_subset["ACAG", "2001Y1"] = 200.0
    assert variables["ACAG", "2001Y1"] == 200.0
    assert vars_subset["ACAG", "2001Y1"] == 200.0
    assert vars_subset_subset["ACAG", "2001Y1"] == 200.0
    variables["ACAG", "2002Y1"] = 300.0
    assert variables["ACAG", "2002Y1"] == 300.0
    assert vars_subset["ACAG", "2002Y1"] == 300.0
    assert vars_subset_subset["ACAG", "2002Y1"] == 300.0

    # c) make subset of subset to be a copy and check if modifications do not propagate
    variables.clear()
    variables.load(f"{SAMPLE_DATA_DIR}/fun.var")
    vars_subset = variables["A*;*_", "1990Y1:2010Y1"]
    assert str(vars_subset.sample) == "1990Y1:2010Y1"
    assert not vars_subset.is_detached
    vars_subset_subset = vars_subset["A*", "2000Y1:"].copy()
    assert str(vars_subset_subset.sample) == "2000Y1:2010Y1"
    assert vars_subset_subset.is_detached

    vars_subset_subset["ACAG", "2000Y1"] = 100.0
    assert variables["ACAG", "2000Y1"] == copy_ACAG["ACAG", "2000Y1"]
    assert vars_subset["ACAG", "2000Y1"] == copy_ACAG["ACAG", "2000Y1"]
    assert vars_subset_subset["ACAG", "2000Y1"] == 100.0
    vars_subset["ACAG", "2001Y1"] = 200.0
    assert variables["ACAG", "2001Y1"] == 200.0
    assert vars_subset["ACAG", "2001Y1"] == 200.0
    assert vars_subset_subset["ACAG", "2001Y1"] == copy_ACAG["ACAG", "2001Y1"]
    variables["ACAG", "2002Y1"] = 300.0
    assert variables["ACAG", "2002Y1"] == 300.0
    assert vars_subset["ACAG", "2002Y1"] == 300.0
    assert vars_subset_subset["ACAG", "2002Y1"] == copy_ACAG["ACAG", "2002Y1"]

def test_variables_numpy_1D():
    variables.clear()
    variables.load(f"{SAMPLE_DATA_DIR}/fun.var")

    # exporting a subset representing a single variable returns 
    # a 1D numpy ndarray
    array = variables["ACAF", "2000Y1:2010Y1"].to_numpy()
    assert array.ndim == 1
    assert array.shape == (11,)

    array *= 1.1 
    variables.from_numpy(array, "ACAF", "2000Y1:2010Y1")
    assert all(variables["ACAF", "2000Y1:2010Y1"].to_numpy() == array)

def test_variables_from_frame():
    # check that pandas nan are converted to IODE NA
    variables.clear()
    assert len(variables) == 0

    # create the pandas DataFrame
    vars_names = [f"{region}_{code}" for region in ["VLA", "WAL", "BXL"] for code in ["00", "01", "02"]]
    periods_list = [f"{i}Y1" for i in range(1960, 1971)]
    data = np.arange(len(vars_names) * len(periods_list), dtype=float).reshape(len(vars_names), len(periods_list))
    df = pd.DataFrame(index=vars_names, columns=periods_list, data=data)
    # set first and last value to nan
    df.loc["VLA_00", "1960Y1"] = np.nan
    df.loc["BXL_02", "1970Y1"] = np.nan

    # load into the IODE Variables database
    variables.from_frame(df)
    assert len(variables) == 9
    assert str(variables.sample) == '1960Y1:1970Y1'

    # check some valid and NA values
    assert is_NA(variables["VLA_00", "1960Y1"])
    assert variables["VLA_00", "1970Y1"] == 10.0
    assert variables["BXL_02", "1960Y1"] == 88.0
    assert is_NA(variables["BXL_02", "1970Y1"])

def test_variables_from_array():
    # check that LArray nan are converted to IODE NA
    variables.clear()
    assert len(variables) == 0

    regions_axis = la.Axis("region=VLA,WAL,BXL")
    code_axis = la.Axis("code=00..02")
    periods_axis = la.Axis("time=1960Y1..1970Y1")
    array = la.ndtest((regions_axis, code_axis, periods_axis), dtype=float)
    # set first and last value to nan
    array["VLA", "00", "1960Y1"] = np.nan
    array["BXL", "02", "1970Y1"] = np.nan
    
    # load the IODE Variables from the Array object
    variables.from_array(array)
    assert len(variables) == 9
    assert str(variables.sample) == '1960Y1:1970Y1'

    # check some valid and NA values
    assert is_NA(variables["VLA_00", "1960Y1"])
    assert variables["VLA_00", "1970Y1"] == 10.0
    assert variables["BXL_02", "1960Y1"] == 88.0
    assert is_NA(variables["BXL_02", "1970Y1"])


# Estimation
# ----------

def test_estimation():
    from iode.iode_cython import EditAndEstimateEquations

    comments.clear()
    equations.clear()
    identities.clear()
    lists.clear()
    scalars.clear()
    tables.clear()
    variables.clear()

    variables.load(f"{SAMPLE_DATA_DIR}/fun.var")

    equations["ACAF"] = Equation("ACAF", "(ACAF/VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995)")
    equations["DPUH"] = Equation("DPUH", "dln(DPUH/DPUHO) := dpuh_1 + dpuh_2 * dln(IHU/PI5) + dln(PC)")

    estimation = EditAndEstimateEquations("1980Y1", "1996Y1")
    estimation.block = "ACAF;DPUH", "ACAF"
    estimation.estimate()

    with pytest.warns(RuntimeWarning, match="No observed values found for 'ACAG'"):
        estimation.get_observed_values("ACAG")
    
    with pytest.warns(RuntimeWarning, match="No fitted values found for 'ACAG'"):
        estimation.get_fitted_values("ACAG")

    with pytest.warns(RuntimeWarning, match="No residual values found for 'ACAG'"):
        estimation.get_residual_values("ACAG")


    variables["VAF"] = 0.0
    equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")
    eq_ACAF = equations["ACAF"]
    eq_ACAG = equations["ACAG"]

    estimation = EditAndEstimateEquations("1980Y1", "1996Y1")
    estimation.block = "ACAF;DPUH", "ACAF"
    with pytest.warns(RuntimeWarning, match=r"Could not estimate equation\(s\) ACAF;DPUH\n"
                                            r"Estimation : NaN Generated"):
        estimation.estimate()

    with pytest.warns(RuntimeWarning, match=r"Could not estimate equation\(s\) ACAF from "
                                            r"'1980Y1' to '1996Y1'\n"
                                            r"Estimation : NaN Generated"):
        eq_ACAF.estimate("1980Y1", "1996Y1")

    with pytest.warns(RuntimeWarning, match=r"Could not estimate equation\(s\) ACAG from "
                                            r"'1980Y1' to '1996Y1'\nNo scalars in your system\n"
                                            r"Estimation : No current estimation"):
        eq_ACAG.estimate("1980Y1", "1996Y1")

    with pytest.warns(RuntimeWarning, match=r"Could not estimate equation\(s\) ACAF from "
                                            r"'1980Y1' to '1996Y1'\n"
                                            r"Estimation : NaN Generated"):
        equations.estimate("1980Y1", "1996Y1", "ACAF")


# Simulation
# ----------

def test_simulation():
    comments.clear()
    equations.clear()
    identities.clear()
    lists.clear()
    scalars.clear()
    tables.clear()
    variables.clear()

    equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")
    lists.load(f"{SAMPLE_DATA_DIR}/fun.lst")
    scalars.load(f"{SAMPLE_DATA_DIR}/fun.scl")
    variables.load(f"{SAMPLE_DATA_DIR}/fun.var")

    simu = Simulation()
    simu.convergence_threshold = 0.01
    simu.max_nb_iterations = 100
    simu.debug = True
    simu.relax = 1.0
    simu.initialization_method = 'TM1'

    with pytest.warns(RuntimeWarning, match=r"Cannot simulate SCC:\nPre-recursive list \"_PRE\" not found!\n" 
                                            r"Recursive list \"_INTER\" not found!\n"
                                            r"Post-recursive list \"_POST\" not found!"):
        simu.model_simulate_SCC("1960Y1", "2015Y1", "_PRE", "_INTER", "_POST")
    
    simu.model_calculate_SCC(100, "_PRE", "_INTER", "_POST")
    with pytest.warns(RuntimeWarning, match=r"PMAB : becomes unavailable at 1960Y1"):
        simu.model_simulate_SCC("1960Y1", "2015Y1", "_PRE", "_INTER", "_POST")
    
    # Test simulation: divergence (max nb iterations = 2)
    simu = Simulation()
    simu.convergence_threshold = 0.01
    simu.max_nb_iterations = 2
    simu.debug = True
    simu.relax = 1.0
    simu.initialization_method = 'TM1'

    with pytest.warns(RuntimeWarning, match=r"Could not simulate the model: the simulation "
                                            r"did not converged"):
        simu.model_simulate("2000Y1", "2010Y1")


# WRITE
# -----

def test_iode_wrt():

    def iode_wrt_1(filename:str="", dest: WriteFileExt = WriteFileExt.A2M):
        w_dest(filename, dest)
        
        w_print(".sep |\n")
        w_print(".esc ~\n")
        
        w_print_pg_header("IODE")
        w_print_pg_footer("- page %d - ")
        
        w_print_tit(1, "Mon titre")
        
        w_print_enum(1)
        w_print("This is an enum paragraph\n\n")
        w_print("This is a normal paragraph\n\n")
        
        w_print(".tb\n")
        w_print(".tborder 1\n")
        w_print(".theader\n")
        w_print("| Syntax | Description\n")
        w_print(".tbody\n")
        w_print("| ~cint L_errno       | Last error number during LEC compilation\n")
        w_print("| ~cchar* L_error()   | Returns a static buffer containing the last LEC compilation error message.\n")
        w_print(".te\n")
            
        w_print(".gb 16.00 10.00\n")
        w_print(".ggrid TMM\n")
        w_print(".galign L\n")
        w_print(".gbox 0 b 0\n")
        w_print(".gtitle Equation ACAF : observed and fitted values\n")
        w_print(".gty L \"(ACAF/VAF[-1])  : observed\" 1980Y1  0.011412042  0.016028202  0.011903394  0.012051363  0.010215556  0.010582964  0.0090882893  0.009277778  0.0082268494  0.0051589358  0.0066405193  0.0068489061  0.0075258742  0.0082727193  0.0019340143  -0.0029850522  0.0069569681 \n")
        w_print(".gty L \"(ACAF/VAF[-1])  : fitted\" 1980Y1  0.012562124  0.012491075  0.012526504  0.011687035  0.011060337  0.010403968  0.0095320575  0.0090522427  0.008714914  0.0076539375  0.0065561227  0.006355248  0.0064942167  0.0062763884  0.0062678674  -0.0029850522  0.0044903364 \n")
        w_print(".ge\n")
        
        w_print_tit(1, "Mon titre 1")
        w_print_par(1, "Mon para level 1 (éàç)")
        w_print(f"Second para 1\n\n")
        w_print_par(2, "Mon para level 2 (âêî)\n")
        w_print(f"Second para level 2\n")
        w_print_tit(2, "Title 2")
        w_print_enum(2, "Mon enum level 2 (âêî)")
        w_print_enum(2, "enum2 suite")
        w_print_enum(2, "enum2 suite")
        
        w_close()

    iode_wrt_1("", WriteFileExt.DUMMY)
    iode_wrt_1(str(IODE_OUTPUT_DIR / "test_wrt.htm"), WriteFileExt.HTML)
    iode_wrt_1(str(IODE_OUTPUT_DIR / "test_wrt.a2m"), WriteFileExt.A2M)
    iode_wrt_1(str(IODE_OUTPUT_DIR / "test_wrt.rtf"), WriteFileExt.RTF)
    iode_wrt_1(str(IODE_OUTPUT_DIR / "test_wrt.csv"), WriteFileExt.CSV)

# PANDAS FUNCTIONS
# ----------------

def test_from_frame_timeit():
    import iode
    import timeit

    variables.clear()

    # create the pandas DataFrame
    vars_names = [f"A_{i}" for i in range(0, 10_000)]           # 10_000 variables
    periods_list = [f"{i}Y1" for i in range(1951, 2051)]        # 100 periods
    data = np.arange(len(vars_names) * len(periods_list), dtype=float).reshape(len(vars_names), len(periods_list))
    df = pd.DataFrame(index=vars_names, columns=periods_list, data=data)

    assert df.shape == (10_000, 100)
    assert df.size == 1000_000

    stmt = "variables.from_frame(df)"
    nb_times = 10
    t = timeit.timeit(stmt, globals={"iode": iode, "df": df, "variables": variables}, number=nb_times)
    logging.info(f"{stmt} (shape {df.shape}): {t} ({nb_times} times) -> {t / nb_times} per loop")

def test_to_frame_timeit():
    import iode
    import timeit

    variables.clear()

    # set sample to 100 periods
    variables.sample = "1951Y1:2050Y1"
    assert variables.nb_periods == 100

    # create 10_000 variables
    for i in range(0, 10_000):
        variables[f"A_{i}"] = f"t + {i}"
    assert len(variables) == 10_000

    stmt = "df = variables.to_frame()"
    nb_times = 10
    t = timeit.timeit(stmt, globals={"iode": iode, "variables": variables}, number=nb_times)
    logging.info(f"{stmt} (shape ({len(variables)}, {variables.nb_periods})): "
                 f"{t} ({nb_times} times) -> {t / nb_times} per loop")
