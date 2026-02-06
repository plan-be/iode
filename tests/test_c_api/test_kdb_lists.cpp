#include "pch.h"


class KDBListsTest : public TestAbstract, public ::testing::Test
{
protected:
    void SetUp() override
    {
        global_ws_lst->load(str_input_test_dir + "fun.al");
    }

    void TearDown() override 
    {
        global_ws_lst->clear();
    }
};


TEST_F(KDBListsTest, Load)
{
    KDBLists kdb(false);
    kdb.load(str_input_test_dir + prefix_filename + "fun.lst");
    EXPECT_EQ(kdb.size(), 17);
}

TEST_F(KDBListsTest, Subset)
{
    std::string pattern = "C*";
    std::string list = global_ws_lst->get("COPY");
    std::string new_list = "ACAF;ACAG;AOUC;AQC";

    // GLOBAL KDB
    EXPECT_EQ(global_ws_lst->size(), 17);
    EXPECT_TRUE(global_ws_lst->is_global_database());
    std::set<std::string> names = global_ws_lst->filter_names(pattern);

    // DEEP COPY SUBSET
    KDBLists* kdb_subset_deep_copy = new KDBLists(global_ws_lst.get(), pattern, true);
    EXPECT_EQ(kdb_subset_deep_copy->size(), names.size());
    EXPECT_TRUE(kdb_subset_deep_copy->is_detached_database());
    kdb_subset_deep_copy->update("COPY", new_list);
    EXPECT_EQ(global_ws_lst->get("COPY"), list);
    EXPECT_EQ(kdb_subset_deep_copy->get("COPY"), new_list);

    // SHALLOW COPY SUBSET
    KDBLists* kdb_subset_shallow_copy = new KDBLists(global_ws_lst.get(), pattern, false);
    EXPECT_EQ(kdb_subset_shallow_copy->size(), names.size());
    EXPECT_TRUE(kdb_subset_shallow_copy->is_subset_database());
    kdb_subset_shallow_copy->update("COPY", new_list);
    EXPECT_EQ(global_ws_lst->get("COPY"), new_list);
    EXPECT_EQ(kdb_subset_shallow_copy->get("COPY"), new_list);
}

TEST_F(KDBListsTest, Save)
{
    // save in binary format
    global_ws_lst->save(str_output_test_dir + "fun.lst");

    // save in ascii format
    global_ws_lst->save(str_output_test_dir + "fun.al");
}

TEST_F(KDBListsTest, Get)
{
    std::string expected_list = "$COPY0;$COPY1;";
    std::string list = global_ws_lst->get("COPY");
    EXPECT_EQ(expected_list, list);
}

TEST_F(KDBListsTest, GetNames)
{
    std::set<std::string> expected_names;
    for (int i=0; i < global_ws_lst->size(); i++) 
        expected_names.insert(global_ws_lst->get_name(i));
    std::set<std::string> names = global_ws_lst->get_names();
    EXPECT_EQ(names, expected_names);
}

TEST_F(KDBListsTest, CreateRemove)
{
    std::string name = "A_COMMENTS";
    std::string new_list = "ACAF;ACAG;AOUC;AQC";

    global_ws_lst->add(name, new_list);
    ASSERT_EQ(global_ws_lst->get(name), new_list);

    global_ws_lst->remove(name);
    EXPECT_THROW(global_ws_lst->get(name), std::invalid_argument);
}

TEST_F(KDBListsTest, Update)
{
    std::string expanded_list = global_ws_lst->get("COPY0") + global_ws_lst->get("COPY1");

    global_ws_lst->update("COPY", expanded_list);
    EXPECT_EQ(global_ws_lst->get("COPY"), expanded_list);
}

TEST_F(KDBListsTest, Filter)
{
    std::string pattern = "C*";
    std::set<std::string> expected_names;
    KDBLists* kdb_subset;

    std::set<std::string> all_names;
    for (int p = 0; p < global_ws_lst->size(); p++) 
        all_names.insert(global_ws_lst->get_name(p));
    for (const std::string& name : all_names) 
        if (name.front() == 'C') 
            expected_names.insert(name);

    int nb_total_lists = global_ws_lst->size();

    // create a subset (shallow copy)
    kdb_subset = new KDBLists(global_ws_lst.get(), pattern, false);
    EXPECT_EQ(kdb_subset->size(), expected_names.size());
    EXPECT_EQ(kdb_subset->get_names(), expected_names);

    // modify an element of the subset and check if the 
    // corresponding element of the global KDB also changes
    std::string name = "COPY";
    std::string expanded_list = global_ws_lst->get("COPY0") + global_ws_lst->get("COPY1");
    kdb_subset->update(name, expanded_list);
    EXPECT_EQ(kdb_subset->get(name), expanded_list);
    EXPECT_EQ(global_ws_lst->get(name), expanded_list);

    // modify an element of the subset and check if it has also 
    // been added to the global KDB
    std::string new_name = "NEW_LIST";
    std::string new_list = "ACAF;ACAG;AOUC;AQC";
    kdb_subset->add(new_name, new_list);
    EXPECT_EQ(kdb_subset->get(new_name), new_list);
    EXPECT_EQ(global_ws_lst->get(new_name), new_list);

    // modify an element of the subset and check if the 
    // corresponding element has also been renamed in the global KDB
    std::string old_name = new_name;
    new_name = "LIST_NEW";
    kdb_subset->rename(old_name, new_name);
    EXPECT_EQ(kdb_subset->get(new_name), new_list);
    EXPECT_EQ(global_ws_lst->get(new_name), new_list);

    // delete an element from the subset and check if it has also 
    // been deleted from the global KDB
    kdb_subset->remove(new_name);
    EXPECT_FALSE(kdb_subset->contains(new_name));
    EXPECT_FALSE(global_ws_lst->contains(new_name));

    // try to add an element to the subset which is already present 
    // in the global KDB
    EXPECT_THROW(kdb_subset->add("ENDO", new_list), std::invalid_argument);

    // delete subset
    delete kdb_subset;
    EXPECT_EQ(global_ws_lst->size(), nb_total_lists);
    EXPECT_EQ(global_ws_lst->get(name), expanded_list);

    // wrong pattern
    pattern = "anjfks";
    EXPECT_THROW(KDBLists(global_ws_lst.get(), pattern, false), std::runtime_error);
}

TEST_F(KDBListsTest, DeepCopy)
{
    std::string pattern = "C*";
    std::set<std::string> expected_names;
    KDBLists* kdb_subset;

    std::set<std::string> all_names;
    for (int p = 0; p < global_ws_lst->size(); p++) 
        all_names.insert(global_ws_lst->get_name(p));
    for (const std::string& name : all_names) 
        if (name.front() == 'C') 
            expected_names.insert(name);

    int nb_total_lists = global_ws_lst->size();

    // create a subset (deep copy)
    kdb_subset = new KDBLists(global_ws_lst.get(), pattern, true);
    EXPECT_EQ(kdb_subset->size(), expected_names.size());
    EXPECT_EQ(kdb_subset->get_names(), expected_names);

    // modify an element of the subset and check if the 
    // corresponding element of the global KDB didn't changed
    std::string name = "COPY";
    std::string list = global_ws_lst->get(name);
    std::string expanded_list = global_ws_lst->get("COPY0") + global_ws_lst->get("COPY1");
    kdb_subset->update(name, expanded_list);
    EXPECT_EQ(kdb_subset->get(name), expanded_list);
    EXPECT_EQ(global_ws_lst->get(name), list);

    // modify an element of the subset and check if it has not 
    // been added to the global KDB
    std::string new_name = "NEW_LIST";
    std::string new_list = "ACAF;ACAG;AOUC;AQC";
    kdb_subset->add(new_name, new_list);
    EXPECT_TRUE(kdb_subset->contains(new_name));
    EXPECT_EQ(kdb_subset->get(new_name), new_list);
    EXPECT_FALSE(global_ws_lst->contains(new_name));

    // modify an element of the subset and check if the 
    // corresponding element has not been renamed in the global KDB
    name = "COPY0";
    new_name = "LIST_NEW";
    kdb_subset->rename(name, new_name);
    EXPECT_TRUE(kdb_subset->contains(new_name));
    EXPECT_FALSE(global_ws_lst->contains(new_name));

    // delete an element from the subset and check if it has not 
    // been deleted from the global KDB
    name = "COPY1";
    kdb_subset->remove(name);
    EXPECT_FALSE(kdb_subset->contains(name));
    EXPECT_TRUE(global_ws_lst->contains(name));

    // delete subset
    delete kdb_subset;
    EXPECT_EQ(global_ws_lst->size(), nb_total_lists);
}

TEST_F(KDBListsTest, CopyFrom)
{
    std::string pattern = "C* T*";
    std::string filename = str_input_test_dir + prefix_filename + "fun.lst";
    int expected_nb_comments = global_ws_lst->size();
    std::set<std::string> v_expected_names;

    // Copy entire file
    global_ws_lst->clear();
    global_ws_lst->copy_from(filename, "*");
    EXPECT_EQ(global_ws_lst->size(), expected_nb_comments); 

    // copy subset
    v_expected_names = global_ws_lst->filter_names(pattern);
    global_ws_lst->clear();
    global_ws_lst->copy_from(filename, pattern);
    EXPECT_EQ(global_ws_lst->size(), v_expected_names.size());  
    EXPECT_EQ(global_ws_lst->get_names(), v_expected_names);  
}

TEST_F(KDBListsTest, Merge)
{
    std::string pattern = "C*";

    // create deep copies kdb
    KDBLists* kdb0 = new KDBLists(global_ws_lst.get(), pattern, true);
    KDBLists* kdb1 = new KDBLists(global_ws_lst.get(), pattern, true);
    KDBLists* kdb_to_merge = new KDBLists(global_ws_lst.get(), pattern, true);

    // add an element to the KDB to be merged
    std::string new_name = "NEW_LIST";
    std::string new_list = "ACAF;ACAG;AOUC;AQC";
    kdb_to_merge->add(new_name, new_list);

    // modify an existing element of the KDB to be merge
    std::string name = "COPY";
    std::string unmodified_list = kdb_to_merge->get(name);
    std::string modified_list = kdb_to_merge->get("COPY0") + kdb_to_merge->get("COPY1");
    kdb_to_merge->update(name, modified_list);


    // merge (overwrite)
    kdb0->merge(*kdb_to_merge, true, false);
    // a) check kdb0 contains new item of KDB to be merged
    EXPECT_TRUE(kdb0->contains(new_name));
    EXPECT_EQ(kdb0->get(new_name), new_list);
    // b) check already existing item has been overwritten
    EXPECT_EQ(kdb0->get(name), modified_list); 

    // merge (NOT overwrite)
    kdb1->merge(*kdb_to_merge, false, false);
    // b) check already existing item has NOT been overwritten
    EXPECT_EQ(kdb1->get(name), unmodified_list);
}

TEST_F(KDBListsTest, Search)
{
    std::string lst_name = "COPY0";
    std::vector<std::string> objs_list;

    global_ws_cmt->load(str_input_test_dir + "fun.ac");
    global_ws_eqs->load(str_input_test_dir + "fun.ae");
    global_ws_idt->load(str_input_test_dir + "fun.ai");
    global_ws_scl->load(str_input_test_dir + "fun.as");
    global_ws_tbl->load(str_input_test_dir + "fun.at");
    global_ws_var->load(str_input_test_dir + "fun.av");

    objs_list = global_ws_cmt->search(lst_name);
    EXPECT_EQ(objs_list.size(), 0);

    objs_list = global_ws_eqs->search(lst_name);
    EXPECT_EQ(objs_list.size(), 0);

    objs_list = global_ws_idt->search(lst_name);
    EXPECT_EQ(objs_list.size(), 0);

    std::vector<std::string> expected_lts = { "COPY", "COPY0" };
    objs_list = global_ws_lst->search(lst_name);
    EXPECT_EQ(objs_list, expected_lts);

    objs_list = global_ws_scl->search(lst_name);
    EXPECT_EQ(objs_list.size(), 0);

    objs_list = global_ws_tbl->search(lst_name);
    EXPECT_EQ(objs_list.size(), 0);

    objs_list = global_ws_var->search(lst_name);
    EXPECT_EQ(objs_list.size(), 0);
}

TEST_F(KDBListsTest, Hash)
{
    std::size_t hash_val = hash_value(*global_ws_lst);

    // change a name
    global_ws_lst->rename("COPY0", "COPY_0");
    std::size_t hash_val_modified = hash_value(*global_ws_lst);
    EXPECT_NE(hash_val, hash_val_modified);
    std::cout << "(rename list) orignal vs modified hash: " << std::to_string(hash_val) << " vs " << std::to_string(hash_val_modified) << std::endl;

    // modify an entry
    hash_val = hash_val_modified;
    std::string expanded_list = global_ws_lst->get("COPY_0") + global_ws_lst->get("COPY1");
    global_ws_lst->update("COPY", expanded_list);
    hash_val_modified = hash_value(*global_ws_lst);
    EXPECT_NE(hash_val, hash_val_modified);
    std::cout << "(modify list) orignal vs modified hash: " << std::to_string(hash_val) << " vs " << std::to_string(hash_val_modified) << std::endl;

    // remove an entry
    hash_val = hash_val_modified;
    global_ws_lst->remove("COPY");
    hash_val_modified = hash_value(*global_ws_lst);
    EXPECT_NE(hash_val, hash_val_modified);
    std::cout << "(delete list) orignal vs modified hash: " << std::to_string(hash_val) << " vs " << std::to_string(hash_val_modified) << std::endl;

    // add an entry
    hash_val = hash_val_modified;
    std::string new_list = "ACAF;ACAG;AOUC;AQC";
    global_ws_lst->add("NEW_ENTRY", new_list);
    hash_val_modified = hash_value(*global_ws_lst);
    EXPECT_NE(hash_val, hash_val_modified);   
    std::cout << "(new    list) orignal vs modified hash: " << std::to_string(hash_val) << " vs " << std::to_string(hash_val_modified) << std::endl; 
}
