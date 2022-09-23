#include "pch.h"


class KDBListsTest : public KDBTest, public ::testing::Test
{
protected:
    KDBLists kdb;

    void SetUp() override
    {
        load_global_kdb(I_LISTS, input_test_dir + "fun.lst");
    }

    // void TearDown() override {}
};


TEST_F(KDBListsTest, Load)
{
    KDBLists kdb2;
    EXPECT_EQ(kdb2.count(), 16);
}

TEST_F(KDBListsTest, Save)
{
    // save in binary format
    save_global_kdb(I_LISTS, output_test_dir + "fun.lst");
    kdb.dump(output_test_dir + "fun.lst");

    // save in ascii format
    save_global_kdb(I_LISTS, output_test_dir + "fun.al");
    kdb.dump(output_test_dir + "fun.al");
}

TEST_F(KDBListsTest, Get)
{
    int pos = 0;
    std::string list;
    std::string expected_list = "$COPY0;$COPY1;";

    // by position
    list = kdb.get(pos);
    EXPECT_EQ(expected_list, list);

    // by name
    std::string name = kdb.get_name(pos);
    list = kdb.get(name);
    EXPECT_EQ(expected_list, list);
}

TEST_F(KDBListsTest, GetNames)
{
    std::vector<std::string> expected_names;
    for (int i=0; i < kdb.count(); i++) expected_names.push_back(kdb.get_name(i));
    std::vector<std::string> names = kdb.get_names();
    EXPECT_EQ(names, expected_names);
}

TEST_F(KDBListsTest, CreateRemove)
{
    std::string name = "A_COMMENTS";
    std::string new_list = "ACAF;ACAG;AOUC;AQC";

    kdb.add(name, new_list);
    ASSERT_EQ(kdb.get(name), new_list);

    kdb.remove(name);
    EXPECT_THROW(kdb.get(name), IodeExceptionFunction);
}

TEST_F(KDBListsTest, Update)
{
    std::string expanded_list = kdb.get("COPY0") + kdb.get("COPY1");

    kdb.update("COPY", expanded_list);
    EXPECT_EQ(kdb.get("COPY"), expanded_list);
}

TEST_F(KDBListsTest, Copy)
{
    std::string name = "COPY";
    std::string list = kdb.get(name);

    std::string list_copy = kdb.copy(name);
    EXPECT_EQ(list_copy, list);
}

TEST_F(KDBListsTest, Filter)
{
    std::string pattern = "C*";
    std::vector<std::string> expected_names;
    KDBLists* local_kdb;
    KDBLists global_kdb;

    std::vector<std::string> all_names;
    for (int p = 0; p < global_kdb.count(); p++) all_names.push_back(global_kdb.get_name(p));
    for (const std::string& name : all_names) if (name.front() == 'C') expected_names.push_back(name);

    int nb_total_comments = global_kdb.count();

    // remove duplicate entries
    // NOTE: std::unique only removes consecutive duplicated elements, 
    //       so the vector needst to be sorted first
    std::sort(expected_names.begin(), expected_names.end());
    std::vector<std::string>::iterator it = std::unique(expected_names.begin(), expected_names.end());  
    expected_names.resize(std::distance(expected_names.begin(), it));

    // create local kdb
    local_kdb = new KDBLists(KDB_SHALLOW_COPY, pattern);
    EXPECT_EQ(local_kdb->count(), expected_names.size());

    // modify an element of the local KDB and check if the 
    // corresponding element of the global KDB also changes
    std::string name = "COPY";
    std::string expanded_list = kdb.get("COPY0") + kdb.get("COPY1");
    local_kdb->update(name, expanded_list);
    EXPECT_EQ(local_kdb->get(name), expanded_list);
    EXPECT_EQ(global_kdb.get(name), expanded_list);

    // add an element to the local KDB and check if it has also 
    // been added to the global KDB
    std::string new_name = "NEW_LIST";
    std::string new_list = "ACAF;ACAG;AOUC;AQC";
    local_kdb->add(new_name, new_list);
    EXPECT_EQ(local_kdb->get(new_name), new_list);
    EXPECT_EQ(global_kdb.get(new_name), new_list);

    // rename an element in the local KDB and check if the 
    // corresponding element has also been renamed in the global KDB
    std::string old_name = new_name;
    new_name = "LIST_NEW";
    local_kdb->rename(old_name, new_name);
    EXPECT_EQ(local_kdb->get(new_name), new_list);
    EXPECT_EQ(global_kdb.get(new_name), new_list);

    // delete an element from the local KDB and check if it has also 
    // been deleted from the global KDB
    local_kdb->remove(new_name);
    EXPECT_FALSE(local_kdb->contains(new_name));
    EXPECT_FALSE(global_kdb.contains(new_name));

    // delete local kdb
    delete local_kdb;
    EXPECT_EQ(global_kdb.count(), nb_total_comments);
    EXPECT_EQ(global_kdb.get(name), expanded_list);
}

TEST_F(KDBListsTest, HardCopy)
{
    std::string pattern = "C*";
    std::vector<std::string> expected_names;
    KDBLists* local_kdb;
    KDBLists global_kdb;

    std::vector<std::string> all_names;
    for (int p = 0; p < global_kdb.count(); p++) all_names.push_back(global_kdb.get_name(p));
    for (const std::string& name : all_names) if (name.front() == 'C') expected_names.push_back(name);

    // remove duplicate entries
    // NOTE: std::unique only removes consecutive duplicated elements, 
    //       so the vector needst to be sorted first
    std::sort(expected_names.begin(), expected_names.end());
    std::vector<std::string>::iterator it = std::unique(expected_names.begin(), expected_names.end());  
    expected_names.resize(std::distance(expected_names.begin(), it));

    int nb_total_comments = global_kdb.count();

    // create local kdb
    local_kdb = new KDBLists(KDB_LOCAL, pattern);
    EXPECT_EQ(local_kdb->count(), expected_names.size());

    // modify an element of the local KDB and check if the 
    // corresponding element of the global KDB didn't changed
    std::string name = "COPY";
    std::string list = global_kdb.get(name);
    std::string expanded_list = kdb.get("COPY0") + kdb.get("COPY1");
    local_kdb->update(name, expanded_list);
    EXPECT_EQ(local_kdb->get(name), expanded_list);
    EXPECT_EQ(global_kdb.get(name), list);

    // add an element to the local KDB and check if it has not 
    // been added to the global KDB
    std::string new_name = "NEW_LIST";
    std::string new_list = "ACAF;ACAG;AOUC;AQC";
    local_kdb->add(new_name, new_list);
    EXPECT_TRUE(local_kdb->contains(new_name));
    EXPECT_EQ(local_kdb->get(new_name), new_list);
    EXPECT_FALSE(global_kdb.contains(new_name));

    // rename an element in the local KDB and check if the 
    // corresponding element has not been renamed in the global KDB
    name = "COPY0";
    new_name = "LIST_NEW";
    local_kdb->rename(name, new_name);
    EXPECT_TRUE(local_kdb->contains(new_name));
    EXPECT_FALSE(global_kdb.contains(new_name));

    // delete an element from the local KDB and check if it has not 
    // been deleted from the global KDB
    name = "COPY1";
    local_kdb->remove(name);
    EXPECT_FALSE(local_kdb->contains(name));
    EXPECT_TRUE(global_kdb.contains(name));

    // delete local kdb
    delete local_kdb;
    EXPECT_EQ(global_kdb.count(), nb_total_comments);
}

TEST_F(KDBListsTest, Merge)
{
    std::string pattern = "C*";

    // create hard copies kdb
    KDBLists kdb0(KDB_LOCAL, pattern);
    KDBLists kdb1(KDB_LOCAL, pattern);
    KDBLists kdb_to_merge(KDB_LOCAL, pattern);

    // add an element to the KDB to be merged
    std::string new_name = "NEW_LIST";
    std::string new_list = "ACAF;ACAG;AOUC;AQC";
    kdb_to_merge.add(new_name, new_list);

    // modify an existing element of the KDB to be merge
    std::string name = "COPY";
    std::string unmodified_list = kdb_to_merge.get(name);
    std::string modified_list = kdb_to_merge.get("COPY0") + kdb_to_merge.get("COPY1");
    kdb_to_merge.update(name, modified_list);


    // merge (overwrite)
    kdb0.merge(kdb_to_merge, true);
    // a) check kdb0 contains new item of KDB to be merged
    EXPECT_TRUE(kdb0.contains(new_name));
    EXPECT_EQ(kdb0.get(new_name), new_list);
    // b) check already existing item has been overwritten
    EXPECT_EQ(kdb0.get(name), modified_list); 

    // merge (NOT overwrite)
    kdb1.merge(kdb_to_merge, false);
    // b) check already existing item has NOT been overwritten
    EXPECT_EQ(kdb1.get(name), unmodified_list);
}
