#include "pch.h"


class ScalarTest : public KDBTest, public ::testing::Test
{
protected:

	void SetUp() override 
    {
        KDBScalars kdb_scl(input_test_dir + "fun.as");
    }

	// void TearDown() override {}
};


TEST_F(ScalarTest, Equivalence_C_CPP)
{
    double val = 1.25;
    double relax = 0.9;
    double std = 6.5;
    char* name = "cpp_scalar";

    // test if a Scalar object can be added to the Scalars KDB via add()
    Scalar scalar(val, relax, std);
    global_ws_scl->set(name, (char*) &scalar);
    bool found = global_ws_scl->contains(name);
    ASSERT_TRUE(found);

    Scalar* scl = KSVAL(global_ws_scl.get(), name);
    ASSERT_EQ(scl->value, val);
    ASSERT_EQ(scl->relax, relax);
    ASSERT_EQ(scl->std, std);

    // test memcpy between a Scalar object and a Scalar object
    scalar.value = 3.6;
    scalar.relax = 0.96;
    scalar.std = 12.3;
    memcpy(scl, &scalar, sizeof(Scalar));
    ASSERT_EQ(scl->value, scalar.value);
    ASSERT_EQ(scl->relax, scalar.relax);
    ASSERT_EQ(scl->std, scalar.std);

    // test if a Scalar object can be passed to the hash function for the objects of type Scalar.
    std::hash<Scalar> scl_hasher;
    std::size_t c_hash = scl_hasher(*scl);
    std::size_t cpp_hash = scl_hasher(static_cast<Scalar>(scalar));
    ASSERT_EQ(c_hash, cpp_hash);
}

TEST_F(ScalarTest, Hash)
{
    std::hash<Scalar> scalar_hasher;
    std::size_t hash_before;
    std::size_t hash_after;

    Scalar* scalar = Scalars.get("acaf1");
    hash_before = scalar_hasher(*scalar);

    // same scalar
    Scalar* same_scalar = Scalars.get("acaf1");
    EXPECT_EQ(scalar, same_scalar);
    hash_after = scalar_hasher(*same_scalar);
    EXPECT_EQ(hash_before, hash_after);

    // different value
    hash_before = hash_after;
    scalar->value += 0.1;
    hash_after = scalar_hasher(*scalar);
    EXPECT_NE(hash_before, hash_after);

    // different relax
    hash_before = hash_after;
    scalar->relax += 0.1;
    hash_after = scalar_hasher(*scalar);
    EXPECT_NE(hash_before, hash_after);

    // different std
    hash_before = hash_after;
    scalar->std += 0.1;
    hash_after = scalar_hasher(*scalar);
    EXPECT_NE(hash_before, hash_after);
}
