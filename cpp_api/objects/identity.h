#pragma once
#include "common.h"
#include "utils/utils.h"


struct Identity
{
    IDT* c_identity;

public:
    Identity();

    Identity(const int pos, KDB* kdb = nullptr);

    Identity(const std::string& name, KDB* kdb = nullptr);

    Identity(const std::string& lec);

    Identity(const Identity& idt);

    ~Identity();

    // required to be used in std::map
    Identity& operator=(const Identity& idt);

    bool operator==(const Identity& other) const;

    std::string get_lec() const { return std::string(c_identity->lec); }

    void set_lec(const std::string& lec) 
    {
        c_identity->lec = copy_string_to_char(lec);
    }

    CLEC* get_clec() const { return c_identity->clec; }
};
