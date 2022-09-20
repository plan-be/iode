#pragma once
#include "time/period.h"
#include "time/sample.h"
#include "objects/equation.h"
#include "kdb_template.h"


class KDBEquations : public KDBTemplate<Equation>
{
protected:
    Equation copy_obj(const Equation& original) const override;

    Equation get_unchecked(const int pos) const override;

public:
    KDBEquations(const EnumIodeKDBType kdb_type = KDB_GLOBAL, const std::string& pattern = "") : 
        KDBTemplate(kdb_type, I_EQUATIONS, pattern) {};

    std::string get_lec(const int pos) const;

    std::string get_lec(const std::string& name) const;

    int add(const std::string& name, const std::string& lec, const std::string& comment = "", const std::string& method = "",
        Sample* sample = nullptr, const std::string& instruments = "", const std::string& block = "", const std::array<float, EQS_NBTESTS>& tests = {0.}, const bool date = true);

    void update(const std::string& name, const std::string& lec = "", const std::string& comment = "", const std::string& method = "",
        Sample* sample = nullptr, const std::string& instruments = "", const std::string& block = "", const std::array<float, EQS_NBTESTS>* tests = nullptr, const bool date = false);

    void update(const int pos, const std::string& lec = "", const std::string& comment = "", const std::string& method = "",
        Sample* sample = nullptr, const std::string& instruments = "", const std::string& block = "", const std::array<float, EQS_NBTESTS>* tests = nullptr, const bool date = false);
};
