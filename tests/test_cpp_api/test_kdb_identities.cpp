#include "pch.h"
#include "../../cpp_api/iode_cpp_api.h"

#include <filesystem>


// To get current path
// std::filesystem::path cwd = std::filesystem::current_path();
// std::string str_path = cwd.string();

std::string input_test_dir = "../../tests/data/";
std::string output_test_dir = "../../tests/output/";


class KDBIdentitiesTest : public ::testing::Test {
protected:
    KDBIdentities kdb;

    void SetUp() override
    {
        kdb.load(input_test_dir + "fun.idt");
    }

    // void TearDown() override {}
};


TEST(KDBIdentitiesTest_, Load)
{
    KDBIdentities kdb;
    kdb.load(input_test_dir + "fun.idt");
    EXPECT_EQ(kdb.count(), 48);
}

TEST_F(KDBIdentitiesTest, GetLec)
{
    int pos = 0;
    std::string lec;
    std::string expected_lec = "((WCRH/QL)/(WCRH/QL)[1990Y1])*(VAFF/(VM+VAFF))[-1]+PM*(VM/(VM+VAFF))[-1]";

    // by position
    lec = kdb.getLec(pos);
    EXPECT_EQ(expected_lec, lec);

    // by name
    std::string name = kdb.getName(pos);
    lec = kdb.getLec(name);
    EXPECT_EQ(expected_lec, lec);
}
