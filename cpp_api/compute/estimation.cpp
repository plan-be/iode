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
    std::string scalars_list;
    for (const std::string& coef_name: coefficients_list) scalars_list += coef_name + ";";

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