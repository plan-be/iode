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
from iode_cython cimport cpp_iode_init, ODE_assign_super_PYIODE

# MAIN
# ----

# Numpy must be initialized before we can use it safely.
import numpy as np
np.import_array()

# get the path to the iode Python installation directory 
# -> constains iode.msg file (to print errors)
installation_dir = str((Path(__file__).parent).resolve())

# initialize SWAP memory (SCR4), create empty workspaces and 
# assign 'super' functions
cpp_iode_init(installation_dir.encode('utf-8'))

# Super fns could be replaced here (optional)
ODE_assign_super_PYIODE() 

# PYIODE API
# ----------

# Included modules by topic
# -------------------------

include "common.pyx"
include "util.pyx"
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