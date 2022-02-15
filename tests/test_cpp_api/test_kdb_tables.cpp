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

TEST_F(KDBTablesTest, Save)
{
    // save in binary format
    kdb.save(output_test_dir + "fun.tbl");

    // save in ascii format
    kdb.save(output_test_dir + "fun.at");
}

TEST_F(KDBTablesTest, Get)
{
    int pos = K_find(K_WS[I_TABLES], "GFRPC");

    // by position
    Table table = kdb.get(pos);

    // by name
    Table table2 = kdb.get("GFRPC");
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

TEST_F(KDBTablesTest, AddRemove)
{
    KDBVariables kdb_var;
    kdb_var.load(input_test_dir + "fun.var");

    // add empty table with 2 columns
    Table table1 = kdb.add_table("table1", 2);

    // add tables and initialize it
    std::string def = "A title";
    std::vector<std::string> vars = { "GOSG", "YDTG", "DTH", "DTF", "IT", "YSSG+COTRES", "RIDG", "OCUG" };
    bool mode = true;
    bool files = true;
    bool date = true;
    Table table2 = kdb.add_table("table2", 2, def, vars, mode, files, date);

    // remove added tables
    kdb.remove_table("table1");
    kdb.remove_table("table2");
}

TEST_F(KDBTablesTest, Copy)
{
    Table original_table = kdb.get("GFRPC");

    Table copy_table = kdb.copy("GFRPC_COPY", "GFRPC");
    EXPECT_EQ(copy_table, original_table);
}
