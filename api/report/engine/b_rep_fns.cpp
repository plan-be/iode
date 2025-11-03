/**
 *  @header4iode
 * 
 *  IODE report @-functions implementation 
 *  --------------------------------------
 *  When an @function() is found in a report line, it is replaced in-place by the 
 *  result of its calculated value.  
 *  
 *  Parameters specifications: 
 *      - parameters are separated by commas.
 *      - spaces are regular characters (and, if present, are included in the parameters)
 *      - parameters enclosed by double quotes "" may contain spaces or commas
 *  
 *  E.g.:
 *        "@take(2,ABCDE)"        => "AB"
 *  
 *  In case of error in the @function syntax, for example an illegal number of parameters,
 *  or if the @function does not exist, the text of the function remained unchanged 
 *  in the report line:
 *  
 *      Correct:
 *        "@take(2,ABCDE)"        => "AB"
 *        "@take(2,"A,BCDE")"     => "A,"
 *      Incorrect:
 *        "@take(2,A,BCDE)"       => "@take(2,A,BCDE)"
 *        "@takes(2,A)"           => "@takes(2,A)"
 *  
 *  Each function in this group has the same syntax:
 *  
 *      U_ch* <function_name>(U_ch** arg)
 *  
 *      @param [in] U_ch**   arg    List of arguments of the function (sep = ',')
 *      @return     U_ch*           allocated string containing the computed value of the function
 *                                  NULL on error
 *  To add a new @function:
 *      1. create the function based on an existing one. The syntax must be identical
 *      2. add the function name / pointer to the table RP_FNS[] (see at the end of the current file).
 *  
 *  More details can be found here : https://iode.plan.be/doku.php?id=les_fonctions_de_rapports
 *  
 *  List of functions
 *  -----------------
 *      U_ch *RPF_iode_version()
 *      U_ch *RPF_take(U_ch** args)
 *      U_ch *RPF_drop(U_ch** args)
 *      U_ch *RPF_replace(U_ch** args)
 *      U_ch *RPF_equal(U_ch** args)
 *      U_ch *RPF_upper(U_ch** args)
 *      U_ch *RPF_lower(U_ch** args)
 *      U_ch *RPF_sqz(U_ch** args)
 *      U_ch *RPF_strip(U_ch** args)
 *      U_ch *RPF_fmtint(U_ch** args)
 *      U_ch *RPF_ansi(U_ch** args)
 *      U_ch *RPF_count(U_ch** args)
 *      U_ch *RPF_index(U_ch** args)
 *      U_ch *RPF_void(U_ch **args)
 *      U_ch *RPF_date(U_ch** args)
 *      U_ch *RPF_time(U_ch** args)
 *      U_ch *RPF_month(U_ch** args)
 *      U_ch *RPF_sstderr(U_ch** args)
 *      U_ch *RPF_srelax(U_ch** args)
 *      U_ch *RPF_ttitle(U_ch** args)
 *      U_ch *RPF_cvalue(U_ch** args)
 *      U_ch *RPF_vvalue(U_ch** args)
 *      U_ch *RPF_lvalue(U_ch** args)
 *      U_ch *RPF_ivalue(U_ch** args)
 *      U_ch *RPF_evalue(U_ch** args)
 *      U_ch *RPF_eqsample(U_ch** args)
 *      U_ch *RPF_eqsamplefromto(U_ch** args, int fromto)
 *      U_ch *RPF_eqsamplefrom(U_ch **args)
 *      U_ch *RPF_eqsampleto(U_ch **args)
 *      U_ch *RPF_eqlhsrhs(U_ch** args, int lhsrhs)
 *      U_ch *RPF_eqlhs(U_ch **args)
 *      U_ch *RPF_eqrhs(U_ch **args)
 *      U_ch *RPF_sample(U_ch** args)
 *      int RPF_vsliste1(CLEC* cl, U_ch*** tbl, int* nb, int type)
 *      U_ch *RPF_vsliste(U_ch** args, int type)
 *      U_ch **RPF_unique(U_ch** tbl)
 *      U_ch *RPF_vliste(U_ch** args)
 *      U_ch *RPF_sliste(U_ch** args)
 *      U_ch *RPF_expand(U_ch** args, int type)
 *      U_ch *RPF_cexpand(U_ch **args)
 *      U_ch *RPF_eexpand(U_ch **args)
 *      U_ch *RPF_iexpand(U_ch **args)
 *      U_ch *RPF_lexpand(U_ch **args)
 *      U_ch *RPF_sexpand(U_ch **args)
 *      U_ch *RPF_texpand(U_ch **args)
 *      U_ch *RPF_vexpand(U_ch **args)
 *      int RPF_CalcPeriod(U_ch** args)
 *      U_ch *RPF_SimNorm(U_ch** args)
 *      U_ch *RPF_SimNIter(U_ch** args)
 *      U_ch *RPF_SimCpu(U_ch** args)
 *      U_ch *RPF_SimMaxit(U_ch** unused)
 *      U_ch *RPF_SimEps(U_ch** unused)
 *      U_ch *RPF_SimRelax(U_ch** unused)
 *      U_ch *RPF_SimSortNbPasses(U_ch** unused)
 *      U_ch *RPF_SimSortAlgo(U_ch** unused)
 *      U_ch *RPF_SimInitValues(U_ch** unused)
 *      U_ch *RPF_SimCpuSCC(U_ch** unused)
 *      U_ch *RPF_SimCpuSort(U_ch** unused)
 *      U_ch *RPF_vtake(U_ch** args)
 *      U_ch *RPF_vdrop(U_ch** args)
 *      U_ch *RPF_vcount(U_ch** args)
 *      U_ch *RPF_memory(U_ch** args)
 *      U_ch *RPF_ChronoReset(U_ch** unused)
 *      U_ch *RPF_ChronoGet(U_ch** unused)
 *      U_ch *RPF_fappend(U_ch** args)
 *      U_ch *RPF_fdelete(U_ch** args)
 *      U_ch *RPF_getdir(U_ch** unused)
 *      U_ch *RPF_chdir(U_ch **args)
 *      U_ch *RPF_mkdir(U_ch **args)
 *      U_ch *RPF_rmdir(U_ch **args)
 *      
 *  
 */
#include "scr4/s_prodir.h"
#include "scr4/s_prost.h"
#include "scr4/s_prodt.h"

#include "api/constants.h"
#include "api/k_super.h"
#include "api/objs/objs.h"
#include "api/objs/pack.h"
#include "api/objs/grep.h"
#include "api/objs/comments.h"
#include "api/objs/equations.h"
#include "api/objs/identities.h"
#include "api/objs/lists.h"
#include "api/objs/scalars.h"
#include "api/objs/variables.h"
#include "api/simulation/simulation.h"
#include "api/print/print.h"

#include "api/report/engine/engine.h"
#include "api/report/undoc/undoc.h"

#ifdef _MSC_VER
    #include <direct.h>
#else
    #include <unistd.h>
#endif
#include <time.h>

#include <algorithm>    // for std::min, std::max


/*--------------------------- STRING MANIPULATIONS -------------------------------*/

/**
 *  Syntax: @take(n,string)
 *  
 *  Note that if the first argument (n) is non numeric, it is interpreted as 0.
 *  The result is then empty (@take(0,...))
 *
 *  Examples 
 *  --------
 *   Success
 *       @take(2,ABCDE)        => "AB"
 *       @take(-2,ABCDE)       => "DE"
 *       @take(2, ABCDE)       => " A"
 *       @take(0,ABCDE)        => ""
 *       @take(2,"A,B,C,D,E")  => "A,"
 *   Errors (#parms <> 2)
 *       @take(2,A,B,C,D,E)    => "@take(2,A,B,C,D,E)"
 *       @take(2,A,BCDE)       => "@take(2,A,BCDE)" 
 */
U_ch *RPF_take(U_ch** args)
{
    U_ch    *res = 0;
    int     n, lg;

    if(SCR_tbl_size(args) != 2) return(res);
    n = atol((char*) args[0]);
    lg = (int)strlen((char*) args[1]);
    if(n > 0) {
        res = (unsigned char*) SCR_malloc(n + 1);
        if(n >= lg) {
            strcpy((char*) res, (char*) args[1]);
            memset(res + lg, ' ', n - lg);
            res[n] = 0;
            return(res);
        }
        else {
            memcpy(res, args[1], n);
            res[n] = 0;
            return(res);
        }
    }
    else {
        n = -n;
        res = (unsigned char*) SCR_malloc(n + 1);
        if(n >= lg) {
            memset(res, ' ', n - lg);
            strcpy(((char*) res) + n - lg, (char*) args[1]);
            return(res);
        }
        else {
            memcpy(res, args[1] + lg - n, n);
            res[n] = 0;
            return(res);
        }
    }
}


/**
 *  Syntax: @drop(n,string)
 *  
 *  Note that if the first argument (n) is non numeric, it is interpreted as 0.
 *  The result is the full parameter 2 (@drop(0,...))
 *  
 *  Examples 
 *  --------
 *     Success
 *         @drop(2,ABCDE)        => "CDE"
 *         @drop(-1,ABCDE)       => "ABCD"
 *         @drop(2, ABCDE)       => "BCDE"
 *         @drop(0,ABCDE)        => "ABCDE"
 *         @drop(2,"A,B,C,D,E")  => "B,C,D,E"
 *         @DROP(2,A)            => ""
 *         @drop(3, "a bc")      => " bc""
 *     Error (#parms <> 2)
 *         @drop(2,A,B,C,D,E)    => "@drop(2,A,B,C,D,E)"
 *         @drop(2,A,BCDE)       => "@drop(2,A,BCDE)"
 *         @drop(n,ABCDE)        => "ABCDE"
 */
U_ch *RPF_drop(U_ch** args)
{
    U_ch    *res = 0;
    int     n, lg;

    if(SCR_tbl_size(args) != 2) return(res);
    n = atol((char*) args[0]);
    lg = (int)strlen((char*) args[1]);
    if(lg == 0) return(res);

    if(n > 0) {
        if(n >= lg) return((unsigned char*) SCR_malloc(1));
        res = (unsigned char*) SCR_malloc(lg - n + 1);
        strcpy((char*) res, (char*) args[1] + n);
        return(res);
    }
    else {
        n = -n;
        if(n >= lg) return((unsigned char*) SCR_malloc(1));
        res = (unsigned char*) SCR_malloc(lg - n + 1);
        SCR_strlcpy(res, args[1], lg - n);
        return(res);
    }
}


/**
 *  Syntax: @replace(string, from, to)
 *  
 *  Examples 
 *  --------
 *     Success
 *         @replace(Buro du Plan,Buro,Bureau)            => Bureau du Plan
 *         @replace(Buro du Buro du Plan,Buro,Bureau)    => Bureau du Bureau du Plan
 *         @replace(Bureauplan,plan, du Plan)            => Bureau du Plan
 *         @replace("Bureau, du Plan",",","")            => Bureau du Plan
 *         @replace("Bureau, du Plan", ",", "")          => Bureau, du Plan 
 *     Error
 *         @replace(Buro plan,Bureau du Plan)            => @replace(Buro plan,Bureau du Plan)
 *         @replace(AB,B,C,D)                            => @replace(AB,B,C,D)
 */
U_ch *RPF_replace(U_ch** args)
{
    U_ch    *res = 0;

    if(SCR_tbl_size(args) != 3) return(res);
    res = SCR_areplace(args[0], args[1], args[2]);
    return(res);
}


/**
 *  Syntax: @equal(string1,string2)
 *  
 *  Examples 
 *  --------
 *      @equal(aa,aa)         =>  1
 *      @equal(aa, aa)        =>  0
 *      @equal(aa,bb)         =>  0
 *      @equal(aa,aa,aa)      =>  0
 *  
 *  TODO: return NULL if #args < 2 ? ou accepter plus d'args
 */
U_ch *RPF_equal(U_ch** args)
{
    U_ch    *res = 0;

    if(SCR_tbl_size(args) != 2)
        res = SCR_stracpy((unsigned char*) "0");
    else {
        if(strcmp((char*) args[0], (char*) args[1]))
            res = SCR_stracpy((unsigned char*) "0");
        else
            res = SCR_stracpy((unsigned char*) "1");
    }
    return(res);
}


/**
 *  Syntax: @upper(text)
 *  
 *  Examples 
 *  --------
 *      @upper(ab cd,ef)      =>  "AB CD EF"
 *      @upper(AB)            =>  "AB"
 *      @upper("abc", "def")  =>  "ABC  "DEF""
 *      @upper("abc","def")   =>  "ABC DEF"
 */
U_ch *RPF_upper(U_ch** args)
{
    U_ch    *res = SCR_mtov(args, ' ');

    SCR_upper(res);
    return(res);
}


/**
 *  Syntax: @lower(text)
 *  
 *  Examples 
 *  --------
 *      @lower(ab cd,ef)      =>  "ab cd ef"
 *      @lower(AB)            =>  "ab"
 *      @lower("abc", "def")  =>  "abc  "def""
 *      @lower("abc","def")   =>  "abc def"
 *  
 */
U_ch *RPF_lower(U_ch** args)
{
    U_ch    *res = SCR_mtov(args, ' ');

    SCR_lower(res);
    return(res);
}


/**
 *  Syntax: @sqz(text)
 *  
 *  Examples 
 *  --------
 *      @sqz( ab    cd )      =>  "abcd"
 *      @sqz(" ab    cd ")    =>  "abcd"
 *      @sqz( ab,"  cd", ef)  =>  "abcdef"
 *      @sqz( ab, " cd", ef)  =>  "ab"cd"ef"
 *  
 */
U_ch *RPF_sqz(U_ch** args)
{
    U_ch    *res = SCR_mtov(args, ' ');

    SCR_sqz(res);
    return(res);
}


/**
 *  Syntax: @strip(text)
 *  
 *  Examples 
 *  --------
 *      @strip( ab    cd )      =>  " ab    cd"
 *      @strip(" ab    cd ")    =>  " ab    cd"
 *      @strip( ab,"  cd", ef)  =>  " ab   cd  ef"
 *      @strip( ab, " cd", ef)  =>  " ab  " cd"  ef"
 *  
 */
U_ch *RPF_strip(U_ch** args)
{
    U_ch    *res = SCR_mtov(args, ' ');

    SCR_strip(res);
    return(res);
}


// @see http://www.xon.be/scr4/libs1/libs1164.htm
/**
 *  Syntax: @fmt(long[,format])
 *  
 *  Examples 
 *  --------
 *      @fmt(123456)               -> "123456"
 *      @fmt(123456,99999999)      -> "  123456"
 *      @fmt(123456,999)           -> "456"
 *      @fmt(123.456,999999)       -> "   123"
 *      @fmt(123456,00999999)      -> "00123456"
 *      @fmt(123456,"ABC999,999Z") -> "ABC   ,123Z"
 *      @fmt(123456,ABC999999Z)    -> "ABC   123Z"
 *  
 *  TODO: correct @fmt(123456,"ABC999,999Z") -> "ABC   ,123Z"
 *  
 *  @see http://www.xon.be/scr4/libs1/libs1164.htm
 */
U_ch *RPF_fmtint(U_ch** args)
{
    U_ch    buf[512];
    int     nb;

    nb = SCR_tbl_size(args);
    switch(nb) {
        case 0 :
            return(SCR_mtov(args, ' '));
        case 2 :
            return(SCR_stracpy(SCR_fmt_long(buf, args[1], atol((char*) args[0]))));
        default :
            return(SCR_stracpy(SCR_fmt_long(buf, (unsigned char*) "999999", atol((char*) args[0]))));
    }
}


/**
 *  Syntax: @ansi(oem_text)
 *  
 *  
 *  TODO: add fn  @oem / @utf8
 */
U_ch *RPF_ansi(U_ch** args)
{
    U_ch    *buf;

    if(SCR_tbl_size(args) < 1) return((U_ch *)0);
    buf = SCR_stracpy(args[0]);
    return(SCR_OemToAnsi(buf, args[0]));
}


/**
 *  Syntax: @count(eq)
 *  
 *  Examples 
 *  --------
 *      @count(ACAF,ACAG)         =>  "2"
 *      @count(ACAF;ACAG)         =>  "1"
 *      @count()                  =>  "0"
 */
U_ch *RPF_count(U_ch** args)
{
    U_ch    *res;
    int     n;

    res = (unsigned char*) SCR_malloc(20);
    n = SCR_tbl_size(args);
    sprintf((char*) res, "%d", n);
    return(res);
}


/**
 *  Syntax: @index(n,a,b,c,d)
 *  
 *  Note that if the first argument (n) is non numeric, it is interpreted as 0.
 *  The result is then empty (@index(0,...))
 *  Examples 
 *  --------
 *      @index(0,A,B,C)           =>  ""
 *      @index(2,A,B,C)           =>  "B"
 *      @index(4,A,B,C)           =>  ""
 *      @index(1,A;B;C)           =>  "A;B;C"
 *      @index(A;B;C)             =>  ""
 *      @index()                  =>  ""
 */
U_ch *RPF_index(U_ch** args)
{
    int     n, pos;

    n = SCR_tbl_size(args) - 1;
    if(n < 1) return((unsigned char*) SCR_malloc(1));
    pos = atol((char*) args[0]);
    if(pos > n || pos < 1) return((unsigned char*) SCR_malloc(1));
    return(SCR_stracpy(args[pos]));
}

/**
 *  Syntax: @void(anything)
 *  
 *  Examples
 *  --------
 *      @void(ABC,DEF,G)          =>  ""
 *      @void(@getdir())          =>  ""
 *      @void(@vexpand(*))        =>  ""
 */
U_ch *RPF_void(U_ch **args)
{
    return(SCR_stracpy((unsigned char*) ""));
}


/*--------------------------- DATE AND TIME -------------------------------*/

/**
 *  Syntax: @date([fmt]) where fmt may contain dd for day, mm for month and yyyy or yy for year.
 *  
 *  Examples
 *  --------
 *      @date()               =>  "26-10-2022"
 *      @date(dd-mm-yyyy)     =>  "26-10-2022"
 *      @date(dd-mm-yy)       =>  "26-10-22"
 *      @date(d-m-y)          =>  "2-1-2"
 *      @date(yy/mm/dd)       =>  "22/10/26"
 *      @date(mm/dd/yyyy)     =>  "10/26/2022"
 *      @date(mm/xxxx/zzzz)   =>  "10/xxxx/zzzz"
 *
 *  @see SCR_long_to_fdate() for more details (http://www.xon.be/scr4/libs1/libs122.htm)
 */
U_ch *RPF_date(U_ch** args)
{
    U_ch    *res, *fmt;

    res = (unsigned char*) SCR_malloc(60);
    if(args[0]) fmt = args[0];
    else        fmt = (unsigned char*) "dd-mm-yyyy";
    SCR_long_to_fdate(SCR_current_date(), (char*) res, (char*) fmt);
    return(res);
}


/**
 *  Syntax: @time([fmt]) where fmt may contain hh for hour, mm for minutes and ss for seconds.
 *  
 *  Examples
 *  --------
 *      @time()               =>  "00:30:14"
 *      @time(hh:mm:ss)       =>  "00:30:14"
 *      @time(hhhh:mm)        =>  "00hh:30"
 *      @time(xxx:mm/dd)      =>  "xxx:30/dd"
 *
 *  @see SCR_long_to_ftime() for more details (http://www.xon.be/scr4/libs1/libs129.htm)
 */
U_ch *RPF_time(U_ch** args)
{
    U_ch    *res, *fmt;

    res = (unsigned char*) SCR_malloc(60);
    if(args[0]) fmt = args[0];
    else        fmt = (unsigned char*) "hh:mm:ss";
    SCR_long_to_ftime(SCR_current_time(), (char*) res, (char*) fmt);
    return(res);
}


U_ch* RPF_MONTHSE[] = 
{
    (unsigned char*) "January",
    (unsigned char*) "February",
    (unsigned char*) "March",
    (unsigned char*) "April",
    (unsigned char*) "May",
    (unsigned char*) "June",
    (unsigned char*) "July",
    (unsigned char*) "August",
    (unsigned char*) "September",
    (unsigned char*) "October",
    (unsigned char*) "November",
    (unsigned char*) "December"
};

U_ch* RPF_MONTHSF[] = 
{
    (unsigned char*) "Janvier",
    (unsigned char*) "Février",
    (unsigned char*) "Mars",
    (unsigned char*) "Avril",
    (unsigned char*) "Mai",
    (unsigned char*) "Juin",
    (unsigned char*) "Juillet",
    (unsigned char*) "Août",
    (unsigned char*) "Septembre",
    (unsigned char*) "Octobre",
    (unsigned char*) "Novembre",
    (unsigned char*) "Decembre"
};

U_ch* RPF_MONTHSN[] = 
{
    (unsigned char*) "Januari",
    (unsigned char*) "Februari",
    (unsigned char*) "Maart",
    (unsigned char*) "April",
    (unsigned char*) "Mei",
    (unsigned char*) "Juni",
    (unsigned char*) "Juli",
    (unsigned char*) "Augustus",
    (unsigned char*) "September",
    (unsigned char*) "Oktober",
    (unsigned char*) "November",
    (unsigned char*) "December"
};


/**
 *  Syntax: @month(month_nb[,{FNE}])
 *  
 *  Examples 
 *  --------
 *      @month(1)             =>  "Janvier"
 *      @month(1,F)           =>  "Janvier"
 *      @month(1,e)           =>  "January"
 *      @month(1,N)           =>  "Januari"
 */
U_ch *RPF_month(U_ch** args)
{
    int     m, lang = 'F';

    if(SCR_tbl_size(args) < 1) return((U_ch *)0);
    m = atoi((char*) args[0]);
    if(m < 1 || m > 12) return((U_ch *)0);
    if(SCR_tbl_size(args) >= 2) lang = SCR_upper_char(args[1][0]);
    switch(lang) {
        case 'N' :
            return(SCR_stracpy(RPF_MONTHSN[m - 1]));
        case 'F' :
            return(SCR_stracpy(RPF_MONTHSF[m - 1]));
        default  :
            return(SCR_stracpy(RPF_MONTHSE[m - 1]));
    }
}


/*--------------------------- OBJECT EXTRACTIONS ----------------------------*/

/**
 *  Syntax: @sstderr(scalar1,scalar2...)
 *  
 *  Examples 
 *  --------
 *      @sstderr(acaf1)               =>  "0.001369"
 *      @sstderr(acaf1,xyz,acaf3)     =>  "0.001369 -- 0.873010"
 *      @sstderr(acaf1,acaf2, acaf3)  =>  "0.001369 0.000001 0.873010"
 */
U_ch *RPF_sstderr(U_ch** args)
{
    int     i, pos;
    Scalar     *scl;
    U_ch    *res = 0, buf[80];

    for(i = 0 ; args[i] ; i++) {
        if(i > 0) res = SCR_strafcat(res, (unsigned char*) " ");
        pos = K_WS[SCALARS]->index_of((char*) args[i]);
        if(pos < 0 || (scl = KSVAL(K_WS[SCALARS], pos)) == 0) {
            strcpy((char*) buf, "--");
        }
        else {
            if(IODE_IS_A_NUMBER(scl->std))
                sprintf((char*) buf, "%lf", (double) scl->std);
            else
                strcpy((char*) buf, "--");
        }
        res = SCR_strafcat(res, buf);
    }
    return(res);
}

/**
 *  Syntax: @srelax(scalar1,scalar2...)
 *  
 *  Examples 
 *  --------
 *      @srelax(acaf1)               =>  "1.000000"
 *      @srelax(acaf1,xyz,acaf3)     =>  "1.000000 -- 1.000000"
 *      @srelax(acaf1,acaf2, acaf3)  =>  "1.000000 1.000000 1.000000"  
 */
U_ch *RPF_srelax(U_ch** args)
{
    int     i, pos;
    Scalar     *scl;
    U_ch    *res = 0, buf[80];

    for(i = 0 ; args[i] ; i++) {
        if(i > 0) res = SCR_strafcat(res, (unsigned char*) " ");
        pos = K_WS[SCALARS]->index_of((char*) args[i]);
        if(pos < 0 || (scl = KSVAL(K_WS[SCALARS], pos)) == 0) {
            strcpy((char*) buf, "--");
        }
        else {
            if(IODE_IS_A_NUMBER(scl->relax))
                sprintf((char*) buf, "%lf", (double) scl->relax);
            else
                strcpy((char*) buf, "--");
        }
        res = SCR_strafcat(res, buf);
    }
    return(res);
}

/**
 *  Syntax: @ttitle(table1,table2...)
 *  
 *  Examples 
 *  --------
 *      @ttitle(C8_1)           =>  "Déterminants de l'output potentiel"
 *      @ttitle( c8_10)         =>  "Coin salarial parafiscal"
 *      @ttitle(C8_1 c8_10)     =>  "Table C8_1 c8_10 not found"
 *      @ttitle(C8_1, c8_10)    =>  "Déterminants de l'output potentiel
 *                                  Coin salarial parafiscal"
 */
U_ch *RPF_ttitle(U_ch** args)
{
    U_ch    *res = 0, buf[128];
    Table     *tbl;
    int     pos, i;

    for(i = 0 ; args[i] ; i++) 
    {
        if(i > 0) res = SCR_strafcat(res, (unsigned char*) "\n");
        pos = K_WS[TABLES]->index_of((char*) args[i]);
        if(pos < 0 || (tbl = KTVAL(K_WS[TABLES], pos)) == 0) 
        {
            sprintf((char*) buf, "Table %s not found", args[i]);
            res = SCR_strafcat(res, buf);
        }
        else 
        {
            res = SCR_strafcat(res, T_get_title(tbl, false));
            delete tbl;
            tbl = nullptr;
        }
    }

    return res;
}


/**
 *  Syntax: @cvalue(CMT1[,CMT1...])
 *  
 *  Examples 
 *  --------
 *      @cvalue(acaf)          =>  "Cmt acaf not found"
 *      @cvalue(ACAF)          =>  "Ondernemingen: ontvangen kapitaaloverdrachten."
 *      @cvalue(ACAG)          =>  "Totale overheid: netto ontvangen kapitaaloverdrachten."
 *      @cvalue(ACAF,ACAG)     =>  "Ondernemingen: ontvangen kapitaaloverdrachten.;Totale overheid: netto ontvangen kapitaaloverdrachten."
 */
U_ch *RPF_cvalue(U_ch** args)
{
    U_ch    *res = 0, buf[128];
    int     pos, i;
    KDB     *kdb = K_WS[COMMENTS];

    if(kdb == NULL) return(res);

    for(i = 0 ; args[i] ; i++) {
        if(i > 0) res = SCR_strafcat(res, (unsigned char*) ";");
        pos = kdb->index_of((char*) args[i]);
        if(pos < 0) {
            sprintf((char*) buf, "Cmt %s not found", args[i]);
            res = SCR_strafcat(res, buf);
        }
        else {
            res = SCR_strafcat(res, (unsigned char*) KCVAL(kdb, pos));
            res = SCR_replace(res, (unsigned char*) "\n", (unsigned char*) " ");
        }
    }
    return(res);
}


/**
 *  Syntax: @vvalue(var1,var2...)
 *  
 *  Examples 
 *  --------
 *      @vvalue(acaf)          =>  "na  na  1.2130001  5.2020001  ..."
 *      @vvalue(ACAF)          =>  "na  na  1.2130001  5.2020001  ..."
 *      @vvalue(ACAG)          =>  "na  na  -11.028999  -15.847   ..."
 *      @vvalue(ACAF,ACAG)     =>  "na  na  1.2130001  5.2020001  ... ;  na  na  -11.028999  ...
 */
U_ch *RPF_vvalue(U_ch** args)
{
    U_ch    *res = 0, buf[128];
    double  *val;
    KDB     *kdb = K_WS[VARIABLES];
    int     pos, i, j;

    if(!kdb) 
        return(res);

    if(kdb->sample == nullptr || kdb->sample->nb_periods == 0) 
        return (U_ch*) "";

    for(i = 0 ; args[i] ; i++) 
    {
        if(i > 0) res = SCR_strafcat(res, (unsigned char*) ";");
        pos = kdb->index_of((char*) args[i]);
        if(pos < 0) 
        {
            sprintf((char*) buf, "VAR %s not found", args[i]);
            res = SCR_strafcat(res, buf);
        }
        else 
        {
            val = KVVAL(kdb, pos, 0);
            for(j = 0 ; j < kdb->sample->nb_periods; j++, val++) 
            {
                IodeFmtVal((char*) buf, *val);
                res = SCR_strafcat(res, buf);
                res = SCR_strafcat(res, (unsigned char*) " ");
            }
        }
    }
    return(res);
}

/**
 *  Syntax: @lvalue(list1[,list2...])
 *  
 *  Note that:
 *      1. if there is more than 1 list, the separator is ','.
 *      2. the ';' in the result are replaced by ','
 *  
 *  Examples 
 *  --------
 *      @lvalue(endo)            =>  "$endo0,$endo1,"
 *      @lvalue(ENDO)            =>  "$endo0,$endo1,"
 *      @lvalue(_SEARCH,XENVI)   =>  "ZKF,XEX,XPWMAB,XPWMS,XPWXAB,XPWXS,XQWXAB,XQWXS,XPOIL,XNATY,XTFP"
 */
U_ch *RPF_lvalue(U_ch** args)
{
    U_ch    *res = 0, buf[128];
    int     pos, i;
    KDB     *kdb = K_WS[LISTS];

    if(kdb == NULL) return(res);

    for(i = 0 ; args[i] ; i++) {
        if(i > 0) res = SCR_strafcat(res, (unsigned char*) ",");
        pos = kdb->index_of((char*) args[i]);
        if(pos < 0) {
            sprintf((char*) buf, "List %s not found", args[i]);
            res = SCR_strafcat(res, buf);
        }
        else {
            res = SCR_strafcat(res, (unsigned char*) KLVAL(kdb, pos));
            res = SCR_replace(res, (unsigned char*) ";", (unsigned char*) ",");
        }
    }
    return(res);
}


/**
 *  Syntax: @ivalue(idt1[,idt2...])
 *  
 *  Note that:  
 *      1. if there is more than 1 idt, the separator is ';'.
 *      2. the \n in the result are replaced by ' ' (spaces)
 *  
 *  Examples 
 *  --------
 *      @ivalue(FLGR)            =>  "FLG/VBBP"
 *      @ivalue(FLGR,IUGR)       =>  "FLG/VBBP;IUG/VBBP"
 *      @ivalue(flgr)            =>  "FLG/VBBP"
 */
U_ch *RPF_ivalue(U_ch** args)                
{
    U_ch    *res = 0, buf[128];
    int     pos, i;
    KDB     *kdb = K_WS[IDENTITIES];

    if(kdb == NULL) return(res);

    for(i = 0 ; args[i] ; i++) {
        if(i > 0) res = SCR_strafcat(res, (unsigned char*) ";");
        pos = kdb->index_of((char*) args[i]);
        if(pos < 0) {
            sprintf((char*) buf, "Idt %s not found", args[i]);
            res = SCR_strafcat(res, buf);
        }
        else {
            res = SCR_strafcat(res, (unsigned char*) KILEC(kdb, pos));
            res = SCR_replace(res, (unsigned char*) "\n", (unsigned char*) " ");
        }
    }
    return(res);
}


/**
 *  Syntax: @evalue(eq1[,eq2...])
 *  
 *  Note that:  
 *      1. if there is more than 1 equation, the separator is ';'.
 *      2. the \n in the result are replaced by ' ' (spaces), thus creating a long line
 *  
 *  Examples 
 *  --------
 *      @evalue(ACAF)            =>  "(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+ acaf4*(TIME=1995)"
 *      @evalue(brugp)           =>  "BRUGP := 0"
 *      @evalue(acaf,brugp)      =>  "(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+ acaf4*(TIME=1995);BRUGP := 0"
 *      @evalue(XXX)             =>  "Eqs XXX not found"
 *      @evalue()                =>  "@evalue()"
 */
U_ch *RPF_evalue(U_ch** args)                
{
    U_ch    *res = 0, buf[128];
    int     pos, i;
    KDB     *kdb = K_WS[EQUATIONS];

    if(kdb == NULL) 
        return(res);

    for(i = 0 ; args[i] ; i++) 
    {
        if(i > 0) 
            res = SCR_strafcat(res, (unsigned char*) ";");
        
        pos = kdb->index_of((char*) args[i]);
        if(pos < 0) 
        {
            sprintf((char*) buf, "Eqs %s not found", args[i]);
            res = SCR_strafcat(res, buf);
        }
        else 
        {
            std::string lec = KELEC(kdb, pos);
            res = SCR_strafcat(res, (unsigned char*) lec.c_str());
            res = SCR_replace(res, (unsigned char*) "\n", (unsigned char*) " ");
        }
    }
    return(res);
}


/**
 *  Syntax: @eqsample(eq)
 *  
 *  Examples 
 *  --------
 *      @eqsample(ACAF)               =>  "1980Y1:1996Y1"
 *      @eqsample(ACAF,ACAG)          =>  "@eqsample(ACAF,ACAG)"
 *      @eqsample(XXX)                =>  "[Eqs XXX not found]"
 *      @eqsample()                   =>  "@eqsample()"
 */
U_ch *RPF_eqsample(U_ch** args)            
{
    U_ch    *res = 0;
    int     pos;
    KDB     *kdb = K_WS[EQUATIONS];

    if(kdb == NULL) 
        return(res);                // Equation WS  empty
    if(SCR_tbl_size(args) != 1) 
        return(res);                // 1! eq

    res = (unsigned char*) SCR_malloc(80);
    pos = kdb->index_of((char*) args[0]);

    if(pos < 0) 
        sprintf((char*) res, "[Eqs %s not found]", args[0]);
    else
    {
        Sample smpl = KESMPL(kdb, pos);
        std::string str_smpl = smpl.to_string();
        sprintf((char*) res, (char*) str_smpl.c_str());
    }       

    return(res);
}


/**
 *  Sub fonction  of RPF_eqsamplefrom() and RPF_eqsampleto().
 *  
 *  @param [in] char**  args    args of RPF_eqsample*() => equation name
 *  @param [in] int     fromto  0 if sample.from must be returned, 1 for sample.to
 *  @return     char*           sample.from or sample.to in text format
 *                              " " if there is no sample in the equation
 *                              "[Eq ... not found]" if the eq does not exist
 */
U_ch *RPF_eqsamplefromto(U_ch** args, int fromto)           
{
    U_ch    *res = 0;
    int     pos;
    KDB     *kdb = K_WS[EQUATIONS];

    if(kdb == NULL) 
        return(res);             // Equation WS  empty

    if(SCR_tbl_size(args) != 1) 
        return(res);            // 1! eq

    res = (unsigned char*) SCR_malloc(30 + (int)strlen((char*) args[0]));
    pos = kdb->index_of((char*) args[0]);

    if(pos < 0) 
        sprintf((char*) res, "[Eqs %s not found]", args[0]);
    else 
    {
        Sample smpl = KESMPL(kdb, pos);
        if(fromto == 0) 
            sprintf((char*) res, (char*) smpl.start_period.to_string().c_str());
        else            
            sprintf((char*) res, (char*) smpl.end_period.to_string().c_str());
    }

    if(res[0] == 0) 
        strcpy((char*) res, " ");   // pour éviter de quitter le rapport si sample vide

    return(res);
}


/**
 *  Syntax: @eqsamplefrom(eq)
 *  
 *  Examples 
 *  --------
 *      @eqsamplefrom(ACAF)           =>  "1980Y1"
 *      @eqsamplefrom(XXX)            =>  "[Eqs XXX not found]"
 *      @eqsamplefrom()               =>  "@eqsamplefrom()"
 */
U_ch *RPF_eqsamplefrom(U_ch **args)
{
    return(RPF_eqsamplefromto(args, 0));
}


/**
 *  Syntax: @eqsampleto(eq)
 *  
 *  Examples 
 *  --------
 *      @eqsampleto(ACAF)             =>  "1996Y1"
 *      @eqsampleto(XXX)              =>  "[Eqs XXX not found]"
 *      @eqsampleto()                 =>  "@eqsampleto()"
 */
U_ch *RPF_eqsampleto(U_ch **args)
{
    return(RPF_eqsamplefromto(args, 1));
}

/**
 *  Sub fonction  of RPF_eqlhs() and RPF_eqrhs().
 *  
 *  @param [in] char**  args    args of RPF_eq?hs() => equation name
 *  @param [in] int     lhsrhs  0 for the LHS, 1 foir the RHS of the equation
 *  @return     char*           allocated LHS or RHS 
 *                              "[Eq ... not found]" if the eq does not exist
 */
U_ch *RPF_eqlhsrhs(U_ch** args, int lhsrhs) 
{
    U_ch    *eq = 0, *rhs;
    int     pos, poscolon;
    KDB     *kdb = K_WS[EQUATIONS];

    if(kdb == NULL) return(eq);             // Equation WS  empty
    if(SCR_tbl_size(args) != 1) return(eq); // 1! eq

    pos = kdb->index_of((char*) args[0]);

    if(pos < 0) {
        eq = (unsigned char*) SCR_malloc(80);
        sprintf((char*) eq, "[Eqs %s not found]", args[0]);
    }
    else 
    {
        std::string lec = KELEC(kdb, pos);
        eq = SCR_stracpy((unsigned char*) lec.c_str());
        poscolon = L_split_eq((char*) eq);
        if(poscolon > 0) {
            if(lhsrhs == 0) eq[poscolon] = 0;
            else {
                rhs = SCR_stracpy(eq);
                strcpy((char*) rhs, ((char*) eq) + poscolon + 2);
                SCR_free(eq);
                eq = rhs;
            }    
        }
    }

    return(eq);
}


/**
 *  Syntax: @eqlhs(eq)
 *  
 *  Examples 
 *  --------
 *      @eqlhs(ACAF)             =>  "(ACAF/VAF[-1]) "
 *      @eqlhs(ACAF,ACAG)        =>  "@eqlhs(ACAF,ACAG)"
 *      @eqlhs(XXX)              =>  "[Eqs XXX not found]"
 *      @eqlhs()                 =>  "@eqlhs()"
 */
U_ch *RPF_eqlhs(U_ch **args)
{
    return(RPF_eqlhsrhs(args, 0));
}


/**
 *  Syntax: @eqrhs(eq)
 *  
 *  Examples 
 *  --------
 *      @eqrlhs(ACAF)             =>  "acaf1+acaf2*GOSF[-1]+
 *                                     acaf4*(TIME=1995)"
 *      @eqrlhs(ACAF,ACAG)        =>  "@eqrhs(ACAF,ACAG)"
 *      @eqrlhs(XXX)              =>  "[Eqs XXX not found]"
 *      @eqrlhs()                 =>  "@eqrhs()"
 */
U_ch *RPF_eqrhs(U_ch **args)
{
    return(RPF_eqlhsrhs(args, 1));
}


/**
 *  Syntax: @sample([{B|E}])
 *  
 *  Examples 
 *  --------
 *      @sample()                 =>  "1960Y1 2015Y1"
 *      @sample(B)                =>  "1960Y1"
 *      @sample(E)                =>  "2015Y1"
 *      @sample(X)                =>  "1960Y1 2015Y1"
 */
U_ch *RPF_sample(U_ch** args)
{
    U_ch    *res = 0, buf[128];
    Sample  *smpl;
    KDB     *kdb = K_WS[VARIABLES];
    char     what = 'F';

    smpl = kdb->sample;
    if(!smpl || smpl->nb_periods == 0) 
        return(res);

    if(args[0]) 
        what = args[0][0];
    
    what = toupper(what);
    std::string s_per1 = smpl->start_period.to_string();
    std::string s_per2 = smpl->end_period.to_string();
    
    switch(what) {
        case 'B':
            sprintf((char*) buf, "%s", (char*) s_per1.c_str());
            break;
        case 'E':
            sprintf((char*) buf, "%s", (char*) s_per2.c_str());
            break;
        default :
            sprintf((char*) buf, "%s %s", (char*) s_per1.c_str(), (char*) s_per2.c_str());
            break;
    }

    res = SCR_strafcat(res, buf);
    return(res);
}


/**
 *  Searches in a CLEC struct the list of scalars or variables and adds to *tbl the new names found.
 *  
 *  @param [in] CLEC*   cl      input CLEC structure
 *  @param [in] U_ch*** tbl     pointer to the table of strings with the scalars or variables
 *  @param [in] int*    nb      pointer to the number of names found
 *  @param [in] int     type    type of object to retrieve: 'S' for scalars, 'V' for variables
 *  @return     int             0, always
 */
int RPF_vsliste1(CLEC* cl, U_ch*** tbl, int* nb, int type)
{
    int     j, k;

    for(j = 0 ; j < cl->nb_names ; j++) {
        if(is_coefficient(cl->lnames[j].name) && type != 'S') continue;
        if(!is_coefficient(cl->lnames[j].name) && type != 'V') continue;
        for(k = 0 ; k < *nb ; k++)
            if(strcmp(cl->lnames[j].name, (char*) (*tbl)[k]) == 0) break;
        if(*nb == 0 || k == *nb) SCR_add_ptr(tbl, nb, (unsigned char*) cl->lnames[j].name);
    }

    return(0);
}


/**
 *  Searches in a list of equations all objects of a given type ('S' or 'V').
 *  
 *  @param [in] U_ch**  args    list of equations to analyse
 *  @param [in] int     type    type of objects to retrieve:  'S' for scalars, 'V' for variables
 *  @return     U_ch*           allocated string with found object names separated by semi-colon (';')
 */
U_ch *RPF_vsliste(U_ch** args, int type)
{
    U_ch  **tbl = 0, *res;
    int   i, nb = 0, pos;

    if(SCR_tbl_size(args) < 1) 
        return(NULL);

    Equation* eq;
    for(i = 0 ; args[i] ; i++) 
    {
        pos = K_WS[EQUATIONS]->index_of((char*) args[i]);
        if(pos < 0) continue;
        eq = KEVAL(K_WS[EQUATIONS], pos);
        RPF_vsliste1(eq->clec, &tbl, &nb, type);
        if(eq) delete eq;
        eq = nullptr;
    }

    SCR_add_ptr(&tbl, &nb, 0L);

    res = SCR_mtov(tbl, ';');
    SCR_free_tbl(tbl);
    return res;
}


/**
 *  Returns a list of unique strings in a table of strings.
 *  
 *  @param [in] U_ch**  tbl     input table of strings
 *  @return     U_ch**          table of unique strings in tbl
 */
U_ch **RPF_unique(U_ch** tbl)
{
    U_ch    **tbl2 = 0;
    int     nl = 0, i, j;

    if(tbl == 0 || tbl[0] == 0) return(tbl2);
    SCR_add_ptr(&tbl2, &nl, tbl[0]);
    for(i = 1 ; tbl[i] ; i++) {
        for(j = 0 ; j < nl ; j++) {
            if(strcmp((char*) tbl[i], (char*) tbl2[j]) == 0) break;
        }
        if(j == nl)
            SCR_add_ptr(&tbl2, &nl, tbl[i]);
    }
    SCR_add_ptr(&tbl2, &nl, 0L);
    return(tbl2);
}


/**
 *  Syntax: @vliste(eq1[,eq2...])
 *  
 *  Examples 
 *  --------
 *      @vliste(ACAF)             =>  "ACAF;VAF;GOSF;TIME"
 *      @vliste(ACAF,ACAG)        =>  "ACAF;VAF;GOSF;TIME;ACAG;VBBP"
 */
U_ch *RPF_vliste(U_ch** args)
{
    return(RPF_vsliste(args, 'V'));
}


/**
 *  Syntax: @sliste(eq1[,eq2...])
 *  
 *  Examples 
 *  --------
 *      @sliste(ACAF)             =>  "acaf1;acaf2;acaf4"
 *      @sliste(ACAF,ACAG)        =>  "acaf1;acaf2;acaf4"
 */
U_ch *RPF_sliste(U_ch** args)
{
    return(RPF_vsliste(args, 'S'));
}


/**
 *  Expands a list of name patterns for a given object type.
 *  
 *  @param [in] U_ch**  args    table of name patterns
 *  @param [in] int     type    object type
 *  @return     U_ch*           list of object names sep by ";"
 */
U_ch *RPF_expand(U_ch** args, int type)
{
    U_ch        *res, *tmp, **tbl = 0, **tbl2;
    int         nb = 0, i;


    for(i = 0 ; args[i] ; i++) {
        tmp = (unsigned char*) K_expand(type, 0L, (char*) args[i], (int) '*');
        if(tmp) SCR_add_ptr(&tbl, &nb, tmp);
        SCR_free(tmp);
    }
    SCR_add_ptr(&tbl, &nb, 0L);
    res = SCR_mtov(tbl, ';');
    SCR_free_tbl(tbl);
    tbl = SCR_vtom(res, ';');
    SCR_free(res);
    tbl2 = RPF_unique(tbl);
    SCR_free_tbl(tbl);
    res = SCR_mtov(tbl2, ';');
    SCR_free_tbl(tbl2);
    if(res == 0) res = SCR_stracpy((unsigned char*) ""); // JMP 04/11/2022 
    return(res);
}


/**
 *  Syntax: @cexpand(pattern1[,pattern2...])
 *  
 *  Examples 
 *  --------
 *      @cexpand(AC*)           =>  "ACAF;ACAG"
 *      @cexpand(*U)            =>  "DPU;DPUU;IFU;IHU;WBU"
 *      @cexpand(AC*,*U)        =>  "ACAF;ACAG;DPU;DPUU;IFU;IHU;WBU"
 *      @cexpand()              =>  "@cexpand()"
 */
U_ch *RPF_cexpand(U_ch **args)
{
    return(RPF_expand(args, COMMENTS));
}


/**
 *  Syntax: @eexpand(pattern1[,pattern2...])
 *  
 *  Examples
 *  --------
 *      @eexpand(AC*)           =>  "ACAF;ACAG"
 *      @eexpand(*U)            =>  "CGU;DPU;DPUU;IFU;IHU;WBU"
 *      @eexpand(AC*,*U)        =>  "ACAF;ACAG;CGU;DPU;DPUU;IFU;IHU;WBU"
 *      @eexpand()              =>  ""
 */
U_ch *RPF_eexpand(U_ch **args)
{
    return(RPF_expand(args, EQUATIONS));
}


/**
 *  Syntax: @iexpand(pattern1[,pattern2...])
 *  
 *  Examples
 *  --------
 *      @iexpand(AC*)           =>  ""
 *      @iexpand(*U)            =>  "MU;NAWRU"
 *      @iexpand(AC*,*U)        =>  "MU;NAWRU"
 *      @iexpand()              =>  ""
 */
U_ch *RPF_iexpand(U_ch **args)
{
    return(RPF_expand(args, IDENTITIES));
}


/**
 *  Syntax: @lexpand(pattern1[,pattern2...])
 *  
 *  Examples
 *  --------
 *      @lexpand(AC*)           =>  ""
 *      @lexpand(*0)            =>  "COPY0;ENDO0;TOTAL0"
 *      @lexpand(AC*,*0)        =>  "COPY0;ENDO0;TOTAL0"
 *      @lexpand()              =>  ""
 */
U_ch *RPF_lexpand(U_ch **args)
{
    return(RPF_expand(args, LISTS));
}


/**
 *  Syntax: @sexpand(pattern1[,pattern2...])
 *  
 *  Examples
 *  --------
 *      @sexpand(AC*)           =>  "acaf1;acaf2;acaf3;acaf4"
 *      @sexpand(ac*)           =>  "acaf1;acaf2;acaf3;acaf4"
 *      @sexpand(*4)            =>  "acaf4;dtf4;gamma4;k4;kl14;nfyh4;pc4;pmab4;pxs4;qc4;qi5_4;qmab_4;qms4;qxab4;qxs4;y4"
 *      @sexpand(AC*,*4)        =>  "acaf1;acaf2;acaf3;acaf4;dtf4;gamma4;k4;kl14;nfyh4;pc4;pmab4;pxs4;qc4;qi5_4;qmab_4;qms4;qxab4;qxs4;y4"
 *      @sexpand()              =>  ""
 */
U_ch *RPF_sexpand(U_ch **args)
{
    return(RPF_expand(args, SCALARS));
}


/**
 *  Syntax: @texpand(pattern1[,pattern2...])
 *  
 *  Examples
 *  --------
 *      @texpand(AC*)           =>  ""
 *      @texpand(*U)            =>  "MULT1RESU;MULT2RESU;NAWRU;T1NIVEAU"
 *      @texpand(AC*,*U)        =>  "MULT1RESU;MULT2RESU;NAWRU;T1NIVEAU"
 *      @texpand()              =>  ""
 */
U_ch *RPF_texpand(U_ch **args)
{
    return(RPF_expand(args, TABLES));
}


/**
 *  Syntax: @vexpand(pattern1[,pattern2...])
 *  
 *  Examples
 *  --------
 * @vexpand(AC*)           =>  "ACAF;ACAG"
 * @vexpand(*U)            =>  "CGU;DPU;DPUU;IFU;IHU;MU;NAWRU;WBU"
 * @vexpand(AC*,*U)        =>  "ACAF;ACAG;CGU;DPU;DPUU;IFU;IHU;MU;NAWRU;WBU"
 * @vexpand()              =>  ""
 */
U_ch *RPF_vexpand(U_ch **args)
{
    return(RPF_expand(args, VARIABLES));
}



/****************** SIMULATION ***************************/

/**
 *  Calculated the position in the current WS of the period args[0].
 *  
 *  @param [in] U_ch** args     argument (args[0] = period)
 *  @return     int              position of the period of -1 on error (out of range...) 
 */
int RPF_CalcPeriod(U_ch** args)
{
    // Check args
    if(SCR_tbl_size(args) == 0) 
        return(-1); // Erreur, arg required
    
    Period per(std::string((char*) args[0]));
    if(per.year == 0) 
        return(-1); // Error

    // Calc diff bw per and WS sample
    int t = per.difference(KV_WS->sample->start_period);
    if(t < 0)  
        return(-1);

    int at = per.difference(KV_WS->sample->end_period);
    if(at > 0)
        return(-1);

    return(t);
}


/**
 *  Syntax: @SimMaxit()
 *  
 *  Examples
 *  --------
 *      @SimMaxit()             =>  "100"
 */
U_ch *RPF_SimMaxit(U_ch** unused)
{
    U_ch    buf[128];

    sprintf((char*) buf, "%d", CSimulation::KSIM_MAXIT);
    return(SCR_stracpy(buf));
}


/**
 *  Syntax: @SimEps()
 *  
 *  Examples
 *  --------
 *      @SimEps()               =>  "0.001"
 */
U_ch *RPF_SimEps(U_ch** unused)
{
    U_ch    buf[128];

    sprintf((char*) buf, "%lg", CSimulation::KSIM_EPS);
    return(SCR_stracpy(buf));
}


/**
 *  Syntax: @SimRelax()
 *  
 *  Examples
 *  --------
 *        @SimRelax()             =>  "1.000000"
 */
U_ch *RPF_SimRelax(U_ch** unused)
{
    U_ch    buf[128];

    sprintf((char*) buf, "%lf", CSimulation::KSIM_RELAX);
    return(SCR_stracpy(buf));
}


/**
 *  Syntax: @SimSortNbPasses()
 *  
 *  Examples
 *  --------
 *        @SimSortNbPasses()        =>  "5"
 */
U_ch *RPF_SimSortNbPasses(U_ch** unused)
{
    U_ch    buf[128];

    sprintf((char*) buf, "%d", CSimulation::KSIM_PASSES);
    return(SCR_stracpy(buf));
}

/**
 *  Syntax: @SimSortAlgo()
 *  
 *  Examples
 *  --------
 *        @SimSortAlgo()        =>  "Triangulation"
 */
U_ch *RPF_SimSortAlgo(U_ch** unused)
{
    char *algos[] = {"Connex", "Triangulation", "None"};
    
    int     isort;

    isort = CSimulation::KSIM_SORT;
    if(isort < 0 || isort > 2)
        isort = 1; // Default value
        
    return(SCR_stracpy((unsigned char*) algos[isort]));
}


/**
 *  Syntax: @SimInitValues()
 *  
 *  Examples
 *  --------
 *        @SimInitValues()        =>  "1"
 */
U_ch *RPF_SimInitValues(U_ch** unused)
{
    U_ch    buf[128];

    sprintf((char*) buf, "%d", CSimulation::KSIM_START);
    return(SCR_stracpy(buf));
}


/**
 *  Sub-fn of RPF_SimNorm().
 *  
 *  @param [in] args U_ch*  period from which norm has to be to retrieved
 *  @return          double convergence criteria of the period arg or -1.0 if no simulation       
 */
double RPF_SimNormReal(U_ch** args)
{
    int		t;

    t = RPF_CalcPeriod(args);
    if(t < 0) return(-1.0);

    // Check si déjà simulation
    if(CSimulation::KSIM_NORMS == 0) return(-1.0); // Pas encore de simulation

    // Return norme t
    return(CSimulation::KSIM_NORMS[t]);
}

/**
 *  Syntax: @SimNorm(period)
 *  
 *  Examples
 *  --------
 *      @SimNorm(2000Y1)        =>  "0.000850147"
 *      @SimNorm(2001Y1)        =>  "0.000798655"
 *      @SimNorm(2002Y1)        =>  "0"
 *      @SimNorm(1990Y1)        =>  "0"
 */
U_ch *RPF_SimNorm(U_ch** args)
{
    char	buf[128];
    double  value;

    value = RPF_SimNormReal(args);
    if(value < 0) return((unsigned char*) SCR_malloc(1));
    sprintf((char*) buf, "%lg", value);
    return(SCR_stracpy((unsigned char*) buf));
}

/**
 *  Subfunction of RPF_SimNIter().
 *  
 *  @param [in] args    char**  period  (eg: args[0] => "2001Y1")
 *  @return             int     nb of iterations to converge in the last simulation for period arg[0]
 */
int RPF_SimNIterInt(U_ch** args)
{
    int		t;

    t = RPF_CalcPeriod(args);
    if(t < 0) return(-1);

    // Check si déjà simulation
    if(CSimulation::KSIM_NITERS == 0) return(-1); // Pas encore de simulation

    // Return norme t
    return(CSimulation::KSIM_NITERS[t]);
}

/**
 *  Syntax: @SimNIter(period)
 *  
 *  Examples
 *  --------
 *      @SimNiter(2000Y1)       =>  "31"
 *      @SimNiter(2001Y1)       =>  "24"
 *      @SimNiter(2002Y1)       =>  "0"
 *      @SimNiter(1990Y1)       =>  "0"
 */
U_ch *RPF_SimNIter(U_ch** args)
{
    int		niter;
    char	buf[128];

    niter = RPF_SimNIterInt(args);
    if(niter < 0) return((unsigned char*) SCR_malloc(1));

    // Return niter in t
    sprintf((char*) buf, "%d", niter);
    return(SCR_stracpy((unsigned char*) buf));
}


/**
 *  Subfunction of RPF_SimCpu().
 *  
 *  @param [in] args    char**  period  (eg: args[0] => "2001Y1")
 *  @return             int     time (in ms) elapsed to reach convergence in the last simulation for period arg[0]
 */
int RPF_SimCpuInt(U_ch** args)
{
    int		t;

    t = RPF_CalcPeriod(args);
    if(t < 0) return(-1);

    // Check si déjà simulation
    if(CSimulation::KSIM_CPUS == 0) return(-1); // Pas encore de simulation

    // Return norme t
    return(CSimulation::KSIM_CPUS[t]);
}


/**
 *  Syntax: @SimCpu(period)
 *  
 *  Returns the time (in ms) elapsed during last simulation of the specidied period.
 *  
 *  Examples
 *  --------
 *      @SimCpu(2000Y1)       =>  "31"
 *      @SimCpu(2001Y1)       =>  "24"
 *      @SimCpu(2002Y1)       =>  "0"
 *      @SimCpu(1990Y1)       =>  "0"
 */
U_ch *RPF_SimCpu(U_ch** args)
{
    int		cpu;
    char	buf[128];

    cpu = RPF_SimCpuInt(args);
    if(cpu < 0) return((unsigned char*) SCR_malloc(1));

    sprintf((char*) buf, "%d", cpu);
    return(SCR_stracpy((unsigned char*) buf));
}



/**
 *  Syntax: @SimCpuSCC()
 *  
 *  Examples
 *  --------
 *        @SimCpuSCC()        =>  "31343"
 */
U_ch *RPF_SimCpuSCC(U_ch** unused)
{
    U_ch    buf[128];

    sprintf((char*) buf, "%d", CSimulation::KSIM_CPU_SCC);
    return(SCR_stracpy(buf));
}

/**
 *  Syntax: @SimCpuReorder()
 *  
 *  Examples
 *  --------
 *        @SimCpuReorder()        =>  "1234"
 */
U_ch *RPF_SimCpuSort(U_ch** unused)
{
    U_ch    buf[128];

    sprintf((char*) buf, "%d", CSimulation::KSIM_CPU_SORT);
    return(SCR_stracpy(buf));
}

/*=================== VTake, ... ==============================*/

/**
 *  Takes the first n (or the last if n < 0) elements of a argument list.
 * 
 *  Syntax: @vtake(n,arg1[,arg2,...])
 *  
 *  Note that if the first argument (n) is non numeric, it is interpreted as 0.
 *  The result is then empty (@vtake(0,...))
 *  
 *  Examples
 *  --------
 *      @vtake(2,A,B,C,D,E)     =>  "A;B"
 *      @vtake(-1;A;B;C;D;E)    =>  "E"
 *      @vtake(3 A B)           =>  "A;B"
 *      @vtake(3,A,"B C")       =>  "A;B;C"
 *      @vtake(A,B)             =>  ""
 *      @vtake()                =>  ""
 */
U_ch *RPF_vtake(U_ch** args)
{
    U_ch    *res = 0, *tmp, *arg;
    int     n, lg;

    if(RP_VSEPS == 0) RP_vseps(0);
    if(SCR_tbl_size(args) < 1) return((unsigned char*) SCR_malloc(1));
    arg = SCR_mtov(args, RP_VSEPS[0]);
    args = SCR_vtoms(arg, (unsigned char*) RP_VSEPS);
    SCR_free(arg);
    lg = SCR_tbl_size(args) - 1;
    if(lg == 0) {
        SCR_free_tbl(args);
        return((unsigned char*) SCR_malloc(1));
    }
    n = atol((char*) args[0]);
    if(n > 0) {
        n = std::min(lg, n);
        tmp = args[n + 1];
        args[n + 1] = 0;
        res = SCR_mtov(args + 1, RP_VSEPS[0]);
        args[n + 1] = tmp;
    }
    else {
        n = lg - std::min(lg, -n);
        res = SCR_mtov(args + n + 1, RP_VSEPS[0]);
    }
    SCR_free_tbl(args);
    return(res);
}


/**
 *  Drops the first n (or the last if n < 0) elements of a argument list.
 * 
 *  Syntax: @vdrop(n,arg1[,arg2,...])
 *  
 *  Note that if the first argument (n) is non numeric, it is interpreted as 0.
 *  The result is then all the remaining parameters (separated by ';') (@vdrop(0,...))
 *  
 *  Examples
 *  --------
 *      @vdrop(2,A,B,C,D,E)     =>  "C;D;E"
 *      @vdrop(-1;A;B;C;D;E)    =>  "A;B;C;D"
 *      @vdrop(3 A B)           =>  ""
 *      @vdrop(A,B,C)           =>  "B;C"
 *      @vdrop()                =>  ""
 */
U_ch *RPF_vdrop(U_ch** args)
{
    U_ch    *res = 0, *tmp, *arg;
    int     n, lg;

    if(RP_VSEPS == 0) RP_vseps(0);
    if(SCR_tbl_size(args) < 1) return((unsigned char*) SCR_malloc(1));
    arg = SCR_mtov(args, RP_VSEPS[0]);
    args = SCR_vtoms(arg, (unsigned char*) RP_VSEPS);
    SCR_free(arg);
    lg = SCR_tbl_size(args) - 1;
    if(lg == 0) {
        SCR_free_tbl(args);
        return((unsigned char*) SCR_malloc(1));
    }
    n = atol((char*) args[0]);
    if(n < 0) {
        n = std::max(lg + n, 0);
        tmp = args[n + 1];
        args[n + 1] = 0;
        res = SCR_mtov(args + 1, RP_VSEPS[0]);
        args[n + 1] = tmp;
    }
    else {
        n = std::min(lg, n);
        res = SCR_mtov(args + n + 1, RP_VSEPS[0]);
    }
    SCR_free_tbl(args);
    return(res);
}


/**
 *  Counts the number of elements of the function.
 * 
 *  Syntax: @vcount(arg1[,arg2,...])
 *  
 *  Examples
 *  --------
 *      @vcount(A,B,C,D,E)      =>  "5"
 *      @vcount(C;D;E)          =>  "3"
 *      @vcount(3 A B)          =>  "3"
 *      @vcount(3 "A B")        =>  "3"
 *      @vcount()               =>  "0"
 */
U_ch *RPF_vcount(U_ch** args)
{
    char    buf[128], *arg;

    if(RP_VSEPS == 0) RP_vseps(0);
    arg = (char*) SCR_mtov(args, RP_VSEPS[0]);
    args = SCR_vtoms((unsigned char*) arg, (unsigned char*) RP_VSEPS);
    SCR_free(arg);

    sprintf((char*) buf,  "%d", SCR_tbl_size(args));
    SCR_free_tbl(args);
    return(SCR_stracpy((unsigned char*) buf));
}


/*---------------------- SYSTEM --------------------------------*/

/**
 *  Syntax: @Version()
 *  
 *  Examples
 *  --------
 *        @IodeVersion()             =>  "6.69"
 */
U_ch *RPF_IodeVersion(U_ch** unused)
{
    U_ch    buf[128];

    sprintf((char*) buf, "%d.%d", IODE_VERSION_MAJOR, IODE_VERSION_MINOR);
    return(SCR_stracpy(buf));
}




/**
 *  Syntax: @memory()
 *  
 *  Examples 
 *  --------
 *      @memory()    =>  "NBA=1260, NBB=15, ALLOC=0.5 MB, FREE=2048.0 MB, USED=0.4 MB "
 */
U_ch *RPF_memory(U_ch** args)
{
    U_ch     *res = 0, buf[128];
    double   convfree, convused, convalloc;

    SWSTAT  st;

    SW_status(&st);

    convalloc = ((double)st.st_convalloc)/(1024.0 * 1024.0),
    convfree = ((double)st.st_convafree + (double)st.st_convfree)/(1024.0*1024.0);
    convused = ((double)st.st_convalloc - (double)st.st_convafree)/(1024.0*1024.0);
    
    sprintf((char*) buf, "NBA=%ld, NBB=%ld, ALLOC=%.1lf MB, FREE=%.1lf MB, USED=%.1lf MB",
            st.st_nballoc - st.st_nbfree,
            st.st_nbblks,
            //(st.st_convalloc)/(1024 * 1024),
            //(st.st_convafree + st.st_convfree)/(1024L*1024L),
            //(st.st_convalloc - st.st_convafree)/(1024L*1024L));  // JMP 29/09/2021 : misplaced )
            convalloc, convfree, convused);
            

    res = SCR_strafcat(res, buf);
    return(res);
}

/*--------------------------- CHRONO ------------------------------- */

static long RPF_CHRONO = 0;     // Current chrono initial time 

/**
 *  Resets the timer to the current millisecond.
 *  
 *  Syntax: @chronoreset()
 *  
 *  
 *  Example
 *  -------
 *      @chronoreset()            =>  ""
 */
U_ch *RPF_ChronoReset(U_ch** unused)
{    
    RPF_CHRONO = WscrGetMS();
    return((unsigned char*) SCR_malloc(1));
}


/**
 *  Returns the number of millisecond elapsed since the last call to RPF_ChronoReset(U_ch** unused).
 *  
 *  Syntax: @chronoget()
 *  
 *  Example
 *  -------
 *      @chronoget()              =>  "47"
 */
U_ch *RPF_ChronoGet(U_ch** unused)
{
    U_ch        buf[128];
    long	    ms;

    ms = WscrGetMS() - RPF_CHRONO;
    sprintf((char*) buf, "%ld", ms);
    return(SCR_stracpy(buf));
}


/*--------------------------- FILES AND DIRECTORIES ------------------------------- */

/**
 *  Syntax: @fappend(filename,line,line...) where line may be NL for newline
 *  
 *  Examples 
 *  --------
 *     @fappend(essais.txt,"line 1",NL,line 2,NL,NL,line 4)  =>  ""
 */
U_ch *RPF_fappend(U_ch** args)
{
    FILE    *fd;
    int     i;

    if(SCR_tbl_size(args) < 1) return(NULL);
    fd = fopen((char*) args[0], "a+");
    if(fd == 0) return(NULL);
    for(i = 1 ; args[i] ; i++) {
        if(strcmp((char*) args[i], "NL") == 0) fprintf(fd, "\n");
        else                                   fprintf(fd, "%s", args[i]);
    }
    fclose(fd);
    return((unsigned char*) SCR_malloc(1));
}


/**
 *  Syntax: @fdelete(filename)
 *  
 *  Examples 
 *  --------
 *     @fdelete(essais.txt)  =>  ""
 */
U_ch *RPF_fdelete(U_ch** args)
{
    if(SCR_tbl_size(args) < 1) return(NULL);
    _unlink((char*) args[0]);
    return((unsigned char*) SCR_malloc(1));
}


/**
 *  Syntax: @getdir()
 *  
 *  Examples
 *  --------
 *      `@getdir()`                 =>  "C:\Usr\iode"
 */
U_ch *RPF_getdir(U_ch** unused)
{
    U_ch    dir[512];

    if(SCR_getcwd(0, (char*) dir) == 0)
        return(SCR_stracpy((unsigned char*) ""));
    else
        return(SCR_stracpy(dir));
}


/**
 *  Syntax: @chdir(dirname)
 *  
 *  Examples
 *  --------
 *      @chdir(..)                =>  "C:\Usr\iode"
 *      @getdir()                 =>  "C:\Usr\iode"
 *      @chdir(hermes)            =>  "C:\Usr\iode\hermes"
 */
U_ch *RPF_chdir(U_ch **args)
{
    int     rc = 0;

    if(SCR_tbl_size(args) > 0) {
        #ifdef _MSC_VER
            rc = _chdir((char*) args[0]); 
        #else
            rc = chdir((char*) args[0]); 
        #endif
    }    

    //ODE_settitle();
    ksettitle();     // JMP 10/12/2021
    return(RPF_getdir(NULL));
}


/**
 *  Syntax: @mkdir(dir)
 *  
 *  Examples
 *  --------
 *      @mkdir(testdir)           =>  ""
 *      @getdir()                 =>  "C:\Usr\iode"
 *      @chdir(testdir)           =>  "C:\Usr\iode\testdir"
 *      @getdir()                 =>  "C:\Usr\iode\testdir"
 */
U_ch *RPF_mkdir(U_ch **args)
{
    int     rc = 0;

    if(SCR_tbl_size(args) > 0)
    #ifdef _MSC_VER
        _mkdir((char*) args[0]);
    #else
        mkdir((char*) args[0], 0777);
    #endif

    return(SCR_stracpy((unsigned char*) ""));
}

/**
 *  Syntax: @rmdir(dir)
 *  
 *  Examples
 *  --------
 *      @getdir()                 =>  "C:\Usr\iode"
 *      @mkdir(testdir)           =>  ""          
 *      @chdir(testdir)           =>  "C:\Usr\iode\testdir"
 *      @chdir(..)                =>  "C:\Usr\iode"
 *      @rmdir(testdir)           =>  ""
 */
U_ch *RPF_rmdir(U_ch **args)
{
    int     rc = 0;

    if(SCR_tbl_size(args) > 0)
        _rmdir((char*) args[0]);

    return(SCR_stracpy((unsigned char*) ""));
}
