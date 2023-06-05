#include "identity.h"

static IDT* extract_identity(KDB* kdb, const int pos)
{
    return K_iunpack(SW_getptr(kdb->k_objs[pos].o_val));
}


Identity::Identity()
{
    c_identity = nullptr;
}

Identity::Identity(const int pos, KDB* kdb)
{
    if (!kdb) kdb = K_WS[I_IDENTITIES];
    if (pos < 0 || pos > kdb->k_nb)
    {
        IodeExceptionInvalidArguments error("Cannot extract Identity", "Identity position must be in range [0, " + 
            std::to_string(kdb->k_nb - 1) + "])");
        error.add_argument("identity position", std::to_string(pos));
        throw error;
    }
    c_identity = extract_identity(kdb, pos);
}

Identity::Identity(const std::string& name, KDB* kdb)
{
    if (!kdb) kdb = K_WS[I_IDENTITIES];
    int pos = K_find(kdb, to_char_array(name));
    if (pos < 0) 
        throw IodeExceptionFunction("Cannot extract Identity", "Identity with name " + name + " does not exist.");
    c_identity = extract_identity(kdb, pos);
}

Identity::Identity(const std::string& lec)
{
    // QUESTION FOR JMP: Should I allocate c_identity->clec by default ?
    c_identity = (IDT*) SW_nalloc(sizeof(IDT));
    c_identity->lec = copy_string_to_char(lec);
    c_identity->clec = L_cc(to_char_array(lec));
}

Identity::Identity(const Identity& idt)
{
    c_identity = (IDT*) SW_nalloc(sizeof(IDT));
    c_identity->lec = copy_char_array(idt.c_identity->lec);
    c_identity->clec = (CLEC*) SW_nalloc(sizeof(CLEC));
    memcpy(c_identity->clec, idt.c_identity->clec, sizeof(CLEC));
}

Identity::~Identity()
{
    if(c_identity)
    {
        SW_nfree(c_identity->lec);
        SW_nfree(c_identity->clec);
        SW_nfree(c_identity);
    }
}

std::vector<std::string> Identity::get_coefficients_list(const bool create_if_not_exit)
{
    if(c_identity->clec == NULL)
        throw IodeException("Please compute the identity " + std::string(c_identity->lec) + " first");

    std::vector<std::string> coeffs = get_scalars_from_clec(c_identity->clec);

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
    if(c_identity->clec == NULL)
        throw IodeException("Please compute the identity " + std::string(c_identity->lec) + " first");

    std::vector<std::string> vars = get_variables_from_clec(c_identity->clec);

    // create variables not yet present in the Variables Database
    if(create_if_not_exit)
    {
        SAMPLE* sample = KSMPL(K_WS[I_VARIABLES]);
        if(sample == NULL || sample->s_nb == 0)
            throw IodeException("Cannot return the list of variables associated with the identity " + 
                                std::string(c_identity->lec) +"\nThe global sample is not yet defined");

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

// required to be used in std::map
Identity& Identity::operator=(const Identity& idt)
{
    c_identity->lec = copy_char_array(idt.c_identity->lec);
    c_identity->clec = L_cc(idt.c_identity->lec);
    return *this;
}

bool Identity::operator==(const Identity& other) const
{
    return this->get_lec() == other.get_lec();
}