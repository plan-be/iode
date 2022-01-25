#pragma once

#include "kdb_abstract.h"
#include "../period.h"
#include "../sample.h"


class KDBEquations : public KDBAbstract
{
public:
    KDBEquations() : KDBAbstract(I_EQUATIONS) {};

    // TODO: create a Class Equation (with a destructor)
    // KEVAL returns a struct with undifined elements ???
    Equation* getEquation(const int pos) const { return KEVAL(getKDB(), pos); }

    Equation* getEquation(const std::string name) const
    {
        int pos = getPosition(name);
        return getEquation(pos);
    }

    // Note: argument set to "", NULL or 0 are unchanged
    void setEquation(std::string name, std::string lec, std::string comment, int method, SAMPLE* sample, std::string instruments, std::string block, float* tests, int date) const
    {
        char* c_name = name.empty() ? NULL : const_cast<char*>(name.c_str());
        char* c_lec = lec.empty() ? NULL : const_cast<char*>(lec.c_str());
        char* c_comment = comment.empty() ? NULL : const_cast<char*>(comment.c_str());
        char* c_instruments = instruments.empty() ? NULL : const_cast<char*>(instruments.c_str());
        char* c_block = block.empty() ? NULL : const_cast<char*>(block.c_str());

        int res = B_DataUpdateEqs(c_name, c_lec, c_comment, method, sample, c_instruments, c_block, tests, date);
        if (res < 0) throw std::runtime_error("something went wrong when trying to update equation " + std::string(name));
    }

    // -- lec --

    std::string getLec(const int pos) const { return KELEC(getKDB(), pos); }

    std::string getLec(const std::string name) const 
    {
        int pos = getPosition(name);
        return getLec(pos);
    }

    void setLec(const int pos, std::string lec)
    {
        std::string name = getName(pos);
        setLec(name, lec);
    }

    void setLec(const std::string name, std::string lec)
    {
        setEquation(name, lec, "", 0, NULL, "", "", NULL, 0);
    }

    // -- solved --

    char getSolved(const int pos) const { return KESOLV(getKDB(), pos); }

    char getSolved(const std::string name) const 
    {
        int pos = getPosition(name);
        return getSolved(pos); 
    }

    // -- method --

    std::string getMethod(const int pos) const
    {
        int i = (int) KEMETH(getKDB(), pos);
        if (i > I_NB_EQ_METHODS) i = 0;
        return vEquationMethods[i];
    }    
    
    std::string getMethod(const std::string name) const
    {
        int pos = getPosition(name);
        return getMethod(pos);
    }

    void setMethod(const int pos, std::string method)
    {
        std::string name = getName(pos);
        setMethod(name, method);
    }

    void setMethod(const std::string name, std::string method)
    {
        int i_method = 0;
        for (int i = 0; i < I_NB_EQ_METHODS; i++) if (method == vEquationMethods[i]) i_method = i;
        setEquation(name, "", "", i_method, NULL, "", "", NULL, 0);
    }

    // -- block --

    std::string getBlock(const int pos) const { return KEBLK(getKDB(), pos); }

    std::string getBlock(const std::string name) const 
    {
        int pos = getPosition(name);
        return getBlock(pos); 
    }

    void setBlock(const int pos, std::string block)
    {
        std::string name = getName(pos);
        setBlock(name, block);
    }

    void setBlock(const std::string name, std::string block)
    {
        setEquation(name, "", "", 0, NULL, "", block, NULL, 0);
    }

    // -- sample --

    Sample getSample(const int pos) const
    {
        return Sample(&KESMPL(getKDB(), pos));
    }

    Sample getSample(const std::string name) const
    {
        int pos = getPosition(name);
        return getSample(pos);
    }

    void setSample(const int pos, std::string from = "", std::string to = "")
    {
        std::string name = getName(pos);
        setSample(name, from, to);
    }

    void setSample(const std::string name, std::string from = "", std::string to = "")
    {
        if (from.empty() || to.empty())
        {
            Sample sample = getSample(name);
            if (from.empty()) from = sample.start_period().to_string();
            if (to.empty())   to = sample.end_period().to_string();
        }
        Sample new_sample = Sample(from, to);
        setEquation(name, "", "", 0, new_sample.c_sample, "", "", NULL, 0);
    }

    // -- comment --

    std::string getComment(const int pos) const
    {
        char* c_oem = KECMT(getKDB(), pos);
        std::string comment = std::string(convert_oem_to_utf8(c_oem));
        return comment;
    }

    std::string getComment(const std::string name) const
    {
        int pos = getPosition(name);
        return getComment(pos);
    }

    void setComment(const int pos, std::string comment)
    {
        std::string name = getName(pos);
        setComment(name, comment);
    }

    void setComment(const std::string name, std::string comment)
    {
        char* c_comment = const_cast<char*>(comment.c_str());
        std::string comment_oem = std::string(convert_utf8_to_oem(c_comment));
        setEquation(name, "", comment_oem, 0, NULL, "", "", NULL, 0);
    }

    // -- instruments --

    std::string getInstruments(const int pos) const { return KEINSTR(getKDB(), pos); }

    std::string getInstruments(const std::string name) const 
    {
        int pos = getPosition(name);
        return getInstruments(pos); 
    }

    void setInstruments(const int pos, std::string instruments)
    {
        std::string name = getName(pos);
        setInstruments(name, instruments);
    }

    void setInstruments(const std::string name, std::string instruments)
    {
        setEquation(name, "", "", 0, NULL, instruments, "", NULL, 0);
    }

    // -- date --

    long getDate(const int pos) const { return KEDATE(getKDB(), pos); }

    long getDate(const std::string name) const 
    {
        int pos = getPosition(name);
        return getDate(pos); 
    }

    void updateDate(const int pos)
    {
        std::string name = getName(pos);
        updateDate(name);
    }

    void updateDate(const std::string name)
    {
        // TODO : check if valid date
        setEquation(name, "", "", 0, NULL, "", "", NULL, 1);
    }

    // -- tests --

    std::array<float, EQS_NBTESTS> getTests(const int pos) const
    {
        KDB* kdb = getKDB();
        std::array<float, EQS_NBTESTS> tests;
        float* c_tests = (float*)K_oval(kdb, pos, 9);
        for (int i = 0; i < EQS_NBTESTS; i++) tests[i] = c_tests[i];
        return tests;
    }

    std::array<float, EQS_NBTESTS> getTests(const std::string name) const
    {
        int pos = getPosition(name);
        return getTests(pos);
    }

    void setTests(const int pos, std::array<float, EQS_NBTESTS> tests)
    {
        std::string name = getName(pos);
        setTests(name, tests);
    }

    void setTests(const std::string name, std::array<float, EQS_NBTESTS> tests)
    {
        float c_tests[EQS_NBTESTS];
        int i = 0;
        for (float& value : tests) c_tests[i++] = value;
        setEquation(name, "", "", 0, NULL, "", "", c_tests, 0);
    }

};