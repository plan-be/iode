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
    std::string name = kdb.getName(pos);
    list = kdb.get(name);
    EXPECT_EQ(expected_list, list);
}
