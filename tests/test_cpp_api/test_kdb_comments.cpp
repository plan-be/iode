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
    KDBComments kdb;
    EXPECT_EQ(kdb.count(), 317);
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
    
    EXPECT_THROW(kdb.get_name(400), std::runtime_error);
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
    EXPECT_THROW(kdb.rename(old_name, "ABCDEFGHIJKLMNOPQRSTUVWXYZ"), std::runtime_error);
    // - name starting with a umber
    EXPECT_THROW(kdb.rename(old_name, "1_NEW_NAME"), std::runtime_error);
    // - name using forbidden special characters
    EXPECT_THROW(kdb.rename(old_name, "NËW_N@ME"), std::runtime_error);
    // - old name does not exists
    EXPECT_THROW(kdb.rename("EMPTY", "NEW_NAME"), std::runtime_error);
    // - new name already exists
    new_name = kdb.get_name(3);
    EXPECT_THROW(kdb.rename(old_name, new_name), std::runtime_error);
    
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

TEST_F(KDBCommentsTest, CreateRemove)
{
    std::string name = "NEW_COMMENT";
    Comment new_comment = "This is a new comment";

    kdb.add(name, new_comment);
    EXPECT_EQ(kdb.get(name), new_comment);

    kdb.remove(name);
    EXPECT_THROW(kdb.get(name), std::runtime_error);

    // expect errors 
    std::string old_name = kdb.get_name(2);
    // - too long name
    EXPECT_THROW(kdb.add("ABCDEFGHIJKLMNOPQRSTUVWXYZ", new_comment), std::runtime_error);
    // - name starting with a umber
    EXPECT_THROW(kdb.add("1_NEW_NAME", new_comment), std::runtime_error);
    // - name using forbidden special characters
    EXPECT_THROW(kdb.add("NËW_N@ME", new_comment), std::runtime_error);
    // - name already exists
    name = kdb.get_name(3);
    EXPECT_THROW(kdb.add(name, new_comment), std::runtime_error);
}

TEST_F(KDBCommentsTest, Update)
{
    std::string name = kdb.get_name(0);
    Comment new_comment = "New Comment";

    // by name
    kdb.update(name, new_comment);
    EXPECT_EQ(kdb.get(name), new_comment);

    // error: name does not exist
    EXPECT_THROW(kdb.update("UNKNOWN", new_comment), std::runtime_error);

    // by position
    kdb.update(1, new_comment);
    EXPECT_EQ(kdb.get(1), new_comment);

    // error: position does not exist
    int beyond_last_pos = kdb.count() + 10;
    EXPECT_THROW(kdb.update(beyond_last_pos, new_comment), std::runtime_error);
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
    EXPECT_THROW(kdb.copy("UNKNOWN"), std::runtime_error);

    // error: position does not exist
    int beyond_last_pos = kdb.count() + 10;
    EXPECT_THROW(kdb.copy(beyond_last_pos), std::runtime_error);
}
