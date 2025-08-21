.. _reports_functions:

Functions
---------

These functions allow you to perform a whole series of operations, including:

- string or list of strings processing,
- text replacement,
- object counting,
- text file processing,
- etc.

These functions are executed within a report line and the result of the function is printed 
in the report output. They can be chained (e.g. `@upper(@ttitle(T1))` returns the title of the 
table named *T1* in uppercase).

The general syntax of these functions is::

    @function_name(arg1, arg2, ...)

String management
~~~~~~~~~~~~~~~~~

- :ref:`@upper(text) <upper>`: converts text to uppercase
- :ref:`@lower(text) <lower>`: converts text to lowercase
- :ref:`@replace(string, from, to) <replace>`: substitutes one text for another
- :ref:`@fmt(val, fmt) <fmt>`: formats an integer
- :ref:`@take(n, string) <take>`: extracts the first n characters of string. If n is negative, extracts the last n characters.
- :ref:`@drop(n, string) <drop>`: removes the first n characters of string. If n is negative, removes the last n characters.
- :ref:`@count(string) <count>`: returns the number of elements in string (separated by commas)
- :ref:`@index(n, list) <index>`: returns the nth element of list
- :ref:`@sqz(string) <sqz>`: removes spaces from string
- :ref:`@strip(string) <strip>`: removes trailing spaces from string
- :ref:`@ansi(text) <ansi>`: converts an Ascii text to Ansi
- :ref:`@equal(t1, t2) <equal>`: compares t1 and t2: returns 1 if equal, 0 otherwise
- :ref:`@void(t1, ...) <void>`: returns no text, whatever the arguments

List of strings management
~~~~~~~~~~~~~~~~~~~~~~~~~~

- :ref:`@vtake(n, list) <vtake>`: take the first n elements of the list (or last n elements if n is negative)
- :ref:`@vdrop(n, list) <vdrop>`: drop the first n elements of the list (or last n elements if n is negative)
- :ref:`@vcount(list) <vcount>`: return the number of elements in the list

File management
~~~~~~~~~~~~~~~

- :ref:`@fdelete(filename) <fdelete>`: deletes the file filename
- :ref:`@fappend(filename, string|NL, ...) <fappend>`: writes the text to a file

Directory management
~~~~~~~~~~~~~~~~~~~~

- :ref:`@getdir() <getdir>`: returns the current directory
- :ref:`@chdir(dirname) <chdir>`: changes the current directory to dirname and returns the new current directory
- :ref:`@mkdir(dirname) <mkdir>`: creates a new directory dirname
- :ref:`@rmdir(dirname) <rmdir>`: deletes the directory dirname

Dates and times
~~~~~~~~~~~~~~~

- :ref:`@date([format]) <date>`: returns the date
- :ref:`@time([format]) <time>`: returns the time
- :ref:`@month(month, language) <month>`: returns the text of the month in the given language
- :ref:`@ChronoReset() <chronoreset>`: resets the timer to 0
- :ref:`@ChronoGet() <chronoget>`: returns the elapsed time (in msecs) since the last timer reset

Object lists
~~~~~~~~~~~~

- :ref:`@cexpand(pattern, ...) <cexpand>`: returns the list of comments matching pattern
- :ref:`@eexpand(pattern, ...) <eexpand>`: returns the list of equations matching pattern
- :ref:`@iexpand(pattern, ...) <iexpand>`: returns the list of identities matching pattern
- :ref:`@lexpand(pattern, ...) <lexpand>`: returns the list of lists matching pattern
- :ref:`@sexpand(pattern, ...) <sexpand>`: returns the list of scalars matching pattern
- :ref:`@texpand(pattern, ...) <texpand>`: returns the list of tables matching pattern
- :ref:`@vexpand(pattern, ...) <vexpand>`: returns the list of variables matching pattern
- :ref:`@vliste(objname, ...) <vliste>`: returns the list of variables in the eqs objname
- :ref:`@sliste(objname, ...) <sliste>`: returns the list of scalars in the eqs objname

Object content
~~~~~~~~~~~~~~

- :ref:`@ttitle(tablename,tablename, ...) <ttitle>`: returns the titles of the tables
- :ref:`@srelax(sclname,sclname, ...) <srelax>`: returns the relax value for scalars
- :ref:`@sstderr(sclname,sclname, ...) <sstderr>`: returns the stderr value for scalars
- :ref:`@cvalue(cmtname,cmtname, ...) <cvalue>`: returns the text of a comment
- :ref:`@vvalue(varname,varname, ...) <vvalue>`: returns the values of variables as text
- :ref:`@sample(B|E|) <sample>`: returns the (beginning/ending of) the current sample as text

Equation content
~~~~~~~~~~~~~~~~

- :ref:`@evalue(eqname, eqname, ...) <evalue>`: returns the LEC text of an equation
- :ref:`@eqsample(eqname) <eqsample>`: returns the estimation sample of the equation eqname
- :ref:`@eqsamplefrom(eqname) <eqsamplefrom>`: returns the FROM part of the estimation sample
- :ref:`@eqsampleto(eqname) <eqsampleto>`: returns the TO part of the estimation sample
- :ref:`@eqlhs(eqname) <eqlhs>`: returns the left-hand side of an equation
- :ref:`@eqrhs(eqname) <eqrhs>`: returns the right-hand side of an equation

Simulations
~~~~~~~~~~~

The following functions allow you to obtain the value of certain simulation parameters:

- :ref:`@SimEps() <simeps>`: returns the value of the convergence criterion used for the last simulation
- :ref:`@SimRelax() <simrelax>`: returns the value of the relaxation parameter used for the last simulation
- :ref:`@SimMaxit() <simmaxit>`: returns the value of the maximum number of iterations used for the last simulation

The simulation results by period can be retrieved using the following functions:

- :ref:`@SimNiter(period) <simniter>`: number of iterations required to solve the model for year period
- :ref:`@SimNorm(period) <simnorm>`: convergence threshold reached when solving the model for year period


.. _upper:

FUNCTION @UPPER
~~~~~~~~~~~~~~~

This function converts the text passed as an argument to uppercase.

Syntax::

     @upper(text)

Example::

     @upper("free text")

Result::

     FREE TEXT

.. _lower:

FUNCTION @LOWER
~~~~~~~~~~~~~~~

This function converts the text passed as an argument to lowercase.

Syntax::

     @lower(text) 

Example::

     @lower("Free Text")

Result::

     free text

.. _replace:

FUNCTION @REPLACE
~~~~~~~~~~~~~~~~~

This function replaces one string with another.
The replacement is case-sensitive.

Syntax::

     @replace(text,replace_from,replace_by)

Example::

     @replace("free text",text,hugs)

Result::

     free hugs

.. _fmt:

FUNCTION @FMT
~~~~~~~~~~~~~

Formats an integer *val* according to a given format *fmt*.
The result is a string transformed to the same length as *fmt*.
The recognized characters in the format are: ``X``, ``x``, ``9``, and ``0``.
They mean that only at the positions of these characters will the characters resulting 
from formatting *val* be placed, in their order of appearance.
Special case: the character ``0`` will be replaced by a 0 if the corresponding character 
in the formatting of *val* is a whitespace.

Syntax::

     @fmt(val,fmt)  
     where val = integer value  
           fmt = format 
      
Example::

     @fmt(123,0009)     ---> 0123  
     @fmt(123,A0000A)   ---> A00123A

.. warning:: 
      
     Whitespaces before and after the format are included in the result.

.. _take:

FUNCTION @TAKE
~~~~~~~~~~~~~~

Extracts the first *n* characters of a string.
If *n* is negative, extracts the last *n* characters.

Syntax::

     @take(n,text)

Example::

     @take(3,IODE)  
     @take(-3,IODE)

Result::

     IOD  
     ODE

.. _drop:

FUNCTION @DROP
~~~~~~~~~~~~~~

Removes the first *n* characters from a string.
If *n* is negative, removes the last *n* characters.

Syntax::

     @drop(n,text)

Example::

     @drop(2,IODE)  
     @drop(-2,IODE)

Result:: 
     
     DE  
     IO

.. _count:

FUNCTION @COUNT
~~~~~~~~~~~~~~~

Returns the number of elements in a string (these elements are separated by commas ``,`` 
in the string).

Syntax::

     @count(list)

Example::

     @count(A,B,C,E)  
     @count(ABC,,,B)

Result::

     4  
     2

.. _index:

FUNCTION @INDEX
~~~~~~~~~~~~~~~

Returns the nth element of an IODE list.

Syntax::

     @index(n,list)

Example::

     @index(2,A,B,C,E)  
     @index(1,ABC,,,B)

Result::

     B  
     ABC

.. _sqz:

FUNCTION @SQZ
~~~~~~~~~~~~~

Removes whitespaces from a string.

Syntax::

      @sqz(string)

Example::

      @sqz(' ABC D  ')   ---> 'ABCD'

.. _strip:

FUNCTION @STRIP
~~~~~~~~~~~~~~~

Removes trailing whitespaces from a string.

Syntax::

      @strip(string)

Example::

      @strip(' ABC D  ')   ---> ' ABC D'

.. _ansi:

FUNCTION @ANSI
~~~~~~~~~~~~~~

This function converts a string encoded in ASCII to characters encoded in ANSI.
It is useful, for example, when generating output files that must be strictly encoded 
in ANSI (such as HTML files).

Syntax::

     @ansi(text)

Example::

     <TC>@ansi(Février)</TC>

Result::
      
     <TC>Février</TC>

.. _equal:

FUNCTION @EQUAL
~~~~~~~~~~~~~~~

Checks if two values are identical.

Syntax::

     equal(a,b)  
     where a and b = any text

Example::

     @equal(123,123)    ---> 1 (True)
     @equal(123, 123)   ---> 0 (False because of the whitespace)
     @equal(%i%,10)     ---> 1 or 0 (True or False depending on the value of i)

.. warning:: 
     
     Whitespaces are counted as characters.

.. _void:

FUNCTION @VOID(ARGS)
~~~~~~~~~~~~~~~~~~~~

Empties the text of its arguments, regardless of what the arguments are.
For example, calling @chdir() returns the name of the new current directory.
With @void(), this text does not appear in the output.

Example::

     Here is the result of chdir()  : @chdir(..)  
     Here is the result with void() : @void(@chdir(..))

Result::

     Here is the result of chdir()  : \usr\iode  
     Here is the result with void() :

.. _vtake:

FUNCTION @VTAKE(N,VALUES)
~~~~~~~~~~~~~~~~~~~~~~~~~

Keeps only the first *n* strings from *values*. If *n* is negative, keeps the last *n*.
The separators between the strings in *values* are defined by the command :ref:`$vseps <vseps>`. 
By default, these are ``,;`` and the whitespace. 
The result is an IODE list whose separator is the first one defined by :ref:`$vseps <vseps>`.

.. warning::

     The comma ``,`` is always a separator, even if it is not included in :ref:`$vseps <vseps>`.

Syntax::

    @vtake(n,values)

Example::

$show @vtake(1,A,B,C)   -> A  
$show @vtake(-1,A,B,C)  -> C  
$show @vdrop(-1,A,B,C)  -> A,B  
$show @vdrop(2,A,B,C)   -> C  
$vseps |  
$show @vtake(1,A B|C)   -> A B  
$show @vtake(1,"A,B"|C) -> A,B  
$show @vdrop(-1,A,B,C)  -> A|B

Example::

     $vseps ;  
     $define LIST A;B;C;D  
     $label next  
     $define ELEMENT @vtake(1, %LIST%)  
        ... Some operation on element %ELEMENT% ...  
     $define LIST @vdrop(1,%LIST%)  
     $goto next @vcount(%LIST%)

.. _vdrop:

FUNCTION @VDROP(N,VALUES)
~~~~~~~~~~~~~~~~~~~~~~~~~

See :ref:`@vtake <vtake>`.

.. _vcount:

FUNCTION @VCOUNT(N,VALUES)
~~~~~~~~~~~~~~~~~~~~~~~~~~

This function returns the number of elements in a list of strings.

Syntax::

     @vcount({list of strings}) 
     
Example::

     $vseps |  
     $show @vcount(A B|C)   -> 2  
     $show @vcount(A;B;C)   -> 1

See also :ref:`@vtake <vtake>`.

.. _fdelete:

FUNCTION @FDELETE
~~~~~~~~~~~~~~~~~

This function deletes a file. It does not return any result.

Syntax::

     @fdelete(filename)

Example::

     @fdelete(test.htm)

Result::

     none -> The file test.htm is deleted.

.. _fappend:

FUNCTION @FAPPEND
~~~~~~~~~~~~~~~~~

This function appends strings to the content of an ASCII file.
It does not return any result.

Syntax::

     @fappend(filename,text|NL,text,...)  
     where NL indicates a line break 
     
Example::

      Report test.rep  
      ----------------  
      @fappend(test.htm,"Data file",NL)  
      @fappend(test.htm,"Parameters:",%*%)  
      
      Call  
      -----  
      test A B C

Result::

      File test.htm  
      ----------------  
      Data file  
      Parameters:A B C

.. _getdir:

FUNCTION @GETDIR ()
~~~~~~~~~~~~~~~~~~~

Returns the current directory.

Example::

     Current directory: @getdir()

Result::

     Current directory: c:\usr\iode

.. _chdir:

FUNCTION @CHDIR(DIRNAME)
~~~~~~~~~~~~~~~~~~~~~~~~

Changes the current directory to dirname and returns the new current directory.

Example::

     Current directory: @getdir()  
     New directory: @chdir(..)

Result::

     Current directory: c:\usr\iode  
     New directory: c:\usr

.. _mkdir:

FUNCTION @MKDIR(DIRNAME)
~~~~~~~~~~~~~~~~~~~~~~~~

Creates a new directory named dirname.
Returns no value.

Example::

     Current directory:         @getdir()
     Creating subdir:           @mkdir(subdir)
     New current directory:     @getdir()

Result::

     Current directory:         c:\usr\iode
     Creating subdir
     New current directory:     c:\usr\iode\subdir

.. _rmdir:

FUNCTION @RMDIR(DIRNAME)
~~~~~~~~~~~~~~~~~~~~~~~~

Deletes the directory dirname.
Returns no value.

.. warning::

     - The directory must be empty before it can be deleted.
     - Use with caution...

.. _date:

FUNCTION @DATE
~~~~~~~~~~~~~~

This function returns the current date.

Syntax::

     @date([format])  
     where format indicates the date format  
     By default, the format is dd-mm-yyyy

Example::

     Today's date is @date()  
     Other format: @date("dd/mm/yy")

Result::

     Today's date is 31-05-1999  
     Other format: 31/05/99 

.. _time:

FUNCTION @TIME
~~~~~~~~~~~~~~

This function returns the current time.

Syntax::

     @time([format])  
     where format indicates the time format  
     By default, the format is hh:mm:ss

Example::

     The time is @time()  
     Other format: @time("hh hours mm minutes")

Result::

     The time is 23:12:55  
     Other format: 23 hours 12 minutes

.. _month:

FUNCTION @MONTH
~~~~~~~~~~~~~~~

This function returns the name of a month in a given language.

Syntax::

     @month(month[,language])  
     where language is F, N or E (E by default)

Example::

     The name of month number 3 is: @month(3)  
     In French and uppercase: @upper(@month(3,F))  
     In Dutch and lowercase: @lower(@month(3,N))

Result::

     The name of month number 3 is: March  
     In French and uppercase: MARS  
     In Dutch and lowercase: maart

.. _chronoreset:

FUNCTION @CHRONORESET()
~~~~~~~~~~~~~~~~~~~~~~~

A timer has been added to allow calculation of processing durations.
`@ChronoReset()` resets the timer to 0.
See example in the function :ref:`@SimEps <simeps>`.

.. _chronoget:

FUNCTION @CHRONOGET()
~~~~~~~~~~~~~~~~~~~~~

Returns the elapsed time (in milliseconds) since the last call to 
:ref:`@ChronoReset <chronoreset>`. See example in the function :ref:`@SimEps <simeps>`.

.. _cexpand:

FUNCTION @CEXPAND
~~~~~~~~~~~~~~~~~

This function returns the list of IODE comments whose name matches one of the strings 
passed as an argument.

Syntax::

     @cexpand(pattern1,pattern2,...)

Example::

     @cexpand(A*,B*,AE)

Result::

     A1;A2;BA;BCXS;AE

The line::

     $Repeat "$DataDuplicateVar _ _1" @vexpand(*)

executes the :ref:`$DataDuplicateVar <dataduplicatevar>` command on all variables in the 
workspace (the new variables are named after the original variable with 1 as a suffix).

See also :ref:`@eexpand <eexpand>`, :ref:`@iexpand <iexpand>`, :ref:`@lexpand <lexpand>`, 
:ref:`@sexpand <sexpand>`, :ref:`@texpand <texpand>` and :ref:`@vexpand <vexpand>`

.. _eexpand:

FUNCTION @EEXPAND
~~~~~~~~~~~~~~~~~

This function returns the list of equations whose name matches one of the strings 
passed as an argument.

Syntax::

     @eexpand(pattern1,pattern2,...)

Example::

     @eexpand(X*)

Result::

     All equations whose name starts with X

See also :ref:`@cexpand <cexpand>`, :ref:`@iexpand <iexpand>`, :ref:`@lexpand <lexpand>`, 
:ref:`@sexpand <sexpand>`, :ref:`@texpand <texpand>` and :ref:`@vexpand <vexpand>`

.. _iexpand:

FUNCTION @IEXPAND
~~~~~~~~~~~~~~~~~

This function returns the list of IODE identities whose name matches one of the strings 
passed as an argument.

Syntax::

     @iexpand(pattern1,pattern2,...)

Example::

     @iexpand(*_1)

Result::

     All identities whose name ends with _1

See also :ref:`@cexpand <cexpand>`, :ref:`@eexpand <eexpand>`, :ref:`@lexpand <lexpand>`, 
:ref:`@sexpand <sexpand>`, :ref:`@texpand <texpand>` and :ref:`@vexpand <vexpand>`

.. _lexpand:

FUNCTION @LEXPAND
~~~~~~~~~~~~~~~~~

This function returns the list of IODE lists whose name matches one of the strings 
passed as an argument.

Syntax::

     @lexpand(pattern1,pattern2,...)

Example::

     $DataUpdateLst list1 @lexpand(_*)

creates an IODE list *list1* containing all lists starting with ``_``.

See also :ref:`@cexpand <cexpand>`, :ref:`@eexpand <eexpand>`, :ref:`@iexpand <iexpand>`, 
:ref:`@sexpand <sexpand>`, :ref:`@texpand <texpand>` and :ref:`@vexpand <vexpand>`

.. _sexpand:

FUNCTION @SEXPAND
~~~~~~~~~~~~~~~~~

This function returns the list of IODE scalars whose name matches one of the strings 
passed as an argument.

Syntax::

     @sexpand(pattern1,pattern2,...)

Example::

     @sexpand(*)

returns the list of all IODE scalars in the workspace.

See also :ref:`@cexpand <cexpand>`, :ref:`@eexpand <eexpand>`, :ref:`@iexpand <iexpand>`, 
:ref:`@lexpand <lexpand>`, :ref:`@texpand <texpand>` and :ref:`@vexpand <vexpand>`

.. _texpand:

FUNCTION @TEXPAND
~~~~~~~~~~~~~~~~~

This function returns the list of IODE tables whose name matches one of the strings 
passed as an argument.

Syntax::

     @texpand(pattern1,pattern2,...)

Example::

     $PrintTbl 1990:5 @texpand(*)

prints all IODE tables in the workspace for the sample 1990 to 1994.

See also :ref:`@cexpand <cexpand>`, :ref:`@eexpand <eexpand>`, :ref:`@iexpand <iexpand>`, 
:ref:`@lexpand <lexpand>`, :ref:`@sexpand <sexpand>` and :ref:`@vexpand <vexpand>`

.. _vexpand:

FUNCTION @VEXPAND
~~~~~~~~~~~~~~~~~

This function returns the list of IODE variables whose name matches one of the strings 
passed as an argument.

Syntax::

      @vexpand(pattern1,pattern2,...)

Example::

      $DataUpdateTbl NEWTBL TITLE;@vexpand(BE*)

creates a table *NEWTBL* containing all series starting with BE.

See also :ref:`@cexpand <cexpand>`, :ref:`@eexpand <eexpand>`, :ref:`@iexpand <iexpand>`, 
:ref:`@lexpand <lexpand>`, :ref:`@sexpand <sexpand>` and :ref:`@texpand <texpand>`

.. _vliste:

FUNCTION @VLISTE
~~~~~~~~~~~~~~~~

This function returns the list of IODE variables used in the equations whose names are 
passed as arguments.

Syntax::

     @vliste(eq1,eq2,...)

Example::

     $DataUpdateLst VARLST @vliste(@eexpand(*))

creates the list *VARLST* containing all series used in the equations of the workspace.

See also :ref:`@sliste <sliste>`.

.. _sliste:

FUNCTION @SLISTE
~~~~~~~~~~~~~~~~

This function returns the list of IODE scalars used in the equations whose names are 
passed as arguments.

Syntax::

     @sliste(eq1,eq2,...)

Example::

     $DataUpdateLst ScalarLST @sliste(@eexpand(*))

creates the list *ScalarLST* containing all IODE scalars used in the equations of the workspace.

See also :ref:`@vliste <vliste>`.

.. _ttitle:

FUNCTION @TTITLE
~~~~~~~~~~~~~~~~

This function returns the titles of the tables passed as arguments.

Syntax::

     @ttitle(tbl1,tbl2,...)

Example::

     @ttitle(tbl)

Result::

     Table title

.. _srelax:

FUNCTION @SRELAX
~~~~~~~~~~~~~~~~

This function returns the relaxation parameter values of the IODE scalars passed as arguments.

Syntax::

     @srelax(scl1,scl2,...)

Example::

     @srelax(scl)

Result::

     0.9

.. _sstderr:

FUNCTION @SSTDERR
~~~~~~~~~~~~~~~~~

This function returns the standard deviation values of the IODE scalars 
passed as arguments.

Syntax::

     @sstderr(scl1,scl2,...)

Example::

     @sstderr(scl1)

Result::

     0.001

.. _cvalue:

FUNCTION @CVALUE
~~~~~~~~~~~~~~~~

This function returns the text of the comments whose names are passed as arguments.

Syntax::

     @cvalue(cmtname,cmtname,...)

Example::

     @cvalue(CMT1,A)

Result::

     Comment 1;Comment for A

.. _vvalue:

FUNCTION @VVALUE
~~~~~~~~~~~~~~~~

This function returns the formatted values of the variables whose names are 
passed as arguments.

Syntax::

     @vvalue(varname,varname,...)

Example::

     @vvalue(A,B)

Result::

     na na 1.2342 -1.22323 1000 1001 1002 1003

.. _sample:

FUNCTION @SAMPLE
~~~~~~~~~~~~~~~~

This function returns the current sample.

Syntax::

     @sample(B|E|)  
          B : first period  
          E : last period  
          empty : first and last periods

Example::

     @sample()  
     @sample(B)  
     @sample(E)

Result::

     1960Y1 1990Y1  
     1960Y1  
     1990Y1

.. _evalue:

FUNCTION @EVALUE
~~~~~~~~~~~~~~~~

This function returns the LEC formula defining an equation.

Syntax::

     @evalue(eqname,eqname,...)

Example::

     @evalue(EQ1)

Result::

     log(EQ1) := c1 + c2 * V3 + c3 * X

.. _eqsample:

FUNCTION @EQSAMPLE(EQNAME)
~~~~~~~~~~~~~~~~~~~~~~~~~~

Returns the estimation sample of the equation *eqname*.

.. _eqsamplefrom:

FUNCTION @EQSAMPLEFROM(EQNAME)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Returns the *FROM* part of the estimation sample.

In the following example, the first estimation year of the equation *ENDO1* is extracted 
in order to re-estimate the equation on a sample extended to the right (for example, after 
obtaining new observations).

Example::

     $EqsEstimate @eqsamplefrom(ENDO1) 2012Y1 ENDO1

.. _eqsampleto:

FUNCTION @EQSAMPLETO(EQNAME)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Returns the *TO* part of the estimation sample.

See example in :ref:`@EqSampleFrom <eqsamplefrom>`.

.. _eqlhs:

FUNCTION @EQLHS(EQNAME)
~~~~~~~~~~~~~~~~~~~~~~~

Returns the left-hand side of an equation.

.. _eqrhs:

FUNCTION @EQRHS(EQNAME)
~~~~~~~~~~~~~~~~~~~~~~~

Returns the right-hand side of an equation.

.. _sqlopen:

FUNCTION @SQLOPEN
~~~~~~~~~~~~~~~~~

.. warning::

     OBSOLETE

Opens an ODBC session with a database. 
The name of the database (DSN name) you provide is the name with which the database 
is defined in your ODBC environment (Start > Settings > Control Panel > ODBC 32). 
For some databases, a username and password are required. 
On success, you get "1" as a result, otherwise "0".

Syntax::

     @SqlOpen(DSN-name[,Username,Password])

Example::

     $goto continue, @SqlOpen(RSZ)  
     $Show  ODBC-Database not opened  
     $Return  
     
     $label continue  
     $Show  ODBC-Database opened  
     ....  
     
     @SqlClose()  
 
.. _sqlquery:

FUNCTION @SQLQUERY
~~~~~~~~~~~~~~~~~~

.. warning::

     OBSOLETE

This command executes an SQL query on your database. 
If the command succeeds, a cursor is created on the "Dynaset" which you can iterate through 
with SqlNext, until the end of the available data. Place the SQL command between quotes so 
there is no confusion if you use special characters such as `,` or `)` in your SQL command. 
Also, group the entire command on one line.
On success, you get the number of columns of the "Dynaset" as a result, otherwise "0".

.. warning::

     You must use @SqlNext() after @SqlQuery() if you want to use the fields of a record.

Syntax::

     @SqlQuery("SQL-command")

Example::

     $goto continue, @SqlOpen(RSZ)  
     $Show  ODBC-Database not opened  
     $Return  
     
     $label continue  
     $Show  ODBC-Database opened  
     $Show  @SqlQuery("SELECT DISTINCT TRIM FROM RSZ_bruto;")  
     ....  
     
     @SqlClose()  
 
.. _sqlnext:

FUNCTION @SQLNEXT
~~~~~~~~~~~~~~~~~

.. warning::

     OBSOLETE

This command iterates through the "Dynaset" created by the last SQL command. 
As long as there are records available in the "Dynaset" you get "1" as a result, 
otherwise "0". On success, you get "1" as a result, otherwise "0".

.. warning::

     You must use @SqlNext() after @SqlQuery() if you want to use the fields of a record.

Syntax::

     @SqlNext() 
     
Example::

     $goto continue, @SqlOpen(RSZ)  
     $Show  ODBC-Database not opened  
     $Return  
     
     $label continue  
     $Show  ODBC-Database opened  
     $Show  @SqlQuery("SELECT DISTINCT TRIM FROM RSZ_bruto;")  
     @SqlNext()  
     $Define BFLD @SqlField(0)  
     
     $label again_date  
     $Show Skipping @SqlField(0)  
     $goto again_date, @SqlNext()  
     $Define EFLD @SqlField(0)  
     
     $Define BEGIN @replace(%BFLD%,/,Q)  
     $Define END   @replace(%EFLD%,/,Q)  
     
     $WsSample %BEGIN% %END%  
     ....  
     
     @SqlClose()  
 
.. _sqlfield:

FUNCTION @SQLFIELD
~~~~~~~~~~~~~~~~~~

.. warning::

     OBSOLETE

With this command you get the content of the nth field of the current record.
On success, you get the content of the field, otherwise "0".

.. warning::

     You must use @SqlNext() before SqlField() or SqlRecord() works.

Syntax::

     @SqlField(field-number) 
     
Example::

     $goto continue, @SqlOpen(RSZ)  
     $Show  ODBC-Database not opened  
     $Return  
     
     $label continue  
     $Show  ODBC-Database opened  
     $Show  @SqlQuery("SELECT DISTINCT TRIM FROM RSZ_bruto;")  
     @SqlNext()  
     $Define BFLD @SqlField(0)  
     
     $label again_date  
     $Show Skipping @SqlField(0)  
     $goto again_date, @SqlNext()  
     $Define EFLD @SqlField(0)  
     
     $Define BEGIN @replace(%BFLD%,/,Q)  
     $Define END   @replace(%EFLD%,/,Q)  
     
     $WsSample %BEGIN% %END%  
     ....  
     
     @SqlClose()  
 
.. _sqlrecord:

FUNCTION @SQLRECORD
~~~~~~~~~~~~~~~~~~~

.. warning::

     OBSOLETE

With this command you get the content of the current record. You get all fields if you do 
not pass any arguments to the function, or all fields from a certain column with one 
argument, or all fields between two columns if you provide two arguments. On success, 
you get the content of the fields, otherwise "0".

.. warning::

     You must use @SqlNext() before SqlField() or SqlRecord() works.

Syntax::

     @SqlRecord() : all fields  
     @SqlRecord(field1-number) : all fields from field1  
     @SqlRecord(field1-number,field2-number) : all fields between field1 and field2 
     
Example::

     $goto continue, @SqlOpen(RSZ)  
     $Show  ODBC-Database not opened  
     $Return  
     
     $label continue  
     $Show  ODBC-Database opened  
     
     $Define PIVOT RETOMA_DBF  
     $Define PREP  RT  
     
     $Msg  NBCOLS @SqlQuery("TRANSFORM Sum(RSZ_bruto.%PIVOT%) AS AGGR SELECT 
     RSZ_bruto.CODRED, NaceTbl.NACE FROM NaceTbl INNER JOIN RSZ_bruto ON 
     NaceTbl.FICTIF=RSZ_bruto.FICTIF GROUP BY RSZ_bruto.CODRED, NaceTbl.NACE PIVOT 
     RSZ_bruto.TRIM;")
 
     $Show  @SqlNext()  
     $label again  
     $DataUpdateVar %PREP%@SqlField(0)@SqlField(1) %BEGIN% @SqlRecord(2, 8)  
     $goto again, @SqlNext()  
     
     ...  
     @SqlClose()  
 
.. _sqlclose:

FUNCTION @SQLCLOSE
~~~~~~~~~~~~~~~~~~

.. warning::

     OBSOLETE

Closes the current ODBC session.
On success, you get "1" as a result, otherwise "0".

Syntax::

     @SqlClose()

Example::

     $goto continue, @SqlOpen(RSZ)  
     $Show  ODBC-Database not opened  
     $Return  
     
     $label continue  
     $Show  ODBC-Database opened  
     ....  
     
     @SqlClose()  

.. _simeps:

FUNCTION @SIMEPS
~~~~~~~~~~~~~~~~

Returns the value of the convergence criterion used for the last simulation.

The example below compares performance according to the number of sorts in the 
simulation algorithm.

Example::

     $ Parameters  
     $ ----------  
     $define modeldir C:\iode\nemesis  
     $define model base_neujobs_rd  
     $define nbtri 5  
     $define simfrom 2011Y1  
     $define simto 2012Y1  
     $define simper %simfrom% %simto%  
     $ModelSimulateParms 0.00001 0.6 500 Both 0 No 1  
           
     $ Output file  
     $ -----------  
     $PrintDest compare.html HTML  
           
     $ Load fresh files  
     $ ----------------  
     $WsLoadVar %modeldir%\%model%  
     $WsLoadScl %modeldir%\%model%  
     $WsLoadEqs %modeldir%\%model%  
           
     $ ==== $ModelCalcSCC ===  
     .par1 tit_1  
     Comparison of performance according to the number of sorts  
           
     $ ==== Loop on Tri tests ===  
     $define i 0  
     $label nexttri  
           
     $ SCC Calculation  
     $ --------------  
     $show $ModelCalcSCC %i%  
     @ChronoReset()  
     $ModelCalcSCC %i% pre inter post  
     $define cpu1  @ChronoGet()  
           
     $ Simulate  
     $ --------  
     $ Reload vars for a clean start and modify exo  
     $show Reload Vars ...  
     $WsLoadVar %modeldir%\%model%  
     $DataUpdateVar RPOIL 2011Y1 1.20  
           
     $show $ModelSimulateSCC  %simper% pre inter post  
     @ChronoReset()  
     $ModelSimulateSCC %simper% pre inter post  
     $define cpu2 @ChronoGet()  
           
     $ Reporting  
     $ ---------  
     .par1 enum_1  
     Sorts : %i%  
     .par1 enum_2  
     SCC Calculation : %cpu1% msec  
     .par1 enum_2  
     Simulation (eps = @simeps(); maxit=@simmaxit()) : %cpu2% msec  
           
     $define j {%simfrom%}   209  $define totit 0  
           
     $label nextsimper  
     .par1 enum_3  
     {%j%@T} : conv = @simnorm({%j%@T}), niter = @simniter({%j%@T})  
     $define simit @simniter({%j%@T})  
     $define totit {%totit% + %simit%}  
     $define j {%j% + 1}  
     $goto nextsimper {%j% <= %simto%}  
     .par1 enum_3  
     Total iterations : %totit%  
     $define i {%i% + 1}  
     $goto nexttri {%i% <= %nbtri%}

.. _simrelax:

FUNCTION @SIMRELAX()
~~~~~~~~~~~~~~~~~~~~

Returns the value of the relaxation parameter used for the last simulation.

See example in the function :ref:`@SimEps <simeps>`.

.. _simmaxit:

FUNCTION @SIMMAXIT()
~~~~~~~~~~~~~~~~~~~~

Returns the value of the maximum number of iterations used for the last simulation.

See example in the function :ref:`@SimEps <simeps>`.

.. _simniter:

FUNCTION @SIMNITER(PERIOD)
~~~~~~~~~~~~~~~~~~~~~~~~~~

Returns the number of iterations required to solve the model for year *period*.

See example in the function :ref:`@SimEps <simeps>`.

.. _simnorm:

FUNCTION @SIMNORM (PERIOD)
~~~~~~~~~~~~~~~~~~~~~~~~~~

Returns the convergence threshold reached when solving the model for year *period*.

See example in the function :ref:`@SimEps <simeps>`.

Python equivalent functions
~~~~~~~~~~~~~~~~~~~~~~~~~~~

To get the Python equivalent of the above functions, refers to the 
:ref:`Equivalence Between IODE Report And Python Functions <report_funcs_equivalence>` table.
