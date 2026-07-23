#pragma once
#include "api/lec/l_common.h"


struct LEC_CONST_REAL: public LEC_VALUE
{
    double value;

public:
    LEC_CONST_REAL(const double value) : LEC_VALUE(L_DCONST), value(value) 
    {
        representation = std::to_string(value);
    }

    LEC_CONST_REAL(const LEC_CONST_REAL& other) = default;

    // extract from the buffer starting at pos_buffer and update pos_buffer
    LEC_CONST_REAL(const unsigned char* buffer, int& pos_buffer) : LEC_VALUE(L_DCONST)
    {
        // the first byte is the type, we can skip it since we already know it's L_DCONST
        pos_buffer++;
        memcpy(&value, buffer + pos_buffer, sizeof(double));
        pos_buffer += sizeof(double);
        representation = std::to_string(value);
    }

    bool operator==(const LEC_CONST_REAL& other) const
    {
        return is_same_type(other) && this->value == other.value;
    }

    bool add_to_stack(std::deque<double>& stack, const int t) const override
    {
        stack.push_back(value);
        return true;
    }

    void add_to_buffer(unsigned char* buffer, int& pos_buffer) const override
    {
        LEC_ABSTRACT::add_to_buffer(buffer, pos_buffer);
        memcpy(buffer + pos_buffer, &value, sizeof(double));
        pos_buffer += sizeof(double);
    }

    int get_length() const override 
    {
        return 1 + sizeof(double);
    }
};

struct LEC_CONST_LONG: public LEC_VALUE
{ 
    long value;

public:
    LEC_CONST_LONG(const long value) : LEC_VALUE(L_LCONST), value(value) 
    {
        representation = std::to_string(value);
    }

    LEC_CONST_LONG(const LEC_CONST_LONG& other) = default;

    // extract from the buffer starting at pos_buffer and update pos_buffer
    LEC_CONST_LONG(const unsigned char* buffer, int& pos_buffer) : LEC_VALUE(L_LCONST)
    {
        // the first byte is the type, we can skip it since we already know it's L_LCONST
        pos_buffer++;
        memcpy(&value, buffer + pos_buffer, sizeof(long));
        pos_buffer += sizeof(long);
        representation = std::to_string(value);
    }

    bool operator==(const LEC_CONST_LONG& other) const
    {
        return is_same_type(other) && this->value == other.value;
    }

    bool add_to_stack(std::deque<double>& stack, const int t) const override
    {
        stack.push_back(static_cast<double>(value));
        return true;
    }

    void add_to_buffer(unsigned char* buffer, int& pos_buffer) const override
    {
        LEC_ABSTRACT::add_to_buffer(buffer, pos_buffer);
        memcpy(buffer + pos_buffer, &value, sizeof(long));
        pos_buffer += sizeof(long);
    }

    int get_length() const override 
    {
        return 1 + sizeof(long);
    }
};

struct LEC_COEF: public LEC_VALUE
{
    int pos;          // position of the coefficient in V_EXEC_POS

public:
    LEC_COEF(const std::string& name, const int pos) : LEC_VALUE(L_COEF), pos(pos) 
    {
        representation = name;
    }

    LEC_COEF(const LEC_COEF& other) = default;

    // extract from the buffer starting at pos_buffer and update pos_buffer
    LEC_COEF(const unsigned char* buffer, int& pos_buffer) : LEC_VALUE(L_COEF)
    {
        // the first byte is the type, we can skip it since we already know it's L_COEF
        pos_buffer++;
        memcpy(&pos, buffer + pos_buffer, sizeof(int));
        pos_buffer += sizeof(int);
    }

    bool operator==(const LEC_COEF& other) const
    {
        return is_same_type(other) && this->pos == other.pos;
    }

    bool add_to_stack(std::deque<double>& stack, const int t) const override
    {
        double value = L_getscl(L_EXEC_DBS, V_EXEC_POS[pos]);
        if(!IODE_IS_A_NUMBER(value)) 
            return false;
        stack.push_back(value);
        return true;
    }

    void add_to_buffer(unsigned char* buffer, int& pos_buffer) const override
    {
        LEC_ABSTRACT::add_to_buffer(buffer, pos_buffer);
        memcpy(buffer + pos_buffer, &pos, sizeof(int));
        pos_buffer += sizeof(int);
    }

    int get_length() const override 
    {
        return 1 + sizeof(int);
    }
};

struct LEC_VAR: public LEC_VALUE
{
    int pos;            // position of the variable in L_NAMES
    short lag;          // lag of the variable (0 if current value, 1 if t-1...)
    short ref;          
    Period per;         // period of the variable (if any)

public:
    // type = L_VAR or L_VART (variable with time) 
    LEC_VAR(const int type, const std::string& name, const int pos, const short lag, const Period& per)
        : LEC_VALUE(type), pos(pos), lag(lag), ref(0), per(per) 
    {
        if(type != L_VAR && type != L_VART)
            throw std::invalid_argument("Invalid type for LEC_VAR: " + std::to_string(type));
        representation = name;
    }

    LEC_VAR(const LEC_VAR& other) = default;

    // extract from the buffer starting at pos_buffer and update pos_buffer
    LEC_VAR(const unsigned char* buffer, int& pos_buffer) : LEC_VALUE(L_VAR)
    {
        type = (int) buffer[pos_buffer];
        pos_buffer++;
        memcpy(&pos, buffer + pos_buffer, sizeof(int));
        pos_buffer += sizeof(int);
        memcpy(&lag, buffer + pos_buffer, sizeof(short));
        pos_buffer += sizeof(short);
        memcpy(&ref, buffer + pos_buffer, sizeof(short));
        pos_buffer += sizeof(short);
        memcpy(&per, buffer + pos_buffer, sizeof(Period));
        pos_buffer += sizeof(Period);
    }

    bool operator==(const LEC_VAR& other) const
    {
        if(!is_same_type(other))
            return false;
        
        if(this->pos != other.pos)
            return false;
        
        if(this->lag != other.lag)
            return false;

        if(this->ref != other.ref) 
            return false;
        
        return this->per == other.per;
    }

    void calculate_ref(const Sample& sample)
    {
        ref = lag;
        if(per.step != 0)
            ref += per.difference(sample.start_period);
    }

    bool add_to_stack(std::deque<double>& stack, const int t) const override
    {
        double* d_ptr = L_getvar(L_EXEC_DBV, V_EXEC_POS[pos]);
        if(!d_ptr) 
            return false;

        int len = (int) ref;
        if(per.year == 0)  
            len += t;
        
        double value = IODE_NAN;
        if(len >= 0 && len < (L_getsmpl(L_EXEC_DBV))->nb_periods) 
            value = d_ptr[len];
        stack.push_back(value);
        return true;
    }

    void add_to_buffer(unsigned char* buffer, int& pos_buffer) const override
    {
        LEC_ABSTRACT::add_to_buffer(buffer, pos_buffer);
        memcpy(buffer + pos_buffer, &pos, sizeof(int));
        pos_buffer += sizeof(int);
        memcpy(buffer + pos_buffer, &lag, sizeof(short));
        pos_buffer += sizeof(short);
        memcpy(buffer + pos_buffer, &ref, sizeof(short));
        pos_buffer += sizeof(short);
        memcpy(buffer + pos_buffer, &per, sizeof(Period));
        pos_buffer += sizeof(Period);
    }

    int get_length() const override 
    {
        int len = 1;                // for type
        len += sizeof(int);         // for pos
        len += sizeof(short) * 2;   // for lag and ref
        len += sizeof(Period);      // for period
        return len;
    }
};

// NOTE: pos is set in function L_link_sample_expr()
struct LEC_PERIOD: public LEC_VALUE
{
    Period period;
    short pos;

public:
    LEC_PERIOD(const Period& period, const short pos) 
        : LEC_VALUE(L_PERIOD), pos(pos), period(period) 
    {
        representation = period.to_string();
    }

    LEC_PERIOD(const LEC_PERIOD& other) = default;
    
    // extract from the buffer starting at pos_buffer and update pos_buffer
    LEC_PERIOD(const unsigned char* buffer, int& pos_buffer) : LEC_VALUE(L_PERIOD)
    {
        // the first byte is the type, we can skip it since we already know it's L_PERIOD
        pos_buffer++;
        memcpy(&period, buffer + pos_buffer, sizeof(Period));
        pos_buffer += sizeof(Period);
        memcpy(&pos, buffer + pos_buffer, sizeof(short));
        pos_buffer += sizeof(short);
        representation = period.to_string();
    }

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

    bool add_to_stack(std::deque<double>& stack, const int t) const override
    {
        stack.push_back(static_cast<double>(pos));
        return true;
    }

    void add_to_buffer(unsigned char* buffer, int& pos_buffer) const override
    {
        LEC_ABSTRACT::add_to_buffer(buffer, pos_buffer);
        memcpy(buffer + pos_buffer, &period, sizeof(Period));
        pos_buffer += sizeof(Period);
        memcpy(buffer + pos_buffer, &pos, sizeof(short));
        pos_buffer += sizeof(short);
    }

    int get_length() const override 
    {
        return 1 + sizeof(Period) + sizeof(short);
    }
};
