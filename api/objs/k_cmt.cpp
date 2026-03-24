#include "api/objs/kdb.h"
#include "api/objs/comments.h"


bool KDBComments::binary_to_obj(const std::string& name, char* pack)
{
    size_t len = (size_t) P_get_len(pack, 0);
    char* value = new char[len];
    strncpy(value, (char*) P_get_ptr(pack, 0), len);
    std::string cmt_oem(value);
    std::string cmt_utf8 = oem_to_utf8(cmt_oem);
    this->k_objs[name] = std::make_shared<Comment>(cmt_utf8);
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
    std::shared_ptr<Comment> cmt_ptr = this->get_obj_ptr(name);
    std::string cmt_utf8 = *cmt_ptr;
    std::string cmt_oem = utf8_to_oem(cmt_utf8);
    char* c_cmt = (char*) cmt_oem.c_str();

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
        std::shared_ptr<Comment> comment = this->get_obj_ptr(name);
        found = wrap_grep_gnl(pattern, *comment, ecase, all);
    }
    return found;
}

char* KDBComments::dde_create_obj_by_name(const std::string& name, int* nc, int* nl)
{
    std::shared_ptr<Comment> comment = this->get_obj_ptr(name);
    char* c_cmt = (char*) comment->c_str();
    return c_cmt;
}

bool KDBComments::print_obj_def(const std::string& name)
{
    std::shared_ptr<Comment> comment_ptr = this->get_obj_ptr(name);
    // W_Print(...) functions expect OEM encoding, so convert value from UTF-8 to OEM before printing 
    std::string comment_oem = utf8_to_oem(*comment_ptr);
    char* c_cmt = (char*) comment_oem.c_str();
    bool success = print_definition_generic(name, c_cmt);
    return success;
}

void KDBComments::update_reference_db()
{
    if(global_ref_cmt[0]) 
        delete global_ref_cmt[0];
    global_ref_cmt[0] = new KDBComments(this, "*", false);      
}
