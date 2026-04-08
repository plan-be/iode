#pragma once

#include "api/constants.h"
#include "api/objs/kdb.h"
#include "api/objs/pack.h"

/*----------------------- TYPEDEF ----------------------------*/

// using is the C++11 version of typedef
using Comment = std::string;

/*----------------------- STRUCTS ----------------------------*/

struct KDBComments : public KDBTemplate<KDBComments, Comment>
{
private:
    // Constructors are private - use Create() factory method instead
    // global or standalone database
    KDBComments(const bool is_global) : KDBTemplate(COMMENTS, is_global) {}

    // copy constructor
    KDBComments(const KDBComments& other): KDBTemplate(other) {}

public:
    /**
     * Factory method to create a managed instance with std::shared_ptr
     * 
     * Usage: auto db = KDB[...]::Create(is_global);
     * 
     * This ensures the instance is managed by shared_ptr, preventing bad_weak_ptr errors
     * when using get_subset() or other methods that rely on std::enable_shared_from_this.
     * 
     * Note: KDB[...] classes should also use KDB[...]::Create(...) for creation.
     * 
     * @param is_global Whether to create a global database (true) or standalone (false)
     * @return          std::shared_ptr<KDB[...]> pointing to the newly created instance
     */
    static std::shared_ptr<KDBComments> Create(const bool is_global)
    {
        return std::shared_ptr<KDBComments>(new KDBComments(is_global));
    }

    bool load_asc(const std::string& filename) override;
    bool save_asc(const std::string& filename) override;

    char* dde_create_obj_by_name(const std::string& name, int* nc, int* nl) override;

    bool print_obj_def(const std::string& name) override;

private:
    bool binary_to_obj(const std::string& name, char* pack) override;
    bool obj_to_binary(char** pack, const std::string& name) override;

    char* sub_convert_obj_version(char* npack, const std::string& name, const int vers) override;

    bool grep_obj(const std::string& name, const std::string& pattern, 
        const bool ecase, const bool forms, const bool texts, const char all) const override;
    
    void update_reference_db() override;
};

/*----------------------- GLOBALS ----------------------------*/

using KDBCommentsPtr = std::shared_ptr<KDBComments>;

// shared_ptr -> automatic memory management
//            -> no need to delete KDB workspaces manually
inline KDBCommentsPtr global_ws_cmt = KDBComments::Create(true);
inline std::array<KDBCommentsPtr, 5> global_ref_cmt = { nullptr };

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
