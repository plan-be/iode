Version 7.0-beta.4
==================

In development

New Features 🎉
---------------

  * (GUI) The tables and text editors in tabs can be splitted (#127)

Improvements ✨
---------------


Fixed Bugs 🛠️
-------------

  * (GUI) Opens .rtf files with Microsoft Word by default instead of displaying their content in a new text tab (#291)
  * (GUI) Pressing (SHIFT+)F5 when displaying the variables will move to the (previous) next mode 
          (level, difference, growth rate) (#320)
  * (GUI) Computed tables (from the menu Data > Edit Tables or Print/Graph > Print/View Tables) can now be edited (#210)
  * (GUI) Fixed graphs generated from the menu Print/Graphs > Graphs from Tables 
          when data other than the current workspace are loaded from other files (file [2..5]) (#328)
  * (GUI) Fixed the user interface crashing when clicking twice on the Coefs button after estimating an equation (#340) 
  * (GUI) Allowed to print the results of an estimation, i.e. to print the coefficients, 
          correlation matrix and the tests output (#226)
  * (GUI) Fixed the user interface crashing when entering a wrong pattern to filter the elements of a database (#293)
  * (GUI) Fixed the user interface crashing when running a report while the `filter` field is used in 
          a IODE database tab (#352)

Miscellaneous 🤷‍♀️
----------------

  * Compile the C API using the default alignment for the C structs (#350)
