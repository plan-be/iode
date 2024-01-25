#include "estimation.h"


std::string dynamic_adjustment(const EnumIodeAdjustmentMethod method, 
    const std::string& eqs, const std::string& c1, const std::string& c2)
{
    IodeExceptionInvalidArguments invalid("Failed to proceed dynamic adjustment");
    // TODO (future) : replace content of L_split_eq() by line below
    std::size_t found = eqs.find(":=");
    if(found == std::string::npos) invalid.add_argument("No := found in lec expression", eqs);
    if(c1.empty()) invalid.add_argument("c1", "empty!");
    if(method == AM_ERROR_CORRECTION_METHOD && c2.empty()) invalid.add_argument("c2", "empty!");
    if(invalid.invalid_args()) throw invalid;

    size_t eqs_size = eqs.size() + 1;
    char* c_eqs = new char[eqs_size];
    strncpy(c_eqs, eqs.c_str(), eqs_size);
    int res = E_DynamicAdjustment(method, &c_eqs, to_char_array(c1), to_char_array(c2));
    std::string adjusted_eqs(c_eqs);
    delete[] c_eqs;
    if(res < 0) throw IodeExceptionFunction("Failed to proceed dynamic adjustment of equation \"" + eqs + "\"", "Unknown");
    return adjusted_eqs;
}

// Note: Same as E_SclToReal()
static void add_df_test_coeff(KDBScalars* kdb, const std::string& coeff_name, IODE_REAL* res, const int pos)
{
    kdb->add(coeff_name, res[pos], res[pos+1], (!L_IS0(res[pos+2])) ? res[pos]/res[pos+2] : L_NAN);
}

// QUESTION FOR JMP : Why E_UnitRoot does not return a new KDB of scalars ? 
//                    In E_UnitRoot, you first create new scalars then you delete them after computation of DF test.
//                    Then, why not working on a local KDB of scalars ?
KDBScalars* dickey_fuller_test(const std::string& lec, bool drift, bool trend, int order)
{
    IODE_REAL* res = E_UnitRoot(to_char_array(lec), drift, trend, order);
    if(!res)
    {
        IodeExceptionFunction error("Cannot perform Unit Root (Dickey Fuller test)", "Unknown");
        error.add_argument("LEC expression", lec);
        error.add_argument("Drift:", drift ? "yes" : "no");
        error.add_argument("Trend", trend ? "yes" : "no");
        error.add_argument("Order", std::to_string(order));
        throw error;
    }

    int pos = 0;
    KDBScalars* kdb_res = new KDBScalars();
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
    : estimation_done(false), sample(nullptr), method(0), current_eq(v_equations.end())
{
    set_sample(from, to);
    kdb_eqs = Equations.subset("", true);
    kdb_scl = Scalars.subset("", true);
}

EditAndEstimateEquations::~EditAndEstimateEquations()
{
    // frees all allocated variables for the last estimation
    if(estimation_done) E_free_work();

    if(sample)  delete sample;
    if(kdb_eqs) delete kdb_eqs;
    if(kdb_scl) delete kdb_scl;
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
        std::vector<std::string> v_equations_ = Equations.get_names(block, false);

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
            check_name(name, I_EQUATIONS);

        // for each equation name from the block
        for(const std::string& name: v_equations_)
        {
            // a. check if the equation is already present in the local database 'kdb_eqs'
            if(!kdb_eqs->contains(name))
            {
	            // no -> check if in the global database
                if(Equations.contains(name))
                {
                    // yes -> copy equation from the global database to 'kdb_eqs' 
                    Equation eq = Equations.get(name);
                    kdb_eqs->add(name, eq);
                }
                else
                    // no -> add a new equation with LEC '<name> := 0' to 'kdb_eqs'
                    kdb_eqs->add(name, name + " := 0");
            }

            // b. add the equation name to the vector 'v_equations'
            v_equations.push_back(name);
        }

        // copy the list of equations names separated by ';' to the 'block' attribute
        this->block = boost::algorithm::join(v_equations, ";");

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
    std::vector<std::string> coefficients_list;

    // for each equation in the local Equations workspace, get the list if corresponding scalars
    std::vector<std::string> tmp_coefs_list;
    for (int i=0; i < kdb_eqs->count(); i++)
    {
        Equation eq = kdb_eqs->get(i);
        tmp_coefs_list = eq.get_coefficients_list();
        std::move(tmp_coefs_list.begin(), tmp_coefs_list.end(), std::back_inserter(coefficients_list));
    }

    // remove duplicated scalars names
    remove_duplicates(coefficients_list);

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
                Scalar scl = Scalars.get(name);
                kdb_scl->add(name, scl);
            }
            else
                // no -> add a new scalar with value = 0.0 and relax = 1.0 to 'kdb_scl'
                kdb_scl->add(name, 0.9, 1.0);
        }
    }

    // remove the scalars associated with equations which are not in the present block to estimate
    for(const std::string& eq_name: kdb_eqs->get_names())
    {
        // if eq_name is not contained in v_equations
        if(find(v_equations.begin(), v_equations.end(), eq_name) == v_equations.end())
        {
            Equation eq = kdb_eqs->get(eq_name);
            for(const std::string& scl_name: eq.get_coefficients_list())
                if(kdb_scl->contains(scl_name))
                    kdb_scl->remove(scl_name);
        }
    }
}

void EditAndEstimateEquations::copy_eq_tests_values()
{
    std::string eq_name;
    std::array<float, EQS_NBTESTS> tests;

    for(int i = 0; i < v_equations.size(); i++)
    {
        tests.fill(0.0);
        tests[0] = (float) MATE(E_MCORRU,      0, i);
        tests[1] = (float) MATE(E_STDEV,       0, i);
        tests[2] = (float) MATE(E_MEAN_Y,      0, i);
        tests[3] = (float) MATE(E_SSRES,       0, i);
        tests[4] = (float) MATE(E_STDERR,      0, i);
        tests[5] = (float) MATE(E_STD_PCT,     0, i);
        tests[6] = (float) MATE(E_FSTAT,       0, i);
        tests[7] = (float) MATE(E_RSQUARE,     0, i);
        tests[8] = (float) MATE(E_RSQUARE_ADJ, 0, i);
        tests[9] = (float) MATE(E_DW,          0, i);
        tests[10]= (float) MATE(E_LOGLIK,      0, i);

        eq_name = v_equations.at(i);
        Equation eq = kdb_eqs->get(eq_name);
        eq.set_tests(tests);
        kdb_eqs->update(eq_name, eq);
    }
}

void EditAndEstimateEquations::update_current_equation(const std::string& lec, const std::string& comment)
{
    if(lec.empty()) 
        throw std::invalid_argument("LEC is empty");

    if(current_eq == v_equations.end())
        return;

    std::string name = *current_eq;
    Equation eq = kdb_eqs->get(name);
    eq.set_lec(lec, name);
    eq.set_comment(comment);
    kdb_eqs->update(name, eq);
}

void EditAndEstimateEquations::estimate()
{
    // frees all allocated variables for the last estimation
    if(estimation_done)
        E_free_work();
    estimation_done = false;

    // endos
    char** c_endos = vector_to_double_char(v_equations);

    // instruments    
    std::vector<std::string> v_instrs;
    boost::split(v_instrs, instruments, boost::is_any_of(";"));
    char** c_instrs = vector_to_double_char(v_instrs);    
    
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

    // NOTE: do NOT free c_endos, c_lecs and c_instrs -> they're will be freed in E_free_work()
    int res = E_est(c_endos, c_lecs, Variables.get_database(), kdb_scl->get_database(), sample, 
                    this->method, c_instrs, ESTIMATION_MAXIT, ESTIMATION_EPS);

    if(res == 0)        
        estimation_done = true;
    else
        B_display_last_error();
}

void EditAndEstimateEquations::save(const std::string& from, const std::string& to)
{
    std::string est_from = sample->start_period().to_string();
    std::string est_to = sample->end_period().to_string();

    std::string no_est_from;
    std::string no_est_to;
    try
    {
        Sample no_est_sample(from, to);
        no_est_from = from;
        no_est_to = to;
    }
    catch(const std::exception& e)
    {
        no_est_from = est_from;
        no_est_to = est_to;
    }

    // copy the Equations referenced in v_equations from the local database to the global one.
    std::string eq_name;
    for(int i = 0; i < v_equations.size(); i++) 
    {
        eq_name = v_equations[i];
        Equation eq = kdb_eqs->get(eq_name);
        eq.set_block(block);
        eq.set_method(method);
        eq.set_instruments(instruments);

        if(estimation_done)
        {
            eq.set_sample(est_from, est_to);
            eq.update_date();

            // create the Scalars containing the results of an estimated equation
            E_tests2scl(&eq, i, E_T, E_NCE);
        }
        else
        {
            eq.set_sample(no_est_from, no_est_to);
            eq.set_tests({0.0});
            eq.reset_date();
        }

        if(Equations.contains(eq_name))
            Equations.update(eq_name, eq);
        else
            Equations.add(eq_name, eq);
    }

    // merge the local Scalars into the global Scalars database
    Scalars.merge(*kdb_scl);
}

void eqs_estimate(const std::string& eqs, const std::string& from, const std::string& to)
{
    std::string error_msg = "Could not estimate equations " + eqs + "\n";

    Sample sample = Variables.get_sample();
    if(sample.nb_periods() == 0)
        throw std::runtime_error(error_msg + "No sample is defined");
    std::string from_ = (from.empty()) ? sample.start_period().to_string() : from;
    std::string to_ = (to.empty()) ? sample.end_period().to_string() : to;

    int res = KE_estim(to_char_array(eqs), to_char_array(from_), to_char_array(to_));
    if(res != 0)
        throw std::runtime_error(error_msg + "from: " + from + " to " + to);
}

void eqs_estimate(const std::vector<std::string>& eqs, const std::string& from, const std::string& to)
{
    std::string s_eqs = boost::algorithm::join(eqs, ",");
    eqs_estimate(s_eqs, from, to);
}
