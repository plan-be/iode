#include "pch.h"


class KDBScalarsTest : public KDBTest, public ::testing::Test
{
protected:
    KDBScalars kdb;

    void SetUp() override
    {
        kdb.load(input_test_dir + "fun.scl");
    }

    // void TearDown() override {}
};


TEST_F(KDBScalarsTest, Load)
{
    KDBScalars kdb;
    kdb.load(input_test_dir + "fun.scl");
    EXPECT_EQ(kdb.count(), 161);
}

TEST_F(KDBScalarsTest, Save)
{
    // save in binary format
    kdb.save(output_test_dir + "fun.scl");

    // save in ascii format
    kdb.save(output_test_dir + "fun.as");
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
