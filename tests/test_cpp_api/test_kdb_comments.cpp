#include "pch.h"


class KDBCommentsTest : public KDBTest, public ::testing::Test 
{
protected:
    void SetUp() override 
    {
        KDBComments kdb_cmt(input_test_dir + "fun.ac");
    }

    // void TearDown() override {}
};


TEST_F(KDBCommentsTest, Load)
{
    KDBComments kdb(input_test_dir + prefix_filename + "fun.cmt");
    EXPECT_EQ(kdb.count(), 317);
}

TEST_F(KDBCommentsTest, Subset)
{
    std::string pattern = "A*";
    std::string comment = Comments.get("ACAF");
    std::string modified = "modified";

    // GLOBAL KDB
    KDBComments kdb_global;
    EXPECT_EQ(kdb_global.count(), 317);
    EXPECT_TRUE(kdb_global.is_global_database());

    // DEEP COPY SUBSET
    KDBComments* kdb_subset_deep_copy = kdb_global.subset(pattern, true);
    std::vector<std::string> names = kdb_global.get_names(pattern);
    EXPECT_EQ(kdb_subset_deep_copy->count(), names.size());
    EXPECT_TRUE(kdb_subset_deep_copy->is_local_database());
    kdb_subset_deep_copy->update("ACAF", modified);
    EXPECT_EQ(kdb_global.get("ACAF"), comment);
    EXPECT_EQ(kdb_subset_deep_copy->get("ACAF"), modified);

    // SHALLOW COPY SUBSET
    KDBComments* kdb_subset_shallow_copy = kdb_global.subset(pattern, false);
    EXPECT_EQ(kdb_subset_shallow_copy->count(), names.size());
    EXPECT_TRUE(kdb_subset_shallow_copy->is_shallow_copy_database());
    kdb_subset_shallow_copy->update("ACAF", modified);
    EXPECT_EQ(kdb_global.get("ACAF"), modified);
    EXPECT_EQ(kdb_subset_shallow_copy->get("ACAF"), modified);
}

TEST_F(KDBCommentsTest, Save)
{
    // save in binary format
    Comments.save(output_test_dir + "fun.cmt");

    // save in ascii format
    Comments.save(output_test_dir + "fun.ac");
}

TEST_F(KDBCommentsTest, Filename)
{
    std::string expected_filename = input_test_dir + "fun.ac";
    std::string filename = Comments.get_filename();
    // WARNING: for a weird reason the filename is set to lower case when the present 
    // test is runned on Github Actions virtual machine (Windows Server)
    for(auto& c : expected_filename) c = tolower(c);
    for(auto& c : filename) c = tolower(c);
    EXPECT_EQ(filename, expected_filename);

    std::string new_filename = output_test_dir + "fun.ac";
    Comments.set_filename(new_filename);
    filename = Comments.get_filename();
    
    for(auto& c : new_filename) c = tolower(c);
    for(auto& c : filename) c = tolower(c);
    EXPECT_EQ(filename, new_filename);
}

TEST_F(KDBCommentsTest, GetName)
{
    std::string name; 
    
    name = Comments.get_name(0);
    EXPECT_EQ(name, "ACAF");
    
    EXPECT_THROW(Comments.get_name(400), std::invalid_argument);
}

TEST_F(KDBCommentsTest, Rename)
{
    int new_pos;
    std::string old_name;
    std::string new_name;

    // rename
    old_name = Comments.get_name(0);
    EXPECT_EQ(old_name, "ACAF");
    new_pos = Comments.rename(old_name, "NEW_NAME");
    EXPECT_EQ(Comments.get_name(new_pos), "NEW_NAME");

    // expect errors 
    old_name = Comments.get_name(2);
    // - too long name
    EXPECT_THROW(Comments.rename(old_name, "ABCDEFGHIJKLMNOPQRSTUVWXYZ"), IodeExceptionFunction);
    // - name starting with a umber
    EXPECT_THROW(Comments.rename(old_name, "1_NEW_NAME"), IodeExceptionFunction);
    // - name using forbidden special characters
    EXPECT_THROW(Comments.rename(old_name, "NËW_N@ME"), IodeExceptionFunction);
    // - old name does not exists
    EXPECT_THROW(Comments.rename("EMPTY", "NEW_NAME"), std::invalid_argument);
    // - new name already exists
    new_name = Comments.get_name(3);
    EXPECT_THROW(Comments.rename(old_name, new_name), std::invalid_argument);
    
    // set by position
    new_pos = Comments.set_name(1, "NEW_POS");
    EXPECT_EQ(Comments.get_name(new_pos), "NEW_POS");
}

TEST_F(KDBCommentsTest, Contains)
{
    // name exists
    EXPECT_TRUE(Comments.contains("ACAF"));

    // name doesn't exist
    EXPECT_FALSE(Comments.contains("UNKNOWN"));
}

TEST_F(KDBCommentsTest, Get)
{
    std::string name = Comments.get_name(0);
    Comment expected_comment = "Ondernemingen: ontvangen kapitaaloverdrachten.";

    // get by name
    EXPECT_EQ(Comments.get(name), expected_comment);

    // get by position
    EXPECT_EQ(Comments.get(0), expected_comment);
}

TEST_F(KDBCommentsTest, GetNames)
{
    std::string list_names;
    std::string expected_list_names;
    std::vector<std::string> names;
    std::vector<std::string> expected_names;

    // no pattern
    for (int i=0; i < Comments.count(); i++)
    {
        expected_names.push_back(Comments.get_name(i));
        expected_list_names += expected_names.back() + ";";
    }
    expected_list_names.pop_back();     // remove trailing ';'

    names = Comments.get_names();
    EXPECT_EQ(names, expected_names);
    list_names = Comments.get_names_as_string();
    EXPECT_EQ(list_names, expected_list_names);

    // pattern
    std::string pattern = "A*;*_";
    expected_names.clear();
    expected_list_names = "";
    for(const std::string& name : names) 
        if(name.front() == 'A' || name.back() == '_')
        {
            expected_names.push_back(name);
            expected_list_names += name + ";";
        }
    expected_list_names.pop_back();     // remove trailing ';'

    names = Comments.get_names(pattern);
    EXPECT_EQ(names, expected_names);
    list_names = Comments.get_names_as_string(pattern);
    EXPECT_EQ(list_names, expected_list_names);
}

TEST_F(KDBCommentsTest, CreateRemove)
{
    std::string name = "NEW_COMMENT";
    Comment new_comment = "This is a new comment";

    Comments.add(name, new_comment);
    EXPECT_EQ(Comments.get(name), new_comment);

    Comments.remove(name);
    EXPECT_THROW(Comments.get(name), std::invalid_argument);

    // expect errors 
    std::string old_name = Comments.get_name(2);
    // - too long name
    EXPECT_THROW(Comments.add("ABCDEFGHIJKLMNOPQRSTUVWXYZ", new_comment), IodeExceptionFunction);
    // - name starting with a umber
    EXPECT_THROW(Comments.add("1_NEW_NAME", new_comment), IodeExceptionFunction);
    // - name using forbidden special characters
    EXPECT_THROW(Comments.add("N�W_N@ME", new_comment), IodeExceptionFunction);
    // - name already exists
    name = Comments.get_name(3);
    EXPECT_THROW(Comments.add(name, new_comment), std::invalid_argument);
}

TEST_F(KDBCommentsTest, Update)
{
    std::string name = Comments.get_name(0);
    Comment new_comment = "New Comment";

    // by name
    Comments.update(name, new_comment);
    EXPECT_EQ(Comments.get(name), new_comment);

    // error: name does not exist
    EXPECT_THROW(Comments.update("UNKNOWN", new_comment), std::invalid_argument);

    // by position
    Comments.update(1, new_comment);
    EXPECT_EQ(Comments.get(1), new_comment);

    // error: position does not exist
    int beyond_last_pos = Comments.count() + 10;
    EXPECT_THROW(Comments.update(beyond_last_pos, new_comment), std::invalid_argument);
}

TEST_F(KDBCommentsTest, Copy)
{
    int pos = 0;
    std::string name = Comments.get_name(pos);

    // by postion
    Comment comment = Comments.get(pos);
    Comment copy_comment = Comments.copy(pos);
    EXPECT_EQ(copy_comment, comment);

    // by name
    copy_comment = Comments.copy(name);
    EXPECT_EQ(copy_comment, comment);

    // add copy
    Comments.add("DUP_" + name, copy_comment);

    // error: name does not exist
    EXPECT_THROW(Comments.copy("UNKNOWN"), std::invalid_argument);

    // error: position does not exist
    int beyond_last_pos = Comments.count() + 10;
    EXPECT_THROW(Comments.copy(beyond_last_pos), std::invalid_argument);
}

TEST_F(KDBCommentsTest, Filter)
{
    std::string pattern = "A*;*_";
    std::vector<std::string> expected_names;
    KDBComments* kdb_subset;

    std::vector<std::string> all_names;
    for (int p = 0; p < Comments.count(); p++) all_names.push_back(Comments.get_name(p));

    int nb_total_comments = Comments.count();
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
    kdb_subset = Comments.subset(pattern);
    EXPECT_EQ(kdb_subset->count(), expected_names.size());
    EXPECT_EQ(kdb_subset->get_names(), expected_names);

    // modify an element of the local KDB and check if the 
    // corresponding element of the global KDB also changes
    std::string name = "ACAF";
    std::string modified_comment = "Modified Comment";
    kdb_subset->update(name, modified_comment);
    EXPECT_EQ(kdb_subset->get(name), modified_comment);
    EXPECT_EQ(Comments.get(name), modified_comment);

    // add an element to the local KDB and check if it has also 
    // been added to the global KDB
    std::string new_name = "NEW_COMMENT";
    std::string new_comment = "New Comment";
    kdb_subset->add(new_name, new_comment);
    EXPECT_EQ(kdb_subset->get(new_name), new_comment);
    EXPECT_EQ(Comments.get(new_name), new_comment);

    // rename an element in the local KDB and check if the 
    // corresponding element has also been renamed in the global KDB
    std::string old_name = new_name;
    new_name = "COMMENT_NEW";
    kdb_subset->rename(old_name, new_name);
    EXPECT_EQ(kdb_subset->get(new_name), new_comment);
    EXPECT_EQ(Comments.get(new_name), new_comment);

    // delete an element from the local KDB and check if it has also 
    // been deleted from the global KDB
    kdb_subset->remove(new_name);
    EXPECT_FALSE(kdb_subset->contains(new_name));
    EXPECT_FALSE(Comments.contains(new_name));

    // try to add an element to the local KDB which is already present 
    // in the global KDB
    EXPECT_THROW(kdb_subset->add("BENEF", new_comment), std::invalid_argument);

    // delete local kdb
    delete kdb_subset;
    EXPECT_EQ(Comments.count(), nb_total_comments);
    EXPECT_EQ(Comments.get(name), modified_comment);

    // wrong pattern
    pattern = "anjfks";
    EXPECT_THROW(Comments.subset(pattern), std::runtime_error);

    // subset of a subset
    pattern = "A*;*_";
    kdb_subset = Comments.subset(pattern);

    std::string pattern_subset_subset = "B*";
    std::vector<std::string> expected_names_subset_subset;
    for(const std::string& name: expected_names)
        if(name.front() == 'B') expected_names_subset_subset.push_back(name);

    KDBComments* kdb_subset_subset = kdb_subset->subset(pattern_subset_subset);
    EXPECT_EQ(kdb_subset_subset->count(), expected_names_subset_subset.size());
    EXPECT_EQ(kdb_subset_subset->get_names(), expected_names_subset_subset);
    delete kdb_subset;
    delete kdb_subset_subset;
}

TEST_F(KDBCommentsTest, DeepCopy)
{
    std::string pattern = "A*;*_";
    std::vector<std::string> expected_names;
    KDBComments* kdb_subset;

    std::vector<std::string> all_names;
    for (int p = 0; p < Comments.count(); p++) all_names.push_back(Comments.get_name(p));

    int nb_total_comments = Comments.count();
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
    kdb_subset = Comments.subset(pattern, true);
    EXPECT_EQ(kdb_subset->count(), expected_names.size());
    EXPECT_EQ(kdb_subset->get_names(), expected_names);
    
    // modify an element of the local KDB and check if the 
    // corresponding element of the global KDB didn't changed
    std::string name = "ACAF";
    std::string comment = Comments.get(name);
    std::string modified_comment = "Modified Comment";
    kdb_subset->update(name, modified_comment);
    EXPECT_EQ(kdb_subset->get(name), modified_comment);
    EXPECT_EQ(Comments.get(name), comment);

    // add an element to the local KDB and check if it has not 
    // been added to the global KDB
    std::string new_name = "NEW_COMMENT";
    std::string new_comment = "New Comment";
    kdb_subset->add(new_name, new_comment);
    EXPECT_TRUE(kdb_subset->contains(new_name));
    EXPECT_EQ(kdb_subset->get(new_name), new_comment);
    EXPECT_FALSE(Comments.contains(new_name));

    // rename an element in the local KDB and check if the 
    // corresponding element has not been renamed in the global KDB
    name = "ACAG";
    new_name = "COMMENT_NEW";
    kdb_subset->rename(name, new_name);
    EXPECT_TRUE(kdb_subset->contains(new_name));
    EXPECT_FALSE(Comments.contains(new_name));

    // delete an element from the local KDB and check if it has not 
    // been deleted from the global KDB
    name = "AOUC";
    kdb_subset->remove(name);
    EXPECT_FALSE(kdb_subset->contains(name));
    EXPECT_TRUE(Comments.contains(name));

    // delete local kdb
    delete kdb_subset;
    EXPECT_EQ(Comments.count(), nb_total_comments);

    // wrong pattern
    pattern = "anjfks";
    EXPECT_THROW(Comments.subset(pattern, true), std::runtime_error);
}

TEST_F(KDBCommentsTest, CopyFrom)
{
    std::string pattern = "A* *_";
    std::string filename = input_test_dir + prefix_filename + "fun.cmt";
    int expected_nb_comments = Comments.count();
    std::vector<std::string> v_expected_names;

    // Copy entire file
    Comments.clear();
    Comments.copy_from(filename, "*");
    EXPECT_EQ(Comments.count(), expected_nb_comments); 

    // copy subset
    v_expected_names = Comments.get_names(pattern);
    Comments.clear();
    Comments.copy_from(filename, pattern);
    EXPECT_EQ(Comments.count(), v_expected_names.size());  
    EXPECT_EQ(Comments.get_names(), v_expected_names);  
}

TEST_F(KDBCommentsTest, Merge)
{
    std::string pattern = "A*";

    // create deep copies kdb
    KDBComments* kdb0 = Comments.subset(pattern, true);
    KDBComments* kdb1 = Comments.subset(pattern, true);
    KDBComments* kdb_to_merge = Comments.subset(pattern, true);

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

    KDBComments kdb_cmt(input_test_dir + "fun.ac");
    KDBEquations kdb_eqs(input_test_dir + "fun.ae");
    KDBIdentities kdb_idt(input_test_dir + "fun.ai");
    KDBLists kdb_lst(input_test_dir + "fun.al");
    KDBScalars kdb_scl(input_test_dir + "fun.as");
    KDBTables kdb_tbl(input_test_dir + "fun.at");
    KDBVariables kdb_var(input_test_dir + "fun.av");

    std::vector<std::string> expected_cmts = { cmt_name };
    objs_list = Comments.search(cmt_name);
    EXPECT_EQ(objs_list, expected_cmts);

    std::vector<std::string> expected_eqs = { cmt_name, "FLF" };
    objs_list = Equations.search(cmt_name);
    EXPECT_EQ(objs_list, expected_eqs);

    objs_list = Identities.search(cmt_name);
    EXPECT_EQ(objs_list.size(), 0);

    std::vector<std::string> expected_lts = { "COPY0", "ENDO0", "TOTAL0", "_RES" };
    objs_list = Lists.search(cmt_name);
    EXPECT_EQ(objs_list, expected_lts);

    objs_list = Scalars.search(cmt_name);
    EXPECT_EQ(objs_list.size(), 0);

    objs_list = Tables.search(cmt_name);
    EXPECT_EQ(objs_list.size(), 0);

    std::vector<std::string> expected_vars = { cmt_name };
    objs_list = Variables.search(cmt_name);
    EXPECT_EQ(objs_list, expected_vars);
}

TEST_F(KDBCommentsTest, Hash)
{
    std::size_t hash_val = hash_value(Comments);

    // change a name
    Comments.rename("ACAF", "NEW_NAME");
    std::size_t hash_val_modified = hash_value(Comments);
    EXPECT_NE(hash_val, hash_val_modified);
    std::cout << "(rename comment) orignal vs modified hash: " << std::to_string(hash_val) << " vs " << std::to_string(hash_val_modified) << std::endl;

    // modify an entry
    hash_val = hash_val_modified;
    Comments.update("NEW_NAME", "modified comment");
    hash_val_modified = hash_value(Comments);
    EXPECT_NE(hash_val, hash_val_modified);
    std::cout << "(modify comment) orignal vs modified hash: " << std::to_string(hash_val) << " vs " << std::to_string(hash_val_modified) << std::endl;

    // remove an entry
    hash_val = hash_val_modified;
    Comments.remove("NEW_NAME");
    hash_val_modified = hash_value(Comments);
    EXPECT_NE(hash_val, hash_val_modified);
    std::cout << "(delete comment) orignal vs modified hash: " << std::to_string(hash_val) << " vs " << std::to_string(hash_val_modified) << std::endl;

    // add an entry
    hash_val = hash_val_modified;
    Comments.add("NEW_ENTRY", "New comment");
    hash_val_modified = hash_value(Comments);
    EXPECT_NE(hash_val, hash_val_modified);   
    std::cout << "(new    comment) orignal vs modified hash: " << std::to_string(hash_val) << " vs " << std::to_string(hash_val_modified) << std::endl; 
}
