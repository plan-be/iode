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
