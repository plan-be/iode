#include "api/objs/scalars.h"
#include "api/print/print.h"

bool Scalar::print_definition() const
{    
    char tcoef[128], trelax[128], tstd[128], tttest[128];
    T_fmt_val(tcoef,  (double) value, 15, K_NBDEC);
    T_fmt_val(trelax, (double) relax, 15, -1);
    T_fmt_val(tstd,   (double) std,   15, K_NBDEC);
    
    double ttest = calculate_t_test();
    T_fmt_val(tttest, ttest, 15, K_NBDEC);
    W_printfReplEsc((char*) "%s ~i(%s, %s, %s)\n",  tcoef, trelax, tstd, tttest);

    return true;        
}

std::size_t hash_value(const Scalar& scalar)
{
    std::hash<Scalar> scl_hash;
    return scl_hash(scalar);
}


Scalar* CKDBScalars::get_obj(const SWHDL handle) const
{    
    char* ptr = SW_getptr(handle);
    if(ptr == nullptr)  
        return nullptr;
    return (Scalar*) P_get_ptr(ptr, 0);
}

Scalar* CKDBScalars::get_obj(const std::string& name) const
{
    SWHDL handle = this->get_handle(name);
    if(handle == 0)  
        throw std::invalid_argument("Scalar with name '" + name + "' not found.");
    
    return get_obj(handle);
}

bool CKDBScalars::set_obj(const std::string& name, const Scalar* value)
{
    char* pack = NULL;
    K_spack(&pack, (char*) value);
    bool success = set_packed_object(name, pack);
    if(!success)
    {
        std::string error_msg = "Failed to set scalar object '" + name + "'";
        kwarning(error_msg.c_str());
    }
    return success;
}

bool CKDBScalars::grep_obj(const std::string& name, const SWHDL handle, 
    const std::string& pattern, const bool ecase, const bool forms, const bool texts, 
    const char all) const
{
    return false;
}

char* CKDBScalars::dde_create_obj_by_name(const std::string& name, int* nc, int* nl)
{
    char* obj = (char*) SCR_stracpy((unsigned char*) "Not yet implemented") ;
    return obj;
}

bool CKDBScalars::print_obj_def(const std::string& name)
{
    W_printfReplEsc((char*) ".par1 enum_%d\n~b%s~B : ", 1, name.c_str());

    Scalar* scl = KSVAL(this, name);
    if(!scl) 
    {
        W_printf((char*) "?\n");
        return false;
    }

    bool success = scl->print_definition();
    return success;
}

void CKDBScalars::update_reference_db()
{
    K_RWS[this->k_type][0] = new CKDBScalars(this, "*");      
}

/**
 *  Retrieves a Scalar info.
 *  
 *  @param [in] KDB*    kdb      KDB containing Scalar name
 *  @param [in] char*   name     name of the scalar 
 *  @param [in] int     info_nb  0 = scalar, 1 = relax , 2 = std, 3: t-test
 *  @return     double           value or IODE_NAN if scalar name not found or t-test undefined
 *  
 */
double K_s_get_info(CKDBScalars* kdb, char* c_name, int info_nb)
{
    
    std::string name = std::string(c_name);
    if(!kdb->contains(name)) 
        return IODE_NAN;
    
    double val = IODE_NAN;
    Scalar* scl = KSVAL(kdb, name);
    switch(info_nb) 
    {
        case 1 :  val = scl->relax; break;
        case 2 :  val = scl->std; break;
        case 3 :  
                if(IODE_IS_A_NUMBER(scl->value) && IODE_IS_A_NUMBER(scl->std) && !IODE_IS_0(scl->std))
                    val = scl->value / scl->std;
                else 
                    val = IODE_NAN;
                break;    
            
        default: val = scl->value; break;
    }    
    return(val);
}

double K_s_get_value (CKDBScalars* kdb, char*name) {return(K_s_get_info(kdb, name, 0));}
double K_s_get_relax (CKDBScalars* kdb, char*name) {return(K_s_get_info(kdb, name, 1));}
double K_s_get_stderr(CKDBScalars* kdb, char*name) {return(K_s_get_info(kdb, name, 2));}
double K_s_get_ttest (CKDBScalars* kdb, char*name) {return(K_s_get_info(kdb, name, 3));}


/**
 *  Sets a Scalar info.
 *  
 *  @param [in] KDB*    kdb      KDB containing Scalar name
 *  @param [in] char*   name     name of the scalar 
 *  @param [in] int     info_nb  0 = scalar, 1 = relax , 2 = std
 *  @param [in] double  value    value to set to info_nb
 *  @return     int              -1 if scalar not found, -2 if info_nb illegal, 0 otherwise
 */
int K_s_set_info(CKDBScalars* kdb, char* c_name, int info_nb, double value)
{   
    std::string name = std::string(c_name);
    if(!kdb->contains(name)) 
        return(-1);
    
    Scalar* scl = KSVAL(kdb, name);
    switch(info_nb) 
    {
        case 0 :  scl->value = value; break;
        case 1 :  scl->relax = value; break;
        case 2 :  scl->std = value; break;
        default:  return(-2);
    }    
    return(0);
}

int K_s_set_value (CKDBScalars* kdb, char*name, double value) {return(K_s_set_info(kdb, name, 0, value));}
int K_s_set_relax (CKDBScalars* kdb, char*name, double value) {return(K_s_set_info(kdb, name, 1, value));}
int K_s_set_stderr(CKDBScalars* kdb, char*name, double value) {return(K_s_set_info(kdb, name, 2, value));}
