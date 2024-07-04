Version7.0-beta.5
=================

Released on 05/01/2024


Improvements 
------------

  * (GUI) Enabled editing Comments, Identities and Lists in box dialogs (shortcut: CTRL + ENTER / RETURN) (#364) 
  * (GUI) Print the number of IODE objects present in the Workspace in the title of the corresponding tab (#371)
  * (GUI) Enabled to copy/paste variables from Excel to Iode by selecting variable names + values and 
          optionally periods (shortcut: CTRL + SHIFT + V) (#381) 
  * (GUI) Enabled to (de)increase the size of the columns of numerical tables by pressing (SHIFT-)F3 (#418) 
  * (GUI) Allow users to (dis)enable the scientific format for numbers (#417)


Fixed Bugs 
----------

  * (GUI) Fixed table cells edition: if the content starts with a double quote, then it is of type string. 
          Otherwise, it is of type LEC (#363)
  * (GUI) Fixed combobox `Object type` not working when opening the box dialog from the menu 
          Workspace > Copy / Merge Into Workspace (#362)
  * (GUI) Fixed opening an IODE data file when the path to that file contains special characters (#382) 
  * (GUI) Fixed interface crashing at launch when the path to a file previously opened in a tab 
          has been modified when the interface was closed (#397)
  * (GUI) Fixed IODE objects tabs displaying the wrong filename when the filter is used (#398)
  * (GUI) Fixed interface crashing when the user tries to clear a KDB while the filter is used (#404) 
  * (GUI) Fixed copy/paste variables from Excel returning an error when the variable's name contains 
          leading and/or trailing spaces (#414)
  * (GUI) Fixed running an IODE report when the path or the filename contains special characters (#415)
  * (GUI) Fixed estimation not running if the block contained more than one equation (#433)
