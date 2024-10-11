#pragma once
#include "cpp_api/time/period.h"
#include "cpp_api/time/sample.h"
#include "cpp_api/objects/equation.h"
#include "kdb_template.h"


class KDBEquations : public KDBTemplate<Equation*>
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

    Equation* get_unchecked(const int pos) const override;

    KDBEquations(KDBEquations* kdb, const bool deep_copy, const std::string& pattern) : 
        KDBTemplate(kdb, deep_copy, pattern) {};

public:
    KDBEquations(const std::string& filepath="") : KDBTemplate(EQUATIONS, filepath) {}

    KDBEquations* subset(const std::string& pattern, const bool deep_copy=false)
    {
        return new KDBEquations(this, deep_copy, pattern);
    }

    std::string get_lec(const int pos) const;

    std::string get_lec(const std::string& name) const;

    int add(const std::string& name, const Equation& obj);

    int add(const std::string& name, const std::string& lec, const std::string& method = "LSQ", const std::string& from = "", const std::string& to = "", 
        const std::string& comment = "", const std::string& instruments = "", const std::string& block = "", const bool date = true);

    void update(const std::string& name, const Equation& obj);

    void update(const std::string& name, const std::string& lec, const std::string& method = "LSQ", const std::string& from = "", const std::string& to = "", 
        const std::string& comment = "", const std::string& instruments = "", const std::string& block = "", const bool date = false);

    void update(const int pos, const std::string& lec, const std::string& method = "LSQ", const std::string& from = "", const std::string& to = "", 
        const std::string& comment = "", const std::string& instruments = "", const std::string& block = "", const bool date = false);
};


inline std::size_t hash_value(KDBEquations const& cpp_kdb)
{
    KDB* kdb = cpp_kdb.get_database();
    if(kdb == NULL) return 0;

    std::size_t seed = 0;
    for(int pos=0; pos < kdb->k_nb; pos++)
    {
        char* o_name = kdb->k_objs[pos].o_name;
        hash_combine<std::string_view>(seed, std::string_view(o_name, strlen(o_name)));
        hash_combine<EQ>(seed, *KEVAL(kdb, pos));
    }
    return seed;
}

inline KDBEquations Equations;
