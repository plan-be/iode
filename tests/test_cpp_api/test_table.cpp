#include "pch.h"


class TablesTest : public KDBTest, public ::testing::Test
{
protected:
    Table* table;

    void SetUp() override
    {
        KDBTables kdb_tbl(input_test_dir + "fun.at");
        table = Tables.get("GFRPC");
    }

    void TearDown() override 
    {
        delete table;
    }
};


// Test that a Table* 'tbl' object added to KT_WS using K_add() and a Table* 'extracted_tbl' object 
// extracted using KTVAL() are exactly the same
TEST_F(TablesTest, AddGetTable)
{
    KDBComments kdb_cmt(input_test_dir + "fun.ac");
    KDBVariables kdb_var(input_test_dir + "fun.av");

    // --- create a C struct Table
    int nb_columns = 2;
    std::string title = "A title";
    std::vector<std::string> v_vars = { "GOSG", "YDTG", "DTH", "DTF", "IT", "YSSG+COTRES", "RIDG", "OCUG"};
    bool mode = true;
    bool files = true;
    bool date = true;

    Table* tbl = new Table(nb_columns, title, v_vars, mode, files, date);

    // test if the Table object has been correctly initialized
    TableCell* cell;
    TableLine* line;
    // --- divider line ---
    cell = &(tbl->divider_line.cells[0]);
    ASSERT_EQ(cell->get_type(), TABLE_CELL_LEC);
    ASSERT_EQ(cell->get_content(), "1");
    cell = &(tbl->divider_line.cells[1]);
    ASSERT_EQ(cell->get_type(), TABLE_CELL_LEC);
    // --- title line ---
    int i = 0;
    line = &tbl->lines[i++];
    ASSERT_EQ(line->get_type(), TABLE_LINE_TITLE);
    cell = &(line->cells[0]);
    ASSERT_EQ(cell->get_content(), title);
    // --- separator line ---
    line = &tbl->lines[i++];
    ASSERT_EQ(line->get_type(), TABLE_LINE_SEP);
    // --- first line ---
    line = &tbl->lines[i++];
    ASSERT_EQ(line->get_type(), TABLE_LINE_CELL);
    cell = &(line->cells[0]);
    ASSERT_EQ(cell->get_type(), TABLE_CELL_LEC);
    cell = &(line->cells[1]);
    ASSERT_EQ(cell->get_type(), TABLE_CELL_STRING);
    ASSERT_EQ(cell->get_content(), "#S");
    // --- separator line ---
    line = &tbl->lines[i++];
    ASSERT_EQ(line->get_type(), TABLE_LINE_SEP);
    // --- variables lines ---
    std::string comment;
    std::string variable;
    for(int v = 0; v < v_vars.size(); v++)
    {
        // std::cout << "testing line " << i << ", var = " << vars[v] << std::endl;
        line = &tbl->lines[i++];
        variable = v_vars[v];
        ASSERT_EQ(line->get_type(), TABLE_LINE_CELL);
        for(int j = 0; j < nb_columns; j++)
        {
            cell = &line->cells[j];
            if(j == 0)
            {
                ASSERT_EQ(cell->get_type(), TABLE_CELL_STRING);
                if(kdb_cmt.contains(variable))
                    ASSERT_EQ(cell->get_content(), kdb_cmt.get(variable));
                else
                    ASSERT_EQ(cell->get_content(), variable);
            }
            else
            {
                ASSERT_EQ(cell->get_type(), TABLE_CELL_LEC);
                ASSERT_EQ(cell->get_content(), variable);
            }
        }
    }
    // --- separator line ---
    line = &tbl->lines[i++];
    ASSERT_EQ(line->get_type(), TABLE_LINE_SEP);
    // --- mode/files/date lines ---
    ASSERT_EQ(tbl->lines[i++].get_type(), TABLE_LINE_MODE);
    ASSERT_EQ(tbl->lines[i++].get_type(), TABLE_LINE_FILES);
    ASSERT_EQ(tbl->lines[i++].get_type(), TABLE_LINE_DATE);

    // --- add the table to the Tables KDB
    char* name = "c_table";
    K_add(KT_WS, name, tbl);

    // --- extract the table from the Table KDB
    int pos = KT_WS->index_of(name);
    Table* extracted_tbl = KTVAL(KT_WS, pos);

    // --- check that both table are exactly the same
    // ----- check all attributes that are not of type TableLine 
    ASSERT_EQ(tbl->get_language(), extracted_tbl->get_language());
    ASSERT_EQ(tbl->repeat_columns, extracted_tbl->repeat_columns);
    ASSERT_EQ(tbl->nb_columns, extracted_tbl->nb_columns);
    ASSERT_EQ(tbl->lines.size(), extracted_tbl->lines.size());
    ASSERT_EQ(tbl->z_min, extracted_tbl->z_min);
    ASSERT_EQ(tbl->z_max, extracted_tbl->z_max);
    ASSERT_EQ(tbl->y_min, extracted_tbl->y_min);
    ASSERT_EQ(tbl->y_max, extracted_tbl->y_max);
    ASSERT_EQ(tbl->attribute, extracted_tbl->attribute);
    ASSERT_EQ(tbl->chart_box, extracted_tbl->chart_box);
    ASSERT_EQ(tbl->chart_shadow, extracted_tbl->chart_shadow);
    ASSERT_EQ(tbl->get_gridx(), extracted_tbl->get_gridx());
    ASSERT_EQ(tbl->get_gridy(), extracted_tbl->get_gridy());
    ASSERT_EQ(tbl->get_graph_axis(), extracted_tbl->get_graph_axis());
    ASSERT_EQ(tbl->get_text_alignment(), extracted_tbl->get_text_alignment());

    TableCell* cell_original;
    TableCell* cell_extracted;

    // ----- check div line 
    ASSERT_EQ(tbl->divider_line.get_type(), extracted_tbl->divider_line.get_type());
    ASSERT_EQ(tbl->divider_line.get_graph_type(), extracted_tbl->divider_line.get_graph_type());
    ASSERT_EQ(tbl->divider_line.right_axis, extracted_tbl->divider_line.right_axis);
    for(int j = 0; j < tbl->nb_columns; j++)
    {
        cell_original = &(tbl->divider_line.cells[j]);
        cell_extracted = &(extracted_tbl->divider_line.cells[j]);
        ASSERT_EQ(cell_original->get_type(), TABLE_CELL_LEC);
        ASSERT_EQ(cell_extracted->get_type(), TABLE_CELL_LEC);
        ASSERT_EQ(cell_original->get_attribute(), cell_extracted->get_attribute());
        if(j == 0)
            ASSERT_EQ(cell_original->get_attribute(), TABLE_CELL_LEFT);
        else
            ASSERT_EQ(cell_original->get_attribute(), TABLE_CELL_DECIMAL);
        ASSERT_EQ(cell_original->get_content(), cell_extracted->get_content());
    }

    // ----- check all lines 
    TableLine* line_original;
    TableLine* line_extracted;
    for(int i = 0; i < tbl->lines.size(); i++)
    {
        line_original = &tbl->lines[i];
        line_extracted = &extracted_tbl->lines[i];

        ASSERT_EQ(line_original->get_type(), line_extracted->get_type());
        ASSERT_EQ(line_original->get_graph_type(), line_extracted->get_graph_type());
        ASSERT_EQ(line_original->right_axis, line_extracted->right_axis);

        switch (line_original->get_type())
        {
        case TableLineType::TABLE_LINE_TITLE:
            cell_original = &line_original->cells[0];
            cell_extracted = &line_extracted->cells[0];
            ASSERT_EQ(cell_original->get_content(), cell_extracted->get_content());
            break;
        case TableLineType::TABLE_LINE_CELL:
            for(int j = 0; j < tbl->nb_columns; j++)
            {
                cell_original = &line_original->cells[j];
                cell_extracted = &line_extracted->cells[j];
                ASSERT_EQ(cell_original->get_type(), cell_extracted->get_type());
                ASSERT_EQ(cell_original->get_attribute(), cell_extracted->get_attribute());
                ASSERT_EQ(cell_original->get_content(), cell_extracted->get_content());
            }
            break;
        default:
            ASSERT_EQ(line_original->cells.size(), 0);
            ASSERT_EQ(line_extracted->cells.size(), 0);
            break;
        }
    }

    // --- free memory
    delete tbl;
    delete extracted_tbl;
}

TEST_F(TablesTest, Equivalence_C_CPP)
{
    TableLine* line;
    TableCell* cell;

    KDBVariables kdb_var(input_test_dir + "fun.av");

    char* c_name = "cpp_table";
    int nb_columns = 2;
    std::string def = "A title";
    std::vector<std::string> vars = { "GOSG", "YDTG", "DTH", "DTF", "IT", "YSSG+COTRES", "RIDG", "OCUG"};
    bool mode = true;
    bool files = true;
    bool date = true;

    // test if a Table object can be added to the Tables KDB via K_add()
    Table table(nb_columns, def, vars, mode, files, date);

    // test if the Table object has been correctly initialized
    // --- divider line ---
    cell = &(table.divider_line.cells[0]);
    ASSERT_EQ(cell->get_type(), TABLE_CELL_LEC);
    ASSERT_EQ(cell->get_content(), "1");
    cell = &(table.divider_line.cells[1]);
    ASSERT_EQ(cell->get_type(), TABLE_CELL_LEC);
    // --- title line ---
    int i = 0;
    line = &table.lines[i++];
    ASSERT_EQ(line->get_type(), TABLE_LINE_TITLE);
    cell = &(line->cells[0]);
    ASSERT_EQ(cell->get_content(), "A title");
    // --- separator line ---
    line = &table.lines[i++];
    ASSERT_EQ(line->get_type(), TABLE_LINE_SEP);
    // --- first line ---
    line = &table.lines[i++];
    ASSERT_EQ(line->get_type(), TABLE_LINE_CELL);
    cell = &(line->cells[0]);
    ASSERT_EQ(cell->get_type(), TABLE_CELL_LEC);
    cell = &(line->cells[1]);
    ASSERT_EQ(cell->get_type(), TABLE_CELL_STRING);
    ASSERT_EQ(cell->get_content(), "#S");
    // --- separator line ---
    line = &table.lines[i++];
    ASSERT_EQ(line->get_type(), TABLE_LINE_SEP);
    // --- variables lines ---
    for(int v = 0; v < vars.size(); v++)
    {
        // std::cout << "testing line " << i << ", var = " << vars[v] << std::endl;
        line = &table.lines[i++];
        ASSERT_EQ(line->get_type(), TABLE_LINE_CELL);
        for(int j = 0; j < nb_columns; j++)
        {
            cell = &line->cells[j];
            if(j == 0)
            {
                ASSERT_EQ(cell->get_type(), TABLE_CELL_STRING);
                ASSERT_EQ(cell->get_content(), vars[v]);
            }
            else
            {
                ASSERT_EQ(cell->get_type(), TABLE_CELL_LEC);
                ASSERT_EQ(cell->get_content(), vars[v]);
            }
        }
    }
    // --- separator line ---
    line = &table.lines[i++];
    ASSERT_EQ(line->get_type(), TABLE_LINE_SEP);
    // --- mode/files/date lines ---
    ASSERT_EQ(table.lines[i++].get_type(), TABLE_LINE_MODE);
    ASSERT_EQ(table.lines[i++].get_type(), TABLE_LINE_FILES);
    ASSERT_EQ(table.lines[i++].get_type(), TABLE_LINE_DATE);

    K_add(KT_WS, c_name, static_cast<Table*>(&table));
    int pos = KT_WS->index_of(c_name);
    ASSERT_GT(pos, -1);

    Table* tbl = KTVAL(KT_WS, pos);
    // test if the restored Table object is the same as the original one
    // --- divider line ---
    cell = &(tbl->divider_line.cells[0]);
    ASSERT_EQ(cell->get_type(), TABLE_CELL_LEC);
    ASSERT_EQ(cell->get_content(), "1");
    cell = &(tbl->divider_line.cells[1]);
    ASSERT_EQ(cell->get_type(), TABLE_CELL_LEC);
    // --- title line ---
    i = 0;
    line = &tbl->lines[i++];
    cell = &(line->cells[0]);
    ASSERT_EQ(cell->get_content(), "A title");
    // --- separator line ---
    ASSERT_EQ(tbl->lines[i++].get_type(), TABLE_LINE_SEP);
    // --- first line ---
    line = &tbl->lines[i++];
    cell = &(line->cells[0]);
    ASSERT_EQ(cell->get_type(), TABLE_CELL_LEC);
    cell = &(line->cells[1]);
    ASSERT_EQ(cell->get_type(), TABLE_CELL_STRING);
    ASSERT_EQ(cell->get_content(), "#S");
    // --- separator line ---
    ASSERT_EQ(tbl->lines[i++].get_type(), TABLE_LINE_SEP);
    // --- variables lines ---
    for(int v = 0; v < vars.size(); v++)
    {
        // std::cout << "testing line " << i << ", var = " << vars[v] << std::endl;
        line = &tbl->lines[i++];
        ASSERT_EQ(line->get_type(), TABLE_LINE_CELL);
        for(int j = 0; j < nb_columns; j++)
        {
            cell = &line->cells[j];
            if(j == 0)
            {
                ASSERT_EQ(cell->get_type(), TABLE_CELL_STRING);
                ASSERT_EQ(cell->get_content(), vars[v]);
            }
            else
            {
                ASSERT_EQ(cell->get_type(), TABLE_CELL_LEC);
                ASSERT_EQ(cell->get_content(), vars[v]);
            }
        }
    }
    // --- separator line ---
    ASSERT_EQ(tbl->lines[i++].get_type(), TABLE_LINE_SEP);
    // --- mode/files/date lines ---
    ASSERT_EQ(tbl->lines[i++].get_type(), TABLE_LINE_MODE);
    ASSERT_EQ(tbl->lines[i++].get_type(), TABLE_LINE_FILES);
    ASSERT_EQ(tbl->lines[i++].get_type(), TABLE_LINE_DATE);

    // test if a Table object can be passed to the hash function for the objects of type Table.
    std::hash<Table> tbl_hasher;
    std::size_t c_hash = tbl_hasher(*tbl);
    std::size_t cpp_hash = tbl_hasher(static_cast<Table>(table));
    ASSERT_EQ(c_hash, cpp_hash);
}

TEST_F(TablesTest, CopyConstructor)
{
    int nb_columns = table->nb_columns;
    KDBVariables kdb_var(input_test_dir + "fun.var");

    Table copied_table(*table);
    ASSERT_EQ(*table, copied_table);

    TableLine& line0 = table->lines[0];
    TableLine copied_line0(line0);
    ASSERT_EQ(line0, copied_line0);

    TableLine& line5 = table->lines[5];
    TableLine copied_line5(line5);
    ASSERT_EQ(line5, copied_line5);

    TableCell& cell0 = line5.cells[0];
    TableCell copied_cell0(cell0);
    ASSERT_EQ(cell0, copied_cell0);
 
    TableCell& cell1 = line5.cells[1];
    TableCell copied_cell1(cell1);
    ASSERT_EQ(cell1, copied_cell1);
}

TEST_F(TablesTest, Dimension)
{
    EXPECT_EQ(table->lines.size(), 31);
    EXPECT_EQ(table->nb_columns, 2);
}

TEST_F(TablesTest, NotLineMethods)
{
    EXPECT_EQ(table->get_language_as_string(), "English");
    table->set_language(TABLE_FRENCH);
    EXPECT_EQ(table->get_language_as_string(), "French");

    EXPECT_FLOAT_EQ(table->z_min, (float) IODE_NAN);
    EXPECT_FLOAT_EQ(table->z_max, (float) IODE_NAN);
    EXPECT_FLOAT_EQ(table->y_min, (float) IODE_NAN);
    EXPECT_FLOAT_EQ(table->y_max, (float) IODE_NAN);
    table->z_min = 0;
    table->z_max = 10;
    table->y_min = 0;
    table->y_max = 100;
    EXPECT_FLOAT_EQ(table->z_min, 0.);
    EXPECT_FLOAT_EQ(table->z_max, 10.);
    EXPECT_FLOAT_EQ(table->y_min, 0.);
    EXPECT_FLOAT_EQ(table->y_max, 100.);

    EXPECT_EQ(table->attribute, '\0');
    // TODO : test setAttribute()

    EXPECT_EQ(table->get_text_alignment(), TABLE_TEXT_LEFT);
    table->set_text_alignment(TABLE_TEXT_RIGHT);
    EXPECT_EQ(table->get_text_alignment(), TABLE_TEXT_RIGHT);

    EXPECT_EQ(table->chart_box, '\0');
    // TODO : test setGraphBox()

    EXPECT_EQ(table->chart_shadow, '\0');
    // TODO : test setGraphShadow()

    EXPECT_EQ(table->get_graph_axis(), TABLE_GRAPH_VALUES);
    table->set_graph_axis(TABLE_GRAPH_PERCENT);
    EXPECT_EQ(table->get_graph_axis(), TABLE_GRAPH_PERCENT);

    EXPECT_EQ(table->get_gridx(), TABLE_GRAPH_MAJOR);
    table->set_gridx(TABLE_GRAPH_MINOR);
    EXPECT_EQ(table->get_gridx(), TABLE_GRAPH_MINOR);

    EXPECT_EQ(table->get_gridy(), TABLE_GRAPH_MAJOR);
    table->set_gridy(TABLE_GRAPH_MINOR);
    EXPECT_EQ(table->get_gridy(), TABLE_GRAPH_MINOR);
}

TEST_F(TablesTest, Divider)
{
    std::string content;
    std::string expected_content;
    int nb_cells = table->nb_columns;

    // get divider line
    TableLine& divider_line = table->divider_line;
    EXPECT_EQ(divider_line.get_type(), TABLE_LINE_CELL);

    // first cell
    TableCell& first_cell = divider_line.cells[0];
    EXPECT_EQ(first_cell.get_type(), TABLE_CELL_LEC);
    EXPECT_EQ((int) first_cell.get_align(), TABLE_CELL_DECIMAL);
    EXPECT_EQ(first_cell.is_bold(), false);
    EXPECT_EQ(first_cell.is_italic(), false);
    EXPECT_EQ(first_cell.is_underline(), false);
    expected_content = "1";
    content = first_cell.get_content(false);
    EXPECT_EQ(content, expected_content);

    // second cell
    TableCell& second_cell = divider_line.cells[1];
    EXPECT_EQ(second_cell.get_type(), TABLE_CELL_LEC);
    EXPECT_EQ((int) second_cell.get_align(), TABLE_CELL_DECIMAL);
    EXPECT_EQ(second_cell.is_bold(), false);
    EXPECT_EQ(second_cell.is_italic(), false);
    EXPECT_EQ(second_cell.is_underline(), false);
    expected_content = "PC";
    content = second_cell.get_content(false);
    EXPECT_EQ(content, expected_content);

    // update first cell
    std::string new_content = "0.5";
    first_cell.set_content(new_content);
    content = first_cell.get_content(false);
    EXPECT_EQ(content, new_content);
    content = table->divider_line.cells[0].get_content(false);
    EXPECT_EQ(content, new_content);

    // update second cell
    std::string new_lec = "GOSG";
    second_cell.set_content(new_lec);
    content = second_cell.get_content(false);
    EXPECT_EQ(content, new_lec);
    content = table->divider_line.cells[1].get_content(false);
    EXPECT_EQ(content, new_lec);
}

TEST_F(TablesTest, LineTitle)
{
    int new_pos;
    std::string expected_title = "Compte de l'ensemble des administrations publiques";

    // get line title
    TableLine& line_title = table->lines[0];

    // get content
    EXPECT_EQ(line_title.get_type(), TableLineType::TABLE_LINE_TITLE);
    std::string title = table->get_title(0);
    EXPECT_EQ(title, expected_title + " ");

    // set content
    std::string new_title = "New Title with special characters !@µèéï";
    table->set_title(0, new_title);
    title = table->get_title(0);
    EXPECT_EQ(title, new_title);

    // add
    int nb_lines = table->lines.size();
    new_title = "New title";
    TableLine* new_line_title = table->add_title(new_title);
    EXPECT_EQ(table->lines.size(), nb_lines + 1);
    new_pos = table->lines.size() - 1;
    title = table->get_title(new_pos);
    EXPECT_EQ(title, new_title);

    // delete
    table->remove_line(new_pos);
    EXPECT_EQ(table->lines.size(), nb_lines);

    // insert 
    new_line_title = table->insert_title(25, new_title, true);
    EXPECT_EQ(table->lines.size(), nb_lines + 1);
    new_pos = 26;
    title = table->get_title(new_pos);
    EXPECT_EQ(title, new_title);
}


TEST_F(TablesTest, LineCells)
{
    int nb_cells = table->nb_columns;
    TableLineType expected_type = TABLE_LINE_CELL;
    TableCell* first_cell;
    TableCell* second_cell;

    // first CELL line
    TableLine& first_line = table->lines[1];
    EXPECT_EQ(first_line.get_type(), expected_type);
    EXPECT_EQ(first_line.get_graph_type(), TABLE_GRAPH_LINE);
    EXPECT_FALSE(first_line.right_axis);
    first_line.set_graph_type(TABLE_GRAPH_BAR);
    EXPECT_EQ(first_line.get_graph_type(), TABLE_GRAPH_BAR);
    EXPECT_EQ(table->lines[1].get_graph_type(), TABLE_GRAPH_BAR);
    // ---- column 0
    first_cell = &first_line.cells[0];
    EXPECT_EQ(first_cell->get_type(), TABLE_CELL_STRING);
    EXPECT_EQ(first_cell->get_align(), TABLE_CELL_LEFT);
    EXPECT_FALSE(first_cell->is_bold());
    EXPECT_TRUE(first_cell->is_italic());
    EXPECT_FALSE(first_cell->is_underline());
    EXPECT_EQ(first_cell->get_content(false), "(divisé par les prix à la consommation)");
    EXPECT_EQ(first_cell->get_content(true), "\"(divisé par les prix à la consommation)\"");
    // ---- column 1
    second_cell = &first_line.cells[1];
    EXPECT_EQ(second_cell->get_type(), TABLE_CELL_STRING);
    EXPECT_EQ((int) second_cell->get_align(), TABLE_CELL_LEFT);
    EXPECT_FALSE(second_cell->is_bold());
    EXPECT_FALSE(second_cell->is_italic());
    EXPECT_FALSE(second_cell->is_underline());
    EXPECT_EQ(second_cell->get_content(false), "");

    // second CELL line
    TableLine& second_line = table->lines[3];
    EXPECT_EQ(second_line.get_type(), expected_type);
    EXPECT_EQ(second_line.get_graph_type(), TABLE_GRAPH_LINE);
    EXPECT_FALSE(second_line.right_axis);
    // ---- column 0
    first_cell = &second_line.cells[0];
    EXPECT_EQ(first_cell->get_type(), TABLE_CELL_STRING);
    EXPECT_EQ(first_cell->get_align(), TABLE_CELL_LEFT);
    EXPECT_FALSE(first_cell->is_bold());
    EXPECT_FALSE(first_cell->is_italic());
    EXPECT_FALSE(first_cell->is_underline());
    EXPECT_EQ(first_cell->get_content(false), " ");
    // ---- column 1
    second_cell = &second_line.cells[1];
    EXPECT_EQ(second_cell->get_type(), TABLE_CELL_STRING);
    EXPECT_EQ(second_cell->get_align(), TABLE_CELL_CENTER);
    EXPECT_FALSE(second_cell->is_bold());
    EXPECT_FALSE(second_cell->is_italic());
    EXPECT_FALSE(second_cell->is_underline());
    EXPECT_EQ(second_cell->get_content(false), "#s");

    // third CELL line
    TableLine& third_line = table->lines[5];
    EXPECT_EQ(third_line.get_type(), expected_type);
    EXPECT_EQ(third_line.get_graph_type(), TABLE_GRAPH_LINE);
    EXPECT_FALSE(third_line.right_axis);
    // ---- column 0
    first_cell = &third_line.cells[0];
    EXPECT_EQ(first_cell->get_type(), TABLE_CELL_STRING);
    EXPECT_EQ(first_cell->get_align(), TABLE_CELL_LEFT);
    EXPECT_TRUE(first_cell->is_bold());
    EXPECT_FALSE(first_cell->is_italic());
    EXPECT_FALSE(first_cell->is_underline());
    EXPECT_EQ(first_cell->get_content(false), "Recettes courantes");
    // ---- column 1
    second_cell = &third_line.cells[1];
    EXPECT_EQ(second_cell->get_type(), TABLE_CELL_LEC);
    EXPECT_EQ(second_cell->get_align(), TABLE_CELL_DECIMAL);
    EXPECT_FALSE(second_cell->is_bold());
    EXPECT_FALSE(second_cell->is_italic());
    EXPECT_FALSE(second_cell->is_underline());
    EXPECT_EQ(second_cell->get_content(false), "GOSG+YDTG+IT+YSSG+COTRES+OCUG+RIDG");

    // fifth CELL line
    TableLine& fifth_line = table->lines[7];
    EXPECT_EQ(fifth_line.get_type(), expected_type);
    EXPECT_EQ(fifth_line.get_graph_type(), TABLE_GRAPH_LINE);
    EXPECT_FALSE(fifth_line.right_axis);
    // ---- column 0
    first_cell = &fifth_line.cells[0];
    EXPECT_EQ(first_cell->get_type(), TABLE_CELL_STRING);
    EXPECT_EQ(first_cell->get_align(), TABLE_CELL_LEFT);
    EXPECT_FALSE(first_cell->is_bold());
    EXPECT_FALSE(first_cell->is_italic());
    EXPECT_FALSE(first_cell->is_underline());
    EXPECT_EQ(first_cell->get_content(false), "       1. Excédent brut d'exploitation");
    // ---- column 1
    second_cell = &fifth_line.cells[1];
    EXPECT_EQ(second_cell->get_type(), TABLE_CELL_LEC);
    EXPECT_EQ(second_cell->get_align(), TABLE_CELL_DECIMAL);
    EXPECT_FALSE(second_cell->is_bold());
    EXPECT_FALSE(second_cell->is_italic());
    EXPECT_FALSE(second_cell->is_underline());
    EXPECT_EQ(second_cell->get_content(false), "GOSG");

    // is bold, italic, underline ?
    first_cell->set_align(TABLE_CELL_LEFT);
    first_cell->set_bold(true);
    first_cell->set_italic(true);
    first_cell = &(table->lines[7].cells[0]);
    EXPECT_EQ(first_cell->get_align(), TABLE_CELL_LEFT);
    EXPECT_TRUE(first_cell->is_bold());
    EXPECT_TRUE(first_cell->is_italic());
    EXPECT_FALSE(first_cell->is_underline());

    // update cells of a line
    TableLine& updated_fifth_line = table->lines[7];
    // ---- column 0
    first_cell = &updated_fifth_line.cells[0];
    first_cell->set_text(" -- DTH -- ");
    EXPECT_EQ(first_cell->get_content(false), " -- DTH -- ");
    EXPECT_EQ(table->lines[7].cells[0].get_content(false), " -- DTH -- ");
    // ---- column 1
    second_cell = &updated_fifth_line.cells[1];
    second_cell->set_lec("DTH");
    EXPECT_EQ(second_cell->get_content(false), "DTH");
    EXPECT_EQ(table->lines[7].cells[1].get_content(false), "DTH");

    // add - delete - insert
    int new_pos;
    std::string first_cell_content = third_line.cells[0].get_content(false);      // string cell
    std::string second_cell_content = third_line.cells[1].get_content(false);     // LEC cell

    // ---- add line
    int nb_lines = table->lines.size();
    TableLine* new_line = table->add_line_with_cells();
    new_pos = table->lines.size() - 1;
    EXPECT_EQ(table->lines.size(), nb_lines + 1);

    first_cell = &new_line->cells[0];
    first_cell->set_text(first_cell_content);
    EXPECT_EQ(table->lines[new_pos].cells[0].get_type(), TABLE_CELL_STRING);
    EXPECT_EQ(table->lines[new_pos].cells[0].get_content(true), "\"" + first_cell_content + "\"");

    second_cell = &new_line->cells[1];
    second_cell->set_lec(second_cell_content);
    EXPECT_EQ(table->lines[new_pos].cells[1].get_type(), TABLE_CELL_LEC);
    EXPECT_EQ(table->lines[new_pos].cells[1].get_content(false), second_cell_content);

    // ---- delete line
    table->remove_line(new_pos);
    EXPECT_EQ(table->lines.size(), nb_lines);

    // ---- insert line
    TableLine* new_line_cell = table->insert_line_with_cells(25, true);
    new_pos = 26;
    EXPECT_EQ(table->lines.size(), nb_lines + 1);

    // Rule: If the content starts with a double quotes, we assume it is a string cell. 
    //       Otherwise, it is a LEC cell.
    new_line_cell->cells[0].set_content("\"" + first_cell_content + "\"");
    EXPECT_EQ(table->lines[new_pos].cells[0].get_type(), TABLE_CELL_STRING);
    EXPECT_EQ(table->lines[new_pos].cells[0].get_content(true), "\"" + first_cell_content + "\"");

    // set_text() forces the cell to become a string cell
    new_line_cell->cells[0].set_text(first_cell_content);
    EXPECT_EQ(table->lines[new_pos].cells[0].get_type(), TABLE_CELL_STRING);
    EXPECT_EQ(table->lines[new_pos].cells[0].get_content(false),  first_cell_content);

    // set_lec forces the cell to become a LEC cell
    new_line_cell->cells[1].set_lec(second_cell_content);
    EXPECT_EQ(table->lines[new_pos].cells[1].get_type(), TABLE_CELL_LEC);
    EXPECT_EQ(table->lines[new_pos].cells[1].get_content(false), second_cell_content);
}

TEST_F(TablesTest, LineSeparator)
{
    int new_pos;
    std::vector<int> line_positions = {2, 4, 26, 30};

    for (const int row: line_positions) 
        EXPECT_EQ(table->lines[row].get_type(), TABLE_LINE_SEP);

    // add
    int nb_lines = table->lines.size();
    TableLine* new_line = table->add_line_separator();
    new_pos = table->lines.size() - 1;
    EXPECT_EQ(table->lines.size(), nb_lines + 1);
    EXPECT_EQ(table->lines[new_pos].get_type(), TABLE_LINE_SEP);

    // delete
    table->remove_line(new_pos);
    EXPECT_EQ(table->lines.size(), nb_lines);

    // insert 
    TableLine* line_inserted = table->insert_line_separator(25, true);
    new_pos = 26;
    EXPECT_EQ(table->lines.size(), nb_lines + 1);
    EXPECT_EQ(table->lines[new_pos].get_type(), TABLE_LINE_SEP);
}

TEST_F(TablesTest, LineMode)
{
    int new_pos;

    EXPECT_EQ(table->lines[27].get_type(), TABLE_LINE_MODE);

    // add
    int nb_lines = table->lines.size();
    TableLine* new_line = table->add_line_mode();
    new_pos = table->lines.size() - 1;
    EXPECT_EQ(table->lines.size(), nb_lines + 1);
    EXPECT_EQ(table->lines[new_pos].get_type(), TABLE_LINE_MODE);

    // delete
    table->remove_line(new_pos);
    EXPECT_EQ(table->lines.size(), nb_lines);

    // insert 
    TableLine* line_inserted = table->insert_line_mode(25, true);
    new_pos = 26;
    EXPECT_EQ(table->lines.size(), nb_lines + 1);
    EXPECT_EQ(table->lines[new_pos].get_type(), TABLE_LINE_MODE);
}

TEST_F(TablesTest, LineFiles)
{
    int new_pos;

    EXPECT_EQ(table->lines[28].get_type(), TABLE_LINE_FILES);

    // add
    int nb_lines = table->lines.size();
    TableLine* new_line = table->add_line_files();
    new_pos = table->lines.size() - 1;
    EXPECT_EQ(table->lines.size(), nb_lines + 1);
    EXPECT_EQ(table->lines[new_pos].get_type(), TABLE_LINE_FILES);

    // delete
    table->remove_line(new_pos);
    EXPECT_EQ(table->lines.size(), nb_lines);

    // insert 
    TableLine* line_inserted = table->insert_line_files(25, true);
    new_pos = 26;
    EXPECT_EQ(table->lines.size(), nb_lines + 1);
    EXPECT_EQ(table->lines[new_pos].get_type(), TABLE_LINE_FILES);
}

TEST_F(TablesTest, LineDate)
{
    int new_pos;

    EXPECT_EQ(table->lines[29].get_type(), TABLE_LINE_DATE);

    // add
    int nb_lines = table->lines.size();
    TableLine* new_line = table->add_line_date();
    new_pos = table->lines.size() - 1;
    EXPECT_EQ(table->lines.size(), nb_lines + 1);
    EXPECT_EQ(table->lines[new_pos].get_type(), TABLE_LINE_DATE);

    // delete
    table->remove_line(new_pos);
    EXPECT_EQ(table->lines.size(), nb_lines);

    // insert 
    TableLine* line_inserted = table->insert_line_date(25, true);
    new_pos = 26;
    EXPECT_EQ(table->lines.size(), nb_lines + 1);
    EXPECT_EQ(table->lines[new_pos].get_type(), TABLE_LINE_DATE);
}

TEST_F(TablesTest, ListCoefficients)
{
    Table* table = Tables.get("ANAKNFF");    
    TableLine& line_5 = table->lines[5];
    EXPECT_EQ(line_5.get_type(), TABLE_LINE_CELL);
    TableCell& first_cell = line_5.cells[0];
    EXPECT_EQ(first_cell.get_content(false), "Output gap ");
    TableCell& second_cell = line_5.cells[1];
    EXPECT_EQ(second_cell.get_content(false), "knff1*ln (QAFF_/(Q_F+Q_I))");

    std::vector<std::string> expected_coeffs = { "knff1" };
    std::vector<std::string> coeffs = second_cell.get_coefficients_from_lec();
    EXPECT_EQ(coeffs, expected_coeffs);
}

TEST_F(TablesTest, ListVariables)
{
    Table* table = Tables.get("ANAKNFF");    
    TableLine& line_5 = table->lines[5];
    EXPECT_EQ(line_5.get_type(), TABLE_LINE_CELL);
    TableCell& first_cell = line_5.cells[0];
    EXPECT_EQ(first_cell.get_content(false), "Output gap ");
    TableCell& second_cell = line_5.cells[1];
    EXPECT_EQ(second_cell.get_content(false), "knff1*ln (QAFF_/(Q_F+Q_I))");

    std::vector<std::string> expected_vars = { "QAFF_", "Q_F", "Q_I" };
    std::vector<std::string> vars = second_cell.get_variables_from_lec();
    EXPECT_EQ(vars, expected_vars);
}

TEST_F(TablesTest, Hash)
{
    std::hash<Table> table_hasher;
    std::size_t hash_original;
    std::size_t hash_before;
    std::size_t hash_after;

    hash_before = table_hasher(*table);
    hash_original = hash_before;

    // same table
    Table* same_table = Tables.get("GFRPC");
    EXPECT_EQ(*table, *same_table);
    hash_after = table_hasher(*same_table);
    EXPECT_EQ(hash_before, hash_after);

    // C++ table vs C table
    int pos = KT_WS->index_of("GFRPC");
    Table* c_table = KTVAL(KT_WS, pos);
    std::size_t c_hash = table_hasher(*c_table);
    EXPECT_EQ(hash_original, c_hash);

    // different title
    Table* table_before = new Table(*table);
    std::string title = table->get_title(0);
    std::string new_title = "New Title";
    table->set_title(0, new_title);
    EXPECT_NE(*table, *table_before);
    hash_after = table_hasher(*table);
    EXPECT_NE(hash_before, hash_after);
    delete table_before;

    // different cell
    table_before = new Table(*table);
    int nb_cells = table->nb_columns;
    TableLine& line = table->lines[5];
    hash_before = hash_after;
    std::string cell_content = line.cells[0].get_content(false);
    line.cells[0].set_text("Nouvelles recettes");
    EXPECT_NE(*table, *table_before);
    hash_after = table_hasher(*table);
    EXPECT_NE(hash_before, hash_after);
    delete table_before;

    table_before = new Table(*table);
    hash_before = hash_after;
    std::string lec_content = line.cells[1].get_content(false);
    line.cells[1].set_lec("GOSG+YDTG+IT+YSSG+COTRES+OCUG");
    EXPECT_NE(*table, *table_before);
    hash_after = table_hasher(*table);
    EXPECT_NE(hash_before, hash_after);
    delete table_before;

    // return to original title and cell content
    table->set_title(0, title);
    line.cells[0].set_text(cell_content);
    line.cells[1].set_lec(lec_content);
    EXPECT_EQ(*table, *same_table);
    hash_after = table_hasher(*table);
    EXPECT_EQ(hash_original, hash_after);
}
