# %% [markdown]
# # Getting Started
# 
# The purpose of the present `Getting Started` section is to give a quick overview of the main objects, methods and functions of the Python `iode` library.
# To get a more detailed presentation of all capabilities of `iode`, read the next sections of the tutorial.
#     
# The [API Reference](../api.rst#api-reference) section of the documentation give you the list of all objects, methods and functions with their individual documentation and examples.
# 
# The [Equivalence IODE Report Commands and IODE Python](../equivalence.rst#equivalence-iode-report-commands-and-iode-python) section contains *equivalence tables* between the IODE report syntax and the Python `iode` syntax.

# %% [markdown]
# To use the Python `iode` library, the first thing to do is to import objects and functions you need from it:

# %%
import numpy as np
from iode import (SAMPLE_DATA_DIR, comments, equations, identities, lists, scalars, 
                  tables, variables, EqMethod, Sample)

# %% [markdown]
# To know the version of the `iode` library installed on your machine, type:

# %%
from iode import __version__
__version__

# %% [markdown]
# To get the list of objects and functions available in the `iode` li library, use the Python function `dir()`:

# %%
import iode
dir(iode)

# %% [markdown]
# To print the documentation of an object, method or function in a Python interactive console, use the `help()` function:

# %%
# ---- print documentation of a function or method ----
help(equations.load)

# %% [markdown]
# ## Load IODE objects
# 
# To load IODE objects from a binary file (i.e. with extension `.cmt`, `.eqs`, `.idt`, `.lst`, `.scl`, `.tbl`, `.var`) or from an ASCII file (i.e. with extension `.ac`, `.ae`, `.ai`, `.al`, `.as`, `.at`, `.av`), use the [load()](../_generated/iode.Comments.load.rst#iode.Comments.load) method of the corresponding object. For example:

# %%
# ---- load equations, identities, scalars and variables ----
# Note: test binary and ASCII 'fun' files are located in the 'SAMPLE_DATA_DIR' 
#       directory of the 'iode' package
comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")
equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")
identities.load(f"{SAMPLE_DATA_DIR}/fun.idt")
lists.load(f"{SAMPLE_DATA_DIR}/fun.lst")
scalars.load(f"{SAMPLE_DATA_DIR}/fun.scl")
tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")
variables.load(f"{SAMPLE_DATA_DIR}/fun.var")

# ---- print the number of objects present in the above workspaces ----
len(comments), len(equations), len(identities), len(lists), len(scalars), len(tables), len(variables)

# %% [markdown]
# ## Save IODE objects
# 
# To save the content of a workspace (or a subset of a workspace), use the [save()](../_generated/iode.Variables.save.rst#iode.Variables.save) method:

# %%
# ---- save workspace (or subset) ----
# save the whole workspace
equations.save('equations.eqs')

# save only a subset of the global variables workspace
vars_subset = variables[["ACAF", "ACAG", "AQC", "BQY", "BVY"]]
vars_subset.save('variables_subset.av')    

print("Check content of the variables_subset.av file:\n")
with open("variables_subset.av", "r") as f:
    print(f.read())
print()

# %% [markdown]
# ## Working with workspaces
# 
# To get the list of objects names present in a workspace, use the [names](../_generated/iode.Comments.names.rst#iode.Comments.names) attribute of the workspace. 
# For example:

# %%
# get the list of all IODE lists
lists.names

# %% [markdown]
# To check if a name is present in a workspace, use the `in` operator. 
# For example:

# %%
if 'ENVI' in lists:
    print("The 'ENVI' IODE list exists")
else:
    print("'ENVI' IODE list not found")

# %% [markdown]
# To iterate over names of a workspace, simply use the Python syntax for the *for loop*:

# %%
print("Iterate over all IODE lists names in the Lists workspace:")
for name in lists:
    print(name)

# %% [markdown]
# To get the current used sample for the Variables, use the [sample](../_generated/iode.Variables.sample.rst#iode.Variables.sample) attribute of the [variables](../_generated/iode.Variables.rst#iode.Variables) workspace:

# %%
# current used sample
variables.sample

# %% [markdown]
# ### Get - add - update - delete IODE object(s)
# 
# In a similar way to Python dictionaries, you can [get](../_generated/iode.Equations.__getitem__.rst), [add](../_generated/iode.Equations.__setitem__.rst), [update](../_generated/iode.Equations.__setitem__.rst) and [delete](../_generated/iode.Equations.__delitem__.rst) IODE objects in a workspace using the `[]` operator.
# 
# * To extract an IODE object from a workspace, use the syntax: `my_obj = workspace[name]`.
# * To add an IODE object to a workspace, use the syntax: `workspace[new_name] = new_obj`.
# * To update an IODE object in a workspace, use the syntax: `workspace[name] = new_value`.
# * To delete an IODE object from a workspace, use the syntax: `del workspace[name]`.

# %% [markdown]
# #### Comments

# %% [markdown]
# Add one comment:

# %%
comments["NEW"] = "A new comment"
comments["NEW"]

# %% [markdown]
# Update a comment:

# %%
comments["NEW"] = "New Value"
comments["NEW"]

# %% [markdown]
# Delete a comment:

# %%
comments.get_names("A*")

# %%
del comments["ACAF"]
comments.get_names("A*")

# %% [markdown]
# #### Equations

# %% [markdown]
# Add one equation:

# %%
equations["TEST"] = "TEST := 0"
equations["TEST"]

# %% [markdown]
# Update an equation:

# %%
equations["ACAF"]

# %%
# update only the LEC
equations["ACAF"] = "(ACAF/VAF[-1]) := acaf1 + acaf2 * GOSF[-1] + acaf4 * (TIME=1995)"
equations["ACAF"]

# %%
# update block and sample of a block of equations to estimation (dictionary)
estim_sample = "2000Y1:2010Y1"
block = "ACAF; ACAG; AOUC"
for eq_name in block.split(';'):
    equations[eq_name] = {"sample": estim_sample, "block": block}

(equations["ACAF"].sample, equations["ACAG"].sample, equations["AOUC"].sample)

# %%
(equations["ACAF"].block, equations["ACAG"].block, equations["AOUC"].block)

# %%
# update lec, method, sample and block
equations["ACAF"].lec = "(ACAF/VAF[-1]) := acaf2 * GOSF[-1] + acaf4 * (TIME=1995)"
equations["ACAF"].method = EqMethod.MAX_LIKELIHOOD
# new equation sample is from 1990Y1 to the last year of Variables
equations["ACAF"].sample = "1990Y1:"
equations["ACAF"].block = "ACAF"
equations["ACAF"]

# %% [markdown]
# Delete an equation:

# %%
equations.get_names("A*")

# %%
del equations["ACAF"]
equations.get_names("A*")

# %% [markdown]
# #### Identities

# %% [markdown]
# Add one identity:

# %%
identities["BDY"] = "YN - YK"
identities["BDY"]

# %% [markdown]
# Update an identity:

# %%
identities["AOUC"]

# %%
identities["AOUC"] = '(WCRH / WCRH[1990Y1]) * (VAFF / (VM+VAFF))[-1] + PM * (VM / (VM+VAFF))[-1]'
identities["AOUC"]

# %% [markdown]
# Delete an identity:

# %%
identities.get_names("W*")

# %%
del identities["W"]
identities.get_names("W*")

# %% [markdown]
# #### Lists

# %% [markdown]
# Add one list:

# %%
# --- by passing a string
lists["A_VAR"] = "ACAF;ACAG;AOUC;AOUC_;AQC"
lists["A_VAR"]

# %%
# --- by passing a Python list
b_vars = variables.get_names("B*")
b_vars

# %%
lists["B_VAR"] = b_vars
lists["B_VAR"]

# %% [markdown]
# Update a list:

# %%
# --- by passing a string
lists["A_VAR"] = "ACAF;ACAG;AOUC;AQC"
lists["A_VAR"]

# %%
# --- by passing a Python list
b_y_vars = variables.get_names("B*Y")
b_y_vars

# %%
lists["B_VAR"] = b_y_vars
lists["B_VAR"]

# %% [markdown]
# Delete a list:

# %%
lists.get_names("C*")

# %%
del lists["COPY"]
lists.get_names("C*")

# %% [markdown]
# #### Scalars

# %% [markdown]
# Add one scalar:

# %%
# 1. default relax to 1.0
scalars["a0"] = 0.1
scalars["a0"]

# %%
# 2. value + relax
scalars["a1"] = 0.1, 0.9
scalars["a1"]

# %% [markdown]
# Update a scalar:

# %%
scalars["acaf1"]

# %%
# only update the value
scalars["acaf1"] = 0.8
scalars["acaf1"]

# %%
# update value and relax (tuple)
scalars["acaf2"] = 0.8, 0.9
scalars["acaf2"]

# %%
# update value and relax (list)
scalars["acaf2"] = (0.7, 0.8)
scalars["acaf2"]

# %%
# update value and relax (dictionary)
scalars["acaf3"] = {"relax": 0.9, "value": 0.8}
scalars["acaf3"]

# %%
# update value and/or relax (Scalar object)
# NOTE: the standard deviation (std) cannot be changed manually
scalars["acaf4"]

# %%
scalars["acaf4"].value = 0.8
scalars["acaf4"].relax = 0.9
scalars["acaf4"]

# %% [markdown]
# Delete a scalar:

# %%
scalars.get_names("a*")

# %%
del scalars["acaf4"]
scalars.get_names("a*")

# %% [markdown]
# #### Tables

# %% [markdown]
# Create an add a new table:

# %%
# 1. specify list of line titles and list of LEC expressions
lines_titles = ["GOSG:", "YDTG:", "DTH:", "DTF:", "IT:", "YSSG+COTRES:", "RIDG:", "OCUG:"]
lines_lecs = ["GOSG", "YDTG", "DTH", "DTF", "IT", "YSSG+COTRES", "RIDG", "OCUG"]
tables["TABLE_CELL_LECS"] = {"nb_columns": 2, "table_title": "New Table", "lecs_or_vars": lines_lecs,
                             "lines_titles": lines_titles, "mode": True, "files": True, "date": True}
tables["TABLE_CELL_LECS"]

# %%
# 2. specify list of variables
vars_list = ["GOSG", "YDTG", "DTH", "DTF", "IT", "YSSG", "COTRES", "RIDG", "OCUG", "$ENVI"]
tables["TABLE_VARS"] = {"nb_columns": 2, "table_title": "New Table", "lecs_or_vars": vars_list,
                        "mode": True, "files": True, "date": True}
tables["TABLE_VARS"]

# %% [markdown]
# Update a table:

# %%
tables["TABLE_CELL_LECS"]               

# %%
tables["TABLE_CELL_LECS"].graph_axis

# %%
from iode import TableGraphAxis
# set graph axis type
tables["TABLE_CELL_LECS"].graph_axis = TableGraphAxis.SEMILOG
tables["TABLE_CELL_LECS"].graph_axis

# %%
# get the first line
tables["TABLE_CELL_LECS"][0]

# %%
# get the last line
tables["TABLE_CELL_LECS"][-1]

# %%
# delete last line
del tables["TABLE_CELL_LECS"][-1]
tables["TABLE_CELL_LECS"]

# %%
# get index of line containing YSSG+COTRES
index = tables["TABLE_CELL_LECS"].index("YSSG+COTRES")
index

# %%
tables["TABLE_CELL_LECS"][index]

# %%
# get line type
tables["TABLE_CELL_LECS"][index].line_type

# %%
# get line graph type
tables["TABLE_CELL_LECS"][index].graph_type

# %%
# know if axis is left
tables["TABLE_CELL_LECS"][index].axis_left

# %%
# update cells
# double quotes "    -> STRING cell
# no double quotes   -> LEC cell
tables["TABLE_CELL_LECS"][index] = ('"YSSG:"', 'YSSG')
tables["TABLE_CELL_LECS"][index]

# %%
# insert a new title line surrounded by two separator lines
tables["TABLE_CELL_LECS"].insert(index + 1, '-')
tables["TABLE_CELL_LECS"].insert(index + 2, "New Title")
tables["TABLE_CELL_LECS"].insert(index + 3, '-')
tables["TABLE_CELL_LECS"]

# %%
# append a new sepatator line
tables["TABLE_CELL_LECS"] += '-'
tables["TABLE_CELL_LECS"]

# %% [markdown]
# Delete a table:

# %%
tables.get_names("G*")

# %%
del tables["GFRLEVEL"]
tables.get_names("G*")

# %% [markdown]
# #### Variables

# %% [markdown]
# Get variable:

# %%
# get the value for a single period -> float 
variables["ACAF", "1990Y1"]

# %%
# get the values for all periods -> Variables
variables["ACAF"]

# %%
# get the values for a range of periods -> Variables
variables["ACAF", "1990Y1:2000Y1"]

# %%
# get the values for a list non-contiguous periods -> pandas Series 
variables["ACAF", ["1990Y1", "1995Y1", "2000Y1"]]

# %% [markdown]
# Add one variable:

# %%
# 1) same value for all periods
variables["A0"] = np.nan
variables["A0"]

# %%
# 2) vector (list) containing a specific value for each period
variables["A1"] = list(range(variables.nb_periods))
variables["A1"]                     

# %%
# 3) LEC expression
variables["A2"] = "t + 10"
variables["A2"]                     

# %%
# 4) another variable
variables["A3"] = variables["ACAF"]
variables["A3"]

# %% [markdown]
# Update a variable.
# 
# Set one value of a variable for a specific period:

# %%
variables["ACAG", "1990Y1"]

# %%
variables["ACAG", "1990Y1"] = -28.2
variables["ACAG", "1990Y1"]

# %% [markdown]
# Update all values of a variable:

# %%
variables["ACAF"]                   

# %%
# 1) same value for all periods
variables["ACAF"] = np.nan
variables["ACAF"]                   

# %%
# 2) vector (list) containing a specific value for each period
variables["ACAF"] = list(range(variables.nb_periods))
variables["ACAF"]                   

# %%
# 3) LEC expression
variables["ACAF"] = "t + 10"
variables["ACAF"]                   

# %%
# 4) another variable
variables["ACAG"]

# %%
variables["ACAF"] = variables["ACAG"]
variables["ACAF"]

# %% [markdown]
# Set the values for range of periods:

# %%
# 1) variable(periods) = same value for all periods
variables["ACAF", "1991Y1:1995Y1"] = 0.0
variables["ACAF", "1991Y1:1995Y1"]

# %%
# 2) variable(periods) = vector (list) containing a specific value for each period
variables["ACAF", "1991Y1:1995Y1"] = [1., 2., 3., 4., 5.]
variables["ACAF", "1991Y1:1995Y1"]

# %%
# 3) variable(periods) = LEC expression
variables["ACAF", "1991Y1:1995Y1"] = "t + 10"
variables["ACAF", "1991Y1:1995Y1"]

# %%
# 4) variable(periods) = Variables object
variables["ACAG", "1991Y1:1995Y1"]

# %%
variables["ACAF", "1991Y1:1995Y1"] = variables["ACAG", "1991Y1:1995Y1"]
variables["ACAF", "1991Y1:1995Y1"]

# %% [markdown]
# Set the values for a list of non-contiguous periods:

# %%
values = [1.0, 3.0, 5.0]
variables["ACAF", ["1991Y1", "1993Y1", "1995Y1"]] = values
variables["ACAF", ["1991Y1", "1993Y1", "1995Y1"]]

# %% [markdown]
# Delete a variable:

# %%
variables.get_names("A*")

# %%
del variables["ACAF"]
variables.get_names("A*")

# %% [markdown]
# ### Workspace subsets
# 
# IODE workspaces can contains a lot objects and it can be sometimes easier to work on a subset of the objects present in a workspace. To get a subset of an IODE workspace, a *pattern* can be passed to the `[]` operator. 
# A (sub-)`pattern` is a list of characters representing a group of object names. It includes some special characters which have a special meaning:
# 
# * `*` : any character sequence, even empty
# * `?` : any character (one and only one)
# * `@` : any alphanumerical char [A-Za-z0-9]
# * `&` : any non alphanumerical char
# * `|` : any alphanumeric character or none at the beginning and end of a string 
# * `!` : any non-alphanumeric character or none at the beginning and end of a string 
# * `\` : escape the next character
# 
# The *pattern* can contain sub-patterns, as well as, object names. The sub-patterns and object names are separated by a *separator* character which is either:
# 
# * a whitespace `' '`
# * a comma `,`
# * a semi-colon `;`
# * a tabulation `\t`
# * a newline `\n`
# 
# Note that the *pattern* can contain references to IODE lists which are prefixed with the symbol `$`.
# 
# <div class="alert alert-info">
# 
# **Note**
# 
# When an IODE object is added, updated or deleted from a subset of a workspace, the change is also applied to the global workspace. For example, if an equation is added to the subset of the *equations* workspace, the the change is also applied to the global *equations* database.
# 
# To create an isolate subset of a workspace, use the [copy](../_generated/iode.Equations.copy.rst#iode.Equations.copy) method. This method returns a new workspace in which each object is a copy of the original object from the global workspace. Any change made to the *copied subset* will not be applied to the global workspace. This can be useful for example if you want to save previous values of scalars before estimating an equation or a block of equations and then restore the original values if the estimated values are not satisfying.
# 
# </div>
# 

# %% [markdown]
# #### Comments - Equations - Identities - Lists - Scalars - Tables

# %% [markdown]
# Let's first begin by reloading the example workspace for all IODE types:

# %%
comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")
equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")
identities.load(f"{SAMPLE_DATA_DIR}/fun.idt")
lists.load(f"{SAMPLE_DATA_DIR}/fun.lst")
scalars.load(f"{SAMPLE_DATA_DIR}/fun.scl")
tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")
variables.load(f"{SAMPLE_DATA_DIR}/fun.var")

# %%
# 1) get subset using a pattern
comments_subset = comments["A*"]
comments_subset.names

# %%
# 2) add a comment to the subset
comments_subset["A0"] = "New Comment"
comments_subset["A0"]

# %%
# --> new comment also appears in the global workspace
"A0" in comments, comments["A0"]

# %%
# 3) update a comment in the subset
comments_subset["A0"] = "Updated Comment"
comments_subset["A0"]

# %%
# --> comment is also updated in the global workspace
comments["A0"]

# %%
# delete comment from the subset
del comments_subset["A0"]
comments_subset.names

# %%
# NOTE: the comment has also been deleted from the global database
"A0" in comments

# %%
comments.get_names("A*")

# %% [markdown]
# #### Variables

# %%
# 1) get subset using a pattern
vars_subset = variables["A*"]
vars_subset.names

# %%
# get the values for a specific period -> Variables
variables["A*", "1990Y1"]

# %%
# get the variable values for range of periods -> Variables
variables["A*", "1990Y1:2000Y1"]

# %%
# get the variable values for a list of non-contiguous periods -> pandas DataFrame
variables["A*", ["1990Y1", "1995Y1", "2000Y1"]]

# %%
# 2) add a variable to the subset
vars_subset["A0"] = np.nan
vars_subset["A0"]              

# %%
# --> new variable also appears in the global workspace
"A0" in variables            

# %%
variables["A0"]

# %%
# 3) update a variable in the subset
vars_subset["A0"] = 0.0
vars_subset["A0"]              

# %%
# --> variable is also updated in the global workspace
variables["A0"]                     

# %%
# delete one variable from a subset of the global database
del vars_subset["A0"]
"A0" in vars_subset

# %%
# NOTE: the variable has also been deleted from the global database
"A0" in variables

# %% [markdown]
# It is also possible to work on subset over names and periods:

# %%
# 1) get subset over names and periods -> Variables
vars_subset = variables["A*", "1991Y1:1995Y1"]
vars_subset.names

# %%
vars_subset

# %%
# 2) update a variable in the subset
vars_subset["ACAF"] = 1.0
vars_subset["ACAF"]

# %%
# --> variable is also updated in the global workspace
variables["ACAF", "1991Y1"]

# %%
variables["ACAF", "1995Y1"]

# %% [markdown]
# <div class="alert alert-info">
# 
# **Note**
# 
# Selecting non-contiguous periods will return a pandas object, not a Variables object. This is because the IODE Variables workspace is designed to work with contiguous periods. The pandas object is not a view of the Variables workspace, but a copy of the data. Modifying a value of the pandas object will not affect the Variables workspace.
# 
# </div>
# 

# %%
df_vars_subset = variables["A*", ["1990Y1", "1995Y1", "2000Y1"]]
df_vars_subset

# %% [markdown]
# ## Estimation
# 
# To estimate either one equation or a block of equations, use the [Equation.estimate](../_generated/iode.Equation.estimate.rst#iode.Equation.estimate) method of the equation object.

# %%
help(equations.estimate)

# %% [markdown]
# Let's start by reloading all workspaces:

# %%
comments.load(f"{SAMPLE_DATA_DIR}/fun.cmt")
equations.load(f"{SAMPLE_DATA_DIR}/fun.eqs")
identities.load(f"{SAMPLE_DATA_DIR}/fun.idt")
lists.load(f"{SAMPLE_DATA_DIR}/fun.lst")
scalars.load(f"{SAMPLE_DATA_DIR}/fun.scl")
tables.load(f"{SAMPLE_DATA_DIR}/fun.tbl")
variables.load(f"{SAMPLE_DATA_DIR}/fun.var")

# %% [markdown]
# Example for one equation:

# %%
# ---- estimate coefficients of one equation ----
print(f"ACAF equation LEC: {equations['ACAF'].lec}")
print(f"ACAF equations coefficients (= scalars): {equations['ACAF'].coefficients}")
print(f"ACAF equations variables: {equations['ACAF'].variables}")

# copy the original values of the coefficients into an isolated scalars workspace
# Useful to restore the original values if the estimation process didn't go well
scalars_copy = scalars[equations['ACAF'].coefficients].copy()
print(f"scalars names in scalars_copy: {scalars_copy.names}")

# reset scalars
for name in equations['ACAF'].coefficients:
    scalars[name] = 0., 1.

# estimate the 'ACAF' equation for the periods ranging from '1980Y1' to '1996Y1'
equations.estimate("1980Y1", "1996Y1", "ACAF")
# or equivalently
equations["ACAF"].estimate("1980Y1", "1996Y1")

print(f"Resulting values for the coefficient 'acaf1': {scalars['acaf1']}")
print(f"Resulting values for the coefficient 'acaf2': {scalars['acaf2']}")
print(f"Resulting values for the coefficient 'acaf4': {scalars['acaf4']}")

# %% [markdown]
# Example for a block of equations:

# %%
# ---- estimate a block of equations ----
print(f"ACAF equation LEC: {equations['ACAF'].lec}")
print(f"ACAF equations coefficients (= scalars): {equations['ACAF'].coefficients}")
print(f"ACAF equations variables: {equations['ACAF'].variables}")
print(f"DPUH equation LEC: {equations['DPUH'].lec}")
print(f"DPUH equations coefficients (= scalars): {equations['DPUH'].coefficients}")
print(f"DPUH equations variables: {equations['DPUH'].variables}")

# copy the original values of the coefficients into an isolated scalars workspace
# Useful to restore the original values if the estimation process didn't go well
scalars_names = equations['ACAF'].coefficients + equations['DPUH'].coefficients
scalars_names = list(set(scalars_names))   # removed duplicates
scalars_copy = scalars[scalars_names].copy()
print(f"scalars names in scalars_copy: {scalars_copy.names}")

# reset scalars
for name in equations['ACAF'].coefficients:
    scalars[name] = 0., 1.
for name in equations['DPUH'].coefficients:
    scalars[name] = 0., 1.

# prepare equations (same block and method)
block = "ACAF;DPUH"
for name in block.split(";"):
    equations[name] = {"block": block, "method": "LSQ"}

# estimation the block 'ACAF;DPUH' for the periods ranging from '1980Y1' to '1996Y1'
equations.estimate("1980Y1", "1996Y1", block)

print(f"Resulting values for the coefficient 'acaf1': {scalars['acaf1']}")
print(f"Resulting values for the coefficient 'acaf2': {scalars['acaf2']}")
print(f"Resulting values for the coefficient 'acaf4': {scalars['acaf4']}")
print(f"Resulting values for the coefficient 'dpuh_1': {scalars['dpuh_1']}")
print(f"Resulting values for the coefficient 'dpuh_2': {scalars['dpuh_2']}")

# %% [markdown]
# ## Simulation
# 
# To simulate a model, you must create and initialize an instance of the [Simulation](../_generated/iode.Simulation.rst#iode.Simulation) class:

# %%
from iode import Simulation

help(Simulation)

# %%
from iode import SimulationSort

print(f"possible sort algorithms: {[member.name for member in SimulationSort]}")

# %%
from iode import SimulationInitialization

print(f"possible initialization methods: {[member.name for member in SimulationInitialization]}")

# %%
# ---- simulation ----
print("Simulation() methods and properties:")
for name in dir(Simulation):
    if not name.startswith('_'):
        print(name)

# %%
# create and initialize a Simulation instance
simu = Simulation(sort_algorithm=SimulationSort.BOTH, initialization_method=SimulationInitialization.TM1)
print(f"Simulation convergence_threshold: {simu.convergence_threshold}")
print(f"Simulation relax: {simu.relax}")
print(f"Simulation max_nb_iterations: {simu.max_nb_iterations}")
print(f"Simulation sort_algorithm: {simu.sort_algorithm}")
print(f"Simulation initialization_method: {simu.initialization_method}")
print(f"Simulation debug: {simu.debug}")
print(f"Simulation nb_passes: {simu.nb_passes}")
print(f"Simulation debug_newton: {simu.debug_newton}")

# %% [markdown]
# To run the simulation, call the [Simulation.model_simulate](../_generated/iode.Simulation.model_simulate.rst#iode.Simulation.model_simulate) method:

# %%
help(Simulation.model_simulate)

# %%
print(f"exogenous variable 'UY': {equations['UY'].lec}")
print(f"endogenous variable 'XNATY': {identities['XNATY']}")
# reset values of exogenous variable
variables["UY", "2000Y1:2015Y1"] = 0.0

print(f"exogenous variable 'UY' before simulation:\n{variables['UY', '1998Y1:2005Y1']}")

# run the simulation for the periods range '2000Y1' to '2015Y1'
simu.model_simulate("2000Y1", "2015Y1")

print(f"exogenous variable 'UY' after simulation:\n{variables['UY', '1998Y1:2005Y1']}")

# %% [markdown]
# ## Import/Export IODE Variables workspace from/to numpy ndarray
# 
# To export the content of the `Variables` workspace (or a subset of it) to a numpy ndarray object, use the [to_ndarray](../_generated/iode.Variables.to_ndarray.rst#iode.Variables.to_ndarray) method.

# %%
len(variables)

# %%
variables.sample

# %%
variables.nb_periods

# %% [markdown]
# Export the whole Variables workspace to a numpy ndarray:

# %%
# export the whole Variables workspace to a numpy ndarray (394 variables x 56 periods)
data = variables.to_ndarray()
data.shape

# %%
data[5, 40]

# %%
variables.i[5, 40]

# %% [markdown]
# Export a subset of names:

# %%
# export a subset of names
vars_subset = variables["A*"]
vars_subset.names


# %%
vars_subset

# %%
data = vars_subset.to_ndarray()
data.shape

# %%
# values of the 'ACAF' variable
data[0]

# %%
# values of the 'AQC' variable
data[-1]

# %% [markdown]
# Export a subset of names and periods:

# %%
# export a subset of names and periods
vars_subset = variables["A*", "2000Y1:2010Y1"]
vars_subset

# %%
data = vars_subset.to_ndarray()
data.shape

# %%
data

# %% [markdown]
# To update a subset of the Variables workspace, use the [from_ndarray](../_generated/iode.Variables.from_ndarray.rst#iode.Variables.from_ndarray) method.

# %%
vars_names = variables.get_names("A*")
vars_names

# %%
first_period = "2000Y1"
last_periods = "2010Y1"
sample = Sample(first_period, last_periods)
nb_periods = sample.nb_periods
nb_periods

# %%
# save original values to restore them later
original_values = variables["A*", "2000Y1:2010Y1"].to_ndarray()
original_values

# %%
# create the numpy ndarray containing the values to copy into the Variables database
data = np.zeros((len(vars_names), nb_periods), dtype=float)
for i in range(len(vars_names)):
    for j in range(nb_periods):
        data[i, j] = i * nb_periods + j
data

# %%
variables["A*", "2000Y1:2010Y1"]

# %%
# copy the numpy ndarray into the Variables database (overriding the existing values)
variables.from_ndarray(data, vars_names, first_period, last_periods)
variables["A*", "2000Y1:2010Y1"]

# %% [markdown]
# If you already work on the subset you whish to update the values, you can skip to specify the value for the parameters *vars_names*, *first_period* and *last_period*:

# %%
vars_subset = variables["A*", "2000Y1:2010Y1"]
vars_subset.from_ndarray(original_values)
vars_subset

# %% [markdown]
# ## Import/Export IODE workspaces from/to pandas Series and DataFrame
# 
# To import / export the content of the `comments`, `identities` and `lists` workspaces from/to a pandas Series object, use the [from_series()](../_generated/iode.Comments.from_series.rst#iode.Comments.from_series) and [to_series()](../_generated/iode.Comments.to_series.rst#iode.Comments.to_series) methods. 
# 
# Alternatively, you can use the [series](../_generated/iode.Comments.series.rst#iode.Comments.series) property to export the content of the `comments`, `identities` and `lists` workspaces to a pandas Series object.

# %%
import pandas as pd

# ---- to pandas Series ----
# See Comments/Identities/Lists.to_series and 
#     Comments/Identities/Lists.series

series_cmt = comments.to_series()
print(f"Comments as pandas Series:\n{series_cmt.info()}")
print()

series_idt = identities.to_series()
print(f"Identities as pandas Series:\n{series_idt.info()}")
print()

series_lst = lists.to_series()
print(f"Lists as pandas Series:\n{series_lst.info()}")

# Alternatively

series_cmt = comments.series
series_idt = identities.series
series_lst = lists.series

# ---- from pandas Series ----
# See Comments/Identities/Lists.from_series

comments.from_series(series_cmt)
identities.from_series(series_idt)
lists.from_series(series_lst)

# %% [markdown]
# To import / export the content of the `equations`, `scalars` and `variables` workspaces from/to a pandas DataFrame object, use the [from_frame()](../_generated/iode.Variables.from_frame.rst#iode.Variables.from_frame) and [to_frame()](../_generated/iode.Variables.to_frame.rst#iode.Variables.to_frame) methods. 
# 
# Alternatively, you can use the [df](../_generated/iode.Variables.df.rst#iode.Variables.df) property to export the content of the `equations`, `scalars` and `variables` workspaces to a pandas DataFrame object.

# %%
# ---- to pandas DataFrame ----
# See Equations/Scalars/Variables.to_frame and
#     Equations/Scalars/Variables.df

df_eqs = equations.to_frame()
print(f"Equations as pandas DataFrame:\n{df_eqs.info()}")
print()

df_scl = scalars.to_frame()
print(f"Scalars as pandas DataFrame:\n{df_scl.info()}")
print()

df_vars = variables.to_frame()
print(f"Variables as pandas DataFrame:\n{df_vars.info()}")

# Alternatively

df_eqs = equations.df
df_scl = scalars.df
df_vars = variables.df

# ---- from pandas DataFrame ----
# See Equations/Scalars/Variables.from_frame

equations.from_frame(df_eqs)
scalars.from_frame(df_scl)
variables.from_frame(df_vars)

# %% [markdown]
# It possible to export a subset of the IODE databases:

# %%
# export a subset of the IODE Variables database as a pandas DataFrame
vars_subset = variables["A*;*_", "2000Y1:2010Y1"]
vars_subset

# %%
df = vars_subset.to_frame()
df.shape

# %%
df

# %% [markdown]
# ## Import/Export the Variables workspace from/to LArray Array
# 
# To import / export the content of the `variables` workspaces from/to a LArray Array object, use the [from_array()](../_generated/iode.Variables.from_array.rst#iode.Variables.from_array) and [to_array()](../_generated/iode.Variables.to_array.rst#iode.Variables.to_array) methods:

# %%
import larray as la

# ---- to LArray Array ----
# See Variables.to_array

arr_vars = variables.to_array()
print(f"Variables as LArray Array:\n{arr_vars.info}")

# ---- from LArray array ----
# See Variables.from_array

variables.from_array(arr_vars)

# %% [markdown]
# It possible to export a subset of the IODE databases:

# %%
# export a subset of the IODE Variables database as a LArray Array
vars_subset = variables["A*;*_", "2000Y1:2010Y1"]
vars_subset

# %%
array = vars_subset.to_array()
array.shape

# %%
array

# %% [markdown]
# ## Execute IODE report commands/files
# 
# To run an IODE command, use the [execute_command()](../_generated/iode.execute_command.rst#iode.execute_command) function:

# %%
from iode import execute_command

# ---- execute IODE commands ----
print("Execute IODE commands (useful for IODE functions and commands not yet ported to Python):")
execute_command("$WsClearVar")
execute_command("$WsSample 2000Y1 2005Y1")
execute_command("$DataCalcVar A t+1")
execute_command("$DataCalcVar B t-1")
execute_command("$DataCalcVar C A/B")
execute_command("$DataCalcVar D grt A")
execute_command("$WsSaveVar test_var.av")
with open("test_var.av", "r") as f:
    print(f.read())

# %% [markdown]
# To run an entire IODE report (i.e. file with the '.rep' extension), call the [execute_report()](../_generated/iode.execute_report.rst#iode.execute_report) function:

# %%
from iode import execute_report

# ---- execute IODE reports ----
print("Execute an IODE report -> execute_report()")
with open("create_var.rep", "w") as f:
    f.write("$WsClearVar\n")
    f.write("$WsSample 2000Y1 2005Y1\n")
    f.write("$DataCalcVar %1% t+1 \n")
    f.write("$DataCalcVar %2% t-1 \n")
    f.write("$DataCalcVar %3% %1%/%2%\n")
    f.write("$DataCalcVar %4% grt %1% \n")
    f.write("$WsSaveVar test_var.av\n")

execute_report("create_var.rep", ["A", "B", "C", "D"])

with open("test_var.av", "r") as f:
    print(f.read())



