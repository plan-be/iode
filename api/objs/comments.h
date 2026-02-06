#pragma once

#include "api/constants.h"
#include "api/objs/kdb.h"
#include "api/objs/pack.h"

/*----------------------- TYPEDEF ----------------------------*/

// using is the C++11 version of typedef
using Comment = std::string;

/*----------------------- STRUCTS ----------------------------*/

struct KDBComments : public KDBTemplate<Comment>
{
    // global or standalone database
    KDBComments(const bool is_global) : KDBTemplate(COMMENTS, is_global) {}

    // subset (shallow or deep copy) 
    KDBComments(KDBComments* db_parent, const std::string& pattern, const bool copy) 
        : KDBTemplate(db_parent, pattern, copy) {}

    // copy constructor
    KDBComments(const KDBComments& other): KDBTemplate(other) {}

    Comment get(const std::string& name) const override;
    bool add(const std::string& name, const Comment& comment) override;
    void update(const std::string& name, const Comment& comment) override;

    bool load_asc(const std::string& filename) override;
    bool save_asc(const std::string& filename) override;

    char* dde_create_obj_by_name(const std::string& name, int* nc, int* nl) override;

    bool print_obj_def(const std::string& name) override;

    void merge_from(const std::string& input_file) override
    {
        KDBComments from(false);    
        KDBTemplate::merge_from(from, input_file);
    }

    bool copy_from_file(const std::string& file, const std::string& objs_names, 
        std::set<std::string>& v_found)
    {
        KDBComments from(false);
        return KDBTemplate::copy_from_file(from, file, objs_names, v_found);
    }

private:
    bool binary_to_obj(const std::string& name, char* pack) override;
    bool obj_to_binary(char** pack, const std::string& name) override;

    bool grep_obj(const std::string& name, const std::string& pattern, 
        const bool ecase, const bool forms, const bool texts, const char all) const override;
    
    void update_reference_db() override;
};

/*----------------------- GLOBALS ----------------------------*/
// unique_ptr -> automatic memory management
//            -> no need to delete KDB workspaces manually
inline std::unique_ptr<KDBComments> global_ws_cmt = std::make_unique<KDBComments>(true);
inline std::array<KDBComments*, 5> global_ref_cmt = { nullptr };

/*------------------------- FUNCS ----------------------------*/

inline std::size_t hash_value(KDBComments const& cpp_kdb)
{
    if(cpp_kdb.size() == 0)
        return 0;

    std::size_t seed = 0;
    for(const auto& [name, cmt_ptr] : cpp_kdb.k_objs)
    {
        hash_combine<std::string>(seed, name); 
        hash_combine<std::string>(seed, *cmt_ptr);
    }
    
    return seed;
}
