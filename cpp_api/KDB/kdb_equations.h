#pragma once

#include "kdb_abstract.h"
#include "kdb_abstract.cpp"
#include "../period.h"
#include "../sample.h"
#include "../equation.h"


class KDBEquations : public KDBAbstract<Equation>
{
protected:

    // CRUD (Create - Read - Update - Delete) + Copy methods

    int add_or_update(const std::string& name, const Equation& equation) override;

    Equation copy_obj(const Equation& original) const override;

    Equation get_unchecked(const int pos) const override;

public:
    KDBEquations(const std::string& pattern = "") : KDBAbstract(I_EQUATIONS, pattern) {};

    std::string get_lec(const int pos) const;

    std::string get_lec(const std::string& name) const;

    Equation add(const std::string& name, const std::string& lec, const std::string& comment, const std::string& method,
        Sample& sample, const std::string& instruments, const std::string& block, const std::array<float, EQS_NBTESTS>& tests, const bool date);
};
