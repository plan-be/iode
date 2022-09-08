#include "pch.h"


class KDBCommentsTest : public KDBTest, public ::testing::Test 
{
protected:
    KDBComments kdb;

    void SetUp() override 
    {
        load_global_kdb(I_COMMENTS, input_test_dir + "fun.cmt");
    }

    // void TearDown() override {}
};


TEST_F(KDBCommentsTest, Load)
{
    KDBComments kdb2;
    EXPECT_EQ(kdb2.count(), 317);
}


TEST_F(KDBCommentsTest, Save)
{
    // save in binary format
    save_global_kdb(I_COMMENTS, output_test_dir + "fun.cmt");
    kdb.dump(output_test_dir + "fun.cmt");

    // save in ascii format
    save_global_kdb(I_COMMENTS, output_test_dir + "fun.ac");
    kdb.dump(output_test_dir + "fun.ac");
}

TEST_F(KDBCommentsTest, GetName)
{
    std::string name; 
    
    name = kdb.get_name(0);
    EXPECT_EQ(name, "ACAF");
    
    EXPECT_THROW(kdb.get_name(400), IodeExceptionFunction);
}

TEST_F(KDBCommentsTest, Rename)
{
    int new_pos;
    std::string old_name;
    std::string new_name;

    // rename
    old_name = kdb.get_name(0);
    EXPECT_EQ(old_name, "ACAF");
    new_pos = kdb.rename(old_name, "NEW_NAME");
    EXPECT_EQ(kdb.get_name(new_pos), "NEW_NAME");

    // expect errors 
    old_name = kdb.get_name(2);
    // - too long name
    EXPECT_THROW(kdb.rename(old_name, "ABCDEFGHIJKLMNOPQRSTUVWXYZ"), IodeExceptionFunction);
    // - name starting with a umber
    EXPECT_THROW(kdb.rename(old_name, "1_NEW_NAME"), IodeExceptionFunction);
    // - name using forbidden special characters
    EXPECT_THROW(kdb.rename(old_name, "N�W_N@ME"), IodeExceptionFunction);
    // - old name does not exists
    EXPECT_THROW(kdb.rename("EMPTY", "NEW_NAME"), IodeExceptionFunction);
    // - new name already exists
    new_name = kdb.get_name(3);
    EXPECT_THROW(kdb.rename(old_name, new_name), IodeExceptionFunction);
    
    // set by position
    new_pos = kdb.set_name(1, "NEW_POS");
    EXPECT_EQ(kdb.get_name(new_pos), "NEW_POS");
}

TEST_F(KDBCommentsTest, Contains)
{
    // name exists
    EXPECT_TRUE(kdb.contains("ACAF"));

    // name doesn't exist
    EXPECT_FALSE(kdb.contains("UNKNOWN"));
}

TEST_F(KDBCommentsTest, Get)
{
    std::string name = kdb.get_name(0);
    Comment expected_comment = "Ondernemingen: ontvangen kapitaaloverdrachten.";

    // get by name
    EXPECT_EQ(kdb.get(name), expected_comment);

    // get by position
    EXPECT_EQ(kdb.get(0), expected_comment);
}

TEST_F(KDBCommentsTest, GetNames)
{
    std::vector<std::string> expected_names;
    for (int i=0; i < kdb.count(); i++) expected_names.push_back(kdb.get_name(i));
    std::vector<std::string> names = kdb.get_names();
    EXPECT_EQ(names, expected_names);
}

TEST_F(KDBCommentsTest, CreateRemove)
{
    std::string name = "NEW_COMMENT";
    Comment new_comment = "This is a new comment";

    kdb.add(name, new_comment);
    EXPECT_EQ(kdb.get(name), new_comment);

    kdb.remove(name);
    EXPECT_THROW(kdb.get(name), IodeExceptionFunction);

    // expect errors 
    std::string old_name = kdb.get_name(2);
    // - too long name
    EXPECT_THROW(kdb.add("ABCDEFGHIJKLMNOPQRSTUVWXYZ", new_comment), IodeExceptionFunction);
    // - name starting with a umber
    EXPECT_THROW(kdb.add("1_NEW_NAME", new_comment), IodeExceptionFunction);
    // - name using forbidden special characters
    EXPECT_THROW(kdb.add("N�W_N@ME", new_comment), IodeExceptionFunction);
    // - name already exists
    name = kdb.get_name(3);
    EXPECT_THROW(kdb.add(name, new_comment), IodeExceptionInitialization);
}

TEST_F(KDBCommentsTest, Update)
{
    std::string name = kdb.get_name(0);
    Comment new_comment = "New Comment";

    // by name
    kdb.update(name, new_comment);
    EXPECT_EQ(kdb.get(name), new_comment);

    // error: name does not exist
    EXPECT_THROW(kdb.update("UNKNOWN", new_comment), IodeExceptionFunction);

    // by position
    kdb.update(1, new_comment);
    EXPECT_EQ(kdb.get(1), new_comment);

    // error: position does not exist
    int beyond_last_pos = kdb.count() + 10;
    EXPECT_THROW(kdb.update(beyond_last_pos, new_comment), IodeExceptionFunction);
}

TEST_F(KDBCommentsTest, Copy)
{
    int pos = 0;
    std::string name = kdb.get_name(pos);

    // by postion
    Comment comment = kdb.get(pos);
    Comment copy_comment = kdb.copy(pos);
    EXPECT_EQ(copy_comment, comment);

    // by name
    copy_comment = kdb.copy(name);
    EXPECT_EQ(copy_comment, comment);

    // error: name does not exist
    EXPECT_THROW(kdb.copy("UNKNOWN"), IodeExceptionFunction);

    // error: position does not exist
    int beyond_last_pos = kdb.count() + 10;
    EXPECT_THROW(kdb.copy(beyond_last_pos), IodeExceptionFunction);
}

TEST_F(KDBCommentsTest, Filter)
{
    std::string pattern = "A*;*_";
    std::vector<std::string> expected_names;
    KDBComments* local_kdb;
    KDBComments global_kdb;

    std::vector<std::string> all_names;
    for (int p = 0; p < global_kdb.count(); p++) all_names.push_back(global_kdb.get_name(p));

    int nb_total_comments = global_kdb.count();
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
    local_kdb = new KDBComments(pattern);
    EXPECT_EQ(local_kdb->count(), expected_names.size());

    // modify an element of the local KDB and check if the 
    // corresponding element of the global KDB also changes
    std::string name = "ACAF";
    std::string modified_comment = "Modified Comment";
    local_kdb->update(name, modified_comment);
    EXPECT_EQ(local_kdb->get(name), modified_comment);
    EXPECT_EQ(global_kdb.get(name), modified_comment);

    // add an element to the local KDB and check if it has also 
    // been added to the global KDB
    std::string new_name = "NEW_COMMENT";
    std::string new_comment = "New Comment";
    local_kdb->add(new_name, new_comment);
    EXPECT_EQ(local_kdb->get(new_name), new_comment);
    EXPECT_EQ(global_kdb.get(new_name), new_comment);

    // rename an element in the local KDB and check if the 
    // corresponding element has also been renamed in the global KDB
    std::string old_name = new_name;
    new_name = "COMMENT_NEW";
    local_kdb->rename(old_name, new_name);
    EXPECT_EQ(local_kdb->get(new_name), new_comment);
    EXPECT_EQ(global_kdb.get(new_name), new_comment);

    // delete an element from the local KDB and check if it has also 
    // been deleted from the global KDB
    local_kdb->remove(new_name);
    EXPECT_FALSE(local_kdb->contains(new_name));
    EXPECT_FALSE(global_kdb.contains(new_name));

    // delete local kdb
    delete local_kdb;
    EXPECT_EQ(global_kdb.count(), nb_total_comments);
    EXPECT_EQ(global_kdb.get(name), modified_comment);
}

TEST_F(KDBCommentsTest, HardCopy)
{
    std::string pattern = "A*;*_";
    std::vector<std::string> expected_names;
    KDBComments* local_kdb;
    KDBComments global_kdb;

    std::vector<std::string> all_names;
    for (int p = 0; p < global_kdb.count(); p++) all_names.push_back(global_kdb.get_name(p));

    int nb_total_comments = global_kdb.count();
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
    local_kdb = new KDBComments(pattern, false);
    EXPECT_EQ(local_kdb->count(), expected_names.size());

    // modify an element of the local KDB and check if the 
    // corresponding element of the global KDB didn't changed
    std::string name = "ACAF";
    std::string comment = global_kdb.get(name);
    std::string modified_comment = "Modified Comment";
    local_kdb->update(name, modified_comment);
    EXPECT_EQ(local_kdb->get(name), modified_comment);
    EXPECT_EQ(global_kdb.get(name), comment);

    // add an element to the local KDB and check if it has not 
    // been added to the global KDB
    std::string new_name = "NEW_COMMENT";
    std::string new_comment = "New Comment";
    local_kdb->add(new_name, new_comment);
    EXPECT_TRUE(local_kdb->contains(new_name));
    EXPECT_EQ(local_kdb->get(new_name), new_comment);
    EXPECT_FALSE(global_kdb.contains(new_name));

    // rename an element in the local KDB and check if the 
    // corresponding element has not been renamed in the global KDB
    name = "ACAG";
    new_name = "COMMENT_NEW";
    local_kdb->rename(name, new_name);
    EXPECT_TRUE(local_kdb->contains(new_name));
    EXPECT_FALSE(global_kdb.contains(new_name));

    // delete an element from the local KDB and check if it has not 
    // been deleted from the global KDB
    name = "AOUC";
    local_kdb->remove(name);
    EXPECT_FALSE(local_kdb->contains(name));
    EXPECT_TRUE(global_kdb.contains(name));

    // delete local kdb
    delete local_kdb;
    EXPECT_EQ(global_kdb.count(), nb_total_comments);
}
