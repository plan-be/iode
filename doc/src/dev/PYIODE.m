
<IODE: python module>
IODE: python module
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..sep |
..esc ~
..ignore ¯

&TI Workspace management
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
| Syntax                             | Description  
| ~bWorkspace content
| ~cws_content(pattern, objtype) ->> List[str]       | Return the names of all objects of a given type, satisfying a pattern specification  
| ~cws_content_cmt(pattern) ->> List[str]            | Returns the list of comment names corresponding to the given pattern
| ~cws_content_eqs(pattern) ->> List[str]            | Returns the list of equation names corresponding to the given pattern
| ~cws_content_idt(pattern) ->> List[str]            | Returns the list of identity names corresponding to the given pattern
| ~cws_content_lst(pattern) ->> List[str]            | Returns the list of list names corresponding to the given pattern
| ~cws_content_scl(pattern) ->> List[str]            | Returns the list of scalar names corresponding to the given pattern
| ~cws_content_tbl(pattern) ->> List[str]            | Returns the list of table names corresponding to the given pattern
| ~cws_content_var(pattern) ->> List[str]            | Returns the list of variable names corresponding to the given pattern
| ~bClear workspaces                 |
| ~cws_clear(filetype: int) | Clear WS of the given filetype (K_CMT..K_VAR)
| ~cws_clear_cmt()          | Clear the comment WS
| ~cws_clear_eqs()          | Clear the equation WS
| ~cws_clear_idt()          | Clear the identity WS
| ~cws_clear_lst()          | Clear the list WS
| ~cws_clear_scl()          | Clear the scalar WS
| ~cws_clear_tbl()          | Clear the table WS
| ~cws_clear_var()          | Clear the varaible WS
| ~cws_clear_all()          | Clear all WS           
| ~bLoad workspaces                  |
| ~cws_load(filename: str, filetype: int) ->> int | Load an IODE file and return the number of read objects
| ~cws_load_cmt(filename: str) ->> int            | Load a comment file and return the number of read objects
| ~cws_load_eqs(filename: str) ->> int            | Load a equation file and return the number of read objects
| ~cws_load_idt(filename: str) ->> int            | Load a identity file and return the number of read objects
| ~cws_load_lst(filename: str) ->> int            | Load a list file and return the number of read objects
| ~cws_load_scl(filename: str) ->> int            | Load a scalar file and return the number of read objects
| ~cws_load_tbl(filename: str) ->> int            | Load a table file and return the number of read objects
| ~cws_load_var(filename: str) ->> int            | Load a variable file and return the number of read objects
| ~bSave workspaces                  |
| ~cws_save(filename: str, filetype: int) | Save the current IODE workspace of a given type
| ~cws_save_cmt(filename: str)            | Save the current workspace of comments
| ~cws_save_eqs(filename: str)            | Save the current workspace of equations
| ~cws_save_idt(filename: str)            | Save the current workspace of identities
| ~cws_save_lst(filename: str)            | Save the current workspace of lists
| ~cws_save_scl(filename: str)            | Save the current workspace of scalars
| ~cws_save_tbl(filename: str)            | Save the current workspace of tables
| ~cws_save_var(filename: str)            | Save the current workspace of variables
| ~bChange of periodicites
| ~cws_ltoh_flow(filename: str, varlist)                         |
| ~cws_ltoh_stock(filename: str, varlist, method: int = LTOH_CS) |
| ~cws_htol_sum(filename: str, varlist, method: int)             |
| ~cws_htol_mean(filename: str, varlist, method: int)            |
| ~cws_htol_last(filename: str, varlist, method: int)            |
..te


&TI Object management
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
| Syntax                                 | Description  
| ~bDelete IODE objects                  |
| 
| ~cdelete_objects(pattern: str = '*', obj_type: int = K_VAR)  |   delete the objects whose names satisfy the given pattern
| 
| ~cdelete_obj(name: str, obj_type: int)  | delete the object named name of type obj_type
| ~cdelete_cmt(name: str)                 | delete the comment named name
| ~cdelete_eqs(name: str)                 | delete the equation named name
| ~cdelete_idt(name: str)                 | delete the identity named name
| ~cdelete_lst(name: str)                 | delete the list named name
| ~cdelete_scl(name: str)                 | delete the scalar named name
| ~cdelete_tbl(name: str)                 | delete the table named name
| ~cdelete_var(name: str)                 | delete the variable named name
|                                         
| ~bComments                              
| ~cget_cmt(name) ->> str                 | return the text of an IODE comment                                        
| ~cset_cmt(name, cmt)                    | update or create an IODE comment from a python str
| 
| ~bEquations 
| ~cget_eqs_lec(eq_name: str) ->> str     | return an IODE equation LEC form as a string
| ~cget_eqs(eq_name: str) ->> Equation    | return an IODE equation as an iode.Equation class instance
| ~cset_eqs(eq_name: str, lec: str)       | update an equation lec form
| 
| ~bIdentities                            |
| ~cget_idt(name) ->> str                 | return the LEC formula of an IODE identity 
| ~cset_idt(name, idt)                    | update or create an identity
| 
| ~bLists                                 |
| ~cget_lst(name) ->> str                 | return an IODE list as a string
| ~cset_lst(name, lst)                    | update or create a list fro a string
| 
| ~bScalars                               |
| ~cget_scl(name: str) ->> Scalar         | return an IODE scalar in a iode.Scalar class instance
| ~cset_scl(py_scl: Scalar)               | create or update an IODE scalar from an iode.Scalar class instance
| 
| ~bTables                                |
| ~bVariables                             |
| ~cget_var(varname: str) ->> List[float] | get an IODE variable in a list of floats  
| ~cget_var_as_ndarray(varname: str, copy = True) ->> np.ndarray | get an IODE variable in a numpy ndarray
| ~cset_var(varname, py_values)          | create or update an IODE variable from a list of floats or a ndarray
..te

&TI Larray / IODE conversions
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
| Syntax                                                                                                                                                                                               | Description  
| ~clarray_to_ws(la_input: la.Array, time_axis_name: str = 'time', sep: str = "_")                                                                                                                     | Copies LArray la_input into IODE KV_WS.    
| ~cws_to_larray(vars_pattern: str = '*', vars_axis_name: str = 'vars', time_axis_name: str = 'time', split_axis_names = '', regex = None, split_sep = None, time_as_floats: bool = False) ->> la.Array | Creates an LArray from the current KV_WS content
| ~cws_load_var_to_larray(filename: str, vars_pattern = '*', vars_axis_name = 'vars', time_axis_name = 'time', split_axis_names = None, regex = None, split_sep = None) ->> la.Array                    | Load an IODE var file into an Larray object with 2 axes (vars and time)  
| ~clarray_get_sample(la_input, time_axis_name = 'time') ->> List[Union[str,float]]                                                                                                                     | Return the first and last time axis labels as a list of 2 strings
..te

&TI Pandas / IODE conversions
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
| Syntax                                                                        | Description  
| ~cdf_to_ws(df_input: pd.DataFrame, time_axis_name: str = 'time')              | Copies DataFrame df_input into IODE KV_WS.    
| ~cws_to_df(vars_pattern: str = '*', vars_axis_name: str = 'vars', time_axis_name: str = 'time', time_as_floats: bool = False) ->> pd.DataFrame | Creates a DataFrame from the current KV_WS content
..te


&TI Sample management
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
| Syntax                                                                                          | Description  
| ~cws_sample_set(per_from="", per_to="") ->> List[str]                                            | Set KV_WS sample
| ~cws_sample_get() ->> List[str]                                                                  | Get KV_WS sample
| ~cws_sample_nb_periods() ->> int                                                                 | Return the number of observations in the current KV_WS.
| ~cws_sample_to_string() ->> str                                                                  | Return the current sample definition in a string: "from to", e.g.: "2000Y1 2020Y1"
| ~cws_sample_to_list(per_from: str = "", per_to: str = "", as_floats: bool = False) ->> List[str] | Return the current sample definition in a list
| ~cws_sample_to_larray_axis(axis_name: str = 'time', per_from:str = '', per_to: str = '', as_floats: bool = False) ->> la.Axis | Return the current sample definition as an larray axis
..te


&TI LEC functions 
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
| Syntax                                                          | Description  
| ~cexec_lec(lec: str, t: int = -1) ->> Union[float, List[float]]  | Compute a LEC formula using the current WS of VARs and SCLs
..te


&TI Identities execution
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
| Syntax                                      | Description  
| ~cidt_execute(idt_list: Union(str, List[str]), sample: Union(str, List[str]), var_files: Union(str, List[str]), scl_files: Union(str, List[str])) | Execute a list of identities on a given sample
..te


&TI Estimation
¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
| Syntax                                      | Description  
| ~ceqs_estimate(eq_list, afrom:str, ato:str) | Estimate an equation or a block of equations on the given sample.
..te


&TI Simulation
¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
| Syntax                                | Description  
| ~cmodel_simulate(sample_from: str, sample_to: str, eqs_list=None, endo_exo_list=None, eps: float = 0.0001, relax: float = 1.0, maxit: int = 100, init_values: int = KV_INIT_TM1, sort_algo: int = SORT_BOTH, nb_passes: int = 5, debug: int = 0, newton_eps: float = 1e-6, newton_maxit: int = 50, newton_debug: int = 0)  |   Simulate a model
..te

&TI Reports
¯¯¯¯¯¯¯¯¯¯¯
..tb
| Syntax                                                                                    | Description  
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

&TI Print functions
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
!! Not yet implemented !!

..tb
| Syntax            | Description  
| ~cprint_obj_def_* | print object definitions (cmt,...)
| ~cprint_obj_title | indicates if only the object "titles" must be printed
| ~cprint_obj_lec   | select the way LEC expressions are to be printed (coefficients replaced by values...)
| ~cprint_obj_infos | select informations to print
..te

&TI Write functions
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
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

&TI Utilities
¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
| Syntax                        | Description  
| ~cversion() ->> str           | Return the Iode version.    
| ~ccstr(pystr) ->> bytes       | Convert a python string (UTF8) to a C null terminated string (ANSI cp850).
| ~cpystr(cstr)->> str          | Convert a C null terminated string (ANSI cp850) into a python string (UTF8).
|                               
| ~csuppress_msgs()             | Suppress the output during an IODE session.
| ~creset_msgs()                | Reset the normal output mechanism during an IODE session.
..te

&TI Utilities using C data types
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
| Syntax                                            | Description  
| ~cpylist(char** c_list)                           | Convert a C vector of char* to a python list of python str
| ~cpyfloats(double *cvar, int lg)                  | Convert a C vector of lg doubles into a list of python floats 
| ~ciodevar_to_ndarray(char *name, int copy = True) | Create an numpy array from the content of an IODE variable
| ~ciodesample_to_ndarray()                         | Convert the current WS sample into a numpy array of doubles
..te

>