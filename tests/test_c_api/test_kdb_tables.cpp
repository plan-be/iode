#include "pch.h"
#include "cpp_api/KDB/kdb_reference.h"


class KDBTablesTest : public TestAbstract, public ::testing::Test
{
protected:
    void SetUp() override
    {
        global_ws_tbl->load(str_input_test_dir + "fun.at");
    }

    // void TearDown() override {}
};


TEST_F(KDBTablesTest, Load)
{
    KDBTables kdb(false);
    kdb.load(str_input_test_dir + prefix_filename + "fun.tbl");
    EXPECT_EQ(kdb.size(), 46);
}

TEST_F(KDBTablesTest, Subset)
{
    std::string pattern = "C*";
    std::string title = global_ws_tbl->get_title("C8_1");
    std::string new_title = "modified title";
    Table* table = global_ws_tbl->get("C8_1");
    table->set_title(0, new_title);

    // GLOBAL KDB
    EXPECT_EQ(global_ws_tbl->size(), 46);
    EXPECT_TRUE(global_ws_tbl->is_global_database());
    std::set<std::string> names = global_ws_tbl->filter_names(pattern);

    // DEEP COPY SUBSET
    KDBTables* kdb_subset_deep_copy = new KDBTables(global_ws_tbl.get(), pattern, true);
    EXPECT_EQ(kdb_subset_deep_copy->size(), names.size());
    EXPECT_TRUE(kdb_subset_deep_copy->is_local_database());
    kdb_subset_deep_copy->update("C8_1", *table);
    EXPECT_EQ(global_ws_tbl->get_title("C8_1"), title);
    EXPECT_EQ(kdb_subset_deep_copy->get_title("C8_1"), new_title);

    // SHALLOW COPY SUBSET
    KDBTables* kdb_subset_shallow_copy = new KDBTables(global_ws_tbl.get(), pattern, false);
    EXPECT_EQ(kdb_subset_shallow_copy->size(), names.size());
    EXPECT_TRUE(kdb_subset_shallow_copy->is_shallow_copy_database());
    kdb_subset_shallow_copy->update("C8_1", *table);
    EXPECT_EQ(global_ws_tbl->get_title("C8_1"), new_title);
    EXPECT_EQ(kdb_subset_shallow_copy->get_title("C8_1"), new_title);
}

TEST_F(KDBTablesTest, Save)
{
    // save in binary format
    global_ws_tbl->save(str_output_test_dir + "fun.tbl");

    // save in ascii format
    global_ws_tbl->save(str_output_test_dir + "fun.at");
}

TEST_F(KDBTablesTest, Get)
{
    Table* table = global_ws_tbl->get("GFRPC");
    EXPECT_EQ(table->get_title(0), "Compte de l'ensemble des administrations publiques ");
    EXPECT_EQ(table->lines.size(), 31);
    EXPECT_EQ(table->nb_columns, 2);
    EXPECT_EQ(table->lines[0].get_type(), TABLE_LINE_TITLE);
    delete table;
}

TEST_F(KDBTablesTest, GetNames)
{
    std::set<std::string> expected_names;
    for (int i=0; i < global_ws_tbl->size(); i++) 
        expected_names.insert(global_ws_tbl->get_name(i));
    std::set<std::string> names = global_ws_tbl->get_names();
    EXPECT_EQ(names, expected_names);
}

TEST_F(KDBTablesTest, GetTitle)
{
    std::string title;
    std::string expected_title = "Déterminants de la croissance de K";

    title = global_ws_tbl->get_title("ANAKNFF");
    EXPECT_EQ(expected_title, title);
}

TEST_F(KDBTablesTest, CreateRemove)
{
    int i;
    std::string name;
    TableLine* line;
    global_ws_var->load(str_input_test_dir + "fun.av");
    global_ws_lst->load(str_input_test_dir + "fun.al");

    std::vector<std::string> vars_envi_list = {"EX", "PWMAB", "PWMS", "PWXAB", "PWXS", 
                                               "QWXAB", "QWXS", "POIL", "NATY", "TFPFHP_"};

    // add empty table with 2 columns
    name = "TABLE1";
    global_ws_tbl->add(name, 2);

    int nb_lines_header = 0;
    int nb_lines_footnotes = 0;
    int nb_lines_vars = 0;
    Table* table = global_ws_tbl->get(name);
    EXPECT_EQ(table->lines.size(), nb_lines_header + nb_lines_vars + nb_lines_footnotes);
    delete table;

    // remove table
    global_ws_tbl->remove(name);

    // add tables and initialize it (variables as list)
    name = "TABLE2";
    std::string def = "A title";
    std::string list_name = "$ENVI";
    std::vector<std::string> vars = {"GOSG", "YDTG", "DTH", "DTF", "IT", "YSSG", "COTRES", 
                                     "RIDG", "OCUG", list_name};
    bool mode = true;
    bool files = true;
    bool date = true;
    global_ws_tbl->add(name, 2, def, vars, mode, files, date);

    // check that list $ENVI has been expanded
    nb_lines_header = 2 + 2; // title + sep line + "#S" + sep line
    nb_lines_footnotes = (mode || files || date) ? 1 + mode + files + date : 0;   // 1 for sep line
    nb_lines_vars = (int) vars.size() + (int) vars_envi_list.size() - 1;    // -1 for list_name which is expanded
    table = global_ws_tbl->get(name);
    EXPECT_EQ(table->lines.size(), nb_lines_header + nb_lines_vars + nb_lines_footnotes);

    // check lines
    std::vector<std::string> expanded_vars = vars;
    expanded_vars.pop_back();                                                                   // remove list_name
    expanded_vars.insert(expanded_vars.end(), vars_envi_list.begin(), vars_envi_list.end());    // add variables contained in the $ENVI list
    
    line = &table->lines[0];
    EXPECT_EQ(line->get_type(), TABLE_LINE_TITLE);
    EXPECT_EQ(line->cells[0].get_content(false), def);
    line = &table->lines[1];
    EXPECT_EQ(line->get_type(), TABLE_LINE_SEP);
    line = &table->lines[2];
    EXPECT_EQ(line->get_type(), TABLE_LINE_CELL);
    EXPECT_EQ(line->cells[0].get_content(false), "");
    EXPECT_EQ(line->cells[1].get_content(false), "#S");
    line = &table->lines[3];
    EXPECT_EQ(line->get_type(), TABLE_LINE_SEP);
    for(i=0; i < nb_lines_vars; i++)
    {
        line = &table->lines[i + nb_lines_header];
        EXPECT_EQ(line->cells[0].get_content(false), expanded_vars[i]);
        EXPECT_EQ(line->cells[1].get_content(false), expanded_vars[i]);
    }
    i += nb_lines_header;
    line = &table->lines[i++];
    EXPECT_EQ(line->get_type(), TABLE_LINE_SEP);
    if(mode)
    {
        line = &table->lines[i++];
        EXPECT_EQ(line->get_type(), TABLE_LINE_MODE);
    }
    if(files)
    {
        line = &table->lines[i++];
        EXPECT_EQ(line->get_type(), TABLE_LINE_FILES);
    }
    if(date)
    {
        line = &table->lines[i++];
        EXPECT_EQ(line->get_type(), TABLE_LINE_DATE);
    }
    delete table;

    // remove table
    global_ws_tbl->remove(name);

    // add tables and initialize it (titles + lecs lists)
    name = "TABLE3";
    std::vector<std::string> titles = {"GOSG:", "YDTG:", "DTH:", "DTF:", "IT:", "YSSG+COTRES:", "RIDG:", "OCUG:"};
    std::vector<std::string> lecs = {"GOSG", "YDTG", "DTH", "DTF", "IT", "YSSG+COTRES", "RIDG", "OCUG"};
    global_ws_tbl->add(name, 2, def, titles, lecs, mode, files, date);

    // check lines
    nb_lines_vars = (int) lecs.size();
    table = global_ws_tbl->get(name);
    EXPECT_EQ(table->lines.size(), nb_lines_header + nb_lines_vars + nb_lines_footnotes);

    line = &table->lines[0];
    EXPECT_EQ(line->get_type(), TABLE_LINE_TITLE);
    EXPECT_EQ(line->cells[0].get_content(false), def);
    line = &table->lines[1];
    EXPECT_EQ(line->get_type(), TABLE_LINE_SEP);
    line = &table->lines[2];
    EXPECT_EQ(line->get_type(), TABLE_LINE_CELL);
    EXPECT_EQ(line->cells[0].get_content(false), "");
    EXPECT_EQ(line->cells[1].get_content(false), "#S");
    line = &table->lines[3];
    EXPECT_EQ(line->get_type(), TABLE_LINE_SEP);
    for(i=0; i < nb_lines_vars; i++)
    {
        line = &table->lines[i + nb_lines_header];
        EXPECT_EQ(line->cells[0].get_content(false), titles[i]);
        EXPECT_EQ(line->cells[1].get_content(false), lecs[i]);
    }
    i += nb_lines_header;
    line = &table->lines[i++];
    EXPECT_EQ(line->get_type(), TABLE_LINE_SEP);
    if(mode)
    {
        line = &table->lines[i++];
        EXPECT_EQ(line->get_type(), TABLE_LINE_MODE);
    }
    if(files)
    {
        line = &table->lines[i++];
        EXPECT_EQ(line->get_type(), TABLE_LINE_FILES);
    }
    if(date)
    {
        line = &table->lines[i++];
        EXPECT_EQ(line->get_type(), TABLE_LINE_DATE);
    }
    delete table;

    // remove table
    global_ws_tbl->remove(name);

    // add tables and initialize it (lecs as string)
    name = "TABLE4";
    std::string lecs_list;
    for(const std::string& lec: lecs) 
        lecs_list += lec + ";";
    lecs_list += list_name;
    global_ws_tbl->add(name, 2, def, lecs_list, mode, files, date);

    // check lines
    nb_lines_vars = (int) lecs.size() + (int) vars_envi_list.size();
    table = global_ws_tbl->get(name);
    EXPECT_EQ(table->lines.size(), nb_lines_header + nb_lines_vars + nb_lines_footnotes);

    std::vector<std::string> expanded_lecs = lecs;
    expanded_lecs.insert(expanded_lecs.end(), vars_envi_list.begin(), vars_envi_list.end());    // add variables contained in the $ENVI list

    line = &table->lines[0];
    EXPECT_EQ(line->get_type(), TABLE_LINE_TITLE);
    EXPECT_EQ(line->cells[0].get_content(false), def);
    line = &table->lines[1];
    EXPECT_EQ(line->get_type(), TABLE_LINE_SEP);
    line = &table->lines[2];
    EXPECT_EQ(line->get_type(), TABLE_LINE_CELL);
    EXPECT_EQ(line->cells[0].get_content(false), "");
    EXPECT_EQ(line->cells[1].get_content(false), "#S");
    line = &table->lines[3];
    EXPECT_EQ(line->get_type(), TABLE_LINE_SEP);
    for(i=0; i < nb_lines_vars; i++)
    {
        line = &table->lines[i + nb_lines_header];
        EXPECT_EQ(line->cells[0].get_content(false), expanded_lecs[i]);
        EXPECT_EQ(line->cells[1].get_content(false), expanded_lecs[i]);
    }
    i += nb_lines_header;
    line = &table->lines[i++];
    EXPECT_EQ(line->get_type(), TABLE_LINE_SEP);
    if(mode)
    {
        line = &table->lines[i++];
        EXPECT_EQ(line->get_type(), TABLE_LINE_MODE);
    }
    if(files)
    {
        line = &table->lines[i++];
        EXPECT_EQ(line->get_type(), TABLE_LINE_FILES);
    }
    if(date)
    {
        line = &table->lines[i++];
        EXPECT_EQ(line->get_type(), TABLE_LINE_DATE);
    }
    delete table;

    // remove table
    global_ws_tbl->remove(name);
}

TEST_F(KDBTablesTest, Filter)
{
    std::string pattern = "A*;*2";
    std::set<std::string> expected_names;
    KDBTables* kdb_subset;

    global_ws_var->load(str_input_test_dir + "fun.av");
    global_ws_lst->load(str_input_test_dir + "fun.al");

    std::set<std::string> all_names;
    for (int p = 0; p < global_ws_tbl->size(); p++) 
        all_names.insert(global_ws_tbl->get_name(p));

    int nb_total_tables = global_ws_tbl->size();
    // A*
    for (const std::string& name : all_names) 
        if (name.front() == 'A') 
            expected_names.insert(name);
    // *2
    for (const std::string& name : all_names) 
        if (name.back() == '2') 
            expected_names.insert(name);

    // create a subset (shallow copy)
    kdb_subset = new KDBTables(global_ws_tbl.get(), pattern, false);
    EXPECT_EQ(kdb_subset->size(), expected_names.size());
    EXPECT_EQ(kdb_subset->get_names(), expected_names);

    // modify an element of the subset and check if it has also 
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
    EXPECT_EQ(global_ws_tbl->get(new_name)->lines.size(), nb_lines_header + nb_lines_vars + nb_lines_footnotes);
    EXPECT_EQ(kdb_subset->get(new_name)->lines.size(), global_ws_tbl->get(new_name)->lines.size());
    EXPECT_EQ(*kdb_subset->get(new_name), *global_ws_tbl->get(new_name));

    // modify an element of the subset and check if the 
    // corresponding element has also been renamed in the global KDB
    std::string old_name = new_name;
    new_name = "TABLE_NEW";
    kdb_subset->rename(old_name, new_name);
    EXPECT_EQ(kdb_subset->get(new_name)->lines.size(), global_ws_tbl->get(new_name)->lines.size());
    EXPECT_EQ(*kdb_subset->get(new_name), *global_ws_tbl->get(new_name));

    // delete an element from the subset and check if it has also 
    // been deleted from the global KDB
    kdb_subset->remove(new_name);
    EXPECT_FALSE(kdb_subset->contains(new_name));
    EXPECT_FALSE(global_ws_tbl->contains(new_name));

    // try to add an element to the subset which is already present 
    // in the global KDB
    EXPECT_THROW(kdb_subset->add("DEF", 2, def, vars, mode, files, date), std::invalid_argument);

    // delete subset
    delete kdb_subset;
    EXPECT_EQ(global_ws_tbl->size(), nb_total_tables);

    // wrong pattern
    pattern = "anjfks";
    EXPECT_THROW(KDBTables(global_ws_tbl.get(), pattern, false), std::runtime_error);
}

TEST_F(KDBTablesTest, DeepCopy)
{
    std::string pattern = "A*;*2";
    std::set<std::string> expected_names;
    KDBTables* kdb_subset;

    global_ws_var->load(str_input_test_dir + "fun.av");
    global_ws_lst->load(str_input_test_dir + "fun.al");

    std::set<std::string> all_names;
    for (int p = 0; p < global_ws_tbl->size(); p++) 
        all_names.insert(global_ws_tbl->get_name(p));

    int nb_total_tables = global_ws_tbl->size();
    // A*
    for (const std::string& name : all_names) 
        if (name.front() == 'A') 
            expected_names.insert(name);
    // *2
    for (const std::string& name : all_names) 
        if (name.back() == '2') 
            expected_names.insert(name);

    // create a subset (deep copy)
    kdb_subset = new KDBTables(global_ws_tbl.get(), pattern, true);
    EXPECT_EQ(kdb_subset->size(), expected_names.size());
    EXPECT_EQ(kdb_subset->get_names(), expected_names);

    // modify an element of the subset and check if it has not 
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
    EXPECT_FALSE(global_ws_tbl->contains(new_name));
    Table* new_table = kdb_subset->get(new_name);
    EXPECT_EQ(new_table->lines.size(), nb_lines_header + nb_lines_vars + nb_lines_footnotes);
    delete new_table;

    // modify an element of the subset and check if the 
    // corresponding element has not been renamed in the global KDB
    std::string name = "DEF2";
    new_name = "TABLE_NEW";
    kdb_subset->rename(name, new_name);
    EXPECT_TRUE(kdb_subset->contains(new_name));
    EXPECT_FALSE(global_ws_tbl->contains(new_name));

    // delete an element from the subset and check if it has not 
    // been deleted from the global KDB
    name = "T2";
    kdb_subset->remove(name);
    EXPECT_FALSE(kdb_subset->contains(name));
    EXPECT_TRUE(global_ws_tbl->contains(name));

    // delete subset
    delete kdb_subset;
    EXPECT_EQ(global_ws_tbl->size(), nb_total_tables);
}

TEST_F(KDBTablesTest, CopyFrom)
{
    std::string pattern = "A* *_";
    std::string filename = str_input_test_dir + prefix_filename + "fun.tbl";
    int expected_nb_comments = global_ws_tbl->size();
    std::set<std::string> v_expected_names;

    // Copy entire file
    global_ws_tbl->clear();
    global_ws_tbl->copy_from(filename, "*");
    EXPECT_EQ(global_ws_tbl->size(), expected_nb_comments); 

    // copy subset
    v_expected_names = global_ws_tbl->filter_names(pattern);
    global_ws_tbl->clear();
    global_ws_tbl->copy_from(filename, pattern);
    EXPECT_EQ(global_ws_tbl->size(), v_expected_names.size());  
    EXPECT_EQ(global_ws_tbl->get_names(), v_expected_names);  
}

TEST_F(KDBTablesTest, Merge)
{
    std::string pattern = "A*";

    // create deep copies kdb
    KDBTables* kdb0 = new KDBTables(global_ws_tbl.get(), pattern, true);
    KDBTables* kdb1 = new KDBTables(global_ws_tbl.get(), pattern, true);
    KDBTables* kdb_to_merge = new KDBTables(global_ws_tbl.get(), pattern, true);

    // add an element to the KDB to be merged
    std::string new_name = "NEW_TABLE";
    std::string def = "A title";
    std::vector<std::string> vars = { "GOSG", "YDTG", "DTH", "DTF", "IT", "YSSG+COTRES", "RIDG", "OCUG", "$ENVI" };
    bool mode = true;
    bool files = true;
    bool date = true;
    kdb_to_merge->add(new_name, 2, def, vars, mode, files, date);
    Table* new_table = kdb_to_merge->get(new_name);

    // modify an existing element of the KDB to be merge
    std::string name = "ANAPRIX";
    Table* unmodified_table = kdb_to_merge->get(name);
    Table* modified_table = new Table(*unmodified_table);
    modified_table->set_title(0, "New Title");
    kdb_to_merge->update(name, *modified_table);

    // merge (overwrite)
    kdb0->merge(*kdb_to_merge, true);
    // a) check kdb0 contains new item of KDB to be merged
    EXPECT_TRUE(kdb0->contains(new_name));
    EXPECT_EQ(*kdb0->get(new_name), *new_table);
    // b) check already existing item has been overwritten
    EXPECT_EQ(*kdb0->get(name), *modified_table); 

    // merge (NOT overwrite)
    kdb1->merge(*kdb_to_merge, false);
    // b) check already existing item has NOT been overwritten
    EXPECT_EQ(*kdb1->get(name), *unmodified_table);
}

TEST_F(KDBTablesTest, Search)
{
    std::string tbl_name = "ENVI";
    std::vector<std::string> objs_list;

    global_ws_cmt->load(str_input_test_dir + "fun.ac");
    global_ws_eqs->load(str_input_test_dir + "fun.ae");
    global_ws_idt->load(str_input_test_dir + "fun.ai");
    global_ws_lst->load(str_input_test_dir + "fun.al");
    global_ws_scl->load(str_input_test_dir + "fun.as");
    global_ws_var->load(str_input_test_dir + "fun.av");

    objs_list = global_ws_cmt->search(tbl_name);
    EXPECT_EQ(objs_list.size(), 0);

    objs_list = global_ws_eqs->search(tbl_name);
    EXPECT_EQ(objs_list.size(), 0);

    objs_list = global_ws_idt->search(tbl_name);
    EXPECT_EQ(objs_list.size(), 0);

    std::vector<std::string> expected_lst = { tbl_name };
    objs_list = global_ws_lst->search(tbl_name);
    EXPECT_EQ(objs_list, expected_lst);

    objs_list = global_ws_scl->search(tbl_name);
    EXPECT_EQ(objs_list.size(), 0);

    std::vector<std::string> expected_tbl = { tbl_name };
    objs_list = global_ws_tbl->search(tbl_name);
    EXPECT_EQ(objs_list, expected_tbl);

    objs_list = global_ws_var->search(tbl_name);
    EXPECT_EQ(objs_list.size(), 0);
}

TEST_F(KDBTablesTest, PrintToFile)
{
    // WARNING: B_PrintTable() resets global_ref_xxx (reference files)

    int res;
    std::string arg;
    std::string gsample;
    std::string names = "C8_1 C8_2 C8_3 C8_4";

    // slightly modify variables
    std::string pattern = "Q_F;Q_I;KNFF;KLFHP;TFPFHP_";
    std::string ref_file = str_input_test_dir + "ref2.av";
    global_ws_var->load(str_input_test_dir + "fun.av");
    KDBVariables* kdb_ref = new KDBVariables(global_ws_var.get(), pattern, true);

    double value;
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
    arg = str_output_test_dir + "c_api_file.csv C";
    res = B_PrintDest(arg.data());
    res = B_PrintNbDec("4");
    arg = gsample + " " + names;
    res = B_PrintTbl(to_char_array(arg));
    EXPECT_EQ(res, 0);
    load_reference_kdb(2, ref_file);
    global_ws_tbl->print_to_file(str_output_test_dir + "cpp_api_file.csv", gsample, names, 4, 'C');
    compare_files(str_output_test_dir + "c_api_file.csv", str_output_test_dir + "cpp_api_file.csv");

    // ---- HTML format ----
    arg = str_output_test_dir + "c_api_file.html H";
    res = B_PrintDest(arg.data());
    res = B_PrintNbDec("4");
    arg = gsample + " " + names;
    res = B_PrintTbl(to_char_array(arg));
    EXPECT_EQ(res, 0);

    load_reference_kdb(2, ref_file);
    global_ws_tbl->print_to_file(str_output_test_dir + "cpp_api_file.html", gsample, names, 4, 'H');
    compare_files(str_output_test_dir + "c_api_file.html", str_output_test_dir + "cpp_api_file.html");

    // two time series (current workspace) - 5 observations
    gsample = "(2010;2010/2009):5";

    // ---- CSV format ----
    arg = str_output_test_dir + "c_api_file.csv C";
    res = B_PrintDest(arg.data());
    res = B_PrintNbDec("4");
    arg = gsample + " " + names;
    res = B_PrintTbl(to_char_array(arg));
    EXPECT_EQ(res, 0);

    load_reference_kdb(2, ref_file);
    global_ws_tbl->print_to_file(str_output_test_dir + "cpp_api_file.csv", gsample, names, 4, 'C');
    compare_files(str_output_test_dir + "c_api_file.csv", str_output_test_dir + "cpp_api_file.csv");

    // ---- HTML format ----
    arg = str_output_test_dir + "c_api_file.html H";
    res = B_PrintDest(arg.data());
    res = B_PrintNbDec("4");
    arg = gsample + " " + names;
    res = B_PrintTbl(to_char_array(arg));
    EXPECT_EQ(res, 0);

    load_reference_kdb(2, ref_file);
    global_ws_tbl->print_to_file(str_output_test_dir + "cpp_api_file.html", gsample, names, 4, 'H');
    compare_files(str_output_test_dir + "c_api_file.html", str_output_test_dir + "cpp_api_file.html");

    // simple time series (current workspace + one extra file) - 5 observations
    gsample = "2010[1-2]:5";

    // ---- CSV format ----
    arg = str_output_test_dir + "c_api_file.csv C";
    res = B_PrintDest(arg.data());
    res = B_PrintNbDec("4");
    arg = gsample + " " + names;
    res = B_PrintTbl(to_char_array(arg));
    EXPECT_EQ(res, 0);

    load_reference_kdb(2, ref_file);
    global_ws_tbl->print_to_file(str_output_test_dir + "cpp_api_file.csv", gsample, names, 4, 'C');
    compare_files(str_output_test_dir + "c_api_file.csv", str_output_test_dir + "cpp_api_file.csv");

    // ---- HTML format ----
    arg = str_output_test_dir + "c_api_file.html H";
    res = B_PrintDest(arg.data());
    res = B_PrintNbDec("4");
    arg = gsample + " " + names;
    res = B_PrintTbl(to_char_array(arg));
    EXPECT_EQ(res, 0);
    
    load_reference_kdb(2, ref_file);
    global_ws_tbl->print_to_file(str_output_test_dir + "cpp_api_file.html", gsample, names, 4, 'H');
    compare_files(str_output_test_dir + "c_api_file.html", str_output_test_dir + "cpp_api_file.html");
}

TEST_F(KDBTablesTest, Hash)
{
    std::size_t hash_val = hash_value(*global_ws_tbl);
    std::hash<Table> table_hasher;

    // change a name
    global_ws_tbl->rename("GFRPC", "NEW_NAME");
    std::size_t hash_val_modified = hash_value(*global_ws_tbl);
    EXPECT_NE(hash_val, hash_val_modified);
    // restore original name
    global_ws_tbl->rename("NEW_NAME", "GFRPC");
    std::size_t hash_val_restored = hash_value(*global_ws_tbl);
    EXPECT_EQ(hash_val, hash_val_restored);

    // remove an entry
    Table* table = global_ws_tbl->get("GFRPC");
    global_ws_tbl->remove("GFRPC");
    hash_val_modified = hash_value(*global_ws_tbl);
    EXPECT_NE(hash_val, hash_val_modified);
    // restore original entry
    global_ws_tbl->add("GFRPC", *table);
    std::size_t hash_original = table_hasher(*table);
    std::size_t hash_restored = table_hasher(*global_ws_tbl->get("GFRPC"));
    EXPECT_EQ(hash_original, hash_restored);
    hash_val_restored = hash_value(*global_ws_tbl);
    EXPECT_EQ(hash_val, hash_val_restored);
 
    // add an entry
    std::string def = "A title";
    std::vector<std::string> vars = { "GOSG", "YDTG", "DTH", "DTF", "IT", "YSSG+COTRES", "RIDG", "OCUG"};
    bool mode = true;
    bool files = true;
    bool date = true;
    global_ws_tbl->add("NEW_ENTRY", 2, def, vars, mode, files, date);
    hash_val_modified = hash_value(*global_ws_tbl);
    EXPECT_NE(hash_val, hash_val_modified);   
    // remove the entry
    global_ws_tbl->remove("NEW_ENTRY");
    hash_val_restored = hash_value(*global_ws_tbl);
    EXPECT_EQ(hash_val, hash_val_restored);
}
