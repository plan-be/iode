#pragma once
#include "kdb_comments.h"


Comment KDBComments::get_unchecked(const int pos) const
{
    Comment comment = std::string(KCVAL(get_database(), pos));
    return oem_to_utf8(comment);
}

int KDBComments::add(const std::string& name, const Comment& comment)
{
    std::string oem_comment = utf8_to_oem(comment);
    return KDBTemplate::add(name, to_char_array(oem_comment));
}

void KDBComments::update(const std::string& name, const Comment& comment)
{
    std::string oem_comment = utf8_to_oem(comment);
    KDBTemplate::update(name, to_char_array(oem_comment));
}

void KDBComments::update(const int pos, const Comment& comment)
{
    std::string oem_comment = utf8_to_oem(comment);
    KDBTemplate::update(pos, to_char_array(oem_comment));
}
