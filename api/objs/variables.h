#pragma once

#include "api/constants.h"
#include "api/time/period.h"
#include "api/time/sample.h"
#include "api/objs/kdb.h"
#include "api/objs/pack.h"

#include <string>

/*----------------------- TYPEDEF ----------------------------*/

// VAR = Variable (pointer to a vector of double)
using VAR = double*;

/*----------------------- DEFINE ----------------------------*/

#define LTOH_LIN    'L'
#define LTOH_CS     'C'
#define LTOH_STEP   'S'

/*----------------------- ENUMS ----------------------------*/

enum IodeVarMode
{
    VAR_MODE_LEVEL,              //< no modification             x[t]
    VAR_MODE_DIFF,               //< difference on one period    (x[t]-x[t-1])
    VAR_MODE_GROWTH_RATE,        //< growth rate on one period   (x[t]/x[t-1] - 1)*100
    VAR_MODE_Y0Y_DIFF,           //< difference on one year      (x[t]-x[t-{nb sub periods}])
    VAR_MODE_Y0Y_GROWTH_RATE     //< growth rate on one year     (x[t]/x[t-{nb sub periods}] - 1) * 100
};

const static int IODE_NB_VAR_MODES = 5;

enum VariablesInitialization
{
    VAR_INIT_TM1,
    VAR_INIT_TM1_A,
    VAR_INIT_EXTRA,
    VAR_INIT_EXTRA_A,
    VAR_INIT_ASIS,
    VAR_INIT_TM1_NA,
    VAR_INIT_EXTRA_NA
};

enum IodeHighToLow
{
    HTOL_LAST,
    HTOL_MEAN,
    HTOL_SUM 
};

enum IodeLowToHigh
{
    LTOH_STOCK,
    LTOH_FLOW
};

/*----------------------- STRUCTS ----------------------------*/

struct CKDBVariables : public CKDBTemplate<double>
{
    /**
     *  Parameters specific to csv output files. 
     *  These parameters can be modified via report commands: 
     *    - $csvdigits
     *    - $csvsep
     *    - $csvdec
     *    - $csvnan
     *    - $csvaxes
     *  
     */
    static char* CSV_SEP;
    static char* CSV_DEC;
    static char* CSV_NAN;
    static char* CSV_AXES;
    static int   CSV_NBDEC;

private:
    bool load_asc_type_ask(const std::string& file_or_string, int type, int ask);

public:
    // global or standalone database
    CKDBVariables(const bool is_global) : CKDBTemplate(VARIABLES, is_global) {}

    // shallow copy database
    CKDBVariables(CKDBVariables* db_parent, const std::string& pattern = "*") 
        : CKDBTemplate(db_parent, pattern) {}

    // copy constructor
    CKDBVariables(const CKDBVariables& other): CKDBTemplate(other) {}

    // NOTE: get_obj() and set_obj() methods to be replaced by operator[] when 
    //       k_objs will be changed to std::map<std::string, T>
    //       T& operator[](const std::string& name)

    double* get_obj(const SWHDL handle) const override;
    double* get_obj(const std::string& name) const override;

    bool set_obj(const std::string& name, const double* value) override;
    bool set_obj(const std::string& name, const std::vector<double>& value);

    bool load_asc(const std::string& filename) override;
    bool save_asc(const std::string& filename) override;
    bool save_csv(const std::string& filename, const std::vector<std::string>& 
        varlist=std::vector<std::string>(), Sample* sample=nullptr) override;

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
inline std::unique_ptr<CKDBVariables> global_ws_var = std::make_unique<CKDBVariables>(true);

/*----------------------- FUNCS ----------------------------*/

double *K_vval(KDB *, int, int);
double *K_vptr(KDB *, char*, int); 


inline double* KVVAL(CKDBVariables* kdb, const std::string& name)
{
    return kdb->get_obj(name);
}

inline double* KVVAL(CKDBVariables* kdb, const std::string& name, const int t)
{
    double* var = kdb->get_obj(name);
    return var + t;
}


/* k_wsvar.c */
int KV_sample(CKDBVariables*, Sample*);
int KV_merge(CKDBVariables*, CKDBVariables*, int);
void KV_merge_del(CKDBVariables*, CKDBVariables*, int);
int KV_add(CKDBVariables* kdb, char* varname);
double KV_get(CKDBVariables*, const std::string& ,int ,int );
void KV_set(CKDBVariables*, const std::string& ,int ,int ,double );
int KV_extrapolate(CKDBVariables*, int, Sample*, char*);
CKDBVariables* KV_aggregate(CKDBVariables*,int ,char *,char *);
void KV_init_values_1(double* val, int t, int method);
//int KV_GetSmpl(Sample *,char *);
double KV_get_at_t(char*varname, int t);
double KV_get_at_per(char*varname, Period* per);
double KV_get_at_aper(char*varname, char* aper);
int KV_set_at_t(char*varname, int t, double val);
int KV_set_at_per(char*varname, Period* per, double val);
int KV_set_at_aper(char*varname, char* aper, double val);
        