#include "pch.h"


class KDBTablesTest : public KDBTest, public ::testing::Test
{
protected:
    KDBTables kdb;

    void SetUp() override
    {
        kdb.load(input_test_dir + "fun.tbl");
    }

    // void TearDown() override {}
};


TEST_F(KDBTablesTest, Load)
{
    KDBTables kdb;
    kdb.load(input_test_dir + "fun.tbl");
    EXPECT_EQ(kdb.count(), 46);
}

TEST_F(KDBTablesTest, GetTitle)
{
    int pos = 0;
    std::string title;
    std::string expected_title = u8"Déterminants de la croissance de K";

    // by position
    title = kdb.getTitle(pos);
    EXPECT_EQ(expected_title, title);

    // by name
    std::string name = kdb.getName(pos);
    title = kdb.getTitle(name);
    EXPECT_EQ(expected_title, title);
}
