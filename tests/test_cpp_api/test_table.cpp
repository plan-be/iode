#include "pch.h"


class TablesTest : public KDBTest, public ::testing::Test
{
protected:
    Table* table;

    void SetUp() override
    {
        KDBTables kdb_tbl(input_test_dir + "fun.tbl");

        int pos = K_find(K_WS[I_TABLES], "GFRPC");
        table = new Table(pos, nullptr);
    }

    void TearDown() override 
    {
        delete table;
    }
};


// Test that a TBL* 'tbl' object added to KT_WS using K_add() and a TBL* 'extracted_tbl' object 
// extracted using KTVAL() are exactly the same
TEST_F(TablesTest, AddGetTBL)
{
    KDBTables kdb_tbl(input_test_dir + "fun.tbl");
    KDBVariables kdb_var(input_test_dir + "fun.var");

    // --- create a C struct TBL
    int nb_columns = 2;
    char* def = "A title";
    char* vars = "GOSG;YDTG;DTH;DTF;IT;YSSG+COTRES;RIDG;OCUG";
    char** lecs = B_ainit_chk(vars, NULL, 0);
    int mode = 1;
    int files = 1;
    int date = 1;

    TBL* tbl = T_create(nb_columns);
	T_auto(tbl, def, lecs, mode, files, date);
	SCR_free_tbl((unsigned char**) lecs);

    // --- add the table to the Tables KDB
    char* name = "c_table";
    K_add(KT_WS, name, tbl);

    // --- extract the table from the Table KDB
    int pos = K_find(KT_WS, name);
    TBL* extracted_tbl = KTVAL(KT_WS, pos);

    // --- check that both table are exactly the same
    // ----- check all attributes that are not of type TLINE 
    ASSERT_EQ(tbl->t_lang, extracted_tbl->t_lang);
    ASSERT_EQ(tbl->t_free, extracted_tbl->t_free);
    ASSERT_EQ(tbl->t_nc, extracted_tbl->t_nc);
    ASSERT_EQ(tbl->t_nl, extracted_tbl->t_nl);
    ASSERT_EQ(tbl->t_zmin, extracted_tbl->t_zmin);
    ASSERT_EQ(tbl->t_zmax, extracted_tbl->t_zmax);
    ASSERT_EQ(tbl->t_ymin, extracted_tbl->t_ymin);
    ASSERT_EQ(tbl->t_ymax, extracted_tbl->t_ymax);
    ASSERT_EQ(tbl->t_attr, extracted_tbl->t_attr);
    ASSERT_EQ(tbl->t_box, extracted_tbl->t_box);
    ASSERT_EQ(tbl->t_shadow, extracted_tbl->t_shadow);
    ASSERT_EQ(tbl->t_gridx, extracted_tbl->t_gridx);
    ASSERT_EQ(tbl->t_gridy, extracted_tbl->t_gridy);
    ASSERT_EQ(tbl->t_axis, extracted_tbl->t_axis);
    ASSERT_EQ(tbl->t_align, extracted_tbl->t_align);

    // ----- check div line 
    TCELL* cells_0;
    TCELL* cells_1;
    ASSERT_EQ(tbl->t_div.tl_type, extracted_tbl->t_div.tl_type);
    ASSERT_EQ(tbl->t_div.tl_graph, extracted_tbl->t_div.tl_graph);
    ASSERT_EQ(tbl->t_div.tl_axis, extracted_tbl->t_div.tl_axis);
    ASSERT_EQ(tbl->t_div.tl_pbyte, extracted_tbl->t_div.tl_pbyte);
    cells_0 = (TCELL*) tbl->t_div.tl_val;
    cells_1 = (TCELL*) extracted_tbl->t_div.tl_val;
    for(int j = 0; j < tbl->t_nc; j++)
    {
        ASSERT_EQ(cells_0[j].tc_type, IT_LEC);
        ASSERT_EQ(cells_1[j].tc_type, IT_LEC);
        ASSERT_EQ(cells_1[0].tc_type, IT_LEC);
        ASSERT_EQ(cells_1[1].tc_type, IT_LEC);
        ASSERT_EQ(cells_0[j].tc_attr, cells_1[j].tc_attr);
        ASSERT_EQ(cells_0[j].tc_attr, KT_LEFT);
        ASSERT_EQ(cells_1[0].tc_attr, KT_LEFT);
        ASSERT_EQ(cells_1[1].tc_attr, KT_LEFT);
        ASSERT_EQ(std::string(T_cell_cont(cells_0, j)), std::string(T_cell_cont(cells_1, j)));
    }

    // ----- check all lines 
    TLINE* line_0;
    TLINE* line_1;
    for(int i = 0; i < tbl->t_nl; i++)
    {
        line_0 = tbl->t_line + i;
        line_1 = extracted_tbl->t_line + i;

        ASSERT_EQ(line_0->tl_type, line_1->tl_type);
        ASSERT_EQ(line_0->tl_graph, line_1->tl_graph);
        ASSERT_EQ(line_0->tl_axis, line_1->tl_axis);
        ASSERT_EQ(line_0->tl_pbyte, line_1->tl_pbyte);

        cells_0 = (TCELL*) line_0->tl_val;
        cells_1 = (TCELL*) line_1->tl_val;
        switch (line_0->tl_type)
        {
        case EnumLineType::IT_TITLE:
            ASSERT_EQ(std::string(T_cell_cont(cells_0, 0)), std::string(T_cell_cont(cells_1, 0)));
            break;
        case EnumLineType::IT_CELL:
            for(int j = 0; j < tbl->t_nc; j++)
            {
                ASSERT_EQ(cells_0[j].tc_type, cells_1[j].tc_type);
                ASSERT_EQ(cells_0[j].tc_attr, cells_1[j].tc_attr);
                if(i == 2 && j == 0)
                {
                    ASSERT_EQ(cells_0[j].tc_attr, KT_LEFT);
                    ASSERT_EQ(cells_1[j].tc_attr, KT_LEFT);
                }
                else
                    ASSERT_EQ(cells_0[j].tc_attr, cells_1[j].tc_attr);
                ASSERT_EQ(std::string(T_cell_cont(cells_0, j)), std::string(T_cell_cont(cells_1, j)));
            }
            break;
        default:
            ASSERT_TRUE(cells_0 == NULL);
            ASSERT_TRUE(cells_1 == NULL);
            break;
        }
    }

    // --- free memory
    T_free(tbl);
    T_free(extracted_tbl);
}

TEST_F(TablesTest, Equivalence_C_CPP)
{
    TCELL* div_cells;

    KDBVariables kdb_var(input_test_dir + "fun.var");

    char* c_name = "cpp_table";
    int nb_columns = 2;
    std::string def = "A title";
    std::vector<std::string> vars = { "GOSG", "YDTG", "DTH", "DTF", "IT", "YSSG+COTRES", "RIDG", "OCUG"};
    bool mode = true;
    bool files = true;
    bool date = true;

    // test if a Table object can be added to the Tables KDB via K_add()
    Table table(nb_columns, def, vars, mode, files, date);
    div_cells = (TCELL*) table.t_div.tl_val;
    ASSERT_EQ(div_cells[0].tc_type, IT_LEC);
    ASSERT_EQ(div_cells[1].tc_type, IT_LEC);

    K_add(KT_WS, c_name, static_cast<TBL*>(&table));
    int pos = K_find(KT_WS, c_name);
    ASSERT_GT(pos, -1);

    TBL* tbl = KTVAL(KT_WS, pos);    
    ASSERT_TRUE(table_equal(table, *tbl));

    // test if a Table object can be passed to the hash function for the objects of type TBL.
    boost::hash<TBL> tbl_hasher;
    std::size_t c_hash = tbl_hasher(*tbl);
    std::size_t cpp_hash = tbl_hasher(static_cast<TBL>(table));
    ASSERT_EQ(c_hash, cpp_hash);
}

TEST_F(TablesTest, Dimension)
{
    EXPECT_EQ(table->nb_lines(), 31);
    EXPECT_EQ(table->nb_columns(), 2);
}

TEST_F(TablesTest, NotLineMethods)
{
    EXPECT_EQ(table->get_language(), "English");
    table->set_language(IT_FRENCH);
    EXPECT_EQ(table->get_language(), "French");

    EXPECT_FLOAT_EQ(table->t_zmin, (float) L_NAN);
    EXPECT_FLOAT_EQ(table->t_zmax, (float) L_NAN);
    EXPECT_FLOAT_EQ(table->t_ymin, (float) L_NAN);
    EXPECT_FLOAT_EQ(table->t_ymax, (float) L_NAN);
    table->t_zmin = 0;
    table->t_zmax = 10;
    table->t_ymin = 0;
    table->t_ymax = 100;
    EXPECT_FLOAT_EQ(table->t_zmin, 0.);
    EXPECT_FLOAT_EQ(table->t_zmax, 10.);
    EXPECT_FLOAT_EQ(table->t_ymin, 0.);
    EXPECT_FLOAT_EQ(table->t_ymax, 100.);

    EXPECT_EQ(table->t_attr, '\0');
    // TODO : test setAttribute()

    EXPECT_EQ(table->get_graph_alignment(), IG_LEFT);
    table->set_graph_alignment(IG_RIGHT);
    EXPECT_EQ(table->get_graph_alignment(), IG_RIGHT);

    EXPECT_EQ(table->t_box, '\0');
    // TODO : test setGraphBox()

    EXPECT_EQ(table->t_shadow, '\0');
    // TODO : test setGraphShadow()

    EXPECT_EQ(table->get_graph_axis(), IG_VALUES);
    table->set_graph_axis(IG_PERCENT);
    EXPECT_EQ(table->get_graph_axis(), IG_PERCENT);

    EXPECT_EQ(table->get_gridx(), IG_MAJOR);
    table->set_gridx(IG_MINOR);
    EXPECT_EQ(table->get_gridx(), IG_MINOR);

    EXPECT_EQ(table->get_gridy(), IG_MAJOR);
    table->set_gridy(IG_MINOR);
    EXPECT_EQ(table->get_gridy(), IG_MINOR);
}

TEST_F(TablesTest, Divider)
{
    EnumCellType expected_type = IT_LEC;
    EnumCellAlign expected_align = IT_DECIMAL;
    // expected font = IT_NORMAL;
    std::string content;
    std::string expected_content;
    int nb_cells = table->nb_columns();

    // get divider line
    TableLine* divider_line = table->get_divider_line();
    EXPECT_EQ(divider_line->get_line_type(), IT_CELL);

    // first cell
    TableCell* first_cell = divider_line->get_cell(0, nb_cells);
    EXPECT_EQ(first_cell->get_type(), expected_type);
    EXPECT_EQ(first_cell->get_align(), expected_align);
    EXPECT_EQ(first_cell->is_bold(), false);
    EXPECT_EQ(first_cell->is_italic(), false);
    EXPECT_EQ(first_cell->is_underline(), false);
    expected_content = "1";
    content = first_cell->get_content(false);
    EXPECT_EQ(content, expected_content);

    // second cell
    TableCell* second_cell = divider_line->get_cell(1, nb_cells);
    EXPECT_EQ(second_cell->get_type(), expected_type);
    EXPECT_EQ(second_cell->get_align(), expected_align);
    EXPECT_EQ(second_cell->is_bold(), false);
    EXPECT_EQ(second_cell->is_italic(), false);
    EXPECT_EQ(second_cell->is_underline(), false);
    expected_content = "PC";
    content = second_cell->get_content(false);
    EXPECT_EQ(content, expected_content);
}

TEST_F(TablesTest, LineTitle)
{
    int new_pos;
    std::string expected_title = "Compte de l'ensemble des administrations publiques";

    // get line title
    TableLine* line_title = table->get_line(0);

    // get content
    EXPECT_EQ(line_title->get_line_type(), EnumLineType::IT_TITLE);
    std::string title = table->get_title(0);
    EXPECT_EQ(title, expected_title + " ");

    // set content
    std::string new_title = "New Title with special characters !@µèéï";
    table->set_title(0, new_title);
    title = table->get_title(0);
    EXPECT_EQ(title, new_title);

    // add
    int nb_lines = table->nb_lines();
    new_title = "New title";
    TableLine* new_line_title = table->add_title(new_title);
    EXPECT_EQ(table->nb_lines(), nb_lines + 1);
    new_pos = table->nb_lines() - 1;
    title = table->get_title(new_pos);
    EXPECT_EQ(title, new_title);

    // delete
    table->delete_line(new_pos);
    EXPECT_EQ(table->nb_lines(), nb_lines);

    // insert 
    new_line_title = table->insert_title(25, new_title, true);
    EXPECT_EQ(table->nb_lines(), nb_lines + 1);
    new_pos = 26;
    title = table->get_title(new_pos);
    EXPECT_EQ(title, new_title);
}


TEST_F(TablesTest, LineCells)
{
    int nb_cells = table->nb_columns();
    EnumLineType expected_type = IT_CELL;

    // first CELL line
    TableLine* first_line = table->get_line(1);
    EXPECT_EQ(first_line->get_line_type(), expected_type);
    EXPECT_EQ(first_line->get_line_graph(), IG_LINE);
    EXPECT_TRUE(first_line->is_left_axis());
    first_line->set_line_graph(IG_BAR);
    EXPECT_EQ(first_line->get_line_graph(), IG_BAR);
    EXPECT_EQ(table->get_line(1)->get_line_graph(), IG_BAR);
    // ---- column 0
    TableCell* first_cell = first_line->get_cell(0, nb_cells);
    EXPECT_EQ(first_cell->get_type(), IT_STRING);
    EXPECT_EQ(first_cell->get_align(), IT_LEFT);
    EXPECT_FALSE(first_cell->is_bold());
    EXPECT_TRUE(first_cell->is_italic());
    EXPECT_FALSE(first_cell->is_underline());
    EXPECT_EQ(first_cell->get_content(false), "(divisé par les prix à la consommation)");
    EXPECT_EQ(first_cell->get_content(true), "\"(divisé par les prix à la consommation)\"");
    // ---- column 1
    TableCell* second_cell = first_line->get_cell(1, nb_cells);
    EXPECT_EQ(second_cell->get_type(), IT_STRING);
    EXPECT_EQ(second_cell->get_align(), IT_LEFT);
    EXPECT_FALSE(second_cell->is_bold());
    EXPECT_FALSE(second_cell->is_italic());
    EXPECT_FALSE(second_cell->is_underline());
    EXPECT_EQ(second_cell->get_content(false), "");

    // second CELL line
    TableLine* second_line = table->get_line(3);
    EXPECT_EQ(second_line->get_line_type(), expected_type);
    EXPECT_EQ(second_line->get_line_graph(), IG_LINE);
    EXPECT_TRUE(second_line->is_left_axis());
    // ---- column 0
    first_cell = second_line->get_cell(0, nb_cells);
    EXPECT_EQ(first_cell->get_type(), IT_STRING);
    EXPECT_EQ(first_cell->get_align(), IT_LEFT);
    EXPECT_FALSE(first_cell->is_bold());
    EXPECT_FALSE(first_cell->is_italic());
    EXPECT_FALSE(first_cell->is_underline());
    EXPECT_EQ(first_cell->get_content(false), " ");
    // ---- column 1
    second_cell = second_line->get_cell(1, nb_cells);
    EXPECT_EQ(second_cell->get_type(), IT_STRING);
    EXPECT_EQ(second_cell->get_align(), IT_CENTER);
    EXPECT_FALSE(second_cell->is_bold());
    EXPECT_FALSE(second_cell->is_italic());
    EXPECT_FALSE(second_cell->is_underline());
    EXPECT_EQ(second_cell->get_content(false), "#s");

    // third CELL line
    TableLine* third_line = table->get_line(5);
    EXPECT_EQ(third_line->get_line_type(), expected_type);
    EXPECT_EQ(third_line->get_line_graph(), IG_LINE);
    EXPECT_TRUE(third_line->is_left_axis());
    // ---- column 0
    first_cell = third_line->get_cell(0, nb_cells);
    EXPECT_EQ(first_cell->get_type(), IT_STRING);
    EXPECT_EQ(first_cell->get_align(), IT_LEFT);
    EXPECT_TRUE(first_cell->is_bold());
    EXPECT_FALSE(first_cell->is_italic());
    EXPECT_FALSE(first_cell->is_underline());
    EXPECT_EQ(first_cell->get_content(false), "Recettes courantes");
    // ---- column 1
    second_cell = third_line->get_cell(1, nb_cells);
    EXPECT_EQ(second_cell->get_type(), IT_LEC);
    EXPECT_EQ(second_cell->get_align(), IT_DECIMAL);
    EXPECT_FALSE(second_cell->is_bold());
    EXPECT_FALSE(second_cell->is_italic());
    EXPECT_FALSE(second_cell->is_underline());
    EXPECT_EQ(second_cell->get_content(false), "GOSG+YDTG+IT+YSSG+COTRES+OCUG+RIDG");

    // fifth CELL line
    TableLine* fifth_line = table->get_line(7);
    EXPECT_EQ(fifth_line->get_line_type(), expected_type);
    EXPECT_EQ(fifth_line->get_line_graph(), IG_LINE);
    EXPECT_TRUE(fifth_line->is_left_axis());
    // ---- column 0
    first_cell = fifth_line->get_cell(0, nb_cells);
    EXPECT_EQ(first_cell->get_type(), IT_STRING);
    EXPECT_EQ(first_cell->get_align(), IT_LEFT);
    EXPECT_FALSE(first_cell->is_bold());
    EXPECT_FALSE(first_cell->is_italic());
    EXPECT_FALSE(first_cell->is_underline());
    EXPECT_EQ(first_cell->get_content(false), "       1. Excédent brut d'exploitation");
    // ---- column 1
    second_cell = fifth_line->get_cell(1, nb_cells);
    EXPECT_EQ(second_cell->get_type(), IT_LEC);
    EXPECT_EQ(second_cell->get_align(), IT_DECIMAL);
    EXPECT_FALSE(second_cell->is_bold());
    EXPECT_FALSE(second_cell->is_italic());
    EXPECT_FALSE(second_cell->is_underline());
    EXPECT_EQ(second_cell->get_content(false), "GOSG");

    // is bold, italic, underline ?
    first_cell->set_align(IT_LEFT);
    first_cell->set_bold(true);
    first_cell->set_italic(true);
    first_cell = table->get_line(7)->get_cell(0, nb_cells);
    EXPECT_EQ(first_cell->get_align(), IT_LEFT);
    EXPECT_TRUE(first_cell->is_bold());
    EXPECT_TRUE(first_cell->is_italic());
    EXPECT_FALSE(first_cell->is_underline());

    // add - delete - insert
    int new_pos;
    std::string first_cell_content = third_line->get_cell(0, nb_cells)->get_content(false);      // string cell
    std::string second_cell_content = third_line->get_cell(1, nb_cells)->get_content(false);     // LEC cell

    // ---- add line
    int nb_lines = table->nb_lines();
    TableLine* new_line = table->add_line_with_cells();
    new_pos = table->nb_lines() - 1;
    EXPECT_EQ(table->nb_lines(), nb_lines + 1);

    first_cell = new_line->get_cell(0, nb_cells);
    first_cell->set_text(first_cell_content);
    EXPECT_EQ(table->get_line(new_pos)->get_cell(0, nb_cells)->get_type(), IT_STRING);
    EXPECT_EQ(table->get_line(new_pos)->get_cell(0, nb_cells)->get_content(true), "\"" + first_cell_content + "\"");

    second_cell = new_line->get_cell(1, nb_cells);
    second_cell->set_lec(second_cell_content);
    EXPECT_EQ(table->get_line(new_pos)->get_cell(1, nb_cells)->get_type(), IT_LEC);
    EXPECT_EQ(table->get_line(new_pos)->get_cell(1, nb_cells)->get_content(false), second_cell_content);

    // ---- delete line
    table->delete_line(new_pos);
    EXPECT_EQ(table->nb_lines(), nb_lines);

    // ---- insert line
    // WARNING: When inserting a new line of type IT_CELL, the attribute TCELL::tc_type of cells is undefined!
    //          See function `T_create_cell()` in file `k_tbl.c` from the C API 
    TableLine* new_line_cell = table->insert_line_with_cells(25, true);
    new_pos = 26;
    EXPECT_EQ(table->nb_lines(), nb_lines + 1);

    // Rule: If the content starts with a double quotes, we assume it is a string cell. 
    //       Otherwise, it is a LEC cell.
    new_line_cell->get_cell(0, nb_cells)->set_content("\"" + first_cell_content + "\"");
    EXPECT_EQ(table->get_line(new_pos)->get_cell(0, nb_cells)->get_type(), IT_STRING);
    EXPECT_EQ(table->get_line(new_pos)->get_cell(0, nb_cells)->get_content(true), "\"" + first_cell_content + "\"");

    // set_text() forces the cell to become a string cell
    new_line_cell->get_cell(0, nb_cells)->set_text(first_cell_content);
    EXPECT_EQ(table->get_line(new_pos)->get_cell(0, nb_cells)->get_type(), IT_STRING);
    EXPECT_EQ(table->get_line(new_pos)->get_cell(0, nb_cells)->get_content(false),  first_cell_content);

    // set_lec forces the cell to become a LEC cell
    new_line_cell->get_cell(1, nb_cells)->set_lec(second_cell_content);
    EXPECT_EQ(table->get_line(new_pos)->get_cell(1, nb_cells)->get_type(), IT_LEC);
    EXPECT_EQ(table->get_line(new_pos)->get_cell(1, nb_cells)->get_content(false), second_cell_content);
}

TEST_F(TablesTest, LineSeparator)
{
    int new_pos;
    std::vector<int> line_positions = {2, 4, 26, 30};

    for (const int row: line_positions) 
        EXPECT_EQ(table->get_line(row)->get_line_type(), IT_LINE);

    // add
    int nb_lines = table->nb_lines();
    TableLine* new_line = table->add_line_separator();
    new_pos = table->nb_lines() - 1;
    EXPECT_EQ(table->nb_lines(), nb_lines + 1);
    EXPECT_EQ(table->get_line(new_pos)->get_line_type(), IT_LINE);

    // delete
    table->delete_line(new_pos);
    EXPECT_EQ(table->nb_lines(), nb_lines);

    // insert 
    TableLine* line_inserted = table->insert_line_separator(25, true);
    new_pos = 26;
    EXPECT_EQ(table->nb_lines(), nb_lines + 1);
    EXPECT_EQ(table->get_line(new_pos)->get_line_type(), IT_LINE);
}

TEST_F(TablesTest, LineMode)
{
    int new_pos;

    EXPECT_EQ(table->get_line(27)->get_line_type(), IT_MODE);

    // add
    int nb_lines = table->nb_lines();
    TableLine* new_line = table->add_line_mode();
    new_pos = table->nb_lines() - 1;
    EXPECT_EQ(table->nb_lines(), nb_lines + 1);
    EXPECT_EQ(table->get_line(new_pos)->get_line_type(), IT_MODE);

    // delete
    table->delete_line(new_pos);
    EXPECT_EQ(table->nb_lines(), nb_lines);

    // insert 
    TableLine* line_inserted = table->insert_line_mode(25, true);
    new_pos = 26;
    EXPECT_EQ(table->nb_lines(), nb_lines + 1);
    EXPECT_EQ(table->get_line(new_pos)->get_line_type(), IT_MODE);
}

TEST_F(TablesTest, LineFiles)
{
    int new_pos;

    EXPECT_EQ(table->get_line(28)->get_line_type(), IT_FILES);

    // add
    int nb_lines = table->nb_lines();
    TableLine* new_line = table->add_line_files();
    new_pos = table->nb_lines() - 1;
    EXPECT_EQ(table->nb_lines(), nb_lines + 1);
    EXPECT_EQ(table->get_line(new_pos)->get_line_type(), IT_FILES);

    // delete
    table->delete_line(new_pos);
    EXPECT_EQ(table->nb_lines(), nb_lines);

    // insert 
    TableLine* line_inserted = table->insert_line_files(25, true);
    new_pos = 26;
    EXPECT_EQ(table->nb_lines(), nb_lines + 1);
    EXPECT_EQ(table->get_line(new_pos)->get_line_type(), IT_FILES);
}

TEST_F(TablesTest, LineDate)
{
    int new_pos;

    EXPECT_EQ(table->get_line(29)->get_line_type(), IT_DATE);

    // add
    int nb_lines = table->nb_lines();
    TableLine* new_line = table->add_line_date();
    new_pos = table->nb_lines() - 1;
    EXPECT_EQ(table->nb_lines(), nb_lines + 1);
    EXPECT_EQ(table->get_line(new_pos)->get_line_type(), IT_DATE);

    // delete
    table->delete_line(new_pos);
    EXPECT_EQ(table->nb_lines(), nb_lines);

    // insert 
    TableLine* line_inserted = table->insert_line_date(25, true);
    new_pos = 26;
    EXPECT_EQ(table->nb_lines(), nb_lines + 1);
    EXPECT_EQ(table->get_line(new_pos)->get_line_type(), IT_DATE);
}

TEST_F(TablesTest, Hash)
{
    boost::hash<Table> table_hasher;
    std::size_t hash_before;
    std::size_t hash_after;

    hash_before = table_hasher(*table);

    // same table
    Table* same_table = new Table("GFRPC", nullptr);
    EXPECT_EQ(*table, *same_table);
    hash_after = table_hasher(*same_table);
    EXPECT_EQ(hash_before, hash_after);

    // different title
    std::string new_title = "New Title";
    table->set_title(0, new_title);
    hash_after = table_hasher(*table);
    EXPECT_NE(hash_before, hash_after);

    // different cell
    int nb_cells = table->nb_columns();
    TableLine* line = table->get_line(5);
    hash_before = hash_after;
    line->get_cell(0, nb_cells)->set_text("Nouvelles recettes");
    hash_after = table_hasher(*table);
    EXPECT_NE(hash_before, hash_after);

    hash_before = hash_after;
    line->get_cell(1, nb_cells)->set_lec("GOSG+YDTG+IT+YSSG+COTRES+OCUG");
    hash_after = table_hasher(*table);
    EXPECT_NE(hash_before, hash_after);
}
