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
    std::string name = kdb.get_name(pos);
    std::string lec;
    std::string expected_lec = "((WCRH/QL)/(WCRH/QL)[1990Y1])*(VAFF/(VM+VAFF))[-1]+PM*(VM/(VM+VAFF))[-1]";

    // by position
    lec = kdb.get_lec(pos);
    EXPECT_EQ(lec, expected_lec);

    // by name
    lec = kdb.get_lec(name);
    EXPECT_EQ(lec, expected_lec);
}

TEST_F(KDBIdentitiesTest, Get)
{
    int pos = 0;
    std::string name = kdb.get_name(pos);
    CLEC* expected_clec = KICLEC(K_WS[I_IDENTITIES], pos);
    std::string expected_lec = "((WCRH/QL)/(WCRH/QL)[1990Y1])*(VAFF/(VM+VAFF))[-1]+PM*(VM/(VM+VAFF))[-1]";

    // by position
    Identity identity_pos = kdb.get(pos);
    EXPECT_EQ(identity_pos.lec, expected_lec);
    EXPECT_EQ(identity_pos.clec, expected_clec);

    // by name
    Identity identity_name = kdb.get(name);
    EXPECT_EQ(identity_name.lec, expected_lec);
    EXPECT_EQ(identity_name.clec, expected_clec);
}

TEST_F(KDBIdentitiesTest, CreateRemove)
{
    std::string name = kdb.get_name(0);
    std::string lec = "((WCRH/QL)/(WCRH/QL)[1990Y1])*(VAFF/(VM+VAFF))[-1]+PM*(VM/(VM+VAFF))[-1]";

    kdb.remove(name);
    EXPECT_THROW(kdb.get(name), std::runtime_error);

    kdb.add(name, lec);
    EXPECT_EQ(kdb.get_lec(name), lec);
}

TEST_F(KDBIdentitiesTest, Update)
{
    std::string name = kdb.get_name(0);
    std::string new_lec = "((WCRH/QL)/(WCRH/QL)[1990Y1])*(VAFF/(VM+VAFF))[-2]+PM*(VM/(VM+VAFF))[-2]";

    kdb.update(name, new_lec);
    EXPECT_EQ(kdb.get_lec(name), new_lec);
}

TEST_F(KDBIdentitiesTest, Copy)
{
    std::string name = kdb.get_name(0);

    Identity identity_copy = kdb.copy(name);

    EXPECT_EQ(std::string(identity_copy.lec), kdb.get_lec(name));
}
