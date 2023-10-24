#pragma once
#include "equation.h"


void Equation::copy_from_EQ_obj(const EQ* obj)
{
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

Equation::Equation(const int pos, KDB* kdb)
{
    if (!kdb) 
        kdb = K_WS[I_EQUATIONS];

    if (pos < 0 || pos > kdb->k_nb)
    {
        IodeExceptionInvalidArguments error("Cannot extract Equation", "Equation position must be in range [0, " + 
            std::to_string(kdb->k_nb - 1) + "])");
        error.add_argument("equation position", std::to_string(pos));
        throw error;
    }

    // Note: KEVAL allocate a new pointer EQ*
    EQ* c_eq = KEVAL(kdb, pos);
    copy_from_EQ_obj(c_eq);
    E_free(c_eq);
    // re-compute CLEC
    char* name = KONAME(kdb, pos);
    this->clec = L_solve(this->lec, name);
}

Equation::Equation(const std::string& name, KDB* kdb)
{
    if (!kdb) 
        kdb = K_WS[I_EQUATIONS];

    int pos = K_find(kdb, to_char_array(name));
    if (pos < 0) 
        throw IodeExceptionFunction("Cannot extract Equation", "Equation with name " + name + " does not exist.");
    
    // Note: KEVAL allocate a new pointer EQ*
    EQ* c_eq = KEVAL(kdb, pos);
    copy_from_EQ_obj(c_eq);
    E_free(c_eq);
    // re-compute CLEC
    this->clec = L_solve(this->lec, to_char_array(name));
}

Equation::Equation(const std::string& name, const std::string& lec, const int method, const std::string& from, const std::string& to, 
    const std::string& comment, const std::string& instruments, const std::string& block, const bool date)
{
    this->lec = NULL, 
    this->clec = NULL, 
    this->cmt = NULL, 
    this->instr = NULL, 
    this->blk = NULL;
    this->date = 0L;
    this->solved = '\0';

    set_lec(lec, name);
    set_method(method);
    set_sample(from, to);
    set_comment(comment);
    set_instruments(instruments);
    set_block(block);
    set_tests({ 0.0f });
    if(date)
        update_date();
}

Equation::Equation(const std::string& name, const std::string& lec, const std::string& method, const std::string& from, const std::string& to, 
    const std::string& comment, const std::string& instruments, const std::string& block, const bool date)
{
    this->lec = NULL, 
    this->clec = NULL, 
    this->cmt = NULL, 
    this->instr = NULL, 
    this->blk = NULL;
    this->date = 0L;
    this->solved = '\0';

    set_lec(lec, name);
    set_method(method);
    set_sample(from, to);
    set_comment(comment);
    set_instruments(instruments);
    set_block(block);
    set_tests({ 0.0f });
    if(date)
        update_date();
}


Equation::Equation(const Equation& other)
{
    copy_from_EQ_obj(static_cast<const EQ*>(&other));
}

Equation::~Equation()
{
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

// -- lec --

std::string Equation::get_lec() const
{ 
    return std::string(this->lec);
}

void Equation::set_lec(const std::string& lec, const std::string& endo)
{
    if(lec.empty())
        throw std::invalid_argument("Passed LEC expression is empty");

    if(endo.empty())
        throw std::invalid_argument("Passed value for endo (equation name) is empty");

    if(this->lec != NULL)
        SW_nfree(this->lec);

    char* c_lec = to_char_array(lec);
    char* c_endo = to_char_array(endo);
    
    // check if LEC expression is valid
    this->clec = L_solve(c_lec, c_endo);
    if(this->clec == NULL) 
        throw std::invalid_argument("Cannot set LEC '" + lec + "' to the equation named '" + endo + "'");
    
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
    return (int) this->method;
}

void Equation::set_method(const int method)
{
    if(method >= I_NB_EQ_METHODS)
        throw std::invalid_argument("Invalid value " + std::to_string(method) + " for the equation method. " + 
            "The passed value must be in the range [0, " + std::to_string(I_NB_EQ_METHODS - 1) + "]");

    this->method = (char) method;
}

std::string Equation::get_method() const
{
    int m = (int) this->method;
    return vEquationMethods[m];
}

void Equation::set_method(const std::string& method)
{
    int m = -1;
    for(int i = 0; i < I_NB_EQ_METHODS; i++) 
        if(method == vEquationMethods[i]) m = i;

    if(m < 0)
        throw std::invalid_argument("The equation method '" + method + "' is not valid.\n" + 
            "Accepted methods are: " + boost::algorithm::join(vEquationMethods, ", "));

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
    SAMPLE* smpl = const_cast<SAMPLE*>(&this->smpl);
    return Sample(smpl);
}

void Equation::set_sample(std::string from, std::string to)
{
    if (from.empty() || to.empty())
    {
        Sample sample(KSMPL(KV_WS));

        if(from.empty()) 
            from = sample.start_period().to_string();

        if(to.empty())   
            to = sample.end_period().to_string();
    }

    Sample new_sample(from, to);
    memcpy(&(this->smpl), new_sample.c_sample, sizeof(SAMPLE));
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

// -- tests --

std::array<float, EQS_NBTESTS> Equation::get_tests() const
{
    std::array<float, EQS_NBTESTS> tests;
    for(int i = 0; i < EQS_NBTESTS; i++) 
        tests[i] = this->tests[i];
    return tests;
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
            // adds a new scalar with values { 0.9, 1.0, L_NAN } to the Scalars Database
            // see K_add() and K_spack()
            if (K_find(K_WS[I_SCALARS], c_name) < 0) 
                K_add(K_WS[I_SCALARS], c_name, NULL);
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
        SAMPLE* sample = KSMPL(K_WS[I_VARIABLES]);
        if(sample == NULL || sample->s_nb == 0)
            throw std::runtime_error("Cannot return the list of variables.\nThe global sample is not yet defined.");

        char* c_name;
        int nb_obs = sample->s_nb;
        for(const std::string& var_name: vars)
        {
            c_name = const_cast<char*>(var_name.data());
            // adds a new variable with nb_obs L_NAN values to the Variables Database
            // see K_add() and K_vpack()
            if (K_find(K_WS[I_VARIABLES], c_name) < 0) 
                K_add(K_WS[I_VARIABLES], c_name, NULL, &nb_obs);
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
    boost::algorithm::trim(lrhs.first);
    // right hand side
    lrhs.second = lec.substr(pos+2);
    boost::algorithm::trim(lrhs.second);

    return lrhs;
}

bool Equation::operator==(const Equation& other) const
{
    if (strcmp(this->lec, other.lec) != 0) return false;
    if (this->method != other.method) return false;
    if (memcmp(&(this->smpl), &(other.smpl), sizeof(SAMPLE)) != 0) return false;
    if (strcmp(this->cmt, other.cmt) != 0) return false;
    if (strcmp(this->blk, other.blk) != 0) return false;
    if (strcmp(this->instr, other.instr) != 0) return false;

    return true;
}

NamedEquation::NamedEquation(const std::string& name) : name(name), eq(Equation(name)) 
{
}

NamedEquation::NamedEquation(const std::string& name, const Equation& eq) : name(name), eq(eq) 
{
}

std::size_t hash_value(EQ const& c_eq)
{
    std::size_t seed = 0;

    // need to wrapp with std::string() because hash_value() and
    // hash_combine() only compare pointer addresses when applied 
    // on char* arrays
    boost::hash_combine(seed, std::string(c_eq.lec));
    boost::hash_combine(seed, c_eq.method);
    boost::hash_combine(seed, c_eq.smpl);
    boost::hash_combine(seed, std::string(c_eq.cmt));
    boost::hash_combine(seed, std::string(c_eq.blk));
    boost::hash_combine(seed, std::string(c_eq.instr));

    return seed;
}

std::size_t hash_value(Equation const& eq)
{
    std::size_t seed = 0;

    // need to wrapp with std::string() because hash_value() and
    // hash_combine() only compare pointer addresses when applied 
    // on char* arrays
    boost::hash_combine(seed, std::string(eq.lec));
    boost::hash_combine(seed, eq.method);
    boost::hash_combine(seed, eq.smpl);
    boost::hash_combine(seed, std::string(eq.cmt));
    boost::hash_combine(seed, std::string(eq.blk));
    boost::hash_combine(seed, std::string(eq.instr));

    return seed;
}
