Version 7.0.11
==============

In development

New Features 
------------


Improvements 
------------

- GUI: inside a text or report tab, the shortcuts `CTRL and +` and `CTRL and =` 
  increase the font size, while the shortcut `CTRL + -` decrease it 
  (closes :issue:`1236`) 

- PYTHON: speed up loading of *comments*, *equations*, *identities*, *lists* and 
  *tables* workspaces.

Fixed Bugs 
----------

- PYTHON: fixed crash when simulating a list of equations instead of the whole 
  equations workspace (closes :issue:`1319`)

- PYTHON: Speed up simulation process (loop over the years) after 
  a significant slowness introduced in version 7.0.5 (:issue:`1256`)

- IODE REPORT + PYTHON: allowed to use empty IODE lists in Python script or 
  IODE reports except when creating subsets of IODE workspaces (closes :issue:`1341`) 


Miscellaneous 
-------------
