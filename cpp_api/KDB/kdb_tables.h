#pragma once
#include "api/objs/tables.h"
#include "cpp_api/computed_table/computed_table.h"
#include "kdb_template.h"


class KDBTables : public KDBTemplate<Table*>
{
protected:

    // WARNING: 1) A top-level const is a const qualifier that applies to an object itself:
    //             - const int x;       this const applies to x, so it is top-level
    //             - int* const ptr;    this const applies to ptr, so it is top-level
    //          2) In contrast, a low-level const is a const qualifier that applies to the object being referenced 
    //             or pointed to:
    //             - const int& ref;    this const applies to the object being referenced, so it is low-level
    //             - const int* ptr;    this const applies to the object being pointed to, so it is low-level
    //          -> In the method copy_obj(const T original) of the template class KDBTemplate, the const is 'top-level'.
    //          -> In the present class, the const qualifier must be placed AFTER the * so that it remains a 'top-level' const. 
    // see https://stackoverflow.com/a/78058445 and 
    // https://www.learncpp.com/cpp-tutorial/type-deduction-with-pointers-references-and-const/?utm_content=cmp-true
    Table* copy_obj(Table* const original) const override;

    Table* get_unchecked(const std::string& name) const override;

    KDBTables(KDBTables* kdb, const bool deep_copy, const std::string& pattern) : 
        KDBTemplate(kdb, deep_copy, pattern) {};

public:
    KDBTables(const std::string& filepath="") : KDBTemplate(TABLES, filepath) {}

    KDBTables* subset(const std::string& pattern, const bool deep_copy=false)
    {
        return new KDBTables(this, deep_copy, pattern);
    }

    std::string get_title(const std::string& name) const;

    bool add(const std::string& name, const Table& obj);

    bool add(const std::string& name, const int nb_columns);

    bool add(const std::string& name, const int nbColumns, const std::string& def, const std::vector<std::string>& vars, 
        bool mode = false, bool files = false, bool date = false);

    bool add(const std::string& name, const int nbColumns, const std::string& def, const std::vector<std::string>& titles, 
        const std::vector<std::string>& lecs, bool mode = false, bool files = false, bool date = false);

    bool add(const std::string& name, const int nbColumns, const std::string& def, const std::string& lecs, 
        bool mode = false, bool files = false, bool date = false);

    /**
     * @brief Compute and print a list of tables to a file.
     * Argument `format` must be in the list:
     *   - 'H' (HTML file)
     *   - 'M' (MIF file)
     *   - 'R' (RTF file)
     *   - 'C' (CSV file)
     *   - 'D' (DUMMY file)
     * 
     * If argument `format` is null (default), the A2M format will be used 
     * to print the output.
     * 
     * If the filename does not contain an extension, it is automatically 
     * added based on the format.
     * 
     * @param destination_file 
     * @param names
     * @param format 
     */
    void print_to_file(const std::string& destination_file, const std::string& gsample, 
        const std::string& names, const int nb_decimals, const char format = '\0');
};


inline std::size_t hash_value(KDBTables const& cpp_kdb)
{
    KDB* kdb = cpp_kdb.get_database();
    if(kdb == NULL) return 0;

    char* ptr;
    Table* table;
    std::size_t seed = 0;
    for(const auto& [name, handle] : kdb->k_objs)
    {
        hash_combine<std::string>(seed, name);

        ptr = SW_getptr(handle);
        table = K_tunpack(ptr, (char*) name.c_str());
        hash_combine<Table>(seed, *table);
    }
    return seed;
}

inline KDBTables Tables;
