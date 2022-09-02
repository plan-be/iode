#include "pch.h"


class KDBEquationsTest : public KDBTest, public ::testing::Test
{
protected:
    KDBEquations kdb;

    void SetUp() override
    {
        load_global_kdb(I_EQUATIONS, input_test_dir + "fun.eqs");
    }

    // void TearDown() override {}
};


TEST_F(KDBEquationsTest, Load)
{
    KDBEquations kdb2;
    EXPECT_EQ(kdb2.count(), 274);
}

TEST_F(KDBEquationsTest, Save)
{
    // save in binary format
    save_global_kdb(I_EQUATIONS, output_test_dir + "fun.eqs");
    kdb.dump(output_test_dir + "fun.eqs");

    // save in ascii format
    save_global_kdb(I_EQUATIONS, output_test_dir + "fun.ae");
    kdb.dump(output_test_dir + "fun.ae");
}

TEST_F(KDBEquationsTest, GetLec)
{
    int pos = 0;
    std::string name = kdb.get_name(pos);
    std::string expected_lec = "(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)";

    // by pos
    EXPECT_EQ(kdb.get_lec(pos), expected_lec);

    // by name
    EXPECT_EQ(kdb.get_lec(name), expected_lec);
}

TEST_F(KDBEquationsTest, Get)
{
    int pos = 0;
    std::string name = kdb.get_name(pos);

    // by pos
    Equation eq1 = kdb.get(pos);

    // by name
    Equation eq2 = kdb.get(name);
}

TEST_F(KDBEquationsTest, CreateRemove)
{
// disable this test function in case AddressSanitizer is activated because 
// it crashes on Windows Server 2019 (Github container)
#if !(defined _MSC_VER && defined __SANITIZE_ADDRESS__)
    std::string name = "ACAF";
    kdb.remove(name);
    EXPECT_THROW(kdb.get(name), std::runtime_error);

    std::string lec = "(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)";
    std::string method = "LSQ";
    Sample sample("1980Y1", "1996Y1");
    std::string comment = "Equation comment";
    std::string block = "ACAF";
    std::string instruments = "Equation instruments";
    std::array<float, EQS_NBTESTS> tests = { 1, 0.0042699, 0.00818467, 5.19945e-05, 0.0019271461, 23.545813, 32.2732, 0.82176137, 0.79629868, 2.3293459, 83.8075 };
    bool date = true;

    kdb.add(name, lec, comment, method, &sample, instruments, block, tests, date);
#endif
}

TEST_F(KDBEquationsTest, Update)
{
    std::string name = "ACAF";
    std::string new_lec = "(ACAF/VAF[-1]) :=acaf2*GOSF[-1]+\nacaf4*(TIME=1995)";

    kdb.update(name, new_lec);
    EXPECT_EQ(kdb.get_lec(name), new_lec);
}

TEST_F(KDBEquationsTest, Copy)
{
    std::string name = "ACAF";
    Equation eq = kdb.get(name);

    Equation copy_eq = kdb.copy(name);
    EXPECT_EQ(copy_eq, eq);
}

TEST_F(KDBEquationsTest, Filter)
{
    std::string pattern = "A*;$ENVI;*_";
    std::vector<std::string> expected_names;
    KDBEquations* local_kdb;
    KDBEquations global_kdb;

    load_global_kdb(I_LISTS, input_test_dir + "fun.lst");

    std::vector<std::string> all_names;
    for (int p = 0; p < global_kdb.count(); p++) all_names.push_back(global_kdb.get_name(p));

    int nb_total_comments = global_kdb.count();
    // A*
    for (const std::string& name : all_names) if (name.front() == 'A') expected_names.push_back(name);
    // $ENVI
    unsigned char** c_expanded_list = KL_expand("$ENVI");
    for (int i = 0; i < SCR_tbl_size(c_expanded_list); i++) expected_names.push_back((char*) c_expanded_list[i]);
    // *_
    for (const std::string& name : all_names) if (name.back() == '_') expected_names.push_back(name);

    // WARNING : K_refer() does NOT remove possible duplicate entries !

    // create local kdb
    local_kdb = new KDBEquations(pattern);
    EXPECT_EQ(local_kdb->count(), expected_names.size());

    // modify an element of the local KDB and check if the 
    // corresponding element of the global KDB also changes
    std::string name = "ACAF";
    std::string modified_lec = "(ACAF/VAF[-1]) :=acaf2*GOSF[-1]+\nacaf4*(TIME=1995)";
    local_kdb->update(name, modified_lec);
    EXPECT_EQ(local_kdb->get_lec(name), modified_lec);
    EXPECT_EQ(global_kdb.get_lec(name), modified_lec);

    // delete an element from the local KDB and check if it has also 
    // been deleted from the global KDB
    local_kdb->remove(name);
    EXPECT_FALSE(local_kdb->contains(name));
    EXPECT_FALSE(global_kdb.contains(name));

    // add an element to the local KDB and check if it has also 
    // been added to the global KDB
    std::string lec = "(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)";
    std::string method = "LSQ";
    Sample sample("1980Y1", "1996Y1");
    std::string comment = "Equation comment";
    std::string block = "ACAF";
    std::string instruments = "Equation instruments";
    std::array<float, EQS_NBTESTS> tests = { 1, 0.0042699, 0.00818467, 5.19945e-05, 0.0019271461, 23.545813, 32.2732, 0.82176137, 0.79629868, 2.3293459, 83.8075 };
    bool date = true;
    local_kdb->add(name, lec, comment, method, &sample, instruments, block, tests, date);
    EXPECT_EQ(local_kdb->get_lec(name), lec);
    EXPECT_EQ(global_kdb.get_lec(name), lec);

    // delete local kdb
    delete local_kdb;
    EXPECT_EQ(global_kdb.count(), nb_total_comments);
    EXPECT_EQ(global_kdb.get_lec(name), lec);
}

TEST_F(KDBEquationsTest, HardCopy)
{
    std::string pattern = "A*;$ENVI;*_";
    std::vector<std::string> expected_names;
    KDBEquations* local_kdb;
    KDBEquations global_kdb;

    load_global_kdb(I_LISTS, input_test_dir + "fun.lst");

    std::vector<std::string> all_names;
    for (int p = 0; p < global_kdb.count(); p++) all_names.push_back(global_kdb.get_name(p));

    int nb_total_comments = global_kdb.count();
    // A*
    for (const std::string& name : all_names) if (name.front() == 'A') expected_names.push_back(name);
    // $ENVI
    unsigned char** c_expanded_list = KL_expand("$ENVI");
    for (int i = 0; i < SCR_tbl_size(c_expanded_list); i++) expected_names.push_back((char*) c_expanded_list[i]);
    // *_
    for (const std::string& name : all_names) if (name.back() == '_') expected_names.push_back(name);

    // remove duplicate entries
    // NOTE: std::unique only removes consecutive duplicated elements, 
    //       so the vector needst to be sorted first
    std::sort(expected_names.begin(), expected_names.end());
    std::vector<std::string>::iterator it = std::unique(expected_names.begin(), expected_names.end());  
    expected_names.resize(std::distance(expected_names.begin(), it));

    // create local kdb
    local_kdb = new KDBEquations(pattern, false);
    EXPECT_EQ(local_kdb->count(), expected_names.size());

    // modify an element of the local KDB and check if the 
    // corresponding element of the global KDB didn't changed
    std::string name = "ACAF";
    std::string lec = global_kdb.get_lec(name);
    std::string modified_lec = "(ACAF/VAF[-1]) :=acaf2*GOSF[-1]+\nacaf4*(TIME=1995)";
    local_kdb->update(name, modified_lec);
    EXPECT_EQ(local_kdb->get_lec(name), modified_lec);
    EXPECT_EQ(global_kdb.get_lec(name), lec);

    // delete an element from the local KDB and check if it has not 
    // been deleted from the global KDB
    name = "ACAG";
    local_kdb->remove(name);
    EXPECT_FALSE(local_kdb->contains(name));
    EXPECT_TRUE(global_kdb.contains(name));

    // delete local kdb
    delete local_kdb;
    EXPECT_EQ(global_kdb.count(), nb_total_comments);
}


TEST_F(KDBEquationsTest, Estimate)
{
    load_global_kdb(I_SCALARS, input_test_dir + "fun.scl");
    load_global_kdb(I_VARIABLES, input_test_dir + "fun.var");

    kdb.equations_estimate("1980Y1", "1996Y1", "ACAF");

    KDBVariables kdb_vars;
    EXPECT_DOUBLE_EQ(round(1e6 * kdb_vars.get_var("_YRES", "1980Y1")) / 1e6, -0.00115);

    Equation eq = kdb.get("ACAF");
    EXPECT_DOUBLE_EQ(round(1e6 * eq.get_tests()[KE_R2 - 10]) / 1e6, 0.821815);
    EXPECT_DOUBLE_EQ(round(1e6 * eq.get_tests()[KE_R2ADJ - 10]) / 1e6, 0.79636);
    EXPECT_DOUBLE_EQ(round(1e6 * eq.get_tests()[KE_DW - 10]) / 1e6, 2.33007);
}
