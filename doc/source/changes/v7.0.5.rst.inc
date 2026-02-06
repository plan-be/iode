Version 7.0.5
=============

In development

New Features 
------------


Improvements 
------------


Fixed Bugs 
----------

- IODE COMMAND: Fixed the IODE command `$PrintTbl` not printing IODE tables to files 
  (closes :issue:`1127`).
- PYTHON: Fixed computing of IODE tables when the sample contains the periods operator `//` 
  (closes :issue:`1130`)
- PYTHON: Fixed a bug in the Python API when printing computed IODE table to A2M files 
  where titles containing the character `#` (closes :issue:`1131`).
- GUI: Fixed a bug in the Equation Editor where saving a new equation did always save it 
  as `eq_name := 0` instead of the actual LEC entered by the user (closes :issue:`1105`).
- GUI: Fixed a bug in the 'filter' field appending the name of a newly created equation 
  twice to the current filter pattern (closes :issue:`1106`). 


Miscellaneous 
-------------

- The memory management of the IODE objects in the IODE workspaces the C/C++ API has been 
  delegated to C++ std containers (in place of the SCR4 library previously). 
  That should not impact the output of estimations or simulations. 
