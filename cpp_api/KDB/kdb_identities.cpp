#pragma once
#include "kdb_identities.h"


// CRUD (Create - Read - Update - Delete) + Copy methods

Identity KDBIdentities::copy_obj(const Identity& original) const
{
    return Identity(original);
}

Identity KDBIdentities::get_unchecked(const int pos) const
{
    return Identity(pos, get_KDB());
}

std::string KDBIdentities::get_lec(const int pos) const
{
    // throw exception if object with passed position is not valid
    get_name(pos);
    return std::string(KILEC(get_KDB(), pos));
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
    char* c_lec = obj.c_identity->lec;
    return KDBTemplate::add(name, c_lec);
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
    B_IdtExecuteTrace(trace ? "Y" : "N");

    Sample sample(from, to);
    char* c_identities_list = to_char_array(identities_list);
    char** idts = B_ainit_chk(c_identities_list, NULL, 0);

    int rc = B_IdtExecuteIdts(sample.c_sample, idts);
    SCR_free_tbl((unsigned char**) idts);
    if (rc != 0) B_display_last_error();
}

void KDBIdentities::execute_identities(const std::string& from, const std::string& to, const std::string& identities_list, 
        const std::string& var_files, const std::string& scalar_files, const bool trace)
{
    IodeExceptionInvalidArguments error("Cannot execute identities");
    if (from.empty()) error.add_argument("from", "empty");
    if (to.empty()) error.add_argument("to", "empty");
    if (error.invalid_args()) throw error;

    Period period_from(from);
    Period period_to(to);
    execute_identities(period_from, period_to, identities_list, var_files, scalar_files, trace);
}
