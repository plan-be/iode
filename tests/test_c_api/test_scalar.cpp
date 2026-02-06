#include "pch.h"


class ScalarTest : public TestAbstract, public ::testing::Test
{
protected:

	void SetUp() override 
    {
        global_ws_scl->load(str_input_test_dir + "fun.as");
    }

	void TearDown() override 
    {
        global_ws_scl->clear();
    }
};


TEST_F(ScalarTest, AddGetScalar)
{
    double val = 1.25;
    double relax = 0.9;
    double std = 6.5;
    std::string name = "scalar";

    Scalar* scalar = new Scalar(val, relax, std);
    global_ws_scl->set_obj_ptr(name, scalar);
    bool found = global_ws_scl->contains(name);
    ASSERT_TRUE(found);

    Scalar* scl = global_ws_scl->get_obj_ptr(name);
    ASSERT_EQ(scl->value, val);
    ASSERT_EQ(scl->relax, relax);
    ASSERT_EQ(scl->std, std);

    std::hash<Scalar> scl_hasher;
    std::size_t c_hash = scl_hasher(*scl);
    std::size_t cpp_hash = scl_hasher(*scalar);
    ASSERT_EQ(c_hash, cpp_hash);
}

TEST_F(ScalarTest, Hash)
{
    std::hash<Scalar> scalar_hasher;
    std::size_t hash_before;
    std::size_t hash_after;

    Scalar scalar = global_ws_scl->get("acaf1");
    hash_before = scalar_hasher(scalar);

    // same scalar
    Scalar same_scalar = global_ws_scl->get("acaf1");
    EXPECT_EQ(scalar, same_scalar);
    hash_after = scalar_hasher(same_scalar);
    EXPECT_EQ(hash_before, hash_after);

    // different value
    hash_before = hash_after;
    scalar.value += 0.1;
    hash_after = scalar_hasher(scalar);
    EXPECT_NE(hash_before, hash_after);

    // different relax
    hash_before = hash_after;
    scalar.relax += 0.1;
    hash_after = scalar_hasher(scalar);
    EXPECT_NE(hash_before, hash_after);

    // different std
    hash_before = hash_after;
    scalar.std += 0.1;
    hash_after = scalar_hasher(scalar);
    EXPECT_NE(hash_before, hash_after);
}
