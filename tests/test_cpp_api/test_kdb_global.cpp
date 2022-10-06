#include "pch.h"


class KDBGlobalTest : public KDBTest, public ::testing::Test
{
protected:
    void SetUp() override
    {
        load_global_kdb(I_EQUATIONS, input_test_dir + "fun.eqs");
        load_global_kdb(I_LISTS, input_test_dir + "fun.lst");
    }

    // void TearDown() override {}
};


TEST_F(KDBGlobalTest, Filter)
{
    std::vector<std::string> list_names;
    std::vector<std::string> expected_list_names;

    KDBEquations global_kdb;
    std::vector<std::string> all_names;
    for (int p = 0; p < global_kdb.count(); p++) all_names.push_back(global_kdb.get_name(p));

    // simple patterns
    list_names = char_array_to_std_string_vector(filter_kdb_names(I_EQUATIONS, "A*"));
    expected_list_names.clear();
    for (const std::string& name : all_names) if (name.front() == 'A') expected_list_names.push_back(name);
    EXPECT_EQ(list_names, expected_list_names);

    list_names = char_array_to_std_string_vector(filter_kdb_names(I_EQUATIONS, "*_"));
    expected_list_names.clear();
    for (const std::string& name : all_names) if (name.back() == '_') expected_list_names.push_back(name);
    EXPECT_EQ(list_names, expected_list_names);

    // list
    std::string list = "$ENVI";
    list_names = char_array_to_std_string_vector(filter_kdb_names(I_EQUATIONS, list));
    expected_list_names.clear();
    char* c_list = const_cast<char*>(list.c_str());
    unsigned char** c_expanded_list = KL_expand(c_list);
    for (int i = 0; i < SCR_tbl_size(c_expanded_list); i++) expected_list_names.push_back((char*) c_expanded_list[i]);
    // Note: filter_kdb_names() calls remove_duplicates() which calls sort()
    std::sort(expected_list_names.begin(), expected_list_names.end());
    EXPECT_EQ(list_names, expected_list_names);

    // complex pattern
    list_names = char_array_to_std_string_vector(filter_kdb_names(I_EQUATIONS, "A*;" + list + ";*_"));
    expected_list_names.clear();
    for (const std::string& name : all_names) if (name.front() == 'A') expected_list_names.push_back(name);
    for (int i = 0; i < SCR_tbl_size(c_expanded_list); i++) expected_list_names.push_back((char*) c_expanded_list[i]);
    for (const std::string& name : all_names) if (name.back() == '_') expected_list_names.push_back(name);
    // Note: filter_kdb_names() calls remove_duplicates() (which calls sort())
    remove_duplicates(expected_list_names);
    EXPECT_EQ(list_names, expected_list_names);
}
