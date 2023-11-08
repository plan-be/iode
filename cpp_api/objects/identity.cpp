#include "identity.h"


void Identity::copy_from_IDT_obj(const IDT* obj)
{
    this->lec = copy_char_array(obj->lec);
    // NOTE : we do not use memcpy() because memcpy() actually makes  
    //        a shallow copy of a struct instead of a deep copy
    this->clec = clec_deep_copy(obj->clec);
}

Identity::Identity(const int pos, KDB* kdb)
{
    if (!kdb) 
        kdb = K_WS[I_IDENTITIES];

    if (pos < 0 || pos > kdb->k_nb)
    {
        IodeExceptionInvalidArguments error("Cannot extract Identity", "Identity position must be in range [0, " + 
            std::to_string(kdb->k_nb - 1) + "])");
        error.add_argument("identity position", std::to_string(pos));
        throw error;
    }
    
    this->lec  = copy_char_array(KILEC(kdb, pos));
    this->clec = clec_deep_copy(KICLEC(kdb, pos));
}

Identity::Identity(const std::string& name, KDB* kdb)
{
    if (!kdb) 
        kdb = K_WS[I_IDENTITIES];

    int pos = K_find(kdb, to_char_array(name));
    if (pos < 0) 
        throw IodeExceptionFunction("Cannot extract Identity", "Identity with name " + name + " does not exist.");
    
    this->lec  = copy_char_array(KILEC(kdb, pos));
    this->clec = clec_deep_copy(KICLEC(kdb, pos));
}

Identity::Identity(const std::string& lec)
{
    set_lec(lec);
}

Identity::Identity(const Identity& other)
{
    copy_from_IDT_obj(static_cast<const IDT*>(&other));
}

Identity::~Identity()
{
    if(this->lec)
        SW_nfree(this->lec);

    if(this->clec)
        SW_nfree(this->clec);
}

std::string Identity::get_lec() const 
{ 
    return std::string(this->lec); 
}

void Identity::set_lec(const std::string& lec) 
{
    this->lec = copy_string_to_char(lec);
    // L_cc returns an allocated CLEC struct pointer.
    this->clec = L_cc(to_char_array(lec));
}

CLEC* Identity::get_clec() const 
{ 
    return this->clec; 
}

// required to be used in std::map
Identity& Identity::operator=(const Identity& other)
{
    copy_from_IDT_obj(static_cast<const IDT*>(&other));
    return *this;
}

bool Identity::operator==(const Identity& other) const
{
    return this->get_lec() == other.get_lec();
}

std::vector<std::string> Identity::get_coefficients_list(const bool create_if_not_exit)
{
    if(this->clec == NULL)
        throw IodeException("Please compute the identity " + std::string(this->lec) + " first");

    std::vector<std::string> coeffs = get_scalars_from_clec(this->clec);

    // create scalars not yet present in the Scalars Database
    if(create_if_not_exit)
    {
        char* c_name;
        for(const std::string& coeff_name: coeffs)
        {
            c_name = const_cast<char*>(coeff_name.data());
            // adds a new scalar with values { 0.9, 1.0, L_NAN } to the Scalars Database
            // see K_add() and K_spack()
            if (K_find(K_WS[I_SCALARS], c_name) < 0) 
                K_add(K_WS[I_SCALARS], c_name, NULL);
        }
    }

    return coeffs;
}

std::vector<std::string> Identity::get_variables_list(const bool create_if_not_exit)
{
    if(this->clec == NULL)
        throw IodeException("Please compute the identity " + std::string(this->lec) + " first");

    std::vector<std::string> vars = get_variables_from_clec(this->clec);

    // create variables not yet present in the Variables Database
    if(create_if_not_exit)
    {
        SAMPLE* sample = KSMPL(K_WS[I_VARIABLES]);
        if(sample == NULL || sample->s_nb == 0)
            throw IodeException("Cannot return the list of variables associated with the identity " + 
                                std::string(this->lec) +"\nThe global sample is not yet defined");

        char* c_name;
        int nb_obs = sample->s_nb;
        for(const std::string& var_name: vars)
        {
            c_name = const_cast<char*>(var_name.data());
            // adds a new variable with nb_obs L_NAN values to the Variables Database
            // see K_add() and K_vpack()
            if (K_find(K_WS[I_VARIABLES], c_name) < 0) 
                K_add(K_WS[I_VARIABLES], c_name, NULL, &nb_obs);
        }
    }

    return vars;
}
