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


Fixed Bugs 
----------

- PYTHON: fixed crash when simulating a list of equations instead of the whole 
  equations workspace (closes :issue:`1319`)


Miscellaneous 
-------------

- PYTHON: add boolean utf8 argument to load() and save() methods of workspaces 
  comments, equations, identities, lists and tables to indicate if the IODE 
  objects are encoded in utf8 encoding (iode version 7.0.11 or later) or in 
  OEM850 encoding (iode version 7.0.10 or earlier) (closes :issue:`1335`)  
