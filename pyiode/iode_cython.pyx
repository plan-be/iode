#  IODE EXTENSION FOR PYTHON
#  =========================
#  
#     @header4iode
#  
#  PYIODE API
#  ==========
#  
#  
#  Interface IODE - Python - IODE
#  ------------------------------
#  During a PYIODE session, IODE data are grouped in workspaces, one by object type (variables, scalars, equations...) 
#  and stored in memory using a proprietary memory management system (IODE-SWAP).
#  
#  IODE functions operate inside IODE-SWAP memory. 
#  Exchanges between IODE objects (in IODE-SWAP) and python objects (larray, numpy or pandas objects in python memory) 
#  are made possible via interface functions.
   
#  cython: binding=True, language_level=3, embedsignature=True

# distutils: language = c++
from pathlib import Path
from iode_cython cimport cpp_iode_init
from iode_cython cimport python_assign_super

# MAIN
# ----

# Numpy must be initialized before we can use it safely.
import numpy as np

# "cimport" is used to import special compile-time information
# about the numpy module (this is stored in a file numpy.pxd which is
# distributed with Numpy).
# Here we've used the name "cnp" to make it easier to understand what
# comes from the cimported module and what comes from the imported module,
# however you can use the same name for both if you wish.
cimport numpy as cnp
cnp.import_array()

# initialize SWAP memory (SCR4), create empty workspaces and 
# assign 'super' functions
cpp_iode_init()

# Super C functions are replaced here
include "super.pyx"
python_assign_super() 

# starting from numpy 2.2, the representation of NumPy scalars (via __repr__) has changed.
# Scalars are now printed as np.float64(3.0) rather than just 3.0.
# However, one can use np.set_printoptions(legacy="1.25") to get the old behavior
# see link https://numpy.org/doc/stable/release/2.0.0-notes.html#representation-of-numpy-scalars-changed 
try:
    np.set_printoptions(legacy="1.25")
except TypeError:
    pass

# PYIODE API
# ----------

# Included modules by topic
# -------------------------

include "common.pyx"
include "util.pyx"
include "a2m_print.pyx"
include "cli.pyx"
include "lec.pyx"
include "iode_database/extra_files.pyx"

include "time/period.pyx"
include "time/sample.pyx"

include "objects/equation.pyx"
include "objects/identity.pyx"
include "objects/scalar.pyx"
include "objects/table.pyx"

include "computed_table/computed_table.pyx"

include "iode_database/abstract_database.pyx"
include "iode_database/comments_database.pyx"
include "iode_database/equations_database.pyx"
include "iode_database/identities_database.pyx"
include "iode_database/lists_database.pyx"
include "iode_database/scalars_database.pyx"
include "iode_database/tables_database.pyx"
include "iode_database/variables_database.pyx"

include "compute/estimation.pyx"
include "compute/simulation.pyx"

include "reports.pyx"
include "wrt.pyx"
include "tests/tests.pyx"
