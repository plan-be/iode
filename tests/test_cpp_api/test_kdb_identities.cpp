#include "pch.h"


class KDBIdentitiesTest : public KDBTest, public ::testing::Test
{
protected:
    void SetUp() override
    {
        KDBIdentities kdb_idt(input_test_dir + "fun.ai");
    }

    // void TearDown() override {}
};


TEST_F(KDBIdentitiesTest, Load)
{
    KDBIdentities kdb(input_test_dir + prefix_filename + "fun.idt");
    EXPECT_EQ(kdb.count(), 48);
}

TEST_F(KDBIdentitiesTest, Subset)
{
    std::string pattern = "A*";
    std::string lec = Identities.get_lec("AOUC");
    std::string new_lec = "((WCRH/QL)/(WCRH/QL)[1990Y1])*(VAFF/(VM+VAFF))[-2]+PM*(VM/(VM+VAFF))[-2]";

    // GLOBAL KDB
    KDBIdentities kdb_global;
    EXPECT_EQ(kdb_global.count(), 48);
    EXPECT_TRUE(kdb_global.is_global_database());

    // DEEP COPY SUBSET
    KDBIdentities* kdb_subset_deep_copy = kdb_global.subset(pattern, true);
    std::vector<std::string> names = kdb_global.get_names(pattern);
    EXPECT_EQ(kdb_subset_deep_copy->count(), names.size());
    EXPECT_TRUE(kdb_subset_deep_copy->is_local_database());
    kdb_subset_deep_copy->update("AOUC", new_lec);
    EXPECT_EQ(kdb_global.get_lec("AOUC"), lec);
    EXPECT_EQ(kdb_subset_deep_copy->get_lec("AOUC"), new_lec);

    // SHALLOW COPY SUBSET
    KDBIdentities* kdb_subset_shallow_copy = kdb_global.subset(pattern, false);
    EXPECT_EQ(kdb_subset_shallow_copy->count(), names.size());
    EXPECT_TRUE(kdb_subset_shallow_copy->is_shallow_copy_database());
    kdb_subset_shallow_copy->update("AOUC", new_lec);
    EXPECT_EQ(kdb_global.get_lec("AOUC"), new_lec);
    EXPECT_EQ(kdb_subset_shallow_copy->get_lec("AOUC"), new_lec);
}

TEST_F(KDBIdentitiesTest, Save)
{
    // save in binary format
    Identities.save(output_test_dir + "fun.idt");

    // save in ascii format
    Identities.save(output_test_dir + "fun.ai");
}

TEST_F(KDBIdentitiesTest, Rename)
{
    int new_pos;
    std::string old_name;
    std::string new_name;

    // rename
    old_name = Identities.get_name(0);
    EXPECT_EQ(old_name, "AOUC");
    new_pos = Identities.rename(old_name, "NEW_NAME");
    EXPECT_EQ(Identities.get_name(new_pos), "NEW_NAME");

    // set by position
    new_pos = Identities.set_name(1, "NEW_POS");
    EXPECT_EQ(Identities.get_name(new_pos), "NEW_POS");
}

TEST_F(KDBIdentitiesTest, GetLec)
{
    int pos = 0;
    std::string name = Identities.get_name(pos);
    std::string lec;
    std::string expected_lec = "((WCRH/QL)/(WCRH/QL)[1990Y1])*(VAFF/(VM+VAFF))[-1]+PM*(VM/\n(VM+VAFF))[-1]";

    // by position
    lec = Identities.get_lec(pos);
    EXPECT_EQ(lec, expected_lec);

    // by name
    lec = Identities.get_lec(name);
    EXPECT_EQ(lec, expected_lec);
}

TEST_F(KDBIdentitiesTest, Get)
{
    int pos = 0;
    std::string name = Identities.get_name(pos);
    CLEC* clec = NULL;
    CLEC* expected_clec = KICLEC(K_WS[IDENTITIES], pos);
    std::string expected_lec = "((WCRH/QL)/(WCRH/QL)[1990Y1])*(VAFF/(VM+VAFF))[-1]+PM*(VM/\n(VM+VAFF))[-1]";

    // by position
    Identity* identity_pos = Identities.get(pos);
    EXPECT_EQ(identity_pos->lec, expected_lec);
    clec = identity_pos->clec;
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
    delete identity_pos;

    // by name
    Identity* identity_name = Identities.get(name);
    EXPECT_EQ(identity_name->lec, expected_lec);
    clec = identity_name->clec;
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
    std::vector<std::string> expected_names;
    for (int i=0; i < Identities.count(); i++) expected_names.push_back(Identities.get_name(i));
    std::vector<std::string> names = Identities.get_names();
    EXPECT_EQ(names, expected_names);
}

TEST_F(KDBIdentitiesTest, CreateRemove)
{
    std::string name = Identities.get_name(0);
    std::string lec = "((WCRH/QL)/(WCRH/QL)[1990Y1])*(VAFF/(VM+VAFF))[-1]+PM*(VM/(VM+VAFF))[-1]";

    Identities.remove(name);
    EXPECT_THROW(Identities.get(name), std::invalid_argument);

    Identities.add(name, lec);
    EXPECT_EQ(Identities.get_lec(name), lec);
}

TEST_F(KDBIdentitiesTest, Update)
{
    std::string name = Identities.get_name(0);
    std::string new_lec = "((WCRH/QL)/(WCRH/QL)[1990Y1])*(VAFF/(VM+VAFF))[-2]+PM*(VM/(VM+VAFF))[-2]";

    Identities.update(name, new_lec);
    EXPECT_EQ(Identities.get_lec(name), new_lec);
}

TEST_F(KDBIdentitiesTest, Copy)
{
    int pos = 0;
    std::string name = Identities.get_name(pos);
    Identity* identity = Identities.get(name);

    Identity* identity_copy = Identities.copy(name);

    EXPECT_EQ(identity_copy->lec, identity->lec);

    CLEC* clec = identity->clec;
    CLEC* clec_copy = identity_copy->clec;
    EXPECT_EQ(clec->tot_lg, clec_copy->tot_lg);
    EXPECT_EQ(clec->exec_lg, clec_copy->exec_lg);
    EXPECT_EQ(clec->nb_names, clec_copy->nb_names);
    EXPECT_EQ(clec->dupendo, clec_copy->dupendo);
    for(int i = 0; i < clec_copy->nb_names; i++)
    {
        EXPECT_EQ(std::string(clec->lnames[i].name), std::string(clec_copy->lnames[i].name));
        EXPECT_EQ(std::string(clec->lnames[i].pad), std::string(clec_copy->lnames[i].pad));
        EXPECT_EQ(clec->lnames[i].pos, clec_copy->lnames[i].pos);
    }
    EXPECT_TRUE(clec_equal(clec, clec_copy));

    // add copy
    Identities.add("DUP_" + name, *identity_copy);

    delete identity;
    delete identity_copy;
}

TEST_F(KDBIdentitiesTest, Filter)
{
    std::string pattern = "A*;*_";
    std::vector<std::string> expected_names;
    KDBIdentities* kdb_subset;

    std::vector<std::string> all_names;
    for (int p = 0; p < Identities.count(); p++) all_names.push_back(Identities.get_name(p));

    int nb_total_identities = Identities.count();
    // A*
    for (const std::string& name : all_names) if (name.front() == 'A') expected_names.push_back(name);
    // *_
    for (const std::string& name : all_names) if (name.back() == '_') expected_names.push_back(name);

     // remove duplicate entries
    // NOTE: std::unique only removes consecutive duplicated elements, 
    //       so the vector needst to be sorted first
    std::sort(expected_names.begin(), expected_names.end());
    std::vector<std::string>::iterator it = std::unique(expected_names.begin(), expected_names.end());  
    expected_names.resize(std::distance(expected_names.begin(), it));

    // create local kdb
    kdb_subset = Identities.subset(pattern);
    EXPECT_EQ(kdb_subset->count(), expected_names.size());
    EXPECT_EQ(kdb_subset->get_names(), expected_names);

    // modify an element of the local KDB and check if the 
    // corresponding element of the global KDB also changes
    std::string name = "AOUC";
    std::string modified_lec = "((WCRH/QL)/(WCRH/QL)[1990Y1])*(VAFF/(VM+VAFF))[-2]+PM*(VM/(VM+VAFF))[-2]";
    kdb_subset->update(name, modified_lec);
    EXPECT_EQ(kdb_subset->get_lec(name), modified_lec);
    EXPECT_EQ(Identities.get_lec(name), modified_lec);

    // add an element to the local KDB and check if it has also 
    // been added to the global KDB
    std::string new_name = "NEW_IDENTITY";
    std::string new_lec = "((WCRH/QL)/(WCRH/QL)[1990Y1])*(VAFF/(VM+VAFF))[-1]+PM*(VM/(VM+VAFF))[-1]";
    kdb_subset->add(new_name, new_lec);
    EXPECT_EQ(kdb_subset->get_lec(new_name), new_lec);
    EXPECT_EQ(Identities.get_lec(new_name), new_lec);

    // rename an element in the local KDB and check if the 
    // corresponding element has also been renamed in the global KDB
    std::string old_name = new_name;
    new_name = "IDENTITY_NEW";
    kdb_subset->rename(old_name, new_name);
    EXPECT_EQ(kdb_subset->get_lec(new_name), new_lec);
    EXPECT_EQ(Identities.get_lec(new_name), new_lec);

    // delete an element from the local KDB and check if it has also 
    // been deleted from the global KDB
    kdb_subset->remove(new_name);
    EXPECT_FALSE(kdb_subset->contains(new_name));
    EXPECT_FALSE(Identities.contains(new_name));

    // try to add an element to the local KDB which is already present 
    // in the global KDB
    EXPECT_THROW(kdb_subset->add("FLGR", new_lec), std::invalid_argument);

    // delete local kdb
    delete kdb_subset;
    EXPECT_EQ(Identities.count(), nb_total_identities);
    EXPECT_EQ(Identities.get_lec(name), modified_lec);

    // wrong pattern
    pattern = "anjfks";
    EXPECT_THROW(Identities.subset(pattern), std::runtime_error);
}

TEST_F(KDBIdentitiesTest, DeepCopy)
{
    std::string pattern = "A*;*_";
    std::vector<std::string> expected_names;
    KDBIdentities* kdb_subset;

    std::vector<std::string> all_names;
    for (int p = 0; p < Identities.count(); p++) all_names.push_back(Identities.get_name(p));

    int nb_total_identities = Identities.count();
    // A*
    for (const std::string& name : all_names) if (name.front() == 'A') expected_names.push_back(name);
    // *_
    for (const std::string& name : all_names) if (name.back() == '_') expected_names.push_back(name);

    // remove duplicate entries
    // NOTE: std::unique only removes consecutive duplicated elements, 
    //       so the vector needst to be sorted first
    std::sort(expected_names.begin(), expected_names.end());
    std::vector<std::string>::iterator it = std::unique(expected_names.begin(), expected_names.end());  
    expected_names.resize(std::distance(expected_names.begin(), it));

    // create local kdb
    kdb_subset = Identities.subset(pattern, true);
    EXPECT_EQ(kdb_subset->count(), expected_names.size());
    EXPECT_EQ(kdb_subset->get_names(), expected_names);

    // modify an element of the local KDB and check if the 
    // corresponding element of the global KDB didn't changed
    std::string name = "AOUC";
    std::string lec = Identities.get_lec(name);
    std::string modified_lec = "((WCRH/QL)/(WCRH/QL)[1990Y1])*(VAFF/(VM+VAFF))[-2]+PM*(VM/(VM+VAFF))[-2]";
    kdb_subset->update(name, modified_lec);
    EXPECT_EQ(kdb_subset->get_lec(name), modified_lec);
    EXPECT_EQ(Identities.get_lec(name), lec);

    // add an element to the local KDB and check if it has not 
    // been added to the global KDB
    std::string new_name = "NEW_IDENTITY";
    std::string new_lec = "((WCRH/QL)/(WCRH/QL)[1990Y1])*(VAFF/(VM+VAFF))[-1]+PM*(VM/(VM+VAFF))[-1]";
    kdb_subset->add(new_name, new_lec);
    EXPECT_TRUE(kdb_subset->contains(new_name));
    EXPECT_EQ(kdb_subset->get_lec(new_name), new_lec);
    EXPECT_FALSE(Identities.contains(new_name));

    // rename an element in the local KDB and check if the 
    // corresponding element has not been renamed in the global KDB
    std::string old_name = "AOUC_";
    new_name = "IDENTITY_NEW";
    kdb_subset->rename(old_name, new_name);
    EXPECT_TRUE(kdb_subset->contains(new_name));
    EXPECT_FALSE(Identities.contains(new_name));

    // delete an element from the local KDB and check if it has not 
    // been deleted from the global KDB
    name = "GAP_";
    kdb_subset->remove(name);
    EXPECT_FALSE(kdb_subset->contains(name));
    EXPECT_TRUE(Identities.contains(name));

    // delete local kdb
    delete kdb_subset;
    EXPECT_EQ(Identities.count(), nb_total_identities);

    // wrong pattern
    pattern = "anjfks";
    EXPECT_THROW(Identities.subset(pattern, true), std::runtime_error);
}

// QUESTION FOR JMP: How to test with variables file, scalars file and trace ?
TEST_F(KDBIdentitiesTest, ExecuteIdentities)
{
    int y_from = 1991;
    int y_to = 2000;
    // GAP2 "100*(QAFF_/(Q_F+Q_I))"
    // GAP_ "100*((QAF_/Q_F)-1)"
    std::string identities_list = "GAP2;GAP_";

    KDBVariables kdb_var(input_test_dir + "fun.av");

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
        kdb_var.set_var("GAP2", period, 0.0);
        kdb_var.set_var("GAP_", period, 0.0);
        // compute values of GAP2 and GAP_
        qaff_ = kdb_var.get_var("QAFF_", period);
        qaf_ = kdb_var.get_var("QAF_", period);
        q_f = kdb_var.get_var("Q_F", period);
        q_i = kdb_var.get_var("Q_I", period);
        expected_gap2.push_back(100.0 * (qaff_ / (q_f + q_i)));
        expected_gap_.push_back(100.0 * ((qaf_ / q_f) - 1.0));
    }

    // compute GAP2 and GAP_
    Identities.execute_identities(std::to_string(y_from)+"Y1", std::to_string(y_to)+"Y1", identities_list);

    Variable computed_gap2;
    Variable computed_gap_;
    computed_gap2.reserve(10);
    computed_gap_.reserve(10);
    for (int y=y_from; y <= y_to; y++)
    {
        period = std::to_string(y) + "Y1";
        computed_gap2.push_back(kdb_var.get_var("GAP2", period));
        computed_gap_.push_back(kdb_var.get_var("GAP_", period));
    }

    EXPECT_DOUBLE_EQ(computed_gap2[0], expected_gap2[0]);
    EXPECT_DOUBLE_EQ(computed_gap_[0], expected_gap_[0]);

    EXPECT_DOUBLE_EQ(computed_gap2[4], expected_gap2[4]);
    EXPECT_DOUBLE_EQ(computed_gap_[4], expected_gap_[4]);

    EXPECT_DOUBLE_EQ(computed_gap2[9], expected_gap2[9]);
    EXPECT_DOUBLE_EQ(computed_gap_[9], expected_gap_[9]);

    // Error
    Variables.clear();
    EXPECT_THROW(Identities.execute_identities(std::to_string(y_from)+"Y1", std::to_string(y_to)+"Y1", 
                 identities_list), std::runtime_error);
}

TEST_F(KDBIdentitiesTest, CopyFrom)
{
    std::string pattern = "A* *_";
    std::string filename = input_test_dir + prefix_filename + "fun.idt";
    int expected_nb_comments = Identities.count();
    std::vector<std::string> v_expected_names;

    // Copy entire file
    Identities.clear();
    Identities.copy_from(filename, "*");
    EXPECT_EQ(Identities.count(), expected_nb_comments); 

    // copy subset
    v_expected_names = Identities.get_names(pattern);
    Identities.clear();
    Identities.copy_from(filename, pattern);
    EXPECT_EQ(Identities.count(), v_expected_names.size());  
    EXPECT_EQ(Identities.get_names(), v_expected_names);  
}

TEST_F(KDBIdentitiesTest, Merge)
{
    std::string pattern = "A*";

    // create deep copies kdb
    KDBIdentities* kdb0 = Identities.subset(pattern, true);
    KDBIdentities* kdb1 = Identities.subset(pattern, true);
    KDBIdentities* kdb_to_merge = Identities.subset(pattern, true);

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

    KDBComments kdb_cmt(input_test_dir + "fun.ac");
    KDBEquations kdb_eqs(input_test_dir + "fun.ae");
    KDBLists kdb_lst(input_test_dir + "fun.al");
    KDBScalars kdb_scl(input_test_dir + "fun.as");
    KDBTables kdb_tbl(input_test_dir + "fun.at");
    KDBVariables kdb_var(input_test_dir + "fun.av");

    objs_list = Comments.search(idt_name);
    EXPECT_EQ(objs_list.size(), 0);

    std::vector<std::string> expected_eqs = { idt_name };
    objs_list = Equations.search(idt_name);
    EXPECT_EQ(objs_list, expected_eqs);

    std::vector<std::string> expected_idt = { idt_name };
    objs_list = Identities.search(idt_name);
    EXPECT_EQ(objs_list, expected_idt);

    std::vector<std::string> expected_lts = { "ENDO0", "IDT", "TOTAL0", "_RES" };
    objs_list = Lists.search(idt_name);
    EXPECT_EQ(objs_list, expected_lts);

    objs_list = Scalars.search(idt_name);
    EXPECT_EQ(objs_list.size(), 0);

    std::vector<std::string> expected_tbl = { "ANAKNFF", "C8_4", "MULT2FR", "MULT2RESU", "T2" };
    objs_list = Tables.search(idt_name);
    EXPECT_EQ(objs_list, expected_tbl);

    std::vector<std::string> expected_vars = { idt_name };
    objs_list = Variables.search(idt_name);
    EXPECT_EQ(objs_list, expected_vars);
}

TEST_F(KDBIdentitiesTest, Hash)
{
    std::size_t hash_val = hash_value(Identities);

    // change a name
    Identities.rename("AOUC", "NEW_NAME");
    std::size_t hash_val_modified = hash_value(Identities);
    EXPECT_NE(hash_val, hash_val_modified);
    std::cout << "(rename identity) orignal vs modified hash: " << std::to_string(hash_val) << " vs " << std::to_string(hash_val_modified) << std::endl;

    // modify an entry
    hash_val = hash_val_modified;
    std::string new_lec = "((WCRH/QL)/(WCRH/QL)[1990Y1])*(VAFF/(VM+VAFF))[-2]+PM*(VM/(VM+VAFF))[-2]";
    Identities.update("NEW_NAME", new_lec);
    hash_val_modified = hash_value(Identities);
    EXPECT_NE(hash_val, hash_val_modified);
    std::cout << "(modify identity) orignal vs modified hash: " << std::to_string(hash_val) << " vs " << std::to_string(hash_val_modified) << std::endl;

    // remove an entry
    hash_val = hash_val_modified;
    Identities.remove("NEW_NAME");
    hash_val_modified = hash_value(Identities);
    EXPECT_NE(hash_val, hash_val_modified);
    std::cout << "(delete identity) orignal vs modified hash: " << std::to_string(hash_val) << " vs " << std::to_string(hash_val_modified) << std::endl;

    // add an entry
    hash_val = hash_val_modified;
    std::string lec = "((WCRH/QL)/(WCRH/QL)[1990Y1])*(VAFF/(VM+VAFF))[-1]+PM*(VM/(VM+VAFF))[-1]";
    Identities.add("NEW_ENTRY", lec);
    hash_val_modified = hash_value(Identities);
    EXPECT_NE(hash_val, hash_val_modified);   
    std::cout << "(new    identity) orignal vs modified hash: " << std::to_string(hash_val) << " vs " << std::to_string(hash_val_modified) << std::endl; 
}
