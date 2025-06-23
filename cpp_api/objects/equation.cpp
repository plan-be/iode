#pragma once
#include "equation.h"


void Equation::copy_from_EQ_obj(const EQ* obj)
{
    this->endo = copy_char_array(obj->endo); 
    this->lec = copy_char_array(obj->lec);
    // NOTE : we do not use memcpy() because memcpy() actually makes  
    //        a shallow copy of a struct instead of a deep copy
    this->clec = clec_deep_copy(obj->clec);
    this->solved = obj->solved;
    this->method = obj->method;
    // NOTE : we can use memcpy() on SAMPLE because SAMPLE does 
    //        not contain attributes which are pointers
    memcpy(&this->smpl, &obj->smpl, sizeof(SAMPLE));
    this->cmt = copy_char_array(obj->cmt);
    this->blk = copy_char_array(obj->blk);
    this->instr = copy_char_array(obj->instr);
    this->date = obj->date;
    memcpy(&(this->tests), &(obj->tests), EQS_NBTESTS * sizeof(float));
}

Equation::Equation(const std::string& name, const std::string& lec, const IodeEquationMethod method, const std::string& from, 
    const std::string& to, const std::string& comment, const std::string& instruments, const std::string& block, const bool date)
{
    this->lec = NULL, 
    this->clec = NULL, 
    this->cmt = NULL, 
    this->instr = NULL, 
    this->blk = NULL;
    this->date = 0L;
    this->solved = '\0';

    this->endo = copy_string_to_char(name);
    set_lec(lec);
    set_method(method);
    set_sample(from, to);
    set_comment(comment);
    set_instruments(instruments);
    set_block(block);
    set_tests({ 0.0f });
    if(date)
        update_date();
}

Equation::Equation(const std::string& name, const std::string& lec, const std::string& method, const std::string& from, 
    const std::string& to, const std::string& comment, const std::string& instruments, const std::string& block, const bool date)
{
    this->lec = NULL, 
    this->clec = NULL, 
    this->cmt = NULL, 
    this->instr = NULL, 
    this->blk = NULL;
    this->date = 0L;
    this->solved = '\0';

    this->endo = copy_string_to_char(name);
    set_lec(lec);
    set_method(method);
    set_sample(from, to);
    set_comment(comment);
    set_instruments(instruments);
    set_block(block);
    set_tests({ 0.0f });
    if(date)
        update_date();
}

Equation::Equation(const EQ* c_other)
{
    copy_from_EQ_obj(c_other);
}

Equation::Equation(const Equation& other)
{
    copy_from_EQ_obj(static_cast<const EQ*>(&other));
}

Equation::~Equation()
{
    SW_nfree(this->endo);
    SW_nfree(this->lec);
    SW_nfree(this->clec);
    SW_nfree(this->cmt);
    SW_nfree(this->blk);
    SW_nfree(this->instr);
}

// required to be used in std::map
Equation& Equation::operator=(const Equation& other)
{
    copy_from_EQ_obj(static_cast<const EQ*>(&other));
    return *this;
}

// -- endo --

std::string Equation::get_endo() const
{
    return std::string(this->endo);
}

// -- lec --

std::string Equation::get_lec() const
{ 
    return std::string(this->lec);
}

void Equation::set_lec(const std::string& lec)
{
    if(lec.empty())
        throw std::invalid_argument("Passed LEC expression is empty");

    char* c_lec = to_char_array(lec);
    
    // check if LEC expression is valid
    this->clec = L_solve(c_lec, this->endo);
    if(this->clec == NULL) 
        throw std::invalid_argument("Cannot set LEC '" + lec + "' to the equation named '" + endo + "'");

    if(this->lec != NULL) 
        SW_nfree(this->lec);
    this->lec = copy_char_array(c_lec);
}

// -- solved --

char Equation::get_solved() const
{
    return this->solved;
}

// -- method --

int Equation::get_method_as_int() const
{
    int m = (int) this->method;
    return (m >= 0 && m < IODE_NB_EQ_METHODS) ? m : 0;
}

void Equation::set_method(const IodeEquationMethod method)
{
    this->method = (char) method;
}

std::string Equation::get_method() const
{
    int m = (int) this->method;

    if(m >= 0 && m < IODE_NB_EQ_METHODS)
        return v_eq_methods[m];
    else
        // invalid value
        return "";
}

void Equation::set_method(const std::string& method)
{
    int m = -1;
    for(int i = 0; i < IODE_NB_EQ_METHODS; i++) 
        if(method == v_eq_methods[i]) m = i;

    if(m < 0)
        throw std::invalid_argument("The equation method '" + method + "' is not valid.\n" + 
            "Accepted methods are: " + join(v_eq_methods, ", "));

    this->method = (char) m;
}

// -- block --

std::string Equation::get_block() const
{
    return std::string(this->blk);
}

void Equation::set_block(const std::string& block)
{
    if(this->blk != NULL)
        SW_nfree(this->blk);

    if (block.empty()) 
        this->blk = copy_char_array("");
    else 
        this->blk = copy_string_to_char(block);
}

// -- sample --

Sample Equation::get_sample() const
{
    return Sample(this->smpl);
}

void Equation::set_sample(std::string from, std::string to)
{
    if (from.empty() || to.empty())
    {
        SAMPLE* c_vars_sample = KSMPL(KV_WS);
        if(c_vars_sample == NULL || c_vars_sample->s_nb == 0)
        {
            this->smpl.s_nb = 0;
            this->smpl.s_p1.p_y = 0;
            this->smpl.s_p1.p_p = '\0';
            this->smpl.s_p1.p_s = 0;
            this->smpl.s_p2.p_y = 0;
            this->smpl.s_p2.p_p = '\0';
            this->smpl.s_p2.p_s = 0;
            kwarning("Variables sample not yet set. Set equation sample to 0.");
            return;
        }

        Sample vars_sample(*c_vars_sample);

        if(from.empty()) 
            from = vars_sample.start_period().to_string();

        if(to.empty())   
            to = vars_sample.end_period().to_string();
    }

    Sample new_sample(from, to);
    memcpy(&(this->smpl), &new_sample, sizeof(SAMPLE));
}

// -- comment --

std::string Equation::get_comment() const
{
    std::string comment_oem = std::string(this->cmt);
    std::string comment = oem_to_utf8(comment_oem);
    return comment;
}

// we assume that comment string is in UTF8 format
void Equation::set_comment(const std::string& comment)
{
    if(this->cmt != NULL)
        SW_nfree(this->cmt);

    if(comment.empty()) 
        this->cmt = copy_char_array("");
    else
    {
        std::string comment_oem = utf8_to_oem(comment);
        this->cmt = copy_string_to_char(comment_oem);
    }
}

// -- instruments --

std::string Equation::get_instruments() const
{
    return std::string(this->instr);
}

void Equation::set_instruments(const std::string& instruments)
{
    if(this->instr != NULL)
        SW_nfree(this->instr);

    if(instruments.empty()) 
        this->instr = copy_char_array("");
    else 
        this->instr = copy_string_to_char(instruments);
}

// -- date --

long Equation::get_date() const
{
    return this->date;
}

std::string Equation::get_date_as_string(const std::string& date_format) const
{
    std::string date;
    // note: dates are stored as follow: yyyymmdd
    //       for example, "18-06-2024" will be stored as long value 20240618 
    long l_date = this->date;
    if(l_date > 0)
    {
        char c_date[12];
        char* c_date_format;
        c_date_format = to_char_array(date_format);
        date = SCR_long_to_fdate(l_date, c_date, c_date_format);
    }

    return date;
}

void Equation::update_date()
{
    this->date = SCR_current_date();
}

void Equation::set_date(const std::string& date, const std::string& date_format)
{
    // note: dates are stored as follow: yyyymmdd
    //       for example, "18-06-2024" will be stored as long value 20240618 
    this->date = SCR_fdate_to_long(to_char_array(date), to_char_array(date_format));
}

// -- tests --

std::array<float, EQS_NBTESTS> Equation::get_tests() const
{
    std::array<float, EQS_NBTESTS> tests;
    for(int i = 0; i < EQS_NBTESTS; i++) 
        tests[i] = this->tests[i];
    return tests;
}

std::map<std::string, float> Equation::get_tests_as_map() const
{
    std::map<std::string, float> m_tests;
    m_tests["corr"] = this->tests[EQ_CORR];
    m_tests["stdev"] = this->tests[EQ_STDEV];
    m_tests["meany"] = this->tests[EQ_MEANY];
    m_tests["ssres"] = this->tests[EQ_SSRES];
    m_tests["stderr"] = this->tests[EQ_STDERR];
    m_tests["stderrp"] = this->tests[EQ_STDERRP];
    m_tests["fstat"] = this->tests[EQ_FSTAT];
    m_tests["r2"] = this->tests[EQ_R2];
    m_tests["r2adj"] = this->tests[EQ_R2ADJ];
    m_tests["dw"] = this->tests[EQ_DW];
    m_tests["loglik"] = this->tests[EQ_LOGLIK];
    return m_tests;
}

float Equation::get_test(const IodeEquationTest t) const
{
    if(t < 0 || t >= EQS_NBTESTS)
        throw std::invalid_argument("get_test: the passed value must be in range [0, " + 
            std::to_string(EQS_NBTESTS-1) + "]");
    return this->tests[t];
}

void Equation::set_tests(const std::array<float, EQS_NBTESTS> tests)
{
    for(int i = 0; i < EQS_NBTESTS; i++) 
        this->tests[i] = tests[i];
}

// -- misc --

std::vector<std::string> Equation::get_coefficients_list(const bool create_if_not_exit)
{
    std::vector<std::string> coeffs = get_scalars_from_clec(this->clec);

    // create scalars not yet present in the Scalars Database
    if(create_if_not_exit)
    {
        char* c_name;
        for(const std::string& coeff_name: coeffs)
        {
            c_name = const_cast<char*>(coeff_name.data());
            // adds a new scalar with values { 0.9, 1.0, IODE_NAN } to the Scalars Database
            // see K_add() and K_spack()
            if (K_find(K_WS[SCALARS], c_name) < 0) 
                K_add(K_WS[SCALARS], c_name, NULL);
        }
    }

    return coeffs;
}

std::vector<std::string> Equation::get_variables_list(const bool create_if_not_exit)
{
    std::vector<std::string> vars = get_variables_from_clec(this->clec);

    // create variables not yet present in the Variables Database
    if(create_if_not_exit)
    {
        SAMPLE* sample = KSMPL(K_WS[VARIABLES]);
        if(sample == NULL || sample->s_nb == 0)
            throw std::runtime_error("Cannot return the list of variables.\nThe global sample is not yet defined.");

        char* c_name;
        int nb_obs = sample->s_nb;
        for(const std::string& var_name: vars)
        {
            c_name = const_cast<char*>(var_name.data());
            // adds a new variable with nb_obs IODE_NAN values to the Variables Database
            // see K_add() and K_vpack()
            if (K_find(K_WS[VARIABLES], c_name) < 0) 
                K_add(K_WS[VARIABLES], c_name, NULL, &nb_obs);
        }
    }

    return vars;
}

std::pair<std::string, std::string> Equation::split_equation()
{
    std::pair<std::string, std::string> lrhs;

    std::string lec = get_lec();
    // same as L_split_eq()
    size_t pos = lec.find(":=");
    // test if := not found -> return
    if(pos == std::string::npos) 
        return lrhs;
    // left hand side
    lrhs.first = lec.substr(0, pos);
    lrhs.first = trim(lrhs.first);
    // right hand side
    lrhs.second = lec.substr(pos+2);
    lrhs.second = trim(lrhs.second);

    return lrhs;
}

bool Equation::operator==(const Equation& other) const
{
    return K_cmp_eqs(static_cast<EQ*>(const_cast<Equation*>(this)), 
                     static_cast<EQ*>(const_cast<Equation*>(&other)), this->endo) == 0;
}

std::size_t hash_value(const Equation& equation)
{
    std::hash<EQ> eq_hash;
    return eq_hash(equation);
}
