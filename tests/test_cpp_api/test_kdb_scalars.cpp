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
    Scalar expected_scalar;
    expected_scalar.val = 0.015768406912684441;
    expected_scalar.relax = 1.;
    expected_scalar.std = 0.0013687137980014086;

    // by position
    Scalar scalar = kdb.get(pos);
    EXPECT_DOUBLE_EQ(expected_scalar.val, scalar.val);
    EXPECT_DOUBLE_EQ(expected_scalar.relax, scalar.relax);
    EXPECT_DOUBLE_EQ(expected_scalar.std, scalar.std);

    // by name
    std::string name = kdb.get_name(pos);
    Scalar scalar2 = kdb.get(name);
    EXPECT_DOUBLE_EQ(expected_scalar.val, scalar2.val);
    EXPECT_DOUBLE_EQ(expected_scalar.relax, scalar2.relax);
    EXPECT_DOUBLE_EQ(expected_scalar.std, scalar2.std);
}

TEST_F(KDBScalarsTest, CreateRemove)
{
    std::string name = "new_scalar";
    Scalar scalar;
    scalar.val = 0.012365879;
    scalar.relax = 1.;
    scalar.std = 0;

    // TODO : force std to be 0 whan adding a new scalar ?
    kdb.add(name, scalar);
    Scalar new_scalar = kdb.get(name);
    EXPECT_DOUBLE_EQ(new_scalar.val, scalar.val);
    EXPECT_DOUBLE_EQ(new_scalar.relax, scalar.relax);
    EXPECT_DOUBLE_EQ(new_scalar.std, scalar.std);

    kdb.remove(name);
    EXPECT_THROW(kdb.get(name), std::runtime_error);
}

TEST_F(KDBScalarsTest, Update)
{
    std::string name = kdb.get_name(0);
    Scalar scalar;
    scalar.val = 0.0158;
    scalar.relax = 0.98;
    scalar.std = 0.00137;

    kdb.update(name, scalar);
    Scalar updated_scalar = kdb.get(name);
    EXPECT_DOUBLE_EQ(updated_scalar.val, scalar.val);
    EXPECT_DOUBLE_EQ(updated_scalar.relax, scalar.relax);
    EXPECT_DOUBLE_EQ(updated_scalar.std, scalar.std);
}

TEST_F(KDBScalarsTest, Copy)
{
    std::string name = kdb.get_name(0);
    Scalar scalar = kdb.get(name);

    Scalar scalar_copy = kdb.copy(name);
    EXPECT_DOUBLE_EQ(scalar_copy.val, scalar.val);
    EXPECT_DOUBLE_EQ(scalar_copy.relax, scalar.relax);
    EXPECT_DOUBLE_EQ(scalar_copy.std, scalar.std);
}
