.. _start_api:

.. currentmodule:: iode


#############
API Reference
#############

*********
WORKSPACE
*********

========
Comments
========

.. autosummary::
   :toctree: _generated

   Comments

   Comments.names
   Comments.iode_type
   Comments.filename
   Comments.description

   Comments.load
   Comments.save
   Comments.clear
   Comments.copy
   Comments.new_detached
   Comments.print_to_file

   Comments.get_names
   Comments.get_names_from_pattern
   Comments.rename
   Comments.search

   Comments.__len__
   Comments.__contains__
   Comments.__iter__
   Comments.__getitem__
   Comments.__setitem__
   Comments.__delitem__
   Comments.i

   Comments.copy_from
   Comments.merge
   Comments.merge_from
   Comments.compare

   Comments.is_detached
   Comments.is_global_workspace

Properties and methods specific to Comments

.. autosummary::
   :toctree: _generated

   Comments.series
   Comments.from_series
   Comments.to_series

   Comments.convert_file

=========
Equations
=========

.. autosummary::
   :toctree: _generated

   Equations

   Equations.names
   Equations.iode_type
   Equations.filename
   Equations.description

   Equations.load
   Equations.save
   Equations.clear
   Equations.copy
   Equations.new_detached
   Equations.print_to_file
   Equations.print_lec
   Equations.print_infos

   Equations.search
   Equations.get_names
   Equations.get_names_from_pattern
   Equations.rename
   Equations.coefficients
   Equations.variables

   Equations.__len__
   Equations.__contains__
   Equations.__iter__
   Equations.__getitem__
   Equations.__setitem__
   Equations.__delitem__
   Equations.i

   Equations.copy_from
   Equations.merge
   Equations.merge_from 
   Equations.compare 

   Equations.is_detached
   Equations.is_global_workspace

Properties and methods specific to Equations

.. autosummary::
   :toctree: _generated

   Equations.df
   Equations.from_frame
   Equations.to_frame

   Equations.estimate
   Equations.estimate_step_wise
   Equations.print_nb_decimals
   Equations.print_equations_as
   Equations.print_equations_lec_as

==========
Identities
==========

.. autosummary::
   :toctree: _generated

   Identities

   Identities.names
   Identities.iode_type
   Identities.filename
   Identities.description

   Identities.load
   Identities.save
   Identities.clear
   Identities.copy
   Identities.new_detached
   Identities.print_to_file

   Identities.search
   Identities.get_names
   Identities.get_names_from_pattern
   Identities.rename
   Identities.coefficients
   Identities.variables

   Identities.__len__
   Identities.__contains__
   Identities.__iter__
   Identities.__getitem__
   Identities.__setitem__
   Identities.__delitem__
   Identities.i

   Identities.copy_from
   Identities.merge
   Identities.merge_from
   Identities.compare

   Identities.is_detached
   Identities.is_global_workspace

Properties and methods specific to Identities

.. autosummary::
   :toctree: _generated

   Identities.series
   Identities.from_series
   Identities.to_series

   Identities.execute

=====
Lists
=====

.. autosummary::
   :toctree: _generated

   Lists

   Lists.names
   Lists.iode_type
   Lists.filename
   Lists.description

   Lists.load
   Lists.save
   Lists.clear
   Lists.copy
   Lists.new_detached
   Lists.print_to_file

   Lists.search
   Lists.get_names
   Lists.get_names_from_pattern
   Lists.rename

   Lists.__len__
   Lists.__contains__
   Lists.__iter__
   Lists.__getitem__
   Lists.__setitem__
   Lists.__delitem__
   Lists.i

   Lists.copy_from
   Lists.merge
   Lists.merge_from
   Lists.compare

   Lists.is_detached
   Lists.is_global_workspace

Properties and methods specific to Lists

.. autosummary::
   :toctree: _generated

   Lists.series
   Lists.from_series
   Lists.to_series

=======
Scalars
=======

.. autosummary::
   :toctree: _generated

   Scalars

   Scalars.names
   Scalars.iode_type
   Scalars.filename
   Scalars.description

   Scalars.load
   Scalars.save
   Scalars.clear
   Scalars.copy
   Scalars.new_detached
   Scalars.print_to_file

   Scalars.search
   Scalars.get_names
   Scalars.get_names_from_pattern
   Scalars.rename

   Scalars.__len__
   Scalars.__contains__
   Scalars.__iter__
   Scalars.__getitem__
   Scalars.__setitem__
   Scalars.__delitem__
   Scalars.i

   Scalars.copy_from
   Scalars.merge
   Scalars.merge_from
   Scalars.compare

   Scalars.is_detached
   Scalars.is_global_workspace

Properties and methods specific to Scalars

.. autosummary::
   :toctree: _generated

   Scalars.df
   Scalars.from_series
   Scalars.from_frame
   Scalars.to_frame

   Scalars.print_nb_decimals

======
Tables
======

.. autosummary::
   :toctree: _generated

   Tables

   Tables.names
   Tables.iode_type
   Tables.filename
   Tables.description

   Tables.load
   Tables.save
   Tables.clear
   Tables.copy
   Tables.new_detached
   Tables.print_to_file

   Tables.search
   Tables.get_names
   Tables.get_names_from_pattern
   Tables.rename
   Tables.coefficients
   Tables.variables

   Tables.__len__
   Tables.__contains__
   Tables.__iter__
   Tables.__getitem__
   Tables.__setitem__
   Tables.__delitem__
   Tables.i

   Tables.copy_from
   Tables.merge
   Tables.merge_from
   Tables.compare

   Tables.is_detached
   Tables.is_global_workspace

Properties and methods specific to Tables

.. autosummary::
   :toctree: _generated

   Tables.print_nb_decimals
   Tables.print_tables_as

=========
Variables
=========

.. autosummary::
   :toctree: _generated

   Variables

   Variables.names
   Variables.iode_type
   Variables.filename
   Variables.description

   Variables.load
   Variables.save
   Variables.clear
   Variables.copy
   Variables.new_detached
   Variables.print_to_file

   Variables.search
   Variables.get_names
   Variables.get_names_from_pattern
   Variables.rename

   Variables.__len__
   Variables.__contains__
   Variables.__iter__
   Variables.__getitem__
   Variables.__setitem__
   Variables.__delitem__
   Variables.i

   Variables.copy_from
   Variables.merge
   Variables.merge_from
   Variables.compare

   Variables.is_detached
   Variables.is_global_workspace

Properties and methods specific to Variables

.. autosummary::
   :toctree: _generated

   Variables.mode
   Variables.sample
   Variables.nb_periods
   Variables.periods
   Variables.periods_as_str
   Variables.periods_as_float
   Variables.threshold

   Variables.__add__
   Variables.__radd__
   Variables.__iadd__
   
   Variables.__sub__
   Variables.__rsub__
   Variables.__isub__
   
   Variables.__mul__
   Variables.__rmul__
   Variables.__imul__
   
   Variables.__truediv__
   Variables.__rtruediv__
   Variables.__itruediv__
   
   Variables.__pow__
   Variables.__ipow__

   Variables.periods_subset
   Variables.low_to_high
   Variables.high_to_low
   Variables.extrapolate
   Variables.seasonal_adjustment
   Variables.trend_correction
   Variables.execute_RAS

   Variables.from_numpy
   Variables.to_numpy
   Variables.df
   Variables.from_frame
   Variables.to_frame
   Variables.from_array
   Variables.to_array

   Variables.plot

   Variables.convert_file
   Variables.export_as_file

=============
Miscellaneous
=============

.. autosummary::
   :toctree: _generated

   load_extra_files
   reset_extra_files

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
   Equation.copy     

   Equation.get_formated_date  
   Equation.split_equation
   Equation.estimate
   Equation.estimate_step_wise

========
IDENTITY
========

.. autosummary::
   :toctree: _generated

   Identity

   Identity.coefficients
   Identity.variables
   Identity.copy 

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
   Scalar.copy

=====
TABLE
=====

.. autosummary::
   :toctree: _generated

   Table

   Table.nb_lines
   Table.nb_columns
   Table.title
   Table.language
   Table.ymin
   Table.ymax
   Table.gridx
   Table.gridy
   Table.graph_axis
   Table.graph_alignment
   Table.box
   Table.shadow
   
   Table.divider
   Table.coefficients
   Table.variables
   Table.compute
   Table.copy
   Table.index
   Table.insert
   Table.__len__
   Table.__getitem__
   Table.__setitem__
   Table.__delitem__
   Table.__iadd__

   Table.plot

.. autosummary::
   :toctree: _generated

   TableLine

   TableLine.line_type
   TableLine.axis_left
   TableLine.graph_type
   TableLine.__len__
   TableLine.__getitem__
   TableLine.__setitem__
   TableLine.__delitem__

.. autosummary::
   :toctree: _generated

   TableCell

   TableCell.cell_type
   TableCell.align
   TableCell.bold
   TableCell.italic
   TableCell.underline
   TableCell.coefficients
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
   ComputedTable.ymin
   ComputedTable.ymax
   ComputedTable.gridx
   ComputedTable.gridy
   ComputedTable.graph_axis
   ComputedTable.graph_alignment

   ComputedTable.is_editable
   ComputedTable.to_array
   ComputedTable.to_frame
   ComputedTable.__getitem__
   ComputedTable.__setitem__
   
   ComputedTable.print_to_file

   ComputedTable.plot
   ComputedTable.plot_data

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
   Period.__float__

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

   Equation.estimate
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
   Simulation.sort_algorithm_long
   Simulation.initialization_method
   Simulation.initialization_method_long
   Simulation.max_nb_iterations
   Simulation.max_nb_iterations_newton
   Simulation.nb_passes
   Simulation.debug
   Simulation.debug_newton

   Simulation.nb_iterations
   Simulation.save_nb_iterations
   Simulation.norm
   Simulation.save_norms
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
   set_t
   increment_t

************************
GRAPHICAL USER INTERFACE
************************

.. autosummary::
   :toctree: _generated

   view_workspace


*****
PRINT
*****

.. autosummary::
   :toctree: _generated

   write_destination
   write_flush
   write_close

   write
   write_title
   write_enum
   write_paragraph
   write_code_block
   write_page_header
   write_page_footer


*************
MISCELLANEOUS
*************

.. autosummary::
   :toctree: _generated

   is_NA
   suppress_msgs
   enable_msgs


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
