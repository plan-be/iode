#include "pch.h"


class TablesTest : public KDBTest, public ::testing::Test
{
protected:
    Table* table;
    KDBTables kdb;

    void SetUp() override
    {
        kdb.load(input_test_dir + "fun.tbl");

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
    std::string expected_title = "Compte de l'ensemble des administrations publiques";

    EXPECT_EQ(table->getLineType(0), IT_TITLE);
    std::string title = table->getTitle(0);
    EXPECT_EQ(title, expected_title + " ");

    std::string new_title = u8"New Title with special characters !@ùµ£éè";
    table->setTitle(0, new_title);
    title = table->getTitle(0);
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
    EXPECT_EQ(table->getCellContent(1, 0, false), u8"(divisé par les prix à la consommation)");
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
    EXPECT_EQ(table->getCellContent(3, 1, false), u8"#s");

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
    EXPECT_EQ(table->getCellContent(7, 0, false), u8"       1. Excédent brut d'exploitation");
    // ---- column 1
    EXPECT_EQ(table->getCellType(7, 1), IT_LEC);
    EXPECT_EQ(table->getCellAlign(7, 1), IT_DECIMAL);
    EXPECT_EQ(table->getCellFont(7, 1), IT_NORMAL);
    EXPECT_EQ(table->getCellContent(7, 1, false), "GOSG");
}

TEST_F(TablesTest, LineSeparator)
{
    std::vector<int> line_positions = {2, 4, 26, 30};

    for (const int row: line_positions) EXPECT_EQ(table->getLineType(row), IT_LINE);
}

TEST_F(TablesTest, LineMode)
{
    EXPECT_EQ(table->getLineType(27), IT_MODE);
}

TEST_F(TablesTest, LineFiles)
{
    EXPECT_EQ(table->getLineType(28), IT_FILES);
}

TEST_F(TablesTest, LineDate)
{
    EXPECT_EQ(table->getLineType(29), IT_DATE);
}
