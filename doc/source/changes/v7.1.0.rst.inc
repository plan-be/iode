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
  * PYTHON: added the :meth:`Variables.execute_RAS` method to execute the RAS algorithm (same as `$DataRasVar`)
  * PYTHON: added the :meth:`Variables.get_names_from_pattern` method to get the names of IODE objects matching 
    a given pattern (same as `$DataPatternXXX`)
  * PYTHON: added the :meth:`Equation.estimate_stepwise` and :meth:`Equations.estimate_stepwise` methods to estimate 
    an equation and searches for the best possible test values for all possible combinations of coefficients 
    (same as `$EqsStepWise`)

Fixed Bugs 
----------


Miscellaneous 
-------------

