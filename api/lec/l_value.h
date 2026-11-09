#pragma once
#include "api/lec/l_common.h"


// for backward compatibility with IODE 6.68
struct CVAR 
{
    short pos = 0;
	short lag = 0;      // lag of the variable (0 if current value, 1 if t-1...)
	short ref = 0;
	short pad = 0;
    Period per;         // period of the variable (if any)

    short get_binary_length() const
    {
        return sizeof(CVAR);
    }

    void append_to_buffer(unsigned char* buffer, int& pos_buffer) const
    {
        memcpy(buffer + pos_buffer, &pos, sizeof(short));
        pos_buffer += sizeof(short);
        memcpy(buffer + pos_buffer, &lag, sizeof(short));
        pos_buffer += sizeof(short);
        memcpy(buffer + pos_buffer, &ref, sizeof(short));
        pos_buffer += sizeof(short);
        memcpy(buffer + pos_buffer, &pad, sizeof(short));
        pos_buffer += sizeof(short);
        memcpy(buffer + pos_buffer, &per, sizeof(Period));
        pos_buffer += sizeof(Period);
    }
};

class LEC_CONST_REAL: public LEC_ABSTRACT_VALUE
{
    double value;

public:
    LEC_CONST_REAL(const double value) : LEC_ABSTRACT_VALUE(L_DCONST), value(value)
    {
#if DEBUG
        representation = std::to_string(value);
#endif
    }

    bool operator==(const LEC_CONST_REAL& other) const
    {
        return is_same_type(other) && this->value == other.value;
    }

    double get_value() const
    {
        return value;
    }

    short get_binary_length() const override
    {
        // 1 byte for type
        return 1 + sizeof(float);
    }

    void append_to_buffer(unsigned char* buffer, int& pos_buffer) const override
    {
        // float for backward compatibility with IODE 6.68
        float f_value = (float) value;
        LEC_ABSTRACT::append_to_buffer(buffer, pos_buffer);
        memcpy(buffer + pos_buffer, &f_value, sizeof(float));
        pos_buffer += sizeof(float);
    }
};

class LEC_CONST_LONG: public LEC_ABSTRACT_VALUE
{
    double value; 

public:
    LEC_CONST_LONG(const long value) : LEC_ABSTRACT_VALUE(L_LCONST)
    {
        this->value = (double) value;
#if DEBUG
        representation = std::to_string(value);
#endif
    }

    bool operator==(const LEC_CONST_LONG& other) const
    {
        return is_same_type(other) && this->value == other.value;
    }

    double get_value() const
    {
        return value;
    }

    short get_binary_length() const override
    {
        // 1 byte for type
        return 1 + sizeof(long);
    }

    void append_to_buffer(unsigned char* buffer, int& pos_buffer) const override
    {
        // long for backward compatibility with IODE 6.68
        long l_value = (long) value;
        LEC_ABSTRACT::append_to_buffer(buffer, pos_buffer);
        memcpy(buffer + pos_buffer, &l_value, sizeof(long));
        pos_buffer += sizeof(long);
    }
};

class LEC_COEF: public LEC_ABSTRACT_VALUE, public CVAR
{
    std::shared_ptr<Scalar> scl_ptr;

public:
    std::string name;   // name of the coefficient (IODE Scalar)

public:
    LEC_COEF(std::string name) 
        : LEC_ABSTRACT_VALUE(L_COEF), name(std::move(name))
    {
#if DEBUG
        representation = this->name;
#endif
    }

    bool operator==(const LEC_COEF& other) const
    {
        if(!is_same_type(other))
            return false;
        
        if(this->name != other.name)
            return false;

        bool this_null = this->scl_ptr.get() == nullptr;
        bool other_null = other.scl_ptr.get() == nullptr;
        if(this_null && other_null)
            return true;
        if(this_null != other_null)
            return false;
        return this->scl_ptr->value == other.scl_ptr->value;
    }

    void link(std::shared_ptr<Scalar> scl_ptr)
    {
        if(!scl_ptr)
        {
            this->scl_ptr.reset();
            std::string msg = "Missing value for the coefficient '" + name + "'\n";
            msg += "Missing or failed linkage of LEC expression";
            kwarning((char*) msg.c_str());
        }

        this->scl_ptr = scl_ptr;
    }

    double get_value() const
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

    short get_binary_length() const override
    {
        // 1 byte for the type
        return 1 + CVAR::get_binary_length();
    }

    void append_to_buffer(unsigned char* buffer, int& pos_buffer) const override
    {
        LEC_ABSTRACT::append_to_buffer(buffer, pos_buffer);
        CVAR::append_to_buffer(buffer, pos_buffer);
    }
};

class LEC_VAR: public LEC_ABSTRACT_VALUE, public CVAR
{
    std::shared_ptr<Variable> var_ptr;

public:
    std::string name;   // name of the variable (IODE Variable)      

public:
    // type = L_VAR or L_VART (variable with time) 
    LEC_VAR(const int type, std::string name, const short lag, const Period& per)
        : LEC_ABSTRACT_VALUE(type), name(std::move(name))
    {
        if(type != L_VAR && type != L_VART)
            throw std::invalid_argument("Invalid type for LEC_VAR: " + std::to_string(type));
        this->lag = lag; 
        this->per = per;
#if DEBUG
        representation = this->name;
#endif
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
        if(!var_ptr)
        {
            this->var_ptr.reset();
            std::string msg = "Missing value for the Variable '" + name + "'\n";
            msg += "Missing or failed linkage of LEC expression";
            kwarning((char*) msg.c_str());
        }

        this->var_ptr = var_ptr;
    }

    void check_nb_periods(const int nb_periods)
    {
        if(nb_periods != var_ptr->size())
        {
            std::string msg = "Linking CLEC: size of linked sample ";
            msg += "(" + std::to_string(nb_periods) + ") is different from the size of ";
            msg += "the passed variable (" + std::to_string(var_ptr->size()) + ")";
            kwarning((char*) msg.c_str());
            var_ptr->clear();
        }
    }

    void calculate_ref(const Sample& sample)
    {
        ref = lag;
        if(per.step != 0)
            ref += per.difference(sample.start_period);
    }

    double get_value(const int t) const
    {
        if(!var_ptr)
        {
            std::string msg = "Missing value for the Variable '" + name + "'\n";
            msg += "Missing or failed linkage of LEC expression";
            kwarning((char*) msg.c_str());
            return IODE_NAN;
        }

        const Variable& var = *var_ptr;

        int len = (int) ref;
        if(per.year == 0)  
            len += t;
        
        double value = IODE_NAN;
        if(len >= 0 && len < var.size()) 
            value = var[len];

        return value;
    }

    short get_binary_length() const override
    {
        // 1 byte for type
        return 1 + CVAR::get_binary_length();
    }

    void append_to_buffer(unsigned char* buffer, int& pos_buffer) const override
    {
        LEC_ABSTRACT::append_to_buffer(buffer, pos_buffer);
        CVAR::append_to_buffer(buffer, pos_buffer);
    }
};

class LEC_PERIOD: public LEC_ABSTRACT_VALUE
{
    Period period;
    double value = IODE_NAN;
    short pos = 0;

public:
    LEC_PERIOD(const Period& period) : LEC_ABSTRACT_VALUE(L_PERIOD), period(period)
    {
#if DEBUG
        representation = period.to_string();
#endif
    }

    bool operator==(const LEC_PERIOD& other) const
    {
        if(!is_same_type(other))
            return false;
        
        if(this->period != other.period)
            return false;
        
        return this->value == other.value;
    }

    void link(const Sample& sample)
    {
        pos = period.difference(sample.start_period);
        value = (double) pos;
    }

    double get_value() const
    {
        return value;
    }

    short get_binary_length() const override
    {
        // 1 byte for type + sizeof(period) + sizeof(pos)
        return 1 + sizeof(Period) + sizeof(short);
    }

    void append_to_buffer(unsigned char* buffer, int& pos_buffer) const override
    {
        LEC_ABSTRACT::append_to_buffer(buffer, pos_buffer);
        memcpy(buffer + pos_buffer, &period, sizeof(Period));
        pos_buffer += sizeof(Period);
        memcpy(buffer + pos_buffer, &pos, sizeof(short));
        pos_buffer += sizeof(short);
    }
};
