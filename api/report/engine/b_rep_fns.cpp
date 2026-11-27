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
 *      std::string <function_name>(const std::vector<std::string>& args)
 *  
 *      @param [in] const std::vector<std::string>&  args   List of arguments of the function (sep = ',')
 *      @return     std::string                             computed value of the function
 *      @throw      RPFunctionError                         on error (e.g. illegal number of arguments):
 *                                                          the text of the function is then left unchanged
 *  To add a new @function:
 *      1. create the function based on an existing one. The syntax must be identical
 *      2. add the function name / pointer to the table RP_FNS[] (see at the end of the current file).
 *  
 *  More details can be found here : https://iode.plan.be/doku.php?id=les_fonctions_de_rapports
 *  
 *  List of functions
 *  -----------------
 *      std::string RPF_iode_version(const std::vector<std::string>& args)
 *      std::string RPF_take(const std::vector<std::string>& args)
 *      std::string RPF_drop(const std::vector<std::string>& args)
 *      std::string RPF_replace(const std::vector<std::string>& args)
 *      std::string RPF_equal(const std::vector<std::string>& args)
 *      std::string RPF_upper(const std::vector<std::string>& args)
 *      std::string RPF_lower(const std::vector<std::string>& args)
 *      std::string RPF_sqz(const std::vector<std::string>& args)
 *      std::string RPF_strip(const std::vector<std::string>& args)
 *      std::string RPF_fmtint(const std::vector<std::string>& args)
 *      std::string RPF_ansi(const std::vector<std::string>& args)
 *      std::string RPF_count(const std::vector<std::string>& args)
 *      std::string RPF_index(const std::vector<std::string>& args)
 *      std::string RPF_void(const std::vector<std::string>& args)
 *      std::string RPF_date(const std::vector<std::string>& args)
 *      std::string RPF_time(const std::vector<std::string>& args)
 *      std::string RPF_month(const std::vector<std::string>& args)
 *      std::string RPF_sstderr(const std::vector<std::string>& args)
 *      std::string RPF_srelax(const std::vector<std::string>& args)
 *      std::string RPF_ttitle(const std::vector<std::string>& args)
 *      std::string RPF_cvalue(const std::vector<std::string>& args)
 *      std::string RPF_vvalue(const std::vector<std::string>& args)
 *      std::string RPF_lvalue(const std::vector<std::string>& args)
 *      std::string RPF_ivalue(const std::vector<std::string>& args)
 *      std::string RPF_evalue(const std::vector<std::string>& args)
 *      std::string RPF_eqsample(const std::vector<std::string>& args)
 *      std::string RPF_eqsamplefromto(const std::vector<std::string>& args, int fromto)
 *      std::string RPF_eqsamplefrom(const std::vector<std::string>& args)
 *      std::string RPF_eqsampleto(const std::vector<std::string>& args)
 *      std::string RPF_eqlhsrhs(const std::vector<std::string>& args, int lhsrhs)
 *      std::string RPF_eqlhs(const std::vector<std::string>& args)
 *      std::string RPF_eqrhs(const std::vector<std::string>& args)
 *      std::string RPF_sample(const std::vector<std::string>& args)
 *      int RPF_vsliste1(const std::shared_ptr<CLEC> clec, std::vector<std::string>& tbl, const int type)
 *      std::string RPF_vsliste(const std::vector<std::string>& args, int type)
 *      std::vector<std::string> RPF_unique(const std::vector<std::string>& tbl)
 *      std::string RPF_vliste(const std::vector<std::string>& args)
 *      std::string RPF_sliste(const std::vector<std::string>& args)
 *      std::string RPF_expand(const std::vector<std::string>& args, int type)
 *      std::string RPF_cexpand(const std::vector<std::string>& args)
 *      std::string RPF_eexpand(const std::vector<std::string>& args)
 *      std::string RPF_iexpand(const std::vector<std::string>& args)
 *      std::string RPF_lexpand(const std::vector<std::string>& args)
 *      std::string RPF_sexpand(const std::vector<std::string>& args)
 *      std::string RPF_texpand(const std::vector<std::string>& args)
 *      std::string RPF_vexpand(const std::vector<std::string>& args)
 *      int RPF_CalcPeriod(const std::vector<std::string>& args)
 *      std::string RPF_SimNorm(const std::vector<std::string>& args)
 *      std::string RPF_SimNIter(const std::vector<std::string>& args)
 *      std::string RPF_SimCpu(const std::vector<std::string>& args)
 *      std::string RPF_SimMaxit(const std::vector<std::string>& args)
 *      std::string RPF_SimEps(const std::vector<std::string>& args)
 *      std::string RPF_SimRelax(const std::vector<std::string>& args)
 *      std::string RPF_SimSortNbPasses(const std::vector<std::string>& args)
 *      std::string RPF_SimSortAlgo(const std::vector<std::string>& args)
 *      std::string RPF_SimInitValues(const std::vector<std::string>& args)
 *      std::string RPF_SimCpuSCC(const std::vector<std::string>& args)
 *      std::string RPF_SimCpuSort(const std::vector<std::string>& args)
 *      std::string RPF_vtake(const std::vector<std::string>& args)
 *      std::string RPF_vdrop(const std::vector<std::string>& args)
 *      std::string RPF_vcount(const std::vector<std::string>& args)
 *      std::string RPF_memory(const std::vector<std::string>& args)
 *      std::string RPF_ChronoReset(const std::vector<std::string>& args)
 *      std::string RPF_ChronoGet(const std::vector<std::string>& args)
 *      std::string RPF_fappend(const std::vector<std::string>& args)
 *      std::string RPF_fdelete(const std::vector<std::string>& args)
 *      std::string RPF_getdir(const std::vector<std::string>& args)
 *      std::string RPF_chdir(const std::vector<std::string>& args)
 *      std::string RPF_mkdir(const std::vector<std::string>& args)
 *      std::string RPF_rmdir(const std::vector<std::string>& args)
 *      
 *  
 */
#include "api/iode_scr4.h"
#include "scr4/base/s_prodir.h"
#include "scr4/strs/s_prost.h"
#include "scr4/date/s_prodt.h"

#include "api/pch.h"
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
#endif
#include <time.h>
#include <chrono>


/**
 *  Formats a double value using a printf-like format (e.g. "%lg").
 *  
 *  @param [in] const char*  fmt     printf-like format with one double conversion
 *  @param [in] double       value   value to format
 *  @return     std::string          formatted value
 */
static std::string format_double(const char* fmt, double value)
{
    char buf[128];
    snprintf(buf, sizeof(buf), fmt, value);
    return std::string(buf);
}


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
std::string RPF_take(const std::vector<std::string>& args)
{
    if(args.size() != 2)
        throw RPFunctionError("@take(): 2 arguments required");

    int n = atol(args[0].c_str());
    const std::string& str = args[1];
    int lg = (int) str.size();
    if(n > 0)
    {
        if(n >= lg)
            return str + std::string(n - lg, ' ');
        else
            return str.substr(0, n);
    }
    else
    {
        n = -n;
        if(n >= lg)
            return std::string(n - lg, ' ') + str;
        else
            return str.substr(lg - n);
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
std::string RPF_drop(const std::vector<std::string>& args)
{
    if(args.size() != 2)
        throw RPFunctionError("@drop(): 2 arguments required");

    int n = atol(args[0].c_str());
    const std::string& str = args[1];
    int lg = (int) str.size();
    if(lg == 0)
        throw RPFunctionError("@drop(): the string is empty");

    if(n > 0)
    {
        if(n >= lg)
            return "";
        return str.substr(n);
    }
    else
    {
        n = -n;
        if(n >= lg)
            return "";
        return str.substr(0, lg - n);
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
std::string RPF_replace(const std::vector<std::string>& args)
{
    if(args.size() != 3)
        throw RPFunctionError("@replace(): 3 arguments required");

    std::string res = args[0];
    replace_all(res, args[1], args[2]);
    return res;
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
 */
std::string RPF_equal(const std::vector<std::string>& args)
{
    if(args.size() != 2)
        return "0";
    return (args[0] == args[1]) ? "1" : "0";
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
std::string RPF_upper(const std::vector<std::string>& args)
{
    std::string res = join(args, " ");
    // SCR_upper() also converts the CP850 accented letters
    SCR_upper((unsigned char*) res.data());
    return res;
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
std::string RPF_lower(const std::vector<std::string>& args)
{
    std::string res = join(args, " ");
    // SCR_lower() only converts A-Z (std::tolower() depends on the current locale)
    SCR_lower((unsigned char*) res.data());
    return res;
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
std::string RPF_sqz(const std::vector<std::string>& args)
{
    std::string res = join(args, " ");
    // removes only the ' ' characters (not the tabs)
    res.erase(std::remove(res.begin(), res.end(), ' '), res.end());
    return res;
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
std::string RPF_strip(const std::vector<std::string>& args)
{
    std::string res = join(args, " ");
    // removes all the trailing characters <= ' ' (spaces, tabs, control characters)
    while(!res.empty() && (unsigned char) res.back() <= ' ')
        res.pop_back();
    return res;
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
std::string RPF_fmtint(const std::vector<std::string>& args)
{
    std::string fmt;
    switch(args.size())
    {
        case 0 :
            return "";
        case 2 :
            fmt = args[1];
            break;
        default :
            fmt = "999999";
            break;
    }

    // the result has the same length as the format
    std::string res(fmt.size(), ' ');
    SCR_fmt_long((unsigned char*) res.data(), (unsigned char*) fmt.data(), atol(args[0].c_str()));
    return res;
}


/**
 *  Syntax: @ansi(oem_text)
 *  
 *  
 *  TODO: add fn  @oem / @utf8
 */
std::string RPF_ansi(const std::vector<std::string>& args)
{
    if(args.size() < 1)
        throw RPFunctionError("@ansi(): 1 argument required");

    std::string res = args[0];
    // in-place conversion
    SCR_OemToAnsi((unsigned char*) res.data(), (unsigned char*) res.data());
    return res;
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
std::string RPF_count(const std::vector<std::string>& args)
{
    return std::to_string(args.size());
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
std::string RPF_index(const std::vector<std::string>& args)
{
    int n = (int) args.size() - 1;
    if(n < 1)
        return "";

    int pos = atol(args[0].c_str());
    if(pos > n || pos < 1)
        return "";
    return args[pos];
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
std::string RPF_void(const std::vector<std::string>& args)
{
    return "";
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
 *  @see long_to_date() in api/utils/utils.h for more details
 */
std::string RPF_date(const std::vector<std::string>& args)
{
    std::string fmt = args.empty() ? std::string("dd-mm-yyyy") : args[0];
    return long_to_date(current_date_to_long(), fmt);
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
std::string RPF_time(const std::vector<std::string>& args)
{
    std::string fmt = args.empty() ? std::string("hh:mm:ss") : args[0];
    // the result has the same length as the format
    std::string res(fmt.size(), ' ');
    SCR_long_to_ftime(SCR_current_time(), res.data(), fmt.data());
    return res;
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
std::string RPF_month(const std::vector<std::string>& args)
{
    if(args.size() < 1)
        throw RPFunctionError("@month(): 1 argument required");

    int m = atoi(args[0].c_str());
    if(m < 1 || m > 12)
        throw RPFunctionError("@month(): the month must be between 1 and 12");

    int lang = 'F';
    if(args.size() >= 2)
        lang = SCR_upper_char((unsigned char) args[1][0]);

    switch(lang)
    {
        case 'N' :
            return std::string((char*) RPF_MONTHSN[m - 1]);
        case 'F' :
            return std::string((char*) RPF_MONTHSF[m - 1]);
        default  :
            return std::string((char*) RPF_MONTHSE[m - 1]);
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
std::string RPF_sstderr(const std::vector<std::string>& args)
{
    if(args.empty())
        throw RPFunctionError("@sstderr(): at least 1 argument required");

    std::vector<std::string> values;
    for(const std::string& arg : args)
    {
        std::string name = global_ws_scl->to_key(arg);
        if(!global_ws_scl->contains(name))
        {
            values.push_back("--");
            continue;
        }

        std::shared_ptr<Scalar> scl_ptr = global_ws_scl->get_obj_ptr(name);
        if(IODE_IS_A_NUMBER(scl_ptr->std))
            values.push_back(std::to_string((double) scl_ptr->std));     // same as "%lf"
        else
            values.push_back("--");
    }
    return join(values, " ");
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
std::string RPF_srelax(const std::vector<std::string>& args)
{
    if(args.empty())
        throw RPFunctionError("@srelax(): at least 1 argument required");

    std::vector<std::string> values;
    for(const std::string& arg : args)
    {
        std::string name = global_ws_scl->to_key(arg);
        if(!global_ws_scl->contains(name))
        {
            values.push_back("--");
            continue;
        }

        std::shared_ptr<Scalar> scl_ptr = global_ws_scl->get_obj_ptr(name);
        if(IODE_IS_A_NUMBER(scl_ptr->relax))
            values.push_back(std::to_string((double) scl_ptr->relax));   // same as "%lf"
        else
            values.push_back("--");
    }
    return join(values, " ");
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
 *                                   Coin salarial parafiscal"
 */
std::string RPF_ttitle(const std::vector<std::string>& args)
{
    if(args.empty())
        throw RPFunctionError("@ttitle(): at least 1 argument required");

    std::vector<std::string> titles;
    for(const std::string& arg : args)
    {
        std::string name = global_ws_tbl->to_key(arg);
        if(!global_ws_tbl->contains(name))
            titles.push_back("Table " + arg + " not found");
        else
            titles.push_back(global_ws_tbl->get_obj_ptr(name)->get_title(false));
    }
    return join(titles, "\n");
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
std::string RPF_cvalue(const std::vector<std::string>& args)
{
    KDBCommentsPtr kdb = global_ws_cmt;
    if(!kdb)
        throw RPFunctionError("@cvalue(): no Comments workspace");
    if(args.empty())
        throw RPFunctionError("@cvalue(): at least 1 argument required");

    std::string res;
    for(size_t i = 0; i < args.size(); i++)
    {
        std::string name = kdb->to_key(args[i]);
        if(i > 0)
            res += ";";
        if(!kdb->contains(name))
            res += "Cmt " + args[i] + " not found";
        else
        {
            Comment comment = kdb->get(name);
            res += comment.c_str();
            replace_all(res, "\n", " ");
        }
    }
    return res;
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
std::string RPF_vvalue(const std::vector<std::string>& args)
{
    char    buf[128];
    double  *val;
    KDBVariablesPtr kdb = global_ws_var;

    if(!kdb)
        throw RPFunctionError("@vvalue(): no Variables workspace");

    if(!kdb->get_sample() || kdb->get_sample()->nb_periods == 0)
        return "";

    if(args.empty())
        throw RPFunctionError("@vvalue(): at least 1 argument required");

    std::string res;
    for(size_t i = 0; i < args.size(); i++)
    {
        std::string name = kdb->to_key(args[i]);
        if(i > 0)
            res += ";";
        if(!kdb->contains(name))
            res += "VAR " + args[i] + " not found";
        else
        {
            val = kdb->get_var_ptr(name);
            for(int j = 0 ; j < kdb->get_sample()->nb_periods; j++, val++)
            {
                IodeFmtVal(buf, *val);
                res += buf;
                res += " ";
            }
        }
    }
    return res;
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
std::string RPF_lvalue(const std::vector<std::string>& args)
{
    KDBListsPtr kdb = global_ws_lst;
    if(!kdb)
        throw RPFunctionError("@lvalue(): no Lists workspace");
    if(args.empty())
        throw RPFunctionError("@lvalue(): at least 1 argument required");

    std::string res;
    for(size_t i = 0; i < args.size(); i++)
    {
        std::string name = kdb->to_key(args[i]);
        if(i > 0)
            res += ",";
        if(!kdb->contains(name))
            res += "List " + args[i] + " not found";
        else
        {
            List lst = kdb->get(name);
            res += lst.c_str();
            replace_all(res, ";", ",");
        }
    }
    return res;
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
std::string RPF_ivalue(const std::vector<std::string>& args)
{
    KDBIdentitiesPtr kdb = global_ws_idt;
    if(!kdb)
        throw RPFunctionError("@ivalue(): no Identities workspace");
    if(args.empty())
        throw RPFunctionError("@ivalue(): at least 1 argument required");

    std::string res;
    for(size_t i = 0; i < args.size(); i++)
    {
        std::string name = kdb->to_key(args[i]);
        if(i > 0)
            res += ";";
        if(!kdb->contains(name))
            res += "Idt " + args[i] + " not found";
        else
        {
            res += kdb->get_obj_ptr(name)->get_lec();
            replace_all(res, "\n", " ");
        }
    }
    return res;
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
std::string RPF_evalue(const std::vector<std::string>& args)
{
    KDBEquationsPtr kdb = global_ws_eqs;
    if(!kdb)
        throw RPFunctionError("@evalue(): no Equations workspace");
    if(args.empty())
        throw RPFunctionError("@evalue(): at least 1 argument required");

    std::string res;
    for(size_t i = 0; i < args.size(); i++)
    {
        std::string name = kdb->to_key(args[i]);
        if(i > 0)
            res += ";";
        if(!kdb->contains(name))
            res += "Eqs " + args[i] + " not found";
        else
        {
            res += kdb->get_obj_ptr(name)->lec;
            replace_all(res, "\n", " ");
        }
    }
    return res;
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
std::string RPF_eqsample(const std::vector<std::string>& args)
{
    KDBEquationsPtr kdb = global_ws_eqs;

    // Equation WS  empty
    if(!kdb)
        throw RPFunctionError("@eqsample(): no Equations workspace");

    // 1! eq
    if(args.size() != 1)
        throw RPFunctionError("@eqsample(): 1 argument required");

    std::string name = kdb->to_key(args[0]);
    if(!kdb->contains(name))
        return "[Eqs " + args[0] + " not found]";

    std::shared_ptr<Equation> eq = kdb->get_obj_ptr(name);
    return eq->sample.to_string();
}


/**
 *  Sub fonction  of RPF_eqsamplefrom() and RPF_eqsampleto().
 *  
 *  @param [in] const std::vector<std::string>&  args    args of RPF_eqsample*() => equation name
 *  @param [in] int     fromto  0 if sample.from must be returned, 1 for sample.to
 *  @return     std::string                         sample.from or sample.to in text format
 *                              " " if there is no sample in the equation
 *                              "[Eq ... not found]" if the eq does not exist
 */
std::string RPF_eqsamplefromto(const std::vector<std::string>& args, int fromto)
{
    KDBEquationsPtr kdb = global_ws_eqs;

    // Equation WS  empty
    if(!kdb)
        throw RPFunctionError("@eqsamplefrom/to(): no Equations workspace");

    // 1! eq
    if(args.size() != 1)
        throw RPFunctionError("@eqsamplefrom/to(): 1 argument required");

    std::string res;
    std::string name = kdb->to_key(args[0]);
    if(!kdb->contains(name))
        res = "[Eqs " + args[0] + " not found]";
    else
    {
        std::shared_ptr<Equation> eq = kdb->get_obj_ptr(name);
        if(fromto == 0)
            res = eq->sample.start_period.to_string();
        else
            res = eq->sample.end_period.to_string();
    }

    if(res.empty())
        res = " ";   // pour éviter de quitter le rapport si sample vide

    return res;
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
std::string RPF_eqsamplefrom(const std::vector<std::string>& args)
{
    return RPF_eqsamplefromto(args, 0);
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
std::string RPF_eqsampleto(const std::vector<std::string>& args)
{
    return RPF_eqsamplefromto(args, 1);
}

/**
 *  Sub fonction  of RPF_eqlhs() and RPF_eqrhs().
 *  
 *  @param [in] const std::vector<std::string>&  args    args of RPF_eq?hs() => equation name
 *  @param [in] int     lhsrhs  0 for the LHS, 1 foir the RHS of the equation
 *  @return     std::string                         LHS or RHS 
 *                              "[Eq ... not found]" if the eq does not exist
 */
std::string RPF_eqlhsrhs(const std::vector<std::string>& args, int lhsrhs)
{
    KDBEquationsPtr kdb = global_ws_eqs;

    // Equation WS  empty
    if(!kdb)
        throw RPFunctionError("@eqlhs/rhs(): no Equations workspace");

    // 1! eq
    if(args.size() != 1)
        throw RPFunctionError("@eqlhs/rhs(): 1 argument required");

    std::string name = kdb->to_key(args[0]);
    if(!kdb->contains(name))
        return "[Eqs " + args[0] + " not found]";

    std::string lec = kdb->get_obj_ptr(name)->lec;
    int poscolon = split_eq(lec);
    if(poscolon > 0)
    {
        if(lhsrhs == 0)
            lec = lec.substr(0, poscolon);
        else
            lec = lec.substr(poscolon + 2);     // skip ":="
    }
    return lec;
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
std::string RPF_eqlhs(const std::vector<std::string>& args)
{
    return RPF_eqlhsrhs(args, 0);
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
std::string RPF_eqrhs(const std::vector<std::string>& args)
{
    return RPF_eqlhsrhs(args, 1);
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
std::string RPF_sample(const std::vector<std::string>& args)
{
    KDBVariablesPtr kdb = global_ws_var;
    std::shared_ptr<Sample> smpl = kdb->get_sample();
    if(!smpl || smpl->nb_periods == 0)
        throw RPFunctionError("@sample(): the sample of the Variables workspace is not defined");

    char what = args.empty() ? 'F' : args[0][0];
    what = (char) std::toupper((unsigned char) what);

    std::string s_per1 = smpl->start_period.to_string();
    std::string s_per2 = smpl->end_period.to_string();
    switch(what)
    {
        case 'B':
            return s_per1;
        case 'E':
            return s_per2;
        default :
            return s_per1 + " " + s_per2;
    }
}


/**
 *  Searches in a CLEC struct the list of scalars or variables and adds to tbl the new names found.
 *
 *  @param [in]      std::shared_ptr<CLEC>      clec    input CLEC structure
 *  @param [in, out] std::vector<std::string>&  tbl     names of the scalars or variables already found
 *  @param [in]      int                        type    type of object to retrieve: 'S' for scalars, 'V' for variables
 *  @return          int                                0, always
 */
int RPF_vsliste1(const std::shared_ptr<CLEC> clec, std::vector<std::string>& tbl, const int type)
{
    if(type != 'S' && type != 'V')
        return 0;

    const std::vector<std::string>& names = (type == 'S') ? clec->v_coeff_names : clec->v_var_names;
    for(const std::string& name : names)
    {
        // adds the name only once
        if(std::find(tbl.begin(), tbl.end(), name) == tbl.end())
            tbl.push_back(name);
    }

    return 0;
}


/**
 *  Searches in a list of equations all objects of a given type ('S' or 'V').
 *  
 *  @param [in] const std::vector<std::string>&  args    list of equations to analyse
 *  @param [in] int     type    type of objects to retrieve:  'S' for scalars, 'V' for variables
 *  @return     std::string                         found object names separated by semi-colon (';')
 */
std::string RPF_vsliste(const std::vector<std::string>& args, int type)
{
    if(args.empty())
        throw RPFunctionError("@vliste/sliste(): at least 1 argument required");

    std::vector<std::string> tbl;
    for(const std::string& arg : args)
    {
        std::string name = global_ws_eqs->to_key(arg);
        if(!global_ws_eqs->contains(name))
            continue;
        std::shared_ptr<Equation> eq_ptr = global_ws_eqs->get_obj_ptr(name);
        RPF_vsliste1(eq_ptr->clec, tbl, type);
    }

    return join(tbl, ";");
}


/**
 *  Returns a list of unique strings in a table of strings.
 *  
 *  @param [in] const std::vector<std::string>&  tbl     input table of strings
 *  @return     std::vector<std::string>            unique strings of tbl (in the order of their first occurrence)
 */
std::vector<std::string> RPF_unique(const std::vector<std::string>& tbl)
{
    std::vector<std::string> tbl2;
    for(const std::string& str : tbl)
    {
        if(std::find(tbl2.begin(), tbl2.end(), str) == tbl2.end())
            tbl2.push_back(str);
    }
    return tbl2;
}


/**
 *  Syntax: @vliste(eq1[,eq2...])
 *  
 *  Examples 
 *  --------
 *      @vliste(ACAF)             =>  "ACAF;VAF;GOSF;TIME"
 *      @vliste(ACAF,ACAG)        =>  "ACAF;VAF;GOSF;TIME;ACAG;VBBP"
 */
std::string RPF_vliste(const std::vector<std::string>& args)
{
    return RPF_vsliste(args, 'V');
}


/**
 *  Syntax: @sliste(eq1[,eq2...])
 *  
 *  Examples 
 *  --------
 *      @sliste(ACAF)             =>  "acaf1;acaf2;acaf4"
 *      @sliste(ACAF,ACAG)        =>  "acaf1;acaf2;acaf4"
 */
std::string RPF_sliste(const std::vector<std::string>& args)
{
    return RPF_vsliste(args, 'S');
}


/**
 *  Expands a list of name patterns for a given object type.
 *  
 *  @param [in] const std::vector<std::string>&  args    table of name patterns
 *  @param [in] int     type    object type
 *  @return     std::string                         list of object names sep by ";"
 */
std::string RPF_expand(const std::vector<std::string>& args, int type)
{
    std::vector<std::string> names;
    for(const std::string& arg : args)
    {
        std::string pattern = arg;
        char* tmp = K_expand(type, 0L, pattern.data(), (int) '*');
        if(tmp)
        {
            for(const std::string& name : split_skip_empty(std::string(tmp), ";"))
                names.push_back(name);
            SCR_free(tmp);
        }
    }

    // removes the duplicates (keeps the first occurrence)
    names = RPF_unique(names);
    return join(names, ";");
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
std::string RPF_cexpand(const std::vector<std::string>& args)
{
    return RPF_expand(args, COMMENTS);
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
std::string RPF_eexpand(const std::vector<std::string>& args)
{
    return RPF_expand(args, EQUATIONS);
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
std::string RPF_iexpand(const std::vector<std::string>& args)
{
    return RPF_expand(args, IDENTITIES);
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
std::string RPF_lexpand(const std::vector<std::string>& args)
{
    return RPF_expand(args, LISTS);
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
std::string RPF_sexpand(const std::vector<std::string>& args)
{
    return RPF_expand(args, SCALARS);
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
std::string RPF_texpand(const std::vector<std::string>& args)
{
    return RPF_expand(args, TABLES);
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
std::string RPF_vexpand(const std::vector<std::string>& args)
{
    return RPF_expand(args, VARIABLES);
}



/****************** SIMULATION ***************************/

/**
 *  Calculated the position in the current WS of the period args[0].
 *  
 *  @param [in] const std::vector<std::string>& args     argument (args[0] = period)
 *  @return     int              position of the period of -1 on error (out of range...) 
 */
int RPF_CalcPeriod(const std::vector<std::string>& args)
{
    // Check args
    if(args.empty())
        return -1; // Erreur, arg required

    Period per(args[0]);
    if(per.year == 0)
        return -1; // Error

    // Calc diff bw per and WS sample
    int t = per.difference(global_ws_var->get_sample()->start_period);
    if(t < 0)
        return -1;

    int at = per.difference(global_ws_var->get_sample()->end_period);
    if(at > 0)
        return -1;

    return t;
}


/**
 *  Syntax: @SimMaxit()
 *  
 *  Examples
 *  --------
 *      @SimMaxit()             =>  "100"
 */
std::string RPF_SimMaxit(const std::vector<std::string>& args)
{
    return std::to_string(global_simu->max_iter);
}


/**
 *  Syntax: @SimEps()
 *  
 *  Examples
 *  --------
 *      @SimEps()               =>  "0.001"
 */
std::string RPF_SimEps(const std::vector<std::string>& args)
{
    return format_double("%lg", global_simu->epsilon);
}


/**
 *  Syntax: @SimRelax()
 *  
 *  Examples
 *  --------
 *        @SimRelax()             =>  "1.000000"
 */
std::string RPF_SimRelax(const std::vector<std::string>& args)
{
    return std::to_string(global_simu->relax);      // same as "%lf"
}


/**
 *  Syntax: @SimSortNbPasses()
 *  
 *  Examples
 *  --------
 *        @SimSortNbPasses()        =>  "5"
 */
std::string RPF_SimSortNbPasses(const std::vector<std::string>& args)
{
    return std::to_string(global_simu->nb_passes);
}

/**
 *  Syntax: @SimSortAlgo()
 *  
 *  Examples
 *  --------
 *        @SimSortAlgo()        =>  "Triangulation"
 */
std::string RPF_SimSortAlgo(const std::vector<std::string>& args)
{
    static const char* algos[] = {"Connex", "Triangulation", "None"};

    int isort = global_simu->sorting_algo;
    if(isort < 0 || isort > 2)
        isort = 1; // Default value

    return std::string(algos[isort]);
}


/**
 *  Syntax: @SimInitValues()
 *  
 *  Examples
 *  --------
 *        @SimInitValues()        =>  "1"
 */
std::string RPF_SimInitValues(const std::vector<std::string>& args)
{
    return std::to_string(global_simu->init_algo);
}


/**
 *  Sub-fn of RPF_SimNorm().
 *  
 *  @param [in] args const std::vector<std::string>&  period from which norm has to be to retrieved
 *  @return          double convergence criteria of the period arg or -1.0 if no simulation       
 */
double RPF_SimNormReal(const std::vector<std::string>& args)
{
    int t = RPF_CalcPeriod(args);
    if(t < 0)
        return -1.0;

    // Pas encore de simulation
    if(global_simu->v_norm.empty())
        return -1.0;

    // Return norme t
    return global_simu->v_norm[t];
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
std::string RPF_SimNorm(const std::vector<std::string>& args)
{
    double value = RPF_SimNormReal(args);
    if(value < 0)
        return "";
    return format_double("%lg", value);
}

/**
 *  Subfunction of RPF_SimNIter().
 *  
 *  @param [in] args    const std::vector<std::string>&  period  (eg: args[0] => "2001Y1")
 *  @return             int     nb of iterations to converge in the last simulation for period arg[0]
 */
int RPF_SimNIterInt(const std::vector<std::string>& args)
{
    int t = RPF_CalcPeriod(args);
    if(t < 0)
        return -1;

    // Pas encore de simulation
    if(global_simu->v_nb_iterations.empty())
        return -1;

    // Return norme t
    return global_simu->v_nb_iterations[t];
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
std::string RPF_SimNIter(const std::vector<std::string>& args)
{
    int niter = RPF_SimNIterInt(args);
    if(niter < 0)
        return "";
    return std::to_string(niter);
}


/**
 *  Subfunction of RPF_SimCpu().
 *  
 *  @param [in] args    const std::vector<std::string>&  period  (eg: args[0] => "2001Y1")
 *  @return             int     time (in ms) elapsed to reach convergence in the last simulation for period arg[0]
 */
int RPF_SimCpuInt(const std::vector<std::string>& args)
{
    int t = RPF_CalcPeriod(args);
    if(t < 0)
        return -1;

    // No simulation done yet
    if(global_simu->v_cpu_time.empty())
        return -1;

    return global_simu->v_cpu_time[t];
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
std::string RPF_SimCpu(const std::vector<std::string>& args)
{
    int cpu = RPF_SimCpuInt(args);
    if(cpu < 0)
        return "";
    return std::to_string(cpu);
}



/**
 *  Syntax: @SimCpuSCC()
 *  
 *  Examples
 *  --------
 *        @SimCpuSCC()        =>  "31343"
 */
std::string RPF_SimCpuSCC(const std::vector<std::string>& args)
{
    return std::to_string(global_simu->cpu_time_scc);
}

/**
 *  Syntax: @SimCpuSort()
 *  
 *  Examples
 *  --------
 *        @SimCpuSort()        =>  "1234"
 */
std::string RPF_SimCpuSort(const std::vector<std::string>& args)
{
    return std::to_string(global_simu->cpu_time_sorting);
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
std::string RPF_vtake(const std::vector<std::string>& args)
{
    int     n, lg;

    if(RP_VSEPS == 0) RP_vseps("");
    if(args.empty())
        return "";

    std::string sep(1, RP_VSEPS[0]);
    std::vector<std::string> v_args = split_skip_empty(join(args, sep), std::string(RP_VSEPS));
    lg = (int) v_args.size() - 1;
    if(lg <= 0)
        return "";

    std::vector<std::string> v_res;
    n = atol(v_args[0].c_str());
    if(n > 0) {
        n = std::min(lg, n);
        v_res.assign(v_args.begin() + 1, v_args.begin() + 1 + n);
    }
    else {
        n = lg - std::min(lg, -n);
        v_res.assign(v_args.begin() + n + 1, v_args.end());
    }
    return join(v_res, sep);
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
std::string RPF_vdrop(const std::vector<std::string>& args)
{
    int     n, lg;

    if(RP_VSEPS == 0) RP_vseps("");
    if(args.empty())
        return "";

    std::string sep(1, RP_VSEPS[0]);
    std::vector<std::string> v_args = split_skip_empty(join(args, sep), std::string(RP_VSEPS));
    lg = (int) v_args.size() - 1;
    if(lg <= 0)
        return "";

    std::vector<std::string> v_res;
    n = atol(v_args[0].c_str());
    if(n < 0) {
        n = std::max(lg + n, 0);
        v_res.assign(v_args.begin() + 1, v_args.begin() + 1 + n);
    }
    else {
        n = std::min(lg, n);
        v_res.assign(v_args.begin() + n + 1, v_args.end());
    }
    return join(v_res, sep);
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
std::string RPF_vcount(const std::vector<std::string>& args)
{
    if(RP_VSEPS == 0) RP_vseps("");
    std::string sep(1, RP_VSEPS[0]);
    std::vector<std::string> v_args = split_skip_empty(join(args, sep), std::string(RP_VSEPS));
    return std::to_string(v_args.size());
}


/*---------------------- SYSTEM --------------------------------*/

/**
 *  Syntax: @Version()
 *  
 *  Examples
 *  --------
 *        @IodeVersion()             =>  "6.69"
 */
std::string RPF_IodeVersion(const std::vector<std::string>& args)
{
    return std::to_string(IODE_VERSION_MAJOR) + "." + std::to_string(IODE_VERSION_MINOR);
}




/**
 *  Syntax: @memory()
 *  
 *  Examples 
 *  --------
 *      @memory()    =>  "NBA=1260, NBB=15, ALLOC=0.5 MB, FREE=2048.0 MB, USED=0.4 MB "
 */
std::string RPF_memory(const std::vector<std::string>& args)
{
    char     buf[256];
    double   convfree, convused, convalloc;

    SWSTAT  st;

    SW_status(&st);

    convalloc = ((double)st.st_convalloc)/(1024.0 * 1024.0),
    convfree = ((double)st.st_convafree + (double)st.st_convfree)/(1024.0*1024.0);
    convused = ((double)st.st_convalloc - (double)st.st_convafree)/(1024.0*1024.0);

    snprintf(buf, sizeof(buf), "NBA=%ld, NBB=%ld, ALLOC=%.1lf MB, FREE=%.1lf MB, USED=%.1lf MB",
            st.st_nballoc - st.st_nbfree,
            st.st_nbblks,
            //(st.st_convalloc)/(1024 * 1024),
            //(st.st_convafree + st.st_convfree)/(1024L*1024L),
            //(st.st_convalloc - st.st_convafree)/(1024L*1024L));  // JMP 29/09/2021 : misplaced )
            convalloc, convfree, convused);

    return std::string(buf);
}

/*--------------------------- CHRONO ------------------------------- */

static std::chrono::steady_clock::time_point RPF_CHRONO = std::chrono::steady_clock::now();     // Current chrono initial time

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
std::string RPF_ChronoReset(const std::vector<std::string>& args)
{
    RPF_CHRONO = std::chrono::steady_clock::now();
    return "";
}


/**
 *  Returns the number of millisecond elapsed since the last call to RPF_ChronoReset().
 *  
 *  Syntax: @chronoget()
 *  
 *  Example
 *  -------
 *      @chronoget()              =>  "47"
 */
std::string RPF_ChronoGet(const std::vector<std::string>& args)
{
    auto elapsed = std::chrono::steady_clock::now() - RPF_CHRONO;
    long long ms = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
    return std::to_string(ms);
}


/*--------------------------- FILES AND DIRECTORIES ------------------------------- */

/**
 *  Syntax: @fappend(filename,line,line...) where line may be NL for newline
 *  
 *  Examples 
 *  --------
 *     @fappend(essais.txt,"line 1",NL,line 2,NL,NL,line 4)  =>  ""
 */
std::string RPF_fappend(const std::vector<std::string>& args)
{
    if(args.size() < 1)
        throw RPFunctionError("@fappend(): at least 1 argument required");

    std::ofstream fd(args[0], std::ios::app);
    if(!fd)
        throw RPFunctionError("@fappend(): cannot open the file '" + args[0] + "'");

    for(size_t i = 1; i < args.size(); i++)
    {
        if(args[i] == "NL")
            fd << "\n";
        else
            fd << args[i];
    }
    return "";
}


/**
 *  Syntax: @fdelete(filename)
 *  
 *  Examples 
 *  --------
 *     @fdelete(essais.txt)  =>  ""
 */
std::string RPF_fdelete(const std::vector<std::string>& args)
{
    if(args.size() < 1)
        throw RPFunctionError("@fdelete(): 1 argument required");

    std::remove(args[0].c_str());
    return "";
}


/**
 *  Syntax: @getdir()
 *  
 *  Examples
 *  --------
 *      `@getdir()`                 =>  "C:\Usr\iode"
 */
std::string RPF_getdir(const std::vector<std::string>& args)
{
    std::error_code ec;
    std::filesystem::path dir = std::filesystem::current_path(ec);
    if(ec)
        return "";

    try
    {
        return dir.string();
    }
    catch(const std::exception&)
    {
        // the path cannot be converted to the current code page
        return "";
    }
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
std::string RPF_chdir(const std::vector<std::string>& args)
{
    if(args.size() > 0)
    {
        std::error_code ec;
        std::filesystem::current_path(std::filesystem::path(args[0]), ec);
    }

    ksettitle();
    return RPF_getdir({});
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
std::string RPF_mkdir(const std::vector<std::string>& args)
{
    if(args.size() > 0)
    {
        std::error_code ec;
        std::filesystem::create_directory(std::filesystem::path(args[0]), ec);
    }
    return "";
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
std::string RPF_rmdir(const std::vector<std::string>& args)
{
    if(args.size() > 0)
    {
        // removes only an empty directory (like _rmdir()): std::filesystem::remove() also removes files
        std::error_code ec;
        std::filesystem::path dir(args[0]);
        if(std::filesystem::is_directory(dir, ec))
            std::filesystem::remove(dir, ec);
    }
    return "";
}
