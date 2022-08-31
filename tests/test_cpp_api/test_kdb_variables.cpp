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
        load_global_kdb(I_VARIABLES, input_test_dir + "fun.var");
    }

    // void TearDown() override {}
};


TEST_F(KDBVariablesTest, Load)
{
    KDBVariables kdb2;
    EXPECT_EQ(kdb2.count(), 394);
}

TEST_F(KDBVariablesTest, Save)
{
    // save in binary format
    save_global_kdb(I_VARIABLES, output_test_dir + "fun.var");
    kdb.dump(output_test_dir + "fun.var");

    // save in ascii format
    save_global_kdb(I_VARIABLES, output_test_dir + "fun.av");
    kdb.dump(output_test_dir + "fun.av");
}

TEST_F(KDBVariablesTest, GetSetVar)
{
    IODE_REAL value;
    IODE_REAL new_value;
    int nb_periods = kdb.get_nb_periods();
    std::string name = kdb.get_name(pos + 1);
    Period start = kdb.get_sample().start_period();

    // period as int
    value = 1.2130001;
    EXPECT_DOUBLE_EQ(kdb.get_var(pos, t), value);
    new_value = 10.0;
    kdb.set_var(pos, t, new_value);
    EXPECT_DOUBLE_EQ(kdb.get_var(pos, t), new_value);

    value = -11.028999;
    EXPECT_DOUBLE_EQ(kdb.get_var(name, t), value);
    new_value = 20.0;
    kdb.set_var(name, t, new_value);
    EXPECT_DOUBLE_EQ(kdb.get_var(name, t), new_value);

    // period as Period object
    Period period = start.shift(t + 1);

    value = 5.2020001;
    EXPECT_DOUBLE_EQ(kdb.get_var(pos, period), value);
    new_value = 30.0;
    kdb.set_var(pos, period, new_value);
    EXPECT_DOUBLE_EQ(kdb.get_var(pos, period), new_value);

    value = -15.847;
    EXPECT_DOUBLE_EQ(kdb.get_var(name, period), value);
    new_value = 40.0;
    kdb.set_var(name, period, new_value);
    EXPECT_DOUBLE_EQ(kdb.get_var(name, period), new_value);

    // period as string
    std::string s_period = start.shift(t + 2).to_string();

    value = 9.184;
    EXPECT_DOUBLE_EQ(kdb.get_var(pos, s_period), value);
    new_value = 50.0;
    kdb.set_var(pos, s_period, new_value);
    EXPECT_DOUBLE_EQ(kdb.get_var(pos, s_period), new_value);

    value = -19.288002;
    EXPECT_DOUBLE_EQ(kdb.get_var(name, s_period), value);
    new_value = 60.0;
    kdb.set_var(name, s_period, new_value);
    EXPECT_DOUBLE_EQ(kdb.get_var(name, s_period), new_value);
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

    // pass a vector with values
    Variable new_var;
    new_var.reserve(nb_periods);
    for (int p = 0; p < nb_periods; p++) new_var.push_back(10. + p);

    kdb.add(new_name, new_var);
    EXPECT_EQ(kdb.get(new_name), new_var);

    // --- already existing name
    std::fill(new_var.begin(), new_var.end(), 1);
    EXPECT_THROW(kdb.add(new_name, new_var), IodeExceptionInitialization);

    kdb.remove(new_name);
    EXPECT_THROW(kdb.get(new_name), IodeExceptionFunction);

    // pass a LEC expression
    for (int p = 0; p < nb_periods; p++) new_var[p] = 10. + p;
    kdb.add(new_name, "10 + t");
    EXPECT_EQ(kdb.get(new_name), new_var);
    kdb.remove(new_name);
    
    new_var[0] = L_NAN;
    for (int p = 1; p < nb_periods; p++) new_var[p] = 1. / p;
    kdb.add(new_name, "1 / t");
    EXPECT_EQ(kdb.get(new_name), new_var);
    kdb.remove(new_name);

    std::fill(new_var.begin(), new_var.end(), L_NAN);
    kdb.add(new_name, "");
    EXPECT_EQ(kdb.get(new_name), new_var);
    kdb.remove(new_name);

    // --- using function that does not exist
    EXPECT_THROW(kdb.add("FAILS", "func(t)"), IodeExceptionFunction);
    // --- using variable taht does not exist
    EXPECT_THROW(kdb.add("FAILS", "ln Z"), IodeExceptionFunction);
}

TEST_F(KDBVariablesTest, Update)
{
    std::string name = kdb.get_name(pos);
    int nb_periods = kdb.get_nb_periods();
    std::string lec;
    Variable updated_var;
    updated_var.reserve(nb_periods);

    // by position
    // pass a vector with values
    for (int p = 0; p < nb_periods; p++) updated_var.push_back(10.0 + p);
    kdb.update(pos, updated_var);
    EXPECT_EQ(kdb.get(name), updated_var);
    // pass a LEC expression
    lec = "10 + t";
    kdb.update(pos, lec);
    EXPECT_EQ(kdb.get(name), updated_var);

    // by name
    // pass a vector with values
    for (int p = 0; p < nb_periods; p++) updated_var[p] = 20.0 + p;
    kdb.update(name, updated_var);
    EXPECT_EQ(kdb.get(name), updated_var);
    // pass a LEC expression
    lec = "20 + t";
    kdb.update(name, lec);
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

TEST_F(KDBVariablesTest, Filter)
{
    std::string pattern = "A*;*_";
    std::vector<std::string> expected_names;
    KDBVariables* local_kdb;
    KDBVariables global_kdb;

    std::vector<std::string> all_names;
    for (int p = 0; p < global_kdb.count(); p++) all_names.push_back(global_kdb.get_name(p));

    int nb_total_comments = global_kdb.count();
    // A*
    for (const std::string& name : all_names) if (name.front() == 'A') expected_names.push_back(name);
    // *_
    for (const std::string& name : all_names) if (name.back() == '_') expected_names.push_back(name);

    // create local kdb
    local_kdb = new KDBVariables(pattern);
    EXPECT_EQ(local_kdb->count(), expected_names.size());

    // modify an element of the local KDB and check if the 
    // corresponding element of the global KDB also changes
    std::string name = "ACAF";
    int nb_periods = kdb.get_nb_periods();
    std::string lec = "10 + t";
    Variable updated_var;
    updated_var.reserve(nb_periods);
    for (int p = 0; p < nb_periods; p++) updated_var.push_back(10. + p);
    local_kdb->update(name, lec);
    EXPECT_EQ(local_kdb->get(name), updated_var);
    EXPECT_EQ(global_kdb.get(name), updated_var);

    // add an element to the local KDB and check if it has also 
    // been added to the global KDB
    std::string new_name = "NEW_VARIABLE";
    Variable new_var;
    new_var.reserve(nb_periods);
    for (int p = 0; p < nb_periods; p++) new_var.push_back(10. + p);
    local_kdb->add(new_name, new_var);
    EXPECT_EQ(local_kdb->get(new_name), new_var);
    EXPECT_EQ(global_kdb.get(new_name), new_var);

    // rename an element in the local KDB and check if the 
    // corresponding element has also been renamed in the global KDB
    std::string old_name = new_name;
    new_name = "VARIABLE_NEW";
    local_kdb->rename(old_name, new_name);
    EXPECT_EQ(local_kdb->get(new_name), new_var);
    EXPECT_EQ(global_kdb.get(new_name), new_var);

    // delete an element from the local KDB and check if it has also 
    // been deleted from the global KDB
    local_kdb->remove(new_name);
    EXPECT_FALSE(local_kdb->contains(new_name));
    EXPECT_FALSE(global_kdb.contains(new_name));

    // delete local kdb
    delete local_kdb;
    EXPECT_EQ(global_kdb.count(), nb_total_comments);
    EXPECT_EQ(global_kdb.get(name), updated_var);
}
