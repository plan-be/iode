#pragma once
#include "kdb_identities.h"


// CRUD (Create - Read - Update - Delete) + Copy methods

void KDBIdentities::add_or_update(const std::string& name, const Identity& identity)
{
    char* c_name = const_cast<char*>(name.c_str());
    int res = K_add(get_KDB(), c_name, identity.lec);
    if (res == -1) throw std::runtime_error("Iode has not been initialized");
    if (res < -1) throw std::runtime_error("Something went wrong when trying to set " + vIodeTypes[type] + " with name " + name);
}

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

void KDBIdentities::add_or_update(const std::string& name, const std::string& lec)
{
    char* c_name = const_cast<char*>(name.c_str());
    char* c_lec = const_cast<char*>(lec.c_str());
    int res = K_add(get_KDB(), c_name, c_lec);
    if (res == -1) throw std::runtime_error("Iode has not been initialized");
    if (res < -1) throw std::runtime_error("Something went wrong when trying to set " + vIodeTypes[type] + " with name " + name);
}

void KDBIdentities::add(const std::string& name, const std::string& lec)
{
    // throw exception if object with passed name already exist
    char* c_name = const_cast<char*>(name.c_str());
    if (K_find(get_KDB(), c_name) >= 0) throw std::runtime_error(vIodeTypes[type] + " with name " + name + " already exists. Use update() method instead.");
    add_or_update(name, lec);
}

void KDBIdentities::update(const std::string& name, const std::string& lec)
{
    // throw exception if object with passed name does not exist
    get_position(name);
    add_or_update(name, lec);
}
