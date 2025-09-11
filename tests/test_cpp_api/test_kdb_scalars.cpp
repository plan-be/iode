#include "pch.h"


class KDBScalarsTest : public KDBTest, public ::testing::Test
{
protected:
    void SetUp() override
    {
        KDBScalars kdb_scl(input_test_dir + "fun.as");
    }

    // void TearDown() override {}
};


TEST_F(KDBScalarsTest, Load)
{
    KDBScalars kdb(input_test_dir + prefix_filename + "fun.scl");
    EXPECT_EQ(kdb.count(), 161);
}

TEST_F(KDBScalarsTest, Subset)
{
    std::string pattern = "a*";
    Scalar* scalar = Scalars.get("acaf1");
    double value = 0.0158;
    double relax = 0.98;
    double std = 0.0;
    Scalar* new_scalar = new Scalar(value, relax, std);

    // GLOBAL KDB
    KDBScalars kdb_global;
    EXPECT_EQ(kdb_global.count(), 161);
    EXPECT_TRUE(kdb_global.is_global_database());

    // DEEP COPY SUBSET
    KDBScalars* kdb_subset_deep_copy = kdb_global.subset(pattern, true);
    std::vector<std::string> names = kdb_global.get_names(pattern);
    EXPECT_EQ(kdb_subset_deep_copy->count(), names.size());
    EXPECT_TRUE(kdb_subset_deep_copy->is_local_database());
    kdb_subset_deep_copy->update("acaf1", value, relax, std);
    EXPECT_EQ(*kdb_global.get("acaf1"), *scalar);
    EXPECT_EQ(*kdb_subset_deep_copy->get("acaf1"), *new_scalar);

    // SHALLOW COPY SUBSET
    KDBScalars* kdb_subset_shallow_copy = kdb_global.subset(pattern, false);
    EXPECT_EQ(kdb_subset_shallow_copy->count(), names.size());
    EXPECT_TRUE(kdb_subset_shallow_copy->is_shallow_copy_database());
    kdb_subset_shallow_copy->update("acaf1", value, relax, std);
    EXPECT_EQ(*kdb_global.get("acaf1"), *new_scalar);
    EXPECT_EQ(*kdb_subset_shallow_copy->get("acaf1"), *new_scalar);

    delete new_scalar;
}

TEST_F(KDBScalarsTest, Save)
{
    // save in binary format
    Scalars.save(output_test_dir + "fun.scl");

    // save in ascii format
    Scalars.save(output_test_dir + "fun.as");
}

TEST_F(KDBScalarsTest, Get)
{
    int pos = 0;
    Scalar expected_scalar(0.015768406912684, 1.0, 0.0013687137980013999);

    // by position
    Scalar* scalar = Scalars.get(pos);
    EXPECT_DOUBLE_EQ(expected_scalar.value, scalar->value);
    EXPECT_DOUBLE_EQ(expected_scalar.relax, scalar->relax);
    EXPECT_DOUBLE_EQ(expected_scalar.std, scalar->std);

    // by name
    std::string name = Scalars.get_name(pos);
    Scalar* scalar2 = Scalars.get(name);
    EXPECT_DOUBLE_EQ(expected_scalar.value, scalar2->value);
    EXPECT_DOUBLE_EQ(expected_scalar.relax, scalar2->relax);
    EXPECT_DOUBLE_EQ(expected_scalar.std, scalar2->std);
}

TEST_F(KDBScalarsTest, GetNames)
{
    std::vector<std::string> expected_names;
    for (int i=0; i < Scalars.count(); i++) expected_names.push_back(Scalars.get_name(i));
    std::vector<std::string> names = Scalars.get_names();
    EXPECT_EQ(names, expected_names);
}

TEST_F(KDBScalarsTest, CreateRemove)
{
    std::string name = "new_scalar";
    double value = 0.012365879;
    double relax = 1.;

    Scalars.add(name, value, relax);
    Scalar* new_scalar = Scalars.get(name);
    EXPECT_DOUBLE_EQ(new_scalar->value, value);
    EXPECT_DOUBLE_EQ(new_scalar->relax, relax);
    EXPECT_TRUE(new_scalar->std < 1e-100);

    Scalars.remove(name);
    EXPECT_THROW(Scalars.get(name), std::invalid_argument);
}

TEST_F(KDBScalarsTest, Update)
{
    std::string name = Scalars.get_name(0);
    double value = 0.0158;
    double relax = 0.98;

    Scalars.update(name, value, relax);
    Scalar* updated_scalar = Scalars.get(name);
    EXPECT_DOUBLE_EQ(updated_scalar->value, value);
    EXPECT_DOUBLE_EQ(updated_scalar->relax, relax);
    EXPECT_TRUE(updated_scalar->std < 1e-100);
}

TEST_F(KDBScalarsTest, Copy)
{
    std::string name = Scalars.get_name(0);
    Scalar* scalar = Scalars.get(name);

    Scalar* scalar_copy = Scalars.copy(name);
    EXPECT_EQ(*scalar_copy, *scalar);

    // add copy
    Scalars.add("dup_" + name, *scalar_copy);
}

TEST_F(KDBScalarsTest, Filter)
{
    std::string pattern = "a*;*_";
    std::vector<std::string> expected_names;
    KDBScalars* kdb_subset;

    std::vector<std::string> all_names;
    for (int p = 0; p < Scalars.count(); p++) all_names.push_back(Scalars.get_name(p));

    int nb_total_scalars = Scalars.count();
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
    kdb_subset = Scalars.subset(pattern);
    EXPECT_EQ(kdb_subset->count(), expected_names.size());
    EXPECT_EQ(kdb_subset->get_names(), expected_names);

    // modify an element of the local KDB and check if the 
    // corresponding element of the global KDB also changes
    std::string name = "acaf1";
    double updated_value = 0.0158;
    double updated_relax = 0.98;
    double updated_std = 0.0;
    Scalar* expected_updated_scalar = new Scalar(updated_value, updated_relax, updated_std);
    kdb_subset->update(name, updated_value, updated_relax, updated_std);
    Scalar* updated_scalar_local = kdb_subset->get(name);
    EXPECT_EQ(*updated_scalar_local, *expected_updated_scalar);
    Scalar* updated_scalar_global = Scalars.get(name);
    EXPECT_EQ(*updated_scalar_global, *expected_updated_scalar);

    // add an element to the local KDB and check if it has also 
    // been added to the global KDB
    std::string new_name = "new_scalar";
    double value = 0.012365879;
    double relax = 1.0;
    double std = 0.0;
    Scalar* expected_new_scalar = new Scalar(value, relax, std);
    kdb_subset->add(new_name, value, relax, std);
    Scalar* new_scalar_local = kdb_subset->get(new_name);
    EXPECT_EQ(*new_scalar_local, *expected_new_scalar);
    Scalar* new_scalar_global = Scalars.get(new_name);
    EXPECT_EQ(*new_scalar_global, *expected_new_scalar);
    delete expected_new_scalar;

    // rename an element in the local KDB and check if the 
    // corresponding element has also been renamed in the global KDB
    std::string old_name = new_name;
    Scalar* old_scalar = Scalars.get(old_name);
    new_name = "scalar_new";
    kdb_subset->rename(old_name, new_name);
    new_scalar_local = kdb_subset->get(new_name);
    EXPECT_EQ(*new_scalar_local, *old_scalar);
    new_scalar_global = Scalars.get(new_name);
    EXPECT_EQ(*new_scalar_global, *old_scalar);

    // delete an element from the local KDB and check if it has also 
    // been deleted from the global KDB
    kdb_subset->remove(new_name);
    EXPECT_FALSE(kdb_subset->contains(new_name));
    EXPECT_FALSE(Scalars.contains(new_name));

    // try to add an element to the local KDB which is already present 
    // in the global KDB
    EXPECT_THROW(kdb_subset->add("gamma_", *new_scalar_local), std::invalid_argument);

    // delete local kdb
    delete kdb_subset;
    EXPECT_EQ(Scalars.count(), nb_total_scalars);
    updated_scalar_global = Scalars.get(name);
    EXPECT_EQ(*updated_scalar_global, *expected_updated_scalar);
    delete expected_updated_scalar;

    // wrong pattern
    pattern = "anjfks";
    EXPECT_THROW(Scalars.subset(pattern), std::runtime_error);
}

TEST_F(KDBScalarsTest, DeepCopy)
{
    std::string pattern = "a*;*_";
    std::vector<std::string> expected_names;
    KDBScalars* kdb_subset;

    std::vector<std::string> all_names;
    for (int p = 0; p < Scalars.count(); p++) all_names.push_back(Scalars.get_name(p));

    int nb_total_scalars = Scalars.count();
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
    kdb_subset = Scalars.subset(pattern, true);
    EXPECT_EQ(kdb_subset->count(), expected_names.size());
    EXPECT_EQ(kdb_subset->get_names(), expected_names);

    // modify an element of the local KDB and check if the 
    // corresponding element of the global KDB didn't changed
    std::string name = "acaf1";
    Scalar* scalar = Scalars.get(name);
    double value = scalar->value;
    double relax = scalar->relax;
    double std = scalar->std;
    Scalar* expected_scalar = new Scalar(value, relax, std);
    double updated_value = 0.0158;
    double updated_relax = 0.98;
    double updated_std = 0.0;
    Scalar* expected_updated_scalar = new Scalar(updated_value, updated_relax, updated_std);
    kdb_subset->update(name, updated_value, updated_relax, updated_std);
    Scalar* updated_scalar_local = kdb_subset->get(name);
    EXPECT_EQ(*updated_scalar_local, *expected_updated_scalar);
    Scalar* scalar_global = Scalars.get(name); 
    EXPECT_EQ(*scalar_global, *expected_scalar);
    delete expected_scalar;
    delete expected_updated_scalar;

    // add an element to the local KDB and check if it has not 
    // been added to the global KDB
    std::string new_name = "new_scalar";
    value = 0.012365879;
    relax = 1.0;
    std = 0.0;
    Scalar* expected_new_scalar = new Scalar(value, relax, std);
    kdb_subset->add(new_name, value, relax, std);
    EXPECT_TRUE(kdb_subset->contains(new_name));
    Scalar* new_scalar_local = kdb_subset->get(new_name);
    EXPECT_EQ(*new_scalar_local, *expected_new_scalar);
    EXPECT_FALSE(Scalars.contains(new_name));
    delete expected_new_scalar;

    // rename an element in the local KDB and check if the 
    // corresponding element has not been renamed in the global KDB
    name = "acaf2";
    Scalar* old_scalar = Scalars.get(name);
    new_name = "scalar_new";
    kdb_subset->rename(name, new_name);
    EXPECT_TRUE(kdb_subset->contains(new_name));
    new_scalar_local = kdb_subset->get(new_name);
    EXPECT_EQ(*new_scalar_local, *old_scalar);
    EXPECT_FALSE(Scalars.contains(new_name));

    // delete an element from the local KDB and check if it has not 
    // been deleted from the global KDB
    name = "acaf3";
    kdb_subset->remove(name);
    EXPECT_FALSE(kdb_subset->contains(name));
    EXPECT_TRUE(Scalars.contains(name));

    // delete local kdb
    delete kdb_subset;
    EXPECT_EQ(Scalars.count(), nb_total_scalars);

    // wrong pattern
    pattern = "anjfks";
    EXPECT_THROW(Scalars.subset(pattern, true), std::runtime_error);
}

TEST_F(KDBScalarsTest, CopyFrom)
{
    std::string pattern = "a* *_";
    std::string filename = input_test_dir + prefix_filename + "fun.scl";
    int expected_nb_comments = Scalars.count();
    std::vector<std::string> v_expected_names;

    // Copy entire file
    Scalars.clear();
    Scalars.copy_from(filename, "*");
    EXPECT_EQ(Scalars.count(), expected_nb_comments); 

    // copy subset
    v_expected_names = Scalars.get_names(pattern);
    Scalars.clear();
    Scalars.copy_from(filename, pattern);
    EXPECT_EQ(Scalars.count(), v_expected_names.size());  
    EXPECT_EQ(Scalars.get_names(), v_expected_names);  
}

TEST_F(KDBScalarsTest, Merge)
{
    std::string pattern = "a*";

    // create deep copies kdb
    KDBScalars* kdb0 = Scalars.subset(pattern, true);
    KDBScalars* kdb1 = Scalars.subset(pattern, true);
    KDBScalars* kdb_to_merge = Scalars.subset(pattern, true);

    // add an element to the KDB to be merged
    std::string new_name = "new_scalar";
    double value = 0.012365879;
    double relax = 1.0;
    double std = 0.0;
    Scalar* new_scalar = new Scalar(value, relax, std);
    kdb_to_merge->add(new_name, value, relax, std);

    // modify an existing element of the KDB to be merge
    std::string name = "acaf1";
    Scalar* unmodified_scalar = kdb_to_merge->get(name);
    double updated_value = 0.0158;
    double updated_relax = 0.98;
    double updated_std = 0.0;
    Scalar* modified_scalar = new Scalar(updated_value, updated_relax, updated_std);
    kdb_to_merge->update(name, updated_value, updated_relax, updated_std);

    // merge (overwrite)
    kdb0->merge(*kdb_to_merge, true);
    // a) check kdb0 contains new item of KDB to be merged
    EXPECT_TRUE(kdb0->contains(new_name));
    EXPECT_EQ(*kdb0->get(new_name), *new_scalar);
    // b) check already existing item has been overwritten
    EXPECT_EQ(*kdb0->get(name), *modified_scalar); 

    // merge (NOT overwrite)
    kdb1->merge(*kdb_to_merge, false);
    // b) check already existing item has NOT been overwritten
    EXPECT_EQ(*kdb1->get(name), *unmodified_scalar);

    delete new_scalar;
    delete modified_scalar;
}

TEST_F(KDBScalarsTest, Search)
{
    std::string scl_name = "gamma_";
    std::vector<std::string> objs_list;

    KDBComments kdb_cmt(input_test_dir + "fun.ac");
    KDBEquations kdb_eqs(input_test_dir + "fun.ae");
    KDBIdentities kdb_idt(input_test_dir + "fun.ai");
    KDBLists kdb_lst(input_test_dir + "fun.al");
    KDBTables kdb_tbl(input_test_dir + "fun.at");
    KDBVariables kdb_var(input_test_dir + "fun.av");

    objs_list = Comments.search(scl_name);
    EXPECT_EQ(objs_list.size(), 0);

    std::vector<std::string> expected_eqs = { "W" };
    objs_list = Equations.search(scl_name);
    EXPECT_EQ(objs_list, expected_eqs);

    std::vector<std::string> expected_idt = { "NAWRU" };
    objs_list = Identities.search(scl_name);
    EXPECT_EQ(objs_list, expected_idt);

    objs_list = Lists.search(scl_name);
    EXPECT_EQ(objs_list.size(), 0);

    std::vector<std::string> expected_scl = { scl_name };
    objs_list = Scalars.search(scl_name);
    EXPECT_EQ(objs_list, expected_scl);

    objs_list = Tables.search(scl_name);
    EXPECT_EQ(objs_list.size(), 0);

    objs_list = Variables.search(scl_name);
    EXPECT_EQ(objs_list.size(), 0);
}

TEST_F(KDBScalarsTest, Hash)
{
    std::size_t hash_val = hash_value(Scalars);

    // change a name
    Scalars.rename("acaf1", "new_name");
    std::size_t hash_val_modified = hash_value(Scalars);
    EXPECT_NE(hash_val, hash_val_modified);
    std::cout << "(rename scalar) orignal vs modified hash: " << std::to_string(hash_val) << " vs " << std::to_string(hash_val_modified) << std::endl;

    // modify an entry
    hash_val = hash_val_modified;
    double value = 0.0158;
    double relax = 0.98;
    Scalars.update("new_name", value, relax);
    hash_val_modified = hash_value(Scalars);
    EXPECT_NE(hash_val, hash_val_modified);
    std::cout << "(modify scalar) orignal vs modified hash: " << std::to_string(hash_val) << " vs " << std::to_string(hash_val_modified) << std::endl;

    // remove an entry
    hash_val = hash_val_modified;
    Scalars.remove("new_name");
    hash_val_modified = hash_value(Scalars);
    EXPECT_NE(hash_val, hash_val_modified);
    std::cout << "(delete scalar) orignal vs modified hash: " << std::to_string(hash_val) << " vs " << std::to_string(hash_val_modified) << std::endl;

    // add an entry
    hash_val = hash_val_modified;
    value = 0.012365879;
    relax = 1.;
    Scalars.add("new_entry", value, relax);
    hash_val_modified = hash_value(Scalars);
    EXPECT_NE(hash_val, hash_val_modified);   
    std::cout << "(new    scalar) orignal vs modified hash: " << std::to_string(hash_val) << " vs " << std::to_string(hash_val_modified) << std::endl; 
}