#include "api/objs/kdb.h"
#include "api/objs/comments.h"


Comment KDBComments::get(const std::string& name) const
{
    std::string oem_comment = KDBTemplate::get(name);
    return oem_to_utf8(oem_comment);
}

bool KDBComments::add(const std::string& name, const Comment& comment)
{
    std::string oem_comment = utf8_to_oem(comment);
    KDBTemplate::add(name, oem_comment);
    return true;
}

void KDBComments::update(const std::string& name, const Comment& comment)
{
    Comment oem_comment = utf8_to_oem(comment);
    KDBTemplate::update(name, oem_comment);
}

bool KDBComments::binary_to_obj(const std::string& name, char* pack)
{
    size_t len = (size_t) P_get_len(pack, 0);
    char* value = new char[len];
    strncpy(value, (char*) P_get_ptr(pack, 0), len);

    Comment* comment = new Comment(value);
    this->k_objs[name] = comment;
    return true;
}

/**
 * Serializes a comment object. 
 *
 * @param [out] pack    (char **)   placeholder for the pointer to the serialized object
 * @param [in]  a1      string      comment name
 * @return                          true if the serialization succeeded, false otherwise
 */
bool KDBComments::obj_to_binary(char** pack, const std::string& name)
{
    Comment* comment = this->get_obj_ptr(name);
    char* c_cmt = (char*) comment->c_str();

    *pack = (char*) P_create();
    *pack = (char*) P_add(*pack, c_cmt, (int) strlen(c_cmt) + 1);

    return true;
}

bool KDBComments::grep_obj(const std::string& name, const std::string& pattern, 
    const bool ecase, const bool forms, const bool texts, const char all) const
{
    bool found = false;
    if(texts)
    {
        Comment* comment = this->get_obj_ptr(name);
        found = wrap_grep_gnl(pattern, *comment, ecase, all);
    }
    return found;
}

char* KDBComments::dde_create_obj_by_name(const std::string& name, int* nc, int* nl)
{
    Comment* comment = this->get_obj_ptr(name);
    char* c_cmt = (char*) comment->c_str();
    return c_cmt;
}

bool KDBComments::print_obj_def(const std::string& name)
{
    Comment* comment = this->get_obj_ptr(name);
    char* c_cmt = (char*) comment->c_str();
    bool success = print_definition_generic(name, c_cmt);
    return success;
}

void KDBComments::update_reference_db()
{
    if(global_ref_cmt[0]) 
        delete global_ref_cmt[0];
    global_ref_cmt[0] = new KDBComments(this, "*", false);      
}
