Version 7.0.6
=============

In development

New Features 
------------

- PYTHON: add support for Python 3.14 (closes :issue:`1147`)

Improvements 
------------

- PYTHON: removed warning message when trying the get the sample of the *Variables* 
  database while it is undefined (closes :issue:`1150`) 

- PYTHON: made :meth:`Variables.from_frame` and :meth:`Variables.from_array` methods to 
  also accept columns / time axis with labels of type int (closes :issue:`1151`) 

Fixed Bugs 
----------

- PYTHON : fixed reading IODE equations, identities, lists or tables files if they 
  contain any LEC expression with non-ASCII characters (closes :issue:`1157`)

- GUI: fixed application crashing when trying to edit an IODE table (closes :issue:`1175`)

Miscellaneous 
-------------

- PYTHON: hide deprecated functions and objects from ``from iode import *`` 
  and ``dir(iode)``. Deprecated functionality remains accessible via explicit 
  imports from ``iode.deprecated`` (closes :issue:`1144`) 

- C API: The memory management of the IODE objects in the IODE workspaces has been 
  delegated to C++ std containers (in place of the SCR4 library previously). 
  That should not impact the output of estimations or simulations. 
  Closes :issue:`1012`

- C API: The structure handling the *compiled LEC* objects (present in IODE *equations*, 
  *identities* and *tables*) has been rewritten. That should not impact the output of 
  estimations or simulations. 
