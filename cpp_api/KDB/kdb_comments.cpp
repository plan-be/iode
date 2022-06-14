#pragma once
#include "kdb_comments.h"


// CRUD (Create - Read - Update - Delete) + Copy methods

int KDBComments::add_or_update(const std::string& name, const Comment& comment)
{
    std::string oem_string = utf8_to_oem(comment);
    char* c_comment_oem = const_cast<char*>(oem_string.c_str());
    char* c_name = const_cast<char*>(name.c_str());
    int pos = K_add(K_WS[I_COMMENTS], c_name, c_comment_oem);
    if (pos == -1) throw std::runtime_error("Iode has not been initialized");
    if (pos < -1) throw std::runtime_error("Cannot create or update " + vIodeTypes[type] + " with name " + name);
    return pos;
}

Comment KDBComments::copy_obj(const Comment& original) const
{
    return Comment(original);
}

Comment KDBComments::get_unchecked(const int pos) const
{
    std::string oem_string = std::string(KCVAL(get_KDB(), pos));
    return oem_to_utf8(oem_string);
}