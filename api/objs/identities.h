#pragma once

#include "api/constants.h"
#include "api/objs/objs.h"
#include "api/objs/variables.h"
#include "api/lec/lec.h"

/*----------------------- MACROS ----------------------------*/

#define KIPTR(name)         K_optr0(KI_WS, name)
#define KILEC(kdb, pos)     K_oval0(kdb, pos)
#define KICLEC(kdb, pos)    ((CLEC *) K_oval1(kdb, pos))

/*----------------------- STRUCTS ----------------------------*/

// Identity (struct with the identity LEC formula and the compiled CLEC structure)
struct Identity
{
    std::string lec;      // LEC expression (may not be an equation)
    CLEC*       clec;     // Compiled version of LEC

public:
    Identity(const std::string& lec)
    {
        set_lec(lec);
    }

    Identity(const Identity& other)
    {
        this->lec = other.lec;
        // NOTE : we do not use memcpy() because memcpy() actually makes  
        //        a shallow copy of a struct instead of a deep copy
        this->clec = clec_deep_copy(other.clec);
    }

    ~Identity()
    {
        this->lec.clear();

        if(this->clec)
            SW_nfree(this->clec);
    }

    void set_lec(const std::string& lec) 
    {
        this->lec = lec;
        // L_cc returns an allocated CLEC struct pointer.
        this->clec = L_cc((char*) lec.c_str());
    }

    // required to be used in std::map
    Identity& operator=(const Identity& other)
    {
        this->lec = other.lec;
        // NOTE : we do not use memcpy() because memcpy() actually makes  
        //        a shallow copy of a struct instead of a deep copy
        this->clec = clec_deep_copy(other.clec);
        return *this;
    }

    bool operator==(const Identity& other) const
    {
        CLEC* cl1 = L_cc((char*) this->lec.c_str());
        CLEC* cl2 = L_cc((char*) other.lec.c_str());

        if(cl1 == NULL || cl2 == NULL || cl1->tot_lg != cl2->tot_lg || memcmp(cl1, cl2, cl1->tot_lg) != 0)
            return false;
        return true;
    }

    std::vector<std::string> get_coefficients_list(const bool create_if_not_exit=true);
    std::vector<std::string> get_variables_list(const bool create_if_not_exit=true);
};
