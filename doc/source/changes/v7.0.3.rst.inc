Version 7.0.3
=============

In development

New Features 
------------

- allowed the users to choose the displayed colors in the IODE report tabs 
  in the Graphical User Interface (GUI) (closes :issue:`1000`)

Improvements 
------------

- Right-clicking on a Identity or a Table in the Graphical User Interface (GUI) 
  now shows a context menu with relevant possible actions (closes :issue:`1037`)
- Avoid warning message saying there is no estimation sample defined when 
  opening the Edit Equation window for an equation for which the estimation sample 
  has not been specified yet in the Graphical User Interface (GUI) (closes :issue:`1040`)

Fixed Bugs 
----------

- forced to use dot as decimal separator in the 
  Graphical User Interface (GUI) for double values (closes :issue:`984`)
- fixed methods :meth:`Variables.from_frame` and :meth:`Variables.from_array` systematically 
  overriding the sample of the *variables* workspace (closes :issue:`998`)
- fixed the identity execution in the Graphical User Interface (GUI) to allow
  the user to specify a period range for the execution (closes :issue:`1005`)
- fixed *mode* not reset to LEVEL when a Variables database is loaded or cleared in the
  Graphical User Interface (GUI) (closes :issue:`1007`)
- allowed users to display Computed Table windows to in full screen in the Graphical User 
  Interface (GUI) (closes :issue:`1033`)
- fixed error message displayed when trying to open a Computed Table window using the 
  IODE report command `$ViewVar` via the Graphical User Interface (GUI) (closes :issue:`1032`)

Miscellaneous 
-------------

