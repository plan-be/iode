#pragma once

#include "kdb_abstract.h"


class KDBComments : public KDBAbstract
{

public:
    KDBComments() : KDBAbstract(COMMENTS) {};

    Comment get(const int pos) const 
    {
        std::string oem = std::string(KCVAL(getKDB(), pos));
        Comment comment = convert_oem_to_utf8(oem);
        return comment;
    }

    Comment get(const std::string name) const
    {
        int pos = getPosition(name);
        return get(pos);
    }

    void set(const std::string name, const Comment& comment) const
    {
        // throws an exception if comment not exists
        getPosition(name);
        std::string comment_oem = convert_utf8_to_oem(comment);
        char* c_comment_oem = const_cast<char*>(comment_oem.c_str());
        char* c_name = const_cast<char*>(name.c_str());
        int res = K_add(getKDB(), c_name, c_comment_oem);
        if (res == -1) throw std::runtime_error("Iode has not been initialized");
        if (res < -1) throw std::runtime_error("Something went wrong when trying to set " + type_name + " with name " + name);
    }

    void set(const int pos, const Comment& comment) const
    {
        std::string name = getName(pos);
        set(name, comment);
    }
};
