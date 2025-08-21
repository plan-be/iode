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

- :ref:`filelist <filelist>`
- :ref:`fileprint <fileprint>`
- :ref:`fileedit <fileedit>`
- :ref:`filedelete <filedelete>`
- :ref:`filerename <filerename>`
- :ref:`filecopy <filecopy>`
- :ref:`SysCopyFile <syscopyfile>`
- :ref:`SysMoveFile <sysmovefile>`
- :ref:`SysDeleteFile <sysdeletefile>`
- :ref:`SysOemToAnsi <sysoemtoansi>`
- :ref:`SysOemToUTF8 <sysoemtoutf8>`
- :ref:`SysAnsiToOem <sysansitooem>`
- :ref:`SysAnsiToUTF8 <sysansitoutf8>`
- :ref:`SysAppendFile <sysappendfile>`
- :ref:`FileImportVar <fileimportvar>`
- :ref:`FileImportCmt <fileimportcmt>`

.. _filelist:

COMMAND FILELIST
^^^^^^^^^^^^^^^^

.. warning:: 

    This function is deprecated as of version 5 of IODE.

.. _fileprint:

COMMAND FILEPRINT
^^^^^^^^^^^^^^^^^

This command allows the user to print a file. The file must be a text file.
The function prints the file as-is to the defined printer (see `Print Destination`).
The file is simply appended to the selected printer file.
The file name (or a mask) is passed as a parameter to the command.

Syntax::

    $FilePrint filename

Example::

    $FileDeleteA2m  imp  
    $PrintDest      imp.a2m  
    $FilePrint      bist92\read.me  
    $FileEdit       imp.a2m  
    $show           End of report...

.. _fileedit:

COMMAND FILEEDIT
^^^^^^^^^^^^^^^^

.. warning:: 

    This function is deprecated as of version 5 of IODE.

.. _filedelete:

COMMAND FILEDELETE XXX
^^^^^^^^^^^^^^^^^^^^^^

*XXX* takes one of the following values::

    cmt,    eqs,    idt,    lst,    scl,    tbl,    var,  
    ac,     ae,     ai,     al,     as,     at,     av,  
    rep,    a2m,    agl,    prf,    dif,    mif,    rtf,  
    ps,     asc,    txt,

The function deletes a file on disk with one of the following extensions::

    cmt,    eqs,    idt,    lst,    scl,    tbl,    var,  
    ac,     ae,     ai,     al,     as,     at,     av,  
    rep,    a2m,    agl,    prf,    dif,    mif,    rtf,  
    ps,     asc,    txt

You pass as a parameter the list of files with the chosen extension to delete.

Syntax::

    $FileDeleteCmt file [list of files]

Example::

    $FileDeleteA2m  imp  
    $PrintDest      imp.a2m  
    
    $PrintObjDefEqs  
    $FileEdit       imp.a2m  
    $show End of report...

.. _filerename:

COMMAND FILERENAME XXX
^^^^^^^^^^^^^^^^^^^^^^

*XXX* takes one of the following values::

    cmt,    eqs,    idt,    lst,    scl,    tbl,    var,  
    ac,     ae,     ai,     al,     as,     at,     av,  
    rep,    a2m,    agl,    prf,    dif,    mif,    rtf,  
    ps,     asc,    txt,

The function renames a file on disk with one of the following extensions::

    cmt,    eqs,    idt,    lst,    scl,    tbl,    var,  
    ac,     ae,     ai,     al,     as,     at,     av,  
    rep,    a2m,    agl,    prf,    dif,    mif,    rtf,  
    ps,     asc,    txt

You pass as parameters the source file name (old name) and the destination 
file name (new name).

Syntax::

    $FileRenameCmt source_file dest_file

Example::

    $FileRenameCmt  maribel old

.. _filecopy:

COMMAND FILECOPYXXX
^^^^^^^^^^^^^^^^^^^

*XXX* takes one of the following values::

    cmt,    eqs,    idt,    lst,    scl,    tbl,    var,  
    ac,     ae,     ai,     al,     as,     at,     av,  
    rep,    a2m,    agl,    prf,    dif,    mif,    rtf,  
    ps,     asc,    txt,

The function copies a file with one of the following extensions::

    cmt,    eqs,    idt,    lst,    scl,    tbl,    var,  
    ac,     ae,     ai,     al,     as,     at,     av,  
    rep,    a2m,    agl,    prf,    dif,    mif,    rtf,  
    ps,     asc,    txt

You pass as parameters the source file name (old name) and the destination 
file name (new name).

Syntax::

    $FileCopyCmt source_file dest_file

Example::

    $FileCopyCmt  matibel old

.. _syscopyfile:

COMMAND SYSCOPYFILE
^^^^^^^^^^^^^^^^^^^

This function allows the user to copy any file on disk.
Unlike the :ref:`FileCopyXxx <filecopy>` function, the extension can be anything.

.. warning:: 

    The file to be copied must be closed in order to be copied.
    If not possible, the function returns an error code.

Syntax::

    $SysCopyFile filein fileout

Example::

    $PrintDest indprix.a2m a2m  
    ...  
    $PrintDest test a2m         (to close the file indprix.a2m)  
    $SysCopyFile indprix.a2m indprix.htm

.. _sysmovefile:

COMMAND SYSMOVEFILE
^^^^^^^^^^^^^^^^^^^

This function allows the user to rename any file on disk.
Unlike the :ref:`FileRenameXxx <filerename>` function, the extension can be anything.

.. warning::

    The file to be renamed must be closed.
    The destination file cannot exist.
    If not possible, the function returns an error code.

Syntax::

    $SysMoveFile filein fileout

Example::

    $PrintDest indprix.a2m a2m  
    ...  
    $PrintDest test a2m         (to close the file indprix.a2m)  
    $SysMoveFile indprix.a2m indprix.htm

.. _sysdeletefile:

COMMAND SYSDELETE FILE
^^^^^^^^^^^^^^^^^^^^^^

This function allows the user to delete any file on disk.
Unlike the :ref:`FileDeleteXxx <filedelete>` function, the extension can be anything.

.. warning:: 

    If the file does not exist, the function does NOT return an error code.

Syntax::

    $SysDeleteFile file1 file2 ...
    
Example::
    
    $PrintDest indprix.a2m a2m  
    ...  
    $PrintDest test a2m         (to close the file indprix.a2m)  
    $SysDeleteFile indprix.htm  
    $SysMoveFile indprix.a2m indprix.htm

.. _sysappendfile:

COMMAND SYSAPPEND FILE
^^^^^^^^^^^^^^^^^^^^^^

This function allows the user to append one file to another.
If not possible, the function returns an error code.

Syntax::

    $SysAppendFile filein fileout

Example::

    $PrintDest tmp.a2m a2m  
    ...  
    $PrintDest test a2m               (to close the file tmp.a2m)  
    $SysAppendFile tmp.a2m result.a2m  
    $PrintDest tmp.a2m a2m  
    ...  
    $PrintDest test a2m               (to close the file tmp.a2m)  
    $SysAppendFile tmp.a2m result.a2m (accumulates the tmp.a2m files in result.a2m)

.. _fileimportvar:

COMMAND FILEIMPORT VAR
^^^^^^^^^^^^^^^^^^^^^^

This function concerns the import of IODE variables in different formats such as
``DIF``, ``ASCII``, ``rotated ASCII``, ``DIF Belgostat``, ``TXT Belgostat``, ``PRN Aremos``
and ``GEM`` (*Belgostat*).

Syntax::

    $FileImportVar format rule infile outfile from to  [trace]
    where format = {A, R, D, N, G, P, T}  
       rule = rulefile to use to translate names  
       infile = file to import  
       outfile = IODE-file with imported series  
       from = begin of sample  
       to   = end of sample  
       trace = debug file (optional)

Example::

        $FileImportVar TXT bstrule.txt bf-06obs.txt bh\p6y.var 1980Y1 2000Y1 p6y.log  
        $FileImportVar GEM rule.gem infile.gem iode\test.var 1980Y1 1990Y1

.. _fileimportcmt:

COMMAND FILEIMPORT CMT
^^^^^^^^^^^^^^^^^^^^^^

This function concerns the import of IODE comments in different formats such as
``DIF``, ``ASCII``, ``rotated ASCII``, ``DIF Belgostat``, ``TXT Belgostat`` 
and ``PRN Aremos``.

Syntax::

    $FileImportCmt format rule infile outfile language [trace]
    where format = {Ascii, Rotated_Ascii, DIF, NIS, GEM, PRN, TXT_Belgostat}  
       rule = rulefile to use to translate names  
       infile = file to import  
       outfile = IODE-file with imported series  
       language =  {E,F,D}  
       trace = debug file (optional)
       
Example::
    
    $FileImportCmt TXT bstrule.txt bf-06ser.txt bh\p6d.cmt D p6d.log


Workspace operations
~~~~~~~~~~~~~~~~~~~~

These functions allow the user to join, copy, load, and save workspaces:

- :ref:`WsLoad <wsload>` : loads a workspace cmt / eqs / idt / lst / scl / tbl / var from a file
- :ref:`WsCopy <wscopy>` : copies a workspace cmt / eqs / idt / lst / scl / tbl / var 
- :ref:`WsMerge <wsmerge>` : merges a workspace cmt / eqs / idt / lst / scl / tbl / var into another
- :ref:`WsClear <wsclear>` : deletes a workspace cmt / eqs / idt / lst / scl / tbl / var 
- :ref:`WsClearAll <wsclearall>` : clears all workspace 
- :ref:`WsDescr <wsdescr>` : describes a workspace cmt / eqs / idt / lst / scl / tbl / var 
- :ref:`WsSave <wssave>` : saves a workspace cmt / eqs / idt / lst / scl / tbl / var to a file
- :ref:`WsSaveCmp <wssavecmp>` : saves a workspace cmt / eqs / idt / lst / scl / tbl / var to a file in compressed form 
- :ref:`WsSample <wssample>` : changes the sample of the *variables* workspace 
- :ref:`WsExtrapolate <wsextrapolate>` : extends series by extrapolation 
- :ref:`WsLtohStock <wsltohstock>` : builds higher frequency series for stocks 
- :ref:`WsLtohFlow <wsltohflow>` : builds higher frequency series for flows 
- :ref:`WsHtolSum <wshtolsum>` : builds lower frequency series (sum) 
- :ref:`WsHtolMean <wshtolmean>` : builds lower frequency series (mean) 
- :ref:`WsHtolLast <wshtollast>` : builds lower frequency series (last observation) 
- :ref:`WsSeasonAdj <wsseasonadj>` : builds seasonally adjusted series 
- :ref:`WsSeasAdj <wsseasadj>` : selects the seasonal adjustment criterion 
- :ref:`WsTrend <wstrend>` : computes trend series (Hodrick-Prescott) using log transformation 
- :ref:`WsTrendStd <wstrendstd>` : computes trend series (Hodrick-Prescott) using the standard method 
- :ref:`WsImport <wsimport>` : imports an ASCII file 
- :ref:`WsExport <wsexport>` : exports to a file in ASCII format 
- :ref:`WsImportEviews <wsimporteviews>` : imports E-Views equations and scalars 
- :ref:`CsvSave <csvsave>` : saves a workspace in csv format 
- :ref:`CsvDigits <csvdigits>` : sets the number of decimals in CSV files 
- :ref:`CsvSep <csvsep>` : sets the cell separator in CSV files 
- :ref:`CsvDec <csvdec>` : sets the decimal separator in CSV files 
- :ref:`CsvNaN <csvnan>` : sets the text indicating an undefined value in CSV files 
- :ref:`CsvAxes <csvaxes>` : sets the name of the variable axis in CSV files 

.. _wsload:

COMMAND WSLOADXXX
^^^^^^^^^^^^^^^^^

*XXX* takes one of the following values::

    cmt / eqs / idt / lst / scl / tbl / var

The LOAD operation of a workspace replaces the contents of the current workspace with the
contents of a disk file: all currently defined objects are deleted and replaced by those 
defined in the file to be loaded.

The command parameter is the path to the file to load.

Syntax::

    $WsLoadXxx filename
    where Xxx = cmt / eqs / idt / lst / scl / tbl / var

Example::

    $WsLoadCmt maribel.cmt

If you load a new variables file, the sampling period of the workspace is replaced by that 
of the loaded file.

.. _wscopy:

COMMAND WSCOPYXXX
^^^^^^^^^^^^^^^^^

*XXX* takes one of the following values::

    cmt / eqs / idt / lst / scl / tbl / var

This function allows the user to add or replace objects from a workspace file saved on disk.
The selected objects are copied into the current workspace and replace the current values.
The process stops as soon as an object cannot be found.
All other objects in the current workspace remain unchanged.

You pass as parameters the name of the workspace file on disk and the list of objects to copy.

Syntax::

    $WsCopyXxx file;file;... {object | list of objects}
    where Xxx = cmt / eqs / idt / lst / scl / tbl  

Example::

    $WsCopyCmt maribel.cmt CMT1 $CMT    
    
If you copy variables (VAR) from another file, the sample is handled as follows:

- if no sample is defined: the sample from the file to copy is used
- if a variables workspace is currently in use, only the data from the new file that 
  falls within the current sampling period is copied
- you can also define the sampling period into which the data from the file will be copied

If series do not exist in the current workspace, they are created.

Syntax::

    $WsCopyVar file;file;... [from to] {object | list of objects}  

Example::

    $WsCopyVar maribel.var 1990Y1 2000Y1 A

copies the variable *A* from the file *maribel.var* into the workspace for the period 
1990 to 2000 only. If *A* does not exist, it is created and only the values for the 
specified period are copied.

Example::

    $WsCopyVar maribel.var;hermes.var A B C
    
same as above, but for the entire sample of the file *maribel.var*.

.. _wsmerge:

COMMAND WSMERGEXXX
^^^^^^^^^^^^^^^^^^

*XXX* takes one of the following values::

    cmt / eqs / idt / lst / scl / tbl / var

This function performs the operation of merging several files with the current workspace:
it adds to the active workspace the objects defined in a file.

The command parameter is the name of the file.

Syntax::

    $WsMergeXxx filename  
    where Xxx = cmt / eqs / idt / lst / scl / tbl / var

Example::

    $WsMergeCmt mycmts.cmt

.. _wsclear:

COMMAND WScalarEARXXX
^^^^^^^^^^^^^^^^^^

*XXX* takes one of the following values::

    cmt / eqs / idt / lst / scl / tbl / var

This function deletes all objects of the specified type.
No parameter is required.

Syntax::

    $WsClearXxx  
    where Xxx = cmt / eqs / idt / lst / scl / tbl / var

Example::

    $WsClearEqs

.. _wsclearall:

COMMAND WScalarEARALL
^^^^^^^^^^^^^^^^^^

Clear all workspaces.

.. _wsdescr:

COMMAND WSDESCRXXX
^^^^^^^^^^^^^^^^^^

*XXX* takes one of the following values::

    cmt / eqs / idt / lst / scl / tbl / var

This function creates or replaces the description of the file.
This description will be saved later if you save the file.

You pass the file description as a parameter.

Syntax::

    $WsDescrXxx text  
    where Xxx = cmt / eqs / idt / lst / scl / tbl / var

Example::

    $WsDescrCmt  The MIRABEL comments file

.. _wssave:

COMMAND WSSAVEXXX
^^^^^^^^^^^^^^^^^

*XXX* takes one of the following values::

    cmt / eqs / idt / lst / scl / tbl / var

This function performs the inverse operation of the LOAD command: it saves the 
current state of the workspace to disk, overwriting any existing files if necessary.

The command parameter is the name of the file to save.

Syntax::

    $WsSaveXxx filename  
    where Xxx = cmt / eqs / idt / lst / scl / tbl / var

Example::

    $WsSaveCmt  current2.cmt

.. _wssavecmp:

COMMAND WSSAVECMP
^^^^^^^^^^^^^^^^^

The different workspaces can be compressed when saving them.
Depending on the type of object and their definition, compression can range from 30% to 75%.
Since each object is compressed individually, the time required for saving can be significant 
for files containing many objects or on slow machines. IODE automatically recognizes and 
converts compressed files. Loading is hardly affected by the decompression procedure. 
There is only one exception: variable files whose series must be loaded using a function such 
as `$WsCopyVar`: in this case, reading is slowed down because the lengths of the compressed 
series become variable and the file must therefore be read sequentially. This function is 
identical to :ref:`$WsSave <wssave>` except that it compresses the files when saving the data.

Syntax::

    $WsSaveCmpXxx filename  
    where Xxx = cmt / eqs / idt / lst / scl / tbl / var

.. _wssample:

COMMAND WSSample
^^^^^^^^^^^^^^^^

This function allows the user to modify the sample of the IODE variables workspace.
If the new sample is shorter than the current sample, data beyond the new range is deleted.
If the new sample is longer than the current sample, the value `NA` (Not Available) is 
assigned to the added periods.

Syntax::

    $WsSample from to

Example::

    $WsSample 1990Y1 2000Y1

.. _wsextrapolate:

COMMAND WSEXTRAPOLATE
^^^^^^^^^^^^^^^^^^^^^

This function allows the user to complete series using a chosen method based on previous periods.
The possible methods are as follows:

- 0: ``Y := Y[-1], if Y is null or NA``
- 1: ``Y := Y[-1], always``
- 2: ``Y := extrapolation, if Y is null or NA``
- 3: ``Y := extrapolation, always``
- 4: ``Y := unchanged, always``
- 5: ``Y := Y[-1], if Y is NA``
- 6: ``Y := extrapolation, if Y is NA``

You must provide the period (included in the one defined in the current workspace) over which
the calculation should be performed. In addition, the list of variables to adapt can also be
specified. If this list is left empty, all series in the workspace are modified.

Syntax::

    $WsExtrapolate [method] from to [list of variables]
    where method: 0 ... 6
          from, to := IODE periods (yyyyPpp)

Example::

    $WsExtrapolate 1993Y1 2000Y1 A B C
    or
    $WsExtrapolate 1993Y1 2000Y1

.. _wsltohstock:

COMMAND WSLTOHSTOCK
^^^^^^^^^^^^^^^^^^^

Builds higher frequency series for stock-type data (Unemployment, Debt, ...).
To do this, the function loads into the IODE variables workspace the list of series from 
the specified file and simultaneously changes the frequency of these series.
The new frequency is the one currently defined in the active workspace.
The new series are added or replace (for existing names) those in the active workspace.

This procedure exists for the following cases:

- annual to monthly
- annual to quarterly
- quarterly to monthly

Two methods are available, one for stocks, the other for flows 
(:ref:`$WsLtoHFlow <wsltohflow>`). For stocks, the interpolation method can be chosen:

- linear: ``A[1980Q{1,2,3,4}] = A[1979Y1] + i * (A[1980Y1] - A[1979Y1])/4 for i = 1,2,3,4``
- cubic splines: ``cubic interpolation``
- step: ``A[1980Q{1,2,3,4}] = A[1980Y1]``    

Syntax::

    $WsLtoHStock {L|C|S} Filename VarList  
  
    with L for linear interpolation  
         C for Cubic Splines interpolation  
         S for Steps interpolation

Example::

    $WsLtoHStock C File1.var A B C

.. _wsltohflow:

COMMAND WSLTOHFLOW
^^^^^^^^^^^^^^^^^^

Builds higher frequency series for flow-type data (GDP, Deficit, ...).
To do this, the function loads into the IODE variables workspace the list of series 
from the specified file and simultaneously changes the frequency of these series.
The new frequency is the one currently defined in the active workspace.
The new series are added or replace (for existing names) those in the active workspace.

This procedure exists for the following cases:

- annual to monthly
- annual to quarterly
- quarterly to monthly

Two methods are available, one for stocks, the other for flows.
For flows, the series is distributed over the sub-periods:

- linear interpolation: ``A[1980Q1] = A[1980Y1] / n`` (where *n* = number of sub-periods)
- cubic spline interpolation: cubic interpolation
- step interpolation: linear

Syntax::

    $WsLtoHFlow {L|C|S}  Filename VarList  

    with L for linear interpolation  
         C for Cubic Splines interpolation  
         S for Steps interpolation

Example::

    $WsLtoHFlow S File1.var A B C

.. _wshtolsum:

COMMAND WSHTOLSUM
^^^^^^^^^^^^^^^^^

Builds lower frequency series by summing the sub-periods.
To do this, the function loads into the IODE variables workspace the list of series from the specified file and simultaneously changes the frequency of these series.
The new frequency is the one currently defined in the active workspace.
The new series are added or replace (for existing names) those in the active workspace.

This procedure exists for the following cases:

- monthly to annual (annual observation = sum of the 12 months)
- quarterly to annual (annual observation = sum of the 4 quarters)
- monthly to quarterly (quarterly observation = sum of the 3 months)

Three methods are available:

- Addition of sub-periods (flow): :ref:`$WsHtoLSum <wshtolsum>`
- Mean of sub-periods (stock): :ref:`$WsHtoLMean <wshtolmean>`
- Last observation (stock): :ref:`$WsHtoLLast <wshtollast>`

If a value is missing (`NA`) for one of the sub-periods, the result is `NA`.

Syntax::

    $WsHtoLSum Filename VarList

Example::

    $WsHtoLSum File1.var A B C

.. _wshtolmean:

COMMAND WSHTOLMEAN
^^^^^^^^^^^^^^^^^^

Builds lower frequency series by taking the mean of the sub-periods.
To do this, the function loads into the IODE variables workspace the list of series 
from the specified file and simultaneously changes the frequency of these series.
The new frequency is the one currently defined in the active workspace.
The new series are added or replace (for existing names) those in the active workspace.

This procedure exists for the following cases:

- monthly to annual (annual observation = mean of the 12 months)
- quarterly to annual (annual observation = mean of the 4 quarters)
- monthly to quarterly (quarterly observation = mean of the 3 months)

Three methods are available:

- Addition of sub-periods (flow): :ref:`$WsHtoLSum <wshtolsum>`
- Mean of sub-periods (stock): :ref:`$WsHtoLMean <wshtolmean>`
- Last observation (stock): :ref:`$WsHtoLLast <wshtollast>`

If a value is missing (`NA`) for one of the sub-periods, the result is `NA`.

Syntax::

    $WsHtoLMean Filename VarList

Example::

    $WsHtoLMean File1.var A B C

.. _wshtollast:

COMMAND WSHTOLLAST
^^^^^^^^^^^^^^^^^^

Builds lower frequency series by taking the last observation of the sub-periods.
To do this, the function loads into the IODE variables workspace the list of series 
from the specified file and simultaneously changes the frequency of these series.
The new frequency is the one currently defined in the active workspace.
The new series are added or replace (for existing names) those in the active workspace.

This procedure exists for the following cases:

- monthly to annual (annual observation = that of December)
- quarterly to annual (annual observation = that of the last quarter)
- monthly to quarterly (quarterly observation = that of the last month of the quarter)

Three methods are available:

- Addition of sub-periods (flow): :ref:`$WsHtoLSum <wshtolsum>`
- Mean of sub-periods (stock): :ref:`$WsHtoLMean <wshtolmean>`
- Last observation (stock): :ref:`$WsHtoLLast <wshtollast>`

If a value is missing (`NA`) for one of the sub-periods, the result is `NA`.

Syntax::

    $WsHtoLLast Filename VarList

Example::

    $WsHtoLLast File1.var A B C

.. _wsseasonadj:

COMMAND WSSEASONADJ
^^^^^^^^^^^^^^^^^^^

Builds seasonally adjusted series using the ``Census XI`` method, as well as the 
trend-cycle and stochastic components.

Naming convention:

- The series containing the trend-cycle component is named ``_Cname`` where *name* 
  is the original name.
- The series containing the stochastic component is named ``_Iname`` where *name* 
  is the original name.
- The seasonally adjusted series keeps its original name.

Note that the seasonally adjusted series is the product of the other two.
To do this, the function loads into the IODE variables workspace the list of series from 
the specified file and simultaneously modifies the selected series if necessary.
The new series are added or replace (for existing names) those in the active workspace.
If a value is missing (`NA`) for one of the periods, the result is `NA` for the entire year.

Syntax::

    $WsSeasonAdj Filename VarList

Example::

    $WsSeasonAdj File1.var A B C

See also :ref:`$WsSeasAdj <wsseasadj>`. 

.. _wsseasadj:

COMMAND WSSEASADJ
^^^^^^^^^^^^^^^^^

This function is identical to :ref:`$WsSeasAdj <wsseasadj>` except that it allows you 
to set the parameter that controls the criterion for detecting whether a seasonal effect 
is present in a series.

Syntax::

    $WsSeasAdj Filename Varname Varname ... Eps   

.. _wstrend:

COMMAND WSTREND
^^^^^^^^^^^^^^^

Implementation of the Hodrick-Prescott method for constructing trend series.
The principle is the same as for seasonal adjustment: the series read from a file
are imported and transformed simultaneously.

Syntax::

    $WsTrend VarFilename Lambda series1 series2 ...

.. _wsimport:

COMMAND WSIMPORTXXX
^^^^^^^^^^^^^^^^^^^

*XXX* takes one of the following values::

    cmt / eqs / idt / lst / scl / tbl / var

This operation allows the user to perform a :ref:`$WsLoad <wsload>` from an ASCII file.
Remember that the LOAD operation of a workspace replaces the contents of the current 
workspace with the contents of a disk file: all currently defined objects are deleted 
and replaced by those defined in the file to be loaded.

You pass the name of the file to load as a parameter to the command.

Syntax::

    $WsImportXxx filename
    where Xxx = cmt / eqs / idt / lst / scl / tbl / var

Example::

    $WsImportCmt myfile.ac

If you load a new IODE variables file, the sampling period of the workspace is replaced 
by that of the loaded file.

.. _wsexport:

COMMAND WSEXPORTXXX
^^^^^^^^^^^^^^^^^^^

*XXX* takes one of the following values::

    cmt / eqs / idt / lst / scl / tbl / var

This function performs the inverse operation of the :ref:`$WsImportXxx <wsimport>` 
function: it saves the current state of the workspace to disk in ASCII format.

You pass the name of the file to save as a parameter to the command.

Syntax::

    $WsExportXxx filename  
    where Xxx = cmt / eqs / idt / lst / scl / tbl / var

Example::

    $WsExportCmt mytest.ac

.. _wsimporteviews:

COMMAND WSIMPORTEVIEWS
^^^^^^^^^^^^^^^^^^^^^^

Extraction of equations, scalars and identities from E-Views export data.

Syntax::

    $WsImportEviews filename

where filename is the name of the file to import. 
The current IODE workspaces are augmented with the detected equations and scalars.

Format of E-Views data::

    Forecasting Equation  
    ====================

    D(B_PQVC) = C(1)*D(PQVC) + C(2)*(D2001+D2002) + C(3)*D(B_PQVC(-3))  
    
    Substituted Coefficients  
    ======================== 

    D(B_PQVC) = 0.894661573413*D(PQVC) - 0.0284533462569*(D2001+D2002) + 
    0.241546373731*D(B_PQVC(-3))  
    
    @IDENTITY gro_w_produz_def  = w_produz_def  / w_produz_def(-1)  - 1  
    
    Identities      
    ========== 
    KEEP COEFS:  
    B_QHOA  = b_1(2)  * b_c0122222  
    B_QHOB  = b_qh_b_14(1)  * B_CO14  
    
    DROP COEFS:  
    B_QHOCADD  = (b_qh_c_1(1)  * B_CO1)  + (b_qh_c_2(1)  * B_CO2)  + (b_qh_c_7(1)  * 
    B_CO7)  + (b_qh_c_13(1)  * B_CO13)  + (b_qh_c_14(1)  * B_CO14)  
    B_QHOCR_NC  = b_qh_cr_14(1)  * B_CO14  
 
The E-views file is interpreted as decribed below:

- the 2 equations following the titles "Forecasting Equation" and "Substituted Coefficients" 
  are extracted.
- the first equation is translated into IODE format:
    - ``D(...)`` is replaced by ``d(...)``
    - ``C(<n>)`` is replaced by the ``endoname_<n>``
    - ``Expr(<lag>)`` is replaced by ``Expr[<lag>]``
    - the first encountered variable is chosen as the endogenous variable
    - the first ``=`` sign is considered as the separator between left and right members 
      of the equation and is therefore replaced by ``:=``
- the members ``NAME(n)`` are remplaced by ``name_n`` or ``name``. If the last directive  
  is ``KEEP COEFS:``, the ``_n`` is kept. If the last directive is ``DROP COEFS:``, ``_n`` 
  is dropped.
- the coefficients values are extracted from the second equation ("Substituted Coefficients").
- the lines ``Estimation Command:`` and ``Estimation Equation:`` are ignored
- the lines beginning with ``@INNOV`` are skipped.
- the lines beginning by ``@IDENTITY`` are extracted and translated in IODE equations 
  with no coefficient.
- every equation is added in the current Equations workspace.
- every detected coefficient is saved in the Scalars workspace.

Lines following the line IDENTITIES
"""""""""""""""""""""""""""""""""""

Lines containing identities are interpreted as equations from the moment:

- either a line containing the text ``Identities:`` is found
- or a first identity prefixed by ``@IDENTITY`` is found

From this point, all lines containing text are interpreted by the program as IODE equations.
If there is a syntax error, reading stops and an error message is produced.

KEEP COEFS: and DROP COEFS: directives
""""""""""""""""""""""""""""""""""""""

These directives must appear alone on a line. They can be uppercase, lowercase, or a 
mix of both. They determine how EVIEWS equation or identity terms of the form ``name(1)`` 
or ``NAME(n)`` are translated into IODE::

    KEEP COEFS:  
    DROP COEFS:

- After ``KEEP COEFS:``, the translation of ``name(1)`` or ``NAME(1)`` gives ``name_1``.
- After ``DROP COEFS:``, the translation of ``name(1)`` or ``NAME(1)`` gives ``name``.

At the start of file reading, ``DROP COEFS:`` is the initial value.

Example of an EVIEWS file::

    Identities  
    ==========
    KEEP COEFS  
    B_QHOA  = b_1(2)  * b_c0122222  
    B_QHOB  = b_qh_b_14(1)  * B_CO14  
    
    DROP COEFS  
    B_QHOCADD  = (b_qh_c_1(1)  * B_CO1)  + (b_qh_c_2(1)  * B_CO2)  + (b_qh_c_7(1)  * B_CO7)  
    B_QHOCR_NC  = b_qh_cr_14(1)  * B_CO14

Result::

    B_QHOA := b_1_2*B_C0122222  
    B_QHOB := b_qh_b_14_1*B_CO14  
    B_QHOCADD := (b_qh_c_1*B_CO1)+(b_qh_c_2*B_CO2)+(b_qh_c_7*B_CO7)  
    B_QHOCR_NC := b_qh_cr_14*B_CO14


Data operations
~~~~~~~~~~~~~~~

The commands operating on data in the active workspaces (in memory) allow you to copy, edit, 
delete, rename, print, etc., the data of a workspace:

- :ref:`datacreate <datacreate>` : creates an object (cmt / eqs / idt / lst / scl / tbl / var)
- :ref:`datadelete <datadelete>` : deletes an object (cmt / eqs / idt / lst / scl / tbl / var)
- :ref:`dataexist <dataexist>` : tests for the existence of an object (cmt / eqs / idt / lst / scl / tbl / var)
- :ref:`dataedit <dataedit>` : edits an object (cmt / eqs / idt / lst / scl / tbl / var)
- :ref:`dataupdate <dataupdate>` : modifies an object (cmt / eqs / idt / lst / scl / tbl / var)
- :ref:`dataappend <dataappend>` : appends an object (cmt | lst)
- :ref:`dataduplicate <dataduplicate>` : duplicates an object (cmt / idt / lst / scl / tbl / var) (an equation cannot be duplicated)
- :ref:`datarename <datarename>` : renames an object (cmt / eqs / idt / lst / scl / tbl / var)
- :ref:`datasearch <datasearch>` : searches for an object cmt / eqs / idt / lst / scl / tbl / var
- :ref:`datascan <datascan>` : scans the objects eqs / idt / tbl
- :ref:`datalistXxx <datalist>` : creates a list of objects whose names match a given criterion cmt / eqs / idt / lst / scl / tbl / var
- :ref:`datalistsort <datalistsort>` : sorts a list in alphabetical order
- :ref:`datacompareEps <datacompareeps>` : sets the equality threshold for variable comparison
- :ref:`datacompareXxx <datacompare>` : compares the workspace and a file and creates lists
- :ref:`datacalclst <datacalclst>` : performs logical operations on lists
- :ref:`datacalcvar <datacalcvar>` : calculates a variable based on a LEC expression
- :ref:`datadisplaygraph <datadisplaygraph>` : displays a graph based on series (without a table)
- :ref:`datasavegraph <datasavegraph>` : saves a graph calculated from series
- :ref:`datawidthvar <datawidthvar>` : sets the column width for editing series
- :ref:`datandecvar <datandecvar>` : sets the number of decimals for editing series
- :ref:`datamodevar <datamodevar>` : sets the mode for editing series
- :ref:`datastartvar <datastartvar>` : sets the first period for editing series
- :ref:`datawidthtbl <datawidthtbl>` : sets the column width for editing tables
- :ref:`datawidthscl <datawidthscl>` : sets the column width for editing scalars
- :ref:`datandecscl <datandecscl>` : sets the number of decimals for editing scalars
- :ref:`dataeditcnf <dataeditcnf>` : changes the editing options for variables
- :ref:`datarasvar <datarasvar>` : RAS method for completing a matrix of series
- :ref:`datapatternXXX <datapattern>` : creates lists of names from a pattern


.. _datacreate:

COMMAND DATACREATEXXX
^^^^^^^^^^^^^^^^^^^^^

*Xxx* takes one of the following values::

    cmt / eqs / idt / lst / scl / tbl / var

This function allows the user to create objects in the current workspace.

If the object *name* already exists, the function returns and signals an error.
Otherwise, the object is created with a default value:

- comments, lists, identities, and tables: empty
- equation: "NAME := NAME"
- scalars: 0.9
- variables: a series with `NA` for the entire sample of the workspace
- tables: creates an empty table

Syntax::

    $DataCreateVar name

.. _datadelete:

COMMAND DATADELETEXXX
^^^^^^^^^^^^^^^^^^^^^

*Xxx* takes one of the following values::

    cmt / eqs / idt / lst / scl / tbl / var

This function allows the user to delete objects in the current workspace.
If the object *name* does not exist, the function returns and signals an error.

Syntax::

    $DataDeleteVar name

.. warning::

    Since version 5.13, wildcards are accepted in the name.

.. _dataexist:

COMMAND DATAEXISTXXX
^^^^^^^^^^^^^^^^^^^^

*Xxx* takes one of the following values::

    cmt / eqs / idt / lst / scl / tbl / var

This function checks for the existence of an object in the current workspace.
If the object *name* does not exist, the function returns and signals an error.

Syntax::

    $DataDeleteVar name

.. _dataedit:

COMMAND DATAEDITXXX
^^^^^^^^^^^^^^^^^^^

.. warning::

    OBSOLETE

*Xxx* takes one of the following values::

    cmt / eqs / idt / lst / scl / tbl / var

This function allows the user to edit objects in the current workspace.
A screen displays the selected objects in a scrollable table (or editing table) in which 
you can navigate.

The editing table consists of the following elements:

- the name of the workspace in the top row
- the names of the objects in the left column
- the definition (or part of the definition) of the objects next to their name on the right
- a scrollbar indicating the current position in the table and the visible proportion of 
  the table

The current object is indicated by its corresponding row being highlighted (inverse video).
The function keys of the scrollable tables allow you to display the value, modify, and create 
objects.

The list of objects to edit is passed as an argument to the command. The elements of the 
list are separated by spaces, commas, or semicolons.
If the list is empty (no argument), all objects appear in the editing table.

Syntax::

    #DataEditXxx [object [list of objects]]  
    {Xxx = cmt / eqs / idt / lst / scl / tbl / var}

.. _dataupdate:

COMMAND DATAUPDATEXXX
^^^^^^^^^^^^^^^^^^^^^

*Xxx* takes one of the following values::

    cmt / eqs / idt / lst / scl / tbl / var

The function changes the content of an object in the current workspace.

If the object *name* does not exist, an object containing the new definition is created.
If the object exists, its content is replaced. The function takes several arguments.
The arguments differ depending on the type of object to update.
The first argument is always the name of the object to change.

Comments
""""""""

Syntax::

    $DataUpdateCmt name comment

Equations
"""""""""

Syntax::

    $DataUpdateEqs name lec_equation

Identities
""""""""""

Syntax::

    $DataUpdateIdt name lec_identity    

Lists
"""""

Syntax::

    $DataUpdateLst name list

Scalars
"""""""

Syntax::

    $DataUpdateScl name value [relax]

Tables
""""""

Syntax::

    $DataUpdateTbl table_name title;lec1;lec2;...

If *title* is a comment name, the comment is used as the title.
Similarly, if variable names are found as LEC forms, and there is a comment for these 
variables, the title of the corresponding row is replaced by the value of the comment.
Suppose there is a comment for *A* in the workspace and not for *B*, and a comment *TIT*::

    Comment A   : "Gross National Product"
    Comment TIT : "Title of my table"

The line::

    $DataUpdateTbl T1 TIT;A;B;A+B

creates the table T1 with the following form::

    +-------------------------------------------+
    |          Title of my table                |
    |                                           |
    |  Gross National Product              A    |
    |  B                                   B    |
    |  A+B                                 A+B  |
    +-------------------------------------------+

Variables
"""""""""

Syntax::

    $DataUpdateVar name [L,l | D,d | G,g] period value1 value2 ...
    where L,l := in value (default)
       D, d := in difference
       G, g := in growth rate
       period := the period at the start of the update

.. _dataappend:

COMMAND DATAAPPENDXXX
^^^^^^^^^^^^^^^^^^^^^

*Xxx* takes one of the following values::

    cmt / eqs / idt / lst / scl / tbl / var

The function appends text to the definition of an object in the current workspace.

If the object *name* does not exist, an object containing the new definition is created.
If the object exists, its content is appended.

Comments
""""""""

Syntax::

    $DataAppendCmt name comment

Lists
"""""

Syntax::

    $DataAppendLst name list

.. _dataduplicate:

COMMAND DATADUPLICATEXXX
^^^^^^^^^^^^^^^^^^^^^^^^

*Xxx* takes one of the following values::

    cmt / idt / lst / scl / tbl / var

Objects defined in the current workspaces can be copied to objects of the same 
type but with a different name.

The `$DataDuplicateXxx` command takes two parameters in succession: the old and 
the new object name.

Syntax::

    $DataDuplicateCmt old_name new_name

Example::

    $DataDuplicateCmt A01 A02

The `$DataDuplicateEqs` command is not implemented and, in fact, has no meaning.
Indeed, an equation is named after its endogenous variable. 
Changing its name therefore changes the name of the endogenous variable, 
which makes no sense.

.. _datarename:

COMMAND DATARENAMEXXX
^^^^^^^^^^^^^^^^^^^^^

*Xxx* takes one of the following values::

    cmt / idt / lst / scl / tbl / var

This function changes the name of an object in the current workspace.

If the object *name* does not exist, the function returns and signals an error.
If the object exists, it will be saved under a new name.
If there is already an object with this new name, it is replaced.

Syntax::

    $DataRenameVar name new_name

The `$DataRenameEqs` command is not implemented and, in fact, has no meaning.
Indeed, an equation is named after its endogenous variable. 
Changing its name therefore changes the name of the endogenous variable, 
which makes no sense.

.. _datasearch:

COMMAND DATASEARCHXXX
^^^^^^^^^^^^^^^^^^^^^

*Xxx* takes one of the following values::

    cmt / eqs / idt / lst / scl / tbl / var

This function allows the user to search in one of the current workspaces for the list of objects
containing a given string.
The result of this search is a list of objects, which can, if needed, be saved in the 
current lists workspace.

The search differs depending on the type of object:

- Comments: the name and the text of the comment are analyzed
- Equations: the name and the LEC form of the equation are analyzed
- Identities: the name and the LEC form of the identity are analyzed
- Lists: the name and the text of the list are analyzed
- Scalars: the name of the scalar is analyzed
- Tables: the name, titles, and LEC forms of the table are analyzed
- Variables: the name of the variable is analyzed

Syntax::

    $DataSearchXxx mask word ecase in_name in_formula in_text list_result  

    (word, ecase, in_name, in_formula, in_text := 0 or 1)  
    (mask := characters and possibly ?, *, ...)  
    (list_result := the name of the result list)

Seven parameters are passed to the command:

- ``mask``: The search text can contain special characters that allow you to specify the search limits:
    - ``*`` : any sequence of characters (including empty)
    - ``?`` : exactly one character (any)
    - ``@`` : any alphanumeric character
    - ``&`` : any non-alphanumeric character
    - ``|`` : any alphanumeric character or none at the start and end of the string
    - ``!`` : any non-alphanumeric character or none at the start and end of the string
    - ``\`` : placed before one of the special characters disables its interpretation
- ``word``: specify *Yes* if the string to search for must be a whole word and not part of a word. Specify *No* if this does not matter.
- ``ecase``: specify if the search should be case-sensitive.
- ``in_name``: specify *Yes* if the search should be done in the object name, *No* otherwise.
- ``in_formula``: specify *Yes* if the search should be done in the object's LEC formula(s), *No* otherwise.
- ``in_text``: specify *Yes* if the search should be done in the object's text, *No* otherwise.
- ``list_result``: it is possible to save the list of objects matching the criteria in the current IODE lists workspace.
  The name of this list must be indicated and must be a valid list name.
  If a list with the same name exists, it will be replaced by the result.

Example::

    $datasearchCmt TE?T 0 0 1 0 1 NEW2  
    #DataEditCMT $NEW2

.. _datascan:

COMMAND DATASCANXXX
^^^^^^^^^^^^^^^^^^^

*Xxx* takes one of the following values::    

    eqs / idt / tbl

This function allows the user to search in one of the current workspaces for the list of 
variables and scalars used in the definition of the objects in this workspace.
The result of this search is saved in two lists, containing on the one hand the names of 
the scalars (list ``_SCAL``) and on the other hand those of the variables (list ``_EXO``).
If no argument is passed to this function, the entire workspace is examined.

Syntax:: 

    $DataScanEqs [name1,name2,...]  
    $DataScanIdt [name1,name2,...]  
    $DataScanTbl [name1,name2,...]

.. _datalist:

COMMAND DATALISTXXX
^^^^^^^^^^^^^^^^^^^

This function builds a list of object names based on a selection criterion (pattern) 
applied to the names. It can operate on objects in a workspace or on objects in a file. 
For example, it can be used to obtain the list of objects whose name starts with *A* and 
ends with *BEL* (`A*BEL`).

The command takes two or three parameters. The first specifies the name of the list to create. 
The second is the pattern. The third (optional) is the name of the file.

Syntax::

    $DataListXxx listname pattern [filename]

Example::

    $DataListVar _BEL_ *BEL*

generates the IODE list `_BEL_` of all variables whose name contains the text *BEL*: 
AX1BEL, BEL1, MIRABELLE, etc.

.. _datalistsort:

COMMAND DATALISTSORT
^^^^^^^^^^^^^^^^^^^^

This function performs an alphanumeric sort on the contents of a list.

The command takes two parameters. The first specifies the name of the list to sort.
The second specifies the name of the resulting sorted list.

Syntax::

    $DataListSort list sorted_list

.. _datacompareeps:

COMMAND DATACOMPAREEPS
^^^^^^^^^^^^^^^^^^^^^^

This function sets the threshold below which the variable equality test is 
considered satisfied.

The comparison test is::   

    if x1 <> 0 :  |(x1 - x2) / x1| < eps  
    else :        |x2| < eps

Syntax::

    $DataCompareEps eps

The comparison threshold is set to 1e-7 by default.

.. _datacompare:

COMMAND DATACOMPAREXXX
^^^^^^^^^^^^^^^^^^^^^^

The contents of the current workspace can be compared to those of a file.
The result of this comparison consists of 4 lists:

- objects found only in the workspace
- objects found only in the file
- objects found in both, with the same definition
- objects found in both, with a different definition

The function takes 5 parameters: the file to compare and the 4 lists to build.

Syntax::

    $DataCompareXxx filename ONE TWO THREE FOR  

    ONE    in workspace only  
    TWO    in file only  
    THREE  in both, equal  
    FOR    in both, different

Example::

    $DataCompareVar myws.var WS FILE EQ DIFF

.. _datacalclst:

COMMAND DATACALCLST
^^^^^^^^^^^^^^^^^^^

This function calculates a list based on two lists by taking their union, 
intersection, or difference.

The command takes 4 parameters:

- the name of the result list
- the name of the first list
- the operation
- the name of the second list

Syntax::

    $DataCalcLst res lst1 op lst2  
    where op = + : union  
            * : intersection  
            - : difference

Example::

    $DataCalcLst _RES LST1 * LST2

generates the IODE list `_RES` of names present in both *LST1* and *LST2*.

.. _datacalcvar:

COMMAND DATACALCVAR
^^^^^^^^^^^^^^^^^^^

This function allows the user to calculate a new series from a LEC formula.
This formula is executed over the entire sample of the IODE variables workspace 
using the values of the other variables.

This function takes 2 parameters:

- the name of the variable to create
- the LEC formula to calculate

Syntax::

    $DataCalcVar name formula

Example::

    $DataCalcVar X  beta + gamma * ln B + alpha * ln C

.. _datadisplaygraph:

COMMAND DATADISPLAYGRAPH
^^^^^^^^^^^^^^^^^^^^^^^^

This function allows the user to display series as a graph.

Syntax::

    $DataDisplayGraph mode chart_type gridX gridY scaling  
                      {ymin | --} {ymax | --} perfrom perto 
                      varname1 varname2 ...  
 
where:

- ``mode``: specifies whether the variables should be displayed as values, differences, 
  or growth rates:
    - ``L`` = level  
    - ``D`` = differences  
    - ``G`` = growth rates  
    - ``d`` = YoY differences: year-over-year differences  
    - ``g`` = YoY growth rates: year-over-year growth rates  
- ``chart_type``: type of chart to produce: ``line``, ``scatter``, ``scatter line``, 
  or ``bar``.
- ``gridX/Y``: type of grid for *X* and *Y*: ``No[X|Y]Grids``, ``Minor[X|Y]Grids``, 
  or ``J(Major[X|Y]Grids)``.
- ``scaling``: type of coordinate transformation to use: ``Level``, ``G(Log)``, ``Semi-Log``, 
  or ``Percent``.
- ``ymin/ymax``: minimum and maximum values for the *Y* axis. If these values are null 
  (``--``), the axis is calculated automatically based on the values to display.
- ``perfrom perto``: print sample (from period *from* to period *to*).
- ``varname1 varname2 ...``: list of variables to include in the graph. 
  The names (or list) can be separated by a space, a ``+``, or a combination of both. 
  Variables separated by a space will be displayed in separate graphs in the same window, 
  while *added* variables will be grouped in the same graph. Thus, if this field contains 
  `X Y+Z T`, three graphs will be constructed with variables *X* (first graph), *Y* and *Z* 
  (grouped in the second graph), and *T* (last graph).

.. _datasavegraph:

COMMAND DATASAVEGRAPH
^^^^^^^^^^^^^^^^^^^^^

This function allows the user to build graphs from series and save the result in an AGL file.

Syntax::

    $DataSaveGraph  aglfilename mode chart_type gridX gridY scaling  
                    {ymin | --} {ymax | --} perfrom perto 
                    varname1 varname2 ...

where:

- ``aglfilename``: name of the AGL file in which the graph will be saved.
- ``mode``: specifies whether the variables should be displayed as values, differences, 
  or growth rates:
    - ``L`` = level  
    - ``D`` = differences  
    - ``G`` = growth rates  
    - ``d`` = YoY differences: year-over-year differences  
    - ``g`` = YoY growth rates: year-over-year growth rates  
- ``chart_type``: type of chart to produce: ``line``, ``scatter``, ``scatter line``, 
  or ``bar``.
- ``gridX/Y``: type of grid for *X* and *Y*: ``No[X|Y]Grids``, ``Minor[X|Y]Grids``, 
  or ``J(Major[X|Y]Grids)``.
- ``scaling``: type of coordinate transformation to use: ``Level``, ``G(Log)``, ``Semi-Log``, 
  or ``Percent``.
- ``ymin/ymax``: minimum and maximum values for the *Y* axis. If these values are null 
  (``--``), the axis is calculated automatically based on the values to display.
- ``perfrom perto``: print sample (from period *from* to period *to*).
- ``varname1 varname2 ...``: list of variables to include in the graph. 
  The names (or list) can be separated by a space, a ``+``, or a combination of both. 
  Variables separated by a space will be displayed in separate graphs in the same window, 
  while *added* variables will be grouped in the same graph. Thus, if this field contains 
  `X Y+Z T`, three graphs will be constructed with variables *X* (first graph), *Y* and *Z* 
  (grouped in the second graph), and *T* (last graph).
 
.. _datawidthvar:

COMMAND DATAWIDTHVAR
^^^^^^^^^^^^^^^^^^^^

.. warning::

    OBSOLETE

This function allows the user to specify the column width for editing statistical series.
The value must be between 2 and 12.

Syntax::

    $DataWidthVar n  
    n between 2 and 12

.. _datandecvar:

COMMAND DATANDECVAR
^^^^^^^^^^^^^^^^^^^

This function allows the user to specify the number of decimals when editing statistical series. 
The value must be between -1 (variable number of decimals) and 6.

Syntax::

    $DataNdecVar n  
    n between -1 and 6

.. _datamodevar:

COMMAND DATAMODEVAR
^^^^^^^^^^^^^^^^^^^

This function allows the user to choose the editing mode for statistical series: Level, 
differences, or growth rates.

Syntax::

    $DataModeVar n  
    where n = 0 for level  
          n = 1 for differences  
          n = 2 for growth rates

.. _datastartvar:

COMMAND DATASTARTVAR
^^^^^^^^^^^^^^^^^^^^

.. warning::

    OBSOLETE

This function allows the user to choose the first visible column when editing statistical series. 
The value must be between 0 (first column) and the number of available periods.

Syntax::

    $DataStartVar n  
    where n is between 0 and the number of periods

.. _datawidthtbl:

COMMAND DATAWIDTHTBL
^^^^^^^^^^^^^^^^^^^^

.. warning::

    OBSOLETE

This function allows the user to specify the column width for editing IODE tables.
The value must be between 2 and 60.

Syntax::

    $DataWidthTbl n
    n between 2 and 60

COMMAND DATAWIDTHScalar
^^^^^^^^^^^^^^^^^^^^

This function allows the user to specify the number of decimals when editing scalars.
The value must be between -1 (variable number of decimals) and 6.

Syntax::

    $DataNdecScl n  
    where n is between -1 and 6

.. _dataeditcnf:

COMMAND DATAEDITCNF
^^^^^^^^^^^^^^^^^^^

This function allows the user to change the display mode of the series table to ``Level`` 
(actual value), ``Difference``, or ``Growth Rate``. 
It also allows the user to choose the number of decimals to display.

This function takes two parameters:

- the display mode: ``{Level | Differences | GrowthRates}``
- the number of decimals to display (-1, 0, 1, 2, ...)

Syntax::

    $DataEditCnf {L | D | G} n  
    (n := -1, 0, 1, 2, 3, ...)

.. _datarasvar:

COMMAND DATARASVAR
^^^^^^^^^^^^^^^^^^

Starting from the values of variables matching the pattern, the values are distributed 
using an RAS method.

Syntax::

    $DataRasVar pattern X_dimension Y_dimension ref_year sum_year [maxit [eps]]

where:

- ``pattern``: the variables used are those that meet the following criteria: *x* 
  is replaced by all values from *$X*, *y* by those from *$Y*
- ``X_dimension``: list of values that *x* in the pattern can take. 
  NOTE: the last in the list is the SUM over the x dimension
- ``Y_dimension``: list of values that *y* in the pattern can take. 
  NOTE: the last in the list is the SUM over the y dimension
- ``ref_year``: reference year: the year for which all data is known
- ``sum_year``: the year for which only the sums are known
- ``maxit``: maximum iterations (default=100)
- ``eps``: the threshold (default=0.001)

Example::

    $WsLoadVar ras.av  
    $DataUpdateLst X R1;R2;R3;R4;R5;R6;RT  
    $DataUpdateLst Y C1;C2;C3;C4;C5;CT  
    $DataRasVar xy $X $Y 1980Y1 1981Y1 10 0.00001

The RAS matrix then looks as follows::

    R1C1 R1C2 R1C3 R1C4 R1C5 R1C6 | R1CT  
    R2C1 R2C2 R2C3 R2C4 R2C5 R2C6 | R2CT  
    R3C1 R3C2 R3C3 R3C4 R3C5 R3C6 | R3CT  
    R4C1 R4C2 R4C3 R4C4 R4C5 R4C6 | R4CT  
    R5C1 R5C2 R5C3 R5C4 R5C5 R5C6 | R5CT  
    -------------------------------  
    RTC1 RTC2 RTC3 RTC4 RTC5 RTC6 | RTCT

with the values for the year ``ref_year``. The new row and column sums have the value 
from ``sum_year``. If values are known in ``sum_year`` for some cells, those are used. 
RAS then calculates the cells so that the new boundary conditions are met and overwrites 
the unknown values with `NA` (Not Available).

.. _datapattern:

COMMAND DATAPATTERNXXX
^^^^^^^^^^^^^^^^^^^^^^

This function creates lists containing the names of objects that match a given pattern.

Syntax::

    $DataPatternXXX list pattern X_dimension [Y_dimension]

where:

- ``list``: the name of the result list.
- ``pattern``: pattern that the object names must match, where *x* is replaced by the 
  elements from *X_dimension* and, if specified, *y* by the elements from *Y_dimension*.
- ``X_dimension``: list of possible *x* values.
- ``Y_dimension`` (optional): list of possible *y* values.

.. warning::

    Only existing elements are included in the list.

Example::

    $WsLoadVar ras.av  
    $DataUpdateLst X R1;R2;R3  
    $DataUpdateLst Y C1;C2  
    $DataPatternVar RC xy $X $Y  

Result::

    $RC=R1C1,R1C2,R2C1,R2C2,R3C1,R3C2  

insofar as R1C1, R1C2, R2C1, R2C2, R3C1, R3C2 are variables in the variables workspace.


Specific operations on equations
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The following commands allow you to modify estimation parameters and estimate equations:

- :ref:`EqsEstimate <eqsestimate>`
- :ref:`EqsStepWise <eqsstepwise>`
- :ref:`EqsSetCmt <eqssetcmt>`
- :ref:`EqsSetSample <eqssetsample>`
- :ref:`EqsSetMethod <eqssetmethod>`
- :ref:`EqsSetInstrs <eqssetinstrs>`
- :ref:`EqsSetBloc <eqssetbloc>`


.. _eqsestimate:

COMMAND EQSESTIMATE
^^^^^^^^^^^^^^^^^^^

This function estimates an equation or a block of equations.
Only the period can be adjusted in this function.
Other parameters (method, instruments, etc.) are those currently defined in the equation.
Other functions allow you to modify the instruments, the method: these are the *EqsSet...* functions.

Syntax::

    $EqsEstimate perfrom perto eqname1 ...

At the end of estimation, certain variables and scalars are automatically created if the process has converged. These variables and scalars can be used for calculations and, as they are objects in the workspace, can be saved for future use.

The tests resulting from the last estimation are saved in scalars.
These scalars have the following names (``e0_*`` for the first equation in the block, ``e1_*`` for the second, ...):

- ``e0_n``: number of periods in the sample
- ``e0_k``: number of estimated coefficients
- ``e0_stdev``: standard deviation of residuals
- ``e0_meany``: mean of Y
- ``e0_ssres``: sum of squared residuals
- ``e0_stderr``: standard error
- ``e0_stderrp``: standard error %
- ``e0_fstat``: F-Stat
- ``e0_r2``: R squared
- ``e0_r2adj``: adjusted R squared
- ``e0_dw``: Durbin-Watson
- ``e0_loglik``: Log Likelihood

The calculated series are also saved:

- ``_YCALC0`` for the right-hand side of the first equation in the block,
  ``_YCALC1`` for the second equation, etc.
- ``_YOBS0`` for the left-hand side of the first equation in the block,
  ``_YOBS1`` for the second equation, etc.
- ``_YRES0`` for the residuals of the first equation in the block, ...

Outside the estimation sample, the values of the series are `NA` (Not Available).

.. _eqsstepwise:

COMMAND EQSSTEPWISE
^^^^^^^^^^^^^^^^^^^

This function estimates a block of equations and searches for the best possible tests 
for all possible combinations of coefficients.

Syntax::

    $EqsStepWise from to eqname leccond {r2|fstat}  
    where from to : estimation period  
          eqname  : equation to estimate  
          leccond : acceptance condition

.. _eqssetcmt:

COMMAND EQSSETCMT
^^^^^^^^^^^^^^^^^

This function sets the comment for an equation.

Syntax::

    $EqsSetCmt eqname comment  
    where eqname is the name of the equation  
          comment is free text

Example::

    $DataUpdateEqs E E := c1 + c2 * F  
    $EqsSetCmt E This is the comment for E  
    $EqsSetSample 1971Y1 1990Y1 E  
    $EqsSetMethod 1 B E  
    $EqsSetInstrs E instruments  
    $EqsSetBloc B E

.. _eqssetsample:

COMMAND EQSSETSample
^^^^^^^^^^^^^^^^^^^^

This function sets the estimation period for a list of equations.
It is useful for changing the estimation period during the execution of a report.

The function modifies the period for all equations whose names are passed as parameters.

Syntax::

    $EqsSetSample from to eqname1 eqname2 ...
    where from and to are the sample period limits
          eqname1 .. are the names of existing equations

Example::

    $DataUpdateEqs E E := c1 + c2 * F
    $EqsSetCmt E This is the comment for E
    $EqsSetSample 1971Y1 1990Y1 E
    $EqsSetMethod 1 B E
    $EqsSetInstrs E instruments
    $EqsSetBloc B E

.. _eqssetmethod:

COMMAND EQSSETMETHOD
^^^^^^^^^^^^^^^^^^^^

This function sets the estimation method for a list of equations.
It is useful for changing the estimation method during the execution of a report.

The function modifies the method for all equations whose names are passed as parameters.

Syntax::

    $EqsSetMethod   {0|1|2|3} eqname1 eqname2 ...  

    where 0 indicates the LSQ method (least squares)  
          1 indicates the Zellner method  
          2 indicates the INF method (2-stage with instruments)  
          3 indicates the GLS method (3-stage with instruments)  
          4 indicates the MAXLIK method (Maximum likelihood - BHHH - from version 6.21)  

          eqname1 .. are the names of existing equations

Example::

    $DataUpdateEqs E E := c1 + c2 * F  
    $EqsSetCmt E This is the comment for E  
    $EqsSetSample 1971Y1 1990Y1 E  
    $EqsSetMethod 1 B E  
    $EqsSetInstrs E instruments  
    $EqsSetBloc B E

.. _eqssetinstrs:

COMMAND EQSSETINSTRS
^^^^^^^^^^^^^^^^^^^^

This function sets the instruments to be used for estimating an equation.

The function modifies the instruments for the specified equation.

Syntax::

    $EqsSetInstrs eqname lec_instr_1;lec_instr_2;...

    where eqname is the name of the equation  
          lec_instr_1, ... are the LEC forms of the instruments

Example::

    $DataUpdateEqs E E := c1 + c2 * F  
    $EqsSetCmt E This is the comment for E  
    $EqsSetSample 1971Y1 1990Y1 E  
    $EqsSetMethod 1 B E  
    $EqsSetInstrs E instruments  
    $EqsSetBloc B E

.. _eqssetbloc:

COMMAND EQSSETBLOC
^^^^^^^^^^^^^^^^^^

This function sets the estimation block for a list of equations.
It is useful for setting the estimation block during the execution of a report.

The function sets the block for all equations whose names are passed as parameters.
The block is the list of equations itself.

Syntax::

    $EqsSetBloc eqname1 eqname2 ...

    where eqname1 ... are the names of existing equations

Example::

    $DataUpdateEqs E E := c1 + c2 * F  
    $EqsSetCmt E This is the comment for E  
    $EqsSetSample 1971Y1 1990Y1 E  
    $EqsSetMethod 1 B E  
    $EqsSetInstrs E instruments  
    $EqsSetBloc B E


Printer configuration
~~~~~~~~~~~~~~~~~~~~~

The following commands are used to configure the printer or the output file for print jobs:

- :ref:`printdest <printdest>` : sets the print destination
- :ref:`printdestnew <printdestnew>` : sets the print destination and resets the print file
- :ref:`printnbdec <printnbdec>` : sets the number of decimals for printing
- :ref:`printlang <printlang>` : sets the default language for printing
- :ref:`PrintA2mAppend <printa2mappend>` : prevents the a2m file from being cleared before printing
- :ref:`PrintFont <printfont>` : sets the font for printing
- :ref:`PrintTableFont <printtablefont>` : sets the font for tables
- :ref:`PrintTableBox <printtablebox>` : sets the border width for tables
- :ref:`PrintTableColor <printtablecolor>` : enables or disables color in tables
- :ref:`PrintTableWidth <printtablewidth>` : sets the width of tables in Frame and Rtf
- :ref:`PrintTableBreak <printtablebreak>` : enables or disables table breaks across multiple pages
- :ref:`PrintTablePage <printtablepage>` : forces a page break before each table
- :ref:`PrintBackground <printbackground>` : sets the background color (tables and charts)
- :ref:`PrintGraphBox <printgraphbox>` : sets the border width for charts
- :ref:`PrintGraphBrush <printgraphbrush>` : sets the background density for charts
- :ref:`PrintGraphSize <printgraphsize>` : sets the size of charts
- :ref:`PrintGraphPage <printgraphpage>` : forces a page break before each chart
- :ref:`PrintRtfHelp <printrtfhelp>` : generates an RTF file for Windows help
- :ref:`PrintRtfTopic <printrtftopic>` : creates a new topic (Windows Help)
- :ref:`PrintRtfLevel <printrtflevel>` : changes the hierarchical level of the following topics
- :ref:`PrintRtfTitle <printrtftitle>` : sets the title for Windows help
- :ref:`PrintRtfCopyright <printrtfcopyright>` : sets the copyright text for Windows help
- :ref:`PrintHtmlHelp <printhtmlhelp>` : generates an HTML file for HtmlHelp
- :ref:`PrintHtmlStrip <printhtmlstrip>` : when generating an HTML file (A2mToHtml), does not generate a header or footer
- :ref:`PrintParanum <printparanum>` : enables numbering of headings
- :ref:`PrintPageHeader <printpageheader>` : sets the page header for printed pages
- :ref:`PrintPageFooter <printpagefooter>` : sets the page footer for printed pages
- :ref:`SetPrinter <setprinter>` : sets the default printer
- :ref:`PrintOrientation <printorientation>` : sets the paper orientation in the printer
- :ref:`PrintDuplex <printduplex>` : sets the duplex (double-sided) mode of the printer
- :ref:`PrintGIFBackColor <printgifbackcolor>` : sets the background color for charts
- :ref:`PrintGIFTransColor <printgiftranscolor>` : sets the color considered as "transparent"
- :ref:`PrintGIFTransparent <printgiftransparent>` : indicates if the GIF file should be transparent
- :ref:`PrintGIFInterlaced <printgifinterlaced>` : indicates if the GIF file should be interlaced
- :ref:`PrintGIFFilled <printgiffilled>` : indicates whether to fill bars in bar charts
- :ref:`PrintGIFFont <printgiffont>` : specifies the font number to use


.. _printdest:

COMMAND PRINTDEST
^^^^^^^^^^^^^^^^^

This function allows the user to specify whether to print to a file and, if so, the name and 
type of the output file. This setting applies to all print jobs initiated from an IODE 
menu or report, until the end of the current report execution or until another report command 
specifies a different destination, in which case the print configuration is updated for all 
subsequent print jobs. The destination file is always reset, even for a2m files. 

This is new as of version 5 of IODE.

The name of the destination file is passed as a parameter to the command:

Syntax::

    $PrintDest [filename] [format]  
    where format = A2M, MIF, HTML, RTF, or CSV

By default, A2M is the chosen format. 
If ``filename`` is not specified, printing will be sent to a printer.

Example::

    $PrintDest test.mif MIF

.. _printdestnew:

COMMAND PRINTDESTNEW
^^^^^^^^^^^^^^^^^^^^

This function is identical to :ref:`$PrintDest <printdest>`, except that it deletes 
the output file.

The name of the destination file is passed as a parameter to the command:

Syntax::

    $PrintDestNew [filename] [format]  
    where format = A2M, MIF, HTML, RTF, or CSV  
    
By default, A2M is the chosen format. 
If ``filename`` is not specified, printing will be sent to a printer.

.. _printnbdec:

COMMAND PRINTNBDEC
^^^^^^^^^^^^^^^^^^

This function allows the user to specify the number of decimals to print when printing tables 
or variables.

The number of decimals is passed as a parameter to the command:
Syntax::

    $PrintNbDec  nb

Example::

    $wsloadvar bist92\bistel  
    $wsloadtbl bist92\tbistelf  
    
    $printdest bist92\bistelf1.a2m  
    #show processing french tables  file 1/2  
    $printnbdec 1  
    $PrintTbl 89:8 HYPEIR  
    $PrintTbl 89/88:8 HYPEIIR

.. _printlang:

COMMAND PRINTLANG
^^^^^^^^^^^^^^^^^

This function allows the user to specify the print language when printing tables. 
The default language is English.

The print language is passed as a parameter to the command:

Syntax::

    $PrintLang {English | French | Dutch}

Example::

    $wsloadvar bist92\bistel  
    $wsloadtbl bist92\tbistelf  
    
    $printdest bist92\bistelf1.a2m  
    #show processing dutch tables  file 1/2  
    $printnbdec 1  
    $printlang Dutch  
    $PrintTbl 89:8 HYPEIR  
    $PrintTbl 89/88:8 HYPEIIR

.. _printa2mappend:

COMMAND PRINTA2MAPPEND
^^^^^^^^^^^^^^^^^^^^^^

Allows the user not to clear the a2m file before printing. 
By default, the result files of print jobs are cleared before a new print. 
Only for a2m files, it is possible to avoid this default behavior and thus accumulate 
several successive print jobs in a single file.

Syntax::

    $PrintA2mAppend [NO|Yes]

Example::

    $PrintA2mAppend Yes  
    $PrintDest test.a2m A2M

These two calls define the file test.a2m as the result file for subsequent print jobs 
and do not clear this file.

.. _printfont:

COMMAND PRINTFONT
^^^^^^^^^^^^^^^^^

Sets the font for printing text paragraphs. In addition to the family, the base size 
(in points) and the size increment can also be specified.
The base size corresponds to text paragraphs, the increment indicates by how many points 
each title level should be increased.

.. warning::

    This value only has an effect before the $PrintDest function and not during the report.

Syntax::

   $PrintFont Times|Helvetica|Courier|Bookman|Palatino [size [incr]]  
 
The Bookman and Palatino fonts are not available for all print destinations.
The first character of the font name is sufficient.
Once this value is set, it remains in effect for all subsequent print jobs, including 
in subsequent reports.

Example::

   $PrintFont Times 10 2

Sets the size of paragraphs par_1, enum_1, etc. to 10 points, paragraph tit_2 to 12 points, 
tit_1 to 14 points, and tit_0 to 18 points.

.. _printtablefont:

COMMAND PRINTTABLEFONT
^^^^^^^^^^^^^^^^^^^^^^

Sets the font for tables only. This font can therefore be different from that of the rest 
of the text, both in size and family.

.. warning::

    This function only has an effect before the $PrintDest function and not during the report.

Syntax::

   $PrintTableFont Times|Helvetica|Courier|Bookman|Palatino [size]

Once this value is set, it remains in effect for all subsequent print jobs, including 
in subsequent reports.

Example::

   $PrintTableFont Palatino 8

Sets the font for all tables to Palatino, 8 points.

.. _printtablebox:

COMMAND PRINTTABLEBOX
^^^^^^^^^^^^^^^^^^^^^

Sets the border width of tables in points. This value can be zero to avoid borders.

Syntax::

   $PrintTableBox  n

Once this value is set, it remains in effect for all subsequent print jobs, including in 
subsequent reports.

Example::

   $PrintTableBox  0

Removes the border from tables.

.. _printtablecolor:

COMMAND PRINTTABLECOLOR
^^^^^^^^^^^^^^^^^^^^^^^

Allows the user to use or not use color in tables.

Syntax::

   $PrintTableColor [NO|Yes]

Once this value is set, it remains in effect for all subsequent print jobs, including 
in subsequent reports.

Example::

   $PrintTableColor No

Removes color from table printing.

.. _printtablewidth:

COMMAND PRINTTABLEWIDTH
^^^^^^^^^^^^^^^^^^^^^^^

Sets the width of tables in Frame and Rtf. The parameters must be expressed in mm. 
Three values can be passed:

-  the total width
-  the width of the first column
-  the width of the following columns

Syntax::

    $PrintTableWidth width [col1 [coln]]

Once this value is set, it remains in effect for all subsequent print jobs, including 
in subsequent reports. 

Example::

    $PrintTableWidth 160 60 15

.. _printtablebreak:

COMMAND PRINTTABLEBREAK
^^^^^^^^^^^^^^^^^^^^^^^

Allows or disallows tables to be split across multiple pages: if a table is placed on the page 
so that it must be printed on two pages, an automatic page break is generated before the table.

Syntax::

    $PrintTableBreak [NO|Yes]

Once this value is set, it remains in effect for all subsequent print jobs, including 
in subsequent reports.

Example::

    $PrintTableBreak Yes

Table splitting is allowed during printing.

.. _printtablepage:

COMMAND PRINTTABLEPAGE
^^^^^^^^^^^^^^^^^^^^^^

Forces a page break before each table.

Syntax::

    $PrintTablePage  [NO|Yes]

Once this value is set, it remains in effect for all subsequent print jobs, 
including in subsequent reports.

Example::

    $PrintTablePage Yes

Forces a page break for each table.

.. _printbackground:

COMMAND PRINTBACKGROUND
^^^^^^^^^^^^^^^^^^^^^^^

Sets the background color (tables and charts). A table will be hatched if the value of 
the "Shadow" graphic attribute is set to Yes.
To avoid backgrounds in all tables, you can use White as the background.

Syntax::

    $PrintBackground Black|Blue|Magenta|Cyan|Red|Green|Yellow|White

Once this value is set, it remains in effect for all subsequent print jobs, 
including in subsequent reports.

Example::

   $PrintBackground Blue

The color of the hatched background in charts and tables is set to blue.

.. _printgraphbox:

COMMAND PRINTGRAPHBOX
^^^^^^^^^^^^^^^^^^^^^

Sets the border width of charts in points.

Syntax::

   $PrintGraphBox n

Once this value is set, it remains in effect for all subsequent print jobs, 
including in subsequent reports.

Example::

   $PrintGraphBox 2

Subsequent charts will have a thick border.

.. _printgraphbrush:

COMMAND PRINTGRAPHBRUSH
^^^^^^^^^^^^^^^^^^^^^^^

Sets the background density of charts. The value is expressed as a percentage.

Syntax::

   $PrintGraphBrush pct|Yes

Once this value is set, it remains in effect for all subsequent print jobs, 
including in subsequent reports.

Example::

   $PrintGraphBrush 10

The density of the hatch "brush" is set to 10%. The color is determined by the $PrintBackground function.

.. _printgraphsize:

COMMAND PRINTGRAPHSIZE
^^^^^^^^^^^^^^^^^^^^^^

Sets the size of charts in mm and the font size used.

Syntax::
   
   $PrintGraphSize  width [height] [fontsize]

where *width* and *height* are expressed in mm and *fontsize* in points.

Once this value is set, it remains in effect for all subsequent print jobs, 
including in subsequent reports.

Example::

   $PrintGraphSize 150 60 8

Subsequent charts will occupy a frame of 15 cm by 6 cm and the text will be 8 points in size.

.. _printgraphpage:

COMMAND PRINTGRAPHPAGE
^^^^^^^^^^^^^^^^^^^^^^

Forces a page break before each chart.

Syntax::

   $PrintGraphPage [NO|Yes]

Once this value is set, it remains in effect for all subsequent print jobs, 
including in subsequent reports.

Example::

   $PrintGraphPage Yes

Each subsequent chart will be placed at the beginning of a page.

.. _printrtfhelp:

COMMAND PRINTRTFHELP
^^^^^^^^^^^^^^^^^^^^

Generates an RTF file for Windows help. This file can then be compiled using Microsoft's 
HCW compiler. The RTF files generated in this way are not identical to those generated 
for WinWord.

Syntax::

    $PrintRtfHelp [YES|No]

Once this value is set, it remains in effect for all subsequent print jobs, 
including in subsequent reports.

Example::
    $PrintRtfHelp YES

The next file defined by $PrintDest ... RTF will be of the Windows Help type.

.. _printrtftopic:

COMMAND PRINTRTFTOPIC
^^^^^^^^^^^^^^^^^^^^^

Creates a new topic for a Windows help file (or a2m). 
This topic will be part of the automatic table of contents generated by IODE. 
The hierarchical level of the topic is set by the $PrintRtfLevel command.
The topic text will also be used as the title.

Syntax::

    $PrintRtfTopic topic title

Example::

    $PrintRtfTopic Summary Tables

Generates in a2m (see a2m language syntax)::

     .topic [auto_number] [level] Summary Tables  
     .par1 tit_[level]  
     Summary Tables  

If the generated file is an a2m file, the .topic command is only used in the case 
of a translation to Windows Help. However, `.par1 tit_....` and the topic text will 
generate a title when printed.

.. _printrtflevel:

COMMAND PRINTRTFLEVEL
^^^^^^^^^^^^^^^^^^^^^

Changes the hierarchical level of the following topics. 
This level change allows indentations to be generated in the resulting help file's table 
of contents.

Syntax::

    $PrintRtfLevel [+|-|n]

Example::

    $PrintRtfLevel +

The level of the next topic will be incremented by 1, thus creating an indentation 
in the generated table of contents.

.. _printrtftitle:

COMMAND PRINTRTFTITLE
^^^^^^^^^^^^^^^^^^^^^

Sets the title of the Windows help file.

Syntax::

    $PrintRtfTitle help title

Example::

    $PrintRtfTitle Hermes Model

.. _printrtfcopyright:

COMMAND PRINTRTFCOPYRIGHT
^^^^^^^^^^^^^^^^^^^^^^^^^

Sets the copyright text for Windows help. 
This text appears in the Help option of the Winhelp program.

Syntax::

    $PrintRtfCopyright copyright text

Example::

    $PrintRtfCopyright (c) Federal Planning Bureau - 1998

.. _printhtmlhelp:

COMMAND PRINTHTMLHELP
^^^^^^^^^^^^^^^^^^^^^

Specifies that the next generation of Html file should be for Windows HtmlHelp. 
This file can then be compiled using Microsoft's HHC compiler.

Syntax::

    $PrintHtmlHelp [YES|No]

Example::

    $PrintHtmlHelp YES  
    $A2mToHtml myfile.a2m myfile.htm

.. _printhtmlstrip:

COMMAND PRINTHTMLSTRIP
^^^^^^^^^^^^^^^^^^^^^^

Specifies that the next generation of Html file should not include the HTML header and 
footer sections (<HEAD>, <BODY>, </BODY>, etc). Only the actual text is translated for 
later manual inclusion in an HTML file.

Syntax::

    $PrintHtmlStrip [YES|No]

Example::

    $PrintHtmlStrip YES  
    $PrintDest test.htm HTML  
    $PrintTbl ....

.. _printparanum:

COMMAND PRINTPARANUM
^^^^^^^^^^^^^^^^^^^^

Allows numbering of headings when printing (1., 1.1, ...).

.. warning::

    This function only has an effect before the $PrintDest function and not during the report.

Syntax::

    $PrintParanum NO  
    $PrintParanum [NO|Yes]

Example::

    $PrintParanum NO

Removes automatic numbering of headings.

.. _printpageheader:

COMMAND PRINTPAGEHEADER
^^^^^^^^^^^^^^^^^^^^^^^

Sets the title of printed pages. The characters %d are replaced by the current page number.

Syntax::

    $PrintPageHeader title for the following pages

Example::

    $PrintPageHeader Modtrim Model

.. _printpagefooter:

COMMAND PRINTPAGEFOOTER
^^^^^^^^^^^^^^^^^^^^^^^

Sets the footnote of printed pages. 
The characters %d are replaced by the current page number.

Syntax::

    $PrintPageFooter  footnote for the following pages

Example::

    $PrintPageFooter - Page %d -

.. _setprinter:

COMMAND SETPRINTER
^^^^^^^^^^^^^^^^^^

Sets the default Windows printer for subsequent print jobs.

Syntax::

    $SetPrinter printer_name

Example::

    $SetPrinter 6_qms

.. _printorientation:

COMMAND PRINTORIENTATION
^^^^^^^^^^^^^^^^^^^^^^^^

Sets the orientation of the current Windows printer.

Syntax::

    $PrintOrientation {Portrait|Landscape}

Example::

    $PrintOrientation Portrait

.. _printduplex:

COMMAND PRINTDUPLEX
^^^^^^^^^^^^^^^^^^^

Sets the duplex (double-sided) mode of the current Windows printer.

Syntax::

    $PrintDuplex {Simplex|Duplex|VerticalDuplex}

Example::

    $PrintDuplex Duplex

.. _printgifbackcolor:

COMMAND PRINTGIFBACKCOLOR
^^^^^^^^^^^^^^^^^^^^^^^^^

Sets the background color of GIF charts generated when creating an HTML document.

Syntax::

    $PrintGIFBackColor {Black|Blue|Magenta|Cyan|Red|Green|Yellow|White}

Example::

    $PrintGIFBackColor Yellow

.. _printgiftranscolor:

COMMAND PRINTGIFTRANSCOLOR
^^^^^^^^^^^^^^^^^^^^^^^^^^

Sets the color considered as "transparent" in GIF files generated when creating an 
HTML document. This allows one of the colors to blend with the background. 
The "Transparent" option must be set by the $PrintGIFTransparent Yes command.

Syntax::

    $PrintGIFTransColor {Black|Blue|Magenta|Cyan|Red|Green|Yellow|White}

Example::

    $PrintGIFTransColor White

.. _printgiftransparent:

COMMAND PRINTGIFTRANSPARENT
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Indicates whether GIF files generated when creating an HTML document should be 
in "transparent" format. This allows one of the colors (to be specified by the 
$PrintGIFTransColor command) to blend with the background.

Syntax::

    $PrintGIFTransparent {Yes|No}

Example::

    $PrintGIFTransparent No

.. _printgifinterlaced:

COMMAND PRINTGIFINTERLACED
^^^^^^^^^^^^^^^^^^^^^^^^^^

Indicates whether GIF files generated when creating an HTML document should be in 
interlaced format.

Syntax::

    $PrintGIFInterlaced {Yes|No}

Example::

    $PrintGIFInterlaced No

.. _printgiffilled:

COMMAND PRINTGIFFILLED
^^^^^^^^^^^^^^^^^^^^^^

Indicates whether to fill bars in bar charts of GIF graphics.

Syntax::

    $PrintGIFFilled {Yes|No}

Example::

    $PrintGIFFilled No

.. _printgiffont:

COMMAND PRINTGIFFONT
^^^^^^^^^^^^^^^^^^^^

Specifies the font to use when creating GIF graphics. 
The available fonts are:

- 0 :  Tiny
- 1 :  Small
- 2 :  MediumBold
- 3 :  Large
- 4 :  Giant
- 5 :  Grayed

Syntax::

    $PrintGIFFont FontNb (between 0 and 5)

Example::

    $PrintGIFFont 1


Object printing
~~~~~~~~~~~~~~~

The following commands are used to print object definitions:

- :ref:`printobjdef <printobjdef>` : prints a definition of an object cmt / eqs / idt / lst / scl / tbl / var
- :ref:`printobjtitle <printobjtitle>` : specifies whether only the titles should be printed
- :ref:`printobjlec <printobjlec>` : specifies the type of equation to print
- :ref:`printobjinfos <printobjinfos>` : specifies the information to print


.. _printobjdef:

COMMAND PRINTOBJDEFXXX
^^^^^^^^^^^^^^^^^^^^^^

Xxx takes one of the following values:
    cmt / eqs / idt / lst / scl / tbl / var

This command prints the definition of objects from the current workspace in A2M format.

The list of comments to print is passed as a parameter to the command.

Syntax::

    $PrintObjDefXxx [list of parameters]  
    where Xxx = {cmt / idt / lst / scl / tbl / var}

Example::

    $PrintObjDefCmt CMT1 CMT2

.. _printobjtitle:

COMMAND PRINTOBJTITLE
^^^^^^^^^^^^^^^^^^^^^

This function is used before :ref:`$PrintObjDefTbl <printobjdeftbl>`.
It indicates whether only the table titles should be printed or the entire definition.

Syntax
    $PrintObjTitle 0 or 1  
    0: full definition  
    1: titles only

.. _printobjlec:

COMMAND PRINTOBJLEC
^^^^^^^^^^^^^^^^^^^

This function is used before :ref:`$PrintObjDefXxx <printobjdef>`.
It indicates how equations should be printed: as is, with coefficients replaced 
by their values, or with t-tests.

Syntax
    $PrintObjLec 0, 1 or 2  
    0: normal LEC form  
    1: coefficients replaced by their value  
    2: coefficients replaced by their value + t-tests

.. _printobjinfos:

COMMAND PRINTOBJINFOS
^^^^^^^^^^^^^^^^^^^^^

This function is used before :ref:`$PrintObjDefEqs <printobjdefeqs>`.
It indicates the information to print:

- equations only
- equations and comments
- equations, comments, and estimation results

Syntax::

    $PrintObjInfos 0, 1 or 2  
    0: equation only  
    1: equation + comment  
    2: equation + comment + estimation


Table compilation and printing
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The following commands are used to compile tables and print or view them:

- :ref:`PrintTblFile <printtblfile>`
- :ref:`PrintTbl <printtbl>`
- :ref:`ViewTblFile <viewtblfile>`
- :ref:`ViewTbl <viewtbl>`
- :ref:`ViewByTbl <viewbytbl>`
- :ref:`PrintVar <printvar>`
- :ref:`ViewVar <viewvar>`
- :ref:`ViewNdec <viewndec>`
- :ref:`ViewWidth <viewwidth>` (obsolete) 
- :ref:`ViewWidth0 <viewwidth0>` (obsolete)  


.. _printtblfile:

COMMAND PRINTTBLFILE
^^^^^^^^^^^^^^^^^^^^

This command allows you to define the files to use when printing a comparison table 
between several files (maximum 4). The print sample can contain references to files 
other than the current workspace, placed in brackets (e.g. ``90:2^[1;2]``). 
File [1] is always the current workspace, the others can be defined either in the input 
page or as an argument in a report.
The purpose of this function is to indicate the files referenced by [2], [3], etc.

The file number (2, 3, 4, or 5) followed by the file name are passed as parameters 
to the command.

Syntax::

    $PrintTblFile n varfilename  
    where n = 2, 3, 4, 5

Example::

    $wsloadvar bist92\bistel  
    $wsloadtbl bist92\tbistelf  
    
    $printdest bist92\bistelf1.a2m  
    #show processing french tables  file 1/2  
    $PrintNbDec 1  
    $PrintTblFile 2 variante  
    $PrintTbl 89:8[1;2] HYPEIR

After a call to :ref:`$PrintTbl <printtbl>`, you must redefine the files to compare: 
:ref:`$PrintTbl <printtbl>` "consumes" the file comparison definitions. 
There can be no space in the sample statement.

.. _printtbl:

COMMAND PRINTTBL
^^^^^^^^^^^^^^^^

This command allows you to build and print tables in A2M format.

The print sample followed by the list of tables to print are passed as parameters to 
the command. The last call to :ref:`$PrintTblFile <printtblfile>` allows you to print 
file comparisons. However, the call to `$PrintTbl`` or :ref:`$PrintVar <printvar>` "consumes" 
the files defined by :ref:`$PrintTblFile <printtblfile>`. If you want to call `$PrintTbl` 
several times to compare files, you must specify the files to compare again before each call.

Syntax::

    $PrintTbl sample table1 [other tables]

Example::

    $wsloadvar bist92\bistel  
    $wsloadtbl bist92\tbistelf  
    
    $printdest bist92\bistelf1.a2m  
    #show processing french tables  file 1/2  
    $printnbdec 1  
    $PrintTbl 89:8 HYPEIR  
    $PrintTbl 89/88:8 HYPEIIR  
    $PrintTbl 89:8 RENAT01 RENAT0D RESL3R RESL31R RESL2

.. warning::

    There can be no space in the sample statement.

Successive calls
""""""""""""""""

The last call to :ref:`$PrintTblFile <printtblfile>` allows the suer to print 
file comparisons. However, the call to `$PrintTbl`` or :ref:`$PrintVar <printvar>` 
"consumes" the files defined by :ref:`$PrintTblFile <printtblfile>`. 
If you want to call `$PrintTbl`` several times to compare files, you must specify the 
files to compare again before each call.

Example::

    $PrintTblFile 2 ref.var  
    $PrintTblFile 3 var1.var  
    $PrintTbl 89:8[1;2;3] HYPEIR  
    $PrintTblFile 2 ref.var  
    $PrintTbl 89/88:8[1/2] HYPEIIR
    
.. _viewtblfile:

COMMAND VIEWTBLFILE
^^^^^^^^^^^^^^^^^^^

This command allows you to define the files to use when viewing a comparison table 
between several files (max 4). The print sample can contain references to files other 
than the current workspace. (e.g. ``90:2[1;2]``). File [1] is always the current workspace, 
the others can be defined either in the input page or as an argument in a report.

The file number (2, 3, 4, or 5) followed by the file name are passed as parameters to 
the command.

Syntax::

    $ViewTblFile n varfilename  
    where n = 2, 3, 4, 5

Example::

    $wsloadvar bist92\bistel  
    $wsloadtbl bist92\tbistelf  
    
    $PrintNbDec 1  
    $ViewTblFile 2 variante  
    $ViewTbl 89:8[1;2] HYPEIR

After a call to :ref:`$ViewTbl <viewtbl>`, you must redefine the files to compare: 
:ref:`$ViewTbl <viewtbl>` "consumes" the file comparison definitions.

.. _viewtbl:

COMMAND VIEWTBL
^^^^^^^^^^^^^^^

This function builds and displays tables in a scrollable table (or editing table).

The print sample followed by the list of tables to view are passed as parameters 
to the command.

Syntax::

    $ViewTbl sample table [list of tables]

Example::

$ViewTbl 90:6 CIIIR0 CIIR

.. warning::

    There can be no space in the sample statement.

.. _viewbytbl:

COMMAND VIEWBYTBL
^^^^^^^^^^^^^^^^^

Alias for the :ref:`$ViewTbl <viewtbl>` command.

.. _printvar:

COMMAND PRINTVAR
^^^^^^^^^^^^^^^^

This command allows the user to build and print comparison tables of series in A2M format. 
LEC forms are accepted and must be separated by semicolons.
To define the files to compare, you must use the :ref:`$PrintTblFile <printtblfile>` command 
as for tables and graphs.

Syntax::

    $PrintVar gsample var1;var2

Example::

    $PrintVar (80Y1/79Y1):10 X+Y;Z;ln(X)

.. warning::

    There can be no spaces in the generalized sample.

.. _viewvar:

COMMAND VIEWVAR
^^^^^^^^^^^^^^^

This command allows you to view comparison tables of series in A2M format.

Syntax::

    $ViewVar gsample lec1 [lec2 ...]  
    
    where gsample is a generalized sample without spaces  
          lec1 and lec2 are LEC forms without spaces

Example::

    $ViewVar (80Y1/79Y1):10  X+Y  Z  ln(X)

.. warning::

    There can be no spaces in the generalized sample.

.. _viewwidth:

COMMAND VIEWWIDTH
^^^^^^^^^^^^^^^^^

.. warning::

    OBSOLETE

This function allows you to specify the column width when displaying tables (calculated). 
The value must be between 2 and 12.

Syntax::

    $ViewWidth n  
    n between 2 and 12

.. _viewwidth0:

COMMAND VIEWWIDTH0
^^^^^^^^^^^^^^^^^^

.. warning::

    OBSOLETE

This function allows you to specify the width of the first column (title column) 
when displaying tables (calculated). The value must be between 2 and 60.

Syntax::

    $ViewWidth0 n  
    n between 2 and 60

.. _viewndec:

COMMAND VIEWNDEC
^^^^^^^^^^^^^^^^

This function allows you to specify the number of decimals for values displayed 
in tables (calculated). The value must be between -1 (variable number) and 6.

Syntax::

    $ViewNdec n  
    n between -1 (variable) and 6


Graphs from tables
~~~~~~~~~~~~~~~~~~

The following commands are used to compile tables and display them as graphs or save them 
in an A2M file:

- :ref:`ViewGr <viewgr>`
- :ref:`PrintGrAll <printgrall>` (obsolete) 
- :ref:`PrintGrWin <printgrwin>` (obsolete) 
- :ref:`PrintGrData <printgrdata>` (obsolete) 
- :ref:`PrintGr <printgr>`
- :ref:`DataPrintGraph <dataprintgraph>`


.. _viewgr:

COMMAND VIEWGR
^^^^^^^^^^^^^^

This command allows you to display tables as graphs. 
Several graphs can be displayed on the same screen. 
The language is set by :ref:`$PrintLang <printlang>`.

Syntax::

    $ViewGr gsample tbl1[+tbl2] tbl3 ...  
    where gsample is a generalized sample without spaces  
          tbl1, tbl2, and tbl3 are table names

When two tables are joined by the ``+`` sign, they are displayed on the same screen 
as two separate graphs.

.. warning::

    There can be no spaces in the generalized sample.

.. _printgrall:

COMMAND PRINTGRALL
^^^^^^^^^^^^^^^^^^

.. warning::

    This command is deprecated as of version 5 of IODE. 
    It is replaced by the :ref:`$PrintGr <printgr>` command.

.. _printgrwin:

COMMAND PRINTGRWIN
^^^^^^^^^^^^^^^^^^

.. warning::

    This command is deprecated as of version 5 of IODE.

.. _printgrdata:

COMMAND PRINTGRDATA
^^^^^^^^^^^^^^^^^^^

.. warning::

    This command is deprecated as of version 5 of IODE.

.. _printgr:

COMMAND PRINTGR
^^^^^^^^^^^^^^^

This command prints one or more graphs defined from tables.

Syntax::

    $PrintGr gsample table_names

Example::

    $PrintGr 80/79:15 T1 T2

.. _dataprintgraph:

COMMAND DATAPRINTGRAPH
^^^^^^^^^^^^^^^^^^^^^^

This command prints graphs built directly from variables (without using tables). 

Syntax::

    $DataPrintGraph   {Level | Diff | Grt}  
                      {Line | Scatter | Bar | Mixt}  
                      {NoXGrids | MinorXGrids | J(MajorXGrids)}  
                      {NoYGrids | MinorYGrids | J(MajorYGrids)}  
                      {Level | G(Log) | Semi-Log | Percent}  
                      {ymin | --} {ymax | --}  
                      perfrom perto varname1 varname2 ...

Example::

    $DataPrintGraph  Level Line No No Level -- -- 1980Y1 1995Y1 X1 Y1 X1+Y1


Model operations
~~~~~~~~~~~~~~~~

The following commands allow you to sort and simulate a model, estimate, and 
recompile equations:

- :ref:`ModelCalcSCC <modelcalcscc>`
- :ref:`ModelSimulateParms <modelsimulateparms>`
- :ref:`ModelSimulate <modelsimulate>`
- :ref:`ModelSimulateSCC <modelsimulatescc>`
- :ref:`ModelExchange <modelexchange>`
- :ref:`ModelCompile <modelcompile>`
- :ref:`ModelSimulateSaveNiters <modelsimulatesaveniters>`
- :ref:`ModelSimulateSaveNorms <modelsimulatesavenorms>`


.. _modelcalcscc:

COMMAND MODELCALCSCC
^^^^^^^^^^^^^^^^^^^^

This function decomposes the model into Strongly Connected Components (SCC) and reorders it.
Three lists are created: pre-recursive equations, interdependent equations, and 
post-recursive equations. When reordering the model, the number of triangulation (sorting) 
iterations for the interdependent block must be specified. This value only affects the list 
of interdependent equations. These 3 lists should only be constructed once for a given 
version of the model.

Syntax::

    $ModelCalcSCC nbtri lstpre lstinter lstpost

where:

- ``nbtri``: number of triangulation iterations to perform
- ``lst[pre|inter|post]``: *NAMES* of the lists intended to contain the results 
  of the equation sorting

Choosing the number of sorts
""""""""""""""""""""""""""""

After decomposition into SCC, the interdependent block is sorted to increase simulation speed. 
The number of passes of the sorting algorithm can be specified in several places:

- As a parameter to the `$ModelCalcSCC` report command
- As the last parameter to the :ref:`$ModelSimulateParms <modelsimulateparms>` report command

.. warning::

    In earlier versions, the number of triangulation passes specified in the 
    simulation screen had no effect

.. _modelsimulateparms:

COMMAND MODELSIMULATEPARMS
^^^^^^^^^^^^^^^^^^^^^^^^^^

This command allows you to specify additional (mainly technical) parameters for a simulation. 
The simulation is actually launched by the :ref:`$ModelSimulate <modelsimulate>` command.

This function sets the following simulation parameters:

- ``eps``: convergence threshold
- ``relax``: relaxation parameter
- ``maxit`` : maximum number of iterations
- ``Connex, Triangulation or None``: reordering algorithm
- initial values (0 to 4)
    - 0 = ``Y := Y[-1], if Y null or NA``
    - 1 = ``Y := Y[-1], always``
    - 2 = ``Y := extrapolation, if Y null or NA``
    - 3 = ``Y := extrapolation, always``
    - 4 = ``Y unchanged``
- simulation with debugging
- debugging of the Newton-Raphson sub-iteration
- number of sorting iterations for the connected components

Syntax::

    $ModelSimulateParms eps relax maxit  
                        {Connex | Triang | None }  
                        0 - 4 (starting values)  
                        {Yes | no }  
                        {yes | No }  
                        nbtri  
    
    where eps   = convergence threshold (real number)  
          relax = real number between 0.1 and 1
    
Example::

    $ModelSimulateParms 0.001 0.7 100 Connex 0 Yes No 2

Debug Option
""""""""""""

The simulation debug option automatically generates lists including pre-recursive and 
post-recursive equations.

.. warning::

    Version 6.36: enabling the debug option generated a ``simul.dbg`` file containing 
    a huge amount of information. In later versions, only the ``_PRE``, ``_INTER`` and 
    ``_POST`` lists (with the model decomposition) are generated.

Sub-iteration Debug Option
"""""""""""""""""""""""""""

A trace file is generated if this option is set to *Y*. 
This file is named ``simul.dbg`` and is located in the directory from which IODE was launched. 
The trace file includes the list of equations for which a sub-iteration (Newton-Raphson) 
was necessary and the number of sub-iterations.

Choosing the number of sorts
""""""""""""""""""""""""""""

After decomposition into SCC, the interdependent block is sorted to increase simulation speed.

.. _modelsimulate:

COMMAND MODELSIMULATE
^^^^^^^^^^^^^^^^^^^^^

This function launches the simulation of a model.
To simulate a model, all the model's equations must be present in the equations workspace, 
and all variables and scalars used in the model's equations must be defined in the variables 
and scalars workspaces. The values of exogenous variables and scalars CANNOT be `NA` 
(Not Available) over the relevant period.

The simulation parameters are defined by the auxiliary command 
:ref:`$ModelSimulateParms <modelsimulateparms>`. The simulation itself is performed by the 
:ref:`$ModelSimulate <modelsimulate>` command.
The two boundary periods (start and end) and the list of equations to simulate are passed 
as arguments to the command. If the list is empty, the simulation will be performed for the 
model consisting of all equations in memory.

Syntax::

    $modelsimulate period1 period2 [list_of_equations]

Example::

    $-filedeletea2m ode.a2m  
    $printdest ode.a2m  
    #modelsimulate  
    $modelsimulate 1990Y1 1995Y1  
    $show test1  
    $modelsimulate 1990Y1 1995Y1 PC0 PIF  
    $show test2  
    $modelsimulate 1990Y1 1995Y1 $LIST  
    $show end of report

.. _modelsimulatescc:

COMMAND MODELSIMULATESCC
^^^^^^^^^^^^^^^^^^^^^^^^

This command launches the simulation of a model previously decomposed into SCC and sorted.
Simulation performance, especially at startup (link, sort), is better with this command 
than with :ref:`$ModelSimulate <modelsimulate>` because the model reordering occurs only 
once and not at each simulation launch.
The simulation process has been divided into 2 steps. The first only handles the reordering 
of the model (:ref:`$ModelCalcSCC <modelcalcscc>`), the second handles the simulation.

The model simulation is based on three lists previously built by the 
:ref:`$ModelCalcSCC <modelcalcscc>` command (or manually).

Syntax::

    $ModelSimulateSCC from to pre inter post

where:

- ``from`` and ``to`` determine the simulation sample
- ``pre``, ``inter`` and ``post`` are the lists that define the execution order of the model.

.. _modelexchange:

COMMAND MODELEXCHANGE
^^^^^^^^^^^^^^^^^^^^^

This command allows you to define or cancel endogenous-exogenous exchanges before 
launching the simulation of a model with *goal seeking*.

Syntax::

    $ModelExchange eqname1-varname1,eqname2-varname2,...

    where eqname1 and eqname2 are equation names (and thus endogenous variable names)
          varname1 and varname2 are exogenous variable names

To cancel the ENDO-EXO exchange, simply call the command without parameters::

    $ModelExchange

.. _modelcompile:

COMMAND MODELCOMPILE
^^^^^^^^^^^^^^^^^^^^

This command recompiles a list of equations or all equations if no list is specified. 
It is only useful if equations use macros in their LEC form.

Syntax::

    $ModelCompile  [eqname1, eqname2, ... ]

    where eqname1 and eqname2 are equation names.

.. _modelsimulatesaveniters:

COMMAND MODELSIMULATESAVENITERS
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This command allows you to save in a variable the number of iterations required for each 
period during the last simulation. The name of the result variable is passed as a parameter.
If there has not yet been a simulation during the session, the variable is created with `NA` 
values. If a simulation has already taken place, the values for non-simulated periods are 
set to 0 and those for simulated periods contain the number of iterations required for 
convergence for that period. If there is no convergence, the value is that of the ``maxit`` 
parameter of the simulation.

Syntax::

    $ModelSimulateSaveNiters varname

Example::

    $ModelSimulateSaveNiters SIM_NITERS

.. _modelsimulatesavenorms:

COMMAND MODELSIMULATESAVENORMS
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This command allows you to save in a variable the convergence threshold (the norm) reached 
for each period during the last simulation. The name of the result variable is passed as a 
parameter. If there has not yet been a simulation during the session, the variable is created 
with `NA` values. If a simulation has already taken place, the values for non-simulated 
periods are set to 0 and those for simulated periods contain the convergence threshold for 
that period.

Syntax::

    $ModelSimulateSaveNorms varname

Example::

    $ModelSimulateSaveNiters SIM_NORMS


Identities executions
~~~~~~~~~~~~~~~~~~~~~

The following commands allow you to execute *identities*:

- :ref:`idtexecute <idtexecute>`
- :ref:`idtexecutetrace <idtexecutetrace>`
- :ref:`idtexecutevarfiles <idtexecutevarfiles>`
- :ref:`idtexecutesclfiles <idtexecutesclfiles>`


.. _idtexecute:

COMMAND IDTEXECUTE
^^^^^^^^^^^^^^^^^^

An identity is an LEC form that indicates how a series (variable) should be constructed.
The execution program calculates all the requested identities and saves the calculation 
results in the variables workspace. These identities are ordered in the logical order of 
execution: if *A* depends on *B*, *B* will be calculated before *A*. 
The user does not need to worry about this ordering. If a loop is detected among the 
identities, execution cannot start and a message is displayed.

The two boundary periods (start and end) and the list of identities to calculate are 
passed as arguments to the command. If the list is empty, the calculation will be performed 
for all identities in memory. Additional parameters can be defined by the commands 
:ref:`IdtExecuteVarFiles <idtexecutevarfiles>` and :ref:`IdtExecuteSclFiles <idtexecutesclfiles>`.

Syntax::

    $IdtExecute period1 period2 [list_of_identities]

Example::

    $IdtExecute 1953Y1 1996Y1 IDT1 IDT2  
    $IdtExecute 1953Y1 1996Y1 $IDTLIST

.. _idtexecutetrace:

COMMAND IDTEXECUTETRACE
^^^^^^^^^^^^^^^^^^^^^^^

The trace of the identity calculation can be saved in the current A2M file defined by 
:ref:`$PrintDest <printdest>`. Useful information (for example, the source file of a 
variable used to construct the identity) will be listed in the *Trace file*.

Depending on the parameter, the trace will or will not be saved during the next call to 
the :ref:`$IdtExecute <idtexecute>` command.

Syntax::

    $IdtExecuteTrace  {Yes | No}

.. _idtexecutevarfiles:

COMMAND IDTEXECUTEVARFILES
^^^^^^^^^^^^^^^^^^^^^^^^^^

This command is part of a group of commands that allow you to specify additional parameters 
(search files) for the calculation of identities. The associated commands are 
:ref:`$IdtExecute <idtexecute>` and :ref:`$IdtExecuteSclFiles <idtexecutesclfiles>`.
The calculation of identities is actually launched by the :ref:`$IdtExecute <idtexecute>` 
command. This command allows you to specify the names of files where the series referenced 
in the identities should be searched. When a series or scalar needed for the execution of an 
identity is encountered, the specified files are analyzed in turn. As soon as the object is 
found, it is loaded into memory for the duration of the calculation. It is then deleted, 
unless it was initially in memory. If an object - variable or scalar - is neither in a 
workspace nor in one of the specified files, the calculation process stops.
This command is only meaningful when the :ref:`$IdtExecute <idtexecute>` command is used 
in command-line mode.

.. warning::

    If the list of files is empty, only the current variables workspace is used for the search. 
    Otherwise, you must specify ``WS`` (in uppercase) in the list of files, at the desired 
    search position.

The list of files is passed as a parameter to the command.

Syntax::

    $IdtExecuteVarFiles file [list of files]

Example::

    $IdtExecuteVarFiles maribel.var WS test.var

.. _idtexecutesclfiles:

COMMAND IDTEXECUTEScalarFILES
^^^^^^^^^^^^^^^^^^^^^^^^^^

This command is part of a group of commands that allow you to specify additional parameters 
(search files) for the calculation of identities. The associated commands are 
:ref:`$IdtExecuteVarFiles <idtexecutevarfiles>` and :ref:`$IdtExecute <idtexecute>`.
The calculation of identities is actually launched by the :ref:`$IdtExecute <idtexecute>` 
command. This function allows you to specify the names of files where the scalars referenced 
in the identities should be searched. When a scalar needed for the execution of an identity 
is encountered, the specified files are analyzed in turn. As soon as the object is found, 
it is loaded into memory for the duration of the calculation.
It is then deleted, unless it was initially in the active workspace.
If an object—variable or scalar—is neither in a workspace nor in one of the specified files, 
the calculation process stops. This function is only meaningful when the 
:ref:`$IdtExecute <idtexecute>` command is used in command-line mode.

.. warning::

    If the list of files is empty, only the current scalars workspace is used for the search. 
    Otherwise, you must specify ``WS`` (in uppercase) in the list of files, at the desired 
    search position.

The list of files is passed as a parameter to the function.

Syntax::

    $IdtExecuteSclFiles file [list of files]

Exemple::

    $IdtExecuteSclFiles WS maribel.scl test.scl


Report operations 
~~~~~~~~~~~~~~~~~~

The following commands allow you to manipulate reports (these functions are recursive):

- :ref:`reportexec <reportexec>`
- :ref:`reportedit <reportedit>` (OBSOLETE)


.. _reportexec:

COMMAND REPORTEXEC
^^^^^^^^^^^^^^^^^^

This command executes an IODE report.
The command is recursive and allows multiple levels of sub-reports.
When sub-reports finish without a critical error, execution continues with the commands 
of the higher-level report.

The report to execute and its arguments are passed as parameters to the command.

Syntax::

    $ReportExec report [arguments]

Example::

    $ReportExec mysimul.rep 1990Y1 1992Y1

The `$ReportExec` function accepts a list as an argument::

    $ReportExec test $LIST

receives as arguments the elements of the `$LIST` list and thus allows you to execute 
a generic report on an initially unknown number of arguments. 
For example, to replace all 0s with `NA` for the list `$LIST` of variables, simply do::

    FILE ZEROTONA.REP  

        $label again  
        $goto end {%0%=0}  
        #show Series processed: %1%  
        $DataCalcVar %1% if(%1%=0, 1/0, %1%)  
        $shift  
        $goto again  
        $label end

The command::

    $ReportExec zerotona.rep $LIST

will replace 0s with NA for the series in the `$LIST` list.

.. _reportedit:

COMMAND REPORTEDIT
^^^^^^^^^^^^^^^^^^

.. warning::

    OBSOLETE

This function allows you to create or modify a report.

The file to edit is passed as a parameter to the command.

Syntax::

    $ReportEdit filename

Example::

    $ReportEdit bist92\bistel.rep


A2M file translation
~~~~~~~~~~~~~~~~~~~~

The following functions allow you to translate A2M files into different formats:

- :ref:`A2mToHtml <a2mtohtml>` : translation to HTML
- :ref:`A2mToRtf <a2mtortf>` : translation to Word (Rtf)
- :ref:`A2mToMif <a2mtomif>` : translation to Frame (MIF)
- :ref:`A2mToCsv <a2mtocsv>` : translation to CSV format
- :ref:`A2mToPrinter <a2mtoprinter>` : interprets and prints an A2M file


.. _a2mtohtml:

COMMAND A2MTOHTML
^^^^^^^^^^^^^^^^^

Translates an A2M file to HTML. If a previous call to the :ref:`$PrintHtmlHelp <printhtmlhelp>` 
command was made, the result will be an HTML file or several files allowing the creation of 
a *HTML Help*.

Syntax::

    $A2mToHtml filein fileout
    where filein = name of the input A2M file  
          fileout = name of the output file

When creating the *HtmlHelp*, several output files will be created:

- fileout.hhp  
- fileout.hhc  
- fileout.hhk  
- fileout.htm  
- T*.htm

.. _a2mtortf:

COMMAND A2MTORTF
^^^^^^^^^^^^^^^^

Translates an A2M file to RTF. If a previous call to the :ref:`$PrintRtfHelp <printrtfhelp>` 
command was made, the result will be an RTF file allowing the creation of a *Windows Help*.

Syntax::

    $A2mToRtf filein fileout
    where filein = name of the input A2M file  
          fileout = name of the output file

When creating the *Windows Help*, several output files will be created:

- fileout.rtf  
- fileout.hpj  
- fileout.cnt

.. _a2mtomif:

COMMAND A2MTOMIF
^^^^^^^^^^^^^^^^

Translates an A2M file to a MIF file.

Syntax::

    $A2mToMif filein fileout
    where filein = name of the input A2M file  
          fileout = name of the output file

.. _a2mtocsv:

COMMAND A2MTOCSV
^^^^^^^^^^^^^^^^

Translates an A2M file to a CSV file.

Syntax::

    $A2mToCsv filein fileout
    where filein = name of the input A2M file  
          fileout = name of the output file

.. _a2mtoprinter:

COMMAND A2MTOPRINTER
^^^^^^^^^^^^^^^^^^^^

.. warning::

    OBSOLETE

Translates an A2M file and prints it on the current printer.

Syntax::
    
    $A2mToPrinter file.a2m


Other report functions
~~~~~~~~~~~~~~~~~~~~~~

- :ref:`StatUnitRoot <statunitroot>` : Dickey-Fuller tests
- :ref:`WsAggrChar <wsaggrchar>` : sets the character to introduce in the code of series created by `WsAggr***`
- :ref:`WsAggrSum <wsaggrsum>` : computes the sum of series
- :ref:`WsAggrMean <wsaggrmean>` : computes the mean of series
- :ref:`WsAggrProd <wsaggrprod>` : computes the product of series


.. _statunitroot:

COMMAND STATUNITROOT
^^^^^^^^^^^^^^^^^^^^

This command computes Dickey-Fuller tests.
The values of the tests are saved in scalars whose name is composed of the prefix ``df_`` 
and the name of the first variable appearing in the formula to be tested.
For example, the test for the formula ``d(A0GR+A0GF)`` is ``df_a0gr``.

Syntax::

    $StatUnitRoot drift trend order expression

where:

- ``drift``: 0 or 1 depending on whether the estimated formula should include 
  a *constant* term (1) or not (0)
- ``trend``: 0 or 1 depending on whether the estimated formula should include 
  a *trend* term (1) or not (0)
- ``order``: the order of the polynomial to estimate for the tests
- ``expression``: *LEC* formula to test

Example::

    $StatUnitRoot 1 1 3 A

Result::

    d(A) := df_ * A[-1]+  
            df_d +                                        /* DRIFT */  
            df_t * t +                                    /* TREND */  
            df1 * d(A[-1]) + df2*d(A[-2]) + df3*d(A[-3])  /* ORDER */

.. _wsaggrchar:

COMMAND WSAGGRCHAR
^^^^^^^^^^^^^^^^^^

Defines the character to introduce in the name of the series created by ``WsAggr...``.
See :ref:`$WsAggrSum <wsaggrsum>`.

.. _wsaggrsum:

COMMAND WSAGGRSUM
^^^^^^^^^^^^^^^^^

Report functions allow you to perform aggregations, products, or sums of series. 
The series to process can be in the current workspace or in an external file.

Four report functions have been defined for this purpose::

    $WsAggrChar [char] : defines the character to introduce in the name of the created series  
    $WsAggrSum  pattern [filename] : sum of the series defined by the pattern  
    $WsAggrMean pattern [filename] : mean of the series defined by the pattern  
    $WsAggrProd pattern [filename] : product of the series defined by the pattern  

If ``filename`` is defined, the series from the file will be used instead of those from the 
current workspace.

``pattern`` is defined as a sequence of name parts placed between brackets or parentheses. 
Each part can contain alphanumeric characters or a question mark.

Rules:

- Name parts in parentheses ``()`` are NOT aggregated.
- Those in brackets ``[]`` are aggregated.

Example::

    Suppose a workspace with series by country (BE, FR, GE), and by sector (S101..S999):

    BES101, BES102 ... BES199  
    BES201, BES202 ... BES299  
    ...  
    BES901, BES902 ... BES999  

    FRS101, FRS102 ... FRS199  
    FRS201, FRS202 ... FRS299  
    ...  
    FRS901, FRS902 ... FRS999  

    ...  

    GBS101, GBS102 ... GBS199  
    GBS201, GBS202 ... GBS299  
    ...  
    GBS901, GBS902 ... GBS999

It is possible to create the sum of all sectors for each country with the commands::

    $WsAggrChar _  
    $WsAggrSum (??)[????]

The created series will be::

    BE____, FR____, ..., GB____

The ``?`` characters in ``()`` specify the series codes to process. 
The others in ``[]`` indicate the parts to aggregate. 
In the above case, the ``?`` characters are replaced by underscores ``_`` (or another 
character according to the argument of :ref:`$WsAggrChar <wsaggrchar>`) in the result series. 
This character can be a blank space. In the example, the created series are then *BE*, *FR*, 
and *GB*.

It is also possible to create the sum of all countries by sector or the sum of all 
countries for certain sectors::

        $WsAggrSum [??](????)   : creates __S101, __S102, ...  
        $WsAggrSum [??](??)[??] : creates __S1__, __S1__, ...

It is even possible to limit the creation to only one or a few codes::

    $WsAggrSum (BE)[S??9]   : creates BES__9  
    $WsAggrSum (BES)[?](?9) : creates BES_09, BES_19, BES_29, ... BES_99

Grouping character
""""""""""""""""""

The :ref:`$WsAggrChar <wsaggrchar>` command allows you to specify the character to place 
in the generated series. This character can be a blank space. To prevent already aggregated 
series from being included in a subsequent aggregation, these series are not included in the 
calculation if the current aggregate character is found at a position in the *pattern* 
corresponding to a ``?`` in ``[]``. Thus, the series ``BE____`` will not be included in the 
calculation ``(BE)[????]``. However, it will be included in the calculation with the pattern 
``[??](????)``.

Suppose the series ``BE____``, ``FR____`` and ``GB____`` are defined as well as ``BES101``, ...
Then, the call ``$WsAggrSum (??)[????]`` will overwrite ``BE____``, ``FR____``, etc.
Therefore, it does not use the series containing the underscore ``_`` after the second 
position, such as ``BE____``. Indeed, if they were included, the same series would be added 
twice. On the other hand, the call ``$WsAggrSum [??](????)`` will generate ``______`` by 
taking the sum of ``BE____``, ``FR____``, ``GB____``, which is correct because the other 
series (like ``BES101``) will be aggregated in other series (``__S101``).

.. _wsaggrmean:

COMMAND WSAGGRMEAN
^^^^^^^^^^^^^^^^^^

Computes the mean of series.
See :ref:`$WsAggrSum <wsaggrsum>`.

.. _wsaggrprod:

COMMAND WSAGGRPROD
^^^^^^^^^^^^^^^^^^

Computes the product of series.
See :ref:`$WsAggrSum <wsaggrsum>`.


Python equivalent functions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To get the Python equivalent of the above IODE commands, refers to the 
:ref:`Equivalence Between IODE Report Commands and Python Methods <report_commands_equivalence>` 
tables.
