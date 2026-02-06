#include "api/objs/equations.h"
#include "api/objs/identities.h"
#include "api/objs/scalars.h"
#include "api/report/commands/commands.h"


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
            // adds a new scalar with values { 0.9, 1.0, IODE_NAN } to the Scalars database
            if (!global_ws_scl->contains(coeff_name)) 
                global_ws_scl->set_obj_ptr(coeff_name, new Scalar(0.9, 1.0, IODE_NAN));
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
            // adds a new variable with nb_obs IODE_NAN values to the Variables database
            if (!global_ws_var->contains(var_name))
                global_ws_var->set_obj_ptr(var_name, new Variable(nb_obs, IODE_NAN));
        }
    }

    return vars;
}

bool Identity::to_binary(char** pack) const
{
    std::string lec = this->get_lec();
    char* c_lec = (char*) lec.c_str();

    *pack = 0;
    if(lec.empty()) 
        return false;

    CLEC* clec = L_cc(c_lec);
    if(clec == NULL)  
        return false;
    
    *pack = (char*) P_create();
    *pack = (char*) P_add(*pack, c_lec, (int) strlen(c_lec) + 1);
    *pack = (char*) P_add(*pack, (char*) clec, clec->tot_lg);

    SW_nfree(clec);
    return true;
}

std::string KDBIdentities::get_lec(const std::string& name) const
{
    // throw exception if object with passed position is not valid
    if(!this->contains(name))
        throw std::invalid_argument("Cannot get the LEC of the identity with name '" + name + 
                                    "' in the database of '" + get_iode_type_str() + "'.\n" +  
                                    "The identity with name '" + name + "' does not exist.");
    Identity* idt = this->get_obj_ptr(name);
    std::string lec = idt->get_lec();
    return lec;
}

bool KDBIdentities::add(const std::string& name, const std::string& lec)
{
    Identity idt(lec);
    return KDBTemplate::add(name, idt);
}

void KDBIdentities::update(const std::string& name, const std::string& lec)
{
    Identity idt(lec);
    KDBTemplate::update(name, idt);
}

bool KDBIdentities::execute_identities(const Period& from, const Period& to, const std::string& identities_list, 
        const std::string& var_files, const std::string& scalar_files, const bool trace)
{
    B_IdtExecuteVarFiles(to_char_array(var_files));
    B_IdtExecuteSclFiles(to_char_array(scalar_files));
    B_IdtExecuteTrace(const_cast<char*>(trace ? "Y" : "N"));

    Sample sample(from, to);
    char* c_identities_list = to_char_array(identities_list);
    char** idts = B_ainit_chk(c_identities_list, NULL, 0);

    int rc = B_IdtExecuteIdts(&sample, idts);
    SCR_free_tbl((unsigned char**) idts);
    if(rc != 0)
    {
        std::string last_error = error_manager.get_last_error();
        if(!last_error.empty())
        {
            error_manager.append_error("Cannot execute identities '" + identities_list + "':\n" + last_error);
            error_manager.display_last_error();
        }
        return false;
    }
    
    return true;
}

bool KDBIdentities::execute_identities(const std::string& from, const std::string& to, const std::string& identities_list, 
        const std::string& var_files, const std::string& scalar_files, const bool trace)
{
    std::string error_msg;
    if (from.empty()) 
        error_msg += "from_period is empty\n";
    if (to.empty()) 
        error_msg += "to_period is empty\n";
    if (!error_msg.empty()) 
        throw std::invalid_argument("Cannot execute identities:\n" + error_msg);

    Period period_from(from);
    Period period_to(to);
    return execute_identities(period_from, period_to, identities_list, var_files, scalar_files, trace);
}

bool KDBIdentities::binary_to_obj(const std::string& name, char* pack)
{
    size_t len = (size_t) P_get_len(pack, 0);
    char* c_lec = new char[len];
    strncpy(c_lec, (char*) P_get_ptr(pack, 0), len);

    Identity* idt = new Identity(std::string(c_lec));
    this->k_objs[name] = idt;
    return true;
}

/**
 * Serializes an identity. 
 *
 * @param [out] pack    (char **)   placeholder for the pointer to the serialized object
 * @param [in]  name    string      identity name
 * @return                          true if the serialization succeeded, false otherwise if ok, -1 if a1 is NULL or the comp
 */
bool KDBIdentities::obj_to_binary(char** pack, const std::string& name)
{
    Identity* idt = this->get_obj_ptr(name);
    if(!idt)
    {
        std::string error_msg = "Cannot serialize identity with name '" + name + 
                                "': no identity with this name exists in the database of '" + get_iode_type_str() + "'.";
        kwarning(error_msg.c_str());
        return false;
    }
    
    bool success = idt->to_binary(pack);
    return success;
}

bool KDBIdentities::grep_obj(const std::string& name, const std::string& pattern, 
    const bool ecase, const bool forms, const bool texts, const char all) const
{
    bool found = false;
    if(forms)
    {
        Identity* idt = this->get_obj_ptr(name);
        std::string lec = idt->get_lec();
        found = wrap_grep_gnl(pattern, lec, ecase, all);
    } 
    return found;
}

char* KDBIdentities::dde_create_obj_by_name(const std::string& name, int* nc, int* nl)
{
    std::string lec = this->get_obj_ptr(name)->get_lec();
    char* obj = new char[lec.size() + 1];
    strncpy(obj, lec.c_str(), lec.size() + 1);
    return obj;
}

bool KDBIdentities::print_obj_def(const std::string& name)
{
    if(!this->contains(name)) 
        return false;
    
    std::string lec = this->get_obj_ptr(name)->get_lec();
    std::string tmp = name + " : " + lec;
    CLEC* clec = this->get_obj_ptr(name)->get_compiled_lec();

    W_printf((char*) ".par1 enum_1\n");
    print_lec_definition(name, tmp, clec, B_EQS_LEC);

    return true;
}

void KDBIdentities::update_reference_db()
{
    if(global_ref_idt[0]) 
        delete global_ref_idt[0];
    global_ref_idt[0] = new KDBIdentities(this, "*", false);      
}
