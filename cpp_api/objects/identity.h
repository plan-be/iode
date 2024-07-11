#pragma once
#include "common.h"
#include "utils/utils.h"
#include "lec/lec.h"


struct Identity: public IDT
{
private:
    void copy_from_IDT_obj(const IDT* obj);

public:
    Identity(const std::string& lec);

    Identity(const IDT* c_other);
    Identity(const Identity& other);

    ~Identity();

    // required to be used in std::map
    Identity& operator=(const Identity& other);

    bool operator==(const Identity& other) const;

    std::string get_lec() const;

    void set_lec(const std::string& lec);

    CLEC* get_clec() const;

    std::vector<std::string> get_coefficients_list(const bool create_if_not_exit=true);

    std::vector<std::string> get_variables_list(const bool create_if_not_exit=true);
};
