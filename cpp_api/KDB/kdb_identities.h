#pragma once
#include "kdb_template.h"


class KDBIdentities : public KDBTemplate<Identity>
{
private:
    int add_or_update(const std::string& name, const std::string& lec);

protected:

    Identity copy_obj(const Identity& original) const override;

    Identity get_unchecked(const int pos) const override;

public:
    KDBIdentities(const std::string& pattern = "") : KDBTemplate(I_IDENTITIES, pattern) {};

    std::string get_lec(const int pos) const;

    std::string get_lec(const std::string& name) const;

    int add(const std::string& name, const std::string& lec);

    void update(const std::string& name, const std::string& lec);

    void update(const int pos, const std::string& lec);

    void execute_identities(const Period& from, const Period& to, const std::string& identities_list, 
                            const std::string& var_files = "", const std::string& scalar_files = "", const bool trace = false);

    void execute_identities(const std::string& from, const std::string& to, const std::string& identities_list, 
                            const std::string& var_files = "", const std::string& scalar_files = "", const bool trace = false);
};
