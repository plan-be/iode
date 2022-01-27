#include "pch.h"
#include "../../cpp_api/iode_cpp_api.h"

#include <filesystem>


// To get current path
// std::filesystem::path cwd = std::filesystem::current_path();
// std::string str_path = cwd.string();

std::string input_test_dir = "../../tests/data/";
std::string output_test_dir = "../../tests/output/";


class KDBTablesTest : public ::testing::Test {
protected:
    KDBTables kdb;

    void SetUp() override
    {
        kdb.load(input_test_dir + "fun.tbl");
    }

    // void TearDown() override {}
};


TEST(KDBTablesTest_, Load)
{
    KDBTables kdb;
    kdb.load(input_test_dir + "fun.tbl");
    EXPECT_EQ(kdb.count(), 46);
}

TEST_F(KDBTablesTest, GetTitle)
{
    int pos = 0;
    std::string title;
    std::string expected_title = u8"Déterminants de la croissance de K";

    // by position
    title = kdb.getTitle(pos);
    EXPECT_EQ(expected_title, title);

    // by name
    std::string name = kdb.getName(pos);
    title = kdb.getTitle(name);
    EXPECT_EQ(expected_title, title);
}
