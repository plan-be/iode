#pragma once
#include "api/time/period.h"
#include "api/time/sample.h"
#include "api/objs/equations.h"
#include "kdb_template.h"


class KDBEquations : public KDBTemplate<KDB, Equation*>
{
protected:
    // WARNING: 1) A top-level const is a const qualifier that applies to an object itself:
    //             - const int x;       this const applies to x, so it is top-level
    //             - int* const ptr;    this const applies to ptr, so it is top-level
    //          2) In contrast, a low-level const is a const qualifier that applies to the object being referenced 
    //             or pointed to:
    //             - const int& ref;    this const applies to the object being referenced, so it is low-level
    //             - const int* ptr;    this const applies to the object being pointed to, so it is low-level
    //          -> In the method copy_obj(const T original) of the template class KDBTemplate, the const is 'top-level'.
    //          -> In the present class, the const qualifier must be placed AFTER the * so that it remains a 'top-level' const. 
    // see https://stackoverflow.com/a/78058445 and 
    // https://www.learncpp.com/cpp-tutorial/type-deduction-with-pointers-references-and-const/?utm_content=cmp-true
    Equation* copy_obj(Equation* const original) const override;

    Equation* get_unchecked(const std::string& name) const override;

public:
    // global or standalone database
    KDBEquations(const bool is_global, const std::string& filepath="") 
        : KDBTemplate(EQUATIONS, is_global, filepath) {}

    // shallow copy database
    KDBEquations(KDBEquations* kdb, const std::string& pattern) : KDBTemplate(kdb, pattern) {};
    
    KDBEquations* subset(const std::string& pattern, const bool deep_copy=false)
    {
        return template_subset<KDB, KDBEquations>(this, pattern, deep_copy);
    }

    std::string get_lec(const std::string& name) const;

    bool add(const std::string& name, const Equation& obj);

    bool add(const std::string& name, const std::string& lec, const std::string& method = "LSQ", const std::string& from = "", const std::string& to = "", 
        const std::string& comment = "", const std::string& instruments = "", const std::string& block = "", const bool date = true);

    void update(const std::string& name, const Equation& obj);

    void update(const std::string& name, const std::string& lec, const std::string& method = "LSQ", const std::string& from = "", const std::string& to = "", 
        const std::string& comment = "", const std::string& instruments = "", const std::string& block = "", const bool date = false);
};


inline std::size_t hash_value(KDBEquations const& cpp_kdb)
{
    KDB* kdb = cpp_kdb.get_database();
    if(!kdb) return 0;

    char* ptr;
    Equation* eq;
    std::size_t seed = 0;
    for(const auto& [name, handle] : kdb->k_objs)
    {
        hash_combine<std::string>(seed, name);
         
        ptr = SW_getptr(handle);
        eq = K_eunpack(ptr, (char*) name.c_str());
        hash_combine<Equation>(seed, *eq);
    }
    return seed;
}

inline KDBEquations Equations(true);
