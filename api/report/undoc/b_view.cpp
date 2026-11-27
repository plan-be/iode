/**
 *  @header4iode
 *  
 *  Functions to display or print calculated tables and tables of variables. The same functions are used
 *  to print and to display tables as graphs or as text.
 *  
 *  The functions generate IODE tables in A2M format based on Table structures and GSample definition.
 *  
 *  List of functions 
 *  -----------------
 *      int B_ViewVar(const std::string& arg, int unused)                | Display a list of variables in the form of tables of max 50 variables.
 *      int B_PrintVar(const std::string& arg, int unused)               | Print a list of variables in the form of tables of max 50 variables.
 *      int B_ViewPrintVar(const std::string& arg, int mode)             | Print or display (according to the mode parameter) variables in the form of tables. 
 *      int B_ViewByTbl(const std::string& arg, int unused)              | $ViewTbl sample table [list of tables]
 *      int B_ViewTbl(const std::string& arg, int unused)                | Alias of B_ViewByTbl()
 *      int B_PrintTbl(const std::string& arg, int unused)               | $PrintTbl gsample table1 [table2...]
 *      int B_ViewGr(const std::string& arg, int unused)                 | $ViewGr gsample tbl1[+tbl2] tbl3 ... 
 *      int B_PrintGr(const std::string& arg, int unused)                | $PrintGr gsample table1 [table2...]
 *      int B_ViewPrintTbl_1(const std::string& c_name, const std::string& smpl)        | Calculate and display (or print according to the value of B_viewmode) a table on a specified GSample.
 *      int B_ViewPrintGr_1(const std::string& names, const std::string& gsmpl)       | Calculate and display (or print according to the value of B_viewmode) a graph on a specified GSample, based on Table definition(s).
 *      int B_ViewPrintTbl(const std::string& arg, int type, int mode)   | Calculate, then print or display (according to the mode parameter) IODE Tables either in the form of graphs or in the form of text (SCROLLs).
 *      int B_ViewTblFile(const std::string& arg, int unused)            | $PrintTblFile n varfilename    (n := 2, 3, 4, 5)
 *      int B_ViewTblEnd()                                  | Close a Print tables or Print variables session.
 */
#include "api/iode_scr4.h"

#include "api/pch.h"
#include "api/k_lang.h"
#include "api/b_args.h"
#include "api/b_errors.h"
#include "api/objs/objs.h"
#include "api/objs/pack.h"
#include "api/objs/grep.h"
#include "api/print/print.h"
#include "api/write/write.h"
#include "api/computed_table/computed_table.h"

#include "api/report/engine/engine.h"
#include "api/report/undoc/undoc.h"



int B_viewmode;         // 0: displays the graph/table on screen, 1: print graph/table

int     ODE_VIEW = 0;   // Used in the DOS GUI to differentiate between display & print
char    ODE_SMPL[81];   // Current GSample for printing / displaying tables and graphs


// Display a list of variables in the form of tables of max 50 variables.
int B_ViewVar(const std::string& arg, int unused)
{
    return(B_ViewPrintVar(arg, 0));
}

// Print a list of variables in the form of tables of max 50 variables.
int B_PrintVar(const std::string& arg, int unused)
{
    return(B_ViewPrintVar(arg, 1));
}


/**
 *  Print or display (according to the mode parameter) variables in the form of tables. 
 *  Temporary tables of maximum 50 variables are constructed then printed of displayed.
 *  
 *  @param [in] arg  const std::string&  sample + list of variables to be printed
 *  @param [in] mode int                 0 to display, 1 to print
 *  @return          int
 */
int B_ViewPrintVar(const std::string& arg, int mode)
{
    std::string arg_str(arg);
    arg_str = trim(arg_str);
    if(arg_str.empty()) 
    {
        error_manager.append_error("Invalid argument");
        return -1;
    }

    size_t pos = arg_str.find(' ');
    if(pos == std::string::npos) 
    {
        error_manager.append_error("Invalid argument");
        return -1;
    }
    std::string sample = arg_str.substr(0, pos);
    std::string vars_str = arg_str.substr(pos + 1);
    vars_str = trim(vars_str);

    std::vector<std::string> vars = split_multi(vars_str, ";\t\n");
    if(vars.size() == 0) 
    {
        error_manager.append_error("Invalid argument");
        W_flush();
        B_ViewTblEnd();
        return -1;
    }

    int rc = 0;
    int chunk_size = 50;
    int nb_decimals = tbl_nb_decimals;
    bool search_comment = false;
    ComputedTable* computed_table = nullptr;
    for (size_t start = 0; start < vars.size(); start += chunk_size) 
    {
        auto end = std::min(start + chunk_size, vars.size());
        std::vector<std::string> chunks(vars.begin() + start, vars.begin() + end);
        if(mode == 0)
        {
            Table tbl(2, "", chunks, false, false, false, search_comment);
            rc = T_view_tbl(&tbl, (char*) sample.c_str(), "TABLE_OF_VARIABLES");
        }
        else 
        {
            try
            {                
                std::shared_ptr<Table> tbl_ptr = std::make_shared<Table>(2, "", chunks, true, true, true, search_comment);
                computed_table = new ComputedTable(tbl_ptr.get(), sample, nb_decimals); 
                computed_table->print_to_file(true, true);
                delete computed_table;
            }
            catch(const std::exception&)
            {
                rc = -1;
            }
        }

        // something went wrong -> exit loop
        if(rc != 0) 
            break;
    }

    // NOTE: do not call W_close() to give the possibility to append 
    //       new tables in the same open file 
    W_flush();

    B_ViewTblEnd();
    return rc;
}


// Display a list of tables
// $ViewTbl sample table [list of tables]
int B_ViewByTbl(const std::string& arg, int unused)
{
    return(B_ViewPrintTbl(arg, 0, 0));
}

// Alias of B_ViewByTbl()
int B_ViewTbl(const std::string& arg, int unused)
{
    return(B_ViewPrintTbl(arg, 0, 0));
}

// Print a list of tables
// $PrintTbl gsample table1 [table2...]
int B_PrintTbl(const std::string& arg, int unused)
{
    return(B_ViewPrintTbl(arg, 0, 1));
}


// Display a list of tables as graphs
// $ViewGr gsample tbl1[+tbl2] tbl3 ... 
int B_ViewGr(const std::string& arg, int unused)
{
    return(B_ViewPrintTbl(arg, 1, 0));
}

// Print a list of tables as graphs
// $PrintGr gsample table1 [table2...]
int B_PrintGr(const std::string& arg, int unused)
{
    return(B_ViewPrintTbl(arg, 1, 1));
}


/**
 *  Calculate and display (or print, according to the value of B_viewmode) a table on a specified GSample.
 *  
 *  @param [in] c_name const std::string&    table name
 *  @param [in] smpl   const std::string&    GSample
 *  @return            int                   0 on success, -1 on error (table not found, illegal gsample...)
 */
int B_ViewPrintTbl_1(const std::string& c_name, const std::string& smpl)
{
    std::string name = global_ws_tbl->to_key(c_name);
    if(!global_ws_tbl->contains(name)) 
    {
        error_manager.append_error("Table '" + std::string(name) + "' not found");
        return -1;
    }

    int rc = 0;
    int nb_decimals = tbl_nb_decimals;
    std::shared_ptr<Table> tbl_ptr = global_ws_tbl->get_obj_ptr(name);
    if(B_viewmode == 0)
    {
        // T_view_tbl() takes char* arguments
        std::string gsmpl = smpl;
        rc = T_view_tbl(tbl_ptr.get(), gsmpl.data(), name.data());
    }
    else
    {
        try
        {
            ComputedTable computed_table(tbl_ptr.get(), smpl, nb_decimals); 
            computed_table.print_to_file(true, true);
            // NOTE: do not call W_close() to give the possibility to append 
            //       new tables in the same open file 
            W_flush();
        }
        catch(const std::exception&)
        {
            rc = -1;
        }
    }

    if(rc < 0) 
        error_manager.append_error("Table '" + name + "' not printed");

    return rc;
}


/**
 *  Calculate and display (or print according to the value of B_viewmode) a graph on a specified GSample, 
 *  based on Table definition(s).
 *  
 *  @param [in] names   const std::string&  list of table names
 *  @param [in] gsmpl   const std::string&  GSample
 *  @return             int                 0 on success, -1 on error (table not found, illegal gsample...)
 */
int B_ViewPrintGr_1(const std::string& names, const std::string& gsmpl)
{
    int rc = 0, hg, ng, i, view = !B_viewmode;
    std::vector<std::string> tbls = split_skip_empty(names, "+-");
    ng = (int) tbls.size();
    if(ng == 0) 
    {
        error_manager.append_error("No tables defined");
        return -1;
    }

    std::string name;
    std::shared_ptr<Table> tbl_ptr;
    for(i = 0; i < ng; i ++) 
    {
        if(view) 
            W_InitDisplay();

        name = global_ws_tbl->to_key(tbls[i]);
        if(!global_ws_tbl->contains(name))
        {
            error_manager.append_error("Table '" + tbls[i] + "' not found");
            rc = -1;
            break;
        }

        tbl_ptr = global_ws_tbl->get_obj_ptr(name);
        hg = T_graph_tbl_1(tbl_ptr, gsmpl, B_viewmode);

        if(view)
        {
            std::string title = tbl_ptr->get_title(false);
            W_EndDisplay((char*) title.c_str(), -ng, -i, -1, -1);
        } 

        if(hg < 0) 
        {
            rc = -1;
            break;
        }
    }

    return rc;
}



/**
 *  Calculate, then print or display (according to the mode parameter) IODE Tables either 
 *  in the form of graphs or in the form of text (SCROLLs).
 *  
 *  @param [in] const std::string&  arg   sample + list of tables to print
 *  @param [in] int                 type  0: print or view in table format
 *                                        1: print in graph format
 *  @param [in] int                 mode  0: view
 *                                        1: print
 *  @return     int                       0 on success-1 on error (no arg, error in execution of tables)
 */
int B_ViewPrintTbl(const std::string& arg, int type, int mode)
{
    int         rc = 0;
    std::string smpl;
    std::string tables;     // arg without the sample

    B_viewmode = mode;

    if(arg.empty() || arg[0] == 0)
    {
        kwarning("No argument passed");
        return -1;
    }
    else
    {
        std::vector<std::string> args = split_skip_empty(arg, " ");
        if(args.empty())
        {
            std::string error_msg = "Invalid argument '" + arg + "'";
            kwarning(error_msg.c_str());
            return -1;
        }

        smpl = args[0];
        // skips the sample and the separator that follows it
        tables = (arg.size() > smpl.size()) ? arg.substr(smpl.size() + 1) : std::string();

        if(mode == 1 || args.size() < 3)
        {
            if(type == 0)
                rc = B_ainit_loop(tables, [&smpl](const std::string& name) { return B_ViewPrintTbl_1(name, smpl); });
            else
                rc = B_ainit_loop(tables, [&smpl](const std::string& names) { return B_ViewPrintGr_1(names, smpl); });
        }
        else
        {
            if(type == 0)
                ODE_VIEW = 1;
            else
                ODE_VIEW = 2;
            SCR_strlcpy((unsigned char*) ODE_SMPL, (unsigned char*) smpl.c_str(), sizeof(ODE_SMPL) - 1); // JMP 10/04/2023
            // ODE_scroll() expects a NULL terminated table of table names
            std::vector<char*> names;
            for(size_t i = 1; i < args.size(); i++)
                names.push_back(args[i].data());
            names.push_back(NULL);
            ODE_scroll(global_ws_tbl.get(), names.data());
        }
    }

    W_flush();
    B_ViewTblEnd();

    if(rc < 0)
    {
        std::string action = (mode == 0) ? "display" : "print";
        std::string names = trim(tables);
        std::string error_msg = "Cannot " + action + " table(s) '" + names + "'";
        error_manager.prepend_error(error_msg);
        if(error_manager.has_errors())
            kwarning((char*) error_manager.get_all_errors().c_str());
    }

    return rc;
}


// Report function:
//      $PrintTblFile n varfilename    (n := 2, 3, 4, 5)
int B_ViewTblFile(const std::string& arg, int unused)
{
    bool   success;
    int    ref, rc = 0;
    std::vector<std::string> args;

    if(arg.empty() || arg[0] == 0)
    {
        error_manager.append_error("Invalid argument");
        return -1;
    }
    else 
    {
        args = split_skip_empty(arg, " ");
        if(args.size() < 2) goto err;

        ref = atoi(args[0].c_str());
        if(ref < 2 || ref > 5) 
        {
            error_manager.append_error("Invalid Reference number");
            rc = -1;
            goto err;
        }

        if(global_ref_var[ref - 1])
            global_ref_var[ref - 1]->clear();
        global_ref_var[ref - 1] = KDBVariables::Create(false);
        success = global_ref_var[ref - 1]->load(args[1]);
        if(!success) 
        {
            global_ref_var[ref - 1]->clear();
            global_ref_var[ref - 1] = nullptr;
            std::string msg = "Error loading the variables file '";
            msg += args[1] + "'";
            kwarning(msg.c_str());
            rc = -1;
            goto err;
        }
    }

err:
    return rc;
}


// Close a Print tables or Print variables session.
int B_ViewTblEnd()
{
    for(int i = 1; i < 5; i++)
    {
        if(global_ref_var[i])
            global_ref_var[i]->clear();
        global_ref_var[i] = nullptr;
    }
    
    ODE_VIEW = 0;
    ODE_SMPL[0] = '\0';
    return 0;
}
