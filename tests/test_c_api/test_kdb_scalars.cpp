#include "pch.h"


class KDBScalarsTest : public TestAbstract, public ::testing::Test
{
protected:
    void SetUp() override
    {
        global_ws_scl->load(str_input_test_dir + "fun.as");
    }

    // void TearDown() override {}
};


TEST_F(KDBScalarsTest, Load)
{
    KDBScalars kdb(false);
    kdb.load(str_input_test_dir + prefix_filename + "fun.scl");
    EXPECT_EQ(kdb.size(), 161);
}

TEST_F(KDBScalarsTest, Subset)
{
    std::string pattern = "a*";
    Scalar* scalar = global_ws_scl->get("acaf1");
    double value = 0.0158;
    double relax = 0.98;
    double std = 0.0;
    Scalar* new_scalar = new Scalar(value, relax, std);

    // GLOBAL KDB
    EXPECT_EQ(global_ws_scl->size(), 161);
    EXPECT_TRUE(global_ws_scl->is_global_database());
    std::set<std::string> names = global_ws_scl->filter_names(pattern);

    // DEEP COPY SUBSET
    KDBScalars* kdb_subset_deep_copy = new KDBScalars(global_ws_scl.get(), pattern, true);
    EXPECT_EQ(kdb_subset_deep_copy->size(), names.size());
    EXPECT_TRUE(kdb_subset_deep_copy->is_local_database());
    kdb_subset_deep_copy->update("acaf1", *new_scalar);
    EXPECT_EQ(*global_ws_scl->get("acaf1"), *scalar);
    EXPECT_EQ(*kdb_subset_deep_copy->get("acaf1"), *new_scalar);

    // SHALLOW COPY SUBSET
    KDBScalars* kdb_subset_shallow_copy = new KDBScalars(global_ws_scl.get(), pattern, false);
    EXPECT_EQ(kdb_subset_shallow_copy->size(), names.size());
    EXPECT_TRUE(kdb_subset_shallow_copy->is_shallow_copy_database());
    kdb_subset_shallow_copy->update("acaf1", *new_scalar);
    EXPECT_EQ(*global_ws_scl->get("acaf1"), *new_scalar);
    EXPECT_EQ(*kdb_subset_shallow_copy->get("acaf1"), *new_scalar);

    delete new_scalar;
}

TEST_F(KDBScalarsTest, Save)
{
    // save in binary format
    global_ws_scl->save(str_output_test_dir + "fun.scl");

    // save in ascii format
    global_ws_scl->save(str_output_test_dir + "fun.as");
}

TEST_F(KDBScalarsTest, Get)
{
    std::string name = "acaf1";
    Scalar expected_scalar(0.015768406912684, 1.0, 0.0013687137980013999);
    Scalar* scalar = global_ws_scl->get(name);
    EXPECT_DOUBLE_EQ(expected_scalar.value, scalar->value);
    EXPECT_DOUBLE_EQ(expected_scalar.relax, scalar->relax);
    EXPECT_DOUBLE_EQ(expected_scalar.std, scalar->std);
}

TEST_F(KDBScalarsTest, GetNames)
{
    std::set<std::string> expected_names;
    for (int i=0; i < global_ws_scl->size(); i++) 
        expected_names.insert(global_ws_scl->get_name(i));
    std::set<std::string> names = global_ws_scl->get_names();
    EXPECT_EQ(names, expected_names);
}

TEST_F(KDBScalarsTest, CreateRemove)
{
    std::string name = "new_scalar";
    double value = 0.012365879;
    double relax = 1.;
    Scalar scl(value, relax);

    global_ws_scl->add(name, scl);
    Scalar* new_scalar = global_ws_scl->get(name);
    EXPECT_DOUBLE_EQ(new_scalar->value, value);
    EXPECT_DOUBLE_EQ(new_scalar->relax, relax);
    EXPECT_TRUE(new_scalar->std < 1e-100);

    global_ws_scl->remove(name);
    EXPECT_THROW(global_ws_scl->get(name), std::invalid_argument);
}

TEST_F(KDBScalarsTest, Update)
{
    std::string name = global_ws_scl->get_name(0);
    double value = 0.0158;
    double relax = 0.98;
    Scalar scl(value, relax);

    global_ws_scl->update(name, scl);
    Scalar* updated_scalar = global_ws_scl->get(name);
    EXPECT_DOUBLE_EQ(updated_scalar->value, value);
    EXPECT_DOUBLE_EQ(updated_scalar->relax, relax);
    EXPECT_TRUE(updated_scalar->std < 1e-100);
}

TEST_F(KDBScalarsTest, Filter)
{
    std::string pattern = "a*;*_";
    std::set<std::string> expected_names;
    KDBScalars* kdb_subset;

    std::set<std::string> all_names;
    for (int p = 0; p < global_ws_scl->size(); p++) 
        all_names.insert(global_ws_scl->get_name(p));

    int nb_total_scalars = global_ws_scl->size();
    // a*
    for (const std::string& name : all_names) 
        if (name.front() == 'a') 
            expected_names.insert(name);
    // *_
    for (const std::string& name : all_names) 
        if (name.back() == '_') 
            expected_names.insert(name);

    // create a subset (shallow copy)
    kdb_subset = new KDBScalars(global_ws_scl.get(), pattern, false);
    EXPECT_EQ(kdb_subset->size(), expected_names.size());
    EXPECT_EQ(kdb_subset->get_names(), expected_names);

    // modify an element of the subset and check if the 
    // corresponding element of the global KDB also changes
    std::string name = "acaf1";
    double updated_value = 0.0158;
    double updated_relax = 0.98;
    double updated_std = 0.0;
    Scalar* expected_updated_scalar = new Scalar(updated_value, updated_relax, updated_std);
    kdb_subset->update(name, *expected_updated_scalar);
    Scalar* updated_scalar_local = kdb_subset->get(name);
    EXPECT_EQ(*updated_scalar_local, *expected_updated_scalar);
    Scalar* updated_scalar_global = global_ws_scl->get(name);
    EXPECT_EQ(*updated_scalar_global, *expected_updated_scalar);

    // modify an element of the subset and check if it has also 
    // been added to the global KDB
    std::string new_name = "new_scalar";
    double value = 0.012365879;
    double relax = 1.0;
    double std = 0.0;
    Scalar* expected_new_scalar = new Scalar(value, relax, std);
    kdb_subset->add(new_name, *expected_new_scalar);
    Scalar* new_scalar_local = kdb_subset->get(new_name);
    EXPECT_EQ(*new_scalar_local, *expected_new_scalar);
    Scalar* new_scalar_global = global_ws_scl->get(new_name);
    EXPECT_EQ(*new_scalar_global, *expected_new_scalar);
    delete expected_new_scalar;

    // modify an element of the subset and check if the 
    // corresponding element has also been renamed in the global KDB
    std::string old_name = new_name;
    Scalar* old_scalar = global_ws_scl->get(old_name);
    new_name = "scalar_new";
    kdb_subset->rename(old_name, new_name);
    new_scalar_local = kdb_subset->get(new_name);
    EXPECT_EQ(*new_scalar_local, *old_scalar);
    new_scalar_global = global_ws_scl->get(new_name);
    EXPECT_EQ(*new_scalar_global, *old_scalar);

    // delete an element from the subset and check if it has also 
    // been deleted from the global KDB
    kdb_subset->remove(new_name);
    EXPECT_FALSE(kdb_subset->contains(new_name));
    EXPECT_FALSE(global_ws_scl->contains(new_name));

    // try to add an element to the subset which is already present 
    // in the global KDB
    EXPECT_THROW(kdb_subset->add("gamma_", *new_scalar_local), std::invalid_argument);

    // delete subset
    delete kdb_subset;
    EXPECT_EQ(global_ws_scl->size(), nb_total_scalars);
    updated_scalar_global = global_ws_scl->get(name);
    EXPECT_EQ(*updated_scalar_global, *expected_updated_scalar);
    delete expected_updated_scalar;

    // wrong pattern
    pattern = "anjfks";
    EXPECT_THROW(KDBScalars(global_ws_scl.get(), pattern, false), std::runtime_error);
}

TEST_F(KDBScalarsTest, DeepCopy)
{
    std::string pattern = "a*;*_";
    std::set<std::string> expected_names;
    KDBScalars* kdb_subset;

    std::set<std::string> all_names;
    for (int p = 0; p < global_ws_scl->size(); p++) 
        all_names.insert(global_ws_scl->get_name(p));

    int nb_total_scalars = global_ws_scl->size();
    // a*
    for (const std::string& name : all_names) 
        if (name.front() == 'a') 
            expected_names.insert(name);
    // *_
    for (const std::string& name : all_names) 
        if (name.back() == '_') 
            expected_names.insert(name);

    // create a subset (deep copy)
    kdb_subset = new KDBScalars(global_ws_scl.get(), pattern, true);
    EXPECT_EQ(kdb_subset->size(), expected_names.size());
    EXPECT_EQ(kdb_subset->get_names(), expected_names);

    // modify an element of the subset and check if the 
    // corresponding element of the global KDB didn't changed
    std::string name = "acaf1";
    Scalar* scalar = global_ws_scl->get(name);
    double value = scalar->value;
    double relax = scalar->relax;
    double std = scalar->std;
    Scalar* expected_scalar = new Scalar(value, relax, std);
    double updated_value = 0.0158;
    double updated_relax = 0.98;
    double updated_std = 0.0;
    Scalar* expected_updated_scalar = new Scalar(updated_value, updated_relax, updated_std);
    kdb_subset->update(name, *expected_updated_scalar);
    Scalar* updated_scalar_local = kdb_subset->get(name);
    EXPECT_EQ(*updated_scalar_local, *expected_updated_scalar);
    Scalar* scalar_global = global_ws_scl->get(name); 
    EXPECT_EQ(*scalar_global, *expected_scalar);
    delete expected_scalar;
    delete expected_updated_scalar;

    // modify an element of the subset and check if it has not 
    // been added to the global KDB
    std::string new_name = "new_scalar";
    value = 0.012365879;
    relax = 1.0;
    std = 0.0;
    Scalar* expected_new_scalar = new Scalar(value, relax, std);
    kdb_subset->add(new_name, *expected_new_scalar);
    EXPECT_TRUE(kdb_subset->contains(new_name));
    Scalar* new_scalar_local = kdb_subset->get(new_name);
    EXPECT_EQ(*new_scalar_local, *expected_new_scalar);
    EXPECT_FALSE(global_ws_scl->contains(new_name));
    delete expected_new_scalar;

    // modify an element of the subset and check if the 
    // corresponding element has not been renamed in the global KDB
    name = "acaf2";
    Scalar* old_scalar = global_ws_scl->get(name);
    new_name = "scalar_new";
    kdb_subset->rename(name, new_name);
    EXPECT_TRUE(kdb_subset->contains(new_name));
    new_scalar_local = kdb_subset->get(new_name);
    EXPECT_EQ(*new_scalar_local, *old_scalar);
    EXPECT_FALSE(global_ws_scl->contains(new_name));

    // delete an element from the subset and check if it has not 
    // been deleted from the global KDB
    name = "acaf3";
    kdb_subset->remove(name);
    EXPECT_FALSE(kdb_subset->contains(name));
    EXPECT_TRUE(global_ws_scl->contains(name));

    // delete subset
    delete kdb_subset;
    EXPECT_EQ(global_ws_scl->size(), nb_total_scalars);
}

TEST_F(KDBScalarsTest, CopyFrom)
{
    std::string pattern = "a* *_";
    std::string filename = str_input_test_dir + prefix_filename + "fun.scl";
    int expected_nb_comments = global_ws_scl->size();
    std::set<std::string> v_expected_names;

    // Copy entire file
    global_ws_scl->clear();
    global_ws_scl->copy_from(filename, "*");
    EXPECT_EQ(global_ws_scl->size(), expected_nb_comments); 

    // copy subset
    v_expected_names = global_ws_scl->filter_names(pattern);
    global_ws_scl->clear();
    global_ws_scl->copy_from(filename, pattern);
    EXPECT_EQ(global_ws_scl->size(), v_expected_names.size());  
    EXPECT_EQ(global_ws_scl->get_names(), v_expected_names);  
}

TEST_F(KDBScalarsTest, Merge)
{
    std::string pattern = "a*";

    // create deep copies kdb
    KDBScalars* kdb0 = new KDBScalars(global_ws_scl.get(), pattern, true);
    KDBScalars* kdb1 = new KDBScalars(global_ws_scl.get(), pattern, true);
    KDBScalars* kdb_to_merge = new KDBScalars(global_ws_scl.get(), pattern, true);

    // add an element to the KDB to be merged
    std::string new_name = "new_scalar";
    double value = 0.012365879;
    double relax = 1.0;
    double std = 0.0;
    Scalar* new_scalar = new Scalar(value, relax, std);
    kdb_to_merge->add(new_name, *new_scalar);

    // modify an existing element of the KDB to be merge
    std::string name = "acaf1";
    Scalar* unmodified_scalar = kdb_to_merge->get(name);
    double updated_value = 0.0158;
    double updated_relax = 0.98;
    double updated_std = 0.0;
    Scalar* modified_scalar = new Scalar(updated_value, updated_relax, updated_std);
    kdb_to_merge->update(name, *modified_scalar);

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

    global_ws_cmt->load(str_input_test_dir + "fun.ac");
    global_ws_eqs->load(str_input_test_dir + "fun.ae");
    global_ws_idt->load(str_input_test_dir + "fun.ai");
    global_ws_lst->load(str_input_test_dir + "fun.al");
    global_ws_tbl->load(str_input_test_dir + "fun.at");
    global_ws_var->load(str_input_test_dir + "fun.av");

    objs_list = global_ws_cmt->search(scl_name);
    EXPECT_EQ(objs_list.size(), 0);

    std::vector<std::string> expected_eqs = { "W" };
    objs_list = global_ws_eqs->search(scl_name);
    EXPECT_EQ(objs_list, expected_eqs);

    std::vector<std::string> expected_idt = { "NAWRU" };
    objs_list = global_ws_idt->search(scl_name);
    EXPECT_EQ(objs_list, expected_idt);

    objs_list = global_ws_lst->search(scl_name);
    EXPECT_EQ(objs_list.size(), 0);

    std::vector<std::string> expected_scl = { scl_name };
    objs_list = global_ws_scl->search(scl_name);
    EXPECT_EQ(objs_list, expected_scl);

    objs_list = global_ws_tbl->search(scl_name);
    EXPECT_EQ(objs_list.size(), 0);

    objs_list = global_ws_var->search(scl_name);
    EXPECT_EQ(objs_list.size(), 0);
}

TEST_F(KDBScalarsTest, Hash)
{
    std::size_t hash_val = hash_value(*global_ws_scl);

    // change a name
    global_ws_scl->rename("acaf1", "new_name");
    std::size_t hash_val_modified = hash_value(*global_ws_scl);
    EXPECT_NE(hash_val, hash_val_modified);
    std::cout << "(rename scalar) orignal vs modified hash: " << std::to_string(hash_val) << " vs " << std::to_string(hash_val_modified) << std::endl;

    // modify an entry
    hash_val = hash_val_modified;
    double value = 0.0158;
    double relax = 0.98;
    Scalar modified_scl(value, relax);
    global_ws_scl->update("new_name", modified_scl);
    hash_val_modified = hash_value(*global_ws_scl);
    EXPECT_NE(hash_val, hash_val_modified);
    std::cout << "(modify scalar) orignal vs modified hash: " << std::to_string(hash_val) << " vs " << std::to_string(hash_val_modified) << std::endl;

    // remove an entry
    hash_val = hash_val_modified;
    global_ws_scl->remove("new_name");
    hash_val_modified = hash_value(*global_ws_scl);
    EXPECT_NE(hash_val, hash_val_modified);
    std::cout << "(delete scalar) orignal vs modified hash: " << std::to_string(hash_val) << " vs " << std::to_string(hash_val_modified) << std::endl;

    // add an entry
    hash_val = hash_val_modified;
    value = 0.012365879;
    relax = 1.;
    Scalar new_scl(value, relax);
    global_ws_scl->add("new_entry", new_scl);
    hash_val_modified = hash_value(*global_ws_scl);
    EXPECT_NE(hash_val, hash_val_modified);   
    std::cout << "(new    scalar) orignal vs modified hash: " << std::to_string(hash_val) << " vs " << std::to_string(hash_val_modified) << std::endl; 
}