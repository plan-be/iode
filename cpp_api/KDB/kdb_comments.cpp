#pragma once
#include "kdb_comments.h"


Comment KDBComments::copy_obj(const Comment& original) const
{
    return Comment(original);
}

Comment KDBComments::get_unchecked(const int pos) const
{
    Comment comment = std::string(KCVAL(get_KDB(), pos));
    return oem_to_utf8(comment);
}

int KDBComments::add(const std::string& name, const Comment& comment)
{
    std::string oem_comment = utf8_to_oem(comment);
    return KDBTemplate::add(name, const_cast<char*>(oem_comment.c_str()));
}

void KDBComments::update(const std::string& name, const Comment& comment)
{
    std::string oem_comment = utf8_to_oem(comment);
    KDBTemplate::update(name, const_cast<char*>(oem_comment.c_str()));
}

void KDBComments::update(const int pos, const Comment& comment)
{
    std::string oem_comment = utf8_to_oem(comment);
    KDBTemplate::update(pos, const_cast<char*>(oem_comment.c_str()));
}
