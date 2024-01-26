#include "pch.h"


class KDBScalarsTest : public KDBTest, public ::testing::Test
{
protected:
    void SetUp() override
    {
        KDBScalars kdb_scl(input_test_dir + "fun.scl");
    }

    // void TearDown() override {}
};


TEST_F(KDBScalarsTest, Load)
{
    KDBScalars kdb(input_test_dir + "fun.scl");
    EXPECT_EQ(kdb.count(), 161);
}

TEST_F(KDBScalarsTest, Subset)
{
    std::string pattern = "a*";
    Scalar scalar = Scalars.get("acaf1");
    IODE_REAL value = 0.0158;
    IODE_REAL relax = 0.98;
    IODE_REAL std = 0.0;
    Scalar new_scalar(value, relax, std);

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
    EXPECT_EQ(kdb_global.get("acaf1"), scalar);
    EXPECT_EQ(kdb_subset_deep_copy->get("acaf1"), new_scalar);

    // SHALLOW COPY SUBSET
    KDBScalars* kdb_subset_shallow_copy = kdb_global.subset(pattern, false);
    EXPECT_EQ(kdb_subset_shallow_copy->count(), names.size());
    EXPECT_TRUE(kdb_subset_shallow_copy->is_shallow_copy_database());
    kdb_subset_shallow_copy->update("acaf1", value, relax, std);
    EXPECT_EQ(kdb_global.get("acaf1"), new_scalar);
    EXPECT_EQ(kdb_subset_shallow_copy->get("acaf1"), new_scalar);
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
    Scalar expected_scalar(0.015768406912684441, 1.0, 0.0013687137980014086);

    // by position
    Scalar scalar = Scalars.get(pos);
    EXPECT_EQ(expected_scalar, scalar);

    // by name
    std::string name = Scalars.get_name(pos);
    Scalar scalar2 = Scalars.get(name);
    EXPECT_EQ(expected_scalar, scalar2);
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
    IODE_REAL value = 0.012365879;
    IODE_REAL relax = 1.;

    Scalars.add(name, value, relax);
    Scalar new_scalar = Scalars.get(name);
    EXPECT_DOUBLE_EQ(new_scalar.val, value);
    EXPECT_DOUBLE_EQ(new_scalar.relax, relax);
    EXPECT_TRUE(new_scalar.std < 1e-100);

    Scalars.remove(name);
    EXPECT_THROW(Scalars.get(name), std::invalid_argument);
}

TEST_F(KDBScalarsTest, Update)
{
    std::string name = Scalars.get_name(0);
    IODE_REAL value = 0.0158;
    IODE_REAL relax = 0.98;

    Scalars.update(name, value, relax);
    Scalar updated_scalar = Scalars.get(name);
    EXPECT_DOUBLE_EQ(updated_scalar.val, value);
    EXPECT_DOUBLE_EQ(updated_scalar.relax, relax);
    EXPECT_TRUE(updated_scalar.std < 1e-100);
}

TEST_F(KDBScalarsTest, Copy)
{
    std::string name = Scalars.get_name(0);
    Scalar scalar = Scalars.get(name);

    Scalar scalar_copy = Scalars.copy(name);
    EXPECT_EQ(scalar_copy, scalar);

    // add copy
    Scalars.add("dup_" + name, scalar_copy);
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
    IODE_REAL updated_value = 0.0158;
    IODE_REAL updated_relax = 0.98;
    IODE_REAL updated_std = 0.0;
    Scalar expected_updated_scalar(updated_value, updated_relax, updated_std);
    kdb_subset->update(name, updated_value, updated_relax, updated_std);
    Scalar updated_scalar_local = kdb_subset->get(name);
    EXPECT_EQ(updated_scalar_local, expected_updated_scalar);
    Scalar updated_scalar_global = Scalars.get(name);
    EXPECT_EQ(updated_scalar_global, expected_updated_scalar);

    // add an element to the local KDB and check if it has also 
    // been added to the global KDB
    std::string new_name = "new_scalar";
    IODE_REAL value = 0.012365879;
    IODE_REAL relax = 1.0;
    IODE_REAL std = 0.0;
    Scalar expected_new_scalar(value, relax, std);
    kdb_subset->add(new_name, value, relax, std);
    Scalar new_scalar_local = kdb_subset->get(new_name);
    EXPECT_EQ(new_scalar_local, expected_new_scalar);
    Scalar new_scalar_global = Scalars.get(new_name);
    EXPECT_EQ(new_scalar_global, expected_new_scalar);

    // rename an element in the local KDB and check if the 
    // corresponding element has also been renamed in the global KDB
    std::string old_name = new_name;
    Scalar old_scalar(old_name);
    new_name = "scalar_new";
    kdb_subset->rename(old_name, new_name);
    new_scalar_local = kdb_subset->get(new_name);
    EXPECT_EQ(new_scalar_local, old_scalar);
    new_scalar_global = Scalars.get(new_name);
    EXPECT_EQ(new_scalar_global, old_scalar);

    // delete an element from the local KDB and check if it has also 
    // been deleted from the global KDB
    kdb_subset->remove(new_name);
    EXPECT_FALSE(kdb_subset->contains(new_name));
    EXPECT_FALSE(Scalars.contains(new_name));

    // delete local kdb
    delete kdb_subset;
    EXPECT_EQ(Scalars.count(), nb_total_scalars);
    updated_scalar_global = Scalars.get(name);
    EXPECT_EQ(updated_scalar_global, expected_updated_scalar);

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
    Scalar scalar = Scalars.get(name);
    IODE_REAL value = scalar.val;
    IODE_REAL relax = scalar.relax;
    IODE_REAL std = scalar.std;
    Scalar expected_scalar(value, relax, std);
    IODE_REAL updated_value = 0.0158;
    IODE_REAL updated_relax = 0.98;
    IODE_REAL updated_std = 0.0;
    Scalar expected_updated_scalar(updated_value, updated_relax, updated_std);
    kdb_subset->update(name, updated_value, updated_relax, updated_std);
    Scalar updated_scalar_local = kdb_subset->get(name);
    EXPECT_EQ(updated_scalar_local, expected_updated_scalar);
    Scalar scalar_global = Scalars.get(name); 
    EXPECT_EQ(scalar_global, expected_scalar);

    // add an element to the local KDB and check if it has not 
    // been added to the global KDB
    std::string new_name = "new_scalar";
    value = 0.012365879;
    relax = 1.0;
    std = 0.0;
    Scalar expected_new_scalar(value, relax, std);
    kdb_subset->add(new_name, value, relax, std);
    EXPECT_TRUE(kdb_subset->contains(new_name));
    Scalar new_scalar_local = kdb_subset->get(new_name);
    EXPECT_EQ(new_scalar_local, expected_new_scalar);
    EXPECT_FALSE(Scalars.contains(new_name));

    // rename an element in the local KDB and check if the 
    // corresponding element has not been renamed in the global KDB
    name = "acaf2";
    Scalar old_scalar(name);
    new_name = "scalar_new";
    kdb_subset->rename(name, new_name);
    EXPECT_TRUE(kdb_subset->contains(new_name));
    new_scalar_local = kdb_subset->get(new_name);
    EXPECT_EQ(new_scalar_local, old_scalar);
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

TEST_F(KDBScalarsTest, Merge)
{
    std::string pattern = "A*";

    // create deep copies kdb
    KDBScalars* kdb0 = Scalars.subset(pattern, true);
    KDBScalars* kdb1 = Scalars.subset(pattern, true);
    KDBScalars* kdb_to_merge = Scalars.subset(pattern, true);

    // add an element to the KDB to be merged
    std::string new_name = "new_scalar";
    IODE_REAL value = 0.012365879;
    IODE_REAL relax = 1.0;
    IODE_REAL std = 0.0;
    Scalar new_scalar(value, relax, std);
    kdb_to_merge->add(new_name, value, relax, std);

    // modify an existing element of the KDB to be merge
    std::string name = "acaf1";
    Scalar unmodified_scalar = kdb_to_merge->get(name);
    IODE_REAL updated_value = 0.0158;
    IODE_REAL updated_relax = 0.98;
    IODE_REAL updated_std = 0.0;
    Scalar modified_scalar(updated_value, updated_relax, updated_std);
    kdb_to_merge->update(name, updated_value, updated_relax, updated_std);

    // merge (overwrite)
    kdb0->merge(*kdb_to_merge, true);
    // a) check kdb0 contains new item of KDB to be merged
    EXPECT_TRUE(kdb0->contains(new_name));
    EXPECT_EQ(kdb0->get(new_name), new_scalar);
    // b) check already existing item has been overwritten
    EXPECT_EQ(kdb0->get(name), modified_scalar); 

    // merge (NOT overwrite)
    kdb1->merge(*kdb_to_merge, false);
    // b) check already existing item has NOT been overwritten
    EXPECT_EQ(kdb1->get(name), unmodified_scalar);
}

TEST_F(KDBScalarsTest, AssociatedObjs)
{
    std::string name = "gamma";
    std::vector<std::string> objs_list;

    KDBComments kdb_cmt(input_test_dir + "fun.cmt");
    KDBEquations kdb_eqs(input_test_dir + "fun.eqs");
    KDBIdentities kdb_idt(input_test_dir + "fun.idt");
    KDBLists kdb_lst(input_test_dir + "fun.lst");
    KDBScalars kdb_scl(input_test_dir + "fun.scl");
    KDBTables kdb_tbl(input_test_dir + "fun.tbl");
    KDBVariables kdb_var(input_test_dir + "fun.var");

    objs_list = Scalars.get_associated_objects_list(name, I_COMMENTS);
    EXPECT_EQ(objs_list.size(), 0);

    std::vector<std::string> expected_eqs = { "W" };
    objs_list = Scalars.get_associated_objects_list(name, I_EQUATIONS);
    EXPECT_EQ(objs_list, expected_eqs);

    std::vector<std::string> expected_idt = { "NAWRU" };
    objs_list = Scalars.get_associated_objects_list(name, I_IDENTITIES);
    EXPECT_EQ(objs_list, expected_idt);

    objs_list = Scalars.get_associated_objects_list(name, I_LISTS);
    EXPECT_EQ(objs_list.size(), 0);

    std::vector<std::string> expected_scl = { name };
    objs_list = Scalars.get_associated_objects_list(name, I_SCALARS);
    EXPECT_EQ(objs_list, expected_scl);

    objs_list = Scalars.get_associated_objects_list(name, I_TABLES);
    EXPECT_EQ(objs_list.size(), 0);

    objs_list = Scalars.get_associated_objects_list(name, I_VARIABLES);
    EXPECT_EQ(objs_list.size(), 0);
}

TEST_F(KDBScalarsTest, Hash)
{
    boost::hash<KDBScalars> kdb_hasher;
    std::size_t hash_val = kdb_hasher(Scalars);

    // change a name
    Scalars.rename("acaf1", "new_name");
    std::size_t hash_val_modified = kdb_hasher(Scalars);
    EXPECT_NE(hash_val, hash_val_modified);
    std::cout << "(rename scalar) orignal vs modified hash: " << std::to_string(hash_val) << " vs " << std::to_string(hash_val_modified) << std::endl;

    // modify an entry
    hash_val = hash_val_modified;
    IODE_REAL value = 0.0158;
    IODE_REAL relax = 0.98;
    Scalars.update("new_name", value, relax);
    hash_val_modified = kdb_hasher(Scalars);
    EXPECT_NE(hash_val, hash_val_modified);
    std::cout << "(modify scalar) orignal vs modified hash: " << std::to_string(hash_val) << " vs " << std::to_string(hash_val_modified) << std::endl;

    // remove an entry
    hash_val = hash_val_modified;
    Scalars.remove("new_name");
    hash_val_modified = kdb_hasher(Scalars);
    EXPECT_NE(hash_val, hash_val_modified);
    std::cout << "(delete scalar) orignal vs modified hash: " << std::to_string(hash_val) << " vs " << std::to_string(hash_val_modified) << std::endl;

    // add an entry
    hash_val = hash_val_modified;
    value = 0.012365879;
    relax = 1.;
    Scalars.add("new_entry", value, relax);
    hash_val_modified = kdb_hasher(Scalars);
    EXPECT_NE(hash_val, hash_val_modified);   
    std::cout << "(new    scalar) orignal vs modified hash: " << std::to_string(hash_val) << " vs " << std::to_string(hash_val_modified) << std::endl; 
}