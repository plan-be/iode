#pragma once
#include "kdb_identities.h"


// CRUD (Create - Read - Update - Delete) + Copy methods

Identity KDBIdentities::copy_obj(const Identity& original) const
{
    Identity identity_copy;
    identity_copy.lec = (char*) SCR_stracpy((unsigned char*) original.lec);
    identity_copy.clec = (CLEC*) SW_nalloc(sizeof(CLEC));
    memcpy(identity_copy.clec, original.clec, sizeof(CLEC));
    return identity_copy;
}

Identity KDBIdentities::get_unchecked(const int pos) const
{
    Identity identity;
    identity.lec = KILEC(get_KDB(), pos);
    identity.clec = KICLEC(get_KDB(), pos);
    return identity;
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
    char* c_lec = const_cast<char*>(lec.c_str());
    return KDBTemplate::add(name, c_lec);
}

void KDBIdentities::update(const std::string& name, const std::string& lec)
{
    char* c_lec = const_cast<char*>(lec.c_str());
    KDBTemplate::update(name, c_lec);
}

void KDBIdentities::update(const int pos, const std::string& lec)
{
    char* c_lec = const_cast<char*>(lec.c_str());
    KDBTemplate::update(pos, c_lec);
}

void KDBIdentities::execute_identities(const Period& from, const Period& to, const std::string& identities_list, 
        const std::string& var_files, const std::string& scalar_files, const bool trace)
{
    IodeExceptionInvalidArguments error("Cannot execute identities");
    if (identities_list.empty()) error.add_argument("identities_list", "empty");
    if (error.invalid_args()) throw error;

    B_IdtExecuteVarFiles(const_cast<char*>(var_files.c_str()));
    B_IdtExecuteSclFiles(const_cast<char*>(scalar_files.c_str()));
    B_IdtExecuteTrace(trace ? "Y" : "N");

    Sample sample(from, to);
    char* c_identities_list = const_cast<char*>(identities_list.c_str());
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
    if (identities_list.empty()) error.add_argument("identities_list", "empty");
    if (error.invalid_args()) throw error;

    Period period_from(from);
    Period period_to(to);
    execute_identities(period_from, period_to, identities_list, var_files, scalar_files, trace);
}
