#pragma one
#include "api/iode.h"
#include "cpp_api/iode_cpp_api.h"
#include <string>
#include <vector>
#include <stdexcept>


static void _c_sanity_checks(KDBVariables* dest, const int dest_t_first, const int dest_t_last, 
                             KDBVariables* source, const int source_t_first, const int source_t_last)
{
    if(source == nullptr)
        throw std::invalid_argument("Variables Source database is empty");

    if(source->get_database() == NULL)
       throw std::invalid_argument("Source Variables database is empty");

    if(source->count() == 0)
       throw std::invalid_argument("Source Variables database is empty");

    if(dest == nullptr)
        throw std::invalid_argument("Destination Variables database is empty");

    if(dest->get_database() == NULL)
      throw std::invalid_argument("Destination Variables database is empty");

    if(dest->count() == 0)
      throw std::invalid_argument("Destination Variables database is empty");

    if(dest_t_first < 0 || dest_t_last < 0 || source_t_first < 0 || source_t_last < 0)
        throw std::invalid_argument("Periods must be positive");

    if(dest_t_first > dest_t_last)
        throw std::invalid_argument("Destination first period " + std::to_string(dest_t_first) + 
                                    " is greater than last period " + std::to_string(dest_t_last));

    if(source_t_first > source_t_last)
        throw std::invalid_argument("Source first period " + std::to_string(source_t_first) + 
                                    " is greater than last period " + std::to_string(source_t_last));

    int nb_periods = source->get_nb_periods();
    if(source_t_last >= nb_periods)
       throw std::invalid_argument("Source last period " + std::to_string(source_t_last) + " must be lower than " + 
                                   "the number of periods (= " + std::to_string(nb_periods) + ") in the source database");

    nb_periods = dest->get_nb_periods();
    if(dest_t_last >= nb_periods)
       throw std::invalid_argument("Destination last period " + std::to_string(dest_t_last) + " must be lower than " + 
                                   "the number of periods (= " + std::to_string(nb_periods) + ") in the destination database");
    
    int dest_nb_periods = dest_t_last - dest_t_first + 1;
    int source_nb_periods = source_t_last - source_t_first + 1;
    // check that the number of values to copy matches between the periods ranges of the two databases
    if(dest_nb_periods != source_nb_periods)
        throw std::invalid_argument("The periods ranges between the source (" + std::to_string(source_nb_periods) + " periods) " + 
                                    "and destination (" + std::to_string(dest_nb_periods) + " periods) databases do not match");
}

void _c_add_var_from_other(const std::string& name, KDBVariables* dest, KDBVariables* source, 
                           const int source_t_first, const int source_t_last)
{
    if(name.empty())
        throw std::invalid_argument("Name of the new variable is empty");

    // sanity checks
    int nb_periods = dest->get_nb_periods();
    _c_sanity_checks(dest, 0, nb_periods - 1, source, source_t_first, source_t_last);

    // check that the source variable exists in the source database
    // NOTE: this can happen if the source or the destination is shallow copy of the global database 
    //       and the variable has been removed from the global database
    // TODO: find a way to delete also the variable from the shallow copies
    double* source_var_ptr = source->get_var_ptr(0);
    if(source_var_ptr == NULL)
    {
        std::string source_name = source->get_name(0);
        throw std::invalid_argument("Variable named '" + source_name + "' seems to no longer exist in the source database");
    }

    // add the variable to the destination database
    KDB* kdb = dest->get_database();
    source_var_ptr += source_t_first;
    K_add(kdb, const_cast<char*>(name.c_str()), source_var_ptr, &nb_periods);
}

void _c_copy_var_content(const std::string& name, KDBVariables* dest, const int dest_t_first, const int dest_t_last, 
                         KDBVariables* source,const int source_t_first, const int source_t_last)
{
    if(name.empty())
       throw std::invalid_argument("Destination variable name is empty");

    // sanity checks
    _c_sanity_checks(dest, dest_t_first, dest_t_last, source, source_t_first, source_t_last);

    // check that the destinaton variable exists in the destination database
    int dest_var_pos = dest->get_position(name);

    // check that the source/destinaton variable exists in the source/destinaton database
    // NOTE: this can happen if the destinaton or the destination is shallow copy of the global database 
    //       and the variable has been removed from the global database
    // TODO: find a way to delete also the variable from the shallow copies

    double* source_var_ptr = source->get_var_ptr(0);
    if(source_var_ptr == NULL)
    {
        std::string source_name = source->get_name(0);
        throw std::invalid_argument("Variable named '" + source_name + "' seems to no longer exist in the source database");
    }
    source_var_ptr += source_t_first;

    double* dest_var_ptr = dest->get_var_ptr(name);
    if(dest_var_ptr == NULL)
        throw std::invalid_argument("Variable named '" + name + "' seems to no longer exist in the destination database");
    dest_var_ptr += dest_t_first;

    // copy the data
    int nb_periods = dest_t_last - dest_t_first + 1;
    memcpy(dest_var_ptr, source_var_ptr, nb_periods * sizeof(double));
}
