#pragma once

#include "kdb_abstract.h"
#include "kdb_abstract.cpp"


class KDBComments : public KDBAbstract<Comment>
{
protected:

    // CRUD (Create - Read - Update - Delete) + Copy methods

    int add_or_update(const std::string& name, const Comment& comment) override;

    Comment copy_obj(const Comment& original) const override;

    Comment get_unchecked(const int pos) const override;

public:
    KDBComments(const std::string& pattern = "") : KDBAbstract(I_COMMENTS, pattern) {};
};
