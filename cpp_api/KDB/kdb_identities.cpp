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
