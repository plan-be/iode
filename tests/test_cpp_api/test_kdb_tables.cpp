#include "pch.h"


class KDBTablesTest : public KDBTest, public ::testing::Test
{
protected:
    KDBTables kdb;

    void SetUp() override
    {
        load_global_kdb(I_TABLES, input_test_dir + "fun.tbl");
    }

    // void TearDown() override {}
};


TEST_F(KDBTablesTest, Load)
{
    KDBTables kdb2;
    EXPECT_EQ(kdb2.count(), 46);
}

TEST_F(KDBTablesTest, Save)
{
    // save in binary format
    save_global_kdb(I_TABLES, output_test_dir + "fun.tbl");
    kdb.dump(output_test_dir + "fun.tbl");

    // save in ascii format
    save_global_kdb(I_TABLES, output_test_dir + "fun.at");
    kdb.dump(output_test_dir + "fun.at");
}

TEST_F(KDBTablesTest, Get)
{
    int pos = K_find(K_WS[I_TABLES], "GFRPC");

    // by position
    Table table = kdb.get(pos);
    EXPECT_EQ(table.getTitle(0), "Compte de l'ensemble des administrations publiques ");
    EXPECT_EQ(table.nbLines(), 31);
    EXPECT_EQ(table.nbColumns(), 2);
    EXPECT_EQ(table.getLineType(0), IT_TITLE);

    // by name
    Table table2 = kdb.get("GFRPC");
    EXPECT_EQ(table2.getTitle(0), "Compte de l'ensemble des administrations publiques ");
    EXPECT_EQ(table2.nbLines(), 31);
    EXPECT_EQ(table2.nbColumns(), 2);
    EXPECT_EQ(table2.getLineType(0), IT_TITLE);
}

TEST_F(KDBTablesTest, GetNames)
{
    std::vector<std::string> expected_names;
    for (int i=0; i < kdb.count(); i++) expected_names.push_back(kdb.get_name(i));
    std::vector<std::string> names = kdb.get_names();
    EXPECT_EQ(names, expected_names);
}

TEST_F(KDBTablesTest, GetTitle)
{
    int pos = 0;
    std::string title;
    std::string expected_title = "Déterminants de la croissance de K";

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
    std::string name;
    load_global_kdb(I_VARIABLES, input_test_dir + "fun.var");
    load_global_kdb(I_LISTS, input_test_dir + "fun.lst");

    std::string list_name = "$ENVI";
    // TODO JMP: is there a way to easily calculate the number of variables in a list ?
    int nb_vars_envi = 10;

    // add empty table with 2 columns
    name = "TABLE1";
    kdb.add(name, 2);

    int nb_lines_header = 0;
    int nb_lines_footnotes = 0;
    int nb_lines_vars = 0;
    Table table1(name);
    EXPECT_EQ(table1.nbLines(), nb_lines_header + nb_lines_vars + nb_lines_footnotes);

    // remove table
    kdb.remove(name);

    // add tables and initialize it (variables as list)
    name = "TABLE2";
    std::string def = "A title";
    std::vector<std::string> vars = { "GOSG", "YDTG", "DTH", "DTF", "IT", "YSSG+COTRES", "RIDG", "OCUG", list_name};
    bool mode = true;
    bool files = true;
    bool date = true;
    kdb.add(name, 2, def, vars, mode, files, date);

    // check that list $ENVI has been expanded
    nb_lines_header = 2 + 2; // title + sep line + "#S" + sep line
    nb_lines_footnotes = (mode || files || date) ? 1 + mode + files + date : 0;   // 1 for sep line
    nb_lines_vars = (int) vars.size() + nb_vars_envi - 1;
    Table table2(name);
    EXPECT_EQ(table2.nbLines(), nb_lines_header + nb_lines_vars + nb_lines_footnotes);

    // remove table
    kdb.remove(name);

    // add tables and initialize it (LEC cells as string)
    name = "TABLE3";
    std::string lecs = "GOSG;YDTG;DTH;DTF;IT;YSSG+COTRES;RIDG;OCUG;" + list_name;
    kdb.add(name, 2, def, lecs, mode, files, date);

    // check that list $ENVI has been expanded
    Table table3(name);
    EXPECT_EQ(table3.nbLines(), nb_lines_header + nb_lines_vars + nb_lines_footnotes);
    // remove table
    kdb.remove(name);
}

TEST_F(KDBTablesTest, Copy)
{
    Table original_table = kdb.get("GFRPC");

    Table copy_table = kdb.copy("GFRPC");
    EXPECT_EQ(copy_table, original_table);
}

TEST_F(KDBTablesTest, Filter)
{
    std::string pattern = "A*;*2";
    std::vector<std::string> expected_names;
    KDBTables* local_kdb;
    KDBTables global_kdb;

    load_global_kdb(I_VARIABLES, input_test_dir + "fun.var");
    load_global_kdb(I_LISTS, input_test_dir + "fun.lst");

    std::vector<std::string> all_names;
    for (int p = 0; p < global_kdb.count(); p++) all_names.push_back(global_kdb.get_name(p));

    int nb_total_comments = global_kdb.count();
    // A*
    for (const std::string& name : all_names) if (name.front() == 'A') expected_names.push_back(name);
    // *2
    for (const std::string& name : all_names) if (name.back() == '2') expected_names.push_back(name);

    // remove duplicate entries
    // NOTE: std::unique only removes consecutive duplicated elements, 
    //       so the vector needst to be sorted first
    std::sort(expected_names.begin(), expected_names.end());
    std::vector<std::string>::iterator it = std::unique(expected_names.begin(), expected_names.end());  
    expected_names.resize(std::distance(expected_names.begin(), it));

    // create local kdb
    local_kdb = new KDBTables(pattern);
    EXPECT_EQ(local_kdb->count(), expected_names.size());

    // add an element to the local KDB and check if it has also 
    // been added to the global KDB
    std::string new_name = "NEW_TABLE";
    std::string def = "A title";
    std::vector<std::string> vars = { "GOSG", "YDTG", "DTH", "DTF", "IT", "YSSG+COTRES", "RIDG", "OCUG", "$ENVI" };
    bool mode = true;
    bool files = true;
    bool date = true;
    local_kdb->add(new_name, 2, def, vars, mode, files, date);
    int nb_vars_envi = 10;
    int nb_lines_header = 2 + 2; // title + sep line + "#S" + sep line
    int nb_lines_footnotes = (mode || files || date) ? 1 + mode + files + date : 0;   // 1 for sep line
    int nb_lines_vars = (int) vars.size() + nb_vars_envi - 1;
    EXPECT_EQ(local_kdb->get(new_name).nbLines(), nb_lines_header + nb_lines_vars + nb_lines_footnotes);
    EXPECT_EQ(global_kdb.get(new_name).nbLines(), nb_lines_header + nb_lines_vars + nb_lines_footnotes);
    EXPECT_EQ(local_kdb->get(new_name).nbLines(), global_kdb.get(new_name).nbLines());
    EXPECT_EQ(local_kdb->get(new_name), global_kdb.get(new_name));

    // rename an element in the local KDB and check if the 
    // corresponding element has also been renamed in the global KDB
    std::string old_name = new_name;
    new_name = "TABLE_NEW";
    local_kdb->rename(old_name, new_name);
    EXPECT_EQ(local_kdb->get(new_name).nbLines(), global_kdb.get(new_name).nbLines());
    EXPECT_EQ(local_kdb->get(new_name), global_kdb.get(new_name));

    // delete an element from the local KDB and check if it has also 
    // been deleted from the global KDB
    local_kdb->remove(new_name);
    EXPECT_FALSE(local_kdb->contains(new_name));
    EXPECT_FALSE(global_kdb.contains(new_name));

    // delete local kdb
    delete local_kdb;
    EXPECT_EQ(global_kdb.count(), nb_total_comments);
}

TEST_F(KDBTablesTest, HardCopy)
{
    std::string pattern = "A*;*2";
    std::vector<std::string> expected_names;
    KDBTables* local_kdb;
    KDBTables global_kdb;

    load_global_kdb(I_VARIABLES, input_test_dir + "fun.var");
    load_global_kdb(I_LISTS, input_test_dir + "fun.lst");

    std::vector<std::string> all_names;
    for (int p = 0; p < global_kdb.count(); p++) all_names.push_back(global_kdb.get_name(p));

    int nb_total_comments = global_kdb.count();
    // A*
    for (const std::string& name : all_names) if (name.front() == 'A') expected_names.push_back(name);
    // *2
    for (const std::string& name : all_names) if (name.back() == '2') expected_names.push_back(name);

    // remove duplicate entries
    // NOTE: std::unique only removes consecutive duplicated elements, 
    //       so the vector needst to be sorted first
    std::sort(expected_names.begin(), expected_names.end());
    std::vector<std::string>::iterator it = std::unique(expected_names.begin(), expected_names.end());  
    expected_names.resize(std::distance(expected_names.begin(), it));

    // create local kdb
    local_kdb = new KDBTables(pattern, false);
    EXPECT_EQ(local_kdb->count(), expected_names.size());

    // add an element to the local KDB and check if it has not 
    // been added to the global KDB
    std::string new_name = "NEW_TABLE";
    std::string def = "A title";
    std::vector<std::string> vars = { "GOSG", "YDTG", "DTH", "DTF", "IT", "YSSG+COTRES", "RIDG", "OCUG", "$ENVI" };
    bool mode = true;
    bool files = true;
    bool date = true;
    local_kdb->add(new_name, 2, def, vars, mode, files, date);
    int nb_vars_envi = 10;
    int nb_lines_header = 2 + 2; // title + sep line + "#S" + sep line
    int nb_lines_footnotes = (mode || files || date) ? 1 + mode + files + date : 0;   // 1 for sep line
    int nb_lines_vars = (int) vars.size() + nb_vars_envi - 1;
    EXPECT_TRUE(local_kdb->contains(new_name));
    EXPECT_FALSE(global_kdb.contains(new_name));
    EXPECT_EQ(local_kdb->get(new_name).nbLines(), nb_lines_header + nb_lines_vars + nb_lines_footnotes);

    // rename an element in the local KDB and check if the 
    // corresponding element has not been renamed in the global KDB
    std::string name = "DEF2";
    new_name = "TABLE_NEW";
    local_kdb->rename(name, new_name);
    EXPECT_TRUE(local_kdb->contains(new_name));
    EXPECT_FALSE(global_kdb.contains(new_name));

    // delete an element from the local KDB and check if it has not 
    // been deleted from the global KDB
    name = "T2";
    local_kdb->remove(name);
    EXPECT_FALSE(local_kdb->contains(name));
    EXPECT_TRUE(global_kdb.contains(name));

    // delete local kdb
    delete local_kdb;
    EXPECT_EQ(global_kdb.count(), nb_total_comments);
}
