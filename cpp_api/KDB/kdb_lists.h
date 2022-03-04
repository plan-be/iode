#pragma once

#include "kdb_abstract.h"
#include "kdb_abstract.cpp"


class KDBLists : public KDBAbstract<List>
{
protected:

    // CRUD (Create - Read - Update - Delete) + Copy methods

    void add_or_update(const std::string& name, const List& list) override;

    List copy_obj(const List& original) const override;

    List get_unchecked(const int pos) const override;

public:
    KDBLists() : KDBAbstract(I_LISTS) {};
};
