Version7.0-beta.6
=================

Released on 12/02/2024

New Features
------------

  * (GUI) When a new object is created and the filter is active, the name of the new object is added to the filter's pattern
  * (GUI) When an object is deleted and the filter is active, the name of the object is removed from the filter's pattern


Improvements 
------------

  * (GUI) Align values right when displaying numbers (#445)
  * (GUI) Possibility to choose between running reports from the project directory or 
          from their parent directory (#447)
  * (GUI) The graphical user interval remembers the last 20 executed commands when the 
          user reopens it (#386) 


Fixed Bugs 
----------

  * (GUI) Fixed tab 'Tests By Equation' not displaying tests values for all equations 
          when the block to estimate contains more than one equation (#443)
  * (GUI) Do not stretch the last column when displaying numeric values + do not stretch 
          the last row in any case (#446)
  * (GUI) Fixed loss of precision when copying from IODE Variables and pasting to an Excel workbook 
          (values are copied with maximum precision possible) (#448)
  * (GUI) Fixed use the shortcut CTRL (+ ALT) + F1...F7 more than once (cascading) (#464)
  * (GUI) Fixed missing field 'Name' when editing an IODE object (#467)  
  * (GUI) Fixed new object not appearing in the list of IODE objects when the filter is active (#468) 
  * (GUI) Fixed user interface crashing when deleting an IODE object via the IODE command line or by 
          executing an IODE report while a subset of the IODE database is displayed 
          (via the Filter field) (#473)   
