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


.. include:: reports_execution_commands.rst


.. include:: reports_functions.rst


.. include:: reports_commands.rst


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
