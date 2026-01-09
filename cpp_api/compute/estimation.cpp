#include "estimation.h"


std::string dynamic_adjustment(const IodeAdjustmentMethod method, 
    const std::string& eqs, const std::string& c1, const std::string& c2)
{
    std::string error_msg;
    // TODO (future) : replace content of L_split_eq() by line below
    std::size_t found = eqs.find(":=");
    if(found == std::string::npos) 
        error_msg += "Missing ':=' in the passed LEC expression: " + eqs + "\n";
    if(c1.empty()) 
        error_msg += "Name for the first coefficient is empty!\n";
    if(method == AM_ERROR_CORRECTION_METHOD && c2.empty()) 
        error_msg += "Name for the second coefficient is empty!\n";
    if(!error_msg.empty()) 
    {
        error_msg = "Failed to proceed dynamic adjustment - invalid argument(s):\n" + error_msg;
        throw std::invalid_argument(error_msg);
    }

    size_t eqs_size = eqs.size() + 1;
    char* c_eqs = new char[eqs_size];
    strncpy(c_eqs, eqs.c_str(), eqs_size);
    int res = E_DynamicAdjustment(method, &c_eqs, to_char_array(c1), to_char_array(c2));
    std::string adjusted_eqs(c_eqs);
    delete[] c_eqs;
    if(res < 0)
    {
        std::string error_msg = "Error during dynamic adjustment of equation \"" + eqs + "\"";
        std::string last_error = error_manager.get_last_error();
        if(!last_error.empty())
            error_msg += "\n" + last_error;
        throw std::runtime_error(error_msg);
    }
    return adjusted_eqs;
}

// Note: +/- the same as E_SclToReal()
static void add_df_test_coeff(KDBScalars* kdb, const std::string& coeff_name, double* res, const int pos)
{
    Scalar scl;
    scl.value = res[pos];
    scl.relax = (res[pos+1] < 1.0) ? res[pos+1] : 1.0;
    scl.std   = (!IODE_IS_0(res[pos+2])) ? res[pos] / res[pos+2] : IODE_NAN;
    kdb->add(coeff_name, scl);
}

// QUESTION FOR JMP : Why E_UnitRoot does not return a new KDB of scalars ? 
//                    In E_UnitRoot, you first create new scalars then you delete them after computation of DF test.
//                    Then, why not working on a local KDB of scalars ?
KDBScalars* dickey_fuller_test(const std::string& lec, bool drift, bool trend, int order)
{
    double* res = E_UnitRoot(to_char_array(lec), drift, trend, order);
    if(!res)
    {
        std::string is_drift = drift ? "yes" : "no";
        std::string is_trend = trend ? "yes" : "no";
        std::string error_msg = "Cannot perform Unit Root (Dickey Fuller test)\n";
        error_msg += "LEC expression: " + lec + "\n";
        error_msg += "Drift: " + is_drift + "\n";
        error_msg += "Trend: " + is_trend + "\n";
        error_msg += "Order: " + std::to_string(order);
        std::string last_error = error_manager.get_last_error();
        if(!last_error.empty())
            error_msg += "\n" + last_error;
        throw std::runtime_error(error_msg);
    }

    int pos = 0;
    KDBScalars* kdb_res = Scalars.subset("", true);
    // order 0
    add_df_test_coeff(kdb_res, "df_", res, pos);
    pos += 3;
    // drift
    if(drift)
    {
        add_df_test_coeff(kdb_res, "df_d", res, pos);
        pos += 3;
    }
    // trend
    if(trend)
    {
        add_df_test_coeff(kdb_res, "df_t", res, pos);
        pos += 3;
    }
    // order > 0
    for(int i=1; i <= order; i++)
    {
        add_df_test_coeff(kdb_res, "df" + std::to_string(i), res, pos);
        pos += 3;
    }

    SW_nfree(res);
    return kdb_res;
}

EditAndEstimateEquations::EditAndEstimateEquations(const std::string& from, const std::string& to)
    : estimation_done(false), sample(nullptr), method(EQ_LSQ), current_eq(v_equations.end())
{
    set_sample(from, to);
    kdb_eqs = Equations.subset("", true);
    kdb_scl = Scalars.subset("", true);
    m_corr = nullptr;
}

EditAndEstimateEquations::~EditAndEstimateEquations()
{
    if(estimation_done) delete estimation;
    if(sample)          delete sample;
    if(kdb_eqs)         delete kdb_eqs;
    if(kdb_scl)         delete kdb_scl;
    if(m_corr)          delete m_corr;
}

void EditAndEstimateEquations::set_block(const std::string& block, const std::string& current_eq_name)
{
    try
    {
        // reset attribute 'block', 'v_equations' and 'current_eq'
        this->block = "";
        v_equations.clear();
        current_eq = v_equations.end();

        // generate a list of equations names from the passed argument 'block'
        // If an equation name specified in the block is not found in the global workspace, add it anyway
        std::vector<std::string> v_equations_ = Equations.filter_names(block, false);

        // if current equation name is not empty
        if(!current_eq_name.empty())
        {
            // if current equation name is not in the list of equations -> append it
            if(find(v_equations_.begin(), v_equations_.end(), current_eq_name) == v_equations_.end())
                v_equations_.push_back(current_eq_name);
        }

        // check each name if is valid
        // If there is an invalid name, throw an error
        for(const std::string& name: v_equations_)
            check_name(name, EQUATIONS);

        // for each equation name from the block
        Equation* eq;
        for(const std::string& name: v_equations_)
        {
            // a. check if the equation is already present in the local database 'kdb_eqs'
            if(!kdb_eqs->contains(name))
            {
	            // no -> check if in the global database
                if(Equations.contains(name))
                {
                    // yes -> copy equation from the global database to 'kdb_eqs' 
                    eq = Equations.get(name);
                    kdb_eqs->add(name, *eq);
                    delete eq;
                }
                // no -> add a new equation with LEC '<name> := 0' to 'kdb_eqs'
                else
                    kdb_eqs->add(name, name + " := 0");
            }

            // b. add the equation name to the vector 'v_equations'
            v_equations.push_back(name);
        }

        // copy the list of equations names separated by ';' to the 'block' attribute
        this->block = join(v_equations, ";");

        if(v_equations.size() > 0)
        {
            if(current_eq_name.empty())
                // if the name of the current equation is empty -> move the iterator to the first equation
                current_eq = v_equations.begin();
            else
                // else -> move the equations iterator to the current equation
                current_eq = find(v_equations.begin(), v_equations.end(), current_eq_name);
        }
    }
    catch(std::exception& e)
    {
        throw std::runtime_error("Cannot update the block of equations to estimate.\nblock = " + 
            block + "\n\n" + std::string(e.what()));
    }
}

void EditAndEstimateEquations::update_scalars()
{
    Equation* eq;
    std::vector<std::string> coefficients_list;

    CKDBEquations* c_kdb_eqs = kdb_eqs->get_database();

    // for each equation in the local Equations workspace, get the list if corresponding scalars
    std::vector<std::string> tmp_coefs_list;
    for (auto& [name, handle] : c_kdb_eqs->k_objs)
    {
        eq = c_kdb_eqs->get_obj(name);
        if(!eq)
            throw std::runtime_error("Estimation: Cannot get equation at position " 
                                     + name + " from the local Equations database.");
        tmp_coefs_list = eq->get_coefficients_list();
        std::move(tmp_coefs_list.begin(), tmp_coefs_list.end(), std::back_inserter(coefficients_list));
        delete eq;
    }

    // remove duplicated scalars names
    sort_and_remove_duplicates(coefficients_list);

    // for each scalar name
    for(const std::string& name: coefficients_list)
    {
        // check if it is already present in the local database 'kdb_scl'
        if(!kdb_scl->contains(name))
        {
            // no -> check if in the global Scalars database
            if(Scalars.contains(name))
            {
                // yes -> copy scalars from the global database to 'kdb_scl' 
                Scalar* scl = Scalars.get(name);
                kdb_scl->add(name, *scl);
            }
            // no -> add a new scalar with value = 0.0 and relax = 1.0 to 'kdb_scl'
            else
                kdb_scl->add(name, 0.0, 1.0, IODE_NAN);
        }
    }

    // remove the scalars associated with equations which are not in the present block to estimate
    for(const std::string& eq_name: kdb_eqs->filter_names("*"))
    {
        // if eq_name is not contained in v_equations
        if(find(v_equations.begin(), v_equations.end(), eq_name) == v_equations.end())
        {
            eq = kdb_eqs->get(eq_name);
            if(!eq)
                throw std::runtime_error("Estimation: Could not find equation named '" + eq_name +
                                         " from the local Equations database.");
            for(const std::string& scl_name: eq->get_coefficients_list())
                if(kdb_scl->contains(scl_name))
                    kdb_scl->remove(scl_name);
        }
    }
}

void EditAndEstimateEquations::update_current_equation(const std::string& lec, const std::string& comment)
{
    if(lec.empty()) 
        throw std::invalid_argument("LEC is empty");

    if(current_eq == v_equations.end())
        return;

    std::string name = *current_eq;
    Equation* eq = kdb_eqs->get(name);
    if(!eq)
        throw std::runtime_error("Estimation: Could not find equation named '" + name +
                                  " from the local Equations database.");
    try
    {
        eq->set_lec(lec);
        eq->set_comment(comment);
        kdb_eqs->update(name, *eq);
        delete eq;
    }
    catch(std::exception& e)
    {
        if(eq) delete eq;
        throw std::runtime_error("Estimation: Cannot update equation named '" + name + 
                                 "':\n" + std::string(e.what()));
    }
}

void EditAndEstimateEquations::estimate(int maxit, double eps)
{
    if(maxit <= 0)
        maxit = DEFAULT_MAXIT;
    
    if(eps <= 0.0)
        eps = DEFAULT_EPS;

    // clear C API errors stack
    error_manager.clear();

    // frees all allocated variables for the last estimation
    if(estimation_done)
        delete estimation;
    estimation_done = false;

    // endos
    char** c_endos = vector_to_double_char(v_equations);   
    
    // list of LEC expressions for each equation of the vector v_equations
    // NOTE: equations are stored in the local database kdb_eqs 
    std::string lec;
    std::vector<std::string> v_lecs;
    for(const std::string& eq_name: v_equations)
    {
        lec = kdb_eqs->get_lec(eq_name);
        v_lecs.push_back(lec);
    }
    char** c_lecs = vector_to_double_char(v_lecs);

    // copy the values of class attributes 'block', 'method' and 'instruments' 
    // to each local equation 
    Equation* eq;
    for(const std::string& eq_name: v_equations) 
    {
        eq = kdb_eqs->get(eq_name);
        if(!eq)
            throw std::runtime_error("Estimation: Could not find equation named '" + eq_name +
                                     " from the local Equations database.");
        try
        {
            eq->set_method(method);
            eq->block = block;
            eq->instruments = instruments;
            kdb_eqs->update(eq_name, *eq);
            delete eq;
        }
        catch(std::exception& e)
        {
            if(eq) delete eq;
            throw std::runtime_error("Estimation: Cannot update equation named '" + eq_name + 
                                     "':\n" + std::string(e.what()));
        }
    }

    // NOTE: do NOT free c_endos, c_lecs and c_instrs -> they're will be freed in 
    // the Estimation destructor
    CKDBEquations* dbe = kdb_eqs->get_database();
    CKDBScalars* dbs = kdb_scl->get_database();
    CKDBVariables* dbv = Variables.get_database();
    int i_method = (int) method;
    
    estimation = new Estimation(c_endos, dbe, dbv, dbs, sample, i_method, maxit, eps);
    int res = estimation->estimate();

    if(res == 0)
    {
        estimation_done = true;
        std::vector<std::string> v_coeffs = kdb_scl->filter_names("*");

        if(m_corr) delete m_corr;
        m_corr = new CorrelationMatrix(v_coeffs, estimation->get_MCORR()); 
    }
    else
    {
        if(m_corr) delete m_corr;
        m_corr = nullptr;

        std::string last_error = error_manager.get_last_error();
        if(!last_error.empty())
            throw std::runtime_error("Could not estimate equation(s) " + join(v_equations, ";") + "\n" + last_error);
    }
}

std::vector<std::string> EditAndEstimateEquations::save(const std::string& from, const std::string& to)
{
    std::string est_from = sample->start_period.to_string();
    std::string est_to = sample->end_period.to_string();

    std::string no_est_from;
    std::string no_est_to;
    if(!estimation_done)
    {
        Sample* vars_sample = Variables.get_sample();  
        // throw an error if the string does not represent a valid period
        Period from_per = (!from.empty()) ? Period(from) : vars_sample->start_period;
        // throw an error if the string does not represent a valid period
        Period to_per = (!to.empty()) ? Period(to) : vars_sample->end_period;
        // check that from is before to
        Sample no_est_sample(from_per, to_per);

        no_est_from = from_per.to_string();
        no_est_to = to_per.to_string();
    }

    // copy the Equations referenced in v_equations from the local database to the global one.
    Equation* eq;
    std::string eq_name;
    std::vector<std::string> v_new_eqs;
    for(int i = 0; i < v_equations.size(); i++) 
    {
        eq_name = v_equations[i];
        eq = kdb_eqs->get(eq_name);    // get the equation from the local database
        if(!eq)
            throw std::runtime_error("Estimation: Could not find equation named '" + eq_name +
                                     " from the local Equations database.");
        try
        {
            eq->set_method(method);
            eq->block = block;
            eq->instruments = instruments;
    
            if(estimation_done)
            {
                eq->set_sample(est_from, est_to);
                eq->update_date();
            }
            else
            {
                eq->set_sample(no_est_from, no_est_to);
                eq->tests.fill(0.0f);
                eq->reset_date();
            }
    
            // update/add the equation in the global database
            if(Equations.contains(eq_name))
                Equations.update(eq_name, *eq);
            else
            {
                Equations.add(eq_name, *eq);
                v_new_eqs.push_back(eq_name);
            }
        }
        catch(std::exception& e)
        {
            if(eq) delete eq;
            throw std::runtime_error("Estimation: Cannot copy/update the equation named '" + 
                                     eq_name + "' from the local estimation database to " + 
                                     "the global database:\n" + std::string(e.what()));
        }
    }

    // merge the local Scalars into the global Scalars database
    Scalars.merge(*kdb_scl);

    return v_new_eqs;
}

void eqs_estimate(const std::string& eqs, const std::string& from, const std::string& to, 
                  int maxit, double eps)
{
    // clear C API errors stack
    error_manager.clear();

    Sample* sample = Variables.get_sample();
    if(sample->nb_periods == 0)
        throw std::runtime_error("Could not perform estimation: No estimation sample is defined");
    std::string from_period = (from.empty()) ? sample->start_period.to_string() : from;
    std::string to_period = (to.empty()) ? sample->end_period.to_string() : to;

    Estimation estimation(to_char_array(eqs), global_ws_eqs.get(), global_ws_var.get(), global_ws_scl.get(), 
                          to_char_array(from_period), to_char_array(to_period), -1, maxit, eps);
    int res = estimation.estimate();
    if(res != 0)
    {
        std::string error_msg = "Could not estimate equation(s) '" + eqs + "' from '" + from + "' to '" + to + "'";
        std::string last_error = error_manager.get_last_error();
        if(!last_error.empty())
            throw std::runtime_error(error_msg + ":\n" + last_error);
    }
}

void eqs_estimate(const std::vector<std::string>& v_eqs, const std::string& from, const std::string& to, 
                  int maxit, double eps)
{
    std::string eqs = join(v_eqs, ";");
    eqs_estimate(eqs, from, to, maxit, eps);
}
