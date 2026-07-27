Version 7.0.9
=============

In development

New Features 
------------


Improvements 
------------


Fixed Bugs 
----------


Miscellaneous 
-------------

- C++ API: The structure handling the *compiled LEC* objects (present in IODE *equations*, 
  *identities* and *tables*) does no longer rely on a unsigned char* buffer to store 
  the compiled LEC expression. That should not impact the output of estimations or simulations. 
