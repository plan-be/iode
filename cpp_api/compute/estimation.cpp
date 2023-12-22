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
    KDBScalars* kdb_res = new KDBScalars(KDB_LOCAL);
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

Estimation::Estimation(const std::string& from, const std::string& to)
    : estimation_done(false), sample(nullptr), kdb_eqs(KDBEquations(KDB_LOCAL, "")), kdb_scl(KDBScalars(KDB_LOCAL, ""))
{
    set_sample(from, to);
}

Estimation::~Estimation()
{
    // frees all allocated variables for the last estimation
    if(estimation_done)
        E_free_work();

    if(sample) 
        delete sample;
}

void Estimation::set_block(const std::string& block)
{
    try
    {
        v_equations.clear();
        kdb_eqs.clear();
        kdb_scl.clear();

        // set kdb_eqs and v_equations
        std::vector<std::string> v_equations_ = Equations.get_names(block);
        for(const std::string& name: v_equations_)
        {
            Equation eq = Equations.get(name);
            kdb_eqs.add(name, eq);
            v_equations.push_back(name);
        }

        // reset iterator to the first equation of the list
        current_eq = v_equations.begin();

        // concatenate list of coefficients of each equations of the block
        std::vector<std::string> coefficients_list;
        std::vector<std::string> tmp_coefs_list;
        for (int i=0; i < kdb_eqs.count(); i++)
        {
            Equation eq = kdb_eqs.get(i);
            tmp_coefs_list = eq.get_coefficients_list();
            std::move(tmp_coefs_list.begin(), tmp_coefs_list.end(), std::back_inserter(coefficients_list));
        }
        // remove duplicate coefficient names
        remove_duplicates(coefficients_list);

        // set kdb_scl
        for(const std::string& name: coefficients_list)
        {
            Scalar scl = Scalars.get(name);
            kdb_scl.add(name, scl);
        }

        // set block
        this->block = block;
    }
    catch(std::exception& e)
    {
        throw std::runtime_error("Cannot update the block of equations to estimate.\nblock = " + 
            block + "\n\n" + std::string(e.what()));
    }
}

/**
 * Equivalent to B_EqsEstimateEqs
 */
void Estimation::equations_estimate(const std::string& block)
{
    // frees all allocated variables for the last estimation
    if(estimation_done)
        E_free_work();

    if(!block.empty())
        set_block(block);

    std::vector<std::string> v_eqs = Equations.get_names(this->block);
    char** c_eqs = vector_to_double_char(v_eqs);
    int rc = KE_est_s(kdb_eqs.get_KDB(), KV_WS, kdb_scl.get_KDB(), sample->c_sample, c_eqs, 0);
    SCR_free_tbl((unsigned char**) c_eqs);
    if(rc == 0)
        estimation_done = true;
    else
    {
        estimation_done = false;
        IodeExceptionFunction func_error("Cannot estimate (block of) equation(s) " + this->block);
        func_error.add_argument("From: ", sample->start_period().to_string());
        func_error.add_argument("To:   ", sample->end_period().to_string());
        func_error.add_argument("(block of) equation(s): ", block);
        throw func_error;
    }

}

void Estimation::save()
{
    if(estimation_done)
        Scalars.merge(kdb_scl);
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
