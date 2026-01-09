#include "api/objs/equations.h"
#include "api/objs/identities.h"
#include "api/objs/scalars.h"


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
            // see add() and K_spack()
            if (!global_ws_scl->contains(coeff_name)) 
                global_ws_scl->set_obj(coeff_name, nullptr);
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
        Sample* sample = global_ws_var->sample;
        if(sample == NULL || sample->nb_periods == 0)
            throw std::runtime_error("Cannot return the list of variables associated with the identity " + 
                                    std::string(this->lec) +"\nThe global sample is not yet defined");

        int nb_obs = sample->nb_periods;
        for(const std::string& var_name: vars)
        {
            // adds a new variable with nb_obs IODE_NAN values to the Variables Database
            // see add() and K_vpack()
            if (!global_ws_var->contains(var_name))
                global_ws_var->set_obj(var_name, (double*) NULL);
        }
    }

    return vars;
}


Identity* CKDBIdentities::get_obj(const SWHDL handle) const
{    
    char* ptr = SW_getptr(handle);
    if(ptr == nullptr)  
        return nullptr;
    return K_iunpack(ptr);
}

Identity* CKDBIdentities::get_obj(const std::string& name) const
{
    SWHDL handle = this->get_handle(name);
    if(handle == 0)  
        throw std::invalid_argument("Identity with name '" + name + "' not found.");
    
    char* ptr = SW_getptr(handle);
    if(ptr == nullptr)  
        return nullptr;
    return K_iunpack(ptr);
}

bool CKDBIdentities::set_obj(const std::string& name, const Identity* value)
{
    char* pack = NULL;
    std::string lec = value->get_lec();
    K_ipack(&pack, (char*) lec.c_str());
    bool success = set_packed_object(name, pack);
    if(!success)
    {
        std::string error_msg = "Failed to set identity object '" + name + "'";
        kwarning(error_msg.c_str());
    }
    return success;
}

bool CKDBIdentities::grep_obj(const std::string& name, const SWHDL handle, 
    const std::string& pattern, const bool ecase, const bool forms, const bool texts, 
    const char all) const
{
    bool found = false;
    if(forms)
    {
        std::string lec = const_cast<CKDBIdentities*>(this)->get_obj(handle)->get_lec();
        found = wrap_grep_gnl(pattern, lec, ecase, all);
    } 
    return found;
}

char* CKDBIdentities::dde_create_obj_by_name(const std::string& name, int* nc, int* nl)
{
    std::string lec = this->get_obj(name)->get_lec();
    char* obj = new char[lec.size() + 1];
    strncpy(obj, lec.c_str(), lec.size() + 1);
    return obj;
}

bool CKDBIdentities::print_obj_def(const std::string& name)
{
    if(!this->contains(name)) 
        return false;
    
    std::string lec = this->get_obj(name)->get_lec();
    std::string tmp = name + " : " + lec;
    CLEC* clec = this->get_obj(name)->get_compiled_lec();

    W_printf((char*) ".par1 enum_1\n");
    print_lec_definition(name, tmp, clec, B_EQS_LEC);

    return true;
}

void CKDBIdentities::update_reference_db()
{
    K_RWS[this->k_type][0] = new CKDBIdentities(this, "*");      
}
