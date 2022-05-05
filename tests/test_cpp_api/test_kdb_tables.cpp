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
    title = kdb.get_title(pos);
    EXPECT_EQ(expected_title, title);

    // by name
    std::string name = kdb.get_name(pos);
    title = kdb.get_title(name);
    EXPECT_EQ(expected_title, title);
}

TEST_F(KDBTablesTest, CreateRemove)
{
    KDBVariables kdb_var;
    kdb_var.load(input_test_dir + "fun.var");

    KDBLists kdb_list;
    kdb_list.load(input_test_dir + "fun.lst");

    std::string list_name = "$ENVI";
    // TODO JMP: is there a way to easily calculate the number of variables in a list ?
    int nb_vars_envi = 10;

    // add empty table with 2 columns
    Table table1 = kdb.add("TABLE1", 2);

    int nb_lines_header = 0;
    int nb_lines_footnotes = 0;
    int nb_lines_vars = 0;
    EXPECT_EQ(table1.nbLines(), nb_lines_header + nb_lines_vars + nb_lines_footnotes);

    // add tables and initialize it (variables as list)
    std::string def = "A title";
    std::vector<std::string> vars = { "GOSG", "YDTG", "DTH", "DTF", "IT", "YSSG+COTRES", "RIDG", "OCUG", list_name};
    bool mode = true;
    bool files = true;
    bool date = true;
    Table table2 = kdb.add("TABLE2", 2, def, vars, mode, files, date);

    // check that list $ENVI has been expanded
    nb_lines_header = 2 + 2; // title + sep line + "#S" + sep line
    nb_lines_footnotes = (mode || files || date) ? 1 + mode + files + date : 0;   // 1 for sep line
    nb_lines_vars = vars.size() + nb_vars_envi - 1;
    EXPECT_EQ(table2.nbLines(), nb_lines_header + nb_lines_vars + nb_lines_footnotes);

    // add tables and initialize it (LEC cells as string)
    std::string lecs = "GOSG;YDTG;DTH;DTF;IT;YSSG+COTRES;RIDG;OCUG;" + list_name;
    Table table3 = kdb.add("TABLE3", 2, def, lecs, mode, files, date);

    // check that list $ENVI has been expanded
    EXPECT_EQ(table3.nbLines(), nb_lines_header + nb_lines_vars + nb_lines_footnotes);

    // remove added tables
    kdb.remove("TABLE1");
    kdb.remove("TABLE2");
    kdb.remove("TABLE3");
}

TEST_F(KDBTablesTest, Copy)
{
    Table original_table = kdb.get("GFRPC");

    Table copy_table = kdb.copy("GFRPC");
    EXPECT_EQ(copy_table, original_table);
}
