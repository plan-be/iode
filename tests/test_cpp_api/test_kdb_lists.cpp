#include "pch.h"


class KDBListsTest : public KDBTest, public ::testing::Test
{
protected:
    void SetUp() override
    {
        KDBLists kdb_lst(input_test_dir + "fun.al");
    }

    // void TearDown() override {}
};


TEST_F(KDBListsTest, Load)
{
    KDBLists kdb(input_test_dir + prefix_filename + "fun.lst");
    EXPECT_EQ(kdb.size(), 17);
}

TEST_F(KDBListsTest, Subset)
{
    std::string pattern = "C*";
    std::string list = Lists.get("COPY");
    std::string new_list = "ACAF;ACAG;AOUC;AQC";

    // GLOBAL KDB
    KDBLists kdb_global;
    EXPECT_EQ(kdb_global.size(), 17);
    EXPECT_TRUE(kdb_global.is_global_database());

    // DEEP COPY SUBSET
    KDBLists* kdb_subset_deep_copy = kdb_global.subset(pattern, true);
    std::vector<std::string> names = kdb_global.get_names(pattern);
    EXPECT_EQ(kdb_subset_deep_copy->size(), names.size());
    EXPECT_TRUE(kdb_subset_deep_copy->is_local_database());
    kdb_subset_deep_copy->update("COPY", new_list);
    EXPECT_EQ(kdb_global.get("COPY"), list);
    EXPECT_EQ(kdb_subset_deep_copy->get("COPY"), new_list);

    // SHALLOW COPY SUBSET
    KDBLists* kdb_subset_shallow_copy = kdb_global.subset(pattern, false);
    EXPECT_EQ(kdb_subset_shallow_copy->size(), names.size());
    EXPECT_TRUE(kdb_subset_shallow_copy->is_shallow_copy_database());
    kdb_subset_shallow_copy->update("COPY", new_list);
    EXPECT_EQ(kdb_global.get("COPY"), new_list);
    EXPECT_EQ(kdb_subset_shallow_copy->get("COPY"), new_list);
}

TEST_F(KDBListsTest, Save)
{
    // save in binary format
    Lists.save(output_test_dir + "fun.lst");

    // save in ascii format
    Lists.save(output_test_dir + "fun.al");
}

TEST_F(KDBListsTest, Get)
{
    int pos = 0;
    std::string list;
    std::string expected_list = "$COPY0;$COPY1;";

    // by position
    list = Lists.get(pos);
    EXPECT_EQ(expected_list, list);

    // by name
    std::string name = Lists.get_name(pos);
    list = Lists.get(name);
    EXPECT_EQ(expected_list, list);
}

TEST_F(KDBListsTest, GetNames)
{
    std::vector<std::string> expected_names;
    for (int i=0; i < Lists.size(); i++) expected_names.push_back(Lists.get_name(i));
    std::vector<std::string> names = Lists.get_names();
    EXPECT_EQ(names, expected_names);
}

TEST_F(KDBListsTest, CreateRemove)
{
    std::string name = "A_COMMENTS";
    std::string new_list = "ACAF;ACAG;AOUC;AQC";

    Lists.add(name, new_list);
    ASSERT_EQ(Lists.get(name), new_list);

    Lists.remove(name);
    EXPECT_THROW(Lists.get(name), std::invalid_argument);
}

TEST_F(KDBListsTest, Update)
{
    std::string expanded_list = Lists.get("COPY0") + Lists.get("COPY1");

    Lists.update("COPY", expanded_list);
    EXPECT_EQ(Lists.get("COPY"), expanded_list);
}

TEST_F(KDBListsTest, Copy)
{
    std::string name = "COPY";
    std::string list = Lists.get(name);

    std::string list_copy = Lists.copy(name);
    EXPECT_EQ(list_copy, list);

    // add copy
    Lists.add("DUP_" + name, list_copy);
}

TEST_F(KDBListsTest, Filter)
{
    std::string pattern = "C*";
    std::vector<std::string> expected_names;
    KDBLists* kdb_subset;

    std::vector<std::string> all_names;
    for (int p = 0; p < Lists.size(); p++) all_names.push_back(Lists.get_name(p));
    for (const std::string& name : all_names) if (name.front() == 'C') expected_names.push_back(name);

    int nb_total_lists = Lists.size();

    // remove duplicate entries
    // NOTE: std::unique only removes consecutive duplicated elements, 
    //       so the vector needst to be sorted first
    std::sort(expected_names.begin(), expected_names.end());
    std::vector<std::string>::iterator it = std::unique(expected_names.begin(), expected_names.end());  
    expected_names.resize(std::distance(expected_names.begin(), it));

    // create local kdb
    kdb_subset = Lists.subset(pattern);
    EXPECT_EQ(kdb_subset->size(), expected_names.size());
    EXPECT_EQ(kdb_subset->get_names(), expected_names);

    // modify an element of the local KDB and check if the 
    // corresponding element of the global KDB also changes
    std::string name = "COPY";
    std::string expanded_list = Lists.get("COPY0") + Lists.get("COPY1");
    kdb_subset->update(name, expanded_list);
    EXPECT_EQ(kdb_subset->get(name), expanded_list);
    EXPECT_EQ(Lists.get(name), expanded_list);

    // add an element to the local KDB and check if it has also 
    // been added to the global KDB
    std::string new_name = "NEW_LIST";
    std::string new_list = "ACAF;ACAG;AOUC;AQC";
    kdb_subset->add(new_name, new_list);
    EXPECT_EQ(kdb_subset->get(new_name), new_list);
    EXPECT_EQ(Lists.get(new_name), new_list);

    // rename an element in the local KDB and check if the 
    // corresponding element has also been renamed in the global KDB
    std::string old_name = new_name;
    new_name = "LIST_NEW";
    kdb_subset->rename(old_name, new_name);
    EXPECT_EQ(kdb_subset->get(new_name), new_list);
    EXPECT_EQ(Lists.get(new_name), new_list);

    // delete an element from the local KDB and check if it has also 
    // been deleted from the global KDB
    kdb_subset->remove(new_name);
    EXPECT_FALSE(kdb_subset->contains(new_name));
    EXPECT_FALSE(Lists.contains(new_name));

    // try to add an element to the local KDB which is already present 
    // in the global KDB
    EXPECT_THROW(kdb_subset->add("ENDO", new_list), std::invalid_argument);

    // delete local kdb
    delete kdb_subset;
    EXPECT_EQ(Lists.size(), nb_total_lists);
    EXPECT_EQ(Lists.get(name), expanded_list);

    // wrong pattern
    pattern = "anjfks";
    EXPECT_THROW(Lists.subset(pattern), std::runtime_error);
}

TEST_F(KDBListsTest, DeepCopy)
{
    std::string pattern = "C*";
    std::vector<std::string> expected_names;
    KDBLists* kdb_subset;

    std::vector<std::string> all_names;
    for (int p = 0; p < Lists.size(); p++) all_names.push_back(Lists.get_name(p));
    for (const std::string& name : all_names) if (name.front() == 'C') expected_names.push_back(name);

    // remove duplicate entries
    // NOTE: std::unique only removes consecutive duplicated elements, 
    //       so the vector needst to be sorted first
    std::sort(expected_names.begin(), expected_names.end());
    std::vector<std::string>::iterator it = std::unique(expected_names.begin(), expected_names.end());  
    expected_names.resize(std::distance(expected_names.begin(), it));

    int nb_total_lists = Lists.size();

    // create local kdb
    kdb_subset = Lists.subset(pattern, true);
    EXPECT_EQ(kdb_subset->size(), expected_names.size());
    EXPECT_EQ(kdb_subset->get_names(), expected_names);

    // modify an element of the local KDB and check if the 
    // corresponding element of the global KDB didn't changed
    std::string name = "COPY";
    std::string list = Lists.get(name);
    std::string expanded_list = Lists.get("COPY0") + Lists.get("COPY1");
    kdb_subset->update(name, expanded_list);
    EXPECT_EQ(kdb_subset->get(name), expanded_list);
    EXPECT_EQ(Lists.get(name), list);

    // add an element to the local KDB and check if it has not 
    // been added to the global KDB
    std::string new_name = "NEW_LIST";
    std::string new_list = "ACAF;ACAG;AOUC;AQC";
    kdb_subset->add(new_name, new_list);
    EXPECT_TRUE(kdb_subset->contains(new_name));
    EXPECT_EQ(kdb_subset->get(new_name), new_list);
    EXPECT_FALSE(Lists.contains(new_name));

    // rename an element in the local KDB and check if the 
    // corresponding element has not been renamed in the global KDB
    name = "COPY0";
    new_name = "LIST_NEW";
    kdb_subset->rename(name, new_name);
    EXPECT_TRUE(kdb_subset->contains(new_name));
    EXPECT_FALSE(Lists.contains(new_name));

    // delete an element from the local KDB and check if it has not 
    // been deleted from the global KDB
    name = "COPY1";
    kdb_subset->remove(name);
    EXPECT_FALSE(kdb_subset->contains(name));
    EXPECT_TRUE(Lists.contains(name));

    // delete local kdb
    delete kdb_subset;
    EXPECT_EQ(Lists.size(), nb_total_lists);

    // wrong pattern
    pattern = "anjfks";
    EXPECT_THROW(Lists.subset(pattern, true), std::runtime_error);
}

TEST_F(KDBListsTest, CopyFrom)
{
    std::string pattern = "C* T*";
    std::string filename = input_test_dir + prefix_filename + "fun.lst";
    int expected_nb_comments = Lists.size();
    std::vector<std::string> v_expected_names;

    // Copy entire file
    Lists.clear();
    Lists.copy_from(filename, "*");
    EXPECT_EQ(Lists.size(), expected_nb_comments); 

    // copy subset
    v_expected_names = Lists.get_names(pattern);
    Lists.clear();
    Lists.copy_from(filename, pattern);
    EXPECT_EQ(Lists.size(), v_expected_names.size());  
    EXPECT_EQ(Lists.get_names(), v_expected_names);  
}

TEST_F(KDBListsTest, Merge)
{
    std::string pattern = "C*";

    // create deep copies kdb
    KDBLists* kdb0 = Lists.subset(pattern, true);
    KDBLists* kdb1 = Lists.subset(pattern, true);
    KDBLists* kdb_to_merge = Lists.subset(pattern, true);

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
    kdb0->merge(*kdb_to_merge, true);
    // a) check kdb0 contains new item of KDB to be merged
    EXPECT_TRUE(kdb0->contains(new_name));
    EXPECT_EQ(kdb0->get(new_name), new_list);
    // b) check already existing item has been overwritten
    EXPECT_EQ(kdb0->get(name), modified_list); 

    // merge (NOT overwrite)
    kdb1->merge(*kdb_to_merge, false);
    // b) check already existing item has NOT been overwritten
    EXPECT_EQ(kdb1->get(name), unmodified_list);
}

TEST_F(KDBListsTest, Search)
{
    std::string lst_name = "COPY0";
    std::vector<std::string> objs_list;

    KDBComments kdb_cmt(input_test_dir + "fun.ac");
    KDBEquations kdb_eqs(input_test_dir + "fun.ae");
    KDBIdentities kdb_idt(input_test_dir + "fun.ai");
    KDBScalars kdb_scl(input_test_dir + "fun.as");
    KDBTables kdb_tbl(input_test_dir + "fun.at");
    KDBVariables kdb_var(input_test_dir + "fun.av");

    objs_list = Comments.search(lst_name);
    EXPECT_EQ(objs_list.size(), 0);

    objs_list = Equations.search(lst_name);
    EXPECT_EQ(objs_list.size(), 0);

    objs_list = Identities.search(lst_name);
    EXPECT_EQ(objs_list.size(), 0);

    std::vector<std::string> expected_lts = { "COPY", "COPY0" };
    objs_list = Lists.search(lst_name);
    EXPECT_EQ(objs_list, expected_lts);

    objs_list = Scalars.search(lst_name);
    EXPECT_EQ(objs_list.size(), 0);

    objs_list = Tables.search(lst_name);
    EXPECT_EQ(objs_list.size(), 0);

    objs_list = Variables.search(lst_name);
    EXPECT_EQ(objs_list.size(), 0);
}

TEST_F(KDBListsTest, Hash)
{
    std::size_t hash_val = hash_value(Lists);

    // change a name
    Lists.rename("COPY0", "COPY_0");
    std::size_t hash_val_modified = hash_value(Lists);
    EXPECT_NE(hash_val, hash_val_modified);
    std::cout << "(rename list) orignal vs modified hash: " << std::to_string(hash_val) << " vs " << std::to_string(hash_val_modified) << std::endl;

    // modify an entry
    hash_val = hash_val_modified;
    std::string expanded_list = Lists.get("COPY_0") + Lists.get("COPY1");
    Lists.update("COPY", expanded_list);
    hash_val_modified = hash_value(Lists);
    EXPECT_NE(hash_val, hash_val_modified);
    std::cout << "(modify list) orignal vs modified hash: " << std::to_string(hash_val) << " vs " << std::to_string(hash_val_modified) << std::endl;

    // remove an entry
    hash_val = hash_val_modified;
    Lists.remove("COPY");
    hash_val_modified = hash_value(Lists);
    EXPECT_NE(hash_val, hash_val_modified);
    std::cout << "(delete list) orignal vs modified hash: " << std::to_string(hash_val) << " vs " << std::to_string(hash_val_modified) << std::endl;

    // add an entry
    hash_val = hash_val_modified;
    std::string new_list = "ACAF;ACAG;AOUC;AQC";
    Lists.add("NEW_ENTRY", new_list);
    hash_val_modified = hash_value(Lists);
    EXPECT_NE(hash_val, hash_val_modified);   
    std::cout << "(new    list) orignal vs modified hash: " << std::to_string(hash_val) << " vs " << std::to_string(hash_val_modified) << std::endl; 
}
