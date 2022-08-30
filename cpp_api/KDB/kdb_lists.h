#pragma once
#include "kdb_template.h"


class KDBLists : public KDBTemplate<List>
{
protected:

    List copy_obj(const List& original) const override;

    List get_unchecked(const int pos) const override;

public:
    KDBLists(const std::string& pattern = "", const bool shallow_copy=true) : 
        KDBTemplate(I_LISTS, pattern, shallow_copy) {};

    int add(const std::string& name, const List& list);

    void update(const std::string& name, const List& list);

    void update(const int pos, const List& list);
};
