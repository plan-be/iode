#include "pch.h"


class KDBListsTest : public KDBTest, public ::testing::Test
{
protected:
    KDBLists kdb;

    void SetUp() override
    {
        kdb.load(input_test_dir + "fun.lst");
    }

    // void TearDown() override {}
};


TEST_F(KDBListsTest, Load)
{
    KDBLists kdb;
    kdb.load(input_test_dir + "fun.lst");
    EXPECT_EQ(kdb.count(), 16);
}

TEST_F(KDBListsTest, Save)
{
    // save in binary format
    kdb.save(output_test_dir + "fun.lst");

    // save in ascii format
    kdb.save(output_test_dir + "fun.al");
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

TEST_F(KDBListsTest, CreateRemove)
{
    std::string name = "A_COMMENTS";
    std::string new_list = "ACAF;ACAG;AOUC;AQC";

    kdb.add(name, new_list);
    ASSERT_EQ(kdb.get(name), new_list);

    kdb.remove(name);
    EXPECT_THROW(kdb.get(name), std::runtime_error);
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
