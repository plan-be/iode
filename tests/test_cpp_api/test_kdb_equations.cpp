#include "pch.h"


class KDBEquationsTest : public KDBTest, public ::testing::Test
{
protected:
    KDBEquations kdb;

    void SetUp() override
    {
        kdb.load(input_test_dir + "fun.eqs");
    }

    // void TearDown() override {}
};


TEST_F(KDBEquationsTest, Load)
{
    KDBEquations kdb;
    kdb.load(input_test_dir + "fun.eqs");
    EXPECT_EQ(kdb.count(), 274);
}

TEST_F(KDBEquationsTest, Save)
{
    // save in binary format
    kdb.save(output_test_dir + "fun.eqs");

    // save in ascii format
    kdb.save(output_test_dir + "fun.ae");
}

TEST_F(KDBEquationsTest, GetLec)
{
    int pos = 0;
    std::string name = kdb.get_name(pos);
    std::string expected_lec = "(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)";

    // by pos
    EXPECT_EQ(kdb.get_lec(pos), expected_lec);

    // by name
    EXPECT_EQ(kdb.get_lec(name), expected_lec);
}

TEST_F(KDBEquationsTest, Get)
{
    int pos = 0;
    std::string name = kdb.get_name(pos);

    // by pos
    Equation eq1 = kdb.get(pos);

    // by name
    Equation eq2 = kdb.get(name);
}

TEST_F(KDBEquationsTest, CreateRemove)
{
    std::string name = "ACAF";
    kdb.remove(name);
    EXPECT_THROW(kdb.get(name), std::runtime_error);

    std::string lec = "(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+\nacaf4*(TIME=1995)";
    std::string method = "LSQ";
    Sample sample("1980Y1", "1996Y1");
    std::string comment = "Equation comment";
    std::string block = "ACAF";
    std::string instruments = "Equation instruments";
    std::array<float, EQS_NBTESTS> tests = { 1, 0.0042699, 0.00818467, 5.19945e-05, 0.0019271461, 23.545813, 32.2732, 0.82176137, 0.79629868, 2.3293459, 83.8075 };
    bool date = true;

    kdb.add(name, lec, comment, method, sample, instruments, block, tests, date);
}

TEST_F(KDBEquationsTest, Update)
{
    std::string name = "ACAF";
    std::string new_lec = "(ACAF/VAF[-1]) :=acaf2*GOSF[-1]+\nacaf4*(TIME=1995)";

    Equation eq = kdb.get(name);
    eq.set_lec(new_lec, name);
    kdb.update(name, eq);
    EXPECT_EQ(kdb.get_lec(name), new_lec);
}

TEST_F(KDBEquationsTest, Copy)
{
    std::string name = "ACAF";
    Equation eq = kdb.get(name);

    Equation copy_eq = kdb.copy(name);
    EXPECT_EQ(copy_eq, eq);
}
