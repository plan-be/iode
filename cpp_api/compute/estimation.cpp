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


void Estimation::set_equations_list(const std::string& str_equations)
{
    this->str_equations = str_equations;

    IodeExceptionInvalidArguments invalid_error("Cannot estimate (block of) equation(s) " + str_equations);
    if (str_equations.empty()) 
    {
        invalid_error.add_argument("(list of) equation(s)", "empty");
        throw invalid_error;
    }

    if (kdb_eqs) delete kdb_eqs;
    try
    {
        kdb_eqs = new KDBEquations(KDB_LOCAL, str_equations);
    }
    catch(IodeException)
    {
        invalid_error.add_argument("(list of) equation(s)", str_equations);
        throw invalid_error;
    }

    v_equations = kdb_eqs->get_names();
    current_eq = v_equations.begin();

    // concatenate list of coefficients of each equations of the block
    std::vector<std::string> coefficients_list;
    std::vector<std::string> tmp_coefs_list;
    std::string eq_name;
    for (int i=0; i < kdb_eqs->count(); i++)
    {
        eq_name = kdb_eqs->get_name(i);
        Equation eq = kdb_eqs->get(i);
        tmp_coefs_list = eq.get_coefficients_list(eq_name);
        std::move(tmp_coefs_list.begin(), tmp_coefs_list.end(), std::back_inserter(coefficients_list));
    }
    // remove duplicate coefficient names
    remove_duplicates(coefficients_list);
    // convert vector to string
    std::string scalars_list = boost::algorithm::join(coefficients_list, ";");

    if(kdb_scl) delete kdb_scl;
    try
    {
        kdb_scl = new KDBScalars(KDB_LOCAL, scalars_list);
    }
    catch(IodeException)
    {
        invalid_error.add_argument("list of found coefficients", scalars_list);
        throw invalid_error;
    }
}

Estimation::Estimation(const Sample& sample, const std::string& str_equations) 
    : sample(nullptr), kdb_eqs(nullptr), kdb_scl(nullptr)
{
    set_sample(sample);
    set_equations_list(str_equations);
}

Estimation::Estimation(const Period& from, const Period& to, const std::string& str_equations)
    : sample(nullptr), kdb_eqs(nullptr), kdb_scl(nullptr)
{
    set_sample(from, to);
    set_equations_list(str_equations);
}

Estimation::Estimation(const std::string& from, const std::string& to, const std::string& str_equations)
    : sample(nullptr), kdb_eqs(nullptr), kdb_scl(nullptr)
{
    set_sample(from, to);
    set_equations_list(str_equations);
}

Estimation::~Estimation()
{
    // frees all allocated variables for the last estimation
    E_free_work();
    delete sample;
    delete kdb_eqs;
    delete kdb_scl;
}

/**
 * Equivalent to B_EqsEstimateEqs
 */
void Estimation::equations_estimate()
{
    // frees all allocated variables for the last estimation
    E_free_work();

    char** c_eqs = filter_kdb_names(I_EQUATIONS, str_equations);
    int rc = KE_est_s(kdb_eqs->get_KDB(), KV_WS, kdb_scl->get_KDB(), sample->c_sample, c_eqs, 0);
    if (rc != 0)
    {
        IodeExceptionFunction func_error("Cannot estimate (block of) equation(s) " + str_equations);
        func_error.add_argument("From: ", sample->start_period().to_string());
        func_error.add_argument("To:   ", sample->end_period().to_string());
        func_error.add_argument("(block of) equation(s): ", str_equations);
        throw func_error;
    }
}

void Estimation::save()
{
    KDBScalars scalars_global;
    scalars_global.merge(*kdb_scl);
}