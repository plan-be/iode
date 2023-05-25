#include "pch.h"


class ScalarTest : public KDBTest, public ::testing::Test
{
protected:

	void SetUp() override 
    {
        load_global_kdb(I_SCALARS, input_test_dir + "fun.scl");
    }

	// void TearDown() override {}
};


TEST_F(ScalarTest, Hash)
{
    boost::hash<SCL> scalar_hasher;
    std::size_t hash_before;
    std::size_t hash_after;

    Scalar scalar("acaf1");
    hash_before = scalar_hasher(*scalar.c_scalar);

    // same scalar
    Scalar same_scalar("acaf1");
    EXPECT_EQ(scalar, same_scalar);
    hash_after = scalar_hasher(*same_scalar.c_scalar);
    EXPECT_EQ(hash_before, hash_after);

    // different value
    hash_before = hash_after;
    scalar.value() += 0.1;
    hash_after = scalar_hasher(*scalar.c_scalar);
    EXPECT_NE(hash_before, hash_after);

    // different relax
    hash_before = hash_after;
    scalar.relax() += 0.1;
    hash_after = scalar_hasher(*scalar.c_scalar);
    EXPECT_NE(hash_before, hash_after);

    // different std
    hash_before = hash_after;
    scalar.std() += 0.1;
    hash_after = scalar_hasher(*scalar.c_scalar);
    EXPECT_NE(hash_before, hash_after);
}
