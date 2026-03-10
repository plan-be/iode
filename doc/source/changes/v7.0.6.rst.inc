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

Fixed Bugs 
----------


Miscellaneous 
-------------

- PYTHON: hide deprecated functions and objects from ``from iode import *`` 
  and ``dir(iode)``. Deprecated functionality remains accessible via explicit 
  imports from ``iode.deprecated`` (closes :issue:`1144`) 
