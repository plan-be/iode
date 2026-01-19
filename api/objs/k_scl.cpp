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


Scalar* KDBScalars::get_obj(const SWHDL handle) const
{    
    char* ptr = SW_getptr(handle);
    if(ptr == nullptr)  
        return nullptr;
    return (Scalar*) P_get_ptr(ptr, 0);
}

Scalar* KDBScalars::get_obj(const std::string& name) const
{
    SWHDL handle = this->get_handle(name);
    if(handle == 0)  
        throw std::invalid_argument("Scalar with name '" + name + "' not found.");
    
    return get_obj(handle);
}

bool KDBScalars::set_obj(const std::string& name, const Scalar* value)
{
    char* pack = NULL;
    std::string key = to_key(name);
    K_spack(&pack, (char*) value);
    bool success = set_packed_object(key, pack);
    if(!success)
    {
        std::string error_msg = "Failed to set scalar object '" + key + "'";
        kwarning(error_msg.c_str());
    }
    return success;
}

Scalar* KDBScalars::get(const std::string& name) const
{
	Scalar* scalar = this->get_obj(name);
	return scalar;
}

bool KDBScalars::add(const std::string& name, const Scalar& obj)
{
    if(this->contains(name))
    {
        std::string msg = "Cannot add scalar: a scalar named '" + name + 
                          "' already exists in the database.";
        throw std::invalid_argument(msg);
    }

    if(this->parent_contains(name))
    {
        std::string msg = "Cannot add scalar: a scalar named '" + name + 
                          "' exists in the parent database of the present subset";
        throw std::invalid_argument(msg);
    }

	Scalar scalar(obj);
	return this->set_obj(name, &scalar);
}

void KDBScalars::update(const std::string& name, const Scalar& obj)
{
    if(!this->contains(name))
    {
        std::string msg = "Cannot update scalar: no scalar named '" + name + 
                          "' exists in the database.";
        throw std::invalid_argument(msg);
    }

	Scalar scalar(obj);
	this->set_obj(name, &scalar);
}

bool KDBScalars::grep_obj(const std::string& name, const SWHDL handle, 
    const std::string& pattern, const bool ecase, const bool forms, const bool texts, 
    const char all) const
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

    Scalar* scl = this->get_obj(name);
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
    if(K_RWS[this->k_type][0]) 
        delete K_RWS[this->k_type][0];
    K_RWS[this->k_type][0] = new KDBScalars(this, "*", false);      
}
