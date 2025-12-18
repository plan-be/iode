#include "api/objs/kdb.h"
#include "api/objs/comments.h"


char* CKDBComments::get_obj(const SWHDL handle) const
{    
    return (char*) P_get_ptr(SW_getptr(handle), 0);
}

char* CKDBComments::get_obj(const std::string& name) const
{
    SWHDL handle = this->get_handle(name);
    if(handle == 0)  
        throw std::invalid_argument("comment with name '" + name + "' not found.");
    
    return get_obj(handle);
}

bool CKDBComments::set_obj(const std::string& name, const char* value)
{
    char* pack = NULL;
    K_cpack(&pack, (char*) value);
    bool success = set_packed_object(name, pack);
    if(!success)
    {
        std::string error_msg = "Failed to set comment object '" + name + "'";
        kwarning(error_msg.c_str());
    }
    return success;
}

bool CKDBComments::set_obj(const std::string& name, const std::string& value)
{
    return set_obj(name, value.c_str());
}

bool CKDBComments::grep_obj(const std::string& name, const SWHDL handle, 
    const std::string& pattern, const bool ecase, const bool forms, const bool texts, 
    const char all) const
{
    bool found = false;
    if(texts) 
        found = wrap_grep_gnl(pattern, KCVAL(this, handle), ecase, all);
    return found;
}

char* CKDBComments::dde_create_obj_by_name(const std::string& name, int* nc, int* nl)
{
    char* obj = KCVAL(this, name);
    return obj;
}

bool CKDBComments::print_obj_def(const std::string& name)
{
    bool success = print_definition_generic(name, KCVAL(this, name));
    return success;
}

void CKDBComments::update_reference_db()
{
    K_RWS[this->k_type][0] = new CKDBComments(this, "*");      
}
