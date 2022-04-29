#include "pch.h"


class KDBVariablesTest : public KDBTest, public ::testing::Test
{
protected:
    KDBVariables kdb;
    int mode = 0;
    int pos = 0;
    int t = 10;
    int t_nan = 5;

    void SetUp() override
    {
        kdb.load(input_test_dir + "fun.var");
    }

    // void TearDown() override {}
};


TEST_F(KDBVariablesTest, Load)
{
    KDBVariables kdb;
    kdb.load(input_test_dir + "fun.var");
    EXPECT_EQ(kdb.count(), 394);
}

TEST_F(KDBVariablesTest, Save)
{
    // save in binary format
    kdb.save(output_test_dir + "fun.var");

    // save in ascii format
    kdb.save(output_test_dir + "fun.av");
}

TEST_F(KDBVariablesTest, GetValue)
{
    IODE_REAL value;
    IODE_REAL expected_value = 1.2130001;

    // by position
    value = kdb.get_var(pos, t, mode);
    EXPECT_DOUBLE_EQ(expected_value, value);    
    value = kdb.get_var(pos, t_nan, mode);
    EXPECT_FALSE(L_ISAN(value));

    // by name
    std::string name = kdb.get_name(pos);
    value = kdb.get_var(name, t, mode);
    EXPECT_DOUBLE_EQ(expected_value, value);    
    value = kdb.get_var(name, t_nan, mode);
    EXPECT_FALSE(L_ISAN(value));
}

TEST_F(KDBVariablesTest, GetSample)
{
    Sample sample = kdb.get_sample();
    Sample expected_sample("1960Y1", "2015Y1");
    EXPECT_EQ(sample.to_string(), expected_sample.to_string());
}

TEST_F(KDBVariablesTest, GetNbPeriods)
{
    int expected_nb_periods = 56;
    EXPECT_EQ(kdb.get_nb_periods(), expected_nb_periods);
}

TEST_F(KDBVariablesTest, GetPeriod)
{
    Period expected_period(1970, 'Y', 1);
    EXPECT_EQ(kdb.get_period(t), expected_period);
}

TEST_F(KDBVariablesTest, Get)
{
    int nb_periods = kdb.get_nb_periods();
    std::string name = kdb.get_name(pos);
    Variable var;
    Variable expected_var;
    
    expected_var.reserve(nb_periods);
    for (int p = 0; p < nb_periods; p++)
        expected_var.push_back(kdb.get_var(name, p, mode));

    // by position
    var = kdb.get(pos);
    EXPECT_EQ(var, expected_var);

    // by name
    var = kdb.get(name);
    EXPECT_EQ(var, expected_var);
}
TEST_F(KDBVariablesTest, CreateRemove)
{
    std::string new_name = "NEW_VAR";
    int nb_periods = kdb.get_nb_periods();
    Variable new_var;

    new_var.reserve(nb_periods);
    for (int p = 0; p < nb_periods; p++)
        new_var.push_back(1.0);

    kdb.add(new_name, new_var);
    EXPECT_EQ(kdb.get(new_name), new_var);

    kdb.remove(new_name);
    EXPECT_THROW(kdb.get(new_name), std::runtime_error);
}

TEST_F(KDBVariablesTest, Update)
{
    std::string name = kdb.get_name(pos);
    int nb_periods = kdb.get_nb_periods();
    Variable updated_var;
    updated_var.reserve(nb_periods);

    // by position
    for (int p = 0; p < nb_periods; p++) updated_var.push_back(1.0);
    kdb.update(pos, updated_var);
    EXPECT_EQ(kdb.get(name), updated_var);

    // by name
    for (int p = 0; p < nb_periods; p++) updated_var[p] = 0.0;
    kdb.update(name, updated_var);
    EXPECT_EQ(kdb.get(name), updated_var);
}

TEST_F(KDBVariablesTest, Copy)
{
    std::string name = kdb.get_name(pos);
    Variable var;
    Variable copy_var;

    // by position
    var = kdb.get(pos);
    copy_var = kdb.copy(pos);
    EXPECT_EQ(copy_var, var);

    // by name
    var = kdb.get(name);
    copy_var = kdb.copy(name);
    EXPECT_EQ(copy_var, var);
}
