#include "pch.h"


class KDBCommentsTest : public KDBTest, public ::testing::Test 
{
protected:
    void SetUp() override 
    {
        load_global_kdb(I_COMMENTS, input_test_dir + "fun.cmt");
    }

    // void TearDown() override {}
};


TEST_F(KDBCommentsTest, Load)
{
    Comments.load(input_test_dir + "fun.cmt");
    EXPECT_EQ(Comments.count(), 317);
}

TEST_F(KDBCommentsTest, CopyConstructor)
{
    std::string comment = Comments.get("ACAF");
    std::string modified = "modified";

    // GLOBAL KDB
    KDBComments kdb_copy(Comments);
    EXPECT_EQ(kdb_copy.count(), 317);
    EXPECT_TRUE(kdb_copy.is_global_kdb());

    // LOCAL KDB
    KDBComments local_kdb(KDB_LOCAL, "A*");
    KDBComments local_kdb_hard_copy(local_kdb);
    EXPECT_EQ(local_kdb.count(), local_kdb_hard_copy.count());
    EXPECT_TRUE(local_kdb_hard_copy.is_local_kdb());
    local_kdb_hard_copy.update("ACAF", modified);
    EXPECT_EQ(local_kdb.get("ACAF"), comment);
    EXPECT_EQ(local_kdb_hard_copy.get("ACAF"), modified);

    // SHALLOW COPY KDB
    KDBComments shallow_kdb(KDB_SHALLOW_COPY, "A*");
    KDBComments shallow_kdb_copy(shallow_kdb);
    EXPECT_EQ(shallow_kdb.count(), shallow_kdb_copy.count());
    EXPECT_TRUE(shallow_kdb_copy.is_shallow_copy());
    shallow_kdb_copy.update("ACAF", modified);
    EXPECT_EQ(shallow_kdb.get("ACAF"), modified);
    EXPECT_EQ(shallow_kdb_copy.get("ACAF"), modified);
}

TEST_F(KDBCommentsTest, Save)
{
    // save in binary format
    save_global_kdb(I_COMMENTS, output_test_dir + "fun.cmt");
    Comments.save(output_test_dir + "fun.cmt");

    // save in ascii format
    save_global_kdb(I_COMMENTS, output_test_dir + "fun.ac");
    Comments.save(output_test_dir + "fun.ac");
}

TEST_F(KDBCommentsTest, Filename)
{
    std::string expected_filename = input_test_dir + "fun.cmt";
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
    
    EXPECT_THROW(Comments.get_name(400), IodeExceptionFunction);
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
    EXPECT_THROW(Comments.rename(old_name, "N�W_N@ME"), IodeExceptionFunction);
    // - old name does not exists
    EXPECT_THROW(Comments.rename("EMPTY", "NEW_NAME"), IodeExceptionFunction);
    // - new name already exists
    new_name = Comments.get_name(3);
    EXPECT_THROW(Comments.rename(old_name, new_name), IodeExceptionFunction);
    
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
    std::vector<std::string> expected_names;
    for (int i=0; i < Comments.count(); i++) expected_names.push_back(Comments.get_name(i));
    std::vector<std::string> names = Comments.get_names();
    EXPECT_EQ(names, expected_names);
}

TEST_F(KDBCommentsTest, CreateRemove)
{
    std::string name = "NEW_COMMENT";
    Comment new_comment = "This is a new comment";

    Comments.add(name, new_comment);
    EXPECT_EQ(Comments.get(name), new_comment);

    Comments.remove(name);
    EXPECT_THROW(Comments.get(name), IodeExceptionFunction);

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
    EXPECT_THROW(Comments.add(name, new_comment), IodeExceptionInitialization);
}

TEST_F(KDBCommentsTest, Update)
{
    std::string name = Comments.get_name(0);
    Comment new_comment = "New Comment";

    // by name
    Comments.update(name, new_comment);
    EXPECT_EQ(Comments.get(name), new_comment);

    // error: name does not exist
    EXPECT_THROW(Comments.update("UNKNOWN", new_comment), IodeExceptionFunction);

    // by position
    Comments.update(1, new_comment);
    EXPECT_EQ(Comments.get(1), new_comment);

    // error: position does not exist
    int beyond_last_pos = Comments.count() + 10;
    EXPECT_THROW(Comments.update(beyond_last_pos, new_comment), IodeExceptionFunction);
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
    EXPECT_THROW(Comments.copy("UNKNOWN"), IodeExceptionFunction);

    // error: position does not exist
    int beyond_last_pos = Comments.count() + 10;
    EXPECT_THROW(Comments.copy(beyond_last_pos), IodeExceptionFunction);
}

TEST_F(KDBCommentsTest, Filter)
{
    std::string pattern = "A*;*_";
    std::vector<std::string> expected_names;
    KDBComments* local_kdb;

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
    local_kdb = new KDBComments(KDB_SHALLOW_COPY, pattern);
    EXPECT_EQ(local_kdb->count(), expected_names.size());

    // modify an element of the local KDB and check if the 
    // corresponding element of the global KDB also changes
    std::string name = "ACAF";
    std::string modified_comment = "Modified Comment";
    local_kdb->update(name, modified_comment);
    EXPECT_EQ(local_kdb->get(name), modified_comment);
    EXPECT_EQ(Comments.get(name), modified_comment);

    // add an element to the local KDB and check if it has also 
    // been added to the global KDB
    std::string new_name = "NEW_COMMENT";
    std::string new_comment = "New Comment";
    local_kdb->add(new_name, new_comment);
    EXPECT_EQ(local_kdb->get(new_name), new_comment);
    EXPECT_EQ(Comments.get(new_name), new_comment);

    // rename an element in the local KDB and check if the 
    // corresponding element has also been renamed in the global KDB
    std::string old_name = new_name;
    new_name = "COMMENT_NEW";
    local_kdb->rename(old_name, new_name);
    EXPECT_EQ(local_kdb->get(new_name), new_comment);
    EXPECT_EQ(Comments.get(new_name), new_comment);

    // delete an element from the local KDB and check if it has also 
    // been deleted from the global KDB
    local_kdb->remove(new_name);
    EXPECT_FALSE(local_kdb->contains(new_name));
    EXPECT_FALSE(Comments.contains(new_name));

    // delete local kdb
    delete local_kdb;
    EXPECT_EQ(Comments.count(), nb_total_comments);
    EXPECT_EQ(Comments.get(name), modified_comment);
}

TEST_F(KDBCommentsTest, HardCopy)
{
    std::string pattern = "A*;*_";
    std::vector<std::string> expected_names;
    KDBComments* local_kdb;

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
    local_kdb = new KDBComments(KDB_LOCAL, pattern);
    EXPECT_EQ(local_kdb->count(), expected_names.size());

    // modify an element of the local KDB and check if the 
    // corresponding element of the global KDB didn't changed
    std::string name = "ACAF";
    std::string comment = Comments.get(name);
    std::string modified_comment = "Modified Comment";
    local_kdb->update(name, modified_comment);
    EXPECT_EQ(local_kdb->get(name), modified_comment);
    EXPECT_EQ(Comments.get(name), comment);

    // add an element to the local KDB and check if it has not 
    // been added to the global KDB
    std::string new_name = "NEW_COMMENT";
    std::string new_comment = "New Comment";
    local_kdb->add(new_name, new_comment);
    EXPECT_TRUE(local_kdb->contains(new_name));
    EXPECT_EQ(local_kdb->get(new_name), new_comment);
    EXPECT_FALSE(Comments.contains(new_name));

    // rename an element in the local KDB and check if the 
    // corresponding element has not been renamed in the global KDB
    name = "ACAG";
    new_name = "COMMENT_NEW";
    local_kdb->rename(name, new_name);
    EXPECT_TRUE(local_kdb->contains(new_name));
    EXPECT_FALSE(Comments.contains(new_name));

    // delete an element from the local KDB and check if it has not 
    // been deleted from the global KDB
    name = "AOUC";
    local_kdb->remove(name);
    EXPECT_FALSE(local_kdb->contains(name));
    EXPECT_TRUE(Comments.contains(name));

    // delete local kdb
    delete local_kdb;
    EXPECT_EQ(Comments.count(), nb_total_comments);
}

TEST_F(KDBCommentsTest, Merge)
{
    std::string pattern = "A*";

    // create hard copies kdb
    KDBComments kdb0(KDB_LOCAL, pattern);
    KDBComments kdb1(KDB_LOCAL, pattern);
    KDBComments kdb_to_merge(KDB_LOCAL, pattern);

    // add an element to the KDB to be merged
    std::string new_name = "NEW_COMMENT";
    std::string new_comment = "New Comment";
    kdb_to_merge.add(new_name, new_comment);

    // modify an existing element of the KDB to be merge
    std::string name = "ACAF";
    std::string unmodified_comment = kdb_to_merge.get(name);
    std::string modified_comment = "Modified Comment";
    kdb_to_merge.update(name, modified_comment);

    // merge (overwrite)
    kdb0.merge(kdb_to_merge, true);
    // a) check kdb0 contains new item of KDB to be merged
    EXPECT_TRUE(kdb0.contains(new_name));
    EXPECT_EQ(kdb0.get(new_name), new_comment);
    // b) check already existing item has been overwritten
    EXPECT_EQ(kdb0.get(name), modified_comment); 

    // merge (NOT overwrite)
    kdb1.merge(kdb_to_merge, false);
    // b) check already existing item has NOT been overwritten
    EXPECT_EQ(kdb1.get(name), unmodified_comment);
}

TEST_F(KDBCommentsTest, AssociatedObjs)
{
    std::string name = "ACAF";
    std::vector<std::string> objs_list;

    load_global_kdb(I_EQUATIONS, input_test_dir + "fun.eqs");
    load_global_kdb(I_IDENTITIES, input_test_dir + "fun.idt");
    load_global_kdb(I_LISTS, input_test_dir + "fun.lst");
    load_global_kdb(I_SCALARS, input_test_dir + "fun.scl");
    load_global_kdb(I_TABLES, input_test_dir + "fun.tbl");
    load_global_kdb(I_VARIABLES, input_test_dir + "fun.var");

    std::vector<std::string> expected_cmts = { name };
    objs_list = Comments.get_associated_objects_list(name, I_COMMENTS);
    EXPECT_EQ(objs_list, expected_cmts);

    std::vector<std::string> expected_eqs = { name, "FLF" };
    objs_list = Comments.get_associated_objects_list(name, I_EQUATIONS);
    EXPECT_EQ(objs_list, expected_eqs);

    objs_list = Comments.get_associated_objects_list(name, I_IDENTITIES);
    EXPECT_EQ(objs_list.size(), 0);

    std::vector<std::string> expected_lts = { "COPY0", "ENDO0", "TOTAL0" };
    objs_list = Comments.get_associated_objects_list(name, I_LISTS);
    EXPECT_EQ(objs_list, expected_lts);

    objs_list = Comments.get_associated_objects_list(name, I_SCALARS);
    EXPECT_EQ(objs_list.size(), 0);

    objs_list = Comments.get_associated_objects_list(name, I_TABLES);
    EXPECT_EQ(objs_list.size(), 0);

    std::vector<std::string> expected_vars = { name };
    objs_list = Comments.get_associated_objects_list(name, I_VARIABLES);
    EXPECT_EQ(objs_list, expected_vars);
}

TEST_F(KDBCommentsTest, Hash)
{
    boost::hash<KDBComments> kdb_hasher;
    std::size_t hash_val = kdb_hasher(Comments);

    // change a name
    Comments.rename("ACAF", "NEW_NAME");
    std::size_t hash_val_modified = kdb_hasher(Comments);
    EXPECT_NE(hash_val, hash_val_modified);
    std::cout << "(rename comment) orignal vs modified hash: " << std::to_string(hash_val) << " vs " << std::to_string(hash_val_modified) << std::endl;

    // modify an entry
    hash_val = hash_val_modified;
    Comments.update("NEW_NAME", "modified comment");
    hash_val_modified = kdb_hasher(Comments);
    EXPECT_NE(hash_val, hash_val_modified);
    std::cout << "(modify comment) orignal vs modified hash: " << std::to_string(hash_val) << " vs " << std::to_string(hash_val_modified) << std::endl;

    // remove an entry
    hash_val = hash_val_modified;
    Comments.remove("NEW_NAME");
    hash_val_modified = kdb_hasher(Comments);
    EXPECT_NE(hash_val, hash_val_modified);
    std::cout << "(delete comment) orignal vs modified hash: " << std::to_string(hash_val) << " vs " << std::to_string(hash_val_modified) << std::endl;

    // add an entry
    hash_val = hash_val_modified;
    Comments.add("NEW_ENTRY", "New comment");
    hash_val_modified = kdb_hasher(Comments);
    EXPECT_NE(hash_val, hash_val_modified);   
    std::cout << "(new    comment) orignal vs modified hash: " << std::to_string(hash_val) << " vs " << std::to_string(hash_val_modified) << std::endl; 
}
