Version 7.0.10
==============

In development

New Features 
------------


Improvements 
------------

- PYTHON: Speed up simulation process (especially the ordering of eqs process) after 
  a significant slowness introduced in version 7.0.5 (closes :issue:`1288`)

- PYTHON: Speed up merging and copying from file into IODE workspaces 
  (closes :issue:`1239`)

Fixed Bugs 
----------

- PYTHON: fixed exporting IODE tables to files when the title or cells from the left 
  column contain line breaks or the special character '&' or '#' (closes :issue:`1297`)

- IODE REPORTS: fixed IODE commands `IF` and `DIV0` returning a wrong value 
  (closes :issue:`1298`)

Miscellaneous 
-------------

