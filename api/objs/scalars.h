#pragma once

#include "api/constants.h"
#include "api/utils/utils.h"
#include "api/objs/kdb.h"
#include "api/objs/pack.h"


/*----------------------- STRUCTS ----------------------------*/

// Scalar (struct containing a scalar value, and if estimated, 
//         its relaxation parameter and its standard error).
struct Scalar 
{
    double    value;        // Value of the scalar
    double    relax;        // Relaxation parameter. Used only for estimation.
			                // If null, the parameter will not be estimated.
			                // If 0 < relax < 1, the convergence of the estimation will be slowed down. To use when the estimation process diverges.
    double    std;          // Standard error. Result of the estimation.

public:
    Scalar(const double value=0.0, const double relax=1.0, const double std=IODE_NAN)
    {
        this->value = value;
        this->relax = relax;
        this->std = std;
    }

    Scalar(const Scalar& scalar)
    {
        this->value = scalar.value;
        this->relax = scalar.relax;
        this->std = scalar.std;
    }

    // required to be used in std::map
    Scalar& operator=(const Scalar& scalar)
    {
        this->value = scalar.value;
        this->relax = scalar.relax;
        this->std = scalar.std;
        return *this;
    }

    // Return the t-test of a scalar or IODE_NAN if it cannot be determined
    double calculate_t_test() const
    {
        if(IODE_IS_A_NUMBER(value) && IODE_IS_A_NUMBER(std) && !IODE_IS_0(std))
            return value / std;
        return IODE_NAN;
    }

    bool print_definition() const;

    bool operator==(const Scalar& other) const
    {
        bool eq = this->value == other.value;
        eq &= this->relax == other.relax;
        eq &= this->std == other.std;
        return eq;
    }
};


// Custom specialization of std::hash can be injected in namespace std.
template<>
struct std::hash<Scalar>
{
    std::size_t operator()(const Scalar& scalar) const noexcept
    {
		std::size_t seed = 0;

        hash_combine<double>(seed, scalar.value);
        hash_combine<double>(seed, scalar.relax);
        hash_combine<double>(seed, scalar.std);

        return seed;
    }
};

/*----------------------- STRUCTS ----------------------------*/

struct CKDBScalars : public CKDBTemplate<Scalar>
{
    // global or standalone database
    CKDBScalars(const bool is_global) : CKDBTemplate(SCALARS, is_global) {}

    // shallow copy database
    CKDBScalars(CKDBScalars* db_parent, const std::string& pattern = "*") 
        : CKDBTemplate(db_parent, pattern) {}

    // copy constructor
    CKDBScalars(const CKDBScalars& other): CKDBTemplate(other) {}

    // NOTE: get_obj() and set_obj() methods to be replaced by operator[] when 
    //       k_objs will be changed to std::map<std::string, T>
    //       T& operator[](const std::string& name)

    Scalar* get_obj(const SWHDL handle) const override;
    Scalar* get_obj(const std::string& name) const override;

    bool set_obj(const std::string& name, const Scalar* value) override;

    bool load_asc(const std::string& filename) override;
    bool save_asc(const std::string& filename) override;

    char* dde_create_obj_by_name(const std::string& name, int* nc, int* nl) override;

    bool print_obj_def(const std::string& name) override;

private:
    bool grep_obj(const std::string& name, const SWHDL handle, 
        const std::string& pattern, const bool ecase, const bool forms, 
        const bool texts, const char all) const override;
    
    void update_reference_db() override;
};

/*----------------------- GLOBALS ----------------------------*/
// unique_ptr -> automatic memory management
//            -> no need to delete KDB workspaces manually
inline std::unique_ptr<CKDBScalars> global_ws_scl = std::make_unique<CKDBScalars>(true);

/*----------------------- FUNCTIONS ----------------------------*/

std::size_t hash_value(const Scalar& scalar);
