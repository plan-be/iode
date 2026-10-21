/** 
 *  @header4iode
 *  
 *  Functions to invert equations in order to solve the equation system with respect to an 
 *  alternative set of variables.
 *  
 *  Exchanges Endo-Exo
 *  ------------------
 *
 *  It is possible to exchange the status of an exogenous variable with that of an endogenous variable. 
 *  That allows, when the value of an endogenous variable is known in advance, 
 *  for example during the first simulation period, to block this endogenous variable by letting an exogenous variable vary 
 *  so that the known value of the endogenous variable is preserved. 
 *  
 *  The method used is to find a path between the exo and the endo by scanning the equations of the model and to modify 
 *  the "status" of the variables accordingly as explained below.
 *   
 *  If "exo" appears in the equation "endo", the path is trivial and we simply replace the endogeneous of that equation.
 *  Note that this requires the use of a Newton-Raphson method to solve the equation with respect 
 *  to the (new) endo (ex exo) variable during the simulation process. 
 *  
 *  If exo does not appear in the equation, we analyse each other endogenous variable in the equation to see 
 *  if their defining equation contains the variable "exo". 
 *  If so, the status in this equation is exchanged and the path between exo and endo is found in one step. 
 *  If not, we continue the process recursively until we have found a path between "endo" and "exo".
 * 
 *  The example below should clarify the process.
 *   
 *      EQ                              ENDO
 *      ------------------------------------
 *      y1 = f1(y2, x1)                 y1
 *      y2 = f2(y3, x1)                 y2
 *      y3 = f3(y1, y2, x1, x2)         y3
 *   
 *  Suppose the exchange y1-x2. 
 *  
 *    x2 does not appear in f1. The only endogenous var in f1 is y2.
 *    x2 does not appear in f2. The only endogenous var in f2 is y3. 
 *    x2 does appear in f3. 
 *      -> x2 becomes the new endogenous of the f3. But as y3 must keep its endogenous "status", we have to change f2 as well.
 *      -> y3 becomes the new endogenous of the f2. Again, y2 must keep its endogenous "status", thus it becomes endo of f1
 *      -> y2 becomes the new endogenous of the f1. 
 *  
 *  Finally, whe obtain the following reorganisation of the model:
 *  
 *      EQ                              ENDO
 *      ------------------------------------
 *      y1 = f1(y2, x1)                 y2
 *      y2 = f2(y3, x1)                 y3
 *      y3 = f3(y1, y2, x1, x2)         x2
 *  
 *  The model is thus solve with respect to {y2,y3,x2} instead of {y1,y2,y3}. The value of y1 if therefore left unchanged.
 *
 *  
 */
#include "api/pch.h"
#include "api/b_errors.h"
#include "api/lec/lec.h"
#include "api/objs/equations.h"
#include "api/simulation/simulation.h"


/**
 *  Recursive function to search a path between posexo and posendo. 
 *  
 *  See more explanation at the top of this module.
 *  
 *  @param [in] string  endo        name of the endogenous variable in v_eqs_ptr
 *  @param [in] string  exo         name of the exogenous variable in v_eqs_ptr
 *  @param [in] int*    depth       current level of recursivity (starts at 0 and increase each time KE)
 *  
 */
bool CSimulation::find_path(const std::string& endo, const std::string& exo, int& depth)
{
    std::shared_ptr<Equation> eq_ptr = sim_dbe->get_obj_ptr(endo);
    std::shared_ptr<CLEC> clec = eq_ptr->clec;

    // Endo and exo are in same equation 
    // => add entry in map_exchange and map_exchange_rev and return 
    for(const auto& name : clec->v_var_names)
    {
        if(name == exo) 
        {
            map_exchange[endo] = exo;
            map_exchange_rev[exo] = endo;
            return true;
        }
    }

    // Endo and exo *not* in the same equation
    // => try to find a path between endo and exo and change endo / exo at each step
    bool success = false;
    int eq_pos = -1;
    for(const auto& name : clec->v_var_names)
    {   
        if(name == endo)
            continue;

        eq_pos = find_eq_pos(name);

        // current variable is not an endogenous of any equation
        if(eq_pos < 0) 
            continue;

        // if path already examined --> continue
        if(path_examined.contains(eq_pos)) 
            continue;
        
        path_examined.insert(eq_pos);

        depth++;
        success = find_path(name, exo, depth);
        // If not found, try the next variable in clec
        if(!success) 
        {
            depth--;
            continue;
        }

        // Path found
        map_exchange[endo] = name;
        map_exchange_rev[name] = endo;
        
        // decrease the depth by 1
        depth--;
        
        return success;
    }

    return false;
}

/**
 *  Modify the model to solve it with respect to another set of variables, 
 *  the variable 'endo' becoming exogenous and the variable 'exo' becoming endogenous.
 *  
 *  @param [in] string  endo        name of the endogenous variable in sim_dbv
 *  @param [in] string  exo         name of the exogenous variable in sim_dbv
 *  @return     int                 0 if the exchange is possible, 
 *                                  -1 otherwise: 
 *                                      endo or exo not found in equations, 
 *                                      path between endo and exo inexistent 
 *  
 */
bool CSimulation::exo_to_endo(const std::string& endo, const std::string& exo)
{
    if(map_exchange.contains(endo))
    {
        std::string error_msg = "Goal Seeking: an exchange for the endogenous variable ";
        error_msg += "'" + endo + "' already exists";
        error_manager.append_error(error_msg);
        return false;
    }

    if(map_exchange_rev.contains(exo))
    {
        std::string error_msg = "Goal Seeking: an exchange for the exogenous variable ";
        error_msg += "'" + exo + "' already exists";
        error_manager.append_error(error_msg);
        return false;
    }

    // search for a path and add an entry to map_exchange and map_exchange_rev
    int depth = 0;
    path_examined.clear();
    bool success = find_path(endo, exo, depth);
    if(!success) 
    {
        std::string error_msg = "Goal Seeking: no exchange ";
        error_msg += "'" + endo + " <-> " + exo + "' possible";
        error_manager.append_error(error_msg);
        return false;
    }

    return true;
}

bool CSimulation::prepare_exchanges()
{
    // Optional goal seeking = exchange exo and endo roles in equations
    if(v_endo_exo.empty()) 
        return true;

    bool success = false;
    std::string endo, exo;
    path_examined.clear();
    for(const std::string& endo_exo : v_endo_exo) 
    {
        std::vector<std::string> pair_endo_exo = split(endo_exo, '-');
        if(pair_endo_exo.size() != 2) 
        {
            std::string err_msg = "'" + endo_exo + "': syntax error in goal ";
            err_msg += "seeking parameter";
            error_manager.append_error(err_msg);
            clear();
            return false;
        }

        endo = pair_endo_exo[0];
        if(!sim_dbe->contains(endo)) 
        {
            std::string err_msg = "Goal Seeking - '" + endo + "': ";
            err_msg += "no such equation in the Equations workspace";
            error_manager.append_error(err_msg);
            clear();
            return false;
        }

        exo = pair_endo_exo[1];
        if(!sim_dbv->contains(exo)) 
        {
            std::string err_msg = "Goal Seeking - '" + exo + "': ";
            err_msg += "cannot find the variable";
            error_manager.append_error(err_msg);
            clear();
            return false;
        }
        
        success = exo_to_endo(endo, exo);
        if(!success) 
        {
            clear();
            return false;
        }
    }

    return success;
}
