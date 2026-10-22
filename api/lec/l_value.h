#pragma once
#include "api/lec/l_common.h"


class LEC_CONST_REAL: public LEC_ABSTRACT_VALUE
{
    double value;

public:
    LEC_CONST_REAL(const double value) : LEC_ABSTRACT_VALUE(L_DCONST), value(value)
    {
        representation = std::to_string(value);
    }

    LEC_CONST_REAL(const LEC_CONST_REAL& other) = default;

    ~LEC_CONST_REAL() override = default;

    std::unique_ptr<LEC_ABSTRACT> clone() const override 
    { 
        return std::make_unique<LEC_CONST_REAL>(*this); 
    }

    bool equals(const LEC_ABSTRACT& other) const override
    {
        const auto* rhs = dynamic_cast<const LEC_CONST_REAL*>(&other);
        return rhs && *this == *rhs;
    }

    bool operator==(const LEC_CONST_REAL& other) const
    {
        return is_same_type(other) && this->value == other.value;
    }

    double get_value(const int t) const override
    {
        return value;
    }
};

class LEC_CONST_LONG: public LEC_ABSTRACT_VALUE
{
    long value; 

public:
    LEC_CONST_LONG(const long value) : LEC_ABSTRACT_VALUE(L_LCONST), value(value)
    {
        representation = std::to_string(value);
    }

    LEC_CONST_LONG(const LEC_CONST_LONG& other) = default;

    ~LEC_CONST_LONG() override = default;

    std::unique_ptr<LEC_ABSTRACT> clone() const override 
    { 
        return std::make_unique<LEC_CONST_LONG>(*this); 
    }

    bool equals(const LEC_ABSTRACT& other) const override
    {
        const auto* rhs = dynamic_cast<const LEC_CONST_LONG*>(&other);
        return rhs && *this == *rhs;
    }

    bool operator==(const LEC_CONST_LONG& other) const
    {
        return is_same_type(other) && this->value == other.value;
    }

    double get_value(const int t) const override
    {
        return (double) value;
    }
};

class LEC_COEF: public LEC_ABSTRACT_VALUE
{
    std::shared_ptr<Scalar> scl_ptr;

public:
    std::string name;   // name of the coefficient (IODE Scalar)

public:
    LEC_COEF(const std::string& name) : LEC_ABSTRACT_VALUE(L_COEF), name(name) 
    {
        representation = name;
    }

    LEC_COEF(const LEC_COEF& other) = default;

    ~LEC_COEF() override = default;

    std::unique_ptr<LEC_ABSTRACT> clone() const override 
    { 
        return std::make_unique<LEC_COEF>(*this); 
    }

    bool equals(const LEC_ABSTRACT& other) const override
    {
        const auto* rhs = dynamic_cast<const LEC_COEF*>(&other);
        return rhs && *this == *rhs;
    }

    bool operator==(const LEC_COEF& other) const
    {
        if(!is_same_type(other))
            return false;
        
        if(this->name != other.name)
            return false;

        return true;
    }

    void link(std::shared_ptr<Scalar>& scl_ptr)
    {
        this->scl_ptr = scl_ptr;
    }

    double get_value(const int t) const override
    {
        if(!scl_ptr)
        {
            std::string msg = "Missing value for the coefficient '" + name + "'\n";
            msg += "Missing or failed linkage of LEC expression";
            kwarning((char*) msg.c_str());
            return IODE_NAN;
        }

        return scl_ptr->value;
    }
};

class LEC_VAR: public LEC_ABSTRACT_VALUE
{
    std::shared_ptr<Variable> var_ptr;

public:
    std::string name;   // name of the variable (IODE Variable)
    short lag;          // lag of the variable (0 if current value, 1 if t-1...)
    short ref;
    int nb_periods;
    Period per;         // period of the variable (if any)

public:
    // type = L_VAR or L_VART (variable with time) 
    LEC_VAR(const int type, const std::string& name, const short lag, const Period& per)
        : LEC_ABSTRACT_VALUE(type), name(name), lag(lag), ref(0), nb_periods(-1), per(per) 
    {
        if(type != L_VAR && type != L_VART)
            throw std::invalid_argument("Invalid type for LEC_VAR: " + std::to_string(type));
        representation = name;
    }

    LEC_VAR(const LEC_VAR& other) = default;

    ~LEC_VAR() override = default;

    std::unique_ptr<LEC_ABSTRACT> clone() const override 
    { 
        return std::make_unique<LEC_VAR>(*this); 
    }

    bool equals(const LEC_ABSTRACT& other) const override
    {
        const auto* rhs = dynamic_cast<const LEC_VAR*>(&other);
        return rhs && *this == *rhs;
    }

    bool operator==(const LEC_VAR& other) const
    {
        if(!is_same_type(other))
            return false;
        
        if(this->name != other.name)
            return false;
        
        if(this->lag != other.lag)
            return false;

        if(this->ref != other.ref) 
            return false;
        
        return this->per == other.per;
    }

    void link(std::shared_ptr<Variable>& var_ptr)
    {
        this->var_ptr = var_ptr;
    }

    void set_nb_periods(const int nb_periods)
    {
        this->nb_periods = nb_periods;
    }

    void calculate_ref(const Sample& sample)
    {
        ref = lag;
        if(per.step != 0)
            ref += per.difference(sample.start_period);
    }

    double get_value(const int t) const override
    {
        if(!var_ptr)
        {
            std::string msg = "Missing value for the Variable '" + name + "'\n";
            msg += "Missing or failed linkage of LEC expression";
            kwarning((char*) msg.c_str());
            return IODE_NAN;
        }

        Variable& var = *var_ptr;
        int len = (int) ref;
        if(per.year == 0)  
            len += t;
        double value = IODE_NAN;
        if(len >= 0 && len < nb_periods) 
            value = var[len];
        return value;
    }
};

class LEC_PERIOD: public LEC_ABSTRACT_VALUE
{
    Period period;
    short pos;

public:
    LEC_PERIOD(const Period& period, const short pos) 
        : LEC_ABSTRACT_VALUE(L_PERIOD), pos(pos), period(period) 
    {
        representation = period.to_string();
    }

    LEC_PERIOD(const LEC_PERIOD& other) = default;

    ~LEC_PERIOD() override = default;

    std::unique_ptr<LEC_ABSTRACT> clone() const override 
    { 
        return std::make_unique<LEC_PERIOD>(*this); 
    }

    bool equals(const LEC_ABSTRACT& other) const override
    {
        const auto* rhs = dynamic_cast<const LEC_PERIOD*>(&other);
        return rhs && *this == *rhs;
    }

    bool operator==(const LEC_PERIOD& other) const
    {
        if(!is_same_type(other))
            return false;
        
        if(this->period != other.period)
            return false;
        
        return this->pos == other.pos;
    }

    void link(const Sample& sample)
    {
        pos = period.difference(sample.start_period);
    }

    double get_value(const int t) const override
    {
        return (double) pos;
    }
};
