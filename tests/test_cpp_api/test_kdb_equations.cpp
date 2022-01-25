#include "pch.h"
#include "../../cpp_api/iode_cpp_api.h"

#include <filesystem>


// To get current path
// std::filesystem::path cwd = std::filesystem::current_path();
// std::string str_path = cwd.string();

std::string input_test_dir = "../../tests/data/";
std::string output_test_dir = "../../tests/output/";


class KDBEquationsTest : public ::testing::Test {
protected:
    KDBEquations kdb;

    void SetUp() override
    {
        kdb.load(input_test_dir + "fun.eqs");
    }

    // void TearDown() override {}
};


TEST(KDBEquationsTest_, Load)
{
    KDBEquations kdb;
    kdb.load(input_test_dir + "fun.eqs");
    EXPECT_EQ(kdb.count(), 274);
}

TEST_F(KDBEquationsTest, GET)
{

}

// -- lec --

TEST_F(KDBEquationsTest, GetLec)
{
    int pos = 0;
    std::string name = kdb.getName(pos);

    std::string lec;
    std::string expected_lec = "(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)";

    // by position
    lec = kdb.getLec(pos);
    EXPECT_EQ(lec, expected_lec);

    // by name
    lec = kdb.getLec(name);
    EXPECT_EQ(lec, expected_lec);
}

TEST_F(KDBEquationsTest, SetLec)
{
    std::string lec;
    std::string new_lec;

    // by position
    // original lec was: (ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995) -> removed acaf1
    new_lec = "(ACAF/VAF[-1]) :=acaf2*GOSF[-1]+\nacaf4*(TIME=1995)";
    kdb.setLec(0, new_lec);
    lec = kdb.getLec(0);
    EXPECT_EQ(lec, new_lec);

    // by name
    // original lec was: ACAG := ACAG[-1]+r VBBP[-1]+(0.006*VBBP[-1]*(TIME=2001)-0.008*(TIME=2008)) -> replaced 0.006 by 0.01
    new_lec = "ACAG := ACAG[-1]+r VBBP[-1]+(0.01*VBBP[-1]*(TIME=2001)-0.008*(TIME=2008))";
    std::string name = kdb.getName(1);
    kdb.setLec(name, new_lec);
    lec = kdb.getLec(name);
    EXPECT_EQ(lec, new_lec);
}

// -- method --

TEST_F(KDBEquationsTest, GetMethod)
{
    int pos = 0;
    std::string name = kdb.getName(pos);

    std::string method;
    std::string expected_method = vEquationMethods[0];

    // by position
    method = kdb.getMethod(pos);
    EXPECT_EQ(method, expected_method);

    // by name
    method = kdb.getMethod(name);
    EXPECT_EQ(method, expected_method);
}

TEST_F(KDBEquationsTest, SetMethod)
{
    int pos = 0;
    std::string name = kdb.getName(1);

    std::string res_method;

    // by position
    for (const std::string& method : vEquationMethods)
    {
        kdb.setMethod(pos, method);
        res_method = kdb.getMethod(pos);
        EXPECT_EQ(res_method, method);
    }

    // by name
    for (const std::string& method : vEquationMethods)
    {
        kdb.setMethod(name, method);
        res_method = kdb.getMethod(name);
        EXPECT_EQ(res_method, method);
    }
}

// -- block --

TEST_F(KDBEquationsTest, GetBlock)
{
    int pos = 0;
    std::string name = kdb.getName(pos);

    std::string block;
    std::string expected_block = "ACAF";

    // by position
    block = kdb.getBlock(pos);
    EXPECT_EQ(block, expected_block);

    // by name
    block = kdb.getBlock(name);
    EXPECT_EQ(block, expected_block);
}

TEST_F(KDBEquationsTest, SetBlock)
{
    int pos = 0;
    std::string name = kdb.getName(1);

    std::string block;
    std::string new_block = "ACAF;AGAF";

    // by position
    kdb.setBlock(pos, new_block);
    block = kdb.getBlock(pos);
    EXPECT_EQ(block, new_block);

    // by name
    kdb.setBlock(name, new_block);
    block = kdb.getBlock(name);
    EXPECT_EQ(block, new_block);
}

// -- sample --

TEST_F(KDBEquationsTest, GetSample)
{
    int pos = 0;
    std::string name = kdb.getName(pos);

    Sample expected_sample("1980Y1", "1996Y1");

    // by position
    Sample sampleByPos = kdb.getSample(pos);
    EXPECT_EQ(sampleByPos.to_string(), expected_sample.to_string());

    // by name
    Sample sampleByName = kdb.getSample(name);
    EXPECT_EQ(sampleByName.to_string(), expected_sample.to_string());
}

TEST_F(KDBEquationsTest, SetSample)
{
    int pos = 0;
    std::string name = kdb.getName(1);

    std::string from = "2000Y1";
    std::string to = "2020Y1";
    Sample new_sample(from, to);

    // by position
    kdb.setSample(pos, from, to);
    Sample sampleByPos = kdb.getSample(pos);
    EXPECT_EQ(sampleByPos.to_string(), new_sample.to_string());

    // by name
    kdb.setSample(name, from, to);
    Sample sampleByName = kdb.getSample(name);
    EXPECT_EQ(sampleByName.to_string(), new_sample.to_string());
}

// -- comment --

TEST_F(KDBEquationsTest, GetComment)
{
    int pos = 0;
    std::string name = kdb.getName(pos);

    std::string comment;
    std::string expected_comment = "";

    // by position
    comment = kdb.getComment(pos);
    EXPECT_EQ(comment, expected_comment);

    // by name
    comment = kdb.getComment(name);
    EXPECT_EQ(comment, expected_comment);
}

TEST_F(KDBEquationsTest, SetComment)
{
    int pos = 0;
    std::string name = kdb.getName(1);

    std::string comment;
    std::string new_comment = "New Comment";

    // by position
    kdb.setComment(pos, new_comment);
    comment = kdb.getComment(pos);
    EXPECT_EQ(comment, new_comment);

    // by name
    kdb.setComment(name, new_comment);
    comment = kdb.getComment(name);
    EXPECT_EQ(comment, new_comment);
}

// -- instruments --

TEST_F(KDBEquationsTest, GetInstruments)
{
    int pos = 0;
    std::string name = kdb.getName(pos);

    std::string instruments;
    std::string expected_instruments = "";

    // by position
    instruments = kdb.getInstruments(pos);
    EXPECT_EQ(instruments, expected_instruments);

    // by name
    instruments = kdb.getInstruments(name);
    EXPECT_EQ(instruments, expected_instruments);
}

TEST_F(KDBEquationsTest, SetInstruments)
{
    int pos = 0;
    std::string name = kdb.getName(1);

    std::string instruments;
    // TODO : find a realistic value for new_instruments
    std::string new_instruments = "no_idea_what_to_put_here";

    // by position
    kdb.setInstruments(pos, new_instruments);
    instruments = kdb.getInstruments(pos);
    EXPECT_EQ(instruments, new_instruments);

    // by name
    kdb.setInstruments(name, new_instruments);
    instruments = kdb.getInstruments(name);
    EXPECT_EQ(instruments, new_instruments);
}

// -- date --

TEST_F(KDBEquationsTest, GetDate)
{
    int pos = 0;
    std::string name = kdb.getName(pos);

    int date;
    int expected_date = 19980612;

    // by position
    date = kdb.getDate(pos);
    EXPECT_EQ(date, expected_date);

    // by name
    date = kdb.getDate(name);
    EXPECT_EQ(date, expected_date);
}

TEST_F(KDBEquationsTest, UpdateDate)
{
    int pos = 0;
    std::string name = kdb.getName(1);

    int date;
    int current_date = SCR_current_date();

    // by position
    kdb.updateDate(pos);
    date = kdb.getDate(pos);
    EXPECT_EQ(date, current_date);

    // by name
    kdb.updateDate(name);
    date = kdb.getDate(name);
    EXPECT_EQ(date, current_date);
}

// -- tests --

TEST_F(KDBEquationsTest, GetTests)
{
    int pos = 0;
    std::string name = kdb.getName(pos);

    std::array<float, EQS_NBTESTS> tests;
    std::array<float, EQS_NBTESTS> expected_tests = { 1, 0.0042699, 0.00818467, 5.19945e-05, 0.00192715, 23.5458, 32.2732, 0.821761, 0.796299, 2.32935, 83.8075};

    // by position
    tests = kdb.getTests(pos);
    for (int i = 0; i < EQS_NBTESTS; i++) EXPECT_NEAR(tests[i], expected_tests[i], 1.e-10);

    // by name
    tests = kdb.getTests(name);
    for (int i = 0; i < EQS_NBTESTS; i++) EXPECT_NEAR(tests[i], expected_tests[i], 1.e-10);
}

TEST_F(KDBEquationsTest, SetTests)
{
    int pos = 0;
    std::string name = kdb.getName(1);

    std::array<float, EQS_NBTESTS> tests;
    std::array<float, EQS_NBTESTS> new_tests = { 1., 2., 3., 4., 5., 6., 7., 8., 9., 10., 11.};

    // by position
    kdb.setTests(pos, new_tests);
    tests = kdb.getTests(pos);
    for (int i = 0; i < EQS_NBTESTS; i++) EXPECT_NEAR(tests[i], new_tests[i], 1.e-10);

    // by name
    kdb.setTests(name, new_tests);
    tests = kdb.getTests(name);
    for (int i = 0; i < EQS_NBTESTS; i++) EXPECT_NEAR(tests[i], new_tests[i], 1.e-10);
}
