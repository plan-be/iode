#pragma once
#include "kdb_identities.h"


// CRUD (Create - Read - Update - Delete) + Copy methods

Identity* KDBIdentities::copy_obj(Identity* const original) const
{
    return new Identity(*original);
}

Identity* KDBIdentities::get_unchecked(const int pos) const
{
	KDB* kdb = get_database();
	std::string lec(KILEC(kdb, pos));
	Identity* idt = new Identity(lec);
	return idt;
}

std::string KDBIdentities::get_lec(const int pos) const
{
    // throw exception if object with passed position is not valid
    get_name(pos);
    return std::string(KILEC(get_database(), pos));
}

std::string KDBIdentities::get_lec(const std::string& name) const
{
    // throw exception if object with passed name does not exist
    int pos = index_of(name);
    return get_lec(pos);
}

bool KDBIdentities::add(const std::string& name, const std::string& lec)
{
    char* c_lec = to_char_array(lec);
    return KDBTemplate::add(name, c_lec);
}

bool KDBIdentities::add(const std::string& name, const Identity& obj)
{
    return KDBTemplate::add(name, (char*) obj.lec.c_str());
}

void KDBIdentities::update(const std::string& name, const std::string& lec)
{
    char* c_lec = to_char_array(lec);
    KDBTemplate::update(name, c_lec);
}

void KDBIdentities::update(const int pos, const std::string& lec)
{
    char* c_lec = to_char_array(lec);
    KDBTemplate::update(pos, c_lec);
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
