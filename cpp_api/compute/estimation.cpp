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


EstimationResults::EstimationResults(const std::string& eqs_list, const std::string& from, const std::string& to) 
    : sample(nullptr), kdb_eqs(nullptr), kdb_scl(nullptr)
{
    std::string error_msg = "Cannot estimate (block of) equation(s): " + eqs_list + "\n";

    if (eqs_list.empty()) 
        throw std::invalid_argument(error_msg + "list of equations empty");

    this->eqs_list = eqs_list;

    try
    {
        sample = new Sample(from, to);
    }
    catch(const std::exception& e)
    {
        throw std::invalid_argument(error_msg + std::string(e.what()));
    }

    try
    {
        kdb_eqs = new KDBEquations(KDB_LOCAL, eqs_list);
    }
    catch(const std::exception& e)
    {
        throw std::invalid_argument(error_msg + std::string(e.what()));
    }

    // concatenate list of coefficients of each equations of the block
    std::vector<std::string> coefficients_list;
    std::vector<std::string> tmp_coefs_list;
    std::string eq_name;
    for (int i=0; i < kdb_eqs->count(); i++)
    {
        eq_name = kdb_eqs->get_name(i);
        Equation eq = kdb_eqs->get(i);
        tmp_coefs_list = eq.get_coefficients_list();
        std::move(tmp_coefs_list.begin(), tmp_coefs_list.end(), std::back_inserter(coefficients_list));
    }
    // remove duplicate coefficient names
    remove_duplicates(coefficients_list);
    // convert vector to string
    std::string scalars_list = boost::algorithm::join(coefficients_list, ";");

    try
    {
        kdb_scl = new KDBScalars(KDB_LOCAL, scalars_list);
    }
    catch(const std::exception& e)
    {
        throw std::invalid_argument(error_msg + "list of estimated scalars: " + 
                                    scalars_list + "\n" + std::string(e.what()));
    }
}

EstimationResults::~EstimationResults()
{
    // frees all allocated variables for the last estimation
    E_free_work();

    if(sample) 
        delete sample;
    if(kdb_eqs)
        delete kdb_eqs;
    if(kdb_scl)
        delete kdb_scl;
}

void EstimationResults::save()
{
    if(kdb_scl)
        Scalars.merge(*kdb_scl);
}

EstimationResults* equations_estimate(const std::string& eqs_list, const std::string& from, const std::string& to)
{
    // frees all allocated variables for the last estimation
    E_free_work();

    Sample vars_sample = Variables.get_sample();
    std::string from_ = from.empty() ? vars_sample.start_period().to_string() : from;
    std::string to_ = to.empty() ? vars_sample.end_period().to_string() : to;

    EstimationResults* est_results = new EstimationResults(eqs_list, from, to);

    char** c_eqs = vector_to_double_char(est_results->get_list_equations());
    KDB* kdb_eqs = est_results->get_equations().get_KDB();
    KDB* kdb_scl = est_results->get_coefficients().get_KDB();
    SAMPLE* smpl = est_results->get_sample().c_sample;

    int rc = KE_est_s(kdb_eqs, KV_WS, kdb_scl, smpl, c_eqs, 0);
    
    SCR_free_tbl((unsigned char**) c_eqs);
    if (rc != 0)
    {
        std::string error_msg = "Cannot estimate (block of) equation(s) " + eqs_list + "\n";
        error_msg += "Equations: ", eqs_list + "\n";
        error_msg += "From:      ", from + "\n";
        error_msg += "To:        ", to + "\n";
        throw std::runtime_error(error_msg);
    }

    return est_results;
}

EstimationResults* equations_estimate(const std::vector<std::string>& eqs_vector, const std::string& from, const std::string& to)
{
    std::string eqs_list = boost::algorithm::join(eqs_vector, ",");
    return equations_estimate(eqs_list, from, to);
}
