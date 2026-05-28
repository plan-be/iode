.. _tutorial_iode_reports:

.. currentmodule:: iode

Purpose And Context
~~~~~~~~~~~~~~~~~~~

- Automation and Scripting: IODE reports use scripting to chain commands and automate tasks, 
  enabling repeatable logic and outputs.
- Report File Extension: IODE report files have the extension `.rep` and are simple text files.
- Report File Structure: Report files contain commands (and comments) edited with 
  the graphical user interface (GUI) or any simple text editor.
- Modularity: A report can be split into sub-reports. *Procedures* are user defined functions 
  that can be reused to avoid code duplication and then improve the maintenance.
- Content: IODE reports contain commands, functions, macros, LEC expressions and comments. 

Running Reports
~~~~~~~~~~~~~~~

- Reports can be run from the graphical user interface (GUI), the command line 
  (Windows terminal), or programmatically from Python.
- Arguments can be passed to the executed report. 

Graphical User Interface (GUI)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Old Interface: to execute a report, you need to go to the menu *Report* 
and click on *Execute report*. 

.. image:: _static/old_gui_execute_report.png
   :align: center

New Interface: to execute a report, you need to double click on the report 
file in the file explorer (left panel) and then click on the *Run* button.

.. image:: _static/new_gui_execute_report.png
   :align: center

Command Line (Windows Terminal)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Open a Windows terminal and run the command::

    iodecmd [-nbrun n] [-v] [-y] [-h] reportfile.rep [arg1] ... [argn]

where:

- `nbrun n`: n = number of repetitions of the report execution (default 1)
- `v`: verbose mode (all messages displayed)
- `y`: answer yes to all questions asked during the report execution
- `h`: display the `iodecmd` options
- `reportfile.rep arg1 ...`: *IODE report* to be executed including its optional arguments

See the section :ref:`Command Line Interface (iodecmd) <cli>` for more details.

Python
^^^^^^

In Python script, IODE reports are called using the :func:`execute_report` function. 
For example:

.. code-block:: python

    import iode as io
    first_year = "2010Y1"
    last_year = "2070Y1"
    convergence = "0.01"
    relax = "1.0"
    io.execute_report("simulation.rep", [first_year, last_year, convergence, relax])

Modular Report Invocation
~~~~~~~~~~~~~~~~~~~~~~~~~

Reports can invoke other reports using the `$ReportExec reportfile.rep [arg1] ... [argn]` 
command for modular and reusable pipelines (see :ref:`$reportexec <reportexec>` 
for more details).

Using the *iode* Python module, reports can also be executed from Python scripts using 
the :func:`execute_report` function (see above).

IODE Commands
~~~~~~~~~~~~~

- IODE commands allow to execute *functionalities* that are present in the menus of the 
  graphical user interface (GUI). 
- They start with the special character `#` or `$` and follow the syntax 
  `$CommandName arg1 arg2 ...` (e.g.: `$WsLoadVar fun.var` is a command line that will 
  load the content of the file `fun.var` in the IODE Variables workspace).
- Those starting with `#` are *interactive* meaning they will open the box dialog associated
  with an item from a menu of the GUI and then will wait for user input before to continue 
  executing the report (NOTE: not all *$command* have an equivalent *#command*).
- Commands whose result (error or name) should be ignored are indicated with a `-` sign 
  between the `$` (or `#`) and their name. For example, if the command `$-WsLoadTbl` fails, 
  its result will be ignored (no action will be executed regardless of the `$OnError` command).
- The names of IODE commands are not case sensitive (e.g.: `$WsLoadVar fun.var` and 
  `$wsloadvar fun.var` are equivalent).

.. note::

  By default, the IODE commands (`$command` or `#command`) must be flush with the left margin. 
  If not, they are considered as simple text (non executable line). To allow indentation 
  of IODE commands in reports, you must enable it with the :ref:`$indent <indent>` command.

It is possible to execute an IODE command in the graphical user interface (GUI) without 
writing a report. 

- Old Interface: to execute an IODE command, you need to go to the menu *Report* 
  and click on *Execute a report line*. 

.. image:: _static/old_gui_execute_command.png
   :align: center

- New Interface: to execute an IODE command, you need to open the *IODE commands* tab 
  in the bottom panel, to type the command in the *Report Command* edit line and then 
  click on the *Run* button or press Enter.

.. image:: _static/new_gui_execute_command.png
   :align: center

In Python, an IODE command can be executed using the :func:`execute_command` function. 
For example:

.. code-block:: python

    import iode as io
    io.execute_command("$WsLoadVar fun.var")

For more details on IODE commands, see the sections 
:ref:`Execution Commands <reports_execution_commands>` and 
:ref:`IODE Commands <reports_iode_commands>`.

IODE functions
~~~~~~~~~~~~~~

- IODE functions allow to perform basic operations such as string or list of strings 
  processing, text replacement, object counting, etc.
- They start with the special character `@` and follow the syntax 
  `@function_name(arg1, arg2, ...)` (e.g.: `@take(2, ABCDE)` will return `AB`).
- The names of IODE functions are not case sensitive (e.g.: `@take(2, ABCDE)` and 
  `@TAKE(2, ABCDE)` are equivalent).

For more details on IODE functions, see the section :ref:`Functions <reports_functions>`.

Macros
~~~~~~

- Macros represent user defined variables that can be used to store values and reuse them 
  later in the report (and sub-reports). They are defined with the :ref:`$define <define>` 
  command (`$define macro_name value`) and their values are accessed using the syntax 
  `%macro_name%`.
- The names of **macros are case sensitive** (e.g.: `$define my_macro 10` and 
  `$define MY_MACRO 10` are two different macros).

Example::

    $define FROM 1990Y1
    $define TO   2000Y1
    $define END  2010Y1

    $EqsEstimate %FROM% %TO% ACAF
    $ModelSimulate %TO% %END%

For more details on macros, see the section :ref:`Macros <reports_macros>`.

LEC Expressions
~~~~~~~~~~~~~~~

- It is possible to evaluate and use the results of LEC expressions inside a report.
- Evaluable expressions are enclosed between `{` and `}`.
- Since IODE works with time vectors, the period at which the LEC expression has to 
  be calculated must be specify:

  - :ref:`$settime period <settime>`: sets the *current evaluation period* :math:`t` to the period *period*,
  - :ref:`$incrtime step <incrtime>`: increments the *current evaluation period* :math:`t` by *step* periods,

- The *current evaluation period* :math:`t` is defined as an integer that represents the 
  number of periods since the first period of the IODE Variables workspace. 
- Additionally, the format of the LEC expression result can be specified using the 
  syntax `{LEC@format}` where *format* is one of the following:

  - `{LEC@99.9}`: calculates the expression, with 4 digits including 1 decimal place
  - `{LEC@.9}`: calculates the expression, with 1 decimal place,
  - `{LEC@T}`: calculates the expression, and converts it to a year (1990Y1, ...)

- Rules for evaluating the expressions: 

  1. If a report line contains a `{`, then the text is read up to encountering a `}`,
  2. The macros present inside the expression are replaced by their values,
  3. If the resulting expression begins with a `$` or `#`, the rest of the expression 
     is executed as a report command. The result (0 or 1) replaces the text in the report. 
  4. Otherwise, the expression is interpreted as a LEC formula and executed for 
     the *current evaluation period* :math:`t` (determined by :ref:`$settime <settime>`). 
     The (formatted) result replaces the text in the report. 

Example::

    $SetTime 1990Y1
    $show The value of the current evaluation period t is {t}
    $IncrTime 10
    $show The incremented value of t is now {t@T}
    $show Value of the coefficient acaf1 with two decimals: {acaf1@.99}
    $show Successfully loaded Variables ? {$WsLoadVar fun.av}

Fore more details on LEC expressions, see the section 
:ref:`LEC Expressions <reports_lec_expressions>`.

Comments
~~~~~~~~

- Lines starting with `#` or `$` but **followed by space** are comments and do not 
  execute and are print as it (e.g.: `$WsLoadVar fun.var` is a command line but 
  `$ WsLoadVar fun.var` is a comment line).
- Expressions between backquotes `\`` are also comments and are not executed 
  (e.g.: the report line `\`@take(2, ABCDE) = \` @take(2,ABCDE)` will print 
  `@take(2, ABCDE) = AB` in the report output).

Report Arguments
~~~~~~~~~~~~~~~~

- Arguments passed to the reports are special macros and are accessed using the syntax 
  `%1%` (first argument), `%2%` (second argument), etc. 
- The special variable `%0%` contains the number of passed arguments
- The special variable `%*%` contains the list of all passed arguments. 
- The command :ref:`$shift p <shift>` shifts the arguments by *p* positions. 
  The former `%n%` argument becomes `%n-p%` argument. Also `%0%` and `%*%` are updated.
- Calling :ref:`$shift <shift>` without specifying *p* will shift arguments by 1 position. 
- After shifting, the *shifted* arguments disappear.

Error Handling
~~~~~~~~~~~~~~

- :ref:`$onerror action <onerror>` specifies what should happen if something 
  goes wrong in a report:

  - *ignore*: (default) the report continues,
  - *return*: the current (sub-)report stops,
  - *abort*: all running reports stop,
  - *quit*: exits the graphical user interface (GUI) and all running reports stop,
  - *display*: (default) displays the error message on the screen,
  - *print*: prints the error message,
  - *noprint*: suppresses the error message

- :ref:`$debug {Yes|No} <debug>`: displays the progress of the output
- :ref:`$abort <abort>`:  stops all running reports
- :ref:`$return <return>`: exits the current (sub-)report and continues with parent report
- :ref:`$quit <quit>`: exits the graphical user interface (GUI) and all running reports stop

Feedback
~~~~~~~~

- :ref:`$sleep nn <sleep>`: IODE sleeps for *nn* microseconds.
- :ref:`$show msg <show>`: IODE displays the message *msg* at the bottom of the screen. 
  The report continues as usual. 
- :ref:`$msg msg <msg>`: IODE pauses the report and displays the message *msg* in a box 
  dialog. The report stops until the user closes the box dialog.

Looping
~~~~~~~

- The :ref:`$foreach <foreach>` command allows you to specify an index and the list 
  of values that this index should successively take.
- The :ref:`$next <next>` command returns to the start of the :ref:`$foreach <foreach>` 
  loop and moves to the next value of the index.

Syntax::

    $indent
    $foreach {index} {values}  
      ...  
      $next {index}

where:

- `{index}` is a macro name of up to 10 characters (for example `i`, `idx`, `COUNTRY`, ...)
- `{values}` is a list of values separated by commas, spaces, or semicolons. 
  The separators can be changed with the :ref:`$vseps <vseps>` command.

Example 1: nested loops::

    $indent
    $foreach I BE BXL VL WAL  
      $foreach J H F  
        $show [%I%;%J%]  
      $next J  
    $next I

Example 2: using lists::

    $indent
    $DataUpdateLst MYLIST X,Y,Z  
    $Define n 0  
    $foreach I @lvalue(MYLIST)  
      $Define n {%n% + 1}  
      $show Element %n% : [%I%]  
      $next I

Jumping
~~~~~~~

- :ref:`$label name <label>`: places an anchor and gives it a name.
- :ref:`$goto name {condition} <goto>`: execution of the report jumps to *name*. 
  An optional *condition* can be specified. 

Example::

    $goto exist {$DataExistVar A}
    $show Variable A does not exist
    $return 
    $label exist
    $show Variable A exists
    $return 

.. warning::

   Be careful of infinite loop if the *goto* refers to a *label* defined above in the *goto*!

.. note::

   In Python, there is no direct equivalent (*goto* like statement have been avoided 
   in modern languages due to their potential to create complex and hard-to-maintain code). 
   Use if-else statements and user defined functions to control the flow of your 
   Python code instead.

Procedures
~~~~~~~~~~

Procedures are groups of commands that can be defined and then called with different
parameters. They are useful to avoid code duplication and to improve the maintenance of 
IODE reports.

Procedures are related to the commands :ref:`$procdef <procdef>`, :ref:`$procend <procend>`, 
and :ref:`$procexec <procexec>`.

Example::

    $procdef procname [fparm1 ...]  
      ... 
    $procend

where:

- `procname` is the name of the procedure (case sensitive).
- `fparm1` is the first parameter of the procedure

A procedure is called simply with the command::

    $procexec procname aparm1 ...

where:

- `procname` is the name of the procedure (case sensitive).
- `aparm1` is the first passed parameter of the procedure

Scope of a procedure definition
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- Procedures must be defined before they can be called.
- Once defined, a procedure remains callable within the same IODE session, even if the report 
  that defined it has finished. You can execute a report whose sole purpose is to load 
  procedures into memory. These procedures will remain available throughout the session.
- A procedure can be replaced at any time by another with the same name.

For more details on procedures, see :ref:`$procdef <procdef>`.

Computed Tables
~~~~~~~~~~~~~~~

One of the most frequently performed operations during a simulation exercise is 
the display of tables of results and charts. It may also be useful to compare 
the results of different simulations or to compare them with observed data.
That is the purpose of the IODE tables. 

An IODE table is computed according to a *generalized sample* and 
optional *extra files* (containing the results of previous simulations).
 
A *generalized sample* contains the following information:
    
- the sampling of the periods to take into account 
- the operations to be performed on the periods
- the list of files involved in the computation of the table
- the operations to be performed between files
- the repetition factor
    
The syntax of the *generalized sample* follows the rules described below. 

Syntax for Periods
^^^^^^^^^^^^^^^^^^
    
- a period is indicated as in LEC: `yyPpp` or `yyyyPpp` where *yyyy* indicates the year, 
  *P* the periodicity and *pp* the sub-period (e.g. 1990Y1) 
- a period can be shifted n periods to the left or right using the operators `<n` and `>n` 
- when used with a zero argument, the shift operators have a special meaning: 

    - `<0` means "first period of the year" 
    - `>0` means "last period of the year" 

- the special periods 'BOS', 'EOS' and 'NOW' can be used to represent the beginning 
  or end of the current sample or the current period (PC clock)
- the special periods 'BOS1', 'EOS1' and 'NOW1' are equivalent to the previous ones, 
  except that they are moved to the first sub-period of the year of 'BOS', 'EOS' and 
  'NOW' respectively (if NOW = 2012M5, NOW1 = 2012M1)
- each period is separated from the next by a semicolon 
- a period or group of periods can be repeated: simply place the colon character (`:`) after 
  the definition of the column or group of columns, followed by the desired number of repetitions. 
  Repetitions are made with an increment of one period, unless followed by an asterisk and a value. 
  This value is then the repeat increment. It can be negative, in which case the periods are 
  presented in decreasing order
- the repeat, increment and shift can be the words PER (or P) or SUB (or S), which 
  respectively indicate the number of periods in a year of the current sample and 
  the current sub-period
- the file definition is optional and is enclosed in square brackets. 
  It applies to all preceding period definitions.
    
Operations on Periods
^^^^^^^^^^^^^^^^^^^^^

- value: (75) 
- growth rate over one or more periods: (75/74, 75/70) 
- average growth rate: (75//70) 
- difference: (75-74, 75-70) 
- average difference: (75--70) 
- average: (75~74) or (75^74) 
- sum of consecutive periods: (70Q1+70Q4) 
- index or base value: (76=70) 

Periods Repetition
^^^^^^^^^^^^^^^^^^

Repetition can be performed with an increment greater than 1 or less than 0: 
simply place a star followed by the step after the number of repetitions 
(70:3*5 = 70, 75, 80).

Syntax for Files
^^^^^^^^^^^^^^^^

- absolute value: [1] 
- difference: [1-2] 
- difference in percent: [1/2] 
- sum: [1+2] 
- average: [1~2] or [1^2]. 
    
The file [1] always refers to the current workspace. 
Extra files (if passed as argument) are numerated from 2 to 5. 
        
Examples
^^^^^^^^

.. code-block:: text

    70; 75; 80:6 = 70:3*5; 81:5 = 70; 75; 80; 81; 82; 83; 84; 85
    70/69:2 = 70/69; 71/70
    (70; 70-69):2 = 70; 70-69; 71; 71-70;
    70[1,2]:2*5 = 70[1]; 70[2]; 75[1]; 75[2]
    (70;75)[1,2-1] = 70[1]; 75[1]; 70[2-1]; 75[2-1]
    (70;75;(80; 80/79):2)[1,2] = 70[1]; 70[2]; 75[1]; 75[2]; 80[1]; 80[2]; 80/79[1]; 80/79[2] 81[1]; 8[2]; 8180[1]; 81/80[2]
    2000Y1>5 = 2005Y1
    1999M1>12 = 2000M1
    EOS<1 = 2019Y1                         (if EOS == 2020Y1)
    BOS<1 = 1959Y1                         (if BOS == 1960Y1)
    EOS<4:5*-1 =2016;2017;2018;2019;2020   (if EOS = 2020Y1)

List of IODE Commands for Computed Tables
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- :ref:`PrintTblFile <printtblfile>`: defines files to use when printing comparison tables
- :ref:`PrintTbl <printtbl>`: builds and prints tables in A2M format
- :ref:`ViewTblFile <viewtblfile>`: defines files to use when viewing comparison tables
- :ref:`ViewTbl <viewtbl>`: builds and displays tables in a scrollable table
- :ref:`ViewByTbl <viewbytbl>`: alias for ViewTbl
- :ref:`PrintVar <printvar>`: builds and prints comparison tables of series in A2M format
- :ref:`ViewVar <viewvar>`: views comparison tables of series in A2M format
- :ref:`ViewNdec <viewndec>`: specifies the number of decimals for values displayed in tables
- :ref:`ViewGr <viewgr>`: displays tables as graphs                                          
- :ref:`PrintGr <printgr>`: prints one or more graphs defined from tables   
- :ref:`DataPrintGraph <dataprintgraph>`: prints graphs built directly from variables 
