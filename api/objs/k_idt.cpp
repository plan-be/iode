#include "api/objs/identities.h"

std::vector<std::string> Identity::get_coefficients_list(const bool create_if_not_exit)
{
    if(this->clec == NULL)
        throw std::runtime_error("Please compute the identity " + std::string(this->lec) + " first");

    std::vector<std::string> coeffs = get_scalars_from_clec(this->clec);

    // create scalars not yet present in the Scalars Database
    if(create_if_not_exit)
    {
        for(const std::string& coeff_name: coeffs)
        {
            // adds a new scalar with values { 0.9, 1.0, IODE_NAN } to the Scalars Database
            // see K_add() and K_spack()
            if (!K_WS[SCALARS]->contains(coeff_name)) 
                K_add(K_WS[SCALARS], (char*) coeff_name.c_str(), NULL);
        }
    }

    return coeffs;
}

std::vector<std::string> Identity::get_variables_list(const bool create_if_not_exit)
{
    if(this->clec == NULL)
        throw std::runtime_error("Please compute the identity " + std::string(this->lec) + " first");

    std::vector<std::string> vars = get_variables_from_clec(this->clec);

    // create variables not yet present in the Variables Database
    if(create_if_not_exit)
    {
        Sample* sample = K_WS[VARIABLES]->sample;
        if(sample == NULL || sample->nb_periods == 0)
            throw std::runtime_error("Cannot return the list of variables associated with the identity " + 
                                    std::string(this->lec) +"\nThe global sample is not yet defined");

        int nb_obs = sample->nb_periods;
        for(const std::string& var_name: vars)
        {
            // adds a new variable with nb_obs IODE_NAN values to the Variables Database
            // see K_add() and K_vpack()
            if (!K_WS[VARIABLES]->contains(var_name))
                K_add(K_WS[VARIABLES], (char*) var_name.c_str(), NULL, &nb_obs);
        }
    }

    return vars;
}