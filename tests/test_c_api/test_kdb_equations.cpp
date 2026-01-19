#include "pch.h"


class KDBEquationsTest : public TestAbstract, public ::testing::Test
{
protected:
    std::array<float, EQS_NBTESTS> tests = { 1.0f, 0.0042699f, 0.00818467f, 5.19945e-05f, 0.0019271461f, 
                                             23.545813f, 32.2732f, 0.82176137f, 0.79629868f, 2.3293459f, 83.8075f };

    void SetUp() override
    {
        global_ws_eqs->load(str_input_test_dir + "fun.ae");
    }

    // void TearDown() override {}
};


TEST_F(KDBEquationsTest, Load)
{
    KDBEquations kdb(false);
    kdb.load(str_input_test_dir + prefix_filename + "fun.eqs");
    EXPECT_EQ(kdb.size(), 274);
}

TEST_F(KDBEquationsTest, Subset)
{
    std::string pattern = "A*";
    std::string lec = global_ws_eqs->get_lec("ACAF");
    std::string new_lec = "(ACAF/VAF[-1]) :=acaf2*GOSF[-1]+\nacaf4*(TIME=1995)";

    // GLOBAL KDB
    EXPECT_EQ(global_ws_eqs->size(), 274);
    EXPECT_TRUE(global_ws_eqs->is_global_database());
    std::set<std::string> names = global_ws_eqs->filter_names(pattern);

    // DEEP COPY SUBSET
    KDBEquations* kdb_subset_deep_copy = new KDBEquations(global_ws_eqs.get(), pattern, true);
    EXPECT_EQ(kdb_subset_deep_copy->size(), names.size());
    EXPECT_TRUE(kdb_subset_deep_copy->is_local_database());
    kdb_subset_deep_copy->update("ACAF", new_lec);
    EXPECT_EQ(global_ws_eqs->get_lec("ACAF"), lec);
    EXPECT_EQ(kdb_subset_deep_copy->get_lec("ACAF"), new_lec);

    // SHALLOW COPY SUBSET
    KDBEquations* kdb_subset_shallow_copy = new KDBEquations(global_ws_eqs.get(), pattern, false);
    EXPECT_EQ(kdb_subset_shallow_copy->size(), names.size());
    EXPECT_TRUE(kdb_subset_shallow_copy->is_shallow_copy_database());
    kdb_subset_shallow_copy->update("ACAF", new_lec);
    EXPECT_EQ(global_ws_eqs->get_lec("ACAF"), new_lec);
    EXPECT_EQ(kdb_subset_shallow_copy->get_lec("ACAF"), new_lec);
}

TEST_F(KDBEquationsTest, Save)
{
    // save in binary format
    global_ws_eqs->save(str_output_test_dir + "fun.eqs");

    // save in ascii format
    global_ws_eqs->save(str_output_test_dir + "fun.ae");
}

TEST_F(KDBEquationsTest, GetLec)
{
    std::string expected_lec = "(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)";
    EXPECT_EQ(global_ws_eqs->get_lec("ACAF"), expected_lec);
}

TEST_F(KDBEquationsTest, Get)
{
    Equation* eq = nullptr;
    std::string expected_lec;

    eq = global_ws_eqs->get("ACAF");
    EXPECT_EQ(eq->lec, "(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)");
    EXPECT_EQ(eq->get_date_as_string(), "12-06-1998");
    EXPECT_EQ(eq->sample.to_string(), "1980Y1:1996Y1");
    EXPECT_EQ(eq->get_method(), "LSQ");
    delete eq;

    eq = global_ws_eqs->get("BVY");
    EXPECT_EQ(eq->lec, "BVY:=YN+YK");
    EXPECT_EQ(eq->get_date_as_string(), "");
    EXPECT_EQ(eq->sample.to_string(), ":");
    EXPECT_EQ(eq->get_method(), "LSQ");
    delete eq;

    eq = global_ws_eqs->get("W");
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
    eq = global_ws_eqs->get("DTH1");
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
    std::set<std::string> expected_names;
    for (int i=0; i < global_ws_eqs->size(); i++) 
        expected_names.insert(global_ws_eqs->get_name(i));
    std::set<std::string> names = global_ws_eqs->get_names();
    EXPECT_EQ(names, expected_names);
}

TEST_F(KDBEquationsTest, CreateRemove)
{
// disable this test function in case AddressSanitizer is activated because 
// it crashes on Windows Server 2019 (Github container)
#if !(defined _MSC_VER && defined __SANITIZE_ADDRESS__)
    std::string name = "ACAF";
    global_ws_eqs->remove(name);
    EXPECT_THROW(global_ws_eqs->get(name), std::invalid_argument);

    std::string lec = "(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)";
    std::string method = "LSQ";
    std::string from = "1980Y1";
    std::string to = "1996Y1";
    std::string comment = "Equation comment";
    std::string block = "ACAF";
    std::string instruments = "Equation instruments";
    bool date = true;

    Equation eq(name, lec, method, from, to, comment, instruments, block, date);
    global_ws_eqs->add(name, eq);
#endif
}

TEST_F(KDBEquationsTest, Update)
{
    std::string name = "ACAF";
    std::string new_lec = "(ACAF/VAF[-1]) :=acaf2*GOSF[-1]+\nacaf4*(TIME=1995)";

    global_ws_eqs->update(name, new_lec);
    EXPECT_EQ(global_ws_eqs->get_lec(name), new_lec);
}

TEST_F(KDBEquationsTest, Filter)
{
    std::string pattern = "A*;$ENVI;*_";
    std::set<std::string> expected_names;
    KDBEquations* kdb_subset;

    global_ws_lst->load(str_input_test_dir + "fun.al");
    EXPECT_TRUE(global_ws_lst->contains("ENVI"));
    EXPECT_EQ(global_ws_lst->get("ENVI"), "EX;PWMAB;PWMS;PWXAB;PWXS;QWXAB;QWXS;POIL;NATY;TFPFHP_");

    std::set<std::string> all_names;
    for (int p = 0; p < global_ws_eqs->size(); p++) 
        all_names.insert(global_ws_eqs->get_name(p));

    int nb_total_equations = global_ws_eqs->size();
    // A*
    for (const std::string& name : all_names) 
        if (name.front() == 'A') 
            expected_names.insert(name);
    // $ENVI
    unsigned char** c_expanded_list = KL_expand(const_cast<char*>("$ENVI"));
    for (int i = 0; i < SCR_tbl_size(c_expanded_list); i++) 
        expected_names.insert((char*) c_expanded_list[i]);
    // *_
    for (const std::string& name : all_names) 
        if (name.back() == '_') 
            expected_names.insert(name);

    // create a subset (shallow copy)
    kdb_subset = new KDBEquations(global_ws_eqs.get(), pattern, false);
    EXPECT_EQ(kdb_subset->size(), expected_names.size());
    EXPECT_EQ(kdb_subset->get_names(), expected_names);

    // modify an element of the subset and check if the 
    // corresponding element of the global KDB also changes
    std::string name = "ACAF";
    std::string modified_lec = "(ACAF/VAF[-1]) :=acaf2*GOSF[-1]+\nacaf4*(TIME=1995)";
    kdb_subset->update(name, modified_lec);
    EXPECT_EQ(kdb_subset->get_lec(name), modified_lec);
    EXPECT_EQ(global_ws_eqs->get_lec(name), modified_lec);

    // delete an element from the subset and check if it has also 
    // been deleted from the global KDB
    kdb_subset->remove(name);
    EXPECT_FALSE(kdb_subset->contains(name));
    EXPECT_FALSE(global_ws_eqs->contains(name));

    // modify an element of the subset and check if it has also 
    // been added to the global KDB
    std::string lec = "(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)";
    std::string method = "LSQ";
    std::string from = "1980Y1";
    std::string to = "1996Y1";
    std::string comment = "Equation comment";
    std::string block = "ACAF";
    std::string instruments = "Equation instruments";
    bool date = true;
    Equation eq(name, lec, method, from, to, comment, instruments, block, date);
    kdb_subset->add(name, eq);
    EXPECT_EQ(kdb_subset->get_lec(name), lec);
    EXPECT_EQ(global_ws_eqs->get_lec(name), lec);

    // try to add an element to the subset which is already present 
    // in the global KDB
    EXPECT_THROW(kdb_subset->add("BQY", eq), std::invalid_argument);

    // delete subset
    delete kdb_subset;
    EXPECT_EQ(global_ws_eqs->size(), nb_total_equations);
    EXPECT_EQ(global_ws_eqs->get_lec(name), lec);

    // wrong pattern
    pattern = "anjfks";
    EXPECT_THROW(KDBEquations(global_ws_eqs.get(), pattern, false), std::runtime_error);
}

TEST_F(KDBEquationsTest, DeepCopy)
{
    std::string pattern = "A*;$ENVI;*_";
    std::set<std::string> expected_names;
    KDBEquations* kdb_subset;

    global_ws_lst->load(str_input_test_dir + "fun.al");
    EXPECT_TRUE(global_ws_lst->contains("ENVI"));
    EXPECT_EQ(global_ws_lst->get("ENVI"), "EX;PWMAB;PWMS;PWXAB;PWXS;QWXAB;QWXS;POIL;NATY;TFPFHP_");

    std::set<std::string> all_names;
    for (int p = 0; p < global_ws_eqs->size(); p++) 
        all_names.insert(global_ws_eqs->get_name(p));

    int nb_total_equations = global_ws_eqs->size();
    // A*
    for (const std::string& name : all_names) 
        if (name.front() == 'A') expected_names.insert(name);
    // $ENVI
    unsigned char** c_expanded_list = KL_expand(const_cast<char*>("$ENVI"));
    for (int i = 0; i < SCR_tbl_size(c_expanded_list); i++) 
        expected_names.insert((char*) c_expanded_list[i]);
    // *_
    for (const std::string& name : all_names) 
        if (name.back() == '_') 
            expected_names.insert(name);

    // create a subset (deep copy)
    kdb_subset = new KDBEquations(global_ws_eqs.get(), pattern, true);
    EXPECT_EQ(kdb_subset->size(), expected_names.size());
    EXPECT_EQ(kdb_subset->get_names(), expected_names);

    // modify an element of the subset and check if the 
    // corresponding element of the global KDB didn't changed
    std::string name = "ACAF";
    std::string lec = global_ws_eqs->get_lec(name);
    std::string modified_lec = "(ACAF/VAF[-1]) :=acaf2*GOSF[-1]+\nacaf4*(TIME=1995)";
    kdb_subset->update(name, modified_lec);
    EXPECT_EQ(kdb_subset->get_lec(name), modified_lec);
    EXPECT_EQ(global_ws_eqs->get_lec(name), lec);

    // delete an element from the subset and check if it has not 
    // been deleted from the global KDB
    name = "ACAG";
    kdb_subset->remove(name);
    EXPECT_FALSE(kdb_subset->contains(name));
    EXPECT_TRUE(global_ws_eqs->contains(name));

    // delete subset
    delete kdb_subset;
    EXPECT_EQ(global_ws_eqs->size(), nb_total_equations);
}

TEST_F(KDBEquationsTest, CopyFrom)
{
    std::string pattern = "A* *_";
    std::string filename = str_input_test_dir + prefix_filename + "fun.eqs";
    int expected_nb_comments = global_ws_eqs->size();
    std::set<std::string> v_expected_names;

    // Copy entire file
    global_ws_eqs->clear();
    global_ws_eqs->copy_from(filename, "*");
    EXPECT_EQ(global_ws_eqs->size(), expected_nb_comments); 

    // copy subset
    v_expected_names = global_ws_eqs->filter_names(pattern);
    global_ws_eqs->clear();
    global_ws_eqs->copy_from(filename, pattern);
    EXPECT_EQ(global_ws_eqs->size(), v_expected_names.size());  
    EXPECT_EQ(global_ws_eqs->get_names(), v_expected_names);  
}

TEST_F(KDBEquationsTest, Merge)
{
    std::string pattern = "A*";

    // create deep copies kdb
    KDBEquations* kdb0 = new KDBEquations(global_ws_eqs.get(), pattern, true);
    KDBEquations* kdb1 = new KDBEquations(global_ws_eqs.get(), pattern, true);
    KDBEquations* kdb_to_merge = new KDBEquations(global_ws_eqs.get(), pattern, true);

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
    Equation new_eq(new_name, new_lec, method, from, to, comment, instruments, block, date);
    kdb_to_merge->add(new_name, new_eq);

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

    global_ws_cmt->load(str_input_test_dir + "fun.ac");
    global_ws_idt->load(str_input_test_dir + "fun.ai");
    global_ws_lst->load(str_input_test_dir + "fun.al");
    global_ws_scl->load(str_input_test_dir + "fun.as");
    global_ws_tbl->load(str_input_test_dir + "fun.at");
    global_ws_var->load(str_input_test_dir + "fun.av");

    std::vector<std::string> expected_cmts = { eqs_name };
    objs_list = global_ws_cmt->search(eqs_name);
    EXPECT_EQ(objs_list, expected_cmts);

    std::vector<std::string> expected_eqs = { eqs_name, "FLG" };
    objs_list = global_ws_eqs->search(eqs_name);
    EXPECT_EQ(objs_list, expected_eqs);

    objs_list = global_ws_idt->search(eqs_name);
    EXPECT_EQ(objs_list.size(), 0);

    std::vector<std::string> expected_lts = { "COPY0", "ENDO0", "TOTAL0", "_RES" };
    objs_list = global_ws_lst->search(eqs_name);
    EXPECT_EQ(objs_list, expected_lts);

    objs_list = global_ws_scl->search(eqs_name);
    EXPECT_EQ(objs_list.size(), 0);

    std::vector<std::string> expected_tbl = { "GFR", "GFRLEVEL" };
    objs_list = global_ws_tbl->search(eqs_name);
    EXPECT_EQ(objs_list, expected_tbl);

    std::vector<std::string> expected_vars = { eqs_name };
    objs_list = global_ws_var->search(eqs_name);
    EXPECT_EQ(objs_list, expected_vars);
}

TEST_F(KDBEquationsTest, Hash)
{
    Equation* eq = nullptr;
    std::size_t hash_val = hash_value(*global_ws_eqs);

    // modify an entry
    eq = global_ws_eqs->get("ACAF");
    std::string lec = eq->lec;
    eq->set_lec("(ACAF/VAF[-1]) :=acaf2*GOSF[-1]+\nacaf4*(TIME=1995)");
    global_ws_eqs->update("ACAF", *eq);
    std::size_t hash_val_modified = hash_value(*global_ws_eqs);
    EXPECT_NE(hash_val, hash_val_modified);
    // restore original entry
    eq->set_lec(lec);
    global_ws_eqs->update("ACAF", *eq);
    Equation* eq_restored = global_ws_eqs->get("ACAF");
    EXPECT_EQ(*eq_restored, *eq);
    std::size_t hash_val_restored = hash_value(*global_ws_eqs);
    EXPECT_EQ(hash_val, hash_val_restored);
    delete eq;
    delete eq_restored;

    // remove an entry
    eq = global_ws_eqs->get("ACAG");
    global_ws_eqs->remove("ACAG");
    hash_val_modified = hash_value(*global_ws_eqs);
    EXPECT_NE(hash_val, hash_val_modified);
    // restore original entry
    global_ws_eqs->add("ACAG", *eq);
    hash_val_restored = hash_value(*global_ws_eqs);
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
    Equation new_eq("TEST", lec, method, from, to, comment, instruments, block, date);
    global_ws_eqs->add("TEST", new_eq);
    hash_val_modified = hash_value(*global_ws_eqs);
    EXPECT_NE(hash_val, hash_val_modified);  
     
    // remove the entry
    global_ws_eqs->remove("TEST");
    hash_val_restored = hash_value(*global_ws_eqs);
    EXPECT_EQ(hash_val, hash_val_restored);
}
