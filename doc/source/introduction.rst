.. _introduction:

Introduction
============

Concepts and Objects
--------------------

IODE operates on objects of 7 different types:

- **Comments**
- **Equations**
- **Identities**
- **Lists**
- **Scalars**
- **Tables**
- **Variables**

These are grouped into 7 dictionaries called *workspaces* (*WS*). 

Different types of objects (e.g. the equations) use formulas in their definition. 
A standard language, specifically adapted for econometric expressions, is used in all these objects. 
This language is called *LEC* which stands for "*Langage Econométrique Condensé*".

Historically, a scripting language has been developed to write and execute *IODE reports*. 
*IODE reports* are text files with an *.rep* extension that contain a sequence of commands to be executed by IODE.
More recently, a *Python* library has been developed to be used in place to or with *IODE reports*.

IODE Objects
------------

A model is a system of *equations*, which are formulas involving *variables*, numerical time series defined 
over a given period, with a specific frequency (annual, quarterly, etc.). 
The *equations* may contain coefficients (possibly estimated) which are dimensionless *variables*, called *Scalars*.

The *variables* themselves are not always obtained as such, but most often result from calculations based on other 
*variables*, possibly coming from several sources. These calculations can be, for example, sector aggregation or a geographic dimension. 
The formulas used to generate those *variables* are called *identities*.

The name given to each *variable* generally does not allow its content to be indicated with sufficient precision. 
IODE allows the creation of *comments* whose name will be identical to that of the *variables* they define. 
These *comments* are simply free text.

When *variables* are available, it is often useful to present them in the form of tables or charts. 
IODE allows the construction of special *tables* for this purpose. Those *tables* do not contain numerical values, 
but formulas and text. Then, these can be "*computed*" so as to obtain numerical tables (called *computed tables*) or charts. 
This approach is very efficient: the same table can be reused to print different versions of the *variables* 
(after simulating a scenario, for example). These *tables* can also be used to compare different scenarios or variants of a model 
by loading different files containing the same *variables*.

There is no notion of a model as an object in IODE: a model is simply a *list* of equations. 
To avoid the tedious work of re-encoding *lists*, these are managed as standalone objects and saved in list files. 
*Lists* are also used in formulas to shorten writing, or passed as parameters to *IODE functions*, etc.

Each object of one of the seven types is identified by a name of up to 20 characters. 
They always start with a letter or an underscore '_'. Their are written in lowercase for *scalars* and in uppercase 
for other objects (so as to distinguish *scalars* and *variables* in *LEC* formulas).

Comments
~~~~~~~~

*Comments* are free text. They are used to document other IODE objects.

Equations
~~~~~~~~~

An *equation* represents an equality mixing *variables* and *scalars* (coefficients) and is part of a model. 
Each equation is composed of the following elements:

- the *LEC* form (the formula scripting language in IODE)
- a free comment (title of the equation)
- the method by which it was estimated (if applicable)
- the possible estimation period
- the names of equations estimated simultaneously (block)
- the instruments used for the estimation

All these definition elements are present in each equation, but may be left empty if not applicable.

The name of an equation is that of its endogenous variable. An equation can never be renamed, 
but it can be deleted and redefined with a new name.

Identities
~~~~~~~~~~

An *identity* is an expression written in the *LEC* language that allows the construction of a 
new statistical series based on already defined series. In general, *identities* are executed
in groups to create or update a set of *variables*. *Identities* can be executed for a specific 
range of periods, or for all periods defined in the workspace.

*Identities* should not be confused with *equations*. They are not part of a model.

Lists
~~~~~

*Lists* are, like comments, free text (or a Python list if the Python syntax is used). 
They are used to simplify writing in various circumstances:

- list of equations defining a model
- list of tables to print
- any argument of a function (such as print period)
- macro in an equation, identity, or table
- etc.

Scalars
~~~~~~~

*Scalars* are essentially estimated coefficients of econometric *equations*. 
For this reason, each scalar contains in its definition:

- its *value*
- the *relaxation* parameter, set to 0 to lock the coefficient during estimation
- its *t-test*, result of the last estimation
- its *standard deviation*, result of the last estimation

Only the *values* of the *scalars* are relevant when calculating a *LEC* expression. 
The other three values (relaxation, t-test and standard deviation) are only meaningful for estimation.

The names of *scalars* must be in lowercase so that *variables* are distinct from *scalars* in *LEC* formulas.

Tables
~~~~~~

One of the most frequently performed operations during a simulation exercise is the display of tables of results and charts. 

Each IODE *table* is a set of lines. A line is composed of two parts (in general):

- a *text* part, which will be the title of the line
- a *formula* part, which will allow the calculation of the numerical values to be placed in the *computed table*::

    TABLE TITLE    
    -----------    
    Gross National Product      GNP    
    Unemployment                UL    
    External Balance            X-I


The lines are actually of several types: 

- *TITLE* lines (centered on the page width), 
- *CELL* lines (title + formula), 
- *SEPARATOR* lines
- *MODE* lines
- *FILES* lines
- *DATE* lines

A table is designed to be "*computed*" over different periods, described by a "*generalized sample*" 
such as::

    1980Y1:10               --> 10 observations from 1980Y1
    1980Y1, 1985Y1, 1990:5  --> 1980, 1985, then 5 observations from 1990Y1
    80/79:5                 --> 5 growth rates from 1980
    ...


It can also contain values from different files::

    (1990:5)[1,2,1-2]   --> values from 1990 to 1994 for files
                            1, 2, and for the difference between the two files.


The *computed table* can be:

- displayed on screen
- printed
- exported as a chart
- exported to a file (in CSV, HTML, ...)
- (Python) converted to a Pandas DataFrame or an larray Array

*Tables* can very well be used in a project that does not include an econometric model: 
the only information used by tables are *variables* and possibly *scalars*.

Variables
~~~~~~~~~

*Variables* are series of numbers.

All *variables* from the "*variables*" workspace are defined over the same range of periods (sample). 
If observations are missing, they take the special value `NA` (Not Available) 
(displayed as `--` in the graphical user interface).

Their names must be in uppercase so that *variables* are distinct from *scalars* in *LEC* formulas.

Workspaces
----------

During an IODE session, the memory space is also divided into 7 parts: each is reserved for 
one type of object and is called a workspace. Therefore, there are always 7 *active* workspace 
during a work session. Initially, all workspaces are empty.

The workspaces are stored in memory for the duration of an IODE session. 
When you exit the program, all data is lost. You must not forget to save their content before exiting 
the IODE report or user interface.

Among the functions that act on entire workspaces, you will find in particular:

    +--------------+-----------------------+----------------------------------------------------------+
    | IODE command | Python method         | Description                                              |
    +==============+=======================+==========================================================+
    | `$WsLoad`    | `workspace.load`      | loads objects from a file into memory                    |
    +--------------+-----------------------+----------------------------------------------------------+
    | `$WsSave`    | `workspace.save`      | dumps the objects of a workspace in a file               |
    +--------------+-----------------------+----------------------------------------------------------+
    | `$WsCopy`    | `workspace.copy_into` | copies objects from a file to a workspace                |
    +--------------+-----------------------+----------------------------------------------------------+
    | `$WsClear`   | `workspace.clear`     | destroys all objects in a workspace                      |
    +--------------+-----------------------+----------------------------------------------------------+
    | `$WsSample`  | `variables.sample`    | changes the range of periods of the variables workspace  |
    +--------------+-----------------------+----------------------------------------------------------+

where *workspace* in the Python method column must be replaced by either *comments*, *equations*, ... or *variables*.

The econometric functions (*estimation*, *simulation*) and the function for constructing series based on *identities* 
indirectly affect *variables* and *scalars* by modifying their values.

Summary:

- At the beginning of a session, the workspace are loaded into memory from files  
- During the session, objects are modified, created, or deleted in the workspaces
- At the end of the session (or at any time), you must save the workspace to files on disk in order to retrieve them in later sessions.  

LEC - the IODE Formula Language
-------------------------------

Whether for *equations*, construction *identities*, or charts, an appropriate mathematical 
formula language is essential. The *LEC* language ("*Langage Econométrique Condensé*") has 
been designed to be well-suited for formulas involving time series. 
It is used whenever a formula is needed in IODE.

The *LEC* language has been designed to be close to the way formulas are written in the literature.

Example 1: Consumption equation:

.. math::

    C_t = a + b~\frac{Y_t}{P_t} + c * C_{t-1}

is translated into the *LEC* language as follows::

    C := a + b * Y / P + c * C[-1]     
    
Example 2: Production equation:

.. math::

    ln(Q_t) = a~ln(K_t) + (1 - a)~ln(L_t) + c * t + B

is translated into the *LEC* language as follows::

    ln Q := a * ln K + (1 - a) * ln L + c * t + b


The *LEC* language will be described in detail later in a separate chapter. 
Here are just a few interesting features, some of which appear in the examples above:

- more than 20 mathematical operators are integrated into the language: trigonometric functions, hyperbolic functions, logarithms, exponentials, max, min, etc.
- more than 10 time functions: maximum, minimum, sum and product over a period, lags, leads, differences and growth rates of any degree (including calculated), moving averages, standard deviations, etc.
- lags, leads, and periods are written simply, including on expressions, and can be combined::

    (A + B)[-1]            
    UL[+1]            
    (X + dln Y)[1985Y1]

- parts of formulas can be temporarily commented out by enclosing them in comments (/\* ... \*/)::

    ln Q := a * ln K /* + (1 - a) ln (L + Y + Z) */

- *lists* (macros) can be used in formulas::

    A + B := c1 + c2 * $LL + c3 * $ZZ

Reports: Scripting IODE
-----------------------

An IODE report is an ASCII file (with a .rep extension) containing two types of elements:

- instructions (lines that start with $ or #)
- free text (all other lines)

Executing a report results in a sequence of operations (loading a file, estimating equations, 
printing a table, etc.). Instructions starting with `$` are commands that do not display 
anything on the screen. Those starting with `#` interact with the screen, either to display 
information or to ask questions before to proceed.

There are several groups of instructions:

- those that control the flow of a report (*Label*, *GoTo*, *Ask*, *Return*, *Quit*, *OnError*, *Foreach*, *ProcExec*, etc.)
- those that execute IODE functions (*WsLoadEqs*, *WsSaveVar*, *FileDeleteA2m*, *DataEditVar*, *PrintTbl*, *ModelSimulate*, *ReportExec*, etc.)

The second type of instructions allows you to perform almost all operations that are normally 
done from the menus of the graphical user interface.

Before execution, each report line is analyzed and, if necessary, modified: you can use macros, 
replace *lists* with their contents, perform calculations, etc.

Report Examples
~~~~~~~~~~~~~~~

The following example loads workspaces of different types. 
Then, it sets the print destination (here to the intermediate format A2M). 
Finally, a beep is produced and a message indicates that the environment is loaded.

Typically, running this report starts an IODE session by setting an initial environment::

    $WsLoadVar fun\fun.var    
    $WsLoadEqs fun\fun.eqs    
    $WsLoadScl fun\fun.scl     
    
    $PrintDest tmp.a2m A2M    
    #Beep    
    #Show Environnement fun chargé


The following report defines the output file for execution (bist92\\bistelf1.rtf), 
then displays a message, builds the *computed tables,* and prints them. 
Free text is included in this report ("TABLES OF ASSUMPTIONS" for example)::

    $PrintDest bist92\bistelf1.rtf RTF    
    #Show processing french tables    
    $PrintNbdec 1        
    
    TABLES OF ASSUMPTIONS        
    -----------------------     
    $PrintTbl 89:8 HYPEIR    
    $PrintTbl 89/88:8 HYPEIIR     
    $PrintNbdec 0    .page        
    
    TABLES OF RESULTS        
    ----------------------     
    $PrintTbl 89:8 RESL00     
    $PrintNbdec 1    
    $PrintTbl 89/88:8 RESL00R    
    $PrintTbl 89:8 RESL03    
    #Beep
