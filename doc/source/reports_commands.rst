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

This command allows you to print a file. The file must be a text file.
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

This function allows you to copy any file on disk.
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

This function allows you to rename any file on disk.
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

This function allows you to delete any file on disk.
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

This function allows you to append one file to another.
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

This function allows you to add or replace objects from a workspace file saved on disk.
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

COMMAND WSCLEARXXX
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

COMMAND WSCLEARALL
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

COMMAND WSSAMPLE
^^^^^^^^^^^^^^^^

This function allows you to modify the sample of the IODE variables workspace.
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

This function allows you to complete series using a chosen method based on previous periods.
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

This operation allows you to perform a :ref:`$WsLoad <wsload>` from an ASCII file.
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

This function allows you to create objects in the current workspace.

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

This function allows you to delete objects in the current workspace.
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

This function allows you to edit objects in the current workspace.
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

This function allows you to search in one of the current workspaces for the list of objects
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

This function allows you to search in one of the current workspaces for the list of 
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

This function allows you to calculate a new series from a LEC formula.
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

This function allows you to display series as a graph.

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

This function allows you to build graphs from series and save the result in an AGL file.

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

This function allows you to specify the column width for editing statistical series.
The value must be between 2 and 12.

Syntax::

    $DataWidthVar n  
    n between 2 and 12

.. _datandecvar:

COMMAND DATANDECVAR
^^^^^^^^^^^^^^^^^^^

This function allows you to specify the number of decimals when editing statistical series. 
The value must be between -1 (variable number of decimals) and 6.

Syntax::

    $DataNdecVar n  
    n between -1 and 6

.. _datamodevar:

COMMAND DATAMODEVAR
^^^^^^^^^^^^^^^^^^^

This function allows you to choose the editing mode for statistical series: Level, 
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

This function allows you to choose the first visible column when editing statistical series. 
The value must be between 0 (first column) and the number of available periods.

Syntax::

    $DataStartVar n  
    where n is between 0 and the number of periods

.. _datawidthtbl:

COMMAND DATAWIDTHTBL
^^^^^^^^^^^^^^^^^^^^

.. warning::

    OBSOLETE

This function allows you to specify the column width for editing IODE tables.
The value must be between 2 and 60.

Syntax::

    $DataWidthTbl n
    n between 2 and 60

COMMAND DATAWIDTHSCL
^^^^^^^^^^^^^^^^^^^^

This function allows you to specify the number of decimals when editing scalars.
The value must be between -1 (variable number of decimals) and 6.

Syntax::

    $DataNdecScl n  
    where n is between -1 and 6

.. _dataeditcnf:

COMMAND DATAEDITCNF
^^^^^^^^^^^^^^^^^^^

This function allows you to change the display mode of the series table to ``Level`` 
(actual value), ``Difference``, or ``Growth Rate``. 
It also allows you to choose the number of decimals to display.

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

COMMAND EQSSETSAMPLE
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

This function allows you to specify whether to print to a file and, if so, the name and 
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

This function allows you to specify the number of decimals to print when printing tables or variables.
INTERACTIVE FUNCTION
In each print screen, the various required parameters are specified.
One of these parameters is "NbDec". A positive integer specifies the number of decimals that will be printed. If the value -1 is specified, the number of decimals is that of the internal definition of the value to be printed (usually arbitrary).
NON-INTERACTIVE FUNCTION
The number of decimals is passed as a parameter to the command:
Syntax
    $PrintNbDec  nb
Example
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

This function allows you to specify the print language when printing tables. The default language is English.
INTERACTIVE FUNCTION
In each print screen, the various required parameters are specified. The "Language" field is a MENU field in which you specify the print language.
NON-INTERACTIVE FUNCTION
The print language is passed as a parameter to the command:
Syntax
    $PrintLang {English | French | Dutch}
Example
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

Allows you not to clear the a2m file before printing. By default, the result files of print jobs are cleared before a new print. Only for a2m files, it is possible to avoid this default behavior and thus accumulate several successive print jobs in a single file.
This function also modifies the corresponding fields in the "Print Preferences" panel.
INTERACTIVE FUNCTION
This option can be modified in the "Print Preferences/A2M" panel, accessible in the "Print Setup" function.
NON-INTERACTIVE FUNCTION
Syntax
    $PrintA2mAppend [NO|Yes]
Example
    $PrintA2mAppend Yes  
    $PrintDest test.a2m A2M
These two calls define the file test.a2m as the result file for subsequent print jobs and do not clear this file.

.. _printfont:

COMMAND PRINTFONT
^^^^^^^^^^^^^^^^^

Sets the font for printing text paragraphs. In addition to the family, the base size (in points) and the size increment can also be specified.
The base size corresponds to text paragraphs, the increment indicates by how many points each title level should be increased.
This function also modifies the corresponding fields in the "Print Preferences" panel.
This value only has an effect before the $PrintDest function and not during the report.
INTERACTIVE FUNCTION
This option can be modified in the "Print Preferences" panel, accessible in the "Print Setup" function.
NON-INTERACTIVE FUNCTION
Syntax
   $PrintFont Times|Helvetica|Courier|Bookman|Palatino [size [incr]]  
 
The Bookman and Palatino fonts are not available for all print destinations.
The first character of the font name is sufficient.
Once this value is set, it remains in effect for all subsequent print jobs, including in subsequent reports. As it is saved in the print configuration panel, it also remains valid for subsequent IODE sessions.
Example
   $PrintFont Times 10 2
Sets the size of paragraphs par_1, enum_1, etc. to 10 points, paragraph tit_2 to 12 points, tit_1 to 14 points, and tit_0 to 18 points.

.. _printtablefont:

COMMAND PRINTTABLEFONT
^^^^^^^^^^^^^^^^^^^^^^

Sets the font for tables only. This font can therefore be different from that of the rest of the text, both in size and family.
This function also modifies the corresponding fields in the "Print Preferences" panel.
This function only has an effect before the $PrintDest function and not during the report.
INTERACTIVE FUNCTION
This option can be modified in the "Print Preferences" panel, accessible in the "Print Setup" function. Each destination can have a different value.
NON-INTERACTIVE FUNCTION
Syntax
   $PrintTableFont Times|Helvetica|Courier|Bookman|Palatino [size]
Once this value is set, it remains in effect for all subsequent print jobs, including in subsequent reports. As it is saved in the print configuration panel, it also remains valid for subsequent IODE sessions.
Example
   $PrintTableFont Palatino 8
Sets the font for all tables to Palatino, 8 points.

.. _printtablebox:

COMMAND PRINTTABLEBOX
^^^^^^^^^^^^^^^^^^^^^

Sets the border width of tables in points. This value can be zero to avoid borders.
This function also modifies the corresponding fields in the "Print Preferences" panel.
INTERACTIVE FUNCTION
This option can be modified in the "Print Preferences" panel, accessible in the "Print Setup" function. Each destination can have a different value.
NON-INTERACTIVE FUNCTION
Syntax
   $PrintTableBox  n
Once this value is set, it remains in effect for all subsequent print jobs, including in subsequent reports. As it is saved in the print configuration panel, it also remains valid for subsequent IODE sessions.
Example
   $PrintTableBox  0
Removes the border from tables.

.. _printtablecolor:

COMMAND PRINTTABLECOLOR
^^^^^^^^^^^^^^^^^^^^^^^

Allows you to use or not use color in tables.
This function also modifies the corresponding fields in the "Print Preferences" panel.
INTERACTIVE FUNCTION
This option can be modified in the "Print Preferences" panel, accessible in the "Print Setup" function. Each destination can have a different value.
NON-INTERACTIVE FUNCTION
Syntax
   $PrintTableColor [NO|Yes]
Once this value is set, it remains in effect for all subsequent print jobs, including in subsequent reports. As it is saved in the print configuration panel, it also remains valid for subsequent IODE sessions.
Example
   $PrintTableColor No
Removes color from table printing.

.. _printtablewidth:

COMMAND PRINTTABLEWIDTH
^^^^^^^^^^^^^^^^^^^^^^^

Sets the width of tables in Frame and Rtf. The parameters must be expressed in mm. Three values can be passed:
-  the total width
-  the width of the first column
-  the width of the following columns
This function also modifies the corresponding fields in the "Print Preferences" panel.
INTERACTIVE FUNCTION
This option can be modified in the "Print Preferences" panel, accessible in the "Print Setup" function. Each destination can have a different value.
NON-INTERACTIVE FUNCTION
Syntax
   $PrintTableWidth width [col1 [coln]]
Once this value is set, it remains in effect for all subsequent print jobs, including in subsequent reports. As it is saved in the print configuration panel, it also remains valid for subsequent IODE sessions.
Example
   $PrintTableWidth 160 60 15

.. _printtablebreak:

COMMAND PRINTTABLEBREAK
^^^^^^^^^^^^^^^^^^^^^^^

Allows or disallows tables to be split across multiple pages: if a table is placed on the page so that it must be printed on two pages, an automatic page break is generated before the table.
This function also modifies the corresponding fields in the "Print Preferences" panel.
INTERACTIVE FUNCTION
This option can be modified in the "Print Preferences" panel, accessible in the "Print Setup" function. Each destination can have a different value.
NON-INTERACTIVE FUNCTION
Syntax
   $PrintTableBreak [NO|Yes]
Once this value is set, it remains in effect for all subsequent print jobs, including in subsequent reports. As it is saved in the print configuration panel, it also remains valid for subsequent IODE sessions.
Example
   $PrintTableBreak Yes
Table splitting is allowed during printing.

.. _printtablepage:

COMMAND PRINTTABLEPAGE
^^^^^^^^^^^^^^^^^^^^^^

Forces a page break before each table.
This function also modifies the corresponding fields in the "Print Preferences" panel.
INTERACTIVE FUNCTION
This option can be modified in the "Print Preferences" panel, accessible in the "Print Setup" function. Each destination can have a different value.
NON-INTERACTIVE FUNCTION
Syntax
   $PrintTablePage  [NO|Yes]
Once this value is set, it remains in effect for all subsequent print jobs, including in subsequent reports. As it is saved in the print configuration panel, it also remains valid for subsequent IODE sessions.
Example
   $PrintTablePage Yes
Forces a page break for each table.

.. _printbackground:

COMMAND PRINTBACKGROUND
^^^^^^^^^^^^^^^^^^^^^^^

Sets the background color (tables and charts). A table will be hatched if the value of the "Shadow" graphic attribute is set to Yes.
To avoid backgrounds in all tables, you can use White as the background.
This function also modifies the corresponding fields in the "Print Preferences" panel.
INTERACTIVE FUNCTION
This option can be modified in the "Print Preferences" panel, accessible in the "Print Setup" function. Each destination can have a different value.
NON-INTERACTIVE FUNCTION
Syntax
   $PrintBackground Black|Blue|Magenta|Cyan|Red|Green|Yellow|White
Once this value is set, it remains in effect for all subsequent print jobs, including in subsequent reports. As it is saved in the print configuration panel, it also remains valid for subsequent IODE sessions.
Example
   $PrintBackground Blue
The color of the hatched background in charts and tables is set to blue.

.. _printgraphbox:

COMMAND PRINTGRAPHBOX
^^^^^^^^^^^^^^^^^^^^^

Sets the border width of charts in points.
This function also modifies the corresponding fields in the "Print Preferences" panel.
INTERACTIVE FUNCTION
This option can be modified in the "Print Preferences" panel, accessible in the "Print Setup" function. Each destination can have a different value.
NON-INTERACTIVE FUNCTION
Syntax
   $PrintGraphBox n
Once this value is set, it remains in effect for all subsequent print jobs, including in subsequent reports. As it is saved in the print configuration panel, it also remains valid for subsequent IODE sessions.
Example
   $PrintGraphBox 2
Subsequent charts will have a thick border.

.. _printgraphbrush:

COMMAND PRINTGRAPHBRUSH
^^^^^^^^^^^^^^^^^^^^^^^

Sets the background density of charts. The value is expressed as a percentage.
This function also modifies the corresponding fields in the "Print Preferences" panel.
INTERACTIVE FUNCTION
This option can be modified in the "Print Preferences" panel, accessible in the "Print Setup" function. Each destination can have a different value.
NON-INTERACTIVE FUNCTION
Syntax
   $PrintGraphBrush pct|Yes
Once this value is set, it remains in effect for all subsequent print jobs, including in subsequent reports. As it is saved in the print configuration panel, it also remains valid for subsequent IODE sessions.
Example
   $PrintGraphBrush 10
The density of the hatch "brush" is set to 10%. The color is determined by the $PrintBackground function.

.. _printgraphsize:

COMMAND PRINTGRAPHSIZE
^^^^^^^^^^^^^^^^^^^^^^

Sets the size of charts in mm and the font size used.
This function also modifies the corresponding fields in the "Print Preferences" panel.
INTERACTIVE FUNCTION
This option can be modified in the "Print Preferences" panel, accessible in the "Print Setup" function. Each destination can have a different value.
NON-INTERACTIVE FUNCTION
Syntax
   $PrintGraphSize  width [height] [fontsize]
where:
- width  and height  are expressed in mm.
- fontsize  is expressed in points
Once this value is set, it remains in effect for all subsequent print jobs, including in subsequent reports. As it is saved in the print configuration panel, it also remains valid for subsequent IODE sessions.
Example
   $PrintGraphSize 150 60 8
Subsequent charts will occupy a frame of 15 cm by 6 cm and the text will be 8 points in size.

.. _printgraphpage:

COMMAND PRINTGRAPHPAGE
^^^^^^^^^^^^^^^^^^^^^^

Forces a page break before each chart.
This function also modifies the corresponding fields in the "Print Preferences" panel.
INTERACTIVE FUNCTION
This option can be modified in the "Print Preferences" panel, accessible in the "Print Setup" function. Each destination can have a different value.
NON-INTERACTIVE FUNCTION
Syntax
   $PrintGraphPage [NO|Yes]
Once this value is set, it remains in effect for all subsequent print jobs, including in subsequent reports. As it is saved in the print configuration panel, it also remains valid for subsequent IODE sessions.
Example
   $PrintGraphPage Yes
Each subsequent chart will be placed at the beginning of a page.

.. _printrtfhelp:

COMMAND PRINTRTFHELP
^^^^^^^^^^^^^^^^^^^^

Generates an RTF file for Windows help. This file can then be compiled using Microsoft's HCW compiler.
The RTF files generated in this way are not identical to those generated for WinWord.
NON-INTERACTIVE FUNCTION
Syntax
    $PrintRtfHelp [YES|No]
Once this value is set, it remains in effect for all subsequent print jobs, including in subsequent reports. As it is saved in the print configuration panel, it also remains valid for subsequent IODE sessions.
Example
    $PrintRtfHelp YES
The next file defined by $PrintDest ... RTF will be of the Windows Help type.

.. _printrtftopic:

COMMAND PRINTRTFTOPIC
^^^^^^^^^^^^^^^^^^^^^

Creates a new topic for a Windows help file (or a2m). This topic will be part of the automatic table of contents generated by IODE. The hierarchical level of the topic is set by the $PrintRtfLevel command.
The topic text will also be used as the title.
INTERACTIVE FUNCTION
There is no equivalent interactive function.
NON-INTERACTIVE FUNCTION
Syntax
    $PrintRtfTopic topic title
Example
    $PrintRtfTopic Summary Tables
Generates in a2m (see a2m language syntax):
     .topic [auto_number] [level] Summary Tables  
     .par1 tit_[level]  
     Summary Tables  

If the generated file is an a2m file, the .topic command is only used in the case of a translation to Windows Help. However, .par1 tit_.... and the topic text will generate a title when printed.

.. _printrtflevel:

COMMAND PRINTRTFLEVEL
^^^^^^^^^^^^^^^^^^^^^

Changes the hierarchical level of the following topics. This level change allows indentations to be generated in the resulting help file's table of contents.
INTERACTIVE FUNCTION
There is no equivalent interactive function.
NON-INTERACTIVE FUNCTION
Syntax
    $PrintRtfLevel [+|-|n]
Example
    $PrintRtfLevel +
The level of the next topic will be incremented by 1, thus creating an indentation in the generated table of contents.

.. _printrtftitle:

COMMAND PRINTRTFTITLE
^^^^^^^^^^^^^^^^^^^^^

Sets the title of the Windows help file.
This function also modifies the corresponding fields in the "Print Preferences/RTF" panel.
INTERACTIVE FUNCTION
This option can be modified in the "Print Preferences" panel, accessible in the "Print Setup" function.
NON-INTERACTIVE FUNCTION
Syntax
    $PrintRtfTitle help title
Example
    $PrintRtfTitle Hermes Model

.. _printrtfcopyright:

COMMAND PRINTRTFCOPYRIGHT
^^^^^^^^^^^^^^^^^^^^^^^^^

Sets the copyright text for Windows help. This text appears in the Help option of the Winhelp program.
This function also modifies the corresponding fields in the "Print Preferences/RTF" panel.
INTERACTIVE FUNCTION
This option can be modified in the "Print Preferences" panel, accessible in the "Print Setup" function.
NON-INTERACTIVE FUNCTION
Syntax
    $PrintRtfCopyright copyright text
Example
    $PrintRtfCopyright (c) Federal Planning Bureau - 1998

.. _printhtmlhelp:

COMMAND PRINTHTMLHELP
^^^^^^^^^^^^^^^^^^^^^

Specifies that the next generation of Html file should be for Windows HtmlHelp. This file can then be compiled using Microsoft's HHC compiler.
INTERACTIVE FUNCTION
This function does not exist in interactive form.
NON-INTERACTIVE FUNCTION
Syntax
    $PrintHtmlHelp [YES|No]
Example
    $PrintHtmlHelp YES  
    $A2mToHtml myfile.a2m myfile.htm

.. _printhtmlstrip:

COMMAND PRINTHTMLSTRIP
^^^^^^^^^^^^^^^^^^^^^^

Specifies that the next generation of Html file should not include the HTML header and footer sections (<HEAD>, <BODY>, </BODY>, etc). Only the actual text is translated for later manual inclusion in an HTML file.
INTERACTIVE FUNCTION
This function does not exist in interactive form.
NON-INTERACTIVE FUNCTION
Syntax
    $PrintHtmlStrip [YES|No]
Example
    $PrintHtmlStrip YES  
    $PrintDest test.htm HTML  
    $PrintTbl ....

.. _printparanum:

COMMAND PRINTPARANUM
^^^^^^^^^^^^^^^^^^^^

Allows numbering of headings when printing (1., 1.1, ...).
This function also modifies the corresponding fields in the "Print Preferences" panel.
This function only has an effect before the $PrintDest function and not during the report.
INTERACTIVE FUNCTION
This option can be modified in the "Print Preferences" panel, accessible in the "Print Setup" function. Each destination can have a different value.
NON-INTERACTIVE FUNCTION
Syntax
    $PrintParanum NO  
    $PrintParanum [NO|Yes]
Example
    $PrintParanum NO
Removes automatic numbering of headings.

.. _printpageheader:

COMMAND PRINTPAGEHEADER
^^^^^^^^^^^^^^^^^^^^^^^

Sets the title of printed pages. The characters %d are replaced by the current page number.
This function also modifies the corresponding fields in the "Print Preferences" panel.
INTERACTIVE FUNCTION
This option can be modified in the "Print Preferences/Printer" panel, accessible in the "Print Setup" function.
NON-INTERACTIVE FUNCTION
Syntax
    $PrintPageHeader title for the following pages
Example
    $PrintPageHeader Modtrim Model

.. _printpagefooter:

COMMAND PRINTPAGEFOOTER
^^^^^^^^^^^^^^^^^^^^^^^

Sets the footnote of printed pages. The characters %d are replaced by the current page number.
This function also modifies the corresponding fields in the "Print Preferences" panel.
INTERACTIVE FUNCTION
This option can be modified in the "Print Preferences/Printer" panel, accessible in the "Print Setup" function.
NON-INTERACTIVE FUNCTION
Syntax
    $PrintPageFooter  footnote for the following pages
Example
    $PrintPageFooter - Page %d -

.. _setprinter:

COMMAND SETPRINTER
^^^^^^^^^^^^^^^^^^

Sets the default Windows printer for subsequent print jobs.
INTERACTIVE FUNCTION
This option is modified in Windows (Control Panel).
NON-INTERACTIVE FUNCTION
Syntax
    $SetPrinter printer_name
Example
    $SetPrinter 6_qms

.. _printorientation:

COMMAND PRINTORIENTATION
^^^^^^^^^^^^^^^^^^^^^^^^

Sets the orientation of the current Windows printer.
This function also modifies the corresponding fields in the "Print Preferences" panel.
INTERACTIVE FUNCTION
This option can be modified in the "Print Preferences/Printer" panel, accessible in the "Print Setup" function.
NON-INTERACTIVE FUNCTION
Syntax
    $PrintOrientation {Portrait|Landscape}
Example
    $PrintOrientation Portrait

.. _printduplex:

COMMAND PRINTDUPLEX
^^^^^^^^^^^^^^^^^^^

Sets the duplex (double-sided) mode of the current Windows printer.
This function also modifies the corresponding fields in the "Print Preferences" panel.
INTERACTIVE FUNCTION
This option can be modified in the "Print Preferences/Printer" panel, accessible in the "Print Setup" function.
NON-INTERACTIVE FUNCTION
Syntax
    $PrintDuplex {Simplex|Duplex|VerticalDuplex}
Example
    $PrintDuplex Duplex

.. _printgifbackcolor:

COMMAND PRINTGIFBACKCOLOR
^^^^^^^^^^^^^^^^^^^^^^^^^

Sets the background color of GIF charts generated when creating an HTML document.
This function also modifies the corresponding fields in the "Print Preferences/GIF" panel.
INTERACTIVE FUNCTION
This option can be modified in the "Print Preferences" panel, accessible in the "Print Setup" function. Each destination can have a different value.
NON-INTERACTIVE FUNCTION
Syntax
    $PrintGIFBackColor {Black|Blue|Magenta|Cyan|Red|Green|Yellow|White}
Example
    $PrintGIFBackColor Yellow

.. _printgiftranscolor:

COMMAND PRINTGIFTRANSCOLOR
^^^^^^^^^^^^^^^^^^^^^^^^^^

Sets the color considered as "transparent" in GIF files generated when creating an HTML document. This allows one of the colors to blend with the background. The "Transparent" option must be set by the $PrintGIFTransparent Yes command.
This function also modifies the corresponding fields in the "Print Preferences/GIF" panel.
INTERACTIVE FUNCTION
This option can be modified in the "Print Preferences" panel, accessible in the "Print Setup" function. Each destination can have a different value.
NON-INTERACTIVE FUNCTION
Syntax
    $PrintGIFTransColor {Black|Blue|Magenta|Cyan|Red|Green|Yellow|White}
Example
    $PrintGIFTransColor White

.. _printgiftransparent:

COMMAND PRINTGIFTRANSPARENT
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Indicates whether GIF files generated when creating an HTML document should be in "transparent" format. This allows one of the colors (to be specified by the $PrintGIFTransColor command) to blend with the background.
This function also modifies the corresponding fields in the "Print Preferences/GIF" panel.
INTERACTIVE FUNCTION
This option can be modified in the "Print Preferences" panel, accessible in the "Print Setup" function. Each destination can have a different value.
NON-INTERACTIVE FUNCTION
Syntax
    $PrintGIFTransparent {Yes|No}
Example
    $PrintGIFTransparent No

.. _printgifinterlaced:

COMMAND PRINTGIFINTERLACED
^^^^^^^^^^^^^^^^^^^^^^^^^^

Indicates whether GIF files generated when creating an HTML document should be in interlaced format.
This function also modifies the corresponding fields in the "Print Preferences/GIF" panel.
INTERACTIVE FUNCTION
This option can be modified in the "Print Preferences" panel, accessible in the "Print Setup" function. Each destination can have a different value.
NON-INTERACTIVE FUNCTION
Syntax
    $PrintGIFInterlaced {Yes|No}
Example
    $PrintGIFInterlaced No

.. _printgiffilled:

COMMAND PRINTGIFFILLED
^^^^^^^^^^^^^^^^^^^^^^

Indicates whether to fill bars in bar charts of GIF graphics.
This function also modifies the corresponding fields in the "Print Preferences/GIF" panel.
INTERACTIVE FUNCTION
This option can be modified in the "Print Preferences" panel, accessible in the "Print Setup" function. Each destination can have a different value.
NON-INTERACTIVE FUNCTION
Syntax
    $PrintGIFFilled {Yes|No}
Example
    $PrintGIFFilled No

.. _printgiffont:

COMMAND PRINTGIFFONT
^^^^^^^^^^^^^^^^^^^^

Specifies the font to use when creating GIF graphics. The available fonts are:
- 0 :  Tiny
- 1 :  Small
- 2 :  MediumBold
- 3 :  Large
- 4 :  Giant
- 5 :  Grayed
This function also modifies the corresponding fields in the "Print Preferences/GIF" panel.
INTERACTIVE FUNCTION
This option can be modified in the "Print Preferences" panel, accessible in the "Print Setup" function. Each destination can have a different value.
NON-INTERACTIVE FUNCTION
Syntax
    $PrintGIFFont FontNb (between 0 and 5)
Example
    $PrintGIFFont 1


Object printing
~~~~~~~~~~~~~~~

The following commands are used to print object definitions:

- :ref:`printobjdef <printobjdef>` : prints a definition of an object cmt / eqs / idt / lst / scl / tbl / var
- :ref:`printobjtitle <printobjtitle>` : specifies whether only the titles should be printed
- :ref:`printobjlec <printobjlec>` : specifies the type of equation to print
- :ref:`printobjinfos <printobjinfos>` : specifies the information to print

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

Identities executions 
~~~~~~~~~~~~~~~~~~~~~

The following commands allow you to execute *identities*:

- :ref:`idtexecute <idtexecute>`
- :ref:`idtexecutetrace <idtexecutetrace>`
- :ref:`idtexecutevarfiles <idtexecutevarfiles>`
- :ref:`idtexecutesclfiles <idtexecutesclfiles>`

Report operations 
~~~~~~~~~~~~~~~~~~

The following commands allow you to manipulate reports (these functions are recursive):

- :ref:`reportexec <reportexec>`
- :ref:`reportedit <reportedit>`

A2M file translation 
~~~~~~~~~~~~~~~~~~~~

The following functions allow you to translate A2M files into different formats:

- :ref:`A2mToHtml <a2mtohtml>` : translation to HTML
- :ref:`A2mToRtf <a2mtortf>` : translation to Word (Rtf)
- :ref:`A2mToMif <a2mtomif>` : translation to Frame (MIF)
- :ref:`A2mToCsv <a2mtocsv>` : translation to CSV format
- :ref:`A2mToPrinter <a2mtoprinter>` : interprets and prints an A2M file

Other report functions 
~~~~~~~~~~~~~~~~~~~~~~

- :ref:`StatUnitRoot <statunitroot>` : Dickey-Fuller tests
- :ref:`WsAggrChar <wsaggrchar>` : sets the character to introduce in the code of series created by `WsAggr***`
- :ref:`WsAggrSum <wsaggrsum>` : computes the sum of series
- :ref:`WsAggrMean <wsaggrmean>` : computes the mean of series
- :ref:`WsAggrProd <wsaggrprod>` : computes the product of series

Python equivalent functions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To get the Python equivalent of the above IODE commands, refers to the 
:ref:`Equivalence Between IODE Report Commands and Python Methods <report_commands_equivalence>` 
tables.
