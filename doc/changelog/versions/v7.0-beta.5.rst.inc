Version7.0-beta.5
=================

In development

New Features 🎉
---------------


Improvements ✨
---------------

  * (GUI) Enabled editing Comments, Identities and Lists in box dialogs (shortcut: CTRL + ENTER / RETURN) (#364) 
  * (GUI) Print the number of IODE objects present in the Workspace in the title of the corresponding tab (#371)
  * (GUI) Enabled to copy/paste variables from Excel to Iode by selecting variable names + values and 
          optionally periods (shortcut: CTRL + SHIFT + V) (381) 


Fixed Bugs 🛠️
-------------

  * (GUI) Fixed table cells edition: if the content starts with a double quote, then it is of type string. 
          Otherwise, it is of type LEC (#363)
  * (GUI) Fixed combobox `Object type` not working when opening the box dialog from the menu 
          Workspace > Copy / Merge Into Workspace (#362)
  * (GUI) Fixed opening an IODE data file when the path to that file contains special characters (#382) 
  * (GUI) Fixed interface crashing at launch when the path to a file previously opened in a tab 
          has been modified when the interface was closed (#397)


Miscellaneous 🤷‍♀️
----------------

