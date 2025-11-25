#pragma once
#include "kdb_comments.h"


Comment KDBComments::get_unchecked(const std::string& name) const
{
    CKDBComments* kdb_cmt = get_database();
    char* value = KCVAL(kdb_cmt, name);
    Comment comment = std::string(value);
    return oem_to_utf8(comment);
}

bool KDBComments::add(const std::string& name, const Comment& comment)
{
    std::string oem_comment = utf8_to_oem(comment);
    return KDBTemplate::add(name, to_char_array(oem_comment));
}

void KDBComments::update(const std::string& name, const Comment& comment)
{
    std::string oem_comment = utf8_to_oem(comment);
    KDBTemplate::update(name, to_char_array(oem_comment));
}
