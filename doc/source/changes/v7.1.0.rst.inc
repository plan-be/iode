Version 7.1.0
=============

In development

New Features 
------------


Improvements 
------------

  * PYTHON: added the boolean argument *compress* to the save() method of comments, ..., variables (same as $WsSaveCmp)
  * PYTHON: added the :meth:`Simulation.nb_iterations` method to get the number of iterations to converge 
    in the last simulation for a given period (same as `@SimNIter`)
  * PYTHON: added the :meth:`Simulation.norm` method to get the norm value of the last simulation 
    for a given period (same as `@SimNorm`) 
  * PYTHON: added the :meth:`Simulation.save_nb_iterations` method to the number of iterations needed 
    to complete each simulated period in a new variable named `var_name` (same as `$ModelSimulateSaveNiters`)
  * PYTHON: added the :meth:`Simulation.save_norms` method to save the lowest norm value reached 
    to complete each simulated period in a new variable named `var_name` (same as `$ModelSimulateSaveNorms`)

Fixed Bugs 
----------


Miscellaneous 
-------------

