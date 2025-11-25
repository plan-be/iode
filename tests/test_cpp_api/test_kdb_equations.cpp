#include "pch.h"


class KDBEquationsTest : public KDBTest, public ::testing::Test
{
protected:
    std::array<float, EQS_NBTESTS> tests = { 1.0f, 0.0042699f, 0.00818467f, 5.19945e-05f, 0.0019271461f, 
                                             23.545813f, 32.2732f, 0.82176137f, 0.79629868f, 2.3293459f, 83.8075f };

    void SetUp() override
    {
        KDBEquations kdb_eqs(true, input_test_dir + "fun.ae");
    }

    // void TearDown() override {}
};


TEST_F(KDBEquationsTest, Load)
{
    KDBEquations kdb(false, input_test_dir + prefix_filename + "fun.eqs");
    EXPECT_EQ(kdb.size(), 274);
}

TEST_F(KDBEquationsTest, Subset)
{
    std::string pattern = "A*";
    std::string lec = Equations.get_lec("ACAF");
    std::string new_lec = "(ACAF/VAF[-1]) :=acaf2*GOSF[-1]+\nacaf4*(TIME=1995)";

    // GLOBAL KDB
    EXPECT_EQ(Equations.size(), 274);
    EXPECT_TRUE(Equations.is_global_database());

    // DEEP COPY SUBSET
    KDBEquations* kdb_subset_deep_copy = Equations.subset(pattern, true);
    std::vector<std::string> names = Equations.filter_names(pattern);
    EXPECT_EQ(kdb_subset_deep_copy->size(), names.size());
    EXPECT_TRUE(kdb_subset_deep_copy->is_local_database());
    kdb_subset_deep_copy->update("ACAF", new_lec);
    EXPECT_EQ(Equations.get_lec("ACAF"), lec);
    EXPECT_EQ(kdb_subset_deep_copy->get_lec("ACAF"), new_lec);

    // SHALLOW COPY SUBSET
    KDBEquations* kdb_subset_shallow_copy = Equations.subset(pattern, false);
    EXPECT_EQ(kdb_subset_shallow_copy->size(), names.size());
    EXPECT_TRUE(kdb_subset_shallow_copy->is_shallow_copy_database());
    kdb_subset_shallow_copy->update("ACAF", new_lec);
    EXPECT_EQ(Equations.get_lec("ACAF"), new_lec);
    EXPECT_EQ(kdb_subset_shallow_copy->get_lec("ACAF"), new_lec);
}

TEST_F(KDBEquationsTest, Save)
{
    // save in binary format
    Equations.save(output_test_dir + "fun.eqs");

    // save in ascii format
    Equations.save(output_test_dir + "fun.ae");
}

TEST_F(KDBEquationsTest, GetLec)
{
    std::string expected_lec = "(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)";
    EXPECT_EQ(Equations.get_lec("ACAF"), expected_lec);
}

TEST_F(KDBEquationsTest, Get)
{
    Equation* eq = nullptr;
    std::string expected_lec;

    eq = Equations.get("ACAF");
    EXPECT_EQ(eq->lec, "(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)");
    EXPECT_EQ(eq->get_date_as_string(), "12-06-1998");
    EXPECT_EQ(eq->sample.to_string(), "1980Y1:1996Y1");
    EXPECT_EQ(eq->get_method(), "LSQ");
    delete eq;

    eq = Equations.get("BVY");
    EXPECT_EQ(eq->lec, "BVY:=YN+YK");
    EXPECT_EQ(eq->get_date_as_string(), "");
    EXPECT_EQ(eq->sample.to_string(), ":");
    EXPECT_EQ(eq->get_method(), "LSQ");
    delete eq;

    eq = Equations.get("W");
    expected_lec = "dln (W/WO) := dln ZJ +gamma1*dln PROD + gamma_ *ln ((NATY-UY)/NATY)[-1]+gamma2\n";
    expected_lec += "+gamma3*(- ln(WCF/(PAF_*WO))[-1]+gamma4*ln (WMIN/ZJ)\n";
    expected_lec += "+gamma5*ln PROD[-1])+(XW)+XWC";
    EXPECT_EQ(eq->lec, expected_lec);
    EXPECT_EQ(eq->get_method(), "LSQ");
    EXPECT_EQ(eq->sample.to_string(), "1975Y1:1997Y1");
    EXPECT_EQ(eq->get_comment(), "");
    EXPECT_EQ(eq->instruments, "");
    EXPECT_EQ(eq->block, "W");
    EXPECT_EQ(eq->get_date_as_string(), "");
    delete eq;

    // other
    eq = Equations.get("DTH1");
    EXPECT_EQ(eq->endo, "DTH1");
    EXPECT_EQ(eq->lec, "DTH1:=DTH1C");
    EXPECT_EQ(eq->solved, 0);
    EXPECT_EQ(eq->method, EQ_LSQ);
    EXPECT_EQ(eq->sample.to_string(), ":");
    EXPECT_EQ(eq->comment, "");
    EXPECT_EQ(eq->block, "DTH1");
    EXPECT_EQ(eq->instruments, "");
    delete eq;
}

TEST_F(KDBEquationsTest, GetNames)
{
    std::vector<std::string> expected_names;
    for (int i=0; i < Equations.size(); i++) expected_names.push_back(Equations.get_name(i));
    std::vector<std::string> names = Equations.get_names();
    EXPECT_EQ(names, expected_names);
}

TEST_F(KDBEquationsTest, CreateRemove)
{
// disable this test function in case AddressSanitizer is activated because 
// it crashes on Windows Server 2019 (Github container)
#if !(defined _MSC_VER && defined __SANITIZE_ADDRESS__)
    std::string name = "ACAF";
    Equations.remove(name);
    EXPECT_THROW(Equations.get(name), std::invalid_argument);

    std::string lec = "(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)";
    std::string method = "LSQ";
    std::string from = "1980Y1";
    std::string to = "1996Y1";
    std::string comment = "Equation comment";
    std::string block = "ACAF";
    std::string instruments = "Equation instruments";
    bool date = true;

    Equations.add(name, lec, method, from, to, comment, instruments, block, date);
#endif
}

TEST_F(KDBEquationsTest, Update)
{
    std::string name = "ACAF";
    std::string new_lec = "(ACAF/VAF[-1]) :=acaf2*GOSF[-1]+\nacaf4*(TIME=1995)";

    Equations.update(name, new_lec);
    EXPECT_EQ(Equations.get_lec(name), new_lec);
}

TEST_F(KDBEquationsTest, Copy)
{
    std::string name = "ACAF";
    Equation* eq = Equations.get(name);
    Equation* copy_eq = Equations.copy(name);
    EXPECT_EQ(*copy_eq, *eq);
    delete eq;
    delete copy_eq;
}

TEST_F(KDBEquationsTest, Filter)
{
    std::string pattern = "A*;$ENVI;*_";
    std::vector<std::string> expected_names;
    KDBEquations* kdb_subset;

    KDBLists kdb_lst(true, input_test_dir + "fun.al");

    std::vector<std::string> all_names;
    for (int p = 0; p < Equations.size(); p++) all_names.push_back(Equations.get_name(p));

    int nb_total_equations = Equations.size();
    // A*
    for (const std::string& name : all_names) if (name.front() == 'A') expected_names.push_back(name);
    // $ENVI
    unsigned char** c_expanded_list = KL_expand(const_cast<char*>("$ENVI"));
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
    kdb_subset = Equations.subset(pattern);
    EXPECT_EQ(kdb_subset->size(), expected_names.size());
    EXPECT_EQ(kdb_subset->get_names(), expected_names);

    // modify an element of the local KDB and check if the 
    // corresponding element of the global KDB also changes
    std::string name = "ACAF";
    std::string modified_lec = "(ACAF/VAF[-1]) :=acaf2*GOSF[-1]+\nacaf4*(TIME=1995)";
    kdb_subset->update(name, modified_lec);
    EXPECT_EQ(kdb_subset->get_lec(name), modified_lec);
    EXPECT_EQ(Equations.get_lec(name), modified_lec);

    // delete an element from the local KDB and check if it has also 
    // been deleted from the global KDB
    kdb_subset->remove(name);
    EXPECT_FALSE(kdb_subset->contains(name));
    EXPECT_FALSE(Equations.contains(name));

    // add an element to the local KDB and check if it has also 
    // been added to the global KDB
    std::string lec = "(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)";
    std::string method = "LSQ";
    std::string from = "1980Y1";
    std::string to = "1996Y1";
    std::string comment = "Equation comment";
    std::string block = "ACAF";
    std::string instruments = "Equation instruments";
    bool date = true;
    kdb_subset->add(name, lec, method, from, to, comment, instruments, block, date);
    EXPECT_EQ(kdb_subset->get_lec(name), lec);
    EXPECT_EQ(Equations.get_lec(name), lec);

    // try to add an element to the local KDB which is already present 
    // in the global KDB
    EXPECT_THROW(kdb_subset->add("BQY", lec, method, from, to, comment, instruments, block, date), std::invalid_argument);

    // delete local kdb
    delete kdb_subset;
    EXPECT_EQ(Equations.size(), nb_total_equations);
    EXPECT_EQ(Equations.get_lec(name), lec);

    // wrong pattern
    pattern = "anjfks";
    EXPECT_THROW(Equations.subset(pattern), std::runtime_error);
}

TEST_F(KDBEquationsTest, DeepCopy)
{
    std::string pattern = "A*;$ENVI;*_";
    std::vector<std::string> expected_names;
    KDBEquations* kdb_subset;

    KDBLists kdb_lst(true, input_test_dir + "fun.al");

    std::vector<std::string> all_names;
    for (int p = 0; p < Equations.size(); p++) all_names.push_back(Equations.get_name(p));

    int nb_total_equations = Equations.size();
    // A*
    for (const std::string& name : all_names) if (name.front() == 'A') expected_names.push_back(name);
    // $ENVI
    unsigned char** c_expanded_list = KL_expand(const_cast<char*>("$ENVI"));
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
    kdb_subset = Equations.subset(pattern, true);
    EXPECT_EQ(kdb_subset->size(), expected_names.size());
    EXPECT_EQ(kdb_subset->get_names(), expected_names);

    // modify an element of the local KDB and check if the 
    // corresponding element of the global KDB didn't changed
    std::string name = "ACAF";
    std::string lec = Equations.get_lec(name);
    std::string modified_lec = "(ACAF/VAF[-1]) :=acaf2*GOSF[-1]+\nacaf4*(TIME=1995)";
    kdb_subset->update(name, modified_lec);
    EXPECT_EQ(kdb_subset->get_lec(name), modified_lec);
    EXPECT_EQ(Equations.get_lec(name), lec);

    // delete an element from the local KDB and check if it has not 
    // been deleted from the global KDB
    name = "ACAG";
    kdb_subset->remove(name);
    EXPECT_FALSE(kdb_subset->contains(name));
    EXPECT_TRUE(Equations.contains(name));

    // delete local kdb
    delete kdb_subset;
    EXPECT_EQ(Equations.size(), nb_total_equations);

    // wrong pattern
    pattern = "anjfks";
    EXPECT_THROW(Equations.subset(pattern, true), std::runtime_error);
}

TEST_F(KDBEquationsTest, CopyFrom)
{
    std::string pattern = "A* *_";
    std::string filename = input_test_dir + prefix_filename + "fun.eqs";
    int expected_nb_comments = Equations.size();
    std::vector<std::string> v_expected_names;

    // Copy entire file
    Equations.clear();
    Equations.copy_from(filename, "*");
    EXPECT_EQ(Equations.size(), expected_nb_comments); 

    // copy subset
    v_expected_names = Equations.filter_names(pattern);
    Equations.clear();
    Equations.copy_from(filename, pattern);
    EXPECT_EQ(Equations.size(), v_expected_names.size());  
    EXPECT_EQ(Equations.get_names(), v_expected_names);  
}

TEST_F(KDBEquationsTest, Merge)
{
    std::string pattern = "A*";

    // create deep copies kdb
    KDBEquations* kdb0 = Equations.subset(pattern, true);
    KDBEquations* kdb1 = Equations.subset(pattern, true);
    KDBEquations* kdb_to_merge = Equations.subset(pattern, true);

    // add an element to the KDB to be merged
    std::string new_name = "ACAF2";
    std::string new_lec = "(ACAF2/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)";
    std::string method = "LSQ";
    std::string from = "1980Y1";
    std::string to = "1996Y1";
    std::string comment = "Equation comment";
    std::string block = "ACAF";
    std::string instruments = "Equation instruments";
    bool date = true;
    kdb_to_merge->add(new_name, new_lec, method, from, to, comment, instruments, block, date);

    // modify an existing element of the KDB to be merge
    std::string name = "ACAF";
    std::string unmodified_lec = kdb_to_merge->get_lec(name);
    std::string modified_lec = "(ACAF/VAF[-1]) :=acaf2*GOSF[-1]+\nacaf4*(TIME=1995)";
    kdb_to_merge->update(name, modified_lec);

    // merge (overwrite)
    kdb0->merge(*kdb_to_merge, true);
    // a) check kdb0 contains new item of KDB to be merged
    EXPECT_TRUE(kdb0->contains(new_name));
    EXPECT_EQ(kdb0->get_lec(new_name), new_lec);
    // b) check already existing item has been overwritten
    EXPECT_EQ(kdb0->get_lec(name), modified_lec); 

    // merge (NOT overwrite)
    kdb1->merge(*kdb_to_merge, false);
    // b) check already existing item has NOT been overwritten
    EXPECT_EQ(kdb1->get_lec(name), unmodified_lec);
}

TEST_F(KDBEquationsTest, Search)
{
    std::string eqs_name = "ACAG";
    std::vector<std::string> objs_list;

    KDBComments kdb_cmt(true, input_test_dir + "fun.ac");
    KDBIdentities kdb_idt(true, input_test_dir + "fun.ai");
    KDBLists kdb_lst(true, input_test_dir + "fun.al");
    KDBScalars kdb_scl(true, input_test_dir + "fun.as");
    KDBTables kdb_tbl(true, input_test_dir + "fun.at");
    KDBVariables kdb_var(true, input_test_dir + "fun.av");

    std::vector<std::string> expected_cmts = { eqs_name };
    objs_list = Comments.search(eqs_name);
    EXPECT_EQ(objs_list, expected_cmts);

    std::vector<std::string> expected_eqs = { eqs_name, "FLG" };
    objs_list = Equations.search(eqs_name);
    EXPECT_EQ(objs_list, expected_eqs);

    objs_list = Identities.search(eqs_name);
    EXPECT_EQ(objs_list.size(), 0);

    std::vector<std::string> expected_lts = { "COPY0", "ENDO0", "TOTAL0", "_RES" };
    objs_list = Lists.search(eqs_name);
    EXPECT_EQ(objs_list, expected_lts);

    objs_list = Scalars.search(eqs_name);
    EXPECT_EQ(objs_list.size(), 0);

    std::vector<std::string> expected_tbl = { "GFR", "GFRLEVEL" };
    objs_list = Tables.search(eqs_name);
    EXPECT_EQ(objs_list, expected_tbl);

    std::vector<std::string> expected_vars = { eqs_name };
    objs_list = Variables.search(eqs_name);
    EXPECT_EQ(objs_list, expected_vars);
}

TEST_F(KDBEquationsTest, Hash)
{
    Equation* eq = nullptr;
    std::size_t hash_val = hash_value(Equations);

    // modify an entry
    eq = Equations.get("ACAF");
    std::string lec = eq->lec;
    eq->set_lec("(ACAF/VAF[-1]) :=acaf2*GOSF[-1]+\nacaf4*(TIME=1995)");
    Equations.update("ACAF", *eq);
    std::size_t hash_val_modified = hash_value(Equations);
    EXPECT_NE(hash_val, hash_val_modified);
    // restore original entry
    eq->set_lec(lec);
    Equations.update("ACAF", *eq);
    Equation* eq_restored = Equations.get("ACAF");
    EXPECT_EQ(*eq_restored, *eq);
    std::size_t hash_val_restored = hash_value(Equations);
    EXPECT_EQ(hash_val, hash_val_restored);
    delete eq;
    delete eq_restored;

    // remove an entry
    eq = Equations.get("ACAG");
    Equations.remove("ACAG");
    hash_val_modified = hash_value(Equations);
    EXPECT_NE(hash_val, hash_val_modified);
    // restore original entry
    Equations.add("ACAG", *eq);
    hash_val_restored = hash_value(Equations);
    EXPECT_EQ(hash_val, hash_val_restored);
    delete eq;

    // add an entry
    lec = "TEST := 0";
    std::string method = "LSQ";
    std::string from = "1980Y1";
    std::string to = "1996Y1";
    std::string comment = "Equation comment";
    std::string block = "TEST";
    std::string instruments = "Equation instruments";
    bool date = true;
    Equations.add("TEST", lec, method, from, to, comment, instruments, block, date);
    hash_val_modified = hash_value(Equations);
    EXPECT_NE(hash_val, hash_val_modified);   
    // remove the entry
    Equations.remove("TEST");
    hash_val_restored = hash_value(Equations);
    EXPECT_EQ(hash_val, hash_val_restored);
}
