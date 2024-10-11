#pragma once
#include "kdb_template.h"


class KDBComments : public KDBTemplate<Comment>
{
protected:

    Comment copy_obj(const Comment original) const override { return original; }

    Comment get_unchecked(const int pos) const override;

    KDBComments(KDBComments* kdb, const bool deep_copy, const std::string& pattern) : 
        KDBTemplate(kdb, deep_copy, pattern) {}

public:
    KDBComments(const std::string& filepath="") : KDBTemplate(COMMENTS, filepath) {}

    KDBComments* subset(const std::string& pattern, const bool deep_copy=false)
    {
        return new KDBComments(this, deep_copy, pattern);
    }
    
    int add(const std::string& name, const Comment& comment);

    void update(const std::string& name, const Comment& comment);

    void update(const int pos, const Comment& comment);
    
};


inline std::size_t hash_value(KDBComments const& cpp_kdb)
{
    KDB* kdb = cpp_kdb.get_database();
    if(kdb == NULL) return 0;

    std::size_t seed = 0;
    for(int pos=0; pos < kdb->k_nb; pos++)
    {
        char* o_name = kdb->k_objs[pos].o_name;
        hash_combine<std::string_view>(seed, std::string_view(o_name, strlen(o_name)));
        // need to wrap with std::string() because hash_value() and
        // hash_combine() only compare pointer addresses when applied 
        // on char* arrays
        hash_combine<std::string>(seed, std::string(KCVAL(kdb, pos)));
    }
    return seed;
}

inline KDBComments Comments;
