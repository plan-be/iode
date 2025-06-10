.. _cli:

Command Line Interface (iodecmd)
================================

The terminal command `iodecmd` allows the user to execute an *IODE report* without using the 
graphical interface.

Using the IODE report command language, it is possible to automate most operations normally 
performed in the graphical interface.

Syntax::

    iodecmd [-nbrun n] [-alloclog filename] [-v] [-y] [-h] reportfile [arg1] ... [argn]

where:

- ``nbrun n``: n = number of repetitions of the report execution (default 1)
- ``allocdoc filename``: memory debugging info stored in filename (developers only)
- ``v``: verbose mode (all messages displayed)
- ``y``: answer yes to all questions asked during the report execution
- ``h``: display the `iodecmd` options
- ``reportfile arg1 ...``: *IODE report* to be executed including its optional arguments

-nbrun
------

This option allows you to run the same report several times. 
This can be useful to detect instability, a memory leak, or to perform performance tests.

-alloclog filename
------------------

The file filename will contain, at the end of processing, the list of memory allocations 
not freed. This option was created for IODE debugging purposes.

-y or - -forceyes
-----------------

This option allows you to automatically continue execution when a question is asked to the user. 
The automatic answer is always positive.

-v or - -verbose
----------------

This option provides information on the memory state after each iteration.

Example::

    c:\> iodecmd -nbrun 2 -v example.foreach.rep

Result::

    IODE Modeling Software 6.56 - (c) 1990-2017 Planning Office - Brussels
    PIB[1990Y1] = 0 +  + 0 = 
    PIB[1991Y1] = 1 + 0 + 0.84147098 = 1.841471
    ....
    PIB[1999Y1] = 9 + 2.1972246 + 0.41211849 = 11.609343
    PIB[2000Y1] = 10 + 2.3025851 + -0.54402111 = 11.758564
    *****Run 1/2 *****
    rc = 0 -- total residual allocation : 4010740
    PIB[1990Y1] = 0 +  + 0 = 
    PIB[1991Y1] = 1 + 0 + 0.84147098 = 1.841471
    ...
    PIB[1999Y1] = 9 + 2.1972246 + 0.41211849 = 11.609343
    PIB[2000Y1] = 10 + 2.3025851 + -0.54402111 = 11.758564
    *****Run 2/2 *****
    rc = 0 -- total residual allocation : 6124268
    After last run : 6124268 bytes allocated

-h or - -help
-------------

Displays the `iodecmd` options.
