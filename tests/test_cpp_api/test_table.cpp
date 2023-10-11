#include "pch.h"


class TablesTest : public KDBTest, public ::testing::Test
{
protected:
    Table* table;
    KDBTables kdb;

    void SetUp() override
    {
        load_global_kdb(I_TABLES, input_test_dir + "fun.tbl");

        int pos = K_find(K_WS[I_TABLES], "GFRPC");
        table = new Table(pos);
    }

    void TearDown() override 
    {
        delete table;
    }
};

TEST_F(TablesTest, Dimension)
{
    EXPECT_EQ(table->nbLines(), 31);
    EXPECT_EQ(table->nbColumns(), 2);
}

TEST_F(TablesTest, NotLineMethods)
{
    EXPECT_EQ(table->getLanguage(), "English");
    table->setLanguage(IT_FRENCH);
    EXPECT_EQ(table->getLanguage(), "French");

    EXPECT_FLOAT_EQ(table->getZMin(), (float) L_NAN);
    EXPECT_FLOAT_EQ(table->getZMax(), (float) L_NAN);
    EXPECT_FLOAT_EQ(table->getYMin(), (float) L_NAN);
    EXPECT_FLOAT_EQ(table->getYMax(), (float) L_NAN);
    table->setZMin(0);
    table->setZMax(10);
    table->setYMin(0);
    table->setYMax(100);
    EXPECT_FLOAT_EQ(table->getZMin(), 0.);
    EXPECT_FLOAT_EQ(table->getZMax(), 10.);
    EXPECT_FLOAT_EQ(table->getYMin(), 0.);
    EXPECT_FLOAT_EQ(table->getYMax(), 100.);

    EXPECT_EQ(table->getAttribute(), '\0');
    // TODO : test setAttribute()

    EXPECT_EQ(table->getGraphAlignement(), IG_LEFT);
    table->setGraphAlignement(IG_RIGHT);
    EXPECT_EQ(table->getGraphAlignement(), IG_RIGHT);

    EXPECT_EQ(table->getGraphBox(), '\0');
    // TODO : test setGraphBox()

    EXPECT_EQ(table->getGraphShadow(), '\0');
    // TODO : test setGraphShadow()

    EXPECT_EQ(table->getGraphAxis(), IG_VALUES);
    table->setGraphAxis(IG_PERCENT);
    EXPECT_EQ(table->getGraphAxis(), IG_PERCENT);

    EXPECT_EQ(table->getGridX(), IG_MAJOR);
    table->setGridX(IG_MINOR);
    EXPECT_EQ(table->getGridX(), IG_MINOR);

    EXPECT_EQ(table->getGridY(), IG_MAJOR);
    table->setGridY(IG_MINOR);
    EXPECT_EQ(table->getGridY(), IG_MINOR);
}

TEST_F(TablesTest, Divider)
{
    EnumCellType expected_type = IT_LEC;
    EnumCellAttribute expected_align = IT_DECIMAL;
    EnumCellAttribute expected_font = IT_NORMAL;
    std::string content;
    std::string expected_content;

    // first cell
    EXPECT_EQ(table->getDividerCellType(0), expected_type);
    EXPECT_EQ(table->getDividerCellAlign(0), expected_align);
    EXPECT_EQ(table->getDividerCellFont(0), expected_font);
    expected_content = "1";
    content = table->getDividerCellContent(0, false);
    EXPECT_EQ(content, expected_content);

    // second cell
    EXPECT_EQ(table->getDividerCellType(1), expected_type);
    EXPECT_EQ(table->getDividerCellAlign(1), expected_align);
    EXPECT_EQ(table->getDividerCellFont(1), expected_font);
    expected_content = "PC";
    content = table->getDividerCellContent(1, false);
    EXPECT_EQ(content, expected_content);
}

TEST_F(TablesTest, LineTitle)
{
    int new_pos;
    std::string expected_title = "Compte de l'ensemble des administrations publiques";

    // get
    EXPECT_EQ(table->getLineType(0), IT_TITLE);
    std::string title = table->getTitle(0);
    EXPECT_EQ(title, expected_title + " ");

    // set
    std::string new_title = "New Title with special characters !@µèéï";
    table->setTitle(0, new_title);
    title = table->getTitle(0);
    EXPECT_EQ(title, new_title);

    // add
    int nb_lines = table->nbLines();
    new_title = "New title";
    new_pos = table->addTitle(new_title);
    EXPECT_EQ(table->nbLines(), nb_lines + 1);
    EXPECT_EQ(new_pos, table->nbLines() - 1);
    title = table->getTitle(new_pos);
    EXPECT_EQ(title, new_title);

    // delete
    table->deleteLine(new_pos);
    EXPECT_EQ(table->nbLines(), nb_lines);

    // insert 
    new_pos = table->insertTitle(25, new_title, true);
    EXPECT_EQ(table->nbLines(), nb_lines + 1);
    EXPECT_EQ(new_pos, 26);
    title = table->getTitle(new_pos);
    EXPECT_EQ(title, new_title);
}


TEST_F(TablesTest, LineCells)
{
    EnumLineType expected_type = IT_CELL;

    // first cell line
    EXPECT_EQ(table->getLineType(1), expected_type);
    EXPECT_EQ(table->getLineGraph(1), IG_LINE);
    EXPECT_TRUE(table->isLeftAxis(1));
    // ---- column 0
    EXPECT_EQ(table->getCellType(1, 0), IT_STRING);
    EXPECT_EQ(table->getCellAlign(1, 0), IT_LEFT);
    EXPECT_EQ(table->getCellFont(1, 0), IT_ITALIC);
    EXPECT_EQ(table->getCellContent(1, 0, false), "(divisé par les prix à la consommation)");
    EXPECT_EQ(table->getCellContent(1, 0, true), "\"(divisé par les prix à la consommation)\"");
    // ---- column 1
    EXPECT_EQ(table->getCellType(1, 1), IT_STRING);
    EXPECT_EQ(table->getCellAlign(1, 1), IT_LEFT);
    EXPECT_EQ(table->getCellFont(1, 1), IT_NORMAL);
    EXPECT_EQ(table->getCellContent(1, 1, false), "");

    // second cell line
    EXPECT_EQ(table->getLineType(3), expected_type);
    EXPECT_EQ(table->getLineGraph(3), IG_LINE);
    EXPECT_TRUE(table->isLeftAxis(3));
    // ---- column 0
    EXPECT_EQ(table->getCellType(3, 0), IT_STRING);
    EXPECT_EQ(table->getCellAlign(3, 0), IT_LEFT);
    EXPECT_EQ(table->getCellFont(3, 0), IT_NORMAL);
    EXPECT_EQ(table->getCellContent(3, 0, false), " ");
    // ---- column 1
    EXPECT_EQ(table->getCellType(3, 1), IT_STRING);
    EXPECT_EQ(table->getCellAlign(3, 1), IT_CENTER);
    EXPECT_EQ(table->getCellFont(3, 1), IT_NORMAL);
    EXPECT_EQ(table->getCellContent(3, 1, false), "#s");

    // third cell line
    EXPECT_EQ(table->getLineType(5), expected_type);
    EXPECT_EQ(table->getLineGraph(5), IG_LINE);
    EXPECT_TRUE(table->isLeftAxis(5));
    // ---- column 0
    EXPECT_EQ(table->getCellType(5, 0), IT_STRING);
    EXPECT_EQ(table->getCellAlign(5, 0), IT_LEFT);
    EXPECT_EQ(table->getCellFont(5, 0), IT_BOLD);
    EXPECT_EQ(table->getCellContent(5, 0, false), "Recettes courantes");
    // ---- column 1
    EXPECT_EQ(table->getCellType(5, 1), IT_LEC);
    EXPECT_EQ(table->getCellAlign(5, 1), IT_DECIMAL);
    EXPECT_EQ(table->getCellFont(5, 1), IT_NORMAL);
    EXPECT_EQ(table->getCellContent(5, 1, false), "GOSG+YDTG+IT+YSSG+COTRES+OCUG+RIDG");

    // fifth cell line
    EXPECT_EQ(table->getLineType(7), expected_type);
    EXPECT_EQ(table->getLineGraph(7), IG_LINE);
    EXPECT_TRUE(table->isLeftAxis(7));
    // ---- column 0
    EXPECT_EQ(table->getCellType(7, 0), IT_STRING);
    EXPECT_EQ(table->getCellAlign(7, 0), IT_LEFT);
    EXPECT_EQ(table->getCellFont(7, 0), IT_NORMAL);
    EXPECT_EQ(table->getCellContent(7, 0, false), "       1. Excédent brut d'exploitation");
    // ---- column 1
    EXPECT_EQ(table->getCellType(7, 1), IT_LEC);
    EXPECT_EQ(table->getCellAlign(7, 1), IT_DECIMAL);
    EXPECT_EQ(table->getCellFont(7, 1), IT_NORMAL);
    EXPECT_EQ(table->getCellContent(7, 1, false), "GOSG");

    // is bold, italic, underline ?
    table->setCellAttribute(7, 0, (char) IT_LEFT + IT_BOLD + IT_ITALIC);
    EXPECT_TRUE(table->isCellBoldFont(7, 0));
    EXPECT_TRUE(table->isCellItalicFont(7, 0));
    EXPECT_FALSE(table->isCellUnderlineFont(7, 0));

    // add - delete - insert
    int new_pos;
    std::string cell1_content = table->getCellContent(5, 0, false);      // string cell
    std::string cell2_content = table->getCellContent(5, 1, false);     // LEC cell

    // ---- add
    int nb_lines = table->nbLines();
    new_pos = table->addLineWithCells();
    EXPECT_EQ(table->nbLines(), nb_lines + 1);
    EXPECT_EQ(new_pos, table->nbLines() - 1);

    table->setCellText(new_pos, 0, cell1_content);
    EXPECT_EQ(table->getCellType(new_pos, 0), IT_STRING);
    EXPECT_EQ(table->getCellContent(new_pos, 0, true), "\"" + cell1_content + "\"");

    table->setCellLec(new_pos, 1, cell2_content);
    EXPECT_EQ(table->getCellType(new_pos, 1), IT_LEC);
    EXPECT_EQ(table->getCellContent(new_pos, 1, false), cell2_content);

    // ---- delete
    table->deleteLine(new_pos);
    EXPECT_EQ(table->nbLines(), nb_lines);

    // ---- insert 
    // WARNING: When inserting a new line of type IT_CELL, the attribute TCELL::tc_type of cells is undefined!
    //          See function `T_create_cell()` in file `k_tbl.c` from the C API 
    new_pos = table->insertLineWithCells(25, true);
    EXPECT_EQ(table->nbLines(), nb_lines + 1);
    EXPECT_EQ(new_pos, 26);

    // Rule: If the content starts with a double quotes, we assume it is a string cell. 
    //       Otherwise, it is a LEC cell.
    table->setCellContent(new_pos, 0, "\"" + cell1_content + "\"");
    EXPECT_EQ(table->getCellType(new_pos, 0), IT_STRING);
    EXPECT_EQ(table->getCellContent(new_pos, 0, true), "\"" + cell1_content + "\"");

    table->setCellType(new_pos, 0, IT_LEC);
    // setCellText() forces the cell to become a string cell
    table->setCellText(new_pos, 0, cell1_content);
    EXPECT_EQ(table->getCellType(new_pos, 0), IT_STRING);
    EXPECT_EQ(table->getCellContent(new_pos, 0, false),  cell1_content);

    // setCellLec forces the cell to become a LEC cell
    table->setCellType(new_pos, 0, IT_STRING);
    table->setCellLec(new_pos, 1, cell2_content);
    EXPECT_EQ(table->getCellType(new_pos, 1), IT_LEC);
    EXPECT_EQ(table->getCellContent(new_pos, 1, false), cell2_content);
}

TEST_F(TablesTest, LineSeparator)
{
    int new_pos;
    std::vector<int> line_positions = {2, 4, 26, 30};

    for (const int row: line_positions) EXPECT_EQ(table->getLineType(row), IT_LINE);

    // add
    int nb_lines = table->nbLines();
    new_pos = table->addLineSeparator();
    EXPECT_EQ(table->nbLines(), nb_lines + 1);
    EXPECT_EQ(new_pos, table->nbLines() - 1);
    EXPECT_EQ(table->getLineType(new_pos), IT_LINE);

    // delete
    table->deleteLine(new_pos);
    EXPECT_EQ(table->nbLines(), nb_lines);

    // insert 
    new_pos = table->insertLineSeparator(25, true);
    EXPECT_EQ(table->nbLines(), nb_lines + 1);
    EXPECT_EQ(new_pos, 26);
    EXPECT_EQ(table->getLineType(new_pos), IT_LINE);
}

TEST_F(TablesTest, LineMode)
{
    int new_pos;

    EXPECT_EQ(table->getLineType(27), IT_MODE);

    // add
    int nb_lines = table->nbLines();
    new_pos = table->addLineMode();
    EXPECT_EQ(table->nbLines(), nb_lines + 1);
    EXPECT_EQ(new_pos, table->nbLines() - 1);
    EXPECT_EQ(table->getLineType(new_pos), IT_MODE);

    // delete
    table->deleteLine(new_pos);
    EXPECT_EQ(table->nbLines(), nb_lines);

    // insert 
    new_pos = table->insertLineMode(25, true);
    EXPECT_EQ(table->nbLines(), nb_lines + 1);
    EXPECT_EQ(new_pos, 26);
    EXPECT_EQ(table->getLineType(new_pos), IT_MODE);
}

TEST_F(TablesTest, LineFiles)
{
    int new_pos;

    EXPECT_EQ(table->getLineType(28), IT_FILES);

    // add
    int nb_lines = table->nbLines();
    new_pos = table->addLineFiles();
    EXPECT_EQ(table->nbLines(), nb_lines + 1);
    EXPECT_EQ(new_pos, table->nbLines() - 1);
    EXPECT_EQ(table->getLineType(new_pos), IT_FILES);

    // delete
    table->deleteLine(new_pos);
    EXPECT_EQ(table->nbLines(), nb_lines);

    // insert 
    new_pos = table->insertLineFiles(25, true);
    EXPECT_EQ(table->nbLines(), nb_lines + 1);
    EXPECT_EQ(new_pos, 26);
    EXPECT_EQ(table->getLineType(new_pos), IT_FILES);
}

TEST_F(TablesTest, LineDate)
{
    int new_pos;

    EXPECT_EQ(table->getLineType(29), IT_DATE);

    // add
    int nb_lines = table->nbLines();
    new_pos = table->addLineDate();
    EXPECT_EQ(table->nbLines(), nb_lines + 1);
    EXPECT_EQ(new_pos, table->nbLines() - 1);
    EXPECT_EQ(table->getLineType(new_pos), IT_DATE);

    // delete
    table->deleteLine(new_pos);
    EXPECT_EQ(table->nbLines(), nb_lines);

    // insert 
    new_pos = table->insertLineDate(25, true);
    EXPECT_EQ(table->nbLines(), nb_lines + 1);
    EXPECT_EQ(new_pos, 26);
    EXPECT_EQ(table->getLineType(new_pos), IT_DATE);
}

TEST_F(TablesTest, Hash)
{
    boost::hash<TBL> table_hasher;
    std::size_t hash_before;
    std::size_t hash_after;

    hash_before = table_hasher(*table->c_table);

    // same table
    Table* same_table = new Table("GFRPC");
    EXPECT_EQ(*table, *same_table);
    hash_after = table_hasher(*same_table->c_table);
    EXPECT_EQ(hash_before, hash_after);

    // different title
    std::string new_title = "New Title";
    table->setTitle(0, new_title);
    hash_after = table_hasher(*table->c_table);
    EXPECT_NE(hash_before, hash_after);

    // different cell
    hash_before = hash_after;
    table->setCellText(5, 0, "Nouvelles recettes");
    hash_after = table_hasher(*table->c_table);
    EXPECT_NE(hash_before, hash_after);

    hash_before = hash_after;
    table->setCellLec(5, 1, "GOSG+YDTG+IT+YSSG+COTRES+OCUG");
    hash_after = table_hasher(*table->c_table);
    EXPECT_NE(hash_before, hash_after);
}
