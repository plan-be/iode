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

- GUI: when copy/pasting Variables values from an Excel file to the Variables view: 
  1. accept periods to be passed as integers if they represent years 
  2. accept to paste only names and values without periods (in that case, the GUI 
  asks for the periods range) (closes :issue:`1343`)

Fixed Bugs 
----------

- PYTHON: fixed crash when simulating a list of equations instead of the whole 
  equations workspace (closes :issue:`1319`)

- PYTHON: Speed up simulation process (loop over the years) after 
  a significant slowness introduced in version 7.0.5 (:issue:`1256`)

- IODE REPORT + PYTHON: allowed to use empty IODE lists in Python script or 
  IODE reports except when creating subsets of IODE workspaces (closes :issue:`1341`) 

- IODE REPORT + PYTHON: fixed broken backward compatibility with IODE 6.68 of 
  generated binary files from saving *Equations*, *Identities* and *Tables* IODE 
  workspaces (closes :issue:`1347`)

- GUI: refresh the Variables view when copy/pasting values from an Excel file

- GUI: fixed pasting Variables values (CTRL + V) copied from an Excel file 
  (which stopped silently as soon as a pasted value was equal to the value 
  already in its cell)

Miscellaneous 
-------------
