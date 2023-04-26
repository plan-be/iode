<IODE: python module>
IODE: python module
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
..sep |
..esc ~
..ignore ¦¡

&TI Introduction
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
The python module for IODE implements part of the IODE API. 
The aim is to implement a python layer for all IODE functionalities so that IODE can be driven directly from a python session.

&TI Python version & iode.pyd
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Each version of python has its own IODE module ~ciode.pyd~C. They are stored in 
different subdirectories of the IODE main directory (usually ~cc:\iode~C). For example: ~c.\py39, .\py310~C...
By default, the ~ciode.pyd~C module for python 3.9 is thus ~cc:/iode/py39/iode.pyd~C. For python 3.10: ~cc:/iode/py310/python.pyd~C.


&TI Memory management
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
During a python session, IODE data are grouped in workspaces, one by object type (variables, scalars, equations...) 
and stored in IODE memory using a proprietary memory management system called IODE-SWAP. IODE functions operate ~binside~B that memory.
 
Exchanges between IODE objects (in IODE-SWAP) and python objects (larray, numpy or pandas objects in python memory) 
are made possible via interface functions described below like ~cget_eqs()~C, ~cset_eqs()~C or ~clarray_to_ws()~C.

&TI Starting a PYIODE session
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
There are two ways to use the IODE module in the python environment.

&IT Method 1: copy iode.pyd in your working directory
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

That method consists simply in copying ~ciode.pyd~C into the working directory. 
This is the simplest method but it requires to know the version of 
python in use (3.9, 3.10...?). Moreover, ~ciode.pyd~C has to be copied with each new version of IODE and/or python. 
 
&IT Method 2: add the iode.pyd directory to the system path
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
It simply consists in adding to the system "path" the directory where the iode.pyd module is located (depending on the 
current version of python). 
This path can (partly) be calculated in python. Here is how to do it: 

&CO
  ## Import the sys module
  import sys
  
  ## Get the current python version and determine the directory of iode.pyd. 
  ## It is assumed here that the IODE installation directory is ~cc:/iode~C
  iode_pyd_dir = f"c:/iode/py3{sys.version_info[1]}"
  
  ## Add the directory to the system "path" 
  sys.path.append(iode_pyd_dir)

  ## Import iode module
  import iode
&TX


&TI Groups of functions
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&EN <Workspace management        >  
&EN <Object management           >  
&EN <Larray / IODE conversions   >  
&EN <Pandas / IODE conversions   >  
&EN <Sample management           >  
&EN <LEC executions              >  
&EN <Identities execution        >  
&EN <Estimation                  >  
&EN <Simulation                  >  
&EN <Reports                     >  
&EN <Printing object definitions >  
&EN <General writing functions   >  
&EN <Utilities                   >  
&EN <Utilities using C data types>  
&EN <Examples                    >  
>


<Workspace management>
Workspace management
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
&IT Workspace content

..tb
| ~bSyntax                                 | ~bDescription  
| ~cws_content(pattern, objtype) ->> List[str]       | Return the names of all objects of a given type, satisfying a pattern specification  
| ~cws_content_cmt(pattern) ->> List[str]            | Return the list of comment names corresponding to the given pattern
| ~cws_content_eqs(pattern) ->> List[str]            | Return the list of equation names corresponding to the given pattern
| ~cws_content_idt(pattern) ->> List[str]            | Return the list of identity names corresponding to the given pattern
| ~cws_content_lst(pattern) ->> List[str]            | Return the list of list names corresponding to the given pattern
| ~cws_content_scl(pattern) ->> List[str]            | Return the list of scalar names corresponding to the given pattern
| ~cws_content_tbl(pattern) ->> List[str]            | Return the list of table names corresponding to the given pattern
| ~cws_content_var(pattern) ->> List[str]            | Return the list of variable names corresponding to the given pattern
..te

&IT Clear workspaces                 
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
..tb
| ~bSyntax                                 | ~bDescription  
| ~cws_clear(filetype: int)                          | Clear WS of the given filetype (K_CMT..K_VAR)
| ~cws_clear_cmt()                                   | Clear the comment WS
| ~cws_clear_eqs()                                   | Clear the equation WS
| ~cws_clear_idt()                                   | Clear the identity WS
| ~cws_clear_lst()                                   | Clear the list WS
| ~cws_clear_scl()                                   | Clear the scalar WS
| ~cws_clear_tbl()                                   | Clear the table WS
| ~cws_clear_var()                                   | Clear the varaible WS
| ~cws_clear_all()                                   | Clear all WS           
..te

&IT Load workspaces                  
..tb
| ~bSyntax                                 | ~bDescription  
| ~cws_load(filename: str, filetype: int) ->> int | Load an IODE file and return the number of read objects
| ~cws_load_cmt(filename: str) ->> int            | Load a comment file and return the number of read objects
| ~cws_load_eqs(filename: str) ->> int            | Load a equation file and return the number of read objects
| ~cws_load_idt(filename: str) ->> int            | Load a identity file and return the number of read objects
| ~cws_load_lst(filename: str) ->> int            | Load a list file and return the number of read objects
| ~cws_load_scl(filename: str) ->> int            | Load a scalar file and return the number of read objects
| ~cws_load_tbl(filename: str) ->> int            | Load a table file and return the number of read objects
| ~cws_load_var(filename: str) ->> int            | Load a variable file and return the number of read objects
..te

&IT Save workspaces                  
..tb
| ~bSyntax                                 | ~bDescription  
| ~cws_save(filename: str, filetype: int)           | Save the current IODE workspace of a given type
| ~cws_save_cmt(filename: str)                      | Save the current workspace of comments
| ~cws_save_eqs(filename: str)                      | Save the current workspace of equations
| ~cws_save_idt(filename: str)                      | Save the current workspace of identities
| ~cws_save_lst(filename: str)                      | Save the current workspace of lists
| ~cws_save_scl(filename: str)                      | Save the current workspace of scalars
| ~cws_save_tbl(filename: str)                      | Save the current workspace of tables
| ~cws_save_var(filename: str)                      | Save the current workspace of variables
..te

&IT Change of periodicites
..tb
| ~bSyntax                                 | ~bDescription  
| ~cws_ltoh_flow(filename: str, varlist)                         | Convert flow variables with low periodicities into higher periodicities
| ~cws_ltoh_stock(filename: str, varlist, method: int = LTOH_CS) | Convert stock variables with low periodicities into higher periodicities
| ~cws_htol_sum(filename: str, varlist, method: int)             | Convert high periodicities variables into lower periodicities by adding the sub period values
| ~cws_htol_mean(filename: str, varlist, method: int)            | Convert high periodicities variables into lower periodicities by taking the averages of sub period values
| ~cws_htol_last(filename: str, varlist, method: int)            | Convert high periodicities variables into lower periodicities by taking the last value of each sub period
..te
>

<Object management>
Object management
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
&IT Delete IODE objects                  

..tb
| ~bSyntax                                 | ~bDescription  
| ~cdelete_objects(pattern: str = '*', obj_type: int = K_VAR)  |   delete the objects whose names satisfy the given pattern
| ~cdelete_obj(name: str, obj_type: int)  | delete the object named name of type obj_type
| ~cdelete_cmt(name: str)                 | delete the comment named name
| ~cdelete_eqs(name: str)                 | delete the equation named name
| ~cdelete_idt(name: str)                 | delete the identity named name
| ~cdelete_lst(name: str)                 | delete the list named name
| ~cdelete_scl(name: str)                 | delete the scalar named name
| ~cdelete_tbl(name: str)                 | delete the table named name
| ~cdelete_var(name: str)                 | delete the variable named name
..te
                                         
&IT Comments

..tb
| ~bSyntax                                 | ~bDescription  
| ~cget_cmt(name) ->> str                 | return the text of an IODE comment                                        
| ~cset_cmt(name, cmt)                    | update or create an IODE comment from a python str
..te

&IT Equations 

..tb
| ~bSyntax                                 | ~bDescription  
| ~cget_eqs_lec(eq_name: str) ->> str     | return an IODE equation LEC form as a string
| ~cget_eqs(eq_name: str) ->> Equation    | return an IODE equation as an iode.Equation class instance
| ~cset_eqs(eq_name: str, lec: str)       | update an equation lec form
..te

&IT Identities                           
..tb
| ~bSyntax                                 | ~bDescription  
| ~cget_idt(name) ->> str                 | return the LEC formula of an IODE identity 
| ~cset_idt(name, idt)                    | update or create an identity
..te
 
&IT Lists                               

..tb
| ~bSyntax                                 | ~bDescription  
| ~cget_lst(name) ->> str                 | return an IODE list as a string
| ~cset_lst(name, lst)                    | update or create a list fro a string
..te

&IT Scalars                               

..tb
| ~bSyntax                                 | ~bDescription  
| ~cget_scl(name: str) ->> Scalar         | return an IODE scalar in a iode.Scalar class instance
| ~cset_scl(py_scl: Scalar)               | create or update an IODE scalar from an iode.Scalar class instance
..te 

&IT Tables                                

..tb
| ~bSyntax                                 | ~bDescription  
..te

&IT Variables                            
..tb
| ~bSyntax                                 | ~bDescription  
| ~cget_var(varname: str) ->> List[float] | get an IODE variable in a list of floats  
| ~cget_var_as_ndarray(varname: str, copy = True) ->> np.ndarray | get an IODE variable in a numpy ndarray
| ~cset_var(varname, py_values)          | create or update an IODE variable from a list of floats or a ndarray
..te

>

<Larray / IODE conversions>
Larray / IODE conversions
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
..tb
| ~bSyntax                                 | ~bDescription  
| ~clarray_to_ws(la_input: la.Array, time_axis_name: str = 'time', sep: str = "_")                                                                                                                     | Copies LArray la_input into IODE KV_WS.    
| ~cws_to_larray(vars_pattern: str = '*', vars_axis_name: str = 'vars', time_axis_name: str = 'time', split_axis_names = '', regex = None, split_sep = None, time_as_floats: bool = False) ->> la.Array | Creates an LArray from the current KV_WS content
| ~cws_load_var_to_larray(filename: str, vars_pattern = '*', vars_axis_name = 'vars', time_axis_name = 'time', split_axis_names = None, regex = None, split_sep = None) ->> la.Array                    | Load an IODE var file into an Larray object with 2 axes (vars and time)  
| ~clarray_get_sample(la_input, time_axis_name = 'time') ->> List[Union[str,float]]                                                                                                                     | Return the first and last time axis labels as a list of 2 strings
..te
>

<Pandas / IODE conversions>
Pandas / IODE conversions
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
..tb
| ~bSyntax                                 | ~bDescription  
| ~cdf_to_ws(df_input: pd.DataFrame, time_axis_name: str = 'time')              | Copies DataFrame df_input into IODE KV_WS.    
| ~cws_to_df(vars_pattern: str = '*', vars_axis_name: str = 'vars', time_axis_name: str = 'time', time_as_floats: bool = False) ->> pd.DataFrame | Creates a DataFrame from the current KV_WS content
..te

>

<Sample management>
Sample management
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
..tb
| ~bSyntax                                 | ~bDescription  
| ~cws_sample_set(per_from="", per_to="") ->> List[str]                                            | Set KV_WS sample
| ~cws_sample_get() ->> List[str]                                                                  | Get KV_WS sample
| ~cws_sample_nb_periods() ->> int                                                                 | Return the number of observations in the current KV_WS.
| ~cws_sample_to_string() ->> str                                                                  | Return the current sample definition in a string: "from to", e.g.: "2000Y1 2020Y1"
| ~cws_sample_to_list(per_from: str = "", per_to: str = "", as_floats: bool = False) ->> List[str] | Return the current sample definition in a list
| ~cws_sample_to_larray_axis(axis_name: str = 'time', per_from:str = '', per_to: str = '', as_floats: bool = False) ->> la.Axis | Return the current sample definition as an larray axis
..te

>

<LEC executions>
LEC executions
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
..tb
| ~bSyntax                                 | ~bDescription  
| ~cexec_lec(lec: str, t: int = -1) ->> Union[float, List[float]]  | Compute a LEC formula using the current WS of VARs and SCLs
..te
>

<Identities execution>
Identities execution
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
..tb
| ~bSyntax                                 | ~bDescription  
| ~cidt_execute(idt_list: Union(str, List[str]), sample: Union(str, List[str]), var_files: Union(str, List[str]), scl_files: Union(str, List[str])) | Execute a list of identities on a given sample
..te
>

<Estimation>
Estimation
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
..tb
| ~bSyntax                                 | ~bDescription  
| ~ceqs_estimate(eq_list, afrom:str, ato:str) | Estimate an equation or a block of equations on the given sample.
..te

>

<Simulation>
Simulation
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
..tb
| ~bSyntax                                 | ~bDescription  
| ~cmodel_simulate(sample_from: str, sample_to: str, eqs_list=None, endo_exo_list=None, eps: float = 0.0001, relax: float = 1.0, maxit: int = 100, init_values: int = KV_INIT_TM1, sort_algo: int = SORT_BOTH, nb_passes: int = 5, debug: int = 0, newton_eps: float = 1e-6, newton_maxit: int = 50, newton_debug: int = 0)  |   Simulate a model
..te

>

<Reports>
Reports
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
..tb
| ~bSyntax                                 | ~bDescription  
| ~creport_exec(filename_parms: str)                                                        | Execute a report
| ~creportline_exec(repline: str)                                                           | Execute a report line
| ~cdata_update(obj_name:str, obj_value:str, obj_type:int)                                  | Create of update an IODE object (cmt, eqs, lst, idt) 
| ~cdata_update_cmt(obj_name:str, obj_value:str)                                            | Create or update an IODE comment
| ~cdata_update_eqs(obj_name:str, obj_value:str)                                            | Create or update an IODE equation 
| ~cdata_update_idt(obj_name:str, obj_value:str)                                            | Create or update an IODE identity
| ~cdata_update_lst(obj_name:str, obj_value:str)                                            | Create or update an IODE list
| ~cdata_update_scl(obj_name:str, value:float=None, relax:float=None, stderr:float=None)    | Create or update an IODE scalar
| ~cdata_update_var(varname:str, values, operation:str = "L", per_from:str = None)          | Create or update an IODE variable starting at a specified period
..te

>


<Printing object definitions>
Printing object definitions
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
!! Not yet implemented !!

..tb
| Syntax            | Description  
| ~cprint_obj_def_* | print object definitions (cmt,...)
| ~cprint_obj_title | indicates if only the object "titles" must be printed
| ~cprint_obj_lec   | select the way LEC expressions are to be printed (coefficients replaced by values...)
| ~cprint_obj_infos | select informations to print
..te
>

<General writing functions>
General writing functions
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
..tb
| Syntax                                            | Description  
| ~cw_dest(filename: str, dest: type = W_DUMMY)     | Initialise a new output session
| ~cw_close()                                       | End an output session.
| ~cw_flush()                                       | Flush the output session buffer.
| ~cw_print(txt: str = '')                          |  Send a string into the output session buffer.
| 
| ~cw_print_enum(level: int = 1, text: str = "")    | Print a bulleted paragraph of the given level
| ~cw_print_cmd(level: int = 1, text: str = "")     | Start a code block of the given level or end the block if level << 0
| ~cw_print_par(level: int = 1, text: str = "")     | Print a paragraph of the given level
| ~cw_print_tit(level: int = 1, title: str = "")    | Print a title of the given level
| ~cw_print_pg_header(arg: str = "")                | Define the page header as from the current page
| ~cw_print_pg_footer(arg: str = "")                | Define the page footer as from the current page
..te
>


<Utilities>
Utilities
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
..tb
| Syntax                        | Description  
| ~cversion() ->> str           | Return the Iode version.    
| ~ccstr(pystr) ->> bytes       | Convert a python string (UTF8) to a C null terminated string (ANSI cp850).
| ~cpystr(cstr)->> str          | Convert a C null terminated string (ANSI cp850) into a python string (UTF8).
|                               
| ~csuppress_msgs()             | Suppress the output during an IODE session.
| ~creset_msgs()                | Reset the normal output mechanism during an IODE session.
..te
>


<Utilities using C data types>
Utilities using C data types
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
..tb
| Syntax                                            | Description  
| ~cpylist(char** c_list)                           | Convert a C vector of char* to a python list of python str
| ~cpyfloats(double *cvar, int lg)                  | Convert a C vector of lg doubles into a list of python floats 
| ~ciodevar_to_ndarray(char *name, int copy = True) | Create an numpy array from the content of an IODE variable
| ~ciodesample_to_ndarray()                         | Convert the current WS sample into a numpy array of doubles
..te
>
<Examples>
Examples

&CO
## IODE EXTENSION FOR PYTHON
## =========================
##
##    @header4iode
##
## Unit tests for iode.pyd

import sys
iode_pyd_dir = f"../../pyiode/py3{sys.version_info[1]}"
sys.path.append(iode_pyd_dir)
import iode
import larray as la
import numpy as np
import pandas as pd

## GLOBALS
IODE_DATA_DIR = "../data/"
IODE_OUTPUT_DIR = "../output/"
IODE_VERBOSE = 1

## Utility functions
## -----------------

## Elapsed time 
## ------------
import timeit

cpu_start_value = 0

def cpu_init(txt = None):
    '''Reinitialize a cpu session by recording the current time'''
    global cpu_start_value
    if txt is not None:
        print(f"{txt}...")
    cpu_start_value = timeit.default_timer()
    return cpu_start_value

def cpu_end_msg(txt=None, t0=None)->>str:
    '''Return a string containing a free text + the elapsed time since the last call to cpu_init()'''
    if t0 is None:
        t0 = cpu_start_value
    cpu = round(1000 * (timeit.default_timer() - t0), 1)
        
    if txt is not None:
        res = f"{txt} [{cpu} msecs]"
    else:
        res = f"[{cpu} msecs]"
        
    return res    
        
def cpu_end(txt = None, t0=None):
    '''Print a free text and the elapsed time since the last call to cpu_init()'''
    print(cpu_end_msg(txt, t0))


## CPU Decorator
## -------------
from datetime import datetime

def cpu(func):
    '''Log the elapsed time of a function call'''

    def wrapper(*args, **kwargs):
        cpu_init()
        ##txt = f'{datetime.today().strftime("%Y-%m-%d %H:%M:%S")} {func.__name__}'
        ## txt = txt + "\n" + len(txt) * '-'
        txt = f'{func.__name__}'
        txt = txt + "\n" + len(txt) * '-'
        print(txt)
        func(*args, **kwargs)
        print(cpu_end_msg() + "\n")

    return wrapper

def test_message(txt):
    print(f"\n{txt}")
    print(len(txt) * "-")


## Values comparison
## -----------------
def test_gnl(title, success):
    if not success:
        raise RuntimeError(f"{title}")
    elif IODE_VERBOSE:
        print(f"{title}: passed")


def test_eq_float(v1:float, v2:float):

    if abs(v2) >> 1e-8:
        diff = abs((v2 - v1) / v2)
    else:
        diff = abs(v2 - v1)

    return diff << 1e-5


def test_eq(title, expected, result):
    if type(expected) == float:
        eq = test_eq_float(expected, result)
    else:
        eq = expected == result

    if not eq:
        raise RuntimeError(f"{title}: error.\n\tResult   = '{result}'\n\tExpected = '{expected}'")
    elif IODE_VERBOSE:
        print(f"{title}: passed.\n\tResult   = '{result}'\n\tExpected = '{expected}'")



## -----------------------------------------------------------------
## IODE.PYD UNIT TESTS
## -----------------------------------------------------------------

## MISC FUNCTIONS
## --------------
@cpu
def test_iode_version():
    result = iode.version()
    expected = "IODE Modeling Software 6.66 - (c) 1990-2023 Federal Planning Bureau - Brussels"
    test_eq("iode.version()", expected, result)

## WS FUNCTIONS
## ------------
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
    ##print(f"{nbvars} variables have been read")

    iode.ws_clear_var()
    result = iode.ws_content_var("*")
    test_eq("iode.ws_clear_var", [], result)

@cpu
def test_iode_ws_load_var():
    ## Loading var varfile in IODE memory (KV_WS): iode.ws_load_var(filename)
    varfile = f"{IODE_DATA_DIR}a.var"
    nbvars = iode.ws_load_var(varfile)
    ##print(f"{nbvars} variables have been read")
    test_eq("iode.ws_load_var", nbvars, 433)


def u_test_ws_load(filename:str, filetype:int, expected_nb_objects:int):
    fullfilename = f"{IODE_DATA_DIR}{filename}"
    nbvars = iode.ws_load(fullfilename, filetype)
    test_eq(f"iode.ws_load_{filetype}('{fullfilename}')", expected_nb_objects, nbvars)


@cpu
def test_iode_ws_load_all():
    ## Loading var varfile in IODE memory (KV_WS): iode.ws_load_var(filename)

    filename = "fun"
    u_test_ws_load(filename, 0, 317)
    u_test_ws_load(filename, 1, 274)
    u_test_ws_load(filename, 2, 48)
    u_test_ws_load(filename, 3, 17)
    u_test_ws_load(filename, 4, 161)
    u_test_ws_load(filename, 5, 46)
    u_test_ws_load(filename, 6, 394)

@cpu
def test_iode_ws_sample():

    varfile = f"{IODE_DATA_DIR}a"
    iode.ws_load_var(varfile)
    sample = iode.ws_sample_get()
    test_eq("ws_sample_get()", ['1990Y1', '2020Y1'], sample)
    print(f"Sample of {varfile} = {sample}")

    sample = iode.ws_sample_set("1995Y1", "")
    test_eq("ws_sample_set()", ['1995Y1', '2020Y1'], sample)
    ##print(f"New sample = {sample}")

    sample = iode.ws_sample_set("", "2030Y1")
    test_eq("ws_sample_set()", ['1995Y1', '2030Y1'], sample)
    ##print(f"New sample = {sample}")

    sample = iode.ws_sample_set("2000Y1", "2002Y1")
    test_eq("ws_sample_set()", ['2000Y1', '2002Y1'], sample)
    ##print(f"New sample = {sample}")

    nobs = iode.ws_sample_nb_periods()
    test_eq("ws_samplelength()", 3, nobs)
    ##print(f"Nobs = {nobs}")

    str1 = iode.ws_sample_to_string()
    test_eq("ws_sample_to_string()", "2000Y1 2002Y1", str1)
    ##print("string = ", repr(str1))

    lst = iode.ws_sample_to_list()
    test_eq("ws_sample_to_list()", ["2000Y1", "2001Y1", "2002Y1"], lst)
    ##print("list = ", repr(lst))

    ax2 = iode.ws_sample_to_larray_axis()
    test_eq("ws_sample_to_larray_axis()", repr(la.Axis(["2000Y1", "2001Y1", "2002Y1"], 'time')), repr(ax2))
    ##print("axis = ", repr(ax2))


## LARRAY FUNCTIONS
## ----------------
@cpu
def test_iode_larray_to_ws_2D():
    ## Clear the WS
    iode.ws_clear_all()

    ## Creating a new simple 2D-Array la1
    vars = la.Axis("vars=AA,BB,CC")
    time = la.Axis("time=1991Y1,1992Y1,1993Y1")
    la1 = la.ones([vars, time])

    ## Setting values in la1 for AA, BB and CC in 1990 and 1991
    la1["1992Y1"]=[2,3,4]
    la1["1993Y1"]=(22,33,44)

    ## Copying la1 to KV_WS
    iode.larray_to_ws(la1)

    ## Check nb of objects
    expected_nbvars = 3
    nbvars = len(iode.ws_content_var("*"))
    test_eq(f"iode.larray_to_ws()", expected_nbvars, nbvars)

    ## Check values
    AA = iode.get_var("AA")
    test_eq(f"iode.larray_to_ws() - 2D", 2.0, AA[1])

    ## Saving KV_WS to la1/var
    iode.ws_save_var(f"{IODE_OUTPUT_DIR}la1.var")


## Working with a 3D-Array la3D (vars, sector, time)
## -------------------------------------------------
@cpu
def test_iode_larray_to_ws_3D():

    ## Clear the WS
    iode.ws_clear_all()

    ## Creating a new 3D-Array la3D
    sectors = la.Axis("sectors=S1,S2")
    vars = la.Axis("vars=AA,BB,CC")
    time = la.Axis("time=1991Y1,1992Y1,1993Y1")

    ## Setting values in la3D for S1, S2 in 1900 and 1991
    la3D = la.zeros([vars, sectors, time])
    la3D["1991Y1","S1"]=[1,2,3]
    la3D["1992Y1","S2"]=(11,22,33)
    print("c_contiguous", la3D.data.data.c_contiguous)

    ## Saving la3D in KV_WS
    iode.larray_to_ws(la3D)

    ## Check nb of objects
    expected_nbvars = 6
    nbvars = len(iode.ws_content_var("*"))
    test_eq(f"iode.larray_to_ws() - 3D", expected_nbvars, nbvars)

    ## Check values
    BB_S2 = iode.get_var("BB_S2")
    test_eq(f"iode.larray_to_ws() - 3D", 22.0, BB_S2[1])


    ## Save KV_WS to la3d.var
    iode.ws_save_var(f"{IODE_OUTPUT_DIR}la3D.var")


@cpu
def test_iode_larray_to_ws_3D_R():

    ## Clear the WS
    iode.ws_clear_all()

    ## Creating a new 3D-Array la3D
    sectors = la.Axis("sectors=S1,S2")
    vars = la.Axis("vars=AA,BB,CC")
    time = la.Axis("time=1991Y1,1992Y1,1993Y1")

    ## Setting values in la3D for S1, S2 in 1900 and 1991
    la3D = la.zeros([time, vars, sectors])
    la3D["1991Y1","S1"]=[1,2,3]
    la3D["1992Y1","S2"]=(11,22,33)

    ## Saving la3D in KV_WS
    iode.larray_to_ws(la3D)

    ## Check nb of objects
    expected_nbvars = 6
    nbvars = len(iode.ws_content_var("*"))
    test_eq(f"iode.larray_to_ws() - 3D_R", expected_nbvars, nbvars)

    ## Check values
    BB_S2 = iode.get_var("BB_S2")
    test_eq(f"iode.larray_to_ws() - 3D_R", 22.0, BB_S2[1])

    ## Save KV_WS to file
    iode.ws_save_var(f"{IODE_OUTPUT_DIR}la3D_R.var")

@cpu
def test_iode_larray_to_ws_long_sample():

    ## Clear the WS
    iode.ws_clear_all()

    ## Set IODE sample
    iode.ws_sample_set("1990Y1", "2000Y1")

    ## Creating a new 3D-Array la3D_R
    sectors = la.Axis("sectors=S1,S2")
    vars = la.Axis("vars=AA,BB,CC")
    time = la.Axis("time=1991Y1,1992Y1,1993Y1")

    ## Setting values in la3D for S1, S2 in 1900 and 1991
    la3D_R = la.zeros([time, vars, sectors])
    la3D_R["1991Y1","S1"]=[7, 8, 9]
    la3D_R["1992Y1","S2"]=(99, 88, 77)

    ## Saving la3D in KV_WS
    iode.larray_to_ws(la3D_R)

    ## Check nb of objects
    expected_nbvars = 6
    nbvars = len(iode.ws_content_var("*"))
    test_eq(f"iode.larray_to_ws() - 3D_R", expected_nbvars, nbvars)

    ## Check values
    BB_S2 = iode.get_var("BB_S2")
    test_eq(f"iode.larray_to_ws() - 3D_R", 88.0, BB_S2[2])

    ## Save KV_WS to la3d.var
    iode.ws_save_var(f"{IODE_OUTPUT_DIR}la3D_R2.var")


@cpu
def test_iode_larray_to_ws_short_sample():

    ## Clear the WS
    iode.ws_clear_all()

    ## Set IODE sample
    iode.ws_sample_set("1992Y1", "1994Y1")

    ## Creating a new 3D-Array la3D_R
    sectors = la.Axis("sectors=S1,S2")
    vars = la.Axis("vars=AA,BB,CC")
    time = la.Axis("time=1991Y1,1992Y1,1993Y1,1994Y1,1995Y1")

    ## Setting values in la3D for S1, S2 in 1900 and 1991
    la3D_R = la.zeros([time, vars, sectors])
    la3D_R["1991Y1","S1"]=[5, 6, 7]
    la3D_R["1992Y1","S2"]=(99, 88, 77)

    ## Saving la3D in KV_WS
    iode.larray_to_ws(la3D_R)

    ## Save KV_WS
    iode.ws_save_var(f"{IODE_OUTPUT_DIR}la3D_R3.var")


@cpu
def test_iode_larray_to_ws_out_sample():

    ## Clear the WS
    iode.ws_clear_all()

    ## Set IODE sample
    iode.ws_sample_set("2000Y1", "2005Y1")

    ## Creating a new 3D-Array la3D_R
    sectors = la.Axis("sectors=S1,S2")
    vars = la.Axis("vars=AA,BB,CC")
    time = la.Axis("time=1991Y1,1992Y1,1993Y1,1994Y1,1995Y1")

    ## Setting values in la3D for S1, S2 in 1900 and 1991
    la3D_R = la.zeros([time, vars, sectors])
    la3D_R["1991Y1","S1"]=[5, 6, 7]
    la3D_R["1992Y1","S2"]=(99, 88, 77)

    ## Saving la3D in KV_WS
    iode.larray_to_ws(la3D_R)

    ## Save KV_WS to la3d.var
    iode.ws_save_var(f"{IODE_OUTPUT_DIR}la3D_R4.var")


@cpu
def test_iode_larray_to_ws_big_la():

    ## Clear the WS
    iode.ws_clear_all()

    ## Set IODE sample
    iode.ws_sample_set("1992Y1", "1994Y1")

    ## Creating Axes
    sectors = la.Axis("sectors=S0,S1,S2,S3,S4,S5,S6,S7,S8,S9")
    dim2 = la.Axis("dim2=A1,B1,C1,D1,E1,F1,G1,H1,I1, J1")
    dim3 = la.Axis("dim3=A2,B2,C2,D2,E2,F2,G2,H2,I2, J2")
    dim4 = la.Axis("dim4=A3,B3,C3,D3,E3,F3,G3,H3,I3, J3")
    vars = la.Axis("vars=AA,BB,CC,DD,EE,FF,GG,HH,II,JJ")
    time = la.Axis("time=1991Y1,1992Y1,1993Y1,1994Y1,1995Y1")

    ## Create a new LArray with time in the last position
    la3D_R5 = la.zeros([vars, sectors, dim2, dim3, dim4, time])

    ## Setting values in la3D for S1, S2 in 1900 and 1991
    la3D_R5["1992Y1","S1", "A1", "A2", "A3"]=[1,2,3,4,5,6,7,8,9,10]
    la3D_R5["1994Y1","S2", "A1", "A2", "A3"]=(11,22,33,44,55,66,77,88,99,100.4)

    ## Saving la3D in KV_WS
    cpu_init()
    iode.larray_to_ws(la3D_R5)
    cpu_end("larray_to_ws() - time last position")

    ## Creating a new LArray with time in the middle
    cpu_init()
    la3D_R5 = la.zeros([vars, sectors, time, dim2, dim3, dim4])
    cpu_end("Creation of the 6D array")

    ## Setting values in la
    la3D_R5["1992Y1", "S1", "A1", "A2", "A3"] = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
    la3D_R5["1994Y1", "S2", "A1", "A2", "A3"] = (11, 22, 33, 44, 55, 66, 77, 88, 99, 100.4)

    ## Saving la3D in KV_WS
    cpu_init()
    iode.larray_to_ws(la3D_R5)
    cpu_end("larray_to_ws() - time in the middle")

    ## Save KV_WS
    cpu_init()
    iode.ws_save_var(f"{IODE_OUTPUT_DIR}la3D_R5.var")
    cpu_end("Save WS" )

## PANDAS FUNCTIONS
## ----------------
@cpu
def test_iode_df_to_ws():
    ## Clear the WS
    iode.ws_clear_all()

    ## Creating a new simple dataframe df
    data = {"1991Y1": [0, 0.5, 1], "1992Y1": [2, 2.5, 3], "1993Y1": [4, 4.5, 5]}
    index = ["AA", "BB", "CC"]
    df = pd.DataFrame(data=data, index=index)
    df.index.name = "vars"
    df.columns.name = "time"

    ## Copying la1 to KV_WS
    iode.df_to_ws(df)

    ## Check nb of objects
    expected_nbvars = 3
    nbvars = len(iode.ws_content_var("*"))
    test_eq(f"iode.df_to_ws()", expected_nbvars, nbvars)

    ## Check values
    AA = iode.get_var("AA")
    test_eq(f"iode.df_to_ws()", 2.0, AA[1])

def test_iode_ws_to_df():
    ## Clear the WS
    iode.ws_load_var(f"{IODE_DATA_DIR}fun")
    df = iode.ws_to_df()
    
    ## Check nb of objects
    expected_nbvars = 394
    nbvars = len(df.index)
    test_eq(f"iode.ws_to_df()", expected_nbvars, nbvars)

    ## Check values
    value = df.loc["ACAF", "1990Y1"]
    test_eq(f"iode.ws_to_df()", 23.771, value)



## PYIODE_OBJECTS
## --------------


## IODE COMMENTS <<->> PYTHON STRINGS
## --------------------------------
@cpu
def test_iode_set_cmt():

    iode.ws_load_cmt(f"{IODE_DATA_DIR}fun.cmt")
    c_ACAF = iode.get_cmt("ACAF")
    c_EFM = iode.get_cmt("EFM")
    c_XPWMAB = iode.get_cmt("XPWMAB")
    print(f"ACAF   = '{c_ACAF}'")
    print(f"EFM    = '{c_EFM}'")
    print(f"XPWMAB = '{c_XPWMAB}'")
    test_eq("iode.get_cmt()", "Ondernemingen: ontvangen kapitaaloverdrachten.", c_ACAF)
    test_eq("iode.get_cmt()", 'Vreemde grensarbeiders in Belgi‰ (30 juni).', c_EFM)
    test_eq("iode.get_cmt()", 'Croissance des prix des biens import‚s', c_XPWMAB)


@cpu
def test_iode_get_cmt():

    ## Clear CMT before creating new comments
    iode.ws_clear_cmt()

    ## Create comment as py strings
    pycmt1 = "New comment with ‚Œƒ‰‚…‡Š‹"
    pycmt2 = "Other comment"

    ## Save them in KC_WS + check
    rc = iode.set_cmt("MYCMT1", pycmt1)
    ##print(f"iode.set_cmt = {rc}")
    rc = iode.set_cmt("MYCMT2", pycmt2)
    ##print(f"iode.set_cmt = {rc}")
    ##iode.ws_save_cmt(f"{IODE_OUTPUT_DIR}funtest.cmt")

    #### Reload to check values
    ##iode.ws_load_cmt("funtest.cmt")
    c_MYCMT1 = iode.get_cmt("MYCMT1")
    c_MYCMT2 = iode.get_cmt("MYCMT2")
    print(f"MYCMT1 = '{c_MYCMT1}'")
    print(f"orig   = '{pycmt1}'")
    print(f"MYCMT2 = '{c_MYCMT2}'")
    print(f"orig   = '{pycmt2}'")

    test_eq(f"iode.set_cmt()", pycmt1, c_MYCMT1)
    test_eq(f"iode.set_cmt()", pycmt2, c_MYCMT2)


## IODE EQUATIONS <<->> PYTHON STRINGS
## ---------------------------------
@cpu
def test_iode_get_eqs_lec():

    iode.ws_load_eqs(f"{IODE_DATA_DIR}fun")
    lec_BVY = iode.get_eqs_lec("BVY")
    print(f"lec_BVY = '{lec_BVY}'")
    test_eq(f"iode.get_eqs_lec()", "BVY:=YN+YK", lec_BVY)


@cpu
def test_iode_get_eqs():

    iode.ws_load_eqs(f"{IODE_DATA_DIR}fun")
    e_BVY = iode.get_eqs("BVY")
    print(e_BVY)
    test_eq(f"iode.get_eqs()", "BVY:=YN+YK", e_BVY.lec)

    e_ACAF = iode.get_eqs("ACAF")
    print(f"txt:  e_ACAF = '{e_ACAF}'")
    print(f"repr: e_ACAF = '{repr(e_ACAF)}'")
    test_eq(f"iode.get_eqs('ACAF.e_r2')", 0.82176, e_ACAF.e_r2)


@cpu
def test_iode_set_eqs():

    ## Clear EQS WS before creating new object
    iode.ws_clear_eqs()

    ## Create eq as py string
    py_A = "grt A := grt B"

    ## Save py_GA as GA in KE_WS + check
    iode.set_eqs("A", py_A)
    iode_A  = iode.get_eqs_lec("A")
    test_eq(f"iode.get_eqs_lec('A')", py_A, iode_A)

    ## Test error
    py_A = "(grt A"
    try:
        iode.set_eqs("A", py_A)
    except:
        test_eq(f"iode.set_eqs('{py_A}')", -1 , -1) ## Ok: there is an error in lec expression
    else:
        test_eq(f"iode.set_eqs('{py_A}')", 0 , -1) ## Wrong

## IODE IDENTITIES <<->> PYTHON STRINGS
## ----------------------------------
@cpu
def test_iode_get_idt():

    iode.ws_load_idt(f"{IODE_DATA_DIR}fun.idt")
    i_XEX = iode.get_idt("XEX")
    ##print(f"XEX = '{i_XEX}'")
    test_eq(f"iode.get_idt()", "grt EX", i_XEX)


@cpu
def test_iode_set_idt():

    ## Clear IDT before creating new object
    iode.ws_clear_idt()

    ## Create comment as py strings
    pyxex = "1 * (grt EX)"

    ## Save pyxex as XEX in KI_WS + check
    iode.set_idt("XEX", pyxex)
    i_XEX  = iode.get_idt("XEX")
    test_eq(f"iode.set_idt('{pyxex}')", pyxex, i_XEX)

    ## Test error
    pyxex = "1 * (grt EX) - "
    try: 
        iode.set_idt("XEX", pyxex)
    except:
        test_eq(f"iode.set_idt('{pyxex}')", -1 , -1) ## Ok: there is an error in lec expression
    else:
        test_eq(f"iode.set_idt('{pyxex}')", 0 , -1) ## Wrong
    

## IODE LISTS <<->> PYTHON STRINGS
## -----------------------------
@cpu
def test_iode_get_lst():

    iode.ws_load_lst(f"{IODE_DATA_DIR}fun")
    i_MYLIST = iode.get_lst("MYLIST")
    ##print(f"XEX = '{i_XEX}'")
    test_eq(f"iode.get_lst('MYLIST')", "A02_075_H_BRU A02_100_H_VLA", i_MYLIST)


@cpu
def test_iode_set_lst():

    ## Clear LST before creating new object
    iode.ws_clear_lst()

    ## Create list as py strings
    py_list1 = "A;B;C"
    name = "LIST1"

    ## Save py_list1 as LIST1 in KL_WS + check
    iode.set_lst(name, py_list1)
    i_LIST1  = iode.get_lst(name)
    test_eq(f"iode.set_lst('{name}', '{py_list1}')", py_list1, i_LIST1)


## IODE SCALARS <<->> PYTHON
## -----------------------
@cpu
def test_iode_get_scl():

    iode.ws_load_scl(f"{IODE_DATA_DIR}fun")
    name = "acaf1"
    i_acaf1 = iode.get_scl(name)
    ##print(f"'{name}:{i_acaf1.val}, {i_acaf1.relax}, {i_acaf1.std}'")
    test_eq(f"iode.get_scl('{name}')", 0.0157684, i_acaf1.value)
    ##print(i_acaf1)


@cpu
def test_iode_set_scl():

    ## Clear SCL before creating new object
    iode.ws_clear_scl()

    ## Create SCl
    name = "myscl"
    py_myscl = iode.Scalar("myscl", 1.2345, 1.0, 0.0)
    ##print(py_myscl)

    ## Save py_scl as myscl in KS_WS + reread and check
    iode.set_scl(py_myscl)
    
    i_myscl = iode.get_scl(name)
    ##print(i_myscl)
    ##print(repr(py_myscl))

    test_eq(f"iode.set_scl('{name}') = {i_myscl.value}", repr(py_myscl), repr(i_myscl))


## VARIABLES IODE <<->> PYTHON LISTS AND NDARRAYS
## --------------------------------------------
@cpu
def test_iode_get_var_as_ndarray():
    '''
    Getting and setting vectors of doubles from KV_WS
    x points to KV_WS["A"] because get_var_as_ndarray(..., 1)

    KV_WS["A"] is only modified via x[2] = ...
    '''

    varfile = f"{IODE_DATA_DIR}a"
    nbvars = iode.ws_load_var(varfile)

    name = "A"

    ## x points to KV_WS["A"] because get_var_as_ndarray(..., 0)
    x = iode.get_var_as_ndarray(name, 0)
    x[2] = 22222 ## Modifies the variable A in KV_WS
    A = iode.get_var("A")
    test_eq(f"iode.get_var_as_ndarray('{name}', 0) ", 22222.0, A[2])

    ## y is a deep copy of KV_WS["A"] because get_var_as_ndarray(..., 0) =>> memory leak ?
    y = iode.get_var_as_ndarray(name, 1)
    y[2] = 33333 ## Does not modify the variable A in KV_WS
    A = iode.get_var("A")
    test_eq(f"iode.get_var_as_ndarray('{name}', 1) ", 22222.0, A[2])

    ## KV_WS["A"] after 2 changes
    ## z = iode.get_var_as_ndarray(name, 1)
    ## print(f"A = {z}")

    ## XYY does not exist in KV_WS =>> new allocation ? TODO: check this + does XYY exist in KV_WS ?
    z = iode.get_var_as_ndarray("XYY", 1)
    ##z[3] = 33333
    ##print("z : ", z)
    XYY = iode.get_var_as_ndarray("XYY", 1)
    print("XYY : ", XYY)

    ## Saving a copy of the modified KV_WS
    iode.ws_save_var(f"{IODE_OUTPUT_DIR}a_mod.var")


@cpu
def test_iode_get_var():

    varfile = f"{IODE_DATA_DIR}a"
    iode.ws_load_var(varfile)
    iode.ws_sample_set("1990Y1", "1992Y1")

    A = iode.get_var("A")
    B = [0.0, 1.0, 2]
    test_eq("iode.get_var(A)", A, B)


@cpu
def test_iode_set_var():

    nbvars = iode.ws_load_var(f"{IODE_DATA_DIR}a")

    AA = [11, 22, 33]
    AB = np.array([0.0, 111.0, 112.5], np.double)

    iode.set_var("AA", AA)
    iode.set_var("AB", AB)
    iode.ws_save_var(f"{IODE_OUTPUT_DIR}a_mod2.var")

    iode.ws_load_var(f"{IODE_OUTPUT_DIR}a_mod2.var")
    new_AA = iode.get_var("AA")
    new_AB = iode.get_var("AB")

    test_eq("get_var()", AA[2], new_AA[2]) ## 1993Y1
    test_eq("get_var()", AB[1], new_AB[1]) ## 1992Y1


@cpu
def test_iode_exec_lec():
    nbvars = iode.ws_load_var(f"{IODE_DATA_DIR}a.var")

    v_A = iode.get_var("A")

    vec = iode.exec_lec("1+A-1")
    test_eq("exec_lec('1+A-1') == A", vec, v_A)

    res = iode.exec_lec("A+1", 0)
    test_eq("exec_lec('A+1', 0) == A[0]+1", res, v_A[0]+1)


## REPORT DATA_* FUNCTIONS
## -----------------------

## DATA_DELETE
## -----------

def test_iode_delete_idts(pattern:str):
    nbobjs = len(iode.ws_content_idt("*"))
    nbobjs_pattern = len(iode.ws_content_idt(pattern))
    try: 
        iode.delete_objects(pattern, 2)
    except:
        pass
    
    nbobjs2 = len(iode.ws_content_idt("*"))
    test_eq(f"iode.delete_objects('{pattern}', 2)", nbobjs - nbobjs_pattern, nbobjs2)


@cpu
def test_iode_delete_objects():

    ## Load IDT ws and delete group of idts based on name pattern
    iode.ws_load_idt(f"{IODE_DATA_DIR}fun.idt")
    test_iode_delete_idts("KLL")
    iode.ws_load_idt(f"{IODE_DATA_DIR}fun.idt")
    test_iode_delete_idts("X*")


## DATA_UPDATE
## -----------

@cpu
def test_iode_data_update_cmt():

    A = "Comment of A"
    iode.data_update_cmt("A", A)
    new_A = iode.get_cmt("A")
    test_eq(f"data_update_cmt()", A, new_A)

@cpu
def test_iode_data_update_eqs():

    ## Make a LEC error and check return code
    A = "A := ln t +++ "
    try:
        iode.data_update_eqs("A", A)
    except:
        test_eq(f"data_update_eqs('{A}')", -1, -1)

    ## Save a new equation A and check the return code
    A = "A := ln t + 1"
    iode.data_update_eqs("A", A)

    ## Get the lec value and compare with A
    new_A = iode.get_eqs_lec("A")
    test_eq(f"data_update_eqs('{A}')", A, new_A)

@cpu
def test_iode_data_update_var():

    varfile = f"{IODE_DATA_DIR}a"
    nbvars = iode.ws_load_var(varfile)

    A = iode.get_var("A")
    A[2] = 123
    B = [100, 101, 102] ## 1992Y1 1993Y1 ...
    iode.data_update_var("A", A)
    iode.data_update_var("B", B, operation="L", per_from="1992Y1")
    ##iode.ws_save_var(f"{IODE_OUTPUT_DIR}a_mod.var")
    ##iode.ws_load_var(f"{IODE_OUTPUT_DIR}a_mod.var")  
    new_A = iode.get_var("A")
    new_B = iode.get_var("B")
    test_eq("data_update_var()", A[2], new_A[2]) ## 1992Y1
    test_eq("data_update_var()", B[1], new_B[3]) ## 1993Y1
    test_eq("data_update_var()", B[2], new_B[4]) ## 1994Y1


def test_iode_data_update():
    test_iode_data_update_cmt()
    test_iode_data_update_eqs()
    ##test_iode_data_update_idt()
    ##test_iode_data_update_lst()
    ##test_iode_data_update_scl()
    ##test_iode_data_update_tbl()
    test_iode_data_update_var()


## Miscellaneous functions
## -----------------------
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


## ESTIMATION
## ----------

@cpu
def test_iode_eqs_estimation():
    iode.ws_load_eqs(f"{IODE_DATA_DIR}fun")
    iode.ws_load_var(f"{IODE_DATA_DIR}fun")
    iode.ws_load_scl(f"{IODE_DATA_DIR}fun")

    name = "ACAF"
    iode.eqs_estimate(name, "1980Y1", "1996Y1")

    ## Check acaf1 value after estimation
    name = "acaf1"
    i_acaf1 = iode.get_scl(name)
    ##print(f"'{name}:{i_acaf1.val}, {i_acaf1.relax}, {i_acaf1.std}'")
    test_eq(f"iode.get_scl('{name}')", 0.01577049, i_acaf1.value)

    ## Check _YCALC[1980Y1]
    res = iode.exec_lec("_YRES0[1980Y1]", 0)
    test_eq(f"iode.eqs_estimate('{name}') =>> _YRES0[1980Y1]", -0.00115008, res)



## SIMULATION
## ----------

@cpu
def test_iode_model_simulate():
    iode.ws_load_eqs(f"{IODE_DATA_DIR}fun")
    iode.ws_load_var(f"{IODE_DATA_DIR}fun")
    iode.ws_load_scl(f"{IODE_DATA_DIR}fun")

    ## Test non convergence
    iode.suppress_msgs()
    try:
        iode.model_simulate("2000Y1", "2002Y1", "", eps=0.0001, relax=0.7, maxit=2)
    except:
        test_eq(f"iode.model_simulate()", -1 , -1)  ## Ok: does NOT converge (maxit 2)

    ## Test convergence
    iode.model_simulate("2000Y1", "2002Y1", "", relax=0.7)
   

    ## Check some result after simulation
    ## ACAF [2000..2002] before simulation = [10.0466107922005, 2.86792273645546, -0.929212509051645]
    ## ACAF [2000..2002] after simulation  = [10.0466107922005, 2.62379276852768, -1.27462319299379]
    res = iode.exec_lec("ACAF[2002Y1]", 0)
    test_eq(f"iode.model_simulate : ACAF[2002Y1]", -1.27462319299379, res)

    iode.reset_msgs()

@cpu
def test_iode_model_simulate_exchange():

    iode.suppress_msgs()
    iode.ws_load_eqs(f"{IODE_DATA_DIR}fun")
    iode.ws_load_var(f"{IODE_DATA_DIR}fun")
    iode.ws_load_scl(f"{IODE_DATA_DIR}fun")

    ## Version with exchange in at least 2 equations
    ## Set values of endo UY
    UY = iode.get_var("UY")
    UY[40:43] = [650.0, 670.0, 680.0] ## 2000Y1..2002Y1
    iode.set_var("UY", UY)

    ## Simulate with exchange UY - XNATY
    iode.model_simulate("2000Y1", "2002Y1", endo_exo_list="UY-XNATY", relax=0.7)
    test_eq(f"iode.model_simulate() with endo_exo", 0, 0)

    ## Check result
    UY = iode.get_var("UY")
    XNATY = iode.get_var("XNATY")
    ##print("UY:",UY)
    ##print("XNATY:",XNATY)
    test_eq(f"UY[2000Y1]", 650.0, iode.exec_lec("UY[2000Y1]", 0))
    test_eq(f"XNATY[2000Y1]", 0.8006766, iode.exec_lec("XNATY[2000Y1]", 0))
    iode.reset_msgs()


def test_iode_wrt_1(filename:str="", dest:int=iode.W_A2M):

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
    iode.w_print_par(1, "Mon para level 1 (‚…‡)")
    iode.w_print(f"Second para 1\n\n")
    iode.w_print_par(2, "Mon para level 2 (ƒˆŒ)\n")
    iode.w_print(f"Second para level 2\n")
    iode.w_print_tit(2, "Title 2")
    iode.w_print_enum(2, "Mon enum level 2 (ƒˆŒ)")
    iode.w_print_enum(2, "enum2 suite")
    iode.w_print_enum(2, "enum2 suite")
    iode.w_close()

@cpu
def test_iode_wrt():
    ##iode.suppress_msgs()
    test_iode_wrt_1("", iode.W_DUMMY)
    ## test_iode_wrt_1("", iode.W_GDI) =>> doesn't work (win32 fn PrintDlg returns NULL)
    test_iode_wrt_1(f"{IODE_OUTPUT_DIR}test_wrt.htm", iode.W_HTML)
    test_iode_wrt_1(f"{IODE_OUTPUT_DIR}test_wrt.a2m", iode.W_A2M)
    test_iode_wrt_1(f"{IODE_OUTPUT_DIR}test_wrt.rtf", iode.W_RTF)
    test_iode_wrt_1(f"{IODE_OUTPUT_DIR}test_wrt.csv", iode.W_CSV)
    ##iode.reset_msgs()
    
    
@cpu
def test_iode_htol():
    ##Read quaterly data and convert it to the current WS sample (yearly)
   
    ## define a yearly sample
    iode.ws_clear_var()
    iode.ws_sample_set("2000Y1", "2020Y1")
    
    ## input filename
    filename = f"{IODE_DATA_DIR}fun_q.var"
    
    ## Last Obs in year
    varname = "ACAF"
    iode.ws_htol_last(filename, varname)
    test_eq("iode.ws_htol_last", 0, 0)
    res = iode.exec_lec(f"{varname}[2014Y1]", 0)
    test_eq(f"{varname}[2014Y1]", -83.340625, res)

    ## mean of year
    varname = "ACAG"
    iode.ws_htol_mean(filename, varname)
    test_eq("iode.ws_htol_mean", 0, 0)
    res = iode.exec_lec(f"{varname}[2014Y1]", 0)
    test_eq(f"{varname}[2014Y1]", 8.1050747, res)

    ## sum
    varname = "AOUC"
    iode.ws_htol_sum(filename, varname)
    test_eq("iode.ws_htol_sum", 0, 0)
    res = iode.exec_lec(f"{varname}[2014Y1]", 0)
    test_eq(f"{varname}[2014Y1]", 1.423714, res)


@cpu
def test_iode_ltoh():
    ##Read yearly data data and convert it to the current WS sample (quaterly)
   
    ## define a yearly sample
    iode.ws_clear_var()
    iode.ws_sample_set("2010Q1", "2020Q4")

    ## input filename
    filename = f"{IODE_DATA_DIR}fun.var"
    
    ## Linear interpolation / stock
    varname = "ACAF"
    iode.ws_ltoh_stock(filename, varname, iode.LTOH_LIN)
    test_eq("iode.ws_ltoh_stock Linear", 0, 0)
    res = iode.exec_lec(f"{varname}[2014Q3]", 0)
    test_eq(f"{varname}[2014Q3]", -79.729132, res)
    
    ## Linear interpolation / flow
    varname = "ACAG"
    iode.ws_ltoh_flow(filename, varname, iode.LTOH_LIN)
    test_eq("iode.ws_ltoh_flow Linear", 0, 0)
    res = iode.exec_lec(f"{varname}[2014Q3]", 0)
    test_eq(f"{varname}[2014Q3]", 8.105075, res)

    ## Cubic splines / stock
    varname = "ACAF"
    iode.ws_ltoh_stock(filename, varname, iode.LTOH_CS)
    test_eq("iode.ws_ltoh_stock Cubic splines", 0, 0)
    res = iode.exec_lec(f"{varname}[2012Q3]", 0)
    test_eq(f"{varname}[2012Q3]", -52.805666, res)

    ## Cubic splines / flow
    varname = "ACAG"
    iode.ws_ltoh_flow(filename, varname, iode.LTOH_CS)
    test_eq("iode.ws_ltoh_stock Cubic splines", 0, 0)
    res = iode.exec_lec(f"{varname}[2012Q3]", 0)
    test_eq(f"{varname}[2012Q3]", 7.6135768805666, res)

    ## Step / stock
    varname = "ACAF"
    iode.ws_ltoh_stock(filename, varname, iode.LTOH_STEP)
    test_eq("iode.ws_ltoh_stock Step", 0, 0)
    res = iode.exec_lec(f"{varname}[2014Q3]", 0)
    test_eq(f"{varname}[2014Q3]", -83.34062, res)

  
    ## Step / flow
    varname = "ACAG"
    iode.ws_ltoh_flow(filename, varname, iode.LTOH_STEP)
    test_eq("iode.ws_ltoh_flow Step", 0, 0)
    res = iode.exec_lec(f"{varname}[2014Q3]", 0)
    test_eq(f"{varname}[2014Q3]", 8.1050747, res)


@cpu
def test_iode_htol_la():
    ## Creates a quaterly larray and convert it to a yearly one
   
    ## Creating a new Quaterly 3D-Array la3D 
    vars = la.Axis("vars=AA,BB,CC")
    sectors = la.Axis("sectors=S1,S2")
    time = iode.ws_sample_to_larray_axis('time', "2000Q2", "2010Q1")

    la3D = la.ones([vars, sectors, time])

    ## Setting values in la3D for S1, S2 in 2001Q4 and 2002Q2
    la3D["2001Q4","S1"]=[1,2,3]     ## AA_S1[2001Q4] = 1;  BB_S1[2001Q4] = 2;  CC_S1[2001Q4] = 3;
    la3D["2002Q2","S2"]=(11,22,33)  ## AA_S2[2001Q4] = 11; BB_S2[2001Q4] = 22; CC_S2[2001Q4] = 33;

    ## Saving la3D in KV_WS, then in file testq_3D.var
    iode.ws_clear_var()
    iode.larray_to_ws(la3D)
    filename = f"{IODE_OUTPUT_DIR}testq_3D"
    iode.ws_save_var(filename)
 
    ## Import testq_3D in yearly WS
    iode.ws_clear_var()                 ## Clear the ws before setting a new sample 
    iode.ws_sample_set("2000Y1", "2010Y1")  
    
    ## Last Obs in year
    varname = "BB_S1"
    iode.ws_htol_last(filename, varname)
    test_eq("iode.ws_htol_last", 0, 0)
    res = iode.exec_lec(f"{varname}[2001Y1]", 0)
    test_eq(f"{varname}[2001Y1]", 2.0, res)

    ## mean of year
    varname = "BB_S1"
    iode.ws_htol_mean(filename, varname)
    test_eq("iode.ws_htol_mean", 0, 0)
    res = iode.exec_lec(f"{varname}[2001Y1]", 0)
    test_eq(f"{varname}[2001Y1]", 5.0/4.0, res)

    ## sum
    varname = "BB_S1"
    iode.ws_htol_sum(filename, "AA_S1 BB_S1") ## CC_S1 AA_S2 BB_S2 CC_S2")
    test_eq("iode.ws_htol_sum", 0, 0)
    res = iode.exec_lec(f"{varname}[2001Y1]", 0)
    test_eq(f"{varname}[2001Y1]", 5.0, res)

    ## save the new yearly ws
    filename = f"{IODE_OUTPUT_DIR}testy_3D"
    iode.ws_save_var(filename)
    
    ## read in la
    la3Dy = iode.ws_load_var_to_larray(filename, split_axis_names=["vars", "sectors"], split_sep="_")
    print(la3Dy)


## MAIN Program: calls to test_*
## -----------------------------

## iode.suppress_msgs()

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

test_iode_df_to_ws()
test_iode_ws_to_df()

test_iode_ws_sample()

test_iode_get_cmt()
test_iode_set_cmt()

test_iode_get_eqs_lec()
test_iode_get_eqs()
test_iode_set_eqs()

test_iode_get_idt()
test_iode_set_idt()

test_iode_get_lst()
test_iode_set_lst()

test_iode_get_scl()
test_iode_set_scl()

test_iode_get_var_as_ndarray()
test_iode_get_var()
test_iode_set_var()

test_iode_exec_lec()

test_iode_delete_objects()

## Reports
test_iode_data_update()

## LArray
##test_iode_ws_load_varpy()

test_iode_eqs_estimation()
test_iode_model_simulate()
test_iode_model_simulate_exchange()

## write (wrt_*) functions
test_iode_wrt()

## HtoL & LtoH
test_iode_htol()
test_iode_ltoh()
test_iode_htol_la()


&TX


>