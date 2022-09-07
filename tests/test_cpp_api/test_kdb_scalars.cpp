#include "pch.h"


class KDBScalarsTest : public KDBTest, public ::testing::Test
{
protected:
    KDBScalars kdb;

    void SetUp() override
    {
        load_global_kdb(I_SCALARS, input_test_dir + "fun.scl");
    }

    // void TearDown() override {}
};


TEST_F(KDBScalarsTest, Load)
{
    KDBScalars kdb2;
    EXPECT_EQ(kdb2.count(), 161);
}

TEST_F(KDBScalarsTest, Save)
{
    // save in binary format
    save_global_kdb(I_SCALARS, output_test_dir + "fun.scl");
    kdb.dump(output_test_dir + "fun.scl");

    // save in ascii format
    save_global_kdb(I_SCALARS, output_test_dir + "fun.as");
    kdb.dump(output_test_dir + "fun.as");
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
    IODE_REAL value = 0.012365879;
    IODE_REAL relax = 1.;

    kdb.add(name, value, relax);
    Scalar new_scalar = kdb.get(name);
    EXPECT_DOUBLE_EQ(new_scalar.val, value);
    EXPECT_DOUBLE_EQ(new_scalar.relax, relax);
    EXPECT_TRUE(new_scalar.std < 1e-100);

    kdb.remove(name);
    EXPECT_THROW(kdb.get(name), IodeExceptionFunction);
}

TEST_F(KDBScalarsTest, Update)
{
    std::string name = kdb.get_name(0);
    IODE_REAL value = 0.0158;
    IODE_REAL relax = 0.98;

    kdb.update(name, value, relax);
    Scalar updated_scalar = kdb.get(name);
    EXPECT_DOUBLE_EQ(updated_scalar.val, value);
    EXPECT_DOUBLE_EQ(updated_scalar.relax, relax);
    EXPECT_TRUE(updated_scalar.std < 1e-100);
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

TEST_F(KDBScalarsTest, Filter)
{
    std::string pattern = "a*;*_";
    std::vector<std::string> expected_names;
    KDBScalars* local_kdb;
    KDBScalars global_kdb;

    std::vector<std::string> all_names;
    for (int p = 0; p < global_kdb.count(); p++) all_names.push_back(global_kdb.get_name(p));

    int nb_total_comments = global_kdb.count();
    // a*
    for (const std::string& name : all_names) if (name.front() == 'a') expected_names.push_back(name);
    // *_
    for (const std::string& name : all_names) if (name.back() == '_') expected_names.push_back(name);

    // remove duplicate entries
    // NOTE: std::unique only removes consecutive duplicated elements, 
    //       so the vector needst to be sorted first
    std::sort(expected_names.begin(), expected_names.end());
    std::vector<std::string>::iterator it = std::unique(expected_names.begin(), expected_names.end());  
    expected_names.resize(std::distance(expected_names.begin(), it));

    // create local kdb
    local_kdb = new KDBScalars(pattern);
    EXPECT_EQ(local_kdb->count(), expected_names.size());

    // modify an element of the local KDB and check if the 
    // corresponding element of the global KDB also changes
    Scalar updated_scalar;
    std::string name = "acaf1";
    IODE_REAL updated_value = 0.0158;
    IODE_REAL updated_relax = 0.98;
    local_kdb->update(name, updated_value, updated_relax);
    updated_scalar = local_kdb->get(name);
    EXPECT_DOUBLE_EQ(updated_scalar.val, updated_value);
    EXPECT_DOUBLE_EQ(updated_scalar.relax, updated_relax);
    updated_scalar = global_kdb.get(name);
    EXPECT_DOUBLE_EQ(updated_scalar.val, updated_value);
    EXPECT_DOUBLE_EQ(updated_scalar.relax, updated_relax);

    // add an element to the local KDB and check if it has also 
    // been added to the global KDB
    std::string new_name = "new_scalar";
    IODE_REAL value = 0.012365879;
    IODE_REAL relax = 1.;
    local_kdb->add(new_name, value, relax);
    Scalar new_scalar_local = local_kdb->get(new_name);
    EXPECT_DOUBLE_EQ(new_scalar_local.val, value);
    EXPECT_DOUBLE_EQ(new_scalar_local.relax, relax);
    Scalar new_scalar = global_kdb.get(new_name);
    EXPECT_DOUBLE_EQ(new_scalar.val, value);
    EXPECT_DOUBLE_EQ(new_scalar.relax, relax);

    // rename an element in the local KDB and check if the 
    // corresponding element has also been renamed in the global KDB
    std::string old_name = new_name;
    new_name = "scalar_new";
    local_kdb->rename(old_name, new_name);
    new_scalar_local = local_kdb->get(new_name);
    EXPECT_DOUBLE_EQ(new_scalar_local.val, value);
    EXPECT_DOUBLE_EQ(new_scalar_local.relax, relax);
    new_scalar = global_kdb.get(new_name);
    EXPECT_DOUBLE_EQ(new_scalar.val, value);
    EXPECT_DOUBLE_EQ(new_scalar.relax, relax);

    // delete an element from the local KDB and check if it has also 
    // been deleted from the global KDB
    local_kdb->remove(new_name);
    EXPECT_FALSE(local_kdb->contains(new_name));
    EXPECT_FALSE(global_kdb.contains(new_name));

    // delete local kdb
    delete local_kdb;
    EXPECT_EQ(global_kdb.count(), nb_total_comments);
    updated_scalar = global_kdb.get(name);
    EXPECT_DOUBLE_EQ(updated_scalar.val, updated_value);
    EXPECT_DOUBLE_EQ(updated_scalar.relax, updated_relax);
}

TEST_F(KDBScalarsTest, HardCopy)
{
    std::string pattern = "a*;*_";
    std::vector<std::string> expected_names;
    KDBScalars* local_kdb;
    KDBScalars global_kdb;

    std::vector<std::string> all_names;
    for (int p = 0; p < global_kdb.count(); p++) all_names.push_back(global_kdb.get_name(p));

    int nb_total_comments = global_kdb.count();
    // a*
    for (const std::string& name : all_names) if (name.front() == 'a') expected_names.push_back(name);
    // *_
    for (const std::string& name : all_names) if (name.back() == '_') expected_names.push_back(name);

    // remove duplicate entries
    // NOTE: std::unique only removes consecutive duplicated elements, 
    //       so the vector needst to be sorted first
    std::sort(expected_names.begin(), expected_names.end());
    std::vector<std::string>::iterator it = std::unique(expected_names.begin(), expected_names.end());  
    expected_names.resize(std::distance(expected_names.begin(), it));

    // create local kdb
    local_kdb = new KDBScalars(pattern, false);
    EXPECT_EQ(local_kdb->count(), expected_names.size());

    // modify an element of the local KDB and check if the 
    // corresponding element of the global KDB didn't changed
    std::string name = "acaf1";
    Scalar scalar;
    scalar = global_kdb.get(name);
    IODE_REAL value = scalar.val;
    IODE_REAL relax = scalar.relax;
    Scalar updated_scalar;
    IODE_REAL updated_value = 0.0158;
    IODE_REAL updated_relax = 0.98;
    local_kdb->update(name, updated_value, updated_relax);
    EXPECT_DOUBLE_EQ(scalar.val, value);
    EXPECT_DOUBLE_EQ(scalar.relax, relax);
    updated_scalar = local_kdb->get(name);
    EXPECT_DOUBLE_EQ(updated_scalar.val, updated_value);
    EXPECT_DOUBLE_EQ(updated_scalar.relax, updated_relax);

    // add an element to the local KDB and check if it has not 
    // been added to the global KDB
    std::string new_name = "new_scalar";
    value = 0.012365879;
    relax = 1.;
    local_kdb->add(new_name, value, relax);
    EXPECT_TRUE(local_kdb->contains(new_name));
    Scalar new_scalar_local = local_kdb->get(new_name);
    EXPECT_DOUBLE_EQ(new_scalar_local.val, value);
    EXPECT_DOUBLE_EQ(new_scalar_local.relax, relax);
    EXPECT_FALSE(global_kdb.contains(new_name));

    // rename an element in the local KDB and check if the 
    // corresponding element has not been renamed in the global KDB
    name = "acaf2";
    value = -7.9615019785706e-06;
    new_name = "scalar_new";
    local_kdb->rename(name, new_name);
    EXPECT_TRUE(local_kdb->contains(new_name));
    new_scalar_local = local_kdb->get(new_name);
    EXPECT_DOUBLE_EQ(new_scalar_local.val, value);
    EXPECT_DOUBLE_EQ(new_scalar_local.relax, relax);
    EXPECT_FALSE(global_kdb.contains(new_name));

    // delete an element from the local KDB and check if it has not 
    // been deleted from the global KDB
    name = "acaf3";
    local_kdb->remove(name);
    EXPECT_FALSE(local_kdb->contains(name));
    EXPECT_TRUE(global_kdb.contains(name));

    // delete local kdb
    delete local_kdb;
    EXPECT_EQ(global_kdb.count(), nb_total_comments);
}
