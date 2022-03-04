#pragma once

#include "kdb_abstract.h"
#include "kdb_abstract.cpp"


class KDBIdentities : public KDBAbstract<Identity>
{
private:
    void add_or_update(const std::string& name, const std::string& lec);

protected:

    // CRUD (Create - Read - Update - Delete) + Copy methods

    void add_or_update(const std::string& name, const Identity& identity) override;

    Identity copy_obj(const Identity& original) const override;

    Identity get_unchecked(const int pos) const override;

public:
    KDBIdentities() : KDBAbstract(I_IDENTITIES) {};

    std::string get_lec(const int pos) const;

    std::string get_lec(const std::string& name) const;

    void add(const std::string& name, const std::string& lec);

    void update(const std::string& name, const std::string& lec);
};
