#include "pch.h"


class KDBEquationsTest : public KDBTest, public ::testing::Test
{
protected:
    std::array<float, EQS_NBTESTS> tests = { 1.0f, 0.0042699f, 0.00818467f, 5.19945e-05f, 0.0019271461f, 
        23.545813f, 32.2732f, 0.82176137f, 0.79629868f, 2.3293459f, 83.8075f };

    void SetUp() override
    {
        load_global_kdb(I_EQUATIONS, input_test_dir + "fun.eqs");
    }

    // void TearDown() override {}
};


TEST_F(KDBEquationsTest, Load)
{
    Equations.load(input_test_dir + "fun.eqs");
    EXPECT_EQ(Equations.count(), 274);
}

TEST_F(KDBEquationsTest, CopyConstructor)
{
    std::string lec = Equations.get_lec("ACAF");
    std::string new_lec = "(ACAF/VAF[-1]) :=acaf2*GOSF[-1]+\nacaf4*(TIME=1995)";

    // GLOBAL KDB
    KDBEquations kdb_copy(Equations);
    EXPECT_EQ(kdb_copy.count(), 274);
    EXPECT_TRUE(kdb_copy.is_global_kdb());

    // LOCAL KDB
    KDBEquations local_kdb(KDB_LOCAL, "A*");
    KDBEquations local_kdb_hard_copy(local_kdb);
    EXPECT_EQ(local_kdb.count(), local_kdb_hard_copy.count());
    EXPECT_TRUE(local_kdb_hard_copy.is_local_kdb());
    local_kdb_hard_copy.update("ACAF", new_lec);
    EXPECT_EQ(local_kdb.get_lec("ACAF"), lec);
    EXPECT_EQ(local_kdb_hard_copy.get_lec("ACAF"), new_lec);

    // SHALLOW COPY KDB
    KDBEquations shallow_kdb(KDB_SHALLOW_COPY, "A*");
    KDBEquations shallow_kdb_copy(shallow_kdb);
    EXPECT_EQ(shallow_kdb.count(), shallow_kdb_copy.count());
    EXPECT_TRUE(shallow_kdb_copy.is_shallow_copy());
    shallow_kdb_copy.update("ACAF", new_lec);
    EXPECT_EQ(shallow_kdb.get_lec("ACAF"), new_lec);
    EXPECT_EQ(shallow_kdb_copy.get_lec("ACAF"), new_lec);
}

TEST_F(KDBEquationsTest, Save)
{
    // save in binary format
    save_global_kdb(I_EQUATIONS, output_test_dir + "fun.eqs");
    Equations.save(output_test_dir + "fun.eqs");

    // save in ascii format
    save_global_kdb(I_EQUATIONS, output_test_dir + "fun.ae");
    Equations.save(output_test_dir + "fun.ae");
}

TEST_F(KDBEquationsTest, GetLec)
{
    int pos = 0;
    std::string name = Equations.get_name(pos);
    std::string expected_lec = "(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)";

    // by pos
    EXPECT_EQ(Equations.get_lec(pos), expected_lec);

    // by name
    EXPECT_EQ(Equations.get_lec(name), expected_lec);
}

TEST_F(KDBEquationsTest, Get)
{
    int pos = 0;
    std::string name = Equations.get_name(pos);

    // by pos
    Equation eq1 = Equations.get(pos);
    EXPECT_EQ(eq1.get_lec(), "(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)");
    EXPECT_EQ(eq1.get_date_as_string(), "12-06-1998");
    EXPECT_EQ(eq1.get_sample().to_string(), "1980Y1:1996Y1");
    EXPECT_EQ(eq1.get_method(), "LSQ");

    // by name
    Equation eq2 = Equations.get(name);
    EXPECT_EQ(eq2.get_lec(), "(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)");
    EXPECT_EQ(eq2.get_date_as_string(), "12-06-1998");
    EXPECT_EQ(eq2.get_sample().to_string(), "1980Y1:1996Y1");
    EXPECT_EQ(eq2.get_method(), "LSQ");

    Equation eq3 = Equations.get("BVY");
    EXPECT_EQ(eq3.get_lec(), "BVY:=YN+YK");
    EXPECT_EQ(eq3.get_date_as_string(), "");
    EXPECT_EQ(eq3.get_sample().to_string(), ":");
    EXPECT_EQ(eq3.get_method(), "");
}

TEST_F(KDBEquationsTest, GetNames)
{
    std::vector<std::string> expected_names;
    for (int i=0; i < Equations.count(); i++) expected_names.push_back(Equations.get_name(i));
    std::vector<std::string> names = Equations.get_names();
    EXPECT_EQ(names, expected_names);
}

TEST_F(KDBEquationsTest, CreateRemove)
{
// disable this test function in case AddressSanitizer is activated because 
// it crashes on Windows Server 2019 (Github container)
#if !(defined _MSC_VER && defined __SANITIZE_ADDRESS__)
    std::string name = "ACAF";
    Equations.remove(name);
    EXPECT_THROW(Equations.get(name), IodeExceptionFunction);

    std::string lec = "(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)";
    std::string method = "LSQ";
    std::string from = "1980Y1";
    std::string to = "1996Y1";
    std::string comment = "Equation comment";
    std::string block = "ACAF";
    std::string instruments = "Equation instruments";
    bool date = true;

    Equations.add(name, lec, method, from, to, comment, instruments, block, date);
#endif
}

TEST_F(KDBEquationsTest, Update)
{
    std::string name = "ACAF";
    std::string new_lec = "(ACAF/VAF[-1]) :=acaf2*GOSF[-1]+\nacaf4*(TIME=1995)";

    Equations.update(name, new_lec);
    EXPECT_EQ(Equations.get_lec(name), new_lec);
}

TEST_F(KDBEquationsTest, Copy)
{
    std::string name = "ACAF";
    Equation eq = Equations.get(name);

    Equation copy_eq = Equations.copy(name);
    EXPECT_EQ(copy_eq, eq);
}

TEST_F(KDBEquationsTest, Filter)
{
    std::string pattern = "A*;$ENVI;*_";
    std::vector<std::string> expected_names;
    KDBEquations* local_kdb;

    load_global_kdb(I_LISTS, input_test_dir + "fun.lst");

    std::vector<std::string> all_names;
    for (int p = 0; p < Equations.count(); p++) all_names.push_back(Equations.get_name(p));

    int nb_total_comments = Equations.count();
    // A*
    for (const std::string& name : all_names) if (name.front() == 'A') expected_names.push_back(name);
    // $ENVI
    unsigned char** c_expanded_list = KL_expand("$ENVI");
    for (int i = 0; i < SCR_tbl_size(c_expanded_list); i++) expected_names.push_back((char*) c_expanded_list[i]);
    // *_
    for (const std::string& name : all_names) if (name.back() == '_') expected_names.push_back(name);

    // remove duplicate entries
    // NOTE: std::unique only removes consecutive duplicated elements, 
    //       so the vector needst to be sorted first
    std::sort(expected_names.begin(), expected_names.end());
    std::vector<std::string>::iterator it = std::unique(expected_names.begin(), expected_names.end());  
    expected_names.resize(std::distance(expected_names.begin(), it));

    // create local kdb
    local_kdb = new KDBEquations(KDB_SHALLOW_COPY, pattern);
    EXPECT_EQ(local_kdb->count(), expected_names.size());

    // modify an element of the local KDB and check if the 
    // corresponding element of the global KDB also changes
    std::string name = "ACAF";
    std::string modified_lec = "(ACAF/VAF[-1]) :=acaf2*GOSF[-1]+\nacaf4*(TIME=1995)";
    local_kdb->update(name, modified_lec);
    EXPECT_EQ(local_kdb->get_lec(name), modified_lec);
    EXPECT_EQ(Equations.get_lec(name), modified_lec);

    // delete an element from the local KDB and check if it has also 
    // been deleted from the global KDB
    local_kdb->remove(name);
    EXPECT_FALSE(local_kdb->contains(name));
    EXPECT_FALSE(Equations.contains(name));

    // add an element to the local KDB and check if it has also 
    // been added to the global KDB
    std::string lec = "(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)";
    std::string method = "LSQ";
    std::string from = "1980Y1";
    std::string to = "1996Y1";
    std::string comment = "Equation comment";
    std::string block = "ACAF";
    std::string instruments = "Equation instruments";
    bool date = true;
    local_kdb->add(name, lec, method, from, to, comment, instruments, block, date);
    EXPECT_EQ(local_kdb->get_lec(name), lec);
    EXPECT_EQ(Equations.get_lec(name), lec);

    // delete local kdb
    delete local_kdb;
    EXPECT_EQ(Equations.count(), nb_total_comments);
    EXPECT_EQ(Equations.get_lec(name), lec);
}

TEST_F(KDBEquationsTest, HardCopy)
{
    std::string pattern = "A*;$ENVI;*_";
    std::vector<std::string> expected_names;
    KDBEquations* local_kdb;

    load_global_kdb(I_LISTS, input_test_dir + "fun.lst");

    std::vector<std::string> all_names;
    for (int p = 0; p < Equations.count(); p++) all_names.push_back(Equations.get_name(p));

    int nb_total_comments = Equations.count();
    // A*
    for (const std::string& name : all_names) if (name.front() == 'A') expected_names.push_back(name);
    // $ENVI
    unsigned char** c_expanded_list = KL_expand("$ENVI");
    for (int i = 0; i < SCR_tbl_size(c_expanded_list); i++) expected_names.push_back((char*) c_expanded_list[i]);
    // *_
    for (const std::string& name : all_names) if (name.back() == '_') expected_names.push_back(name);

    // remove duplicate entries
    // NOTE: std::unique only removes consecutive duplicated elements, 
    //       so the vector needst to be sorted first
    std::sort(expected_names.begin(), expected_names.end());
    std::vector<std::string>::iterator it = std::unique(expected_names.begin(), expected_names.end());  
    expected_names.resize(std::distance(expected_names.begin(), it));

    // create local kdb
    local_kdb = new KDBEquations(KDB_LOCAL, pattern);
    EXPECT_EQ(local_kdb->count(), expected_names.size());

    // modify an element of the local KDB and check if the 
    // corresponding element of the global KDB didn't changed
    std::string name = "ACAF";
    std::string lec = Equations.get_lec(name);
    std::string modified_lec = "(ACAF/VAF[-1]) :=acaf2*GOSF[-1]+\nacaf4*(TIME=1995)";
    local_kdb->update(name, modified_lec);
    EXPECT_EQ(local_kdb->get_lec(name), modified_lec);
    EXPECT_EQ(Equations.get_lec(name), lec);

    // delete an element from the local KDB and check if it has not 
    // been deleted from the global KDB
    name = "ACAG";
    local_kdb->remove(name);
    EXPECT_FALSE(local_kdb->contains(name));
    EXPECT_TRUE(Equations.contains(name));

    // delete local kdb
    delete local_kdb;
    EXPECT_EQ(Equations.count(), nb_total_comments);
}

TEST_F(KDBEquationsTest, Merge)
{
    std::string pattern = "A*";

    // create hard copies kdb
    KDBEquations kdb0(KDB_LOCAL, pattern);
    KDBEquations kdb1(KDB_LOCAL, pattern);
    KDBEquations kdb_to_merge(KDB_LOCAL, pattern);

    // add an element to the KDB to be merged
    std::string new_name = "ACAF2";
    std::string new_lec = "(ACAF2/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)";
    std::string method = "LSQ";
    std::string from = "1980Y1";
    std::string to = "1996Y1";
    std::string comment = "Equation comment";
    std::string block = "ACAF";
    std::string instruments = "Equation instruments";
    bool date = true;
    kdb_to_merge.add(new_name, new_lec, method, from, to, comment, instruments, block, date);

    // modify an existing element of the KDB to be merge
    std::string name = "ACAF";
    std::string unmodified_lec = kdb_to_merge.get_lec(name);
    std::string modified_lec = "(ACAF/VAF[-1]) :=acaf2*GOSF[-1]+\nacaf4*(TIME=1995)";
    kdb_to_merge.update(name, modified_lec);

    // merge (overwrite)
    kdb0.merge(kdb_to_merge, true);
    // a) check kdb0 contains new item of KDB to be merged
    EXPECT_TRUE(kdb0.contains(new_name));
    EXPECT_EQ(kdb0.get_lec(new_name), new_lec);
    // b) check already existing item has been overwritten
    EXPECT_EQ(kdb0.get_lec(name), modified_lec); 

    // merge (NOT overwrite)
    kdb1.merge(kdb_to_merge, false);
    // b) check already existing item has NOT been overwritten
    EXPECT_EQ(kdb1.get_lec(name), unmodified_lec);
}

TEST_F(KDBEquationsTest, AssociatedObjs)
{
    std::string name = "ACAG";
    std::vector<std::string> objs_list;

    load_global_kdb(I_COMMENTS, input_test_dir + "fun.cmt");
    load_global_kdb(I_IDENTITIES, input_test_dir + "fun.idt");
    load_global_kdb(I_LISTS, input_test_dir + "fun.lst");
    load_global_kdb(I_SCALARS, input_test_dir + "fun.scl");
    load_global_kdb(I_TABLES, input_test_dir + "fun.tbl");
    load_global_kdb(I_VARIABLES, input_test_dir + "fun.var");

    std::vector<std::string> expected_cmts = { name };
    objs_list = Equations.get_associated_objects_list(name, I_COMMENTS);
    EXPECT_EQ(objs_list, expected_cmts);

    std::vector<std::string> expected_eqs = { name, "FLG" };
    objs_list = Equations.get_associated_objects_list(name, I_EQUATIONS);
    EXPECT_EQ(objs_list, expected_eqs);

    objs_list = Equations.get_associated_objects_list(name, I_IDENTITIES);
    EXPECT_EQ(objs_list.size(), 0);

    std::vector<std::string> expected_lts = { "COPY0", "ENDO0", "TOTAL0" };
    objs_list = Equations.get_associated_objects_list(name, I_LISTS);
    EXPECT_EQ(objs_list, expected_lts);

    objs_list = Equations.get_associated_objects_list(name, I_SCALARS);
    EXPECT_EQ(objs_list.size(), 0);

    std::vector<std::string> expected_tbl = { "GFR", "GFRLEVEL" };
    objs_list = Equations.get_associated_objects_list(name, I_TABLES);
    EXPECT_EQ(objs_list, expected_tbl);

    std::vector<std::string> expected_vars = { name };
    objs_list = Equations.get_associated_objects_list(name, I_VARIABLES);
    EXPECT_EQ(objs_list, expected_vars);
}

TEST_F(KDBEquationsTest, Hash)
{
    boost::hash<KDBEquations> kdb_hasher;
    std::size_t hash_val = kdb_hasher(Equations);

    // modify an entry
    std::string new_lec = "(ACAF/VAF[-1]) :=acaf2*GOSF[-1]+\nacaf4*(TIME=1995)";
    Equations.update("ACAF", new_lec);
    std::size_t hash_val_modified = kdb_hasher(Equations);
    EXPECT_NE(hash_val, hash_val_modified);
    std::cout << "(modify equation) orignal vs modified hash: " << std::to_string(hash_val) << " vs " << std::to_string(hash_val_modified) << std::endl;

    // remove an entry
    hash_val = hash_val_modified;
    Equations.remove("ACAF");
    hash_val_modified = kdb_hasher(Equations);
    EXPECT_NE(hash_val, hash_val_modified);
    std::cout << "(delete equation) orignal vs modified hash: " << std::to_string(hash_val) << " vs " << std::to_string(hash_val_modified) << std::endl;

    // add an entry
    hash_val = hash_val_modified;
    std::string lec = "(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)";
    std::string method = "LSQ";
    std::string from = "1980Y1";
    std::string to = "1996Y1";
    std::string comment = "Equation comment";
    std::string block = "ACAF";
    std::string instruments = "Equation instruments";
    bool date = true;
    Equations.add("ACAF", lec, method, from, to, comment, instruments, block, date);
    hash_val_modified = kdb_hasher(Equations);
    EXPECT_NE(hash_val, hash_val_modified);   
    std::cout << "(new    equation) orignal vs modified hash: " << std::to_string(hash_val) << " vs " << std::to_string(hash_val_modified) << std::endl; 
}
