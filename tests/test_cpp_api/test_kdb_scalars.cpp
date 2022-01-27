#include "pch.h"
#include "../../cpp_api/iode_cpp_api.h"

#include <filesystem>


// To get current path
// std::filesystem::path cwd = std::filesystem::current_path();
// std::string str_path = cwd.string();

std::string input_test_dir = "../../tests/data/";
std::string output_test_dir = "../../tests/output/";


class KDBScalarsTest : public ::testing::Test {
protected:
    KDBScalars kdb;

    void SetUp() override
    {
        kdb.load(input_test_dir + "fun.scl");
    }

    // void TearDown() override {}
};


TEST(KDBScalarsTest_, Load)
{
    KDBScalars kdb;
    kdb.load(input_test_dir + "fun.scl");
    EXPECT_EQ(kdb.count(), 161);
}

TEST_F(KDBScalarsTest, Get)
{
    int pos = 0;
    Scalar* scalar;
    Scalar expected_scalar;
    expected_scalar.val = 0.015768406912684441;
    expected_scalar.relax = 1.;
    expected_scalar.std = 0.0013687137980014086;

    // by position
    scalar = kdb.get(pos);
    EXPECT_DOUBLE_EQ(expected_scalar.val, scalar->val);
    EXPECT_DOUBLE_EQ(expected_scalar.relax, scalar->relax);
    EXPECT_DOUBLE_EQ(expected_scalar.std, scalar->std);

    // by name
    std::string name = kdb.getName(pos);
    scalar = kdb.get(name);
    EXPECT_DOUBLE_EQ(expected_scalar.val, scalar->val);
    EXPECT_DOUBLE_EQ(expected_scalar.relax, scalar->relax);
    EXPECT_DOUBLE_EQ(expected_scalar.std, scalar->std);
}
