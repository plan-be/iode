<IODE: python module>
IODE: python module
�����������������
..sep |
..esc ~
..ignore ��

&TI Introduction
�����������������
The python module for IODE implements part of the IODE API. 
The aim is to implement a python layer for all IODE functionalities so that IODE can be driven directly from a python session.

&TI Python version & iode.pyd
�����������������
Each version of python has its own IODE module ~ciode.pyd~C. They are stored in 
different subdirectories of the IODE main directory (usually ~cc:\iode~C). For example: ~c.\py39, .\py310~C...
By default, the ~ciode.pyd~C module for python 3.9 is thus ~cc:/iode/py39/iode.pyd~C. For python 3.10: ~cc:/iode/py310/python.pyd~C.


&TI Memory management
�����������������
During a python session, IODE data are grouped in workspaces, one by object type (variables, scalars, equations...) 
and stored in IODE memory using a proprietary memory management system called IODE-SWAP. IODE functions operate ~binside~B that memory.
 
Exchanges between IODE objects (in IODE-SWAP) and python objects (larray, numpy or pandas objects in python memory) 
are made possible via interface functions described below like ~cget_eqs()~C, ~cset_eqs()~C or ~clarray_to_ws()~C.

&TI Starting a PYIODE session
����������������������������������
There are two ways to use the IODE module in the python environment.

&IT Method 1: copy iode.pyd in your working directory
����������������������������������

That method consists simply in copying ~ciode.pyd~C into the working directory. 
This is the simplest method but it requires to know the version of 
python in use (3.9, 3.10...?). Moreover, ~ciode.pyd~C has to be copied with each new version of IODE and/or python. 
 
&IT Method 2: add the iode.pyd directory to the system path
����������������������������������
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
����������������������������������

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
����������������������������������
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
����������������������������������
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
����������������������������������
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
����������������������������������
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
����������������������������������
..tb
| ~bSyntax                                 | ~bDescription  
| ~cdf_to_ws(df_input: pd.DataFrame, time_axis_name: str = 'time')              | Copies DataFrame df_input into IODE KV_WS.    
| ~cws_to_df(vars_pattern: str = '*', vars_axis_name: str = 'vars', time_axis_name: str = 'time', time_as_floats: bool = False) ->> pd.DataFrame | Creates a DataFrame from the current KV_WS content
..te

>

<Sample management>
Sample management
����������������������������������
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
����������������������������������
..tb
| ~bSyntax                                 | ~bDescription  
| ~cexec_lec(lec: str, t: int = -1) ->> Union[float, List[float]]  | Compute a LEC formula using the current WS of VARs and SCLs
..te
>

<Identities execution>
Identities execution
����������������������������������
..tb
| ~bSyntax                                 | ~bDescription  
| ~cidt_execute(idt_list: Union(str, List[str]), sample: Union(str, List[str]), var_files: Union(str, List[str]), scl_files: Union(str, List[str])) | Execute a list of identities on a given sample
..te
>

<Estimation>
Estimation
����������������������������������
..tb
| ~bSyntax                                 | ~bDescription  
| ~ceqs_estimate(eq_list, afrom:str, ato:str) | Estimate an equation or a block of equations on the given sample.
..te

>

<Simulation>
Simulation
����������������������������������
..tb
| ~bSyntax                                 | ~bDescription  
| ~cmodel_simulate(sample_from: str, sample_to: str, eqs_list=None, endo_exo_list=None, eps: float = 0.0001, relax: float = 1.0, maxit: int = 100, init_values: int = KV_INIT_TM1, sort_algo: int = SORT_BOTH, nb_passes: int = 5, debug: int = 0, newton_eps: float = 1e-6, newton_maxit: int = 50, newton_debug: int = 0)  |   Simulate a model
..te

>

<Reports>
Reports
����������������������������������
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
����������������������������������
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
����������������������������������
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
����������������������������������
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
����������������������������������
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

See test_iode.py file from the directory tests/test_python from the IODE Public repository.

&TX


>