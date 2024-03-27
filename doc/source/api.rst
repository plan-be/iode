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

   Database.load
   Database.save
   Database.clear

   Database.subset
   Database.is_subset
   Database.is_copy_subset

   Database.iode_type
   Database.filename
   Database.description

   Database.__getitem__
   Database.__setitem__
   Database.__delitem__
   Database.__contains__
   Database.__len__

   Database.get_names
   Database.rename

   Database.get_associated_objects_list

   Database.copy_into
   Database.merge
   Database.merge_into
   

.. autosummary::
   :toctree: _generated

   ws_content
   ws_content_eqs
   ws_content_idt
   ws_content_lst
   ws_content_scl
   ws_content_tbl
   ws_content_var
   
   ws_clear
   ws_clear_eqs
   ws_clear_idt
   ws_clear_lst
   ws_clear_scl
   ws_clear_tbl
   ws_clear_var
   ws_clear_all

   ws_load
   ws_load_eqs
   ws_load_idt
   ws_load_lst
   ws_load_scl
   ws_load_tbl
   ws_load_var

   ws_save
   ws_save_eqs
   ws_save_idt
   ws_save_lst
   ws_save_scl
   ws_save_tbl
   ws_save_var


************
IODE OBJECTS
************

.. autosummary::
   :toctree: _generated

   delete_objects
   delete_obj
   data_update

========
EQUATION
========

.. autosummary::
   :toctree: _generated

   Equation
   Equation.get_coefficients_list
   Equation.get_date_format  
   Equation.get_variables_list
   Equation.set_lec
   Equation.set_sample       
   Equation.split_equation

   get_eqs_lec
   get_eqs
   set_eqs
   data_update_eqs
   delete_eqs

========
IDENTITY
========

.. autosummary::
   :toctree: _generated

   get_idt
   set_idt
   idt_execute
   data_update_idt
   delete_idt

====
LIST
====

.. autosummary::
   :toctree: _generated

   get_lst
   set_lst
   data_update_lst
   delete_lst

======
SCALAR
======

.. autosummary::
   :toctree: _generated

   Scalar
   get_scl
   set_scl   
   data_update_scl
   delete_scl

=====
TABLE
=====

.. autosummary::
   :toctree: _generated

   delete_tbl

========
VARIABLE
========

.. autosummary::
   :toctree: _generated

   get_var
   get_var_as_ndarray
   set_var
   data_update_var
   delete_var


****
TIME
****

.. autosummary::
   :toctree: _generated

   Period
   Period.difference
   Period.nb_periods_per_year  
   Period.shift
   Period.to_float

   Sample
   Sample.get_list_periods     
   Sample.get_period_position  
   Sample.intersection

   ws_sample_get
   ws_sample_set


**********
ESTIMATION
**********

.. autosummary::
   :toctree: _generated

   eqs_estimate


**********
SIMULATION
**********

.. autosummary::
   :toctree: _generated
   
   model_simulate


*************
REPORTS - LEC
*************

.. autosummary::
   :toctree: _generated

   exec_lec
   report_exec
   reportline_exec


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

======
PANDAS
======

.. autosummary::
   :toctree: _generated

   df_to_ws
   ws_to_df

======
LARRAY
======

.. autosummary::
   :toctree: _generated

   larray_to_ws
   ws_to_larray
   ws_load_var_to_larray
   larray_get_sample


*************
MISCELLANEOUS
*************

.. autosummary::
   :toctree: _generated

   version
   suppress_msgs
   reset_msgs


**********
DEPRECATED
**********

.. autosummary::
   :toctree: _generated 

   ws_content_cmt
   ws_clear_cmt
   ws_load_cmt
   ws_save_cmt

   delete_cmt
   get_cmt
   set_cmt
   data_update_cmt

   ws_sample_nb_periods
   ws_sample_to_string
   ws_sample_to_list
