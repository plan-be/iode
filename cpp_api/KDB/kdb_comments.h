#pragma once
#include "kdb_template.h"


class KDBComments : public KDBTemplate<CKDBComments, Comment>
{
protected:

    Comment copy_obj(const Comment original) const override { return original; }

    Comment get_unchecked(const std::string& name) const override;
    
public:
    // global or standalone database
    KDBComments(const bool is_global, const std::string& filepath="") 
        : KDBTemplate(COMMENTS, is_global, filepath) {}
    
    // shallow copy database
    KDBComments(KDBComments* kdb, const std::string& pattern) : KDBTemplate(kdb, pattern) {}

    KDBComments* subset(const std::string& pattern, const bool deep_copy=false)
    {
        return template_subset<CKDBComments, KDBComments>(this, pattern, deep_copy);
    }
    
    bool add(const std::string& name, const Comment& comment);

    void update(const std::string& name, const Comment& comment);
};


inline std::size_t hash_value(KDBComments const& cpp_kdb)
{
    KDB* kdb = cpp_kdb.get_database();
    if(!kdb) return 0;

    char* cmt;
    std::size_t seed = 0;
    for(const auto& [name, handle] : kdb->k_objs)
    {
        hash_combine<std::string>(seed, name); 
        // need to wrap with std::string() because hash_value() and
        // hash_combine() only compare pointer addresses when applied 
        // on char* arrays
        cmt = (char*) P_get_ptr(SW_getptr(handle), 0);
        hash_combine<std::string>(seed, std::string(cmt));
    }
    return seed;
}

inline KDBComments Comments(true);
