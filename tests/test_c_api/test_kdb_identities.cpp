#include "pch.h"


class KDBIdentitiesTest : public TestAbstract, public ::testing::Test
{
protected:
    void SetUp() override
    {
        global_ws_idt->load(str_input_test_dir + "fun.ai");
    }

    // void TearDown() override {}
};


TEST_F(KDBIdentitiesTest, Load)
{
    KDBIdentities kdb(false);
    kdb.load(str_input_test_dir + prefix_filename + "fun.idt");
    EXPECT_EQ(kdb.size(), 48);
}

TEST_F(KDBIdentitiesTest, Subset)
{
    std::string pattern = "A*";
    std::string lec = global_ws_idt->get_lec("AOUC");
    std::string new_lec = "((WCRH/QL)/(WCRH/QL)[1990Y1])*(VAFF/(VM+VAFF))[-2]+PM*(VM/(VM+VAFF))[-2]";

    // GLOBAL KDB
    EXPECT_EQ(global_ws_idt->size(), 48);
    EXPECT_TRUE(global_ws_idt->is_global_database());
    std::set<std::string> names = global_ws_idt->filter_names(pattern);

    // DEEP COPY SUBSET
    KDBIdentities* kdb_subset_deep_copy = new KDBIdentities(global_ws_idt.get(), pattern, true);
    EXPECT_EQ(kdb_subset_deep_copy->size(), names.size());
    EXPECT_TRUE(kdb_subset_deep_copy->is_local_database());
    kdb_subset_deep_copy->update("AOUC", new_lec);
    EXPECT_EQ(global_ws_idt->get_lec("AOUC"), lec);
    EXPECT_EQ(kdb_subset_deep_copy->get_lec("AOUC"), new_lec);

    // SHALLOW COPY SUBSET
    KDBIdentities* kdb_subset_shallow_copy = new KDBIdentities(global_ws_idt.get(), pattern, false);
    EXPECT_EQ(kdb_subset_shallow_copy->size(), names.size());
    EXPECT_TRUE(kdb_subset_shallow_copy->is_shallow_copy_database());
    kdb_subset_shallow_copy->update("AOUC", new_lec);
    EXPECT_EQ(global_ws_idt->get_lec("AOUC"), new_lec);
    EXPECT_EQ(kdb_subset_shallow_copy->get_lec("AOUC"), new_lec);
}

TEST_F(KDBIdentitiesTest, Save)
{
    // save in binary format
    global_ws_idt->save(str_output_test_dir + "fun.idt");

    // save in ascii format
    global_ws_idt->save(str_output_test_dir + "fun.ai");
}

TEST_F(KDBIdentitiesTest, Rename)
{
    bool success;
    std::string old_name;
    std::string new_name;

    // rename
    old_name = global_ws_idt->get_name(0);
    EXPECT_EQ(old_name, "AOUC");
    success = global_ws_idt->rename(old_name, "NEW_NAME");
    EXPECT_TRUE(success);
    EXPECT_TRUE(global_ws_idt->contains("NEW_NAME"));
}

TEST_F(KDBIdentitiesTest, GetLec)
{
    std::string expected_lec = "((WCRH/QL)/(WCRH/QL)[1990Y1])*(VAFF/(VM+VAFF))[-1]+PM*(VM/\n(VM+VAFF))[-1]";
    std::string lec = global_ws_idt->get_lec("AOUC");
    EXPECT_EQ(lec, expected_lec);
}

TEST_F(KDBIdentitiesTest, Get)
{
    std::string name = "AOUC";
    CLEC* clec = NULL;
    CLEC* expected_clec = global_ws_idt->get_obj(name)->get_compiled_lec();
    std::string expected_lec = "((WCRH/QL)/(WCRH/QL)[1990Y1])*(VAFF/(VM+VAFF))[-1]+PM*(VM/\n(VM+VAFF))[-1]";

    // by name
    Identity* identity_name = global_ws_idt->get(name);
    EXPECT_EQ(identity_name->get_lec(), expected_lec);
    clec = identity_name->get_compiled_lec();
    EXPECT_EQ(clec->tot_lg, expected_clec->tot_lg);
    EXPECT_EQ(clec->exec_lg, expected_clec->exec_lg);
    EXPECT_EQ(clec->nb_names, expected_clec->nb_names);
    EXPECT_EQ(clec->dupendo, expected_clec->dupendo);
    for(int i = 0; i < expected_clec->nb_names; i++)
    {
        EXPECT_EQ(std::string(clec->lnames[i].name), std::string(expected_clec->lnames[i].name));
        EXPECT_EQ(std::string(clec->lnames[i].pad), std::string(expected_clec->lnames[i].pad));
        EXPECT_EQ(clec->lnames[i].pos, expected_clec->lnames[i].pos);
    }
    EXPECT_TRUE(clec_equal(clec, expected_clec));
    delete identity_name;
}

TEST_F(KDBIdentitiesTest, GetNames)
{
    std::set<std::string> expected_names;
    for(int i=0; i < global_ws_idt->size(); i++) 
        expected_names.insert(global_ws_idt->get_name(i));
    std::set<std::string> names = global_ws_idt->get_names();
    EXPECT_EQ(names, expected_names);
}

TEST_F(KDBIdentitiesTest, CreateRemove)
{
    std::string name = global_ws_idt->get_name(0);
    std::string lec = "((WCRH/QL)/(WCRH/QL)[1990Y1])*(VAFF/(VM+VAFF))[-1]+PM*(VM/(VM+VAFF))[-1]";

    global_ws_idt->remove(name);
    EXPECT_THROW(global_ws_idt->get(name), std::invalid_argument);

    global_ws_idt->add(name, lec);
    EXPECT_EQ(global_ws_idt->get_lec(name), lec);
}

TEST_F(KDBIdentitiesTest, Update)
{
    std::string name = global_ws_idt->get_name(0);
    std::string new_lec = "((WCRH/QL)/(WCRH/QL)[1990Y1])*(VAFF/(VM+VAFF))[-2]+PM*(VM/(VM+VAFF))[-2]";

    global_ws_idt->update(name, new_lec);
    EXPECT_EQ(global_ws_idt->get_lec(name), new_lec);
}

TEST_F(KDBIdentitiesTest, Filter)
{
    std::string pattern = "A*;*_";
    std::set<std::string> expected_names;
    KDBIdentities* kdb_subset;

    std::set<std::string> all_names;
    for (int p = 0; p < global_ws_idt->size(); p++) 
        all_names.insert(global_ws_idt->get_name(p));

    int nb_total_identities = global_ws_idt->size();
    // A*
    for (const std::string& name : all_names) 
        if (name.front() == 'A') 
            expected_names.insert(name);
    // *_
    for (const std::string& name : all_names) 
        if (name.back() == '_') 
            expected_names.insert(name);

    // create a subset (shallow copy)
    kdb_subset = new KDBIdentities(global_ws_idt.get(), pattern, false);
    EXPECT_EQ(kdb_subset->size(), expected_names.size());
    EXPECT_EQ(kdb_subset->get_names(), expected_names);

    // modify an element of the subset and check if the 
    // corresponding element of the global KDB also changes
    std::string name = "AOUC";
    std::string modified_lec = "((WCRH/QL)/(WCRH/QL)[1990Y1])*(VAFF/(VM+VAFF))[-2]+PM*(VM/(VM+VAFF))[-2]";
    kdb_subset->update(name, modified_lec);
    EXPECT_EQ(kdb_subset->get_lec(name), modified_lec);
    EXPECT_EQ(global_ws_idt->get_lec(name), modified_lec);

    // modify an element of the subset and check if it has also 
    // been added to the global KDB
    std::string new_name = "NEW_IDENTITY";
    std::string new_lec = "((WCRH/QL)/(WCRH/QL)[1990Y1])*(VAFF/(VM+VAFF))[-1]+PM*(VM/(VM+VAFF))[-1]";
    kdb_subset->add(new_name, new_lec);
    EXPECT_EQ(kdb_subset->get_lec(new_name), new_lec);
    EXPECT_EQ(global_ws_idt->get_lec(new_name), new_lec);

    // modify an element of the subset and check if the 
    // corresponding element has also been renamed in the global KDB
    std::string old_name = new_name;
    new_name = "IDENTITY_NEW";
    kdb_subset->rename(old_name, new_name);
    EXPECT_EQ(kdb_subset->get_lec(new_name), new_lec);
    EXPECT_EQ(global_ws_idt->get_lec(new_name), new_lec);

    // delete an element from the subset and check if it has also 
    // been deleted from the global KDB
    kdb_subset->remove(new_name);
    EXPECT_FALSE(kdb_subset->contains(new_name));
    EXPECT_FALSE(global_ws_idt->contains(new_name));

    // try to add an element to the subset which is already present 
    // in the global KDB
    EXPECT_THROW(kdb_subset->add("FLGR", new_lec), std::invalid_argument);

    // delete subset
    delete kdb_subset;
    EXPECT_EQ(global_ws_idt->size(), nb_total_identities);
    EXPECT_EQ(global_ws_idt->get_lec(name), modified_lec);

    // wrong pattern
    pattern = "anjfks";
    EXPECT_THROW(KDBIdentities(global_ws_idt.get(), pattern, false), std::runtime_error);
}

TEST_F(KDBIdentitiesTest, DeepCopy)
{
    std::string pattern = "A*;*_";
    std::set<std::string> expected_names;
    KDBIdentities* kdb_subset;

    std::set<std::string> all_names;
    for (int p = 0; p < global_ws_idt->size(); p++) 
        all_names.insert(global_ws_idt->get_name(p));

    int nb_total_identities = global_ws_idt->size();
    // A*
    for (const std::string& name : all_names) 
        if (name.front() == 'A') 
            expected_names.insert(name);
    // *_
    for (const std::string& name : all_names) 
        if (name.back() == '_') 
            expected_names.insert(name);

    // create a subset (deep copy)
    kdb_subset = new KDBIdentities(global_ws_idt.get(), pattern, true);
    EXPECT_EQ(kdb_subset->size(), expected_names.size());
    EXPECT_EQ(kdb_subset->get_names(), expected_names);

    // modify an element of the subset and check if the 
    // corresponding element of the global KDB didn't changed
    std::string name = "AOUC";
    std::string lec = global_ws_idt->get_lec(name);
    std::string modified_lec = "((WCRH/QL)/(WCRH/QL)[1990Y1])*(VAFF/(VM+VAFF))[-2]+PM*(VM/(VM+VAFF))[-2]";
    kdb_subset->update(name, modified_lec);
    EXPECT_EQ(kdb_subset->get_lec(name), modified_lec);
    EXPECT_EQ(global_ws_idt->get_lec(name), lec);

    // modify an element of the subset and check if it has not 
    // been added to the global KDB
    std::string new_name = "NEW_IDENTITY";
    std::string new_lec = "((WCRH/QL)/(WCRH/QL)[1990Y1])*(VAFF/(VM+VAFF))[-1]+PM*(VM/(VM+VAFF))[-1]";
    kdb_subset->add(new_name, new_lec);
    EXPECT_TRUE(kdb_subset->contains(new_name));
    EXPECT_EQ(kdb_subset->get_lec(new_name), new_lec);
    EXPECT_FALSE(global_ws_idt->contains(new_name));

    // modify an element of the subset and check if the 
    // corresponding element has not been renamed in the global KDB
    std::string old_name = "AOUC_";
    new_name = "IDENTITY_NEW";
    kdb_subset->rename(old_name, new_name);
    EXPECT_TRUE(kdb_subset->contains(new_name));
    EXPECT_FALSE(global_ws_idt->contains(new_name));

    // delete an element from the subset and check if it has not 
    // been deleted from the global KDB
    name = "GAP_";
    kdb_subset->remove(name);
    EXPECT_FALSE(kdb_subset->contains(name));
    EXPECT_TRUE(global_ws_idt->contains(name));

    // delete subset
    delete kdb_subset;
    EXPECT_EQ(global_ws_idt->size(), nb_total_identities);
}

// QUESTION FOR JMP: How to test with variables file, scalars file and trace ?
TEST_F(KDBIdentitiesTest, ExecuteIdentities)
{
    bool success;
    int y_from = 1991;
    int y_to = 2000;
    // GAP2 "100*(QAFF_/(Q_F+Q_I))"
    // GAP_ "100*((QAF_/Q_F)-1)"
    std::string identities_list = "GAP2;GAP_";

    global_ws_var->load(str_input_test_dir + "fun.av");

    std::string period;
    Variable expected_gap2;
    Variable expected_gap_;
    expected_gap2.reserve(10);
    expected_gap_.reserve(10);
    double qaff_;
    double qaf_;
    double q_f;
    double q_i;
    for (int y=y_from; y <= y_to; y++)
    {
        period = std::to_string(y) + "Y1";
        // reset GAP2 and GAP_
        global_ws_var->set_var("GAP2", period, 0.0);
        global_ws_var->set_var("GAP_", period, 0.0);
        // compute values of GAP2 and GAP_
        qaff_ = global_ws_var->get_var("QAFF_", period);
        qaf_ = global_ws_var->get_var("QAF_", period);
        q_f = global_ws_var->get_var("Q_F", period);
        q_i = global_ws_var->get_var("Q_I", period);
        expected_gap2.push_back(100.0 * (qaff_ / (q_f + q_i)));
        expected_gap_.push_back(100.0 * ((qaf_ / q_f) - 1.0));
    }

    // compute GAP2 and GAP_
    success = global_ws_idt->execute_identities(std::to_string(y_from)+"Y1", 
                std::to_string(y_to)+"Y1", identities_list);
    EXPECT_TRUE(success);

    Variable computed_gap2;
    Variable computed_gap_;
    computed_gap2.reserve(10);
    computed_gap_.reserve(10);
    for (int y=y_from; y <= y_to; y++)
    {
        period = std::to_string(y) + "Y1";
        computed_gap2.push_back(global_ws_var->get_var("GAP2", period));
        computed_gap_.push_back(global_ws_var->get_var("GAP_", period));
    }

    EXPECT_DOUBLE_EQ(computed_gap2[0], expected_gap2[0]);
    EXPECT_DOUBLE_EQ(computed_gap_[0], expected_gap_[0]);

    EXPECT_DOUBLE_EQ(computed_gap2[4], expected_gap2[4]);
    EXPECT_DOUBLE_EQ(computed_gap_[4], expected_gap_[4]);

    EXPECT_DOUBLE_EQ(computed_gap2[9], expected_gap2[9]);
    EXPECT_DOUBLE_EQ(computed_gap_[9], expected_gap_[9]);

    // Error -> empty Variables KDB
    global_ws_var->clear();
    EXPECT_TRUE(global_ws_var->get_sample() == nullptr);
    EXPECT_EQ(global_ws_var->size(), 0);
    EXPECT_EQ(global_ws_var->get_filename_utf8(), std::string(I_DEFAULT_FILENAME));
    success = global_ws_idt->execute_identities(std::to_string(y_from) + "Y1", 
                         std::to_string(y_to) + "Y1", identities_list);
    EXPECT_FALSE(success);
}

TEST_F(KDBIdentitiesTest, CopyFrom)
{
    std::string pattern = "A* *_";
    std::string filename = str_input_test_dir + prefix_filename + "fun.idt";
    int expected_nb_comments = global_ws_idt->size();
    std::set<std::string> v_expected_names;

    // Copy entire file
    global_ws_idt->clear();
    global_ws_idt->copy_from(filename, "*");
    EXPECT_EQ(global_ws_idt->size(), expected_nb_comments); 

    // copy subset
    v_expected_names = global_ws_idt->filter_names(pattern);
    global_ws_idt->clear();
    global_ws_idt->copy_from(filename, pattern);
    EXPECT_EQ(global_ws_idt->size(), v_expected_names.size());  
    EXPECT_EQ(global_ws_idt->get_names(), v_expected_names);  
}

TEST_F(KDBIdentitiesTest, Merge)
{
    std::string pattern = "A*";

    // create deep copies kdb
    KDBIdentities* kdb0 = new KDBIdentities(global_ws_idt.get(), pattern, true);
    KDBIdentities* kdb1 = new KDBIdentities(global_ws_idt.get(), pattern, true);
    KDBIdentities* kdb_to_merge = new KDBIdentities(global_ws_idt.get(), pattern, true);

    // add an element to the KDB to be merged
    std::string new_name = "NEW_IDENTITY";
    std::string new_lec = "((WCRH/QL)/(WCRH/QL)[1990Y1])*(VAFF/(VM+VAFF))[-1]+PM*(VM/(VM+VAFF))[-1]";
    kdb_to_merge->add(new_name, new_lec);

    // modify an existing element of the KDB to be merge
    std::string name = "AOUC";
    std::string unmodified_lec = kdb_to_merge->get_lec(name);
    std::string modified_lec = "((WCRH/QL)/(WCRH/QL)[1990Y1])*(VAFF/(VM+VAFF))[-2]+PM*(VM/(VM+VAFF))[-2]";
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

TEST_F(KDBIdentitiesTest, Search)
{
    std::string idt_name = "RENT";
    std::vector<std::string> objs_list;

    global_ws_cmt->load(str_input_test_dir + "fun.ac");
    global_ws_eqs->load(str_input_test_dir + "fun.ae");
    global_ws_lst->load(str_input_test_dir + "fun.al");
    global_ws_scl->load(str_input_test_dir + "fun.as");
    global_ws_tbl->load(str_input_test_dir + "fun.at");
    global_ws_var->load(str_input_test_dir + "fun.av");

    objs_list = global_ws_cmt->search(idt_name);
    EXPECT_EQ(objs_list.size(), 0);

    std::vector<std::string> expected_eqs = { idt_name };
    objs_list = global_ws_eqs->search(idt_name);
    EXPECT_EQ(objs_list, expected_eqs);

    std::vector<std::string> expected_idt = { idt_name };
    objs_list = global_ws_idt->search(idt_name);
    EXPECT_EQ(objs_list, expected_idt);

    std::vector<std::string> expected_lts = { "ENDO0", "IDT", "TOTAL0", "_RES" };
    objs_list = global_ws_lst->search(idt_name);
    EXPECT_EQ(objs_list, expected_lts);

    objs_list = global_ws_scl->search(idt_name);
    EXPECT_EQ(objs_list.size(), 0);

    std::vector<std::string> expected_tbl = { "ANAKNFF", "C8_4", "MULT2FR", "MULT2RESU", "T2" };
    objs_list = global_ws_tbl->search(idt_name);
    EXPECT_EQ(objs_list, expected_tbl);

    std::vector<std::string> expected_vars = { idt_name };
    objs_list = global_ws_var->search(idt_name);
    EXPECT_EQ(objs_list, expected_vars);
}

TEST_F(KDBIdentitiesTest, Hash)
{
    std::size_t hash_val = hash_value(*global_ws_idt);

    // change a name
    global_ws_idt->rename("AOUC", "NEW_NAME");
    std::size_t hash_val_modified = hash_value(*global_ws_idt);
    EXPECT_NE(hash_val, hash_val_modified);
    std::cout << "(rename identity) orignal vs modified hash: " << std::to_string(hash_val) << " vs " << std::to_string(hash_val_modified) << std::endl;

    // modify an entry
    hash_val = hash_val_modified;
    std::string new_lec = "((WCRH/QL)/(WCRH/QL)[1990Y1])*(VAFF/(VM+VAFF))[-2]+PM*(VM/(VM+VAFF))[-2]";
    global_ws_idt->update("NEW_NAME", new_lec);
    hash_val_modified = hash_value(*global_ws_idt);
    EXPECT_NE(hash_val, hash_val_modified);
    std::cout << "(modify identity) orignal vs modified hash: " << std::to_string(hash_val) << " vs " << std::to_string(hash_val_modified) << std::endl;

    // remove an entry
    hash_val = hash_val_modified;
    global_ws_idt->remove("NEW_NAME");
    hash_val_modified = hash_value(*global_ws_idt);
    EXPECT_NE(hash_val, hash_val_modified);
    std::cout << "(delete identity) orignal vs modified hash: " << std::to_string(hash_val) << " vs " << std::to_string(hash_val_modified) << std::endl;

    // add an entry
    hash_val = hash_val_modified;
    std::string lec = "((WCRH/QL)/(WCRH/QL)[1990Y1])*(VAFF/(VM+VAFF))[-1]+PM*(VM/(VM+VAFF))[-1]";
    global_ws_idt->add("NEW_ENTRY", lec);
    hash_val_modified = hash_value(*global_ws_idt);
    EXPECT_NE(hash_val, hash_val_modified);   
    std::cout << "(new    identity) orignal vs modified hash: " << std::to_string(hash_val) << " vs " << std::to_string(hash_val_modified) << std::endl; 
}
