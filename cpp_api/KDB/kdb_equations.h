#pragma once
#include "../period.h"
#include "../sample.h"
#include "../equation.h"
#include "kdb_template.h"


class KDBEquations : public KDBTemplate<Equation>
{
protected:
    Equation copy_obj(const Equation& original) const override;

    Equation get_unchecked(const int pos) const override;

public:
    KDBEquations(const std::string& pattern = "", const bool shallow_copy=true) : 
        KDBTemplate(I_EQUATIONS, pattern, shallow_copy) {};

    std::string get_lec(const int pos) const;

    std::string get_lec(const std::string& name) const;

    int add(const std::string& name, const std::string& lec, const std::string& comment = "", const std::string& method = "",
        Sample* sample = nullptr, const std::string& instruments = "", const std::string& block = "", const std::array<float, EQS_NBTESTS>& tests = {0.}, const bool date = true);

    void update(const std::string& name, const std::string& lec = "", const std::string& comment = "", const std::string& method = "",
        Sample* sample = nullptr, const std::string& instruments = "", const std::string& block = "", const std::array<float, EQS_NBTESTS>* tests = nullptr, const bool date = false);

    void update(const int pos, const std::string& lec = "", const std::string& comment = "", const std::string& method = "",
        Sample* sample = nullptr, const std::string& instruments = "", const std::string& block = "", const std::array<float, EQS_NBTESTS>* tests = nullptr, const bool date = false);

    // -- estimation --

    /** 
     * Equivalent to B_EqsEstimateEqs
     */
    void equations_estimate(const Sample& sample, const std::string& equations_list);

    void equations_estimate(const Period& from, const Period& to, const std::string& equations_list);

    void equations_estimate(const std::string& from, const std::string& to, const std::string& equations_list);
};
