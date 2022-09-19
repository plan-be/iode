#pragma once
#include "equation.h"


EQ* create_equation_deep_copy(EQ* original_equation)
{
    EQ* copy_eq = (EQ*) SW_nalloc(sizeof(EQ));
    copy_eq->lec = copy_char_array(original_equation->lec);
    copy_eq->clec = (CLEC*) SW_nalloc(sizeof(CLEC));
    memcpy(copy_eq->clec, original_equation->clec, sizeof(CLEC));
    copy_eq->solved = original_equation->solved;
    copy_eq->method = original_equation->method;
    memcpy(&copy_eq->smpl, &original_equation->smpl, sizeof(SAMPLE));
    copy_eq->cmt = copy_char_array(original_equation->cmt);
    copy_eq->blk = copy_char_array(original_equation->blk);
    copy_eq->instr = copy_char_array(original_equation->instr);
    copy_eq->date = original_equation->date;
    memcpy(&(copy_eq->tests), &(original_equation->tests), EQS_NBTESTS * sizeof(float));

    return copy_eq;
}

bool equation_equal(EQ* c_eq1, EQ* c_eq2)
{
    if (c_eq1 == NULL || c_eq2 == NULL) return false;

    if (strcmp(c_eq1->lec, c_eq2->lec) != 0) return false;
    if (c_eq1->method != c_eq2->method) return false;
    if (memcmp(&(c_eq1->smpl), &(c_eq2->smpl), sizeof(SAMPLE)) != 0) return false;
    if (strcmp(c_eq1->cmt, c_eq2->cmt) != 0) return false;
    if (strcmp(c_eq1->blk, c_eq2->blk) != 0) return false;
    if (strcmp(c_eq1->instr, c_eq2->instr) != 0) return false;

    return true;
}

EQ* prepare_equation(const std::string& name, const bool add_obj, const std::string& lec, const std::string& comment,
    const std::string& method, Sample* sample, const std::string& instruments, const std::string& block, const std::array<float, EQS_NBTESTS>* tests,
    const bool date)
{
    char* c_name = to_char_array(name);

    EQ* eq;
    if (add_obj) eq = (EQ*) SW_nalloc(sizeof(EQ));
    else
    {
        int pos = K_find(K_WS[I_EQUATIONS], c_name);
        eq = KEVAL(K_WS[K_EQS], pos);
    }

    if (add_obj && lec.empty()) 
        throw IodeExceptionInitialization("Equation with name " + name, "Empty LEC expression");

    if (!lec.empty())
    {
        SW_nfree(eq->lec);
        eq->lec = copy_string_to_char(lec);

        // check LEC expression is valid
        CLEC* clec = L_solve(eq->lec, c_name);
        if (clec == NULL)
        {
            SW_nfree(eq->lec);
            std::string action = add_obj ? "create new" : "update";
            IodeExceptionInvalidArguments error("Cannot " + action + " Equation with name " + name);
            error.add_argument("lec", lec);
            throw error;
        }
        SW_nfree(clec);
    }

    if (!comment.empty())
    {
        SW_nfree(eq->cmt);
        eq->cmt = copy_string_to_char(comment);
    }
    if (!instruments.empty())
    {
        SW_nfree(eq->instr);
        eq->instr = copy_string_to_char(instruments);
    }
    if (!block.empty())
    {
        SW_nfree(eq->blk);
        eq->blk = copy_string_to_char(block);
    }

    if (!method.empty())
    {
        int i_method = 0;
        for (int i = 0; i < I_NB_EQ_METHODS; i++) if (method == vEquationMethods[i]) i_method = i;
        eq->method = i_method;
    }

    if (date) eq->date = SCR_current_date();
    else eq->date = 0L;

    if (tests)
    {
        float* c_tests = const_cast<float*>(tests->data());
        memcpy(&(eq->tests), c_tests, EQS_NBTESTS * sizeof(float));   /* FLOAT 12-04-98 */
    }
    else memset(&(eq->tests), 0, EQS_NBTESTS * sizeof(float)); /* JMP 12-04-98 */

    if (sample) memcpy(&(eq->smpl), sample->c_sample, sizeof(SAMPLE));

    return eq;
}


Equation::Equation()
{
    c_equation = nullptr;
}

Equation::Equation(const int pos, KDB* kdb)
{
    if (!kdb) kdb = K_WS[I_EQUATIONS];
    if (pos < 0 || pos > kdb->k_nb)
    {
        IodeExceptionInvalidArguments error("Cannot extract Equation", "Equation position must be in range [0, " + 
            std::to_string(kdb->k_nb - 1) + "])");
        error.add_argument("equation position", std::to_string(pos));
        throw error;
    }
    // Note: KEVAL allocate a new pointer EQ*
    c_equation = KEVAL(kdb, pos);
}

Equation::Equation(const std::string& name, KDB* kdb)
{
    if (!kdb) kdb = K_WS[I_EQUATIONS];
    int pos = K_find(kdb, to_char_array(name));
    if (pos < 0) throw IodeExceptionFunction("Cannot extract Equation", "Equation with name " + name + " does not exist.");
    // Note: KEVAL allocate a new pointer EQ*
    c_equation = KEVAL(kdb, pos);
}

Equation::Equation(const Equation& eq)
{
    c_equation = create_equation_deep_copy(eq.c_equation);
}

Equation::~Equation()
{
    if (c_equation) E_free(c_equation);
}

// required to be used in std::map
Equation& Equation::operator=(const Equation& eq)
{
    this->c_equation = create_equation_deep_copy(eq.c_equation);
    return *this;
}

// -- lec --

std::string Equation::get_lec() const
{ 
    return std::string(c_equation->lec);
}

void Equation::set_lec(const std::string& lec, const std::string& endo)
{
    if (lec.empty() || endo.empty())
    {
        c_equation->lec = NULL;
    }
    else
    {
        char* c_lec = to_char_array(lec);
        char* c_endo = to_char_array(endo);
        // check if LEC expression is valid
        c_equation->clec = L_solve(c_lec, c_endo);
        if (c_equation->clec == 0) 
        {
            IodeExceptionInvalidArguments error("Cannot set LEC to equation named " + endo);
            error.add_argument("lec", lec);
            throw error;
        }
        c_equation->lec = copy_char_array(c_lec);
    }
}

// -- solved --

char Equation::get_solved() const
{
    return c_equation->solved;
}

// -- method --

int Equation::get_method_as_int() const
{
    return (int) c_equation->method;
}

std::string Equation::get_method() const
{
    int m = get_method_as_int();
    if (m > I_NB_EQ_METHODS) m = 0;
    return vEquationMethods[m];
}

void Equation::set_method(const std::string& method)
{
    int m = 0;
    for (int i = 0; i < I_NB_EQ_METHODS; i++) if (method == vEquationMethods[i]) m = i;
    c_equation->method = (char) m;
}

// -- block --

std::string Equation::get_block() const
{
    return std::string(c_equation->blk);
}

void Equation::set_block(const std::string& block)
{
    SW_nfree(c_equation->blk);
    if (block.empty()) c_equation->blk = NULL;
    else c_equation->blk = copy_string_to_char(block);
}

// -- sample --

Sample Equation::get_sample() const
{
    return Sample(&c_equation->smpl);
}

void Equation::set_sample(std::string from, std::string to)
{
    if (from.empty() || to.empty())
    {
        Sample sample = get_sample();
        if (from.empty()) from = sample.start_period().to_string();
        if (to.empty())   to = sample.end_period().to_string();
    }
    Sample new_sample(from, to);
    memcpy(&(c_equation->smpl), new_sample.c_sample, sizeof(SAMPLE));
}

// -- comment --

std::string Equation::get_comment() const
{
    std::string comment_oem = std::string(c_equation->cmt);
    std::string comment = oem_to_utf8(comment_oem);
    return comment;
}

// we assume that comment string is in UTF8 format
void Equation::set_comment(const std::string& comment)
{
    SW_nfree(c_equation->cmt);
    if (comment.empty()) c_equation->cmt = NULL;
    else
    {
        std::string comment_oem = utf8_to_oem(comment);
        c_equation->cmt = copy_string_to_char(comment_oem);
    }
}

// -- instruments --

std::string Equation::get_instruments() const
{
    return std::string(c_equation->instr);
}

void Equation::set_instruments(const std::string& instruments)
{
    SW_nfree(c_equation->instr);
    if (instruments.empty()) c_equation->instr = NULL;
    else c_equation->instr = copy_string_to_char(instruments);
}

// -- date --

long Equation::get_date() const
{
    return c_equation->date;
}

std::string Equation::get_date_as_string(const std::string& date_format) const
{
    std::string date;
    long l_date = c_equation->date;
    if (l_date > 0)
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
    c_equation->date = SCR_current_date();
}

// -- tests --

std::array<float, EQS_NBTESTS> Equation::get_tests() const
{
    std::array<float, EQS_NBTESTS> tests;
    for (int i = 0; i < EQS_NBTESTS; i++) tests[i] = c_equation->tests[i];
    return tests;
}

void Equation::set_tests(const std::array<float, EQS_NBTESTS> tests)
{
    for (int i = 0; i < EQS_NBTESTS; i++) c_equation->tests[i] = tests[i];
}

// -- misc --

std::vector<std::string> Equation::get_coefficients_list(const std::string& enddo)
{
    std::vector<std::string> coefs_list;

    // make sure CLEC is up to date
    set_lec(get_lec(), enddo);

    CLEC* clec = c_equation->clec;
    for(int j = 0 ; j < clec->nb_names ; j++) 
    {
        char* item_name = clec->lnames[j].name;
        if(L_ISCOEF(item_name)) 
        {
            // add new scalar coef to KS_WS if not exist yet
            int pos = K_find(KS_WS, item_name);
            if (pos < 0) K_add(KS_WS, item_name, NULL);
            coefs_list.push_back(std::string(item_name));
        }
    }
    return coefs_list;
}

bool Equation::operator==(const Equation& other) const
{
    return equation_equal(c_equation, other.c_equation);
}
