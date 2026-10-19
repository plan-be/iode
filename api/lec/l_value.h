#pragma once
#include "api/lec/l_common.h"


struct LEC_CONST_REAL: public TP_LEC_VALUE<>
{
    double value;

public:
    LEC_CONST_REAL(const double value) : TP_LEC_VALUE<>(L_DCONST), value(value) 
    {
        representation = std::to_string(value);
    }

    LEC_CONST_REAL(const LEC_CONST_REAL& other) = default;

    bool operator==(const LEC_CONST_REAL& other) const
    {
        return is_same_type(other) && this->value == other.value;
    }

    bool add_to_stack(std::deque<double>& stack) const override
    {
        stack.push_back(value);
        return true;
    }
};

struct LEC_CONST_LONG: public TP_LEC_VALUE<>
{ 
    long value;

public:
    LEC_CONST_LONG(const long value) : TP_LEC_VALUE<>(L_LCONST), value(value) 
    {
        representation = std::to_string(value);
    }

    LEC_CONST_LONG(const LEC_CONST_LONG& other) = default;

    bool operator==(const LEC_CONST_LONG& other) const
    {
        return is_same_type(other) && this->value == other.value;
    }

    bool add_to_stack(std::deque<double>& stack) const override
    {
        stack.push_back(static_cast<double>(value));
        return true;
    }
};

struct LEC_COEF: public TP_LEC_VALUE<const double>
{
    std::string name;   // name of the coefficient (IODE Scalar)
    int pos_in_clec;    // position in the vector v_clec_coeffs

public:
    LEC_COEF(const std::string& name) : TP_LEC_VALUE<const double>(L_COEF), name(name) 
    {
        representation = name;
        pos_in_clec = -1;
    }

    LEC_COEF(const LEC_COEF& other) = default;

    bool operator==(const LEC_COEF& other) const
    {
        if(!is_same_type(other))
            return false;
        
        if(this->name != other.name)
            return false;

        if(this->pos_in_clec != other.pos_in_clec)
            return false;

        return true;
    }

    void set_position(const int pos_in_clec)
    {
        this->pos_in_clec = pos_in_clec;
    }

    bool add_to_stack(std::deque<double>& stack, const double value) const override
    {
        stack.push_back(value);
        return true;
    }
};

struct LEC_VAR: public TP_LEC_VALUE<Variable&, const int, const int>
{
    std::string name;   // name of the variable (IODE Variable)
    int pos_in_clec;    // position in the vector v_clec_vars
    short lag;          // lag of the variable (0 if current value, 1 if t-1...)
    short ref;          
    Period per;         // period of the variable (if any)

public:
    // type = L_VAR or L_VART (variable with time) 
    LEC_VAR(const int type, const std::string& name, const short lag, const Period& per)
        : TP_LEC_VALUE<Variable&, const int, const int>(type), name(name), lag(lag), ref(0), per(per) 
    {
        if(type != L_VAR && type != L_VART)
            throw std::invalid_argument("Invalid type for LEC_VAR: " + std::to_string(type));
        representation = name;
        pos_in_clec = -1;
    }

    LEC_VAR(const LEC_VAR& other) = default;

    bool operator==(const LEC_VAR& other) const
    {
        if(!is_same_type(other))
            return false;
        
        if(this->name != other.name)
            return false;

        if(this->pos_in_clec != other.pos_in_clec)
            return false;
        
        if(this->lag != other.lag)
            return false;

        if(this->ref != other.ref) 
            return false;
        
        return this->per == other.per;
    }

    void set_position(const int pos_in_clec)
    {
        this->pos_in_clec = pos_in_clec;
    }

    void calculate_ref(const Sample& sample)
    {
        ref = lag;
        if(per.step != 0)
            ref += per.difference(sample.start_period);
    }

    bool add_to_stack(std::deque<double>& stack, Variable& var, 
        const int nb_periods, const int t) const override
    {
        int len = (int) ref;
        if(per.year == 0)  
            len += t;
        
        double value = IODE_NAN;
        if(len >= 0 && len < nb_periods) 
            value = var[len];
        stack.push_back(value);
        return true;
    }
};

// NOTE: pos is set in function link_sample()
struct LEC_PERIOD: public TP_LEC_VALUE<>
{
    Period period;
    short pos;

public:
    LEC_PERIOD(const Period& period, const short pos) 
        : TP_LEC_VALUE<>(L_PERIOD), pos(pos), period(period) 
    {
        representation = period.to_string();
    }

    LEC_PERIOD(const LEC_PERIOD& other) = default;

    bool operator==(const LEC_PERIOD& other) const
    {
        if(!is_same_type(other))
            return false;
        
        if(this->period != other.period)
            return false;
        
        return this->pos == other.pos;
    }

    void calculate_pos(const Sample& sample)
    {
        pos = period.difference(sample.start_period);
    }

    bool add_to_stack(std::deque<double>& stack) const override
    {
        stack.push_back(static_cast<double>(pos));
        return true;
    }
};
