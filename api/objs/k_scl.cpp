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

bool Scalar::to_binary(char** pack) const
{
    *pack = (char*) P_create();
    *pack = (char*) P_add(*pack, (char*) this, sizeof(Scalar));
    return true;
}

std::size_t hash_value(const Scalar& scalar)
{
    std::hash<Scalar> scl_hash;
    return scl_hash(scalar);
}

bool KDBScalars::binary_to_obj(const std::string& name, char* pack)
{
    Scalar* scl_ptr = (Scalar*) P_get_ptr(pack, 0);
    if(!scl_ptr) 
        return false;

    Scalar* scl = new Scalar(*scl_ptr);
    this->k_objs[name] = scl;
    return true;
}

/**
 * Serializes a Scalar object. 
 *
 * @param [out] pack    (char **)   placeholder for the pointer to the serialized object
 * @param [in]  name    string      scalar name
 * @return                          true if the serialization succeeded, false otherwise 
 */
bool KDBScalars::obj_to_binary(char** pack, const std::string& name)
{
    Scalar* scl = this->get_obj_ptr(name);
    if(!scl) 
        return false;

    bool success = scl->to_binary(pack);
    return success;
}

bool KDBScalars::grep_obj(const std::string& name, const std::string& pattern, 
    const bool ecase, const bool forms, const bool texts, const char all) const
{
    return false;
}

char* KDBScalars::dde_create_obj_by_name(const std::string& name, int* nc, int* nl)
{
    char* obj = (char*) SCR_stracpy((unsigned char*) "Not yet implemented") ;
    return obj;
}

bool KDBScalars::print_obj_def(const std::string& name)
{
    W_printfReplEsc((char*) ".par1 enum_%d\n~b%s~B : ", 1, name.c_str());

    Scalar* scl = this->get_obj_ptr(name);
    if(!scl) 
    {
        W_printf((char*) "?\n");
        return false;
    }

    bool success = scl->print_definition();
    return success;
}

void KDBScalars::update_reference_db()
{
    if(global_ref_scl[0]) 
        delete global_ref_scl[0];
    global_ref_scl[0] = new KDBScalars(this, "*", false);      
}
