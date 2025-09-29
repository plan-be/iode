#include "pch.h"


class KDBTablesTest : public KDBTest, public ::testing::Test
{
protected:
    void SetUp() override
    {
        KDBTables kdb_tbl(input_test_dir + "fun.at");
    }

    // void TearDown() override {}
};


TEST_F(KDBTablesTest, Load)
{
    KDBTables kdb(input_test_dir + prefix_filename + "fun.tbl");
    EXPECT_EQ(kdb.count(), 46);
}

TEST_F(KDBTablesTest, Subset)
{
    std::string pattern = "C*";
    std::string title = Tables.get_title("C8_1");
    std::string new_title = "modified title";
    Table table = Tables.get("C8_1");
    table.set_title(0, new_title);

    // GLOBAL KDB
    KDBTables kdb_global;
    EXPECT_EQ(kdb_global.count(), 46);
    EXPECT_TRUE(kdb_global.is_global_database());

    // DEEP COPY SUBSET
    KDBTables* kdb_subset_deep_copy = kdb_global.subset(pattern, true);
    std::vector<std::string> names = kdb_global.get_names(pattern);
    EXPECT_EQ(kdb_subset_deep_copy->count(), names.size());
    EXPECT_TRUE(kdb_subset_deep_copy->is_local_database());
    kdb_subset_deep_copy->update("C8_1", table);
    EXPECT_EQ(kdb_global.get_title("C8_1"), title);
    EXPECT_EQ(kdb_subset_deep_copy->get_title("C8_1"), new_title);

    // SHALLOW COPY SUBSET
    KDBTables* kdb_subset_shallow_copy = kdb_global.subset(pattern, false);
    EXPECT_EQ(kdb_subset_shallow_copy->count(), names.size());
    EXPECT_TRUE(kdb_subset_shallow_copy->is_shallow_copy_database());
    kdb_subset_shallow_copy->update("C8_1", table);
    EXPECT_EQ(kdb_global.get_title("C8_1"), new_title);
    EXPECT_EQ(kdb_subset_shallow_copy->get_title("C8_1"), new_title);
}

TEST_F(KDBTablesTest, Save)
{
    // save in binary format
    Tables.save(output_test_dir + "fun.tbl");

    // save in ascii format
    Tables.save(output_test_dir + "fun.at");
}

TEST_F(KDBTablesTest, Get)
{
    int pos = K_find(K_WS[TABLES], const_cast<char*>("GFRPC"));

    // by position
    Table table = Tables.get(pos);
    EXPECT_EQ(table.get_title(0), "Compte de l'ensemble des administrations publiques ");
    EXPECT_EQ(table.lines.size(), 31);
    EXPECT_EQ(table.nb_columns, 2);
    EXPECT_EQ(table.lines[0].get_type(), TABLE_LINE_TITLE);

    // by name
    Table table2 = Tables.get("GFRPC");
    EXPECT_EQ(table2.get_title(0), "Compte de l'ensemble des administrations publiques ");
    EXPECT_EQ(table2.lines.size(), 31);
    EXPECT_EQ(table2.nb_columns, 2);
    EXPECT_EQ(table2.lines[0].get_type(), TABLE_LINE_TITLE);
}

TEST_F(KDBTablesTest, GetNames)
{
    std::vector<std::string> expected_names;
    for (int i=0; i < Tables.count(); i++) expected_names.push_back(Tables.get_name(i));
    std::vector<std::string> names = Tables.get_names();
    EXPECT_EQ(names, expected_names);
}

TEST_F(KDBTablesTest, GetTitle)
{
    int pos = 0;
    std::string title;
    std::string expected_title = "Déterminants de la croissance de K";

    // by position
    title = Tables.get_title(pos);
    EXPECT_EQ(expected_title, title);

    // by name
    std::string name = Tables.get_name(pos);
    title = Tables.get_title(name);
    EXPECT_EQ(expected_title, title);
}

TEST_F(KDBTablesTest, CreateRemove)
{
    int i;
    std::string name;
    TableLine* line;
    KDBVariables kdb_var(input_test_dir + "fun.av");
    KDBLists kdb_lst(input_test_dir + "fun.al");

    std::string list_name = "$ENVI";
    std::vector<std::string> vars_envi_list = {"EX", "PWMAB", "PWMS", "PWXAB", "PWXS", 
        "QWXAB", "QWXS", "POIL", "NATY", "TFPFHP_"};

    // add empty table with 2 columns
    name = "TABLE1";
    Tables.add(name, 2);

    int nb_lines_header = 0;
    int nb_lines_footnotes = 0;
    int nb_lines_vars = 0;
    Table table1 = Tables.get(name);
    EXPECT_EQ(table1.lines.size(), nb_lines_header + nb_lines_vars + nb_lines_footnotes);

    // remove table
    Tables.remove(name);

    // add tables and initialize it (variables as list)
    name = "TABLE2";
    std::string def = "A title";
    std::vector<std::string> vars = {"GOSG", "YDTG", "DTH", "DTF", "IT", "YSSG", "COTRES", "RIDG", "OCUG", list_name};
    bool mode = true;
    bool files = true;
    bool date = true;
    Tables.add(name, 2, def, vars, mode, files, date);

    // check that list $ENVI has been expanded
    nb_lines_header = 2 + 2; // title + sep line + "#S" + sep line
    nb_lines_footnotes = (mode || files || date) ? 1 + mode + files + date : 0;   // 1 for sep line
    nb_lines_vars = vars.size() + vars_envi_list.size() - 1;    // -1 for list_name which is expanded
    Table table2 = Tables.get(name);
    EXPECT_EQ(table2.lines.size(), nb_lines_header + nb_lines_vars + nb_lines_footnotes);

    // check lines
    std::vector<std::string> expanded_vars = vars;
    expanded_vars.pop_back();                                                                   // remove list_name
    expanded_vars.insert(expanded_vars.end(), vars_envi_list.begin(), vars_envi_list.end());    // add variables contained in the $ENVI list
    
    line = &table2.lines[0];
    EXPECT_EQ(line->get_type(), TABLE_LINE_TITLE);
    EXPECT_EQ(line->cells[0].get_content(false), def);
    line = &table2.lines[1];
    EXPECT_EQ(line->get_type(), TABLE_LINE_SEP);
    line = &table2.lines[2];
    EXPECT_EQ(line->get_type(), TABLE_LINE_CELL);
    EXPECT_EQ(line->cells[0].get_content(false), "");
    EXPECT_EQ(line->cells[1].get_content(false), "#S");
    line = &table2.lines[3];
    EXPECT_EQ(line->get_type(), TABLE_LINE_SEP);
    for(i=0; i < nb_lines_vars; i++)
    {
        line = &table2.lines[i + nb_lines_header];
        EXPECT_EQ(line->cells[0].get_content(false), expanded_vars[i]);
        EXPECT_EQ(line->cells[1].get_content(false), expanded_vars[i]);
    }
    i += nb_lines_header;
    line = &table2.lines[i++];
    EXPECT_EQ(line->get_type(), TABLE_LINE_SEP);
    if(mode)
    {
        line = &table2.lines[i++];
        EXPECT_EQ(line->get_type(), TABLE_LINE_MODE);
    }
    if(files)
    {
        line = &table2.lines[i++];
        EXPECT_EQ(line->get_type(), TABLE_LINE_FILES);
    }
    if(date)
    {
        line = &table2.lines[i++];
        EXPECT_EQ(line->get_type(), TABLE_LINE_DATE);
    }

    // remove table
    Tables.remove(name);

    // add tables and initialize it (titles + lecs lists)
    name = "TABLE3";
    std::vector<std::string> titles = {"GOSG:", "YDTG:", "DTH:", "DTF:", "IT:", "YSSG+COTRES:", "RIDG:", "OCUG:"};
    std::vector<std::string> lecs = {"GOSG", "YDTG", "DTH", "DTF", "IT", "YSSG+COTRES", "RIDG", "OCUG"};
    Tables.add(name, 2, def, titles, lecs, mode, files, date);

    // check lines
    nb_lines_vars = (int) lecs.size();
    Table table3 = Tables.get(name);
    EXPECT_EQ(table3.lines.size(), nb_lines_header + nb_lines_vars + nb_lines_footnotes);

    line = &table3.lines[0];
    EXPECT_EQ(line->get_type(), TABLE_LINE_TITLE);
    EXPECT_EQ(line->cells[0].get_content(false), def);
    line = &table3.lines[1];
    EXPECT_EQ(line->get_type(), TABLE_LINE_SEP);
    line = &table3.lines[2];
    EXPECT_EQ(line->get_type(), TABLE_LINE_CELL);
    EXPECT_EQ(line->cells[0].get_content(false), "");
    EXPECT_EQ(line->cells[1].get_content(false), "#S");
    line = &table3.lines[3];
    EXPECT_EQ(line->get_type(), TABLE_LINE_SEP);
    for(i=0; i < nb_lines_vars; i++)
    {
        line = &table3.lines[i + nb_lines_header];
        EXPECT_EQ(line->cells[0].get_content(false), titles[i]);
        EXPECT_EQ(line->cells[1].get_content(false), lecs[i]);
    }
    i += nb_lines_header;
    line = &table3.lines[i++];
    EXPECT_EQ(line->get_type(), TABLE_LINE_SEP);
    if(mode)
    {
        line = &table3.lines[i++];
        EXPECT_EQ(line->get_type(), TABLE_LINE_MODE);
    }
    if(files)
    {
        line = &table3.lines[i++];
        EXPECT_EQ(line->get_type(), TABLE_LINE_FILES);
    }
    if(date)
    {
        line = &table3.lines[i++];
        EXPECT_EQ(line->get_type(), TABLE_LINE_DATE);
    }

    // remove table
    Tables.remove(name);

    // add tables and initialize it (lecs as string)
    name = "TABLE4";
    std::string lecs_list;
    for(const std::string& lec: lecs) 
        lecs_list += lec + ";";
    lecs_list += list_name;
    Tables.add(name, 2, def, lecs_list, mode, files, date);

    // check lines
    nb_lines_vars = lecs.size() + vars_envi_list.size();
    Table table4 = Tables.get(name);
    EXPECT_EQ(table4.lines.size(), nb_lines_header + nb_lines_vars + nb_lines_footnotes);

    std::vector<std::string> expanded_lecs = lecs;
    expanded_lecs.insert(expanded_lecs.end(), vars_envi_list.begin(), vars_envi_list.end());    // add variables contained in the $ENVI list

    line = &table4.lines[0];
    EXPECT_EQ(line->get_type(), TABLE_LINE_TITLE);
    EXPECT_EQ(line->cells[0].get_content(false), def);
    line = &table4.lines[1];
    EXPECT_EQ(line->get_type(), TABLE_LINE_SEP);
    line = &table4.lines[2];
    EXPECT_EQ(line->get_type(), TABLE_LINE_CELL);
    EXPECT_EQ(line->cells[0].get_content(false), "");
    EXPECT_EQ(line->cells[1].get_content(false), "#S");
    line = &table4.lines[3];
    EXPECT_EQ(line->get_type(), TABLE_LINE_SEP);
    for(i=0; i < nb_lines_vars; i++)
    {
        line = &table4.lines[i + nb_lines_header];
        EXPECT_EQ(line->cells[0].get_content(false), expanded_lecs[i]);
        EXPECT_EQ(line->cells[1].get_content(false), expanded_lecs[i]);
    }
    i += nb_lines_header;
    line = &table4.lines[i++];
    EXPECT_EQ(line->get_type(), TABLE_LINE_SEP);
    if(mode)
    {
        line = &table4.lines[i++];
        EXPECT_EQ(line->get_type(), TABLE_LINE_MODE);
    }
    if(files)
    {
        line = &table4.lines[i++];
        EXPECT_EQ(line->get_type(), TABLE_LINE_FILES);
    }
    if(date)
    {
        line = &table4.lines[i++];
        EXPECT_EQ(line->get_type(), TABLE_LINE_DATE);
    }

    // remove table
    Tables.remove(name);
}

TEST_F(KDBTablesTest, Copy)
{
    Table original_table = Tables.get("GFRPC");

    Table copy_table = Tables.copy("GFRPC");
    EXPECT_EQ(copy_table, original_table);

    // add copy
    Tables.add("DUP_GFRPC", copy_table);
}

TEST_F(KDBTablesTest, Filter)
{
    std::string pattern = "A*;*2";
    std::vector<std::string> expected_names;
    KDBTables* kdb_subset;

    KDBVariables kdb_var(input_test_dir + "fun.av");
    KDBLists kdb_lst(input_test_dir + "fun.al");

    std::vector<std::string> all_names;
    for (int p = 0; p < Tables.count(); p++) all_names.push_back(Tables.get_name(p));

    int nb_total_tables = Tables.count();
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
    kdb_subset = Tables.subset(pattern);
    EXPECT_EQ(kdb_subset->count(), expected_names.size());
    EXPECT_EQ(kdb_subset->get_names(), expected_names);

    // add an element to the local KDB and check if it has also 
    // been added to the global KDB
    std::string new_name = "NEW_TABLE";
    std::string def = "A title";
    std::vector<std::string> vars = { "GOSG", "YDTG", "DTH", "DTF", "IT", "YSSG+COTRES", "RIDG", "OCUG", "$ENVI" };
    bool mode = true;
    bool files = true;
    bool date = true;
    kdb_subset->add(new_name, 2, def, vars, mode, files, date);
    int nb_vars_envi = 10;
    int nb_lines_header = 2 + 2; // title + sep line + "#S" + sep line
    int nb_lines_footnotes = (mode || files || date) ? 1 + mode + files + date : 0;   // 1 for sep line
    int nb_lines_vars = (int) vars.size() + nb_vars_envi - 1;
    EXPECT_EQ(kdb_subset->get(new_name)->lines.size(), nb_lines_header + nb_lines_vars + nb_lines_footnotes);
    EXPECT_EQ(Tables.get(new_name)->lines.size(), nb_lines_header + nb_lines_vars + nb_lines_footnotes);
    EXPECT_EQ(kdb_subset->get(new_name)->lines.size(), Tables.get(new_name)->lines.size());
    EXPECT_EQ(*kdb_subset->get(new_name), *Tables.get(new_name));

    // rename an element in the local KDB and check if the 
    // corresponding element has also been renamed in the global KDB
    std::string old_name = new_name;
    new_name = "TABLE_NEW";
    kdb_subset->rename(old_name, new_name);
    EXPECT_EQ(kdb_subset->get(new_name)->lines.size(), Tables.get(new_name)->lines.size());
    EXPECT_EQ(*kdb_subset->get(new_name), *Tables.get(new_name));

    // delete an element from the local KDB and check if it has also 
    // been deleted from the global KDB
    kdb_subset->remove(new_name);
    EXPECT_FALSE(kdb_subset->contains(new_name));
    EXPECT_FALSE(Tables.contains(new_name));

    // try to add an element to the local KDB which is already present 
    // in the global KDB
    EXPECT_THROW(kdb_subset->add("DEF", 2, def, vars, mode, files, date), std::invalid_argument);

    // delete local kdb
    delete kdb_subset;
    EXPECT_EQ(Tables.count(), nb_total_tables);

    // wrong pattern
    pattern = "anjfks";
    EXPECT_THROW(Tables.subset(pattern), std::runtime_error);
}

TEST_F(KDBTablesTest, DeepCopy)
{
    std::string pattern = "A*;*2";
    std::vector<std::string> expected_names;
    KDBTables* kdb_subset;

    KDBVariables kdb_var(input_test_dir + "fun.av");
    KDBLists kdb_lst(input_test_dir + "fun.al");

    std::vector<std::string> all_names;
    for (int p = 0; p < Tables.count(); p++) all_names.push_back(Tables.get_name(p));

    int nb_total_tables = Tables.count();
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
    kdb_subset = Tables.subset(pattern, true);
    EXPECT_EQ(kdb_subset->count(), expected_names.size());
    EXPECT_EQ(kdb_subset->get_names(), expected_names);

    // add an element to the local KDB and check if it has not 
    // been added to the global KDB
    std::string new_name = "NEW_TABLE";
    std::string def = "A title";
    std::vector<std::string> vars = { "GOSG", "YDTG", "DTH", "DTF", "IT", "YSSG+COTRES", "RIDG", "OCUG", "$ENVI" };
    bool mode = true;
    bool files = true;
    bool date = true;
    kdb_subset->add(new_name, 2, def, vars, mode, files, date);
    int nb_vars_envi = 10;
    int nb_lines_header = 2 + 2; // title + sep line + "#S" + sep line
    int nb_lines_footnotes = (mode || files || date) ? 1 + mode + files + date : 0;   // 1 for sep line
    int nb_lines_vars = (int) vars.size() + nb_vars_envi - 1;
    EXPECT_TRUE(kdb_subset->contains(new_name));
    EXPECT_FALSE(Tables.contains(new_name));
    Table* new_table = kdb_subset->get(new_name);
    EXPECT_EQ(new_table->lines.size(), nb_lines_header + nb_lines_vars + nb_lines_footnotes);
    delete new_table;

    // rename an element in the local KDB and check if the 
    // corresponding element has not been renamed in the global KDB
    std::string name = "DEF2";
    new_name = "TABLE_NEW";
    kdb_subset->rename(name, new_name);
    EXPECT_TRUE(kdb_subset->contains(new_name));
    EXPECT_FALSE(Tables.contains(new_name));

    // delete an element from the local KDB and check if it has not 
    // been deleted from the global KDB
    name = "T2";
    kdb_subset->remove(name);
    EXPECT_FALSE(kdb_subset->contains(name));
    EXPECT_TRUE(Tables.contains(name));

    // delete local kdb
    delete kdb_subset;
    EXPECT_EQ(Tables.count(), nb_total_tables);

    // wrong pattern
    pattern = "anjfks";
    EXPECT_THROW(Tables.subset(pattern, true), std::runtime_error);
}

TEST_F(KDBTablesTest, CopyFrom)
{
    std::string pattern = "A* *_";
    std::string filename = input_test_dir + prefix_filename + "fun.tbl";
    int expected_nb_comments = Tables.count();
    std::vector<std::string> v_expected_names;

    // Copy entire file
    Tables.clear();
    Tables.copy_from(filename, "*");
    EXPECT_EQ(Tables.count(), expected_nb_comments); 

    // copy subset
    v_expected_names = Tables.get_names(pattern);
    Tables.clear();
    Tables.copy_from(filename, pattern);
    EXPECT_EQ(Tables.count(), v_expected_names.size());  
    EXPECT_EQ(Tables.get_names(), v_expected_names);  
}

TEST_F(KDBTablesTest, Merge)
{
    std::string pattern = "A*";

    // create deep copies kdb
    KDBTables* kdb0 = Tables.subset(pattern, true);
    KDBTables* kdb1 = Tables.subset(pattern, true);
    KDBTables* kdb_to_merge = Tables.subset(pattern, true);

    // add an element to the KDB to be merged
    std::string new_name = "NEW_TABLE";
    std::string def = "A title";
    std::vector<std::string> vars = { "GOSG", "YDTG", "DTH", "DTF", "IT", "YSSG+COTRES", "RIDG", "OCUG", "$ENVI" };
    bool mode = true;
    bool files = true;
    bool date = true;
    kdb_to_merge->add(new_name, 2, def, vars, mode, files, date);
    Table new_table = kdb_to_merge->get(new_name);

    // modify an existing element of the KDB to be merge
    std::string name = "ANAPRIX";
    Table unmodified_table = kdb_to_merge->get(name);
    Table modified_table = kdb_to_merge->copy(name);
    modified_table.set_title(0, "New Title");
    kdb_to_merge->update(name, modified_table);

    // merge (overwrite)
    kdb0->merge(*kdb_to_merge, true);
    // a) check kdb0 contains new item of KDB to be merged
    EXPECT_TRUE(kdb0->contains(new_name));
    EXPECT_EQ(kdb0->get(new_name), new_table);
    // b) check already existing item has been overwritten
    EXPECT_EQ(kdb0->get(name), modified_table); 

    // merge (NOT overwrite)
    kdb1->merge(*kdb_to_merge, false);
    // b) check already existing item has NOT been overwritten
    EXPECT_EQ(kdb1->get(name), unmodified_table);
}

TEST_F(KDBTablesTest, Search)
{
    std::string tbl_name = "ENVI";
    std::vector<std::string> objs_list;

    KDBComments kdb_cmt(input_test_dir + "fun.ac");
    KDBEquations kdb_eqs(input_test_dir + "fun.ae");
    KDBIdentities kdb_idt(input_test_dir + "fun.ai");
    KDBLists kdb_lst(input_test_dir + "fun.al");
    KDBScalars kdb_scl(input_test_dir + "fun.as");
    KDBVariables kdb_var(input_test_dir + "fun.av");

    objs_list = Comments.search(tbl_name);
    EXPECT_EQ(objs_list.size(), 0);

    objs_list = Equations.search(tbl_name);
    EXPECT_EQ(objs_list.size(), 0);

    objs_list = Identities.search(tbl_name);
    EXPECT_EQ(objs_list.size(), 0);

    std::vector<std::string> expected_lst = { tbl_name };
    objs_list = Lists.search(tbl_name);
    EXPECT_EQ(objs_list, expected_lst);

    objs_list = Scalars.search(tbl_name);
    EXPECT_EQ(objs_list.size(), 0);

    std::vector<std::string> expected_tbl = { tbl_name };
    objs_list = Tables.search(tbl_name);
    EXPECT_EQ(objs_list, expected_tbl);

    objs_list = Variables.search(tbl_name);
    EXPECT_EQ(objs_list.size(), 0);
}

TEST_F(KDBTablesTest, PrintToFile)
{
    // WARNING: B_PrintTable() resets K_RWS (reference files)

    int res;
    std::string arg;
    std::string gsample;
    std::string names = "C8_1 C8_2 C8_3 C8_4";

    // slightly modify variables
    std::string ref_file = input_test_dir + "ref2.av";
    double value;
    KDBVariables kdb_var(input_test_dir + "fun.av");
    KDBVariables* kdb_ref = Variables.subset("Q_F;Q_I;KNFF;KLFHP;TFPFHP_", true);
    for(int t=0; t < kdb_ref->get_nb_periods(); t++)
    {
        for(const std::string& name: kdb_ref->get_names())
        {
            value = kdb_ref->get_var(name, t) * 0.98;
            kdb_ref->set_var(name, t, value);
        }
    }
    kdb_ref->save(ref_file);
    delete kdb_ref;

    // simple time series (current workspace) - 10 observations
    gsample = "2000:10";

    // ---- CSV format ----
    arg = output_test_dir + "c_api_file.csv C";
    res = B_PrintDest(arg.data());
    res = B_PrintNbDec("4");
    arg = gsample + " " + names;
    res = B_PrintTbl(to_char_array(arg));
    EXPECT_EQ(res, 0);
    load_reference_kdb(2, VARIABLES, ref_file);
    Tables.print_to_file(output_test_dir + "cpp_api_file.csv", gsample, names, 4, 'C');
    compare_files(output_test_dir + "c_api_file.csv", output_test_dir + "cpp_api_file.csv");

    // ---- HTML format ----
    arg = output_test_dir + "c_api_file.html H";
    res = B_PrintDest(arg.data());
    res = B_PrintNbDec("4");
    arg = gsample + " " + names;
    res = B_PrintTbl(to_char_array(arg));
    EXPECT_EQ(res, 0);

    load_reference_kdb(2, VARIABLES, ref_file);
    Tables.print_to_file(output_test_dir + "cpp_api_file.html", gsample, names, 4, 'H');
    compare_files(output_test_dir + "c_api_file.html", output_test_dir + "cpp_api_file.html");

    // two time series (current workspace) - 5 observations
    gsample = "(2010;2010/2009):5";

    // ---- CSV format ----
    arg = output_test_dir + "c_api_file.csv C";
    res = B_PrintDest(arg.data());
    res = B_PrintNbDec("4");
    arg = gsample + " " + names;
    res = B_PrintTbl(to_char_array(arg));
    EXPECT_EQ(res, 0);

    load_reference_kdb(2, VARIABLES, ref_file);
    Tables.print_to_file(output_test_dir + "cpp_api_file.csv", gsample, names, 4, 'C');
    compare_files(output_test_dir + "c_api_file.csv", output_test_dir + "cpp_api_file.csv");

    // ---- HTML format ----
    arg = output_test_dir + "c_api_file.html H";
    res = B_PrintDest(arg.data());
    res = B_PrintNbDec("4");
    arg = gsample + " " + names;
    res = B_PrintTbl(to_char_array(arg));
    EXPECT_EQ(res, 0);

    load_reference_kdb(2, VARIABLES, ref_file);
    Tables.print_to_file(output_test_dir + "cpp_api_file.html", gsample, names, 4, 'H');
    compare_files(output_test_dir + "c_api_file.html", output_test_dir + "cpp_api_file.html");

    // simple time series (current workspace + one extra file) - 5 observations
    gsample = "2010[1-2]:5";

    // ---- CSV format ----
    arg = output_test_dir + "c_api_file.csv C";
    res = B_PrintDest(arg.data());
    res = B_PrintNbDec("4");
    arg = gsample + " " + names;
    res = B_PrintTbl(to_char_array(arg));
    EXPECT_EQ(res, 0);

    load_reference_kdb(2, VARIABLES, ref_file);
    Tables.print_to_file(output_test_dir + "cpp_api_file.csv", gsample, names, 4, 'C');
    compare_files(output_test_dir + "c_api_file.csv", output_test_dir + "cpp_api_file.csv");

    // ---- HTML format ----
    arg = output_test_dir + "c_api_file.html H";
    res = B_PrintDest(arg.data());
    res = B_PrintNbDec("4");
    arg = gsample + " " + names;
    res = B_PrintTbl(to_char_array(arg));
    EXPECT_EQ(res, 0);
    
    load_reference_kdb(2, VARIABLES, ref_file);
    Tables.print_to_file(output_test_dir + "cpp_api_file.html", gsample, names, 4, 'H');
    compare_files(output_test_dir + "c_api_file.html", output_test_dir + "cpp_api_file.html");
}

TEST_F(KDBTablesTest, Hash)
{
    std::size_t hash_val = hash_value(Tables);
    std::hash<TBL> table_hasher;

    // change a name
    Tables.rename("GFRPC", "NEW_NAME");
    std::size_t hash_val_modified = hash_value(Tables);
    EXPECT_NE(hash_val, hash_val_modified);
    // restore original name
    Tables.rename("NEW_NAME", "GFRPC");
    std::size_t hash_val_restored = hash_value(Tables);
    EXPECT_EQ(hash_val, hash_val_restored);

    // remove an entry
    Table table = Tables.get("GFRPC");
    Tables.remove("GFRPC");
    hash_val_modified = hash_value(Tables);
    EXPECT_NE(hash_val, hash_val_modified);
    // restore original entry
    Tables.add("GFRPC", table);
    std::size_t hash_original = table_hasher(table);
    std::size_t hash_restored = table_hasher(*Tables.get("GFRPC"));
    EXPECT_EQ(hash_original, hash_restored);
    hash_val_restored = hash_value(Tables);
    EXPECT_EQ(hash_val, hash_val_restored);
 
    // add an entry
    std::string def = "A title";
    std::vector<std::string> vars = { "GOSG", "YDTG", "DTH", "DTF", "IT", "YSSG+COTRES", "RIDG", "OCUG"};
    bool mode = true;
    bool files = true;
    bool date = true;
    Tables.add("NEW_ENTRY", 2, def, vars, mode, files, date);
    hash_val_modified = hash_value(Tables);
    EXPECT_NE(hash_val, hash_val_modified);   
    // remove the entry
    Tables.remove("NEW_ENTRY");
    hash_val_restored = hash_value(Tables);
    EXPECT_EQ(hash_val, hash_val_restored);
}
