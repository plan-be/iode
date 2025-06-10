.. _iode_reports:

IODE Reports
============

*IODE reports* are special files with an extension ``.rep`` that contain a set of instructions 
and macros. They are used to automate tasks, generate documents, and control the execution of 
various operations within IODE.

The execution of a report can be controlled interactively (`ASK` and `PROMPT` functions) or 
programmatically (`GOTO`). By using macros (`define`) and report arguments, the same report 
can be used in different situations.

The present section is split into the following sub-sections:

- :ref:`Reports Definition <reports_definition>`
- :ref:`Reports Structure <reports_structure>`
- :ref:`Execution Commands <reports_execution_commands>`
- :ref:`Functions <reports_functions>`
- :ref:`IODE Commands <reports_iode_commands>`
- :ref:`Macros <reports_macros>`
- :ref:`LEC Expressions <reports_lec_expressions>`
- :ref:`Report Lines Interpretation <reports_lines_interpretation>`


.. _reports_definition:

Reports Definition
------------------

A report is a text file containing a set of instructions, macros, and free text, whose 
interpretation and execution result, on the one hand, in the chaining of operations (loading a 
file, estimating, simulating, etc.) and, on the other hand, in the construction of a"finished" 
document. This document can be a paper document or an RTF, MIF, CSV, or even HTML file.


.. _reports_structure:

Reports Structure
-----------------

A report is a text file with an extension ``.rep`` that can be edited with any text editor 
or within the graphical interface. 

A report contains the following types of information:

- free text: titles, comments, etc.
- A2M commands (text formatting)
- report execution commands
- macros
- LEC expressions
- report functions
- IODE commands

Full Screen Commands
~~~~~~~~~~~~~~~~~~~~

The *IODE commands* can open box dialogs if the report is executed in the graphical interface.

Full screen commands consist of a keyword preceded by the character ``#`` and have the 
following syntax::

    #command

For example: the command::

    #WsLoadVar or #WsLoad

will popup an input box dialog allowing you to specify the name of the variable file to load.

Inline Commands
~~~~~~~~~~~~~~~

Inline commands consist of a keyword preceded by the character ``$`` and have the 
following syntax::

    $command [options] [parameters]

For example, the command::

    $WsLoadVar fun.var

will load the content of the file named ``fun.var`` into the *variables* workspace.

The keyword of inline commands often includes two parts (this is the case for all inline 
commands that manipulate objects): the actual command (generally identical to the full screen 
command) and a suffix qualifying the manipulated object. For example, the full screen command 
`#WsLoad` corresponds to the inline commands `$WsLoadVar`, `$WsLoadIdt`, etc., respectively 
for the variable and identity objects. The valid suffixes are as follows (not all are valid 
for all functions)::

    cmt,    eqs,    idt,    lst,    scl,    tbl,    var,    
    ac,     ae,     ai,     al,     as,     at,     av,    
    rep,    a2m,    prf,    dif,    mif,    rtf,    ps,     
    asc,    txt,    htm,    csv

Syntax Rules
~~~~~~~~~~~~

The ``$`` or ``#`` character indicating a command MUST start the line and be DIRECTLY followed 
by the command keyword (if it is a command to execute). A ``$`` or ``#`` character located further 
in the line will be printed as is. Therefore, there can only be one command per report line.

The keyword can be written in lowercase, uppercase, or both. 
For example::

    WsLoadVar == wsloadvar == WSLOADVAR

Commands whose result (error or name) should be ignored are indicated with a ``-`` sign 
between the ``$`` (or ``#``) and the keyword. For example, if the command::

    $-WsLoadTbl

fails, its result will be ignored (no action will be executed regardless of the `$OnError` command).

Commands whose ``$`` or ``#`` sign is repeated twice (``$$`` or ``##``) will be printed in the 
report without being executed.

Comments
~~~~~~~~

Lines where the ``$`` or ``#`` sign is followed by a space are comment lines, not printed and 
not executed.

Nesting
~~~~~~~

Reports can be nested (unlimited number of nestings), that is, a report can execute another report.

Print Result
~~~~~~~~~~~~

The report is saved in the selected *a2m* file (appended to the existing file if it exists). 
The `$PrintDest` command allows you to specify the name of this file.

Example
~~~~~~~

The following report loads a table definition file and a series file, displays a message, 
builds the tables, prints them, and ends execution with a beep sound::

    $wsloadvar bist92\\bistel    
    $wsloadtbl bist92\\tbistelf

    $printdest bist92\\bistelf1.a2m    
    #show processing french tables  file 1/2    
    $printnbdec 1    
    $PrintTbl 89:8 HYPEIR    
    $PrintTbl 89/88:8 HYPEIIR

    $printnbdec 0    
    $PrintTbl 89:8 RESL00

    $printnbdec 1    
    $PrintTbl 89/88:8 RESL00R    
    $PrintTbl 89:8 RESL03    
    #beep

The produced report also contains any error messages.


.. _reports_execution_commands:

Execution Commands
------------------

These commands can be preceded by either the ``$`` or ``#`` character and control the execution 
flow of the report.

The available execution commands (keywords) are:

- ``$abort`` : interrupts the report (current and upper levels)
- ``$ask`` : prompts a question
- ``$beep`` : produces a beep sound
- ``$chdir`` : changes the current directory
- ``$debug`` : indicates the file and line currently being executed
- ``$define`` : defines a macro
- ``$foreach`` : loops over a list of values
- ``$goto`` : (un)conditional branch to a label
- ``$incrtime`` : increments the variable `t` (default is 1) for the calculation of LEC formulas defined in the report
- ``$indent`` : enables or disables indentation of accepted instructions
- ``$label`` : marks a branch point
- ``$maximize`` : maximizes the IODE window
- ``$minimize`` : minimizes the IODE window
- ``$mkdir`` : creates a new directory
- ``$multiline``: enables or disables writing a command over multiple lines
- ``$msg`` : displays a message and waits for a key press
- ``$next`` : ends a loop
- ``$noparsing``: enables or disables parsing of text between backquotes
- ``$procdef`` : defines a procedure
- ``$procend`` : ends the definition of a procedure
- ``$procexec`` : executes a procedure
- ``$onerror`` : action to take in case of error
- ``$prompt`` : defines a macro by prompting the user
- ``$quit`` : exits IODE
- ``$quitode`` : exits IODE
- ``$repeat`` : loops over the arguments
- ``$repeatstring`` : defines the characters to be replaced in `$repeat`
- ``$return`` : exits the current report
- ``$rmdir`` : deletes an empty directory
- ``$settime`` : sets the variable `t` for the calculation of LEC formulas defined in the report
- ``$shift`` : shifts the report arguments one position to the left
- ``$show`` : displays a message at the bottom of the screen
- ``$sleep`` : pauses the process for a short period
- ``$system`` : executes a system command
- ``$vseps`` : redefines the separators used by the functions `@vdrop()`, `@vtake()`, and `@vcount()`.


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

- ``@upper(text)``: converts text to uppercase
- ``@lower(text)``: converts text to lowercase
- ``@replace(string, from, to)``: substitutes one text for another
- ``@fmt(val, fmt)``: formats an integer
- ``@take(n, string)``: extracts the first n characters of string. If n is negative, extracts the last n characters.
- ``@drop(n, string)``: removes the first n characters of string. If n is negative, removes the last n characters.
- ``@count(string)``: returns the number of elements in string (separated by commas)
- ``@index(n, list)``: returns the nth element of list
- ``@sqz(string)``: removes spaces from string
- ``@strip(string)``: removes trailing spaces from string
- ``@ansi(text)``: converts an Ascii text to Ansi
- ``@equal(t1, t2)``: compares t1 and t2: returns 1 if equal, 0 otherwise
- ``@void(t1, ...)``: returns no text, whatever the arguments

List of strings management
~~~~~~~~~~~~~~~~~~~~~~~~~~

- ``@vtake(n, list)``: take the first n elements of the list (or last n elements if n is negative)
- ``@vdrop(n, list)``: drop the first n elements of the list (or last n elements if n is negative)
- ``@vcount(list)``: return the number of elements in the list

File management
~~~~~~~~~~~~~~~

- ``@fdelete(filename)``: deletes the file filename
- ``@fappend(filename, string|NL, ...)``: writes the text to a file

Directory management
~~~~~~~~~~~~~~~~~~~~

- ``@getdir()``: returns the current directory
- ``@chdir(dirname)``: changes the current directory to dirname and returns the new current directory
- ``@mkdir(dirname)``: creates a new directory dirname
- ``@rmdir(dirname)``: deletes the directory dirname

Dates and times
~~~~~~~~~~~~~~~

- ``@date([format])``: returns the date
- ``@time([format])``: returns the time
- ``@month(month, language)``: returns the text of the month in the given language
- ``@ChronoReset()``: resets the timer to 0
- ``@ChronoGet()``: returns the elapsed time (in msecs) since the last timer reset

Object lists
~~~~~~~~~~~~

- ``@cexpand(pattern, ...)``: returns the list of comments matching pattern
- ``@eexpand(pattern, ...)``: returns the list of equations matching pattern
- ``@iexpand(pattern, ...)``: returns the list of identities matching pattern
- ``@lexpand(pattern, ...)``: returns the list of lists matching pattern
- ``@sexpand(pattern, ...)``: returns the list of scalars matching pattern
- ``@texpand(pattern, ...)``: returns the list of tables matching pattern
- ``@vexpand(pattern, ...)``: returns the list of variables matching pattern
- ``@vliste(objname, ...)``: returns the list of variables in the eqs objname
- ``@sliste(objname, ...)``: returns the list of scalars in the eqs objname

Object content
~~~~~~~~~~~~~~

- ``@ttitle(tablename,tablename, ...)``: returns the titles of the tables
- ``@srelax(sclname,sclname, ...)``: returns the relax value for scalars
- ``@sstderr(sclname,sclname, ...)``: returns the stderr value for scalars
- ``@cvalue(cmtname,cmtname, ...)``: returns the text of a comment
- ``@vvalue(varname,varname, ...)``: returns the values of variables as text
- ``@sample(B|E|)``: returns the (beginning/ending of) the current sample as text

Equation content
~~~~~~~~~~~~~~~~

- ``@evalue(eqname, eqname, ...)``: returns the LEC text of an equation
- ``@eqsample(eqname)``: returns the estimation sample of the equation eqname
- ``@eqsamplefrom(eqname)``: returns the FROM part of the estimation sample
- ``@eqsampleto(eqname)``: returns the TO part of the estimation sample
- ``@eqlhs(eqname)``: returns the left-hand side of an equation
- ``@eqrhs(eqname)``: returns the right-hand side of an equation

Simulations
~~~~~~~~~~~

The following functions allow you to obtain the value of certain simulation parameters:

- ``@SimEps()``: returns the value of the convergence criterion used for the last simulation
- ``@SimRelax()``: returns the value of the relaxation parameter used for the last simulation
- ``@SimMaxit()``: returns the value of the maximum number of iterations used for the last simulation

The simulation results by period can be retrieved using the following functions:

``@SimNiter(period)``: number of iterations required to solve the model for year period
``@SimNorm(period)``: convergence threshold reached when solving the model for year period

Python equivalent functions
~~~~~~~~~~~~~~~~~~~~~~~~~~~

To get the Python equivalent of the above functions, refers to the 
:ref:`Equivalence Between IODE Report And Python Functions <report_funcs_equivalence>` table.


.. _reports_iode_commands:

IODE Commands 
-------------

*IODE commands* allow the user to execute all *IODE* functionalities that are present in the 
graphical interface.

*IODE commands* can be either full screen commands or command lines (without a screen interface). 
Full screen commands consist of a keyword preceded by the character ``#``. 
They are only available when the report is executed in the graphical interface. 
They have the following syntax::

    #command

For example, the command `#WsLoadVar` will popup an input box dialog allowing you to specify 
the name of the variable file to load.

Inline commands consist of a keyword preceded by the character ``$``. 
They have the following syntax::

    $command options parameters

For example, the command::

    $WsLoadVar fun.var

will load the content of the file named ``fun.var`` into the *variables* workspace.

The keywords are mnemonics of the functions executed. 
For example, the keyword `WsCopyScl` performs a copy (`Copy`) of a workspace (`Ws`) 
of scalars (`Scl`). `DataDuplicateIdt`` duplicates (`Duplicate`) data (`Data`) from the 
*identities* workspace (`Idt`).

File operations
~~~~~~~~~~~~~~~

The commands operating on files allow you to copy, edit, delete, rename, print, etc., any file 
of a type recognized by IODE. The commands are as follows:

- ``filelist`` 
- ``fileprint`` 
- ``fileedit`` 
- ``filedelete`` 
- ``filerename`` 
- ``filecopy`` 
- ``SysCopyFile`` 
- ``SysMoveFile`` 
- ``SysDeleteFile`` 
- ``SysOemToAnsi`` 
- ``SysOemToUTF8`` 
- ``SysAnsiToOem`` 
- ``SysAnsiToUTF8`` 
- ``SysAppendFile`` 
- ``FileImportVar`` 
- ``FileImportCmt`` 

Workspace operations
~~~~~~~~~~~~~~~~~~~~

These functions allow the user to join, copy, load, and save workspaces:

- ``WsLoad`` : loads a workspace cmt / eqs / idt / lst / scl / tbl / var from a file
- ``WsCopy`` : copies a workspace cmt / eqs / idt / lst / scl / tbl / var 
- ``WsMerge`` : merges a workspace cmt / eqs / idt / lst / scl / tbl / var into another
- ``WsClear`` : deletes a workspace cmt / eqs / idt / lst / scl / tbl / var 
- ``WsClearAll`` : clears all workspace 
- ``WsDescr`` : describes a workspace cmt / eqs / idt / lst / scl / tbl / var 
- ``WsSave`` : saves a workspace cmt / eqs / idt / lst / scl / tbl / var to a file
- ``WsSaveCmp`` : saves a workspace cmt / eqs / idt / lst / scl / tbl / var to a file in compressed form 
- ``WsSample`` : changes the sample of the *variables* workspace 
- ``WsExtrapolate`` : extends series by extrapolation 
- ``WsLtohStock`` : builds higher frequency series for stocks 
- ``WsLtohFlow`` : builds higher frequency series for flows 
- ``WsHtolSum`` : builds lower frequency series (sum) 
- ``WsHtolMean`` : builds lower frequency series (mean) 
- ``WsHtolLast`` : builds lower frequency series (last observation) 
- ``WsSeasonAdj`` : builds seasonally adjusted series 
- ``WsSeasAdj`` : selects the seasonal adjustment criterion 
- ``WsTrend`` : computes trend series (Hodrick-Prescott) using log transformation 
- ``WsTrendStd`` : computes trend series (Hodrick-Prescott) using the standard method 
- ``WsImport`` : imports an ASCII file 
- ``WsExport`` : exports to a file in ASCII format 
- ``WsImportEviews`` : imports E-Views equations and scalars 
- ``CsvSave`` : saves a workspace in csv format 
- ``CsvDigits`` : sets the number of decimals in CSV files 
- ``CsvSep`` : sets the cell separator in CSV files 
- ``CsvDec`` : sets the decimal separator in CSV files 
- ``CsvNaN`` : sets the text indicating an undefined value in CSV files 
- ``CsvAxes`` : sets the name of the variable axis in CSV files 

Data operations
~~~~~~~~~~~~~~~

The commands operating on data in the active workspaces (in memory) allow you to copy, edit, 
delete, rename, print, etc., the data of a workspace:

- ``datacreate`` : creates an object (cmt / eqs / idt / lst / scl / tbl / var)
- ``datadelete`` : deletes an object (cmt / eqs / idt / lst / scl / tbl / var)
- ``dataexist`` : tests for the existence of an object (cmt / eqs / idt / lst / scl / tbl / var)
- ``dataedit`` : edits an object (cmt / eqs / idt / lst / scl / tbl / var)
- ``dataupdate`` : modifies an object (cmt / eqs / idt / lst / scl / tbl / var)
- ``dataappend`` : appends an object (cmt | lst)
- ``dataduplicate`` : duplicates an object (cmt / idt / lst / scl / tbl / var) (an equation cannot be duplicated)
- ``datarename`` : renames an object (cmt / eqs / idt / lst / scl / tbl / var)
- ``datasearch`` : searches for an object cmt / eqs / idt / lst / scl / tbl / var
- ``datascan`` : scans the objects eqs / idt / tbl
- ``datalistXxx`` : creates a list of objects whose names match a given criterion cmt / eqs / idt / lst / scl / tbl / var
- ``datalistsort`` : sorts a list in alphabetical order
- ``datacompareEps`` : sets the equality threshold for variable comparison
- ``datacompareXxx`` : compares the WS and a file and creates lists
- ``datacalclst`` : performs logical operations on lists
- ``datacalcvar`` : calculates a variable based on a LEC expression
- ``datadisplaygraph`` : displays a graph based on series (without a table)
- ``datasavegraph`` : saves a graph calculated from series
- ``datawidthvar`` : sets the column width for editing series
- ``datandecvar`` : sets the number of decimals for editing series
- ``datamodevar`` : sets the mode for editing series
- ``datastartvar`` : sets the first period for editing series
- ``datawidthtbl`` : sets the column width for editing tables
- ``datawidthscl`` : sets the column width for editing scalars
- ``datandecscl`` : sets the number of decimals for editing scalars
- ``dataeditcnf`` : changes the editing options for variables
- ``datarasvar`` : RAS method for completing a matrix of series
- ``datapatternXXX`` : creates lists of names from a pattern

Specific operations on equations 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The following commands allow you to modify estimation parameters and estimate equations:

- ``EqsEstimate`` 
- ``EqsStepWise`` 
- ``EqsSetCmt`` 
- ``EqsSetSample`` 
- ``EqsSetMethod`` 
- ``EqsSetInstrs`` 
- ``EqsSetBloc`` 

Printer configuration 
~~~~~~~~~~~~~~~~~~~~~

The following commands are used to configure the printer or the output file for print jobs:

- ``printdest`` : sets the print destination
- ``printdestnew`` : sets the print destination and resets the print file
- ``printnbdec`` : sets the number of decimals for printing
- ``printlang`` : sets the default language for printing
- ``PrintA2mAppend`` : prevents the a2m file from being cleared before printing
- ``PrintFont`` : sets the font for printing
- ``PrintTableFont`` : sets the font for tables
- ``PrintTableBox`` : sets the border width for tables
- ``PrintTableColor`` : enables or disables color in tables
- ``PrintTableWidth`` : sets the width of tables in Frame and Rtf
- ``PrintTableBreak`` : enables or disables table breaks across multiple pages
- ``PrintTablePage`` : forces a page break before each table
- ``PrintBackground`` : sets the background color (tables and charts)
- ``PrintGraphBox`` : sets the border width for charts
- ``PrintGraphBrush`` : sets the background density for charts
- ``PrintGraphSize`` : sets the size of charts
- ``PrintGraphPage`` : forces a page break before each chart
- ``PrintRtfHelp`` : generates an RTF file for Windows help
- ``PrintRtfTopic`` : creates a new topic (Windows Help)
- ``PrintRtfLevel`` : changes the hierarchical level of the following topics
- ``PrintRtfTitle`` : sets the title for Windows help
- ``PrintRtfCopyright`` : sets the copyright text for Windows help
- ``PrintHtmlHelp`` : generates an HTML file for HtmlHelp
- ``PrintHtmlStrip`` : when generating an HTML file (A2mToHtml), does not generate a header or footer
- ``PrintParanum`` : enables numbering of headings
- ``PrintPageHeader`` : sets the page header for printed pages
- ``PrintPageFooter`` : sets the page footer for printed pages
- ``SetPrinter`` : sets the default printer
- ``PrintOrientation`` : sets the paper orientation in the printer
- ``PrintDuplex`` : sets the duplex (double-sided) mode of the printer
- ``PrintGIFBackColor`` : sets the background color for charts
- ``PrintGIFTransColor`` : sets the color considered as "transparent"
- ``PrintGIFTransparent`` : indicates if the GIF file should be transparent
- ``PrintGIFInterlaced`` : indicates if the GIF file should be interlaced
- ``PrintGIFFilled`` : indicates whether to fill bars in bar charts
- ``PrintGIFFont`` : specifies the font number to use

Object printing
~~~~~~~~~~~~~~~

The following commands are used to print object definitions:

- ``printobjdef`` : prints a definition of an object cmt / eqs / idt / lst / scl / tbl / var
- ``printobjtitle`` : specifies whether only the titles should be printed
- ``printobjlec`` : specifies the type of equation to print
- ``printobjinfos`` : specifies the information to print

Table compilation and printing 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The following commands are used to compile tables and print or view them:

- ``PrintTblFile`` 
- ``PrintTbl`` 
- ``ViewTblFile`` 
- ``ViewTbl`` 
- ``ViewByTbl`` 
- ``PrintVar`` 
- ``ViewVar``
- ``ViewNdec`` 
- ``ViewWidth`` (obsolete) 
- ``ViewWidth0`` (obsolete)  

Graphs from tables
~~~~~~~~~~~~~~~~~~

The following commands are used to compile tables and display them as graphs or save them 
in an A2M file:

- ``ViewGr`` 
- ``PrintGrAll`` (obsolete) 
- ``PrintGrWin`` (obsolete) 
- ``PrintGrData`` (obsolete) 
- ``PrintGr`` 
- ``DataPrintGraph`` 

Model operations
~~~~~~~~~~~~~~~~

The following commands allow you to sort and simulate a model, estimate, and 
recompile equations:

- ``ModelCalcSCC`` 
- ``ModelSimulateParms`` 
- ``ModelSimulate`` 
- ``ModelSimulateSCC`` 
- ``ModelExchange`` 
- ``ModelCompile`` 
- ``ModelSimulateSaveNiters`` 
- ``ModelSimulateSaveNorms`` 

Identities executions 
~~~~~~~~~~~~~~~~~~~~~

The following commands allow you to execute *identities*:

- ``idtexecute`` 
- ``idtexecutetrace`` 
- ``idtexecutevarfiles`` 
- ``idtexecutesclfiles`` 

Report operations 
~~~~~~~~~~~~~~~~~~

The following commands allow you to manipulate reports (these functions are recursive):

- ``reportexec`` 
- ``reportedit`` 

A2M file translation 
~~~~~~~~~~~~~~~~~~~~

The following functions allow you to translate A2M files into different formats:

- ``A2mToHtml`` : translation to HTML
- ``A2mToRtf`` : translation to Word (Rtf)
- ``A2mToMif`` : translation to Frame (MIF)
- ``A2mToCsv`` : translation to CSV format
- ``A2mToPrinter`` : interprets and prints an A2M file

Other report functions 
~~~~~~~~~~~~~~~~~~~~~~

- ``StatUnitRoot`` : Dickey-Fuller tests
- ``WsAggrChar`` : sets the character to introduce in the code of series created by `WsAggr***`
- ``WsAggrSum`` : computes the sum of series
- ``WsAggrMean`` : computes the mean of series
- ``WsAggrProd`` : computes the product of series

Python equivalent functions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To get the Python equivalent of the above IODE commands, refers to the 
:ref:`Equivalence Between IODE Report Commands and Python Methods <report_commands_equivalence>` 
tables.


.. _reports_macros:

Macros
------

Two types of macros can be used in IODE reports.

The first are local macros, which are only known to the currently running report. 
These are the report arguments (``%n%``). 
You can use these values as long as you know the order in which they are passed. 
A sub-report, of course, receives new definitions for the ``%n%`` variables. 
When returning to the higher-level report, the values of ``%n%`` are restored to 
their original state.

If the ``%`` character needs to appear in the text, it must be doubled. 
Thus::

    %%1%%

is not considered a macro but is replaced in the output by the text `%1%`.

Syntax::

    %n%    
    n := position of the argument    
    (%0% := number of arguments passed to the report)    
    (%*% := all arguments passed to the report)

For example, let's assume we have a report *invert.rep* with the following content::

    $show %2% %1%    
    $show %*%    
    $shift    
    $show %*%    
    $Return

Then, the call::

    $ReportExec invert one two

will display the following output::

    two one    
    one two    
    two

The second type of macros are global: they are known as soon as they are defined and can be 
used in all sub-reports via their name. Their content can be modified during the execution 
of the report (or sub-reports) by the `$Define` instruction with the same name. 
The last definition encountered overrides the previous ones.

Syntax::

    %name%    
    name := name of the macro (see $Define)

You can also use them to globalize a local variable and thus use a report parameter in a 
sub-report::

    $Define FILE %1%    
    $Define VAR %2%    
    $WsCopyVar %FILE% %VAR%


.. _reports_lec_expressions:

LEC Expressions
---------------

It is possible to interleave text and LEC formula commands: this allows, for example, to 
base `$goto` conditions on the values of variables or scalars. If variables are used, the 
calculation period corresponds to the current value of `t`, set by the `$SetTime` and 
`$IncrTime` commands.

To indicate a LEC formula to be calculated in a report, simply enclose it in 
curly braces ``{}``. You can format its result as follows::

    {LEC}       the result is formatted automatically
    {LEC@T}     the result is formatted as a period (1990Y1)
    {LEC@99.9}  the result is formatted in 3 digits with one decimal
    {LEC@.99}   the result is formatted with two decimals

For example, the two following report lines::

    $SetTime 1990Y1
    The GNP in {t@T} is {PNB@9999.9} billion FB.

will display the output::

    The GNP in 1990Y1 is 3089.0 billion FB.

If the characters ``{`` and ``}`` must appear in the text, they must be doubled. 
Thus::

    {{example of lec formula}}

is not considered a LEC formula to be calculated but is replaced in the output by the text::

    {example of lec formula}


.. _reports_lines_interpretation:

Report Lines Interpretation
---------------------------

A report line is interpreted before being (possibly) executed. 
Each line is interpreted from left to right. 
As soon as one of the special characters (``%``, ``{``, ``}`` or ``@``) is encountered, 
a specific processing is applied.

If ``%`` is encountered:

- if the next character is ``%``, the double ``%`` is replaced by ``%`` in the output text.  
Example: *increase of 10%% of GNP* -> *increase of 10% of GNP*
- otherwise, the macro is replaced by its value or empty if the macro does not exist. 
Example: *the variable %VAR%* -> *the variable XYZ*

If ``{`` is encountered:

- if the next character is ``=``, the content between braces is considered as an Excel reference and replaced  
- if the next character is ``{``, the double ``{`` is replaced by ``{`` in the output text.  
Example: *{{example of lec formula}}* -> *{example of lec formula}* 
- the text is read up to ``}``, macros are replaced  
- if the resulting text starts with ``$`` or ``#``, it is a report command that is executed 
and the result (0 or 1) is inserted in the text  
- otherwise, the resulting text is calculated as a LEC formula at the current period defined 
by `$SetTime`. If the formula ends with `@T` or `@999.999`, the result is formatted accordingly.

If ``@`` is encountered:

- if the next character is ``@``, the double ``@`` is replaced by ``@`` in the output text.  
Example: *@@example of lec formula* -> *@example of lec formula*
- the text is read up to the closing parenthesis  
- the corresponding function is executed. Note that if there are no parentheses, the text 
remains unchanged (e.g.: ``gb@plan.be`` remains as is).

For example, the following report lines::

    $define VAL 123.123        
    $msg {%VAL%@999.9} 
    gb@plan.be

will display the output::

    123.1 gb@plan.be
