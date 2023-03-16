
<IODE: Execution of identities>
..sep |
..esc ~
..ignore ¯

IODE: execution of identities
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
&TI Introduction
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
IODE identities are simple LEC expressions. Each identity has a name which is the name of the variable that 
will be created or updated by the "execution" (i.e. calculation) of that identity. 

&TI Order of execution
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
The order of execution of a set of identities is computed by IODE in such a way 
that the "dependent" identities are calculated after their predecessors.

&IT Example
¯¯¯¯¯¯¯¯¯¯¯¯
Suppose we have 3 identities to execute:
..tb    
| Idt name   | Formula
| ~cA          | ~cB+C
| ~cB          | ~cC * 2 * alpha
| ~cE          | ~cX + Y
..te

The order of execution is automatically calcuted =>> [1, 0, 2].
Indeed, B must be computed before A because A is a successor of B. E remains in 3d position.

&TI VAR and SCL source files
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯


The variables and scalars needed to compute the identities are read either in the current WS or 
in a set of external files. 

In the example above, the variables X, Y and C, and the scalar alpha, are needed to perform the calculation. 
If no information on external files are given, X, Y and C are searched in the current KV_WS and, similarly, alpha in KS_WS.
If one of them cannot be found, the process stops with an error.



If these objects are not present in KV_WS / KS_WS, the user must thus provide one or more source files. 
Note the order in which these files are specified is important because as soon as an object is found, 
it is not searched in the following files.

To specify that an object must be searched in the current WS before the other files, the special name ~cWS~C must be given.


Let's say that 
&EN X and C are in the current WS, 
&EN X and Y can be found in filexy.var 
&EN alpha is in filealpha.scl.

If we provide as VAR source files ~c"WS filexy.var"~C:

..tb
| ~bName     | ~bFormula            | Source
| ~cA          | ~cB+C              | C:WS, B:calculated
| ~cB          | ~cC * 2 * alpha    | C:WS
| ~cE          | ~cX + Y            | ~bX:WS~B, Y:filexy
..te                                

&EN ~bX~B and C will be read in WS, 
&EN Y will be read in filexy


On the other hand, if we provide as VAR source files ~c"filexy.var"~C without ~c"WS"~C, C cannot be found and 
the execution will stop.

Finally, if we provide as VAR source files ~c"filexy.var WS"~C:

..tb
| ~bName     | ~bFormula            | Source
| ~cA          | ~cB+C              | C:WS, B:calculated
| ~cB          | ~cC * 2 * alpha    | C:WS
| ~cE          | ~cX + Y            | ~bX:filexy~B and Y: filexy
..te                                

&EN ~bX and Y~B will be read in filexy.var
&EN C will be read in WS, 

The same principle applies to scalar source files.

&TI Execution sample
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
When VARs are read from external files, they are shortened or expanded to match the current WS sample.

On the other hand, one can execute identities on a sub-sample. In this case, there are 2 possibilities:
&EN if a calculated VAR already exists in the current WS, its values are left unchanged outside the calculation sample
&EN if the VAR is created, the values outside the calculation sample are set to L_NAN.
 

&TI List of functions
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
| Syntax                                 | Description  
| ~cKDB *KI_exec(KDB* dbi, KDB* dbv, int nv, char* vfiles[], KDB* dbs, int ns, char* sfiles[], SAMPLE* smpl) | Executes all identities in dbi using the input series of dbv and scalars of dbs. Missing vars and scalars are collected from vfiles and sfiles.
..te
>