<IODE: Reports>
IODE: Reports
¯¯¯¯¯¯¯¯¯¯¯¯
..sep |
..esc ~
..ignore ¯

&EN <Example of report with @-functions>
&EN <Example of report with $procdef and $foreach>

>

<Example of report with @-functions>
Example of report with @-functions
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯

&TI Report
¯¯¯¯¯¯¯¯¯
&CO
    $ `@-functions()` tests and examples
    $ --------------------------------
    $ Syntax: rep_fns input_dir output_dir
    $ Example: rep_fns data output

    $debug 1
    $indent
    $goto continue {%0% >>= 2}
        $show Syntax: rep_fns input_dir output_dir
        $show Syntax error. Report aborted.
        $return
    $label continue
    $define input_dir %1%
    $define output_dir %2%
    $label continue

    $PrintDest %output_dir%\rep_fns.a2m A2M

    STRINGS
    =======

    `@take()`
    `-------`
        Success
            `@take(2,ABCDE)`        =>> "@take(2,ABCDE)"
            `@take(-2,ABCDE)`       =>> "@take(-2,ABCDE)"
            `@take(2, ABCDE)`       =>> "@take(2, ABCDE)"
            `@take(0,ABCDE)`        =>> "@take(0,ABCDE)"
            `@take(2,"A,B,C,D,E")`  =>> "@take(2,"A,B,C,D,E")"

        Errors (##parms <<>> 2)
            `@take(2,A,B,C,D,E)`    =>> "@take(2,A,B,C,D,E)"
            `@take(2,A,BCDE)`       =>> "@take(2,A,BCDE)"

    `@drop()`
    `-------`
        Success
            `@drop(2,ABCDE)      `  =>> "@drop(2,ABCDE)"
            `@drop(-1,ABCDE)     `  =>> "@drop(-1,ABCDE)"
            `@drop(2, ABCDE)     `  =>> "@drop(2, ABCDE)"
            `@drop(0,ABCDE)      `  =>> "@drop(0,ABCDE)"
            `@drop(2,"A,B,C,D,E")`  =>> "@drop(2,"A,B,C,D,E")"
            `@DROP(2,A)          `  =>> "@DROP(2,A)"
            `@drop(3, "a bc")    `  =>> "@drop(3, "a bc")"

        Error (##parms <<>> 2)
            `@drop(2,A,B,C,D,E)`    =>> "@drop(2,A,B,C,D,E)"
            `@drop(2,A,BCDE)`       =>> "@drop(2,A,BCDE)"
            `@drop(n,ABCDE)`        =>> "@drop(n,ABCDE)"

    `@replace()`
    `----------`
        Success
            `@replace(Buro du Plan,Buro,Bureau)`            =>> @replace(Buro du Plan,Buro,Bureau)
            `@replace(Buro du Buro du Plan,Buro,Bureau)`    =>> @replace(Buro du Buro du Plan,Buro,Bureau)
            `@replace(Bureauplan,plan, du Plan)`            =>> @replace(Bureauplan,plan, du Plan)
            `@replace("Bureau, du Plan",",","") `           =>> @replace("Bureau, du Plan",",","")
            `@replace("Bureau, du Plan", ",", "") `         =>> @replace("Bureau, du Plan", ",", "")

        Error
            `@replace(Buro plan,Bureau du Plan)`            =>> @replace(Buro plan,Bureau du Plan)
            `@replace(AB,B,C,D)`                            =>> @replace(AB,B,C,D)

    `@equal()`
    `--------`
            `@equal(aa,aa)      `   =>>  "@equal(aa,aa)"
            `@equal(aa, aa)     `   =>>  "@equal(aa, aa)"
            `@equal(aa,bb)      `   =>>  "@equal(aa,bb)"
            `@equal(aa,aa,aa)   `   =>>  "@equal(aa,aa,aa)"

    `@upper()`
    `-------
            `@upper(ab cd,ef)    `  =>>  "@upper(ab cd,ef)"
            `@upper(AB)          `  =>>  "@upper(AB)"
            `@upper("abc", "def")`  =>>  "@upper("abc", "def")"
            `@upper("abc","def") `  =>>  "@upper("abc","def")"

    `@lower()`
    `-------
            `@lower(ab cd,ef)    `  =>>  "@lower(ab cd,ef)"
            `@lower(AB)          `  =>>  "@lower(AB)"
            `@lower("abc", "def")`  =>>  "@lower("abc", "def")"
            `@lower("abc","def") `  =>>  "@lower("abc","def")"

    `@sqz()`
    `------`
            `@sqz( ab    cd )`      =>>  "@sqz( ab    cd )"
            `@sqz(" ab    cd ")`    =>>  "@sqz(" ab    cd ")"
            `@sqz( ab,"  cd", ef)`  =>>  "@sqz( ab,"  cd", ef)"
            `@sqz( ab, " cd", ef)`  =>>  "@sqz( ab, " cd", ef)"

    `@strip()`
    `--------`
            `@strip( ab    cd )`      =>>  "@strip( ab    cd )"
            `@strip(" ab    cd ")`    =>>  "@strip(" ab    cd ")"
            `@strip( ab,"  cd", ef)`  =>>  "@strip( ab,"  cd", ef)"
            `@strip( ab, " cd", ef)`  =>>  "@strip( ab, " cd", ef)"


    `@fmt()  `
    `--------`
            `@fmt(123456)              ` ->> "@fmt(123456)"
            `@fmt(123456,99999999)     ` ->> "@fmt(123456,99999999)"
            `@fmt(123456,999)          ` ->> "@fmt(123456,999)"
            `@fmt(123.456,999999)      ` ->> "@fmt(123.456,999999)"
            `@fmt(123456,00999999)     ` ->> "@fmt(123456,00999999)"
            `@fmt(123456,"ABC999,999Z")` ->> "@fmt(123,"ABC999,999Z")"
            `@fmt(123456,ABC999999Z)   ` ->> "@fmt(123,ABC999999Z)"


    `@ansi()        `
    `--------`
            `@ansi(àâäéèêëîïöôùç)   `    ->> "@ansi(àâäéèêëîïöôùç)"




&TX
&TI Resulting a2m file
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
&CO
    STRINGS
    =======

    @void()
    -------
        @void(ABC,DEF,G)          =>>  ""
        @void(@getdir())          =>>  ""
        @void(@vexpand(*))        =>>  ""
&TX
>

<Example of report with $procdef and $foreach>
&TX
Example of report with $procdef and $foreach

&TI Report
&CO
    $ Testing various procedures
    $ --------------------------
    $
    $  Syntax: rep_proc input_dir output_dir
    $
    $  Example: rep_proc data output
    $
    $debug 1
    $indent
    $goto continue {%0% >>= 2}
        $show Syntax: rep_proc input_dir output_dir
        $show Syntax error. Report aborted.
        $return
    $label continue

    $define input_dir %1%
    $define output_dir %2%
    $ Define output destination
    $ -------------------------
    $PrintDest %output_dir%\rep_proc.a2m A2M

    $ ----------------------------------------------------------
    $ printtbl: prints a list of variables on a specific period
    $procdef printtbl year vars
        $foreach var %vars%
            %var%[%year%] = {%var%[%year%]}
        $next var
    $procend

    $ ----------------------------------------------------------
    $ sumvars: creates a new series names %result% = sum of %vars%
    $procdef sumvars result vars
        $define eq 0
        $foreach var %vars%
            $define eq %eq% + %var%
        $next var
        $DataCalcVar %result% %eq%
    $procend

    $ ----------------------------------------------------------
    $ sumvarsmsg: displays msg, then calls sumvars result vars
    $procdef sumvarsmsg msg result vars
        %msg%
        $ During the call : result = '%result%' vars = '%vars%'
        $procexec sumvars %result% %vars%
        %result%[1992Y1] = {%result%[1992Y1]}
    $procend

    $WsSample 1990Y1 2000Y1
    $DataCalcVar PIB_A t
    $DataCalcVar PIB_B ln t
    $DataCalcVar PIB_C sin(t)
    $procexec sumvarsmsg "Hello IODE!" PIB PIB_A PIB_B
    $procexec sumvarsmsg "Hello World" PIB3 PIB_A PIB_C
    $procexec printtbl 1992Y1 PIB_A PIB_B PIB_C PIB PIB3

    $ Load objects
    $ ------------
    $WsLoadCmt %input_dir%\fun
    $WsLoadEqs %input_dir%\fun
    $WsLoadIdt %input_dir%\fun
    $WsLoadLst %input_dir%\fun
    $WsLoadScl %input_dir%\fun
    $WsLoadTbl %input_dir%\fun
    $WsLoadVar %input_dir%\fun

    $procdef print_eq eqs
    .par1 tit_1 Equations with coefficients
        $PrintObjLec 1
        $foreach eq %eqs%
            $PrintObjDefEqs %eq%
        $next eq
    $procend
    $procexec print_eq ACAG ACAF

&TX
&TI Resulting a2m file
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
&CO
        Hello IODE!






        PIB[1992Y1] = 2.6931472

        Hello World






        PIB3[1992Y1] = 2.9092974




            PIB_A[1992Y1] = 2


            PIB_B[1992Y1] = 0.69314718


            PIB_C[1992Y1] = 0.90929743


            PIB[1992Y1] = 2.6931472


            PIB3[1992Y1] = 2.9092974



    .par1 tit_1 Equations with coefficients



    .par1 enum_1

    \bACAG\B  :=  \bACAG\B[-1 ] + r \iVBBP\I[-1 ] + (0.006*\iVBBP\I[-1 ]*(\iTIME\I= 2001) -0.008*(\iTIME\I= 2008) )


    .par1 enum_1

    (\bACAF\B/\iVAF\I[-1 ])   := \i0.015768407\I + \i-7.961502e-06\I*\iGOSF\I[-1 ] +
    \i-0.0085051842\I*(\iTIME\I= 1995)

&TX

>