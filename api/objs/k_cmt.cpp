#include "api/objs/kdb.h"
#include "api/objs/comments.h"


CMT CKDBComments::get_obj(const SWHDL handle) const
{    
    return (CMT) P_get_ptr(SW_getptr(handle), 0);
}

CMT CKDBComments::get_obj(const std::string& name) const
{
    SWHDL handle = this->get_handle(name);
    if(handle == 0)  
        throw std::invalid_argument("comment with name '" + name + "' not found.");
    
    return get_obj(handle);
}

void CKDBComments::set_obj(const std::string& name, const CMT& value)
{
    KDB::set(name, value);
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
    return KCVAL(this, name);
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
