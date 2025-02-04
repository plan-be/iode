from pathlib import Path
from iode import comments, equations, identities, lists, scalars, tables, variables
from iode import SAMPLE_DATA_DIR, Sample, view_workspace
import larray as la


# load IODE objects
comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")
equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")
identities.load(f"{SAMPLE_DATA_DIR}/fun.idt")
lists.load(f"{SAMPLE_DATA_DIR}/fun.lst")
scalars.load(f"{SAMPLE_DATA_DIR}/fun.scl")
tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")
variables.load(f"{SAMPLE_DATA_DIR}/fun.var")

# create subsets
cmt_subset = comments["A*;*_"]
vars_subset = variables["A*", "2000Y1:2010Y1"]

# save subsets
cmt_subset.save("cmt_subset.ac")
vars_subset.save("vars_subset.av")

# create a copy of the variables database
vars_copy = variables.copy()

# will open a viewer showing all IODE databases
view_workspace()

# change some values of vars_copy
vars_copy["AOUC"] = 0.0
vars_copy["AQC", "2000Y1:2010Y1"] = [vars_copy["AQC", period] * 1.1 for period in vars_copy["AQC", "2000Y1:2010Y1"].periods]
vars_copy["BENEF"] = 'BENEF[-1] * 1.05'

# export variables and vars_copy as larray Array objects
baseline = variables.to_array()
variant = vars_copy.to_array()

# compare baseline and variant
la.compare(baseline, variant, names=['baseline', 'variant'])
