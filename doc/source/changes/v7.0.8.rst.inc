Version 7.0.8
=============

In development

New Features 
------------


Improvements 
------------

- DOC: added new section *Methods And Algorithms* in the documentation.

Fixed Bugs 
----------

- DOC: fixed wrong definition of the keyboard shortcut Alt + F[1-7] 
  in the PDF containing the list of keyboard shortcuts.

- GUI: fixed remembering previously executed commands in the Report Commands 
  console (closes :issue:`1228`)

- PYTHON + GUI: clean the errors stack after the execution of an IODE report 
  or an IODE function/command (closes :issue:`1229`)

- PYTHON: fixed calling `execute_command(f"$WsLoadXxx filepath")` not 
  changing the workspace at the Python level (closes :issue:`1230`)

- PYTHON: fixed IODE command $DataExistXxx returning always 1 (closes :issue:`1237`)

- GUI: fixed insert new IODE tables through the "Add Tables" button 
  (closes issue:`1251`)

Miscellaneous 
-------------

- C++ API: replaced pointer attributes Sample* by smart pointer shared_ptr<Sample>. 
  That should not impact the output of estimations or simulations.

- C++ API: replaced pointer attributes CLEC* by smart pointer shared_ptr<CLEC>. 
  That should not impact the output of estimations or simulations.
