#pragma once

#include "kdb_abstract.h"
#include "kdb_abstract.cpp"
#include "../period.h"
#include "../sample.h"


// TODO: wrapp functions from k_wsvar.c in KDBVariables
class KDBVariables : public KDBAbstract<Variable>
{
private:
    Variable new_var_from_lec(const std::string& lec);

protected:

    // CRUD (Create - Read - Update - Delete) + Copy methods

    int add_or_update(const std::string& name, const Variable& variable) override;

    Variable copy_obj(const Variable& original) const override;

    Variable get_unchecked(const int pos) const override;

public:
    KDBVariables(const std::string& pattern = "") : KDBAbstract(I_VARIABLES, pattern) {};

    IODE_REAL get_var(const int pos, const int t, const int mode) const;

    IODE_REAL get_var(const std::string& name, const int t, const int mode) const;

    void set_var(const int pos, const int t, const int mode, const IODE_REAL value);

    void set_var(const std::string& name, const int t, const int mode, const IODE_REAL value);

    void add(const std::string& name, const Variable& variable);

    void add(const std::string& name, const std::string& lec);

    void update(const int pos, const Variable& variable);

    void update(const int pos, const std::string& lec);

    void update(const std::string& name, const Variable& variable);

    void update(const std::string& name, const std::string& lec);

    Sample get_sample() const;

    void set_sample(const std::string& from, const std::string& to);

    void set_sample(const Period& from, const Period& to);

    int get_nb_periods() const;

    Period get_period(const int t) const;
};
