#include "pch.h"
#include "../../cpp_api/iode_cpp_api.h"

#include <filesystem>


// To get current path
// std::filesystem::path cwd = std::filesystem::current_path();
// std::string str_path = cwd.string();

std::string input_test_dir = "../../tests/data/";
std::string output_test_dir = "../../tests/output/";


class KDBListsTest : public ::testing::Test {
protected:
    KDBLists kdb;

    void SetUp() override
    {
        kdb.load(input_test_dir + "fun.lst");
    }

    // void TearDown() override {}
};


TEST(KDBListsTest_, Load)
{
    KDBLists kdb;
    kdb.load(input_test_dir + "fun.lst");
    EXPECT_EQ(kdb.count(), 16);
}

TEST_F(KDBListsTest, Get)
{
    int pos = 0;
    std::string list;
    std::string expected_list = "$COPY0;$COPY1;";

    // by position
    list = kdb.get(pos);
    EXPECT_EQ(expected_list, list);

    // by name
    std::string name = kdb.getName(pos);
    list = kdb.get(name);
    EXPECT_EQ(expected_list, list);
}
