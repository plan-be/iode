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
	
	IDT* c_idt = (IDT*) SW_nalloc(sizeof(IDT));
    c_idt->lec = copy_char_array(KILEC(kdb, pos));
    // NOTE : we do not use memcpy() because memcpy() actually makes  
    //        a shallow copy of a struct instead of a deep copy
    c_idt->clec = clec_deep_copy(KICLEC(kdb, pos));
	return static_cast<Identity*>(c_idt);
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
    int pos = get_position(name);
    return get_lec(pos);
}

int KDBIdentities::add(const std::string& name, const std::string& lec)
{
    char* c_lec = to_char_array(lec);
    return KDBTemplate::add(name, c_lec);
}

int KDBIdentities::add(const std::string& name, const Identity& obj)
{
    return KDBTemplate::add(name, obj.lec);
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

void KDBIdentities::execute_identities(const Period& from, const Period& to, const std::string& identities_list, 
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
    if (rc != 0)
    {
        std::string last_error = get_last_error();
        if(!last_error.empty())
            throw std::runtime_error("Cannot execute identities '" + identities_list + "'\n" + last_error);
    }
}

void KDBIdentities::execute_identities(const std::string& from, const std::string& to, const std::string& identities_list, 
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
    execute_identities(period_from, period_to, identities_list, var_files, scalar_files, trace);
}
