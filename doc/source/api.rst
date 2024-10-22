.. _start_api:

.. currentmodule:: iode


#############
API Reference
#############

*********
WORKSPACE
*********

.. note::
   Below, **Database** represents either:

   * **Comments**
   * **Equations**
   * **Identities**
   * **Lists**
   * **Scalars**
   * **Tables**
   * **Variables**

.. autosummary::
   :toctree: _generated
   :template: database_methods.rst

   Database.names
   Database.iode_type
   Database.filename
   Database.description

   Database.load
   Database.save
   Database.clear

   Database.is_subset
   Database.is_copy_subset
   Database.copy

   Database.get_names
   Database.rename

   Database.__len__
   Database.__contains__
   Database.__iter__
   Database.__getitem__
   Database.__setitem__
   Database.__delitem__

   Database.search

   Database.copy_from
   Database.merge
   Database.merge_from

========
Comments
========

.. autosummary::
   :toctree: _generated

   Comments 
   Comments.series

   Comments.from_series
   Comments.to_series

=========
Equations
=========

.. autosummary::
   :toctree: _generated

   Equations
   Equations.df

   Equations.estimate 
   Equations.from_frame
   Equations.to_frame

==========
Identities
==========

.. autosummary::
   :toctree: _generated

   Identities
   Identities.series

   Identities.execute
   Identities.from_series
   Identities.to_series

=====
Lists
=====

.. autosummary::
   :toctree: _generated

   Lists
   Lists.series

   Lists.from_series
   Lists.to_series

=======
Scalars
=======

.. autosummary::
   :toctree: _generated

   Scalars
   Scalars.df

   Scalars.from_series
   Scalars.from_frame
   Scalars.to_frame

======
Tables
======

.. autosummary::
   :toctree: _generated

   Tables
   Tables.print_to_file

=========
Variables
=========

.. autosummary::
   :toctree: _generated

   Variables

   Variables.mode
   Variables.sample
   Variables.nb_periods
   Variables.periods
   Variables.periods_as_float
   Variables.df
   
   Variables.periods_subset
   Variables.low_to_high
   Variables.high_to_low
   Variables.extrapolate
   Variables.seasonal_adjustment
   Variables.trend_correction
   Variables.from_frame
   Variables.to_frame
   Variables.from_array
   Variables.to_array

=============
Miscellaneous
=============

.. autosummary::
   :toctree: _generated

   load_extra_files

************
IODE OBJECTS
************

========
EQUATION
========

.. autosummary::
   :toctree: _generated

   Equation

   Equation.endogenous
   Equation.lec
   Equation.sample
   Equation.method
   Equation.comment
   Equation.block
   Equation.instruments
   Equation.tests
   Equation.coefficients
   Equation.variables      

   Equation.get_formated_date  
   Equation.split_equation
   Equation.estimate

========
IDENTITY
========

.. autosummary::
   :toctree: _generated

   Identity

   Identity.coefficients
   Identity.variables  

====
LIST
====

.. autosummary::
   :toctree: _generated

   split_list

======
SCALAR
======

.. autosummary::
   :toctree: _generated

   Scalar
   Scalar.value
   Scalar.relax
   Scalar.std

=====
TABLE
=====

.. autosummary::
   :toctree: _generated

   Table

   Table.nb_lines
   Table.nb_columns
   Table.language
   Table.ymin
   Table.ymax
   Table.gridx
   Table.gridy
   Table.graph_axis
   Table.graph_alignment
   Table.box
   Table.shadow
   
   Table.compute
   Table.index
   Table.insert
   Table.__len__
   Table.__getitem__
   Table.__setitem__
   Table.__delitem__
   Table.__iadd__

.. autosummary::
   :toctree: _generated

   TableLine

   TableLine.axis_left
   TableLine.graph_type
   TableLine.line_type

.. autosummary::
   :toctree: _generated

   TableCell

   TableCell.cell_type
   TableCell.align
   TableCell.bold
   TableCell.italic
   TableCell.underline
   TableCell.variables

==============
COMPUTED TABLE
==============

.. autosummary::
   :toctree: _generated

   ComputedTable

   ComputedTable.nb_decimals
   ComputedTable.nb_lines
   ComputedTable.lines
   ComputedTable.nb_columns
   ComputedTable.columns
   ComputedTable.nb_files
   ComputedTable.files
   ComputedTable.nb_operations_between_files
   ComputedTable.sample
   ComputedTable.nb_periods
   ComputedTable.title

   ComputedTable.is_editable
   ComputedTable.to_array
   ComputedTable.to_frame
   ComputedTable.__getitem__
   ComputedTable.__setitem__
   ComputedTable.print_to_file

****
TIME
****

.. autosummary::
   :toctree: _generated

   Period
   Period.year
   Period.periodicity
   Period.step
   Period.difference
   Period.nb_periods_per_year  
   Period.shift
   Period.to_float

   Sample
   Sample.start
   Sample.end
   Sample.nb_periods
   Sample.index  
   Sample.get_period_list     
   Sample.intersection


**********
ESTIMATION
**********

.. autosummary::
   :toctree: _generated

   Equations.estimate
   dynamic_adjustment
   dickey_fuller_test 


**********
SIMULATION
**********

.. autosummary::
   :toctree: _generated
   
   Simulation

   Simulation.convergence_threshold
   Simulation.relax
   Simulation.sort_algorithm
   Simulation.initialization_method
   Simulation.max_nb_iterations
   Simulation.max_nb_iterations_newton
   Simulation.nb_passes
   Simulation.debug
   Simulation.debug_newton

   Simulation.model_exchange
   Simulation.model_compile
   Simulation.model_simulate
   Simulation.model_calculate_SCC
   Simulation.model_simulate_SCC


***
LEC
***

.. autosummary::
   :toctree: _generated

   execute_lec

*******
REPORTS
*******

.. autosummary::
   :toctree: _generated

   execute_report
   execute_command

*****
PRINT
*****

.. autosummary::
   :toctree: _generated

   w_dest
   w_close
   w_flush
   w_print

   w_print_enum
   w_print_cmd
   w_print_par
   w_print_tit
   w_print_pg_header
   w_print_pg_footer


***************
OTHER LIBRARIES
***************


*************
MISCELLANEOUS
*************

.. autosummary::
   :toctree: _generated

   is_NA
   suppress_msgs
   reset_msgs


**********
DEPRECATED
**********

.. autosummary::
   :toctree: _generated 

   ws_content
   ws_clear
   ws_clear_all
   ws_load
   ws_save

   delete_objects
   delete_obj
   data_update

   ws_content_cmt
   ws_clear_cmt
   ws_load_cmt
   ws_save_cmt

   ws_content_eqs
   ws_clear_eqs
   ws_load_eqs
   ws_save_eqs

   ws_content_idt
   ws_clear_idt
   ws_load_idt
   ws_save_idt

   ws_content_lst
   ws_clear_lst
   ws_load_lst
   ws_save_lst

   ws_content_scl
   ws_clear_scl
   ws_load_scl
   ws_save_scl

   ws_content_tbl
   ws_clear_tbl
   ws_load_tbl
   ws_save_tbl

   ws_content_var
   ws_clear_var
   ws_load_var
   ws_save_var

   df_to_ws
   ws_to_df

   larray_to_ws
   ws_to_larray

   delete_cmt
   get_cmt
   set_cmt
   data_update_cmt

   get_eqs_lec
   get_eqs
   set_eqs
   data_update_eqs
   delete_eqs

   get_idt
   set_idt
   idt_execute
   data_update_idt
   delete_idt

   get_lst
   set_lst
   data_update_lst
   delete_lst

   get_scl
   set_scl   
   data_update_scl
   delete_scl

   delete_tbl

   get_var
   get_var_as_ndarray
   set_var
   data_update_var
   delete_var

   ws_sample_nb_periods
   ws_sample_to_string
   ws_sample_to_list
   ws_sample_get
   ws_sample_set

   exec_lec

   eqs_estimate

   model_simulate
   model_calc_scc
   model_simulate_scc
   model_simulate_save_parms
   model_simulate_maxit
   model_simulate_eps
   model_simulate_relax
   model_simulate_nb_passes
   model_simulate_sort_algo
   model_simulate_init_values
   model_simulate_niter
   model_simulate_norm

   report_exec
   reportline_exec
