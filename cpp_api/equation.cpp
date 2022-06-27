#pragma once
#include "equation.h"


EQ* create_equation_deep_copy(EQ* original_equation)
{
    EQ* copy_eq = (EQ*) SW_nalloc(sizeof(EQ));
    copy_eq->lec = (char*) SCR_stracpy((unsigned char*) original_equation->lec);
    copy_eq->clec = (CLEC*) SW_nalloc(sizeof(CLEC));
    memcpy(copy_eq->clec, original_equation->clec, sizeof(CLEC));
    copy_eq->solved = original_equation->solved;
    copy_eq->method = original_equation->method;
    memcpy(&copy_eq->smpl, &original_equation->smpl, sizeof(SAMPLE));
    copy_eq->cmt = (char*) SCR_stracpy((unsigned char*) original_equation->cmt);
    copy_eq->blk = (char*) SCR_stracpy((unsigned char*) original_equation->blk);
    copy_eq->instr = (char*) SCR_stracpy((unsigned char*) original_equation->instr);
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
    char* c_name = const_cast<char*>(name.c_str());

    EQ* eq;
    if (add_obj) eq = (EQ*) SW_nalloc(sizeof(EQ));
    else
    {
        int pos = K_find(K_WS[I_EQUATIONS], c_name);
        eq = KEVAL(K_WS[K_EQS], pos);
    }

    if (add_obj && lec.empty()) throw std::runtime_error("Cannot create new equation with name " + name + ".Passed lec expression is empty.");

    if (!lec.empty())
    {
        std::string action = add_obj ? "create new" : "update";
        char* c_lec = const_cast<char*>(lec.c_str());
        CLEC* clec = L_solve(c_lec, c_name);
        if (clec == NULL) throw std::runtime_error("Cannot" + action + "equation with name " + name + ". Passed lec expression is not valid:\n" + lec);
        SW_nfree(clec);
        SW_nfree(eq->lec);

        eq->lec = (char*)SCR_stracpy((unsigned char*)c_lec);
    }

    if (!comment.empty())
    {
        unsigned char* c_comment = (unsigned char*) const_cast<char*>(comment.c_str());
        SW_nfree(eq->cmt);
        eq->cmt = (char*)SCR_stracpy(c_comment);
    }
    if (!instruments.empty())
    {
        unsigned char* c_instruments = (unsigned char*) const_cast<char*>(instruments.c_str());
        SW_nfree(eq->instr);
        eq->instr = (char*)SCR_stracpy(c_instruments);
    }
    if (!block.empty())
    {
        unsigned char* c_block = (unsigned char*) const_cast<char*>(block.c_str());
        SW_nfree(eq->blk);
        eq->blk = (char*)SCR_stracpy(c_block);
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


Equation::Equation(const int pos, KDB* kdb)
{
    if (!kdb) kdb = K_WS[I_EQUATIONS];
    if (pos < 0 || pos > kdb->k_nb) throw std::runtime_error("Equation: invalid position " + std::to_string(pos));
    // Note: KEVAL allocate a new pointer EQ*
    c_equation = KEVAL(kdb, pos);
}

Equation::Equation(const std::string& name, KDB* kdb)
{
    if (!kdb) kdb = K_WS[I_EQUATIONS];
    int pos = K_find(kdb, const_cast<char*>(name.c_str()));
    if (pos < 0) throw std::runtime_error("Equation with name " + name + " does not exist.");
    // Note: KEVAL allocate a new pointer EQ*
    c_equation = KEVAL(kdb, pos);
}

Equation::Equation(const Equation& eq)
{
    c_equation = create_equation_deep_copy(eq.c_equation);
}

Equation::~Equation()
{
    SW_nfree(c_equation);
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
        char* c_lec = const_cast<char*>(lec.c_str());
        char* c_endo = const_cast<char*>(endo.c_str());
        // TODO: not sure about this
        c_equation->clec = L_solve(c_lec, c_endo);
        if (c_equation->clec == 0) throw std::runtime_error("Wrong LEC expression: " + lec);
        c_equation->lec = (char*) SCR_stracpy((unsigned char*) c_lec);
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
    if (block.empty())
    {
        c_equation->blk = NULL;
    }
    else
    {
        char* c_block = const_cast<char*>(block.c_str());
        c_equation->blk = (char*) SCR_stracpy((unsigned char*) c_block);
    }
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
    if (comment.empty())
    {
        c_equation->cmt = NULL;
    }
    else
    {
        std::string comment_oem = utf8_to_oem(comment);
        char* c_comment_oem = const_cast<char*>(comment_oem.c_str());
        c_equation->cmt = (char*) SCR_stracpy((unsigned char*) c_comment_oem);
    }
}

// -- instruments --

std::string Equation::get_instruments() const
{
    return std::string(c_equation->instr);
}

void Equation::set_instruments(const std::string& instruments)
{
    if (instruments.empty())
    {
        c_equation->instr = NULL;
    }
    else
    {
        char* c_instruments = const_cast<char*>(instruments.c_str());
        c_equation->instr = (char*) SCR_stracpy((unsigned char*) c_instruments);
    }
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
        c_date_format = const_cast<char*>(date_format.c_str());
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

bool Equation::operator==(const Equation& other) const
{
    return equation_equal(c_equation, other.c_equation);
}
