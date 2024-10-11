#pragma once
#include "kdb_template.h"
#include "cpp_api/objects/scalar.h"


class KDBScalars : public KDBTemplate<Scalar*>
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
    Scalar* copy_obj(Scalar* const original) const override;

    // WARNING: the returned scalar MUST NOT be deleted
    Scalar* get_unchecked(const int pos) const override;

    KDBScalars(KDBScalars* kdb, const bool deep_copy, const std::string& pattern) : 
        KDBTemplate(kdb, deep_copy, pattern) {};

public:
    KDBScalars(const std::string& filepath="") : KDBTemplate(SCALARS, filepath) {}

    KDBScalars* subset(const std::string& pattern, const bool deep_copy=false)
    {
        return new KDBScalars(this, deep_copy, pattern);
    }

    int add(const std::string& name, const Scalar& obj);

    int add(const std::string& name, const double value, const double relax, const double std=IODE_NAN);

    void update(const std::string& name, const Scalar& obj);

    void update(const std::string& name, const double value, const double relax, const double std = IODE_NAN);

    void update(const int pos, const double value, const double relax, const double std = IODE_NAN);
};


inline std::size_t hash_value(KDBScalars const& cpp_kdb)
{
    KDB* kdb = cpp_kdb.get_database();
    if(kdb == NULL) return 0;

    std::size_t seed = 0;
    for(int pos=0; pos < kdb->k_nb; pos++)
    {
        char* o_name = kdb->k_objs[pos].o_name;
        hash_combine<std::string_view>(seed, std::string_view(o_name, strlen(o_name)));
        hash_combine<SCL>(seed, *KSVAL(kdb, pos));
    }
    return seed;
}

inline KDBScalars Scalars;
