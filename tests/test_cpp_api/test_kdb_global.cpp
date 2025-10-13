#include "pch.h"


class KDBGlobalTest : public KDBTest, public ::testing::Test
{
protected:
    void SetUp() override
    {
        KDBEquations kdb_eqs(input_test_dir + "fun.ae");
        KDBLists kdb_lst(input_test_dir + "fun.al");
    }

    // void TearDown() override {}
};


TEST_F(KDBGlobalTest, Filter)
{
    std::vector<std::string> list_names;
    std::vector<std::string> expected_list_names;

    std::vector<std::string> all_names;
    for (int p = 0; p < Equations.size(); p++) all_names.push_back(Equations.get_name(p));

    // simple patterns
    list_names = Equations.get_names("A*");
    expected_list_names.clear();
    for (const std::string& name : all_names) if (name.front() == 'A') expected_list_names.push_back(name);
    EXPECT_EQ(list_names, expected_list_names);

    list_names = Equations.get_names("*_");
    expected_list_names.clear();
    for (const std::string& name : all_names) if (name.back() == '_') expected_list_names.push_back(name);
    EXPECT_EQ(list_names, expected_list_names);

    // list
    std::string list = "$ENVI";
    list_names = Equations.get_names(list);
    expected_list_names.clear();
    char* c_list = const_cast<char*>(list.c_str());
    unsigned char** c_expanded_list = KL_expand(c_list);
    for (int i = 0; i < SCR_tbl_size(c_expanded_list); i++) expected_list_names.push_back((char*) c_expanded_list[i]);
    // Note: get_names() calls sort_and_remove_duplicates() which calls sort()
    std::sort(expected_list_names.begin(), expected_list_names.end());
    EXPECT_EQ(list_names, expected_list_names);

    // complex pattern
    list_names = Equations.get_names("A*;" + list + ";*_");
    expected_list_names.clear();
    for (const std::string& name : all_names) if (name.front() == 'A') expected_list_names.push_back(name);
    for (int i = 0; i < SCR_tbl_size(c_expanded_list); i++) expected_list_names.push_back((char*) c_expanded_list[i]);
    for (const std::string& name : all_names) if (name.back() == '_') expected_list_names.push_back(name);
    // Note: get_names() calls sort_and_remove_duplicates() (which calls sort())
    sort_and_remove_duplicates(expected_list_names);
    EXPECT_EQ(list_names, expected_list_names);

    // pattern containing a name that does not exist in the global workspace
    // -> name is skipped by default (must_exist = true)
    list_names = Equations.get_names("A*;DO_NOT_EXIST");
    expected_list_names.clear();
    for (const std::string& name : all_names) if (name.front() == 'A') expected_list_names.push_back(name);
    // Note: get_names() calls sort_and_remove_duplicates() (which calls sort())
    sort_and_remove_duplicates(expected_list_names);
    EXPECT_EQ(list_names, expected_list_names);

    // must_exist = false
    list_names = Equations.get_names("A*;DO_NOT_EXIST", false);
    expected_list_names.push_back("DO_NOT_EXIST");
    EXPECT_EQ(list_names, expected_list_names);
}

TEST_F(KDBGlobalTest, LowToHigh)
{
    std::string varfile = input_test_dir + "fun.av";

    // Set the sample for the variable WS
    Variables.set_sample("2010Q1", "2020Q4");
    
    // Linear interpolation / stock
    low_to_high(LTOH_STOCK, 'L', varfile, "ACAF");
    EXPECT_TRUE(Variables.contains("ACAF"));
    EXPECT_DOUBLE_EQ(round(Variables.get_var("ACAF", "2014Q3") * 1e6) / 1e6, -79.729132);
    
    // Linear interpolation / flow
    low_to_high(LTOH_FLOW, 'L', varfile, "ACAG");
    EXPECT_TRUE(Variables.contains("ACAG"));
    EXPECT_DOUBLE_EQ(round(Variables.get_var("ACAG", "2014Q3") * 1e6) / 1e6, 8.105075);
        
    // Cubic Splines / stock
    low_to_high(LTOH_STOCK, 'C', varfile, "ACAF");
    EXPECT_DOUBLE_EQ(round(Variables.get_var("ACAF", "2012Q3") * 1e6) / 1e6, -52.805666 );
    
    // Cubic splines / flow
    low_to_high(LTOH_FLOW, 'C', varfile, "ACAG");
    EXPECT_DOUBLE_EQ(round(Variables.get_var("ACAG", "2012Q3") * 1e6) / 1e6, 7.613577);
    
    // Step / stock
    low_to_high(LTOH_STOCK, 'S', varfile, "ACAF");
    EXPECT_DOUBLE_EQ(round(Variables.get_var("ACAF", "2014Q3") * 1e6) / 1e6, -83.340625);
    
    // Step / flow
    low_to_high(LTOH_FLOW, 'S', varfile, "ACAG");
    EXPECT_DOUBLE_EQ(round(Variables.get_var("ACAG", "2014Q3") * 1e6) / 1e6, 8.105075);
}

TEST_F(KDBGlobalTest, HighToLow)
{
    std::string varfile = input_test_dir + "fun_q.var";

    // Set the sample for the variable WS
    Variables.set_sample("2000Y1", "2020Y1");
    
    // Last Obs
    high_to_low(HTOL_LAST, varfile, "ACAF");
    EXPECT_TRUE(Variables.contains("ACAF"));
    EXPECT_DOUBLE_EQ(round(Variables.get_var("ACAF", "2014Y1") * 1e6) / 1e6, -83.340625);
    
    // Mean
    high_to_low(HTOL_MEAN, varfile, "ACAG");
    EXPECT_TRUE(Variables.contains("ACAG"));
    EXPECT_DOUBLE_EQ(round(Variables.get_var("ACAG", "2014Y1") * 1e6) / 1e6, 8.105075);
        
    // Sum
    high_to_low(HTOL_SUM, varfile, "AOUC");
    EXPECT_TRUE(Variables.contains("AOUC"));
    EXPECT_DOUBLE_EQ(round(Variables.get_var("AOUC", "2014Y1") * 1e6) / 1e6, 1.423714);
}
