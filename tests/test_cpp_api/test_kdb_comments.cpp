#include "pch.h"
#include "../../cpp_api/iode_cpp_api.h"

#include <filesystem>


// To get current path
// std::filesystem::path cwd = std::filesystem::current_path();
// std::string str_path = cwd.string();

std::string input_test_dir = "../../tests/data/";
std::string output_test_dir = "../../tests/output/";


class KDBCommentsTest : public ::testing::Test {
protected:
    KDBComments kdb;

    void SetUp() override 
    {
        kdb.load(input_test_dir + "fun.cmt");
    }

    // void TearDown() override {}
};


TEST(KDBCommentsTest_, Load)
{
    KDBComments kdb;
    kdb.load(input_test_dir + "fun.cmt");
    EXPECT_EQ(kdb.count(), 317);
}


TEST_F(KDBCommentsTest, GetName)
{
    std::string name; 
    
    name = kdb.getName(0);
    EXPECT_EQ(name, "ACAF");
    
    EXPECT_THROW(kdb.getName(400), std::runtime_error);
}

TEST_F(KDBCommentsTest, Rename)
{
    int new_pos;
    std::string old_name;
    std::string new_name;

    // rename
    old_name = kdb.getName(0);
    EXPECT_EQ(old_name, "ACAF");
    new_pos = kdb.rename(old_name, "NEW_NAME");
    // temporary fix
    new_pos -= 1;
    EXPECT_EQ(kdb.getName(new_pos), "NEW_NAME");

    // expect errors 
    old_name = kdb.getName(2);
    // - too long name
    EXPECT_THROW(kdb.rename(old_name, "ABCDEFGHIJKLMNOPQRSTUVWXYZ"), std::runtime_error);
    // - old name does not exists
    EXPECT_THROW(kdb.rename("EMPTY", "NEW_NAME"), std::runtime_error);
    // - new name already exists
    new_name = kdb.getName(3);
    EXPECT_THROW(kdb.rename(old_name, new_name), std::runtime_error);
    
    
    // set by position
    new_pos = kdb.setName(1, "NEW_POS");
    // temporary fix
    new_pos -= 1;
    EXPECT_EQ(kdb.getName(new_pos), "NEW_POS");
}


TEST_F(KDBCommentsTest, Get)
{
    std::string name = kdb.getName(0);
    std::string expected_comment = "Ondernemingen: ontvangen kapitaaloverdrachten.";

    // get by name
    EXPECT_EQ(kdb.get(name), expected_comment);

    // get by position
    EXPECT_EQ(kdb.get(0), expected_comment);
}


TEST_F(KDBCommentsTest, Set)
{
    std::string name = kdb.getName(0);
    std::string new_comment = "New Comment";

    // by name
    kdb.set(name, new_comment);
    EXPECT_EQ(kdb.get(name), new_comment);

    // by position
    kdb.set(1, new_comment);
    EXPECT_EQ(kdb.get(1), new_comment);
}
