#pragma once
#include "kdb_template.h"


class KDBComments : public KDBTemplate<Comment>
{
protected:

    Comment copy_obj(const Comment& original) const override;

    Comment get_unchecked(const int pos) const override;

public:
    KDBComments(const std::string& pattern = "", const bool shallow_copy=true) : 
        KDBTemplate(I_COMMENTS, pattern, shallow_copy) {};

    int add(const std::string& name, const Comment& comment);

    void update(const std::string& name, const Comment& comment);

    void update(const int pos, const Comment& comment);
};
