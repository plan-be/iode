.. _reports_execution_commands:

Execution Commands
------------------

These commands can be preceded by either the ``$`` or ``#`` character and control the execution 
flow of the report.

The available execution commands (keywords) are:

- :ref:`$abort <abort>` : interrupts the report (current and upper levels)
- :ref:`$ask <ask>` : prompts a question
- :ref:`$beep <beep>` : produces a beep sound
- :ref:`$chdir <chdir>` : changes the current directory
- :ref:`$debug <debug>` : indicates the file and line currently being executed
- :ref:`$define <define>` : defines a macro
- :ref:`$foreach <foreach>` : loops over a list of values
- :ref:`$goto <goto>` : (un)conditional branch to a label
- :ref:`$incrtime <incrtime>` : increments the variable `t` (default is 1) for the calculation of LEC formulas defined in the report
- :ref:`$indent <indent>` : enables or disables indentation of accepted instructions
- :ref:`$label <label>` : marks a branch point
- :ref:`$maximize <maximize>` : maximizes the IODE window
- :ref:`$minimize <minimize>` : minimizes the IODE window
- :ref:`$mkdir <mkdir>` : creates a new directory
- :ref:`$multiline <multiline>`: enables or disables writing a command over multiple lines
- :ref:`$msg <msg>` : displays a message and waits for a key press
- :ref:`$next <next>` : ends a loop
- :ref:`$noparsing <noparsing>`: enables or disables parsing of text between backquotes
- :ref:`$procdef <procdef>` : defines a procedure
- :ref:`$procend <procend>` : ends the definition of a procedure
- :ref:`$procexec <procexec>` : executes a procedure
- :ref:`$onerror <onerror>` : action to take in case of error
- :ref:`$prompt <prompt>` : defines a macro by prompting the user
- :ref:`$quit <quit>` : exits IODE
- :ref:`$quitode <quitode>` : exits IODE
- :ref:`$repeat <repeat>` : loops over the arguments
- :ref:`$repeatstring <repeatstring>` : defines the characters to be replaced in `$repeat`
- :ref:`$return <return>` : exits the current report
- :ref:`$rmdir <rmdir>` : deletes an empty directory
- :ref:`$settime <settime>` : sets the variable `t` for the calculation of LEC formulas defined in the report
- :ref:`$shift <shift>` : shifts the report arguments one position to the left
- :ref:`$show <show>` : displays a message at the bottom of the screen
- :ref:`$sleep <sleep>` : pauses the process for a short period
- :ref:`$system <system>` : executes a system command
- :ref:`$vseps <vseps>` : redefines the separators used by the functions `@vdrop()`, `@vtake()`, and `@vcount()`.


.. _define:

COMMAND $DEFINE
~~~~~~~~~~~~~~~~

This command allows you to define macros that can be used later in the report and its 
sub-reports. A macro is a kind of global variable that remains defined throughout the 
execution of the current report.

Syntax::

    $Define name text  
    name := name used to refer to the content (text) of the  
            macro (the name must start with a letter and not  
            exceed 10 characters)  

The name is referenced in a report by enclosing it in ``%`` signs::

    $Define a Hello World!  
    $Show %a%

Example::

    $Define VAR A  
    $GoTo exist {$DataExistVar %VAR%}  
    $Show Variable %VAR% does not exist  
    $Return  
    ....  
    $Label exist  
    $Show Variable %VAR% exists  
    $Return  
    ...

If a report has arguments, these can be used as macros whose name is ``%n%``, where *n* is 
the argument number. The special value ``%0%`` contains the number of arguments passed when 
calling the report. For instance, the *exist.rep* report below::

    $Define VAR %1%  
    $GoTo exist {$DataExistVar %VAR%}  
    $Show Variable %VAR% does not exist  
    $Return  
    ....  
    $Label exist  
    $Show Variable %VAR% exists  
    $Return  
    ...

executed with *A* as an argument, checks if the variable *A* exists.

You can also store calculated values in macros::

    $Define PNB {PNB}  
    $Define GPNB {r PNB * 100}  
    The value of PNB is %PNB% (difference of %GPNB% percent)

Uppercase/lowercase mode
^^^^^^^^^^^^^^^^^^^^^^^^

If the percent sign is immediately followed by the character:

- ``#`` : the content of the macro is converted to uppercase
- ``!`` : the content of the macro is converted to lowercase
- ``-`` : non-alphanumeric characters are removed from the macro

The following commands::

            $define TABLE_NAME TBL1  
            $define ARG0 Abc[-3]  
            First table: %!TABLE_NAME%  
            Second table: %#TABLE_NAME%  
            ARG1: %ARG0O%  
            ARG2: %-ARG0%

will produce at execution::

            First table: tbl1  
            Second table: TBL1  
            ARG1: Abc[-3]  
            ARG2: Abc3

.. _label:

COMMAND $LABEL
~~~~~~~~~~~~~~

This instruction indicates and names a branch point in a report. This branch point can 
later be reached by an instruction such as :ref:`$goto <goto>` or :ref:`$ask <ask>`.

Example::

    $label again  
    $goto fin {%0%=0}  
    #show arg0 : %1%  
    $DataCalcVar %1% if(%1%=0, 1/0, %1%)  
    $shift  
    $goto again  
    $label fin

In this example, all 0 are replaced by `NA` for the list of variables (initially unknown) 
passed as parameters to the report. As a reminder, ``%0%`` is the number of arguments, 
``%1%`` is the first argument, etc.

.. _goto:

COMMAND $GOTO
~~~~~~~~~~~~~

This command performs a jump to a branch point (label) in the current report.

Syntax::

    $GoTo label [{condition}]  
    label := branch point indicated by the Label command  
    condition := LEC expression or report command

Note: If the label does not exist, the execution of the report (and all higher-level reports) 
is interrupted.

Use case::

    ...
    $Label loop  
    ...  
    $GoTo loop  
    ...

It is also possible to construct certain tests to direct the subsequent execution of the report.

If a condition is placed after the label name, it is executed first. The condition can be 
either an IODE command (for example ``$DataExistVar``) or an LEC formula 
(for example, ``t != 1993Y1``). The command or the LEC formula must be placed between 
curly braces ``{}``. The *Goto* occurs if the IODE command can be validly executed or if 
the result of the LEC formula is nonzero. To differentiate an IODE command from an LEC formula, 
the command must always start with the ``$`` character.

Example (command)::

    ...  
    $GoTo exist {$DataExistVar A}  
    $Show Variable A does not exist  
    $Return  
    ....  
    $Label exist  
    $Show Variable A exists  
    $Return  
    ...  
 
or (LEC formula)::

    ...  
    $GoTo end {X + Y > 1000}  
    ...  
    $Return  
    ....  
    $Label end  
    $Show X + Y exceeds the limit of 1000  
    $Return  
    ...

It is also possible to invert the result of an IODE command, just as with an LEC formula. 
To do this, simply place an exclamation mark ``!`` after the ``$``.

Example::

    ...  
    $GoTo notexist {$!DataExistVar A}  
    $Show Variable A does exist  
    $Return  
    ....  
    $Label notexist  
    $Show Variable A does not exist  
    $Return  
    ...

In the same way, LEC formulas can easily be adapted to return the desired value 
(``!`` operator).

.. _onerror:

COMMAND $ONERROR
~~~~~~~~~~~~~~~~

This command specifies the action to take when an error is encountered in an IODE command, 
if the error should not be ignored (indicated by a ``-`` after the ``$`` or ``#``).
The possible actions are: ``RETURN``, ``ABORT``, ``QUIT``, ``DISPLAY`` (default), ``PRINT``, 
``NOPRINT``, and ``IGNORE`` (default):

- The ``IGNORE`` action is the default: ignores the error and continues executing the report.
- The ``RETURN`` action interrupts the current report, returning to the upper level if applicable.
- The ``ABORT`` action interrupts the current report and all higher-level reports.
- The ``QUIT`` action terminates the IODE program. It is especially useful in combination with 
  the ``-rep`` option of the `iodecmd` program.
- The ``DISPLAY`` action is active by default; it prints an error message in the report 
  corresponding to the error encountered during the execution of the last IODE command. 
  It also displays all corresponding error messages on the screen.
- The ``PRINT`` action only prints the error messages.
- The ``NOPRINT`` action suppresses the printing of the message.

Notes:

- The actions ``RETURN``, ``ABORT``, ``QUIT``, and ``IGNORE`` are mutually exclusive. 
  The action executed is the one corresponding to the last ``ONERROR`` command encountered.
- The actions ``DISPLAY``, ``PRINT``, and ``NOPRINT`` are also mutually exclusive but can 
  be combined with one of the four instructions above.
- Only one action can appear per command line.

Syntax::

    $OnError action  
    action := RETURN, ABORT, QUIT, IGNORE, DISPLAY, PRINT or NOPRINT

Example::

    $OnError noprint  
    $OnError ignore  
    ...  
    $OnError noprint  
    $OnError abort  
    ...

.. _return:

COMMAND $RETURN
~~~~~~~~~~~~~~~

This instruction indicates that the execution of the current report is finished.
If it is a sub-report, the calling report continues.

Syntax::

    $Return

Example::

    Report createvar.rep  
    ---------------------  
    # Create series %1%  
    $GoTo exist {$DataExistVar %1%}  
    $DataCalcVar %1% 0.9  
    $Show Variable %1% created  
    $Return  
    $Label exist  
    $Show Variable %1% exists  
    
    Call  
    -----  
    $ExecRep createvar A

.. _abort:

COMMAND $ABORT
~~~~~~~~~~~~~~

This instruction indicates that the execution of the report must be interrupted.
If the current report is a sub-report, all higher-level reports will also be interrupted.

Syntax::

    $Abort

Example::

    ...  
    $GoTo continue {$DataExistVar A}  
    $Show Variable A does not exist  
    $Abort  
    ....  
    $Label continue  
    ...

.. _quitode:

COMMAND $QUITODE
~~~~~~~~~~~~~~~~
See :ref:`$QUIT <quit>`.

.. _quit:

COMMAND $QUIT
~~~~~~~~~~~~~

This instruction indicates that the execution of the report must be interrupted and 
that the graphical interface must be closed.

Syntax::

    $Quit

Example::

    $WsLoadVar refsim  
    $ModelSimulate 1997Y1 1998Y1  
    $WsSaveVar myws  
    $Quit

This function is particularly useful when a report is launched from a command file 
(*DOS* or *Unix*). By allowing IODE to exit, it returns control to the command file, 
which can then proceed with another program (printing, another simulation, etc.).

.. _show:

COMMAND $SHOW
~~~~~~~~~~~~~

This command produces a beep sound and displays a message in a new box dialog of 
the graphical interface during the execution of the report.

Syntax::

    $Show message  
    message := any text
    
.. _msg:

COMMAND $MSG
~~~~~~~~~~~~

This command displays the text of the argument and waits for a key press before continuing.
This allows you to temporarily pause the report to display a message to the user, 
unlike the :ref:`$show <show>` function which displays in a box dialog but does not pause.

Syntax::

    $Msg message  
    message := any text

.. _beep:

COMMAND $BEEP
~~~~~~~~~~~~~

This command produces a beep sound during the execution of the report.

Syntax::

    $Beep

.. _ask:

COMMAND $ASK
~~~~~~~~~~~~

This command displays a question to which the answer can be `Yes` or `No`.

- If the answer is `No`, the report continues on the next line. 
- If the answer is `Yes`, the execution of the report resumes from the indicated label. 
- If the label does not exist, the execution of the report (and all higher-level reports) 
  is interrupted. The question and the label are separated by a comma `,` or a semicolon `;`.

Syntax::

    $Ask label,question  

    where label    := branch point indicated by the Label command  
          question := any text

Example::

    ...
    $Label loop  
    ...  
    $Ask loop,return to loop?  
    ...

.. _prompt:

COMMAND $PROMPT
~~~~~~~~~~~~~~~

This command allows you to define macros that can be used later in the report and its 
sub-reports. A macro is a kind of global variable that remains defined throughout the 
execution of the current report.

The `Prompt` function asks a question to the report user and stores their answer in a 
variable that can be used later in the report.

Syntax::

    #Prompt VarName Question  
    Question := question to be asked  
    VarName := name used to refer to the content (text) of the  
               macro (the name must start with a letter and not  
               exceed 10 characters)  
  
The name is referenced in a report by enclosing between two ``%`` signs.

Example::

    #Prompt Begin Start period?  
    $SetTime %Begin%

.. _settime:

COMMAND $SETTIME
~~~~~~~~~~~~~~~~

This command sets the period ``t`` for which the LEC formulas used in the reports 
must be calculated. It MUST be executed before calculating an LEC formula in a report, 
otherwise the execution period is unknown and the formula will not be executed.

Syntax::

    $SetTime period  
    period := the period value to assign to t

The following report lines::

    $SetTime 1991Y1  
    The value of PNB for the year {t@T} is {PNB} billion.

produce the output::

    The value of PNB for the year 1991Y1 is 4000.32 billion.

.. _incrtime:

COMMANDE $INCRTIME
~~~~~~~~~~~~~~~~~~

This command increases the period ``t`` by as many units as indicated. 
If no argument is given, the increment is set to 1.

Syntax::

    $IncrTime [step]  
    step := number of periods to add

The following report lines::

    $SetTime 1990Y1  
    $Label print  
    The value of PNB for the year {t@T} is {PNB} billion.  
    $IncrTime 5  
    $goto print {t < 2000Y1}  
    $Return

produce the result::

    The value of PNB for the year 1990Y1 is 4132.32 billion.  
    The value of PNB for the year 1995Y1 is 4240.32 billion.

.. _system:

COMMAND $SYSTEM
~~~~~~~~~~~~~~~

This report command is used to execute a system command.
Once the command is executed, the report continues on the next line.
The system command (followed by any parameters) is passed as an argument.
Multiple commands can be chained together; they must be separated by a semicolon ``;``.
Before executing the system command, the terminal is reset to standard mode and the 
screen is cleared. After executing the command, the screen is restored to its initial state.

.. warning::

    WARNING

    - No verification is performed regarding the validity or appropriateness of the system 
      command. A command like ``del *.*`` or ``format c:`` is accepted.
    - Memory space is limited for executing the system command because IODE remains loaded 
      in memory (DOS version).
    - It may be useful to end the list of system commands to execute with the ``PAUSE`` 
      command (in *DOS*) in order to temporarily keep the terminal in standard mode 
      (to view the results of the system command).

Syntax::

    $System command1 [;command2;...]
    command := valid operating system command [+ arguments]

Example::

    $System qode -d qms result.a2m

.. _shift:

COMMAND $SHIFT
~~~~~~~~~~~~~~

This function shifts the report arguments one position to the left. 
As a result, the first argument is lost. This feature allows you to perform operations 
on an unknown number of elements. Since ``%0%`` represents the number of report arguments, 
after :ref:`$shift <shift>`, this value is decremented. In this way, a simple calculated 
:ref:`$goto <goto>` as in the example below allows you to exit the loop as soon as ``%0%`` 
is zero.

Example::

    $label again  
    $goto end {%0%=0}  
    #show arg0 : %1%  
    $DataCalcVar %1% if(%1%=0, 1/0, %1%)  
    $shift  
    $goto again  
    $label end

.. _minimize:

COMMAND $MINIMIZE
~~~~~~~~~~~~~~~~~

.. warning::

    OBSOLETE

This command minimizes the IODE window (for example, to display Excel charts 
generated by the current simulation).

Syntax::

    $Minimize

.. _maximize:

COMMAND $MAXIMIZE
~~~~~~~~~~~~~~~~~

.. warning::

    OBSOLETE

This command restores the IODE window.

Syntax::

    $Maximize

.. _sleep:

COMMAND $SLEEP
~~~~~~~~~~~~~~

Pauses the process for *n* milliseconds. 
This allows a client to retain control in case of requests that are too fast.

Syntax::

    $Sleep n

where ``n`` = number of milliseconds to pause.

.. _debug:

COMMAND $DEBUG
~~~~~~~~~~~~~~

This command displays in the output window the name of the current report and 
the current line. It thus makes it easy to identify a problematic line in case of 
an error, for example.

Syntax::

    $debug {Yes|No}

.. _repeat:

COMMAND $REPEAT
~~~~~~~~~~~~~~~

This function allows you to execute a command on a list of arguments without having to create 
a report for this purpose. The position of the argument in the command to be repeated is 
represented by the string defined by :ref:`$repeatstring <repeatstring>`.
By default, the underscore character ``_`` is the replacement character.

Syntax::

    $repeat command arg1 arg2 arg3

where *command* is executed for each argument by replacing ``_`` with arg1, then arg2, etc.

Examples:

1. Sorting multiple lists::

    $RepeatString ++  
    $Repeat "$DataListSort ++ ++" A B C

is equivalent to the three commands::

    $DataListSort A A  
    $DataListSort B B  
    $DataListSort C C

2. Duplicate Vars: With the @fn() functions, you can copy all the variables 
from a workspace in a single operation::

    $Repeat "$DataDuplicateVar _ _1" @vexpand(*)

.. _repeatstring:

COMMAND $REPEATSTRING
~~~~~~~~~~~~~~~~~~~~~

This function allows you to specify the characters to be replaced in the first argument 
of :ref:`$repeat <repeat>`. By default, this string consists of only the underscore 
character ``_``.

Syntaxe::

    $RepeatString string

Exemple::

    $RepeatString --  
    $Repeat "$DataListSort -- --" A B C

.. _chdir:

COMMAND $CHDIR
~~~~~~~~~~~~~~

This report command changes the current directory.
Files are always loaded relative to the current directory.
At the end of the report, the previous directory is NOT restored.

Syntax::

    $chdir dirname  
    dirname := directory name (relative or absolute)

Example::

    $chdir ..  
    $chdir c:\usr\iode

.. _mkdir:

COMMAND $MKDIR
~~~~~~~~~~~~~~

This report command creates a new directory.

Syntax::

    $mkdir dirname  
    dirname := directory name (relative or absolute)

Example::

    $mkdir ..\test  
    $mkdir c:\usr\iode\test

.. _multiline:

COMMAND $MULTILINE
~~~~~~~~~~~~~~~~~~

By default, a report line can be continued on the next line if it ends with a space followed 
by a backslash ``\``. In this case, the newline character ``\n`` is inserted between the lines.

This allows you, for example, to construct equations in a report that span several lines, 
making them easier to read.

To prevent this extension (for compatibility with previous versions), simply place the 
`$multiline 0` command before the relevant lines. Without an argument, multi-lines are 
accepted.

Syntax::

    $multiline [{Nn0}]

For example, you can now write::

     $multiline 1    
     $DataUpdateEqs A A := c1 + \    
     c2 * t

The definition of the equation *A* will then be on two lines.

Conversely, without multiline, an error is generated. For example::

     $multiline 0    
     $DataUpdateEqs A A := c1 + \    
     c2 * t

tries to create an equation ending with ``+ \``, which produces the following result::

    essais.rep[23] - $multiline 0    
    essais.rep[24] - $DataUpdateEqs A A := c1 + \    
    (string)[7]:syntax error    
    Error : dataupdateeqs A A := c1 + \

.. _rmdir:

COMMAND $RMDIR
~~~~~~~~~~~~~~

This report command deletes a directory.

Syntax::

    $rmdir dirname  
    dirname := directory name (relative or absolute)

Example::

    $rmdir ..\test  
    $rmdir c:\usr\iode\test

.. _vseps:

COMMAND $VSEPS
~~~~~~~~~~~~~~

This command redefines the separators used by the functions ``@vdrop()``, ``@vtake()`` 
and ``@vcount()``, as well as in :ref:`$foreach <foreach>` and :ref:`$next <next>`.

Syntax::

    $vseps {seps}

Example::

    $vseps ;|

See also :ref:`@vtake <vtake>`.

.. _foreach:

COMMAND $FOREACH
~~~~~~~~~~~~~~~~

This command simplifies writing loops or nested loops.
The :ref:`$foreach <foreach>` command allows you to specify an index and the list of values 
that this index should successively take.
The :ref:`$next <next>` command returns to the start of the :ref:`$foreach <foreach>` loop 
and moves to the next value of the index.

Syntax::

    $foreach {index} {values}  
    ...  
    $next {index}

where:

- ``{index}`` is a macro name of up to 10 characters (for example `i`, `idx`, `COUNTRY`, ...)
- ``{values}`` is a list of values separated by commas, spaces, or semicolons. 
  The separators can be changed with the :ref:`$vseps <vseps>` command.

Example 1: nested loops::

    $foreach I BE BXL VL WAL  
    $foreach J H F  
    $show [%I%;%J%]  
    $next J  
    $next I

Example 2: using lists::

    $DataUpdateLst MYLIST X,Y,Z  
    $Define n 0  
    $foreach I @lvalue(MYLIST)  
    $Define n {%n% + 1}  
    $show Element %n% : [%I%]  
    $next I

.. _next:

COMMAND $NEXT
~~~~~~~~~~~~~

See :ref:`$foreach <foreach>`.

.. _procdef:

COMMAND $PROCDEF
~~~~~~~~~~~~~~~~

This command indicates the start of the definition of an IODE procedure.
It is related to the commands :ref:`$procdef <procdef>`, :ref:`$procend <procend>`, and
:ref:`$procexec <procexec>`. The instructions in this group allow you to build procedures, 
that is, lists of commands that can be reused and parameterized.

Example::

    $procdef procname [fparm1 ...]  
    ... 
    $procend

where:

- ``procname`` is the name of the procedure (case sensitive).
- ``fparm1`` is the first formal parameter of the procedure

A procedure is called simply with the command::

    $procexec procname aparm1 ...

where:

- ``procname`` is the name of the procedure (case sensitive).
- ``aparm1`` is the first actual parameter of the procedure

Parameters
^^^^^^^^^^

Formal parameters are treated in the procedure as :ref:`defines <define>` 
(local to the procedure): they must be referenced as ``%fparm%``. 
Their values are set as follows:

- If there are fewer or an equal number of actual parameters than formal parameters, 
  the values of the actual parameters are assigned in order to the first formal parameters. 
  Any extra formal parameters are considered empty.
- If there are more actual parameters than formal parameters, the formal parameters (except 
  the last one) receive the values of the first actual parameters, in the order they are passed. 
  The last formal parameter receives the value of all remaining actual parameters.

Example with more actual parameters than formal parameters::

    $indent  
    $procdef print list  
        $foreach i %list%  
            $show print : %i%  
        $next i  
    $procend  
    $procexec print A B C

Result::

    print : A  
    print : B  
    print : C

You can see that the formal parameter ``list`` receives all the values passed 
to the procedure. The loop therefore iterates 3 times.

Example with fewer actual parameters than formal parameters::

    $indent  
    $procdef print title list  
        $show %title%  
        $foreach i %list%  
            $show print : %i%  
        $next i  
    $procend  
    $procexec print "My Title"

Result::

    My Title

This time, the first parameter ``title`` contains "My Title", which is printed before 
the loop. However, the loop does not execute because the ``list`` parameter is empty.

Scope of a procedure definition
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- Procedures must be defined before they can be called.
- Once defined, a procedure remains callable within the same IODE session, even if the report 
  that defined it has finished. You can execute a report whose sole purpose is to load 
  procedures into memory. These procedures will remain available throughout the session.

A procedure can be replaced at any time by another with the same name.

Scope of formal parameters
^^^^^^^^^^^^^^^^^^^^^^^^^^

Formal parameters are treated as :ref:`defines <define>` whose scope is limited to the 
current procedure. Therefore, if a :ref:`define <define>` exists before the procedure call 
with the same name as one of the parameters, this :ref:`define <define>` cannot be used 
within the procedure. After the procedure, it regains its previous value.

Example::

    $define title Mortality quotients  
    $show Before the proc: %title%  
  
    $procdef print title list  
        $show Inside the proc: %title%  
        $foreach i %list%  
            $show print: %i%  
        $next i  
    $procend  
  
    $procexec print "My Title"  
    $show After the proc: %title%

Result::

    Before the proc: Mortality quotients  
    Inside the proc: My Title  
    After the proc: Mortality quotients

.. _procend:

COMMAND $PROCEND
~~~~~~~~~~~~~~~~

This command indicates the end of a procedure definition.

See :ref:`$procdef <procdef>` for more details.

.. _procexec:

COMMAND $PROCEXEC
~~~~~~~~~~~~~~~~~

This command allows the execution of a procedure.

See :ref:`$procdef <procdef>` for more details.

.. _indent:

COMMAND $INDENT
~~~~~~~~~~~~~~~

By default, commands (``$command`` or ``#command``) must be flush with the left margin. 
If not, they are considered as plain text.
To allow indentation of commands in reports, you must enable it with the `$indent` command. 
Without arguments, it indicates that from this point on, commands do not need to be adjacent 
to the margin. With the argument ``N``, ``n``, or ``0`` (`$indent [{Nn0}]`), commands must 
be adjacent to the margin. To avoid compatibility issues with reports created in older 
versions of IODE, the default is to NOT accept indentation.

For example, you can write::

    $indent  
    
    $procdef print list  
    $------------------  
        $foreach i %list%  
            $show print : %i%  
        $next i  
    $procend  
    
    $procdef print2 list  
    $-------------------  
        $foreach i %list%  
            $show print2 : %i%  
            $procexec print 1 2 3  
        $next i  
    $procend  
  
    $procexec print2 A B C

The result produced is as follows::

    print2 : A  
    print : 1  
    print : 2  
    print : 3  
    print2 : B  
    print : 1  
    print : 2  
    print : 3

Python equivalent functions
~~~~~~~~~~~~~~~~~~~~~~~~~~~

To get the Python equivalent of the above commands, refers to the 
:ref:`Equivalence Between IODE Report Execution Commands And Python Functions <report_execution_commands_equivalence>` table.
