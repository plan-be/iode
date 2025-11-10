#pragma once
#include "api/all.h"
#include "cpp_api/iode_cpp_api.h"
#include <string>
#include <vector>
#include <stdexcept>


static void _c_sanity_checks(KDBVariables* dest, const int dest_t_first, const int dest_t_last, 
                             KDBVariables* source, const int source_t_first, const int source_t_last)
{
    if(source == nullptr)
        throw std::invalid_argument("C API: Variables Source database is empty");

    if(source->get_database() == NULL)
       throw std::invalid_argument("C API: Source Variables database is empty");

    if(source->size() == 0)
       throw std::invalid_argument("C API: Source Variables database is empty");

    if(dest == nullptr)
        throw std::invalid_argument("C API: Destination Variables database is empty");

    if(dest->get_database() == NULL)
      throw std::invalid_argument("C API: Destination Variables database is empty");

    if(dest->size() == 0)
      throw std::invalid_argument("C API: Destination Variables database is empty");

    if(dest_t_first < 0 || dest_t_last < 0 || source_t_first < 0 || source_t_last < 0)
        throw std::invalid_argument("C API: Periods must be positive");

    if(dest_t_first > dest_t_last)
        throw std::invalid_argument("C API: Destination first period " + std::to_string(dest_t_first) + 
                                    " is greater than last period " + std::to_string(dest_t_last));

    if(source_t_first > source_t_last)
        throw std::invalid_argument("C API: Source first period " + std::to_string(source_t_first) + 
                                    " is greater than last period " + std::to_string(source_t_last));

    int nb_periods = source->get_nb_periods();
    if(source_t_last >= nb_periods)
       throw std::invalid_argument("C API: Source last period " + std::to_string(source_t_last) + " must be lower than " + 
                                   "the number of periods (= " + std::to_string(nb_periods) + ") in the source database");

    nb_periods = dest->get_nb_periods();
    if(dest_t_last >= nb_periods)
       throw std::invalid_argument("C API: Destination last period " + std::to_string(dest_t_last) + " must be lower than " + 
                                   "the number of periods (= " + std::to_string(nb_periods) + ") in the destination database");
    
    int dest_nb_periods = dest_t_last - dest_t_first + 1;
    int source_nb_periods = source_t_last - source_t_first + 1;
    // check that the number of values to copy matches between the periods ranges of the two databases
    if(dest_nb_periods != source_nb_periods)
        throw std::invalid_argument("C API: The periods ranges between the source (" + std::to_string(source_nb_periods) + " periods) " + 
                                    "and destination (" + std::to_string(dest_nb_periods) + " periods) databases do not match");
}

void _c_add_var_from_other(const std::string& dest_name, KDBVariables* dest, KDBVariables* source, 
                           const int source_t_first, const int source_t_last)
{
    if(dest_name.empty())
        throw std::invalid_argument("C API: Name of the new variable is empty");

    // sanity checks
    int nb_periods = dest->get_nb_periods();
    _c_sanity_checks(dest, 0, nb_periods - 1, source, source_t_first, source_t_last);

    // check that the source variable exists in the source database
    // NOTE: this can happen if the source or the destination is shallow copy of the global database 
    //       and the variable has been removed from the global database
    // TODO: find a way to delete also the variable from the shallow copies
    std::string source_var_name = source->get_name(0);
    double* source_var_ptr = source->get_var_ptr(source_var_name);
    if(source_var_ptr == NULL)
        throw std::invalid_argument("C API: Variable named '" + source_var_name + "' seems to not exist in the source database");

    // add the variable to the destination database
    KDB* kdb = dest->get_database();
    source_var_ptr += source_t_first;
    K_add(kdb, const_cast<char*>(dest_name.c_str()), source_var_ptr, &nb_periods);
}

void _c_copy_var_content(const std::string& dest_name, KDBVariables* dest, const int dest_t_first, const int dest_t_last, 
                         const std::string& source_name, KDBVariables* source, const int source_t_first, const int source_t_last)
{
    if(dest_name.empty())
       throw std::invalid_argument("C API: Destination variable name is empty");

    if(source_name.empty())
       throw std::invalid_argument("C API: Source variable name is empty");

    // sanity checks
    _c_sanity_checks(dest, dest_t_first, dest_t_last, source, source_t_first, source_t_last);

    // check that the destination variable exists in the destination database
    // NOTE: this can happen if the destination or the destination is shallow copy of the global database 
    //       and the variable has been removed from the global database
    bool found = dest->contains(dest_name);
    if(!found)
        throw std::invalid_argument("C API: Variable named '" + dest_name + "' seems to not exist in the destination database");
    double* dest_var_ptr = dest->get_var_ptr(dest_name);
    if(dest_var_ptr == NULL)
        throw std::runtime_error("C API: Could not get the values of the Variable named '" + dest_name + "'");
    dest_var_ptr += dest_t_first;

    // check that the source variable exists in the source database
    found = source->contains(source_name);
    if(!found)
        throw std::invalid_argument("C API: Variable named '" + source_name + "' seems to not exist in the source database");
    double* source_var_ptr = source->get_var_ptr(source_name);
    if(source_var_ptr == NULL)
        throw std::runtime_error("C API: Could not get the values of the Variable named '" + source_name + "'");
    source_var_ptr += source_t_first;

    // copy the data
    int nb_periods = dest_t_last - dest_t_first + 1;
    memcpy(dest_var_ptr, source_var_ptr, nb_periods * sizeof(double));
}

enum BinaryOperation 
{
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_POW
};

void _c_operation_scalar(const int op, KDBVariables* database, int t_first, int t_last, const double value)
{
    // sanity checks
    if(database == NULL)
        throw std::invalid_argument("C API: database is NULL");
    if(t_first < 0 || t_last < 0)
        throw std::invalid_argument("C API: time range must be non-negative");
    if(t_first > t_last)
        throw std::invalid_argument("C API: invalid time range -> t_first > t_last");
    if(t_first >= database->get_nb_periods())
        throw std::invalid_argument("C API: t_first exceeds the number of periods in the database");
    if(t_last >= database->get_nb_periods())
        throw std::invalid_argument("C API: t_last exceeds the number of periods in the database");

    // perform the operation
    double* var_ptr;
    switch(op)
    {
    case OP_ADD:
        for(auto& [name, _] : database->k_objs)
        {
            var_ptr = database->get_var_ptr(name);
            for(int t = t_first; t <= t_last; t++)
                var_ptr[t] += value;
        }
        break;
    case OP_SUB:  
        for(auto& [name, _] : database->k_objs)
        {
            var_ptr = database->get_var_ptr(name);
            for(int t = t_first; t <= t_last; t++)
                var_ptr[t] -= value;
        }
        break;
    case OP_MUL: 
        for(auto& [name, _] : database->k_objs)
        {
            var_ptr = database->get_var_ptr(name);
            for(int t = t_first; t <= t_last; t++)
                var_ptr[t] *= value;
        }
        break;
    case OP_DIV:
        if(value == 0)
            throw std::invalid_argument("C API: Division by zero");
        for(auto& [name, _] : database->k_objs)
        {
            var_ptr = database->get_var_ptr(name);
            for(int t = t_first; t <= t_last; t++)
                var_ptr[t] /= value;
        }
        break;
    case OP_POW: 
        for(auto& [name, _] : database->k_objs)
        {
            var_ptr = database->get_var_ptr(name);
            for(int t = t_first; t <= t_last; t++)
                var_ptr[t] = pow(var_ptr[t], value);
        }
        break;
    }
}

void _c_operation_one_period(const int op, KDBVariables* database, const int t, const double* values, const int nb_values)
{
    // sanity checks
    if(database == NULL)
        throw std::invalid_argument("C API: database is NULL");
    if(values == NULL)
        throw std::invalid_argument("C API: values is NULL");
    if(t < 0)
        throw std::invalid_argument("C API: time index 't' must be non-negative");
    if(t >= database->get_nb_periods())
        throw std::invalid_argument("C API: time index 't' exceeds the number of periods in the database");
    if(nb_values != database->size())
        throw std::invalid_argument("C API: number of values must match the number of variables in the database");

    KDB* db = database->get_database();
    if(db == NULL)
        throw std::invalid_argument("C API: database is NULL");

    int i = 0;
    double value;
    switch(op)
    {
    case OP_ADD:
        for(auto& [name, _] : database->k_objs)
            *KVVAL(db, name, t) += values[i++];
        break;
    case OP_SUB: 
        for(auto& [name, _] : database->k_objs) 
            *KVVAL(db, name, t) -= values[i++];
        break;
    case OP_MUL: 
        for(auto& [name, _] : database->k_objs) 
            *KVVAL(db, name, t) *= values[i++];
        break;
    case OP_DIV:
        for(auto& [name, _] : database->k_objs)
        {
            value = values[i++];
            if(value == 0)
                throw std::invalid_argument("C API: Division by zero");
            *KVVAL(db, name, t) /= value;
        }
        break;
    case OP_POW: 
        for(auto& [name, _] : database->k_objs)
            *KVVAL(db, name, t) = pow(*KVVAL(db, name, t), values[i++]);
        break;
    }
}

void _c_operation_one_var(const int op, KDBVariables* database, const std::string& name, int t_first, int t_last, const double* values)
{
    double value;

    // sanity checks
    if(database == NULL)
        throw std::invalid_argument("C API: database is NULL");
    if(values == NULL)
        throw std::invalid_argument("C API: values is NULL");
    if(name.empty())
        throw std::invalid_argument("C API: variable name is empty");
    if(t_first < 0)
        throw std::invalid_argument("C API: time index 't_first' must be non-negative");
    if(t_last < 0)
        throw std::invalid_argument("C API: time index 't_last' must be non-negative");
    if(t_first > t_last)
        throw std::invalid_argument("C API: time index 't_first' must be less than or equal to 't_last'");
    if(t_last >= database->get_nb_periods())
        throw std::invalid_argument("C API: time index 't_last' must be less than the number of time steps");

    bool found = database->contains(name);
    if(!found)
        throw std::invalid_argument("C API: Variable named '" + name + "' seems to not exist in the database");
    
    double* var_ptr = database->get_var_ptr(name);
    switch(op)
    {
    case OP_ADD:
        for(int t = t_first; t <= t_last; t++)
            var_ptr[t] += values[t - t_first];
        break;
    case OP_SUB: 
        for(int t = t_first; t <= t_last; t++) 
            var_ptr[t] -= values[t - t_first];
        break;
    case OP_MUL: 
        for(int t = t_first; t <= t_last; t++) 
            var_ptr[t] *= values[t - t_first];
        break;
    case OP_DIV:
        for(int t = t_first; t <= t_last; t++)
        {
            value = values[t - t_first];
            if(value == 0)
                throw std::invalid_argument("C API: Division by zero");
            var_ptr[t] /= value;
        }
        break;
    case OP_POW: 
        for(int t = t_first; t <= t_last; t++)
            var_ptr[t] = pow(var_ptr[t], values[t - t_first]);
        break;
    }
}

void _c_operation_between_two_vars(const int op, KDBVariables* database, const std::string& name, const int t_first, const int t_last, 
    KDBVariables* other, const std::string& other_name, const int other_t_first, const int other_t_last)
{
    if(name.empty())
    throw std::invalid_argument("C API: Destination variable name is empty");

    if(other_name.empty())
    throw std::invalid_argument("C API: Source variable name is empty");

    // sanity checks
    _c_sanity_checks(database, t_first, t_last, other, other_t_first, other_t_last);

    // check that the destination variable exists in the destination database
    // NOTE: this can happen if the destination or the destination is shallow copy of the global database 
    //       and the variable has been removed from the global database
    bool found = other->contains(other_name);
    if(!found)
        throw std::invalid_argument("C API: Variable named '" + other_name + "' seems to not exist in the source database");
    double* other_var_ptr = other->get_var_ptr(other_name);
    if(other_var_ptr == NULL)
        throw std::runtime_error("C API: Could not get the values of the Variable named '" + other_name + "'");
    other_var_ptr += other_t_first;

    _c_operation_one_var(op, database, name, t_first, t_last, other_var_ptr);
}
