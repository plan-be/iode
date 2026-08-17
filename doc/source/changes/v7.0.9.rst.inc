Version 7.0.9
=============

In development


Miscellaneous 
-------------

- PYTHON: updated required version of pandas to 3.0 or higher (closes :issue:`1222`)

- C++ API: The structure handling the *compiled LEC* objects (present in IODE *equations*, 
  *identities* and *tables*) does no longer rely on a unsigned char* buffer to store 
  the compiled LEC expression. That should not impact the output of estimations or simulations. 
