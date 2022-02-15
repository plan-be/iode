#include "pch.h"


class KDBIdentitiesTest : public KDBTest, public ::testing::Test
{
protected:
    KDBIdentities kdb;

    void SetUp() override
    {
        kdb.load(input_test_dir + "fun.idt");
    }

    // void TearDown() override {}
};


TEST_F(KDBIdentitiesTest, Load)
{
    KDBIdentities kdb;
    kdb.load(input_test_dir + "fun.idt");
    EXPECT_EQ(kdb.count(), 48);
}

TEST_F(KDBIdentitiesTest, Save)
{
    // save in binary format
    kdb.save(output_test_dir + "fun.idt");

    // save in ascii format
    kdb.save(output_test_dir + "fun.ai");
}

TEST_F(KDBIdentitiesTest, GetLec)
{
    int pos = 0;
    std::string lec;
    std::string expected_lec = "((WCRH/QL)/(WCRH/QL)[1990Y1])*(VAFF/(VM+VAFF))[-1]+PM*(VM/(VM+VAFF))[-1]";

    // by position
    lec = kdb.getLec(pos);
    EXPECT_EQ(expected_lec, lec);

    // by name
    std::string name = kdb.getName(pos);
    lec = kdb.getLec(name);
    EXPECT_EQ(expected_lec, lec);
}
