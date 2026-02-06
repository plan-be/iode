#include "api/objs/kdb.h"
#include "api/objs/comments.h"


Comment KDBComments::get(const std::string& name) const
{
    std::string oem_comment = *(this->get_obj(name));
    return oem_to_utf8(oem_comment);
}

bool KDBComments::add(const std::string& name, const Comment& comment)
{
    if(this->contains(name))
    {
        std::string msg = "Cannot add comment: a comment named '" + name + 
                          "' already exists in the database.";
        throw std::invalid_argument(msg);
    }

    if(this->parent_contains(name))
    {
        std::string msg = "Cannot add comment: a comment named '" + name + 
                          "' exists in the parent database of the present subset";
        throw std::invalid_argument(msg);
    }

    std::string oem_comment = utf8_to_oem(comment);
    return this->set_obj(name, oem_comment);
}

void KDBComments::update(const std::string& name, const Comment& comment)
{
    if(!this->contains(name))
    {
        std::string msg = "Cannot update comment: no comment named '" + name + 
                          "' exists in the database.";
        throw std::invalid_argument(msg);
    }
    
    std::string oem_comment = utf8_to_oem(comment);
    this->set_obj(name, oem_comment);
}

bool KDBComments::unpack_obj(const std::string& name, const char* packed_obj)
{
    std::string* comment = new std::string(packed_obj);
    if(comment->empty())
    {
        delete comment;
        return false;
    }

    this->k_objs[name] = comment;
    return true;
}

char* KDBComments::pack_obj(const std::string& name)
{
    std::string* comment = this->k_objs[name];
    return comment->c_str();
}

bool KDBComments::grep_obj(const std::string& name, const std::string& pattern, 
    const bool ecase, const bool forms, const bool texts, const char all) const
{
    bool found = false;
    if(texts) 
        found = wrap_grep_gnl(pattern, this->get_obj(handle), ecase, all);
    return found;
}

char* KDBComments::dde_create_obj_by_name(const std::string& name, int* nc, int* nl)
{
    char* obj = this->get_obj(name);
    return obj;
}

bool KDBComments::print_obj_def(const std::string& name)
{
    bool success = print_definition_generic(name, this->get_obj(name));
    return success;
}

void KDBComments::update_reference_db()
{
    if(global_ref_cmt[0]) 
        delete global_ref_cmt[0];
    global_ref_cmt[0] = new KDBComments(this, "*", false);      
}
