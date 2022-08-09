#pragma once
#include "../period.h"
#include "../sample.h"
#include "kdb_template.h"


// TODO: wrapp functions from k_wsvar.c in KDBVariables
class KDBVariables : public KDBTemplate<Variable>
{
private:
    Variable new_var_from_lec(const std::string& lec);

protected:

    Variable copy_obj(const Variable& original) const override;

    Variable get_unchecked(const int pos) const override;

public:
    KDBVariables(const std::string& pattern = "") : KDBTemplate(I_VARIABLES, pattern) {};

    IODE_REAL get_var(const int pos, const int t, const int mode = K_LEVEL) const;

    IODE_REAL get_var(const int pos, const std::string& period, const int mode = K_LEVEL) const;

    IODE_REAL get_var(const int pos, const Period& period, const int mode = K_LEVEL) const;

    IODE_REAL get_var(const std::string& name, const int t, const int mode = K_LEVEL) const;

    IODE_REAL get_var(const std::string& name, const std::string& period, const int mode = K_LEVEL) const;

    IODE_REAL get_var(const std::string& name, const Period& period, const int mode = K_LEVEL) const;

    void set_var(const int pos, const int t, const IODE_REAL value, const int mode = K_LEVEL);

    void set_var(const int pos, const std::string& period, const IODE_REAL value, const int mode = K_LEVEL);

    void set_var(const int pos, const Period& period, const IODE_REAL value, const int mode = K_LEVEL);

    void set_var(const std::string& name, const int t, const IODE_REAL value, const int mode = K_LEVEL);

    void set_var(const std::string& name, const std::string& period, const IODE_REAL value, const int mode = K_LEVEL);

    void set_var(const std::string& name, const Period& period, const IODE_REAL value, const int mode = K_LEVEL);

    int add(const std::string& name, const Variable& variable);

    int add(const std::string& name, const std::string& lec);

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
