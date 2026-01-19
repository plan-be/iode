#include "pch.h"


class KDBCommentsTest : public TestAbstract, public ::testing::Test 
{
protected:
    void SetUp() override 
    {
        global_ws_cmt->load(str_input_test_dir + "fun.ac");
    }

    // void TearDown() override {}
};


TEST_F(KDBCommentsTest, Load)
{
    KDBComments kdb(false);
    kdb.load(str_input_test_dir + prefix_filename + "fun.cmt");
    EXPECT_EQ(kdb.size(), 317);
}

TEST_F(KDBCommentsTest, Subset)
{
    std::string pattern = "A*";
    std::string comment = global_ws_cmt->get("ACAF");
    std::string modified = "modified";

    // GLOBAL KDB
    EXPECT_EQ(global_ws_cmt->size(), 317);
    EXPECT_TRUE(global_ws_cmt->is_global_database());
    std::set<std::string> names = global_ws_cmt->filter_names(pattern);

    // DEEP COPY SUBSET
    KDBComments* kdb_subset_deep_copy = new KDBComments(global_ws_cmt.get(), pattern, true);
    EXPECT_EQ(kdb_subset_deep_copy->size(), names.size());
    EXPECT_TRUE(kdb_subset_deep_copy->is_local_database());
    kdb_subset_deep_copy->update("ACAF", modified);
    EXPECT_EQ(global_ws_cmt->get("ACAF"), comment);
    EXPECT_EQ(kdb_subset_deep_copy->get("ACAF"), modified);

    // SHALLOW COPY SUBSET
    KDBComments* kdb_subset_shallow_copy = new KDBComments(global_ws_cmt.get(), pattern, false);
    EXPECT_EQ(kdb_subset_shallow_copy->size(), names.size());
    EXPECT_TRUE(kdb_subset_shallow_copy->is_shallow_copy_database());
    kdb_subset_shallow_copy->update("ACAF", modified);
    EXPECT_EQ(global_ws_cmt->get("ACAF"), modified);
    EXPECT_EQ(kdb_subset_shallow_copy->get("ACAF"), modified);
}

TEST_F(KDBCommentsTest, Save)
{
    // save in binary format
    global_ws_cmt->save(str_output_test_dir + "fun.cmt");

    // save in ascii format
    global_ws_cmt->save(str_output_test_dir + "fun.ac");
}

TEST_F(KDBCommentsTest, Filename)
{
    std::string expected_filename = str_input_test_dir + "fun.ac";
    std::string filename = global_ws_cmt->get_filename_utf8();
    // WARNING: for a weird reason the filename is set to lower case when the present 
    // test is runned on Github Actions virtual machine (Windows Server)
    for(auto& c : expected_filename) c = tolower(c);
    for(auto& c : filename) c = tolower(c);
    EXPECT_EQ(filename, expected_filename);

    std::string new_filename = str_output_test_dir + "fun.ac";
    global_ws_cmt->set_filename_utf8(new_filename);
    filename = global_ws_cmt->get_filename_utf8();
    
    for(auto& c : new_filename) c = tolower(c);
    for(auto& c : filename) c = tolower(c);
    EXPECT_EQ(filename, new_filename);
}

TEST_F(KDBCommentsTest, GetName)
{
    std::string name; 
    
    name = global_ws_cmt->get_name(0);
    EXPECT_EQ(name, "ACAF");
    
    EXPECT_THROW(global_ws_cmt->get_name(400), std::out_of_range);
}

TEST_F(KDBCommentsTest, Rename)
{
    bool success;
    std::string old_name;
    std::string new_name;
    std::string error_msg;

    // rename
    old_name = global_ws_cmt->get_name(0);
    EXPECT_EQ(old_name, "ACAF");
    success = global_ws_cmt->rename(old_name, "NEW_NAME");
    EXPECT_TRUE(success);
    EXPECT_TRUE(global_ws_cmt->contains("NEW_NAME"));

    // expect errors 
    old_name = global_ws_cmt->get_name(2);

    // - too long name
    error_msg = "Cannot accept name \"ABCDEFGHIJKLMNOPQRSTUVWXYZ\" as Comment: ";
    error_msg += "Iode names cannot exceed 20 characters. ABCDEFGHIJKLMNOPQRSTUVWXYZ = 26 characters";
    EXPECT_THROW(global_ws_cmt->rename(old_name, error_msg), std::invalid_argument);
    
    // - name starting with a umber
    error_msg = "Cannot accept name \"1_NEW_NAME\" as Comment: Comment name must only ";
    error_msg += "contains capital or lowercase letters, digits and underscore";
    EXPECT_THROW(global_ws_cmt->rename(old_name, error_msg), std::invalid_argument);
    
    // - name using forbidden special characters
    error_msg = "Cannot accept name \"NËW_N@ME\" as Comment: Comment name must only ";
    error_msg += "contains capital or lowercase letters, digits and underscore";
    EXPECT_THROW(global_ws_cmt->rename(old_name, error_msg), std::invalid_argument);
    
    // - old name does not exists
    EXPECT_THROW(global_ws_cmt->rename("EMPTY", "NEW_NAME"), std::invalid_argument);
    
    // - new name already exists
    new_name = global_ws_cmt->get_name(3);
    EXPECT_THROW(global_ws_cmt->rename(old_name, new_name), std::invalid_argument);
}

TEST_F(KDBCommentsTest, Contains)
{
    // name exists
    EXPECT_TRUE(global_ws_cmt->contains("ACAF"));

    // name doesn't exist
    EXPECT_FALSE(global_ws_cmt->contains("UNKNOWN"));
}

TEST_F(KDBCommentsTest, Get)
{
    Comment expected_comment = "Ondernemingen: ontvangen kapitaaloverdrachten.";
    EXPECT_EQ(global_ws_cmt->get("ACAF"), expected_comment);
}

TEST_F(KDBCommentsTest, GetNames)
{
    std::string list_names;
    std::string expected_list_names;
    std::set<std::string> names;
    std::set<std::string> expected_names;

    // no pattern
    std::string name;
    for(int i=0; i < global_ws_cmt->size(); i++)
    {
        name = global_ws_cmt->get_name(i);
        expected_names.insert(name);
        expected_list_names += name + ";";
    }
    expected_list_names.pop_back();     // remove trailing ';'

    names = global_ws_cmt->get_names();
    EXPECT_EQ(names, expected_names);
    list_names = global_ws_cmt->get_names_as_string();
    EXPECT_EQ(list_names, expected_list_names);

    // pattern
    std::string pattern = "A*;*_";
    expected_names.clear();
    expected_list_names = "";
    for(const std::string& name : names) 
        if(name.front() == 'A' || name.back() == '_')
        {
            expected_names.insert(name);
            expected_list_names += name + ";";
        }
    expected_list_names.pop_back();     // remove trailing ';'

    std::set<std::string> set_names = global_ws_cmt->filter_names(pattern);
    std::set<std::string> set_expected_names(expected_names.begin(), expected_names.end());
    EXPECT_EQ(set_names, set_expected_names);
}

TEST_F(KDBCommentsTest, CreateRemove)
{
    std::string name = "NEW_COMMENT";
    Comment new_comment = "This is a new comment";

    global_ws_cmt->add(name, new_comment);
    EXPECT_EQ(global_ws_cmt->get(name), new_comment);

    global_ws_cmt->remove(name);
    EXPECT_THROW(global_ws_cmt->get(name), std::invalid_argument);

    // expect errors 
    std::string old_name = global_ws_cmt->get_name(2);
    // - too long name
    EXPECT_THROW(global_ws_cmt->add("ABCDEFGHIJKLMNOPQRSTUVWXYZ", new_comment), std::invalid_argument);
    // - name starting with a umber
    EXPECT_THROW(global_ws_cmt->add("1_NEW_NAME", new_comment), std::invalid_argument);
    // - name using forbidden special characters
    EXPECT_THROW(global_ws_cmt->add("N�W_N@ME", new_comment), std::invalid_argument);
    // - name already exists
    name = global_ws_cmt->get_name(3);
    EXPECT_THROW(global_ws_cmt->add(name, new_comment), std::invalid_argument);
}

TEST_F(KDBCommentsTest, Update)
{
    std::string name = "ACAF";
    Comment new_comment = "New Comment";

    // by name
    global_ws_cmt->update(name, new_comment);
    EXPECT_EQ(global_ws_cmt->get(name), new_comment);

    // error: name does not exist
    EXPECT_THROW(global_ws_cmt->update("UNKNOWN", new_comment), std::invalid_argument);
}

TEST_F(KDBCommentsTest, Filter)
{
    std::string pattern = "A*;*_";
    std::set<std::string> expected_names;
    KDBComments* kdb_subset;

    std::set<std::string> all_names;
    for (int p = 0; p < global_ws_cmt->size(); p++) 
        all_names.insert(global_ws_cmt->get_name(p));

    int nb_total_comments = global_ws_cmt->size();
    // A*
    for (const std::string& name : all_names) 
        if (name.front() == 'A') 
            expected_names.insert(name);
    // *_
    for (const std::string& name : all_names) 
        if (name.back() == '_') 
            expected_names.insert(name);

    // create a subset (shallow copy)
    kdb_subset = new KDBComments(global_ws_cmt.get(), pattern, false);
    EXPECT_EQ(kdb_subset->size(), expected_names.size());
    EXPECT_EQ(kdb_subset->get_names(), expected_names);

    // modify an element of the subset and check if the 
    // corresponding element of the global KDB also changes
    std::string name = "ACAF";
    std::string modified_comment = "Modified Comment";
    kdb_subset->update(name, modified_comment);
    EXPECT_EQ(kdb_subset->get(name), modified_comment);
    EXPECT_EQ(global_ws_cmt->get(name), modified_comment);

    // modify an element of the subset and check if it has also 
    // been added to the global KDB
    std::string new_name = "NEW_COMMENT";
    std::string new_comment = "New Comment";
    kdb_subset->add(new_name, new_comment);
    EXPECT_EQ(kdb_subset->get(new_name), new_comment);
    EXPECT_EQ(global_ws_cmt->get(new_name), new_comment);

    // modify an element of the subset and check if the 
    // corresponding element has also been renamed in the global KDB
    std::string old_name = new_name;
    new_name = "COMMENT_NEW";
    kdb_subset->rename(old_name, new_name);
    EXPECT_EQ(kdb_subset->get(new_name), new_comment);
    EXPECT_EQ(global_ws_cmt->get(new_name), new_comment);

    // delete an element from the subset and check if it has also 
    // been deleted from the global KDB
    kdb_subset->remove(new_name);
    EXPECT_FALSE(kdb_subset->contains(new_name));
    EXPECT_FALSE(global_ws_cmt->contains(new_name));

    // try to add an element to the subset which is already present 
    // in the global KDB
    EXPECT_THROW(kdb_subset->add("BENEF", new_comment), std::invalid_argument);

    // delete subset
    delete kdb_subset;
    EXPECT_EQ(global_ws_cmt->size(), nb_total_comments);
    EXPECT_EQ(global_ws_cmt->get(name), modified_comment);

    // wrong pattern
    pattern = "anjfks";
    EXPECT_THROW(KDBComments(global_ws_cmt.get(), pattern, false), std::runtime_error);

    // subset of a subset
    pattern = "A*;*_";
    kdb_subset = new KDBComments(global_ws_cmt.get(), pattern, false);

    std::string pattern_subset_subset = "B*";
    std::set<std::string> expected_names_subset_subset;
    for(const std::string& name: expected_names)
        if(name.front() == 'B') 
            expected_names_subset_subset.insert(name);

    KDBComments* kdb_subset_subset = new KDBComments(kdb_subset, pattern_subset_subset, false);
    EXPECT_EQ(kdb_subset_subset->size(), expected_names_subset_subset.size());
    EXPECT_EQ(kdb_subset_subset->get_names(), expected_names_subset_subset);
    delete kdb_subset;
    delete kdb_subset_subset;
}

TEST_F(KDBCommentsTest, DeepCopy)
{
    std::string pattern = "A*;*_";
    std::set<std::string> expected_names;
    KDBComments* kdb_subset;

    std::set<std::string> all_names;
    for (int p = 0; p < global_ws_cmt->size(); p++) 
        all_names.insert(global_ws_cmt->get_name(p));

    int nb_total_comments = global_ws_cmt->size();
    // A*
    for (const std::string& name : all_names) 
        if (name.front() == 'A') 
            expected_names.insert(name);
    // *_
    for (const std::string& name : all_names) 
        if (name.back() == '_') 
            expected_names.insert(name);

    // create a subset (deep copy)
    kdb_subset = new KDBComments(global_ws_cmt.get(), pattern, true);
    EXPECT_EQ(kdb_subset->size(), expected_names.size());
    EXPECT_EQ(kdb_subset->get_names(), expected_names);
    
    // modify an element of the subset and check if the 
    // corresponding element of the global KDB didn't changed
    std::string name = "ACAF";
    std::string comment = global_ws_cmt->get(name);
    std::string modified_comment = "Modified Comment";
    kdb_subset->update(name, modified_comment);
    EXPECT_EQ(kdb_subset->get(name), modified_comment);
    EXPECT_EQ(global_ws_cmt->get(name), comment);

    // modify an element of the subset and check if it has not 
    // been added to the global KDB
    std::string new_name = "NEW_COMMENT";
    std::string new_comment = "New Comment";
    kdb_subset->add(new_name, new_comment);
    EXPECT_TRUE(kdb_subset->contains(new_name));
    EXPECT_EQ(kdb_subset->get(new_name), new_comment);
    EXPECT_FALSE(global_ws_cmt->contains(new_name));

    // modify an element of the subset and check if the 
    // corresponding element has not been renamed in the global KDB
    name = "ACAG";
    new_name = "COMMENT_NEW";
    kdb_subset->rename(name, new_name);
    EXPECT_TRUE(kdb_subset->contains(new_name));
    EXPECT_FALSE(global_ws_cmt->contains(new_name));

    // delete an element from the subset and check if it has not 
    // been deleted from the global KDB
    name = "AOUC";
    kdb_subset->remove(name);
    EXPECT_FALSE(kdb_subset->contains(name));
    EXPECT_TRUE(global_ws_cmt->contains(name));

    // delete subset
    delete kdb_subset;
    EXPECT_EQ(global_ws_cmt->size(), nb_total_comments);
}

TEST_F(KDBCommentsTest, CopyFrom)
{
    std::string pattern = "A* *_";
    std::string filename = str_input_test_dir + prefix_filename + "fun.cmt";
    int expected_nb_comments = global_ws_cmt->size();
    std::set<std::string> v_expected_names;

    // Copy entire file
    global_ws_cmt->clear();
    global_ws_cmt->copy_from(filename, "*");
    EXPECT_EQ(global_ws_cmt->size(), expected_nb_comments); 

    // copy subset
    v_expected_names = global_ws_cmt->filter_names(pattern);
    global_ws_cmt->clear();
    global_ws_cmt->copy_from(filename, pattern);
    EXPECT_EQ(global_ws_cmt->size(), v_expected_names.size());  
    EXPECT_EQ(global_ws_cmt->get_names(), v_expected_names);  
}

TEST_F(KDBCommentsTest, Merge)
{
    std::string pattern = "A*";

    // create deep copies kdb
    KDBComments* kdb0 = new KDBComments(global_ws_cmt.get(), pattern, true);
    KDBComments* kdb1 = new KDBComments(global_ws_cmt.get(), pattern, true);
    KDBComments* kdb_to_merge = new KDBComments(global_ws_cmt.get(), pattern, true);

    // add an element to the KDB to be merged
    std::string new_name = "NEW_COMMENT";
    std::string new_comment = "New Comment";
    kdb_to_merge->add(new_name, new_comment);

    // modify an existing element of the KDB to be merge
    std::string name = "ACAF";
    std::string unmodified_comment = kdb_to_merge->get(name);
    std::string modified_comment = "Modified Comment";
    kdb_to_merge->update(name, modified_comment);

    // merge (overwrite)
    kdb0->merge(*kdb_to_merge, true);
    // a) check kdb0 contains new item of KDB to be merged
    EXPECT_TRUE(kdb0->contains(new_name));
    EXPECT_EQ(kdb0->get(new_name), new_comment);
    // b) check already existing item has been overwritten
    EXPECT_EQ(kdb0->get(name), modified_comment); 

    // merge (NOT overwrite)
    kdb1->merge(*kdb_to_merge, false);
    // b) check already existing item has NOT been overwritten
    EXPECT_EQ(kdb1->get(name), unmodified_comment);
}

TEST_F(KDBCommentsTest, Search)
{
    std::string cmt_name = "ACAF";
    std::vector<std::string> objs_list;

    global_ws_eqs->load(str_input_test_dir + "fun.ae");
    global_ws_idt->load(str_input_test_dir + "fun.ai");
    global_ws_lst->load(str_input_test_dir + "fun.al");
    global_ws_scl->load(str_input_test_dir + "fun.as");
    global_ws_tbl->load(str_input_test_dir + "fun.at");
    global_ws_var->load(str_input_test_dir + "fun.av");

    std::vector<std::string> expected_cmts = { cmt_name };
    objs_list = global_ws_cmt->search(cmt_name);
    EXPECT_EQ(objs_list, expected_cmts);

    std::vector<std::string> expected_eqs = { cmt_name, "FLF" };
    objs_list = global_ws_eqs->search(cmt_name);
    EXPECT_EQ(objs_list, expected_eqs);

    objs_list = global_ws_idt->search(cmt_name);
    EXPECT_EQ(objs_list.size(), 0);

    std::vector<std::string> expected_lts = { "COPY0", "ENDO0", "TOTAL0", "_RES" };
    objs_list = global_ws_lst->search(cmt_name);
    EXPECT_EQ(objs_list, expected_lts);

    objs_list = global_ws_scl->search(cmt_name);
    EXPECT_EQ(objs_list.size(), 0);

    objs_list = global_ws_tbl->search(cmt_name);
    EXPECT_EQ(objs_list.size(), 0);

    std::vector<std::string> expected_vars = { cmt_name };
    objs_list = global_ws_var->search(cmt_name);
    EXPECT_EQ(objs_list, expected_vars);
}

TEST_F(KDBCommentsTest, Hash)
{
    std::size_t hash_val = hash_value(*global_ws_cmt);

    // change a name
    global_ws_cmt->rename("ACAF", "NEW_NAME");
    std::size_t hash_val_modified = hash_value(*global_ws_cmt);
    EXPECT_NE(hash_val, hash_val_modified);
    std::cout << "(rename comment) orignal vs modified hash: " << std::to_string(hash_val) << " vs " << std::to_string(hash_val_modified) << std::endl;

    // modify an entry
    hash_val = hash_val_modified;
    global_ws_cmt->update("NEW_NAME", "modified comment");
    hash_val_modified = hash_value(*global_ws_cmt);
    EXPECT_NE(hash_val, hash_val_modified);
    std::cout << "(modify comment) orignal vs modified hash: " << std::to_string(hash_val) << " vs " << std::to_string(hash_val_modified) << std::endl;

    // remove an entry
    hash_val = hash_val_modified;
    global_ws_cmt->remove("NEW_NAME");
    hash_val_modified = hash_value(*global_ws_cmt);
    EXPECT_NE(hash_val, hash_val_modified);
    std::cout << "(delete comment) orignal vs modified hash: " << std::to_string(hash_val) << " vs " << std::to_string(hash_val_modified) << std::endl;

    // add an entry
    hash_val = hash_val_modified;
    global_ws_cmt->add("NEW_ENTRY", "New comment");
    hash_val_modified = hash_value(*global_ws_cmt);
    EXPECT_NE(hash_val, hash_val_modified);   
    std::cout << "(new    comment) orignal vs modified hash: " << std::to_string(hash_val) << " vs " << std::to_string(hash_val_modified) << std::endl; 
}
