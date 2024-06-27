#pragma once

#include "cpp_api/iode_cpp_api.h"
#include <string>
#include <vector>
#include <stdexcept>

#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/ndarray.h>

namespace nb = nanobind;


/*
 * nanobind supports zero-copy exchange using two protocols:
 * - The classic buffer protocol.
 * - DLPack, a GPU-compatible generalization of the buffer protocol.
 * 
 * see https://nanobind.readthedocs.io/en/latest/ndarray.html for documentation
 */

using numpy_var = nb::ndarray<nb::numpy, double, nb::ndim<1>>;
using numpy_var_contiguous = nb::ndarray<nb::numpy, double, nb::c_contig, nb::ndim<1>>;

using numpy_vars = nb::ndarray<nb::numpy, double, nb::ndim<2>>;
using numpy_vars_contiguous = nb::ndarray<nb::numpy, double, nb::c_contig, nb::ndim<2>>;

struct VarInput
{
    std::string name;
    int nb_periods;
    int t_start;
    int t_end;

    VarInput(const std::string& first_period, const std::string& last_period)
    {
        if(Variables.get_nb_periods() == 0)
            Variables.set_sample(first_period, last_period);

        Sample input_sample(first_period, last_period);
        this->nb_periods = input_sample.nb_periods();

        // throw an error if the first or last period from 'periods' is not in the Variables sample
        Sample sample = Variables.get_sample();
        this->t_start = sample.get_period_position(first_period);
        this->t_end = sample.get_period_position(last_period);
    }
};

/** Get an IODE variable in a numpy ndarray
 * see https://nanobind.readthedocs.io/en/latest/ndarray.html#binding-functions-that-return-arrays 
 *     https://nanobind.readthedocs.io/en/latest/ndarray.html#constraint-types 
 *     https://nanobind.readthedocs.io/en/latest/api_extra.html#array-annotations 
 */
inline numpy_var get_var_as_ndarray(const std::string& name)
{
    size_t nb_obs = (size_t) Variables.get_nb_periods();
    if(nb_obs == 0)
        return numpy_var(NULL, { 0 });

    double* var_ptr = Variables.get_var_ptr(name);
    return numpy_var(var_ptr, { nb_obs });
}


inline void __set_var_from_ndarray(const std::string& name, const std::string& first_period, 
    const std::string& last_period, numpy_var& array)
{
    VarInput vi(first_period, last_period);

    if(array.shape(0) != vi.nb_periods)
        throw std::invalid_argument("Cannot add or update of the Variable '" + name + "'.\n" + 
            "Expected Python data array of size " + std::to_string(vi.nb_periods) + 
            " but got array of size " + std::to_string(array.shape(0)));

    Sample sample = Variables.get_sample();
    int nb_periods_ws = sample.nb_periods();

    auto v = array.view();

    // add new Variable
    if(!Variables.contains(name))
    {
        Variable variable(nb_periods_ws, IODE_NAN);
        double* var_data_ptr = variable.data();
    
        // fill leading values with IODE NaN
        for(int j=0; j < vi.t_start; j++)
            var_data_ptr[j] = IODE_NAN;
        
        // copy values from the passed Numpy array
        for(int j=0; j < vi.nb_periods; j++) 
            var_data_ptr[vi.t_start + j] = v(j);

        // fill trailing values with IODE NaN
        for(int j=vi.t_end; j < nb_periods_ws; j++)
            var_data_ptr[j] = IODE_NAN;

        Variables.add(name, variable);
    }
    // update existing Variable
    else
    {
        Variable values;
        values.reserve(vi.nb_periods);
        
        // fill vector of values
        for(int j=0; j < vi.nb_periods; j++)
            values.push_back(v(j));
        
        if(vi.t_start == 0 && vi.nb_periods == nb_periods_ws)
            Variables.update(name, values);
        else
            // update() with t_start argument calls get(name) internally which takes time
            Variables.update(name, vi.t_start, values);
    }
}

inline void __set_var_from_ndarray_contiguous(const std::string& name, const std::string& first_period, 
    const std::string& last_period, numpy_var_contiguous& array)
{
    VarInput vi(first_period, last_period);

    if(array.shape(0) != vi.nb_periods)
        throw std::invalid_argument("Cannot add or update of the Variable '" + name + "'.\n" + 
            "Expected Python data array of size " + std::to_string(vi.nb_periods) + 
            " but got array of size " + std::to_string(array.shape(0)));
    
    Sample sample = Variables.get_sample();
    int nb_periods_ws = sample.nb_periods();

    double* np_data_ptr = array.data();

    // add new Variable
    if(!Variables.contains(name))
    {
        Variable variable(nb_periods_ws, IODE_NAN);
        double* var_data_ptr = variable.data();
    
        // fill leading values with IODE NaN
        for(int j=0; j < vi.t_start; j++)
            var_data_ptr[j] = IODE_NAN;
        
        // copy values from the passed Numpy array
        memcpy(var_data_ptr + vi.t_start, np_data_ptr, vi.nb_periods * sizeof(double));

        // fill trailing values with IODE NaN
        for(int j=vi.t_end; j < nb_periods_ws; j++)
            var_data_ptr[j] = IODE_NAN;

        Variables.add(name, variable);
    }
    // update existing Variable
    else
    {
        double* var_data_ptr = Variables.get_var_ptr(name);
        memcpy(var_data_ptr + vi.t_start, np_data_ptr, vi.nb_periods * sizeof(double));
    }
}

inline void __set_vars_from_ndarray(const std::vector<std::string>& names, const std::string& first_period, 
    const std::string& last_period, numpy_vars& array)
{
    VarInput vi(first_period, last_period);

    int nb_names = (int) names.size();
    if(array.shape(0) != nb_names || array.shape(1) != vi.nb_periods)
        throw std::invalid_argument(std::string("Cannot update the values from the Variables workspace.\n") + 
            "Expected Python data array of shape (" + std::to_string(nb_names) + ", " + std::to_string(vi.nb_periods) + 
            ") but got array of shape (" + std::to_string(array.shape(0)) + ", " + std::to_string(array.shape(1)) + ")");

    Sample sample = Variables.get_sample();
    int nb_periods_ws = sample.nb_periods();

    std::string name;
    Variable variable(nb_periods_ws, IODE_NAN);
    Variable values(vi.nb_periods, IODE_NAN);
    
    auto v = array.view();
    for(int i=0; i < nb_names; i++)
    {
        name = names[i];

        // add new Variable
        if(!Variables.contains(name))
        {
            double* var_data_ptr = variable.data();
            // fill leading values with IODE NaN
            for(int j=0; j < vi.t_start; j++)
                var_data_ptr[j] = IODE_NAN;
            
            // copy values from the passed Numpy array
            for(int j=0; j < vi.nb_periods; j++) 
                var_data_ptr[vi.t_start + j] = v(i, j);

            // fill trailing values with IODE NaN
            for(int j=vi.t_end; j < nb_periods_ws; j++)
                var_data_ptr[j] = IODE_NAN;

            Variables.add(name, variable);
        }
        // update existing Variable
        else
        {
            // fill vector of values
            for(int j=0; j < vi.nb_periods; j++)
                values[j] = v(i, j);
            
            if(vi.t_start == 0 && vi.nb_periods == nb_periods_ws)
                Variables.update(name, values);
            else
                // update() with t_start argument calls get(name) internally which takes time
                Variables.update(name, vi.t_start, values);
        }
    }
}

inline void __set_vars_from_ndarray_contiguous(const std::vector<std::string>& names, const std::string& first_period, 
    const std::string& last_period, numpy_vars_contiguous& array)
{
    VarInput vi(first_period, last_period);

    int nb_names = (int) names.size();
    if(array.shape(0) != nb_names || array.shape(1) != vi.nb_periods)
        throw std::invalid_argument(std::string("Cannot update the values from the Variables workspace.\n") + 
            "Expected Python data array of shape (" + std::to_string(nb_names) + ", " + std::to_string(vi.nb_periods) + 
            ") but got array of shape (" + std::to_string(array.shape(0)) + ", " + std::to_string(array.shape(1)) + ")");

    Sample sample = Variables.get_sample();
    int nb_periods_ws = sample.nb_periods();

    std::string name;
    Variable variable(nb_periods_ws, IODE_NAN);
    Variable values(vi.nb_periods, IODE_NAN);
    
    double* np_data_ptr = array.data();
    for(int i=0; i < nb_names; i++)
    {
        name = names[i];

        // add new Variable
        if(!Variables.contains(name))
        {
            double* var_data_ptr = variable.data();
            // fill leading values with IODE NaN
            for(int j=0; j < vi.t_start; j++)
                var_data_ptr[j] = IODE_NAN;
            
            // copy values from the passed Numpy array
            memcpy(var_data_ptr + vi.t_start, np_data_ptr + (i * vi.nb_periods), vi.nb_periods * sizeof(double));

            // fill trailing values with IODE NaN
            for(int j=vi.t_end; j < nb_periods_ws; j++)
                var_data_ptr[j] = IODE_NAN;

            Variables.add(name, variable);
        }
        // update existing Variable
        else
        {
            double* var_data_ptr = Variables.get_var_ptr(name);
            memcpy(var_data_ptr + vi.t_start, np_data_ptr + (i * vi.nb_periods), vi.nb_periods * sizeof(double));
        }
    }
}
