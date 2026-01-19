#include "pch.h"


class KDBVariablesTest : public TestAbstract, public ::testing::Test
{
protected:
    IodeVarMode mode = VAR_MODE_LEVEL;
    int pos = 0;
    int t = 10;
    int t_nan = 5;

    void SetUp() override
    {
        global_ws_var->load(str_input_test_dir + "fun.av");
    }

    // void TearDown() override {}
};

class KDBVariablesEmptyTest : public TestAbstract, public ::testing::Test
{
};


TEST_F(KDBVariablesTest, Load)
{
    KDBVariables kdb(false);
    kdb.load(str_input_test_dir + prefix_filename + "fun.var");
    EXPECT_EQ(kdb.size(), 394);
}

TEST_F(KDBVariablesTest, Subset)
{
    std::string pattern = "A*";
    Variable var = global_ws_var->get("ACAF");
    std::string lec = "10 + t";
    Variable new_var;
    new_var.reserve(var.size());
    for (int p = 0; p < var.size(); p++) new_var.push_back(10.0 + p);

    // GLOBAL KDB
    EXPECT_EQ(global_ws_var->size(), 394);
    EXPECT_TRUE(global_ws_var->is_global_database());
    std::set<std::string> names = global_ws_var->filter_names(pattern);

    // DEEP COPY SUBSET
    KDBVariables* kdb_subset_deep_copy = new KDBVariables(global_ws_var.get(), pattern, true);
    EXPECT_EQ(kdb_subset_deep_copy->size(), names.size());
    EXPECT_TRUE(kdb_subset_deep_copy->is_local_database());
    kdb_subset_deep_copy->update("ACAF", lec);
    EXPECT_EQ(global_ws_var->get("ACAF"), var);
    EXPECT_EQ(kdb_subset_deep_copy->get("ACAF"), new_var);

    // SHALLOW COPY SUBSET
    KDBVariables* kdb_subset_shallow_copy = new KDBVariables(global_ws_var.get(), pattern, false);
    EXPECT_EQ(kdb_subset_shallow_copy->size(), names.size());
    EXPECT_TRUE(kdb_subset_shallow_copy->is_shallow_copy_database());
    kdb_subset_shallow_copy->update("ACAF", lec);
    EXPECT_EQ(global_ws_var->get("ACAF"), new_var);
    EXPECT_EQ(kdb_subset_shallow_copy->get("ACAF"), new_var);
}

TEST_F(KDBVariablesTest, Save)
{
    // save in binary format
    global_ws_var->save(str_output_test_dir + "fun.var");

    // save in ascii format
    global_ws_var->save(str_output_test_dir + "fun.av");
}

TEST_F(KDBVariablesTest, GetSetVar)
{
    double value;
    double new_value;
    int nb_periods = global_ws_var->get_nb_periods();
    std::string name = global_ws_var->get_name(pos);
    std::string next_var = global_ws_var->get_name(pos + 1);
    Period start = global_ws_var->get_sample()->start_period;

    // period as int
    value = 1.2130001;
    EXPECT_DOUBLE_EQ(global_ws_var->get_var(name, t), value);
    new_value = 10.0;
    global_ws_var->set_var(name, t, new_value);
    EXPECT_DOUBLE_EQ(global_ws_var->get_var(name, t), new_value);

    value = -11.028999;
    EXPECT_DOUBLE_EQ(global_ws_var->get_var(next_var, t), value);
    new_value = 20.0;
    global_ws_var->set_var(next_var, t, new_value);
    EXPECT_DOUBLE_EQ(global_ws_var->get_var(next_var, t), new_value);

    // period as Period object
    Period period = start.shift(t + 1);

    value = 5.2020001;
    EXPECT_DOUBLE_EQ(global_ws_var->get_var(name, period), value);
    new_value = 30.0;
    global_ws_var->set_var(name, period, new_value);
    EXPECT_DOUBLE_EQ(global_ws_var->get_var(name, period), new_value);

    value = -15.847;
    EXPECT_DOUBLE_EQ(global_ws_var->get_var(next_var, period), value);
    new_value = 40.0;
    global_ws_var->set_var(next_var, period, new_value);
    EXPECT_DOUBLE_EQ(global_ws_var->get_var(next_var, period), new_value);

    // period as string
    std::string s_period = start.shift(t + 2).to_string();

    value = 9.184;
    EXPECT_DOUBLE_EQ(global_ws_var->get_var(name, s_period), value);
    new_value = 50.0;
    global_ws_var->set_var(name, s_period, new_value);
    EXPECT_DOUBLE_EQ(global_ws_var->get_var(name, s_period), new_value);

    value = -19.288002;
    EXPECT_DOUBLE_EQ(global_ws_var->get_var(next_var, s_period), value);
    new_value = 60.0;
    global_ws_var->set_var(next_var, s_period, new_value);
    EXPECT_DOUBLE_EQ(global_ws_var->get_var(next_var, s_period), new_value);

    // get pointers to a variable's data
    double* data = global_ws_var->get_var_ptr(name);
    EXPECT_TRUE(data != NULL);
    value = global_ws_var->get_var(name, 0);
    EXPECT_DOUBLE_EQ(*data, value);
    EXPECT_DOUBLE_EQ(data[0], value);
    value = global_ws_var->get_var(name, t);
    EXPECT_DOUBLE_EQ(*(data + t), value);
    EXPECT_DOUBLE_EQ(data[t], value);
}

TEST_F(KDBVariablesEmptyTest, Sample)
{
    Sample* sample_undef = global_ws_var->get_sample();
    EXPECT_TRUE(sample_undef == nullptr);
    EXPECT_EQ(global_ws_var->get_nb_periods(), 0);

    EXPECT_THROW(global_ws_var->set_sample("", "2015Y1"), std::invalid_argument);
    EXPECT_THROW(global_ws_var->set_sample("1960Y1", ""), std::invalid_argument);

    // set sample
    global_ws_var->set_sample("1960Y1", "2015Y1");
    EXPECT_EQ(global_ws_var->get_sample()->to_string(), "1960Y1:2015Y1");
}

TEST_F(KDBVariablesTest, Sample)
{
    Sample* sample = global_ws_var->get_sample();
    Sample expected_sample("1960Y1", "2015Y1");
    EXPECT_EQ(sample->to_string(), expected_sample.to_string());

    // does nothing
    global_ws_var->set_sample("", "");
    EXPECT_EQ(sample->to_string(), expected_sample.to_string());

    // update 'to'
    global_ws_var->set_sample("", "2016Y1");
    EXPECT_EQ(global_ws_var->get_sample()->to_string(), "1960Y1:2016Y1");

    // update 'from'
    global_ws_var->set_sample("1959Y1", "");
    EXPECT_EQ(global_ws_var->get_sample()->to_string(), "1959Y1:2016Y1");

    // update sample
    global_ws_var->set_sample("1950Y1", "2020Y1");
    EXPECT_EQ(global_ws_var->get_sample()->to_string(), "1950Y1:2020Y1");
}

TEST_F(KDBVariablesTest, GetNbPeriods)
{
    int expected_nb_periods = 56;
    EXPECT_EQ(global_ws_var->get_nb_periods(), expected_nb_periods);
}

TEST_F(KDBVariablesTest, GetPeriod)
{
    std::string expected_period = "1970Y1";
    EXPECT_EQ(global_ws_var->get_period(t), expected_period);

    float expected_fperiod = 1970.0;
    EXPECT_FLOAT_EQ(global_ws_var->get_period_as_float(t), expected_fperiod);
}

TEST_F(KDBVariablesTest, GetPeriodList)
{
    int first_year = 1960;
    int last_year = 2015;
    std::vector<std::string> expected_periods;
    std::vector<float> expected_fperiods;

    // all periods
    int start = first_year;
    int end = last_year;
    // as string
    for(int y = start; y <= end; y++)
        expected_periods.push_back(std::to_string(y) + "Y1");
    EXPECT_EQ(global_ws_var->get_list_periods(), expected_periods);
    // as float
    for(int y = start; y <= end; y++)
        expected_fperiods.push_back((float) y);
    EXPECT_EQ(global_ws_var->get_list_periods_as_float(), expected_fperiods);

    // subset of periods (from)
    start = 1970;
    end = last_year;
    // as string
    expected_periods.clear();
    for(int y = start; y <= end; y++)
        expected_periods.push_back(std::to_string(y) + "Y1");
    EXPECT_EQ(global_ws_var->get_list_periods("1970Y1"), expected_periods);
    // as float
    expected_fperiods.clear();
    for(int y = start; y <= end; y++)
        expected_fperiods.push_back((float) y);
    EXPECT_EQ(global_ws_var->get_list_periods_as_float("1970Y1"), expected_fperiods);

    // subset of periods (to)
    start = first_year;
    end = 2010;
    // as string
    expected_periods.clear();
    for(int y = start; y <= end; y++)
        expected_periods.push_back(std::to_string(y) + "Y1");
    EXPECT_EQ(global_ws_var->get_list_periods("", "2010Y1"), expected_periods);
    // as float
    expected_fperiods.clear();
    for(int y = start; y <= end; y++)
        expected_fperiods.push_back((float) y);
    EXPECT_EQ(global_ws_var->get_list_periods_as_float("", "2010Y1"), expected_fperiods);
}

TEST_F(KDBVariablesTest, Get)
{
    int nb_periods = global_ws_var->get_nb_periods();
    std::string name = "ACAF";
    Variable var;
    Variable expected_var;

    EXPECT_EQ(global_ws_var->get_var(name, "1990Y1", VAR_MODE_LEVEL), 23.771);
    EXPECT_NEAR(global_ws_var->get_var(name, "1990Y1", VAR_MODE_DIFF), 6.606, 1e-3);
    EXPECT_NEAR(global_ws_var->get_var(name, "1990Y1", VAR_MODE_GROWTH_RATE), 38.485, 1e-3);
    EXPECT_NEAR(global_ws_var->get_var(name, "1990Y1", VAR_MODE_Y0Y_DIFF), 6.606, 1e-3);
    EXPECT_NEAR(global_ws_var->get_var(name, "1990Y1", VAR_MODE_Y0Y_GROWTH_RATE), 38.485, 1e-3);
    
    int t_1990 = global_ws_var->get_sample()->get_period_position("1990Y1");
    EXPECT_EQ(global_ws_var->get_var(name, t_1990, VAR_MODE_LEVEL), 23.771);
    EXPECT_NEAR(global_ws_var->get_var(name, t_1990, VAR_MODE_DIFF), 6.606, 1e-3);
    EXPECT_NEAR(global_ws_var->get_var(name, t_1990, VAR_MODE_GROWTH_RATE), 38.485, 1e-3);
    EXPECT_NEAR(global_ws_var->get_var(name, t_1990, VAR_MODE_Y0Y_DIFF), 6.606, 1e-3);
    EXPECT_NEAR(global_ws_var->get_var(name, t_1990, VAR_MODE_Y0Y_GROWTH_RATE), 38.485, 1e-3);

    Period p_1990("1990Y1");
    EXPECT_EQ(global_ws_var->get_var(name, p_1990, VAR_MODE_LEVEL), 23.771);
    EXPECT_NEAR(global_ws_var->get_var(name, p_1990, VAR_MODE_DIFF), 6.606, 1e-3);
    EXPECT_NEAR(global_ws_var->get_var(name, p_1990, VAR_MODE_GROWTH_RATE), 38.485, 1e-3);
    EXPECT_NEAR(global_ws_var->get_var(name, p_1990, VAR_MODE_Y0Y_DIFF), 6.606, 1e-3);
    EXPECT_NEAR(global_ws_var->get_var(name, p_1990, VAR_MODE_Y0Y_GROWTH_RATE), 38.485, 1e-3);

    expected_var.reserve(nb_periods);
    for (int p = 0; p < nb_periods; p++)
        expected_var.push_back(global_ws_var->get_var(name, p, mode));

    var = global_ws_var->get(name);
    EXPECT_EQ(var, expected_var);
}

TEST_F(KDBVariablesTest, GetNames)
{
    std::set<std::string> expected_names;
    for (int i=0; i < global_ws_var->size(); i++) 
        expected_names.insert(global_ws_var->get_name(i));
    std::set<std::string> names = global_ws_var->get_names();
    EXPECT_EQ(names, expected_names);
}

TEST_F(KDBVariablesTest, CreateRemove)
{
    std::string new_name = "NEW_VAR";
    int nb_periods = global_ws_var->get_nb_periods();

    // pass a vector with values
    Variable new_var;
    new_var.reserve(nb_periods);
    for (int p = 0; p < nb_periods; p++) 
        new_var.push_back(10. + p);

    global_ws_var->add(new_name, new_var);
    EXPECT_EQ(global_ws_var->get(new_name), new_var);

    // --- already existing name
    std::fill(new_var.begin(), new_var.end(), 1);
    EXPECT_THROW(global_ws_var->add(new_name, new_var), std::invalid_argument);

    global_ws_var->remove(new_name);
    EXPECT_THROW(global_ws_var->get(new_name), std::invalid_argument);

    // --- wrong vector size
    new_var.pop_back();
    EXPECT_THROW(global_ws_var->add(new_name, new_var), std::length_error);

    // pass a LEC expression
    new_var.clear();
    for (int p = 0; p < nb_periods; p++) 
        new_var.push_back(10. + p);
    global_ws_var->add(new_name, "10 + t");
    EXPECT_EQ(global_ws_var->get(new_name), new_var);
    global_ws_var->remove(new_name);
    
    new_var[0] = IODE_NAN;
    for (int p = 1; p < nb_periods; p++) 
        new_var[p] = 1. / p;
    global_ws_var->add(new_name, "1 / t");
    EXPECT_EQ(global_ws_var->get(new_name), new_var);
    global_ws_var->remove(new_name);

    std::fill(new_var.begin(), new_var.end(), IODE_NAN);
    global_ws_var->add(new_name, "");
    EXPECT_EQ(global_ws_var->get(new_name), new_var);
    global_ws_var->remove(new_name);

    // --- using function that does not exist
    EXPECT_THROW(global_ws_var->add("FAILS", "func(t)"), std::runtime_error);
    // --- using variable that does not exist
    EXPECT_THROW(global_ws_var->add("FAILS", "ln Z"), std::runtime_error);
}

TEST_F(KDBVariablesTest, Update)
{
    std::string name;
    std::string lec;
    Variable expected_var;
    int nb_periods = global_ws_var->get_nb_periods();
    Sample* sample = global_ws_var->get_sample();

    Variable values;
    for(int y=1980; y <= 1990; y++)
        values.push_back((double) y);

    std::string first_period = "1980Y1";
    std::string last_period = "1990Y1";
    int t_first = sample->get_period_position(first_period);
    int t_last = sample->get_period_position(last_period);
    
    int t_last_wrong = t_last + 1;
    std::string last_period_wrong = "1991Y1";

    // 1) pass a vector with values
    name = global_ws_var->get_name(pos);
    expected_var.clear();
    for (int p = 0; p < nb_periods; p++) 
        expected_var.push_back(20.0 + p);
    global_ws_var->update(name, expected_var);
    EXPECT_EQ(global_ws_var->get(name), expected_var);
    // -- wrong size of the passed vector of new values
    expected_var.pop_back();
    EXPECT_THROW(global_ws_var->update(name, expected_var), std::length_error);

    // 2) for a given range of periods (as int)
    name = global_ws_var->get_name(pos+1);
    expected_var = global_ws_var->get(name);
    for(int i=0; i < values.size(); i++)
        expected_var[t_first + i] = values[i];
    global_ws_var->update(name, values, t_first, t_last);
    EXPECT_EQ(global_ws_var->get(name), expected_var);
    // -- wrong last period
    EXPECT_THROW(global_ws_var->update(name, values, t_first, t_last_wrong), 
        std::length_error);

    // 3) for a given range of periods (as string)
    name = global_ws_var->get_name(pos+2);
    expected_var = global_ws_var->get(name);
    for(int i=0; i < values.size(); i++)
        expected_var[t_first + i] = values[i];
    global_ws_var->update(name, values, first_period, last_period);
    EXPECT_EQ(global_ws_var->get(name), expected_var);
    // -- wrong last period
    EXPECT_THROW(global_ws_var->update(name, values, first_period, last_period_wrong), 
        std::length_error);

    // 4) pass a LEC expression
    name = global_ws_var->get_name(pos+3);
    expected_var.clear();
    for(int t=0; t < nb_periods; t++)
        expected_var.push_back(30.0 + t);
    lec = "30 + t";
    global_ws_var->update(name, lec);
    EXPECT_EQ(global_ws_var->get(name), expected_var);

    // 5) pass a LEC expression for a given range of periods (as int)
    expected_var = global_ws_var->get(name);
    for(int i=0; i < values.size(); i++)
        expected_var[t_first + i] = 40.0 + t_first + i;
    lec = "40 + t";
    global_ws_var->update(name, lec, first_period, last_period);
    EXPECT_EQ(global_ws_var->get(name), expected_var);
}

TEST_F(KDBVariablesTest, Filter)
{
    std::string pattern = "A*;*_";
    std::set<std::string> expected_names;
    KDBVariables* kdb_subset;

    std::set<std::string> all_names;
    for (int p = 0; p < global_ws_var->size(); p++) 
        all_names.insert(global_ws_var->get_name(p));

    int nb_total_variables = global_ws_var->size();
    // A*
    for (const std::string& name : all_names) 
        if (name.front() == 'A') 
            expected_names.insert(name);
    // *_
    for (const std::string& name : all_names) 
        if (name.back() == '_') 
            expected_names.insert(name);

    // create a subset (shallow copy)
    kdb_subset = new KDBVariables(global_ws_var.get(), pattern, false);
    EXPECT_EQ(kdb_subset->size(), expected_names.size());
    EXPECT_EQ(kdb_subset->get_names(), expected_names);

    // modify an element of the subset and check if the 
    // corresponding element of the global KDB also changes
    std::string name = "ACAF";
    int nb_periods = global_ws_var->get_nb_periods();
    std::string lec = "10 + t";
    Variable updated_var;
    updated_var.reserve(nb_periods);
    for (int p = 0; p < nb_periods; p++) updated_var.push_back(10. + p);
    kdb_subset->update(name, lec);
    EXPECT_EQ(kdb_subset->get(name), updated_var);
    EXPECT_EQ(global_ws_var->get(name), updated_var);

    // modify an element of the subset and check if it has also 
    // been added to the global KDB
    std::string new_name = "NEW_VARIABLE";
    Variable new_var;
    new_var.reserve(nb_periods);
    for (int p = 0; p < nb_periods; p++) new_var.push_back(10. + p);
    kdb_subset->add(new_name, new_var);
    EXPECT_EQ(kdb_subset->get(new_name), new_var);
    EXPECT_EQ(global_ws_var->get(new_name), new_var);

    // modify an element of the subset and check if the 
    // corresponding element has also been renamed in the global KDB
    std::string old_name = new_name;
    new_name = "VARIABLE_NEW";
    kdb_subset->rename(old_name, new_name);
    EXPECT_EQ(kdb_subset->get(new_name), new_var);
    EXPECT_EQ(global_ws_var->get(new_name), new_var);

    // delete an element from the subset and check if it has also 
    // been deleted from the global KDB
    kdb_subset->remove(new_name);
    EXPECT_FALSE(kdb_subset->contains(new_name));
    EXPECT_FALSE(global_ws_var->contains(new_name));

    // try to add an element to the subset which is already present 
    // in the global KDB
    EXPECT_THROW(kdb_subset->add("BENEF", new_var), std::invalid_argument);

    // delete subset
    delete kdb_subset;
    EXPECT_EQ(global_ws_var->size(), nb_total_variables);
    EXPECT_EQ(global_ws_var->get(name), updated_var);

    // wrong pattern
    pattern = "anjfks";
    EXPECT_THROW(KDBVariables(global_ws_var.get(), pattern, false), std::runtime_error);
}

TEST_F(KDBVariablesTest, DeepCopy)
{
    std::string pattern = "A*;*_";
    std::set<std::string> expected_names;
    KDBVariables* kdb_subset;

    std::set<std::string> all_names;
    for (int p = 0; p < global_ws_var->size(); p++) 
        all_names.insert(global_ws_var->get_name(p));

    int nb_total_variables = global_ws_var->size();
    // A*
    for (const std::string& name : all_names) 
        if (name.front() == 'A') 
            expected_names.insert(name);
    // *_
    for (const std::string& name : all_names) 
        if (name.back() == '_') 
            expected_names.insert(name);

    // create a subset (deep copy)
    kdb_subset = new KDBVariables(global_ws_var.get(), pattern, true);
    EXPECT_EQ(kdb_subset->size(), expected_names.size());
    EXPECT_EQ(kdb_subset->get_names(), expected_names);

    // modify an element of the subset and check if the 
    // corresponding element of the global KDB didn't changed
    std::string name = "ACAF";
    Variable var = global_ws_var->get(name);
    int nb_periods = global_ws_var->get_nb_periods();
    std::string lec = "10 + t";
    Variable updated_var;
    updated_var.reserve(nb_periods);
    for (int p = 0; p < nb_periods; p++) updated_var.push_back(10. + p);
    kdb_subset->update(name, lec);
    EXPECT_EQ(kdb_subset->get(name), updated_var);
    EXPECT_EQ(global_ws_var->get(name), var);

    // modify an element of the subset and check if it has not 
    // been added to the global KDB
    std::string new_name = "NEW_VARIABLE";
    Variable new_var;
    new_var.reserve(nb_periods);
    for (int p = 0; p < nb_periods; p++) new_var.push_back(10. + p);
    kdb_subset->add(new_name, new_var);
    EXPECT_TRUE(kdb_subset->contains(new_name));
    EXPECT_EQ(kdb_subset->get(new_name), new_var);
    EXPECT_FALSE(global_ws_var->contains(new_name));

    // modify an element of the subset and check if the 
    // corresponding element has not been renamed in the global KDB
    name = "ACAG";
    new_name = "VARIABLE_NEW";
    kdb_subset->rename(name, new_name);
    EXPECT_TRUE(kdb_subset->contains(new_name));
    EXPECT_FALSE(global_ws_var->contains(new_name));

    // delete an element from the subset and check if it has not 
    // been deleted from the global KDB
    name = "AOUC";
    kdb_subset->remove(name);
    EXPECT_FALSE(kdb_subset->contains(name));
    EXPECT_TRUE(global_ws_var->contains(name));

    // delete subset
    delete kdb_subset;
    EXPECT_EQ(global_ws_var->size(), nb_total_variables);
}

TEST_F(KDBVariablesTest, CopyFrom)
{
    std::string filename = str_input_test_dir + prefix_filename + "fun.var";
    int expected_nb_comments = global_ws_var->size();
    std::vector<std::string> v_expected_names;

    // Copy entire file
    global_ws_var->clear();
    global_ws_var->copy_from(filename, "", "", "*");
    EXPECT_EQ(global_ws_var->size(), expected_nb_comments); 
    EXPECT_NEAR(global_ws_var->get_var("ACAF", "1992Y1"), 30.159, 1e-3);
    EXPECT_NEAR(global_ws_var->get_var("ACAG", "1992Y1"), -40.286, 1e-3);

    // copy partial WS (92-93) on an existing one
    global_ws_var->clear();
    global_ws_var->set_sample("1990Y1", "2000Y1");
    global_ws_var->add("ACAF", "t");
    global_ws_var->copy_from(filename, "1992Y1", "1993Y1", "ACAF ACAG");
    EXPECT_DOUBLE_EQ(global_ws_var->get_var("ACAF", "1991Y1"), 1.0); 
    EXPECT_NEAR(global_ws_var->get_var("ACAF", "1992Y1"), 30.159, 1e-3); 
    EXPECT_DOUBLE_EQ(global_ws_var->get_var("ACAG", "1991Y1"), IODE_NAN);
    EXPECT_NEAR(global_ws_var->get_var("ACAG", "1992Y1"), -40.286, 1e-3);

    // copy partial WS on an existing one without specifying the sample
    global_ws_var->clear();
    global_ws_var->set_sample("1990Y1", "2000Y1");
    global_ws_var->copy_from(filename, "", "", "ACAF ACAG");
    EXPECT_NEAR(global_ws_var->get_var("ACAF", "1992Y1"), 30.159, 1e-3);
    EXPECT_NEAR(global_ws_var->get_var("ACAG", "1992Y1"), -40.286, 1e-3);
}

TEST_F(KDBVariablesTest, Merge)
{
    std::string pattern = "A*";

    // create deep copies kdb
    KDBVariables* kdb0 = new KDBVariables(global_ws_var.get(), pattern, true);
    KDBVariables* kdb1 = new KDBVariables(global_ws_var.get(), pattern, true);
    KDBVariables* kdb_to_merge = new KDBVariables(global_ws_var.get(), pattern, true);

    int nb_periods = kdb_to_merge->get_nb_periods();

    // add an element to the KDB to be merged
    std::string new_name = "NEW_VARIABLE";
    Variable new_var;
    new_var.reserve(nb_periods);
    for (int p = 0; p < nb_periods; p++) new_var.push_back(10. + p);
    kdb_to_merge->add(new_name, new_var);

    // modify an existing element of the KDB to be merge
    std::string name = "ACAF";
    Variable unmodified_var = kdb_to_merge->get(name);
    std::string lec = "10 + t";
    Variable modified_var;
    modified_var.reserve(nb_periods);
    for (int p = 0; p < nb_periods; p++) modified_var.push_back(10. + p);
    kdb_to_merge->update(name, lec);

    // merge (overwrite)
    kdb0->merge(*kdb_to_merge, true);
    // a) check kdb0 contains new item of KDB to be merged
    EXPECT_TRUE(kdb0->contains(new_name));
    EXPECT_EQ(kdb0->get(new_name), new_var);
    // b) check already existing item has been overwritten
    EXPECT_EQ(kdb0->get(name), modified_var); 

    // merge (NOT overwrite)
    kdb1->merge(*kdb_to_merge, false);
    // b) check already existing item has NOT been overwritten
    EXPECT_EQ(kdb1->get(name), unmodified_var);
}

TEST_F(KDBVariablesTest, Search)
{
    std::string var_name = "AOUC";
    std::vector<std::string> objs_list;

    global_ws_cmt->load(str_input_test_dir + "fun.ac");
    global_ws_eqs->load(str_input_test_dir + "fun.ae");
    global_ws_idt->load(str_input_test_dir + "fun.ai");
    global_ws_lst->load(str_input_test_dir + "fun.al");
    global_ws_scl->load(str_input_test_dir + "fun.as");
    global_ws_tbl->load(str_input_test_dir + "fun.at");

    std::vector<std::string> expected_cmts = { var_name };
    objs_list = global_ws_cmt->search(var_name);
    EXPECT_EQ(objs_list, expected_cmts);

    std::vector<std::string> expected_eqs = { var_name, "PC", "PIF", "PXS", "QXAB" };
    objs_list = global_ws_eqs->search(var_name);
    EXPECT_EQ(objs_list, expected_eqs);

    std::vector<std::string> expected_idt = { var_name };
    objs_list = global_ws_idt->search(var_name);
    EXPECT_EQ(objs_list, expected_idt);

    std::vector<std::string> expected_lts = { "COPY0", "ENDO0", "TOTAL0", "_RES" };
    objs_list = global_ws_lst->search(var_name);
    EXPECT_EQ(objs_list, expected_lts);

    objs_list = global_ws_scl->search(var_name);
    EXPECT_EQ(objs_list.size(), 0);

    std::vector<std::string> expected_tbl = { "ANAPRIX", "MULT1FR", "MULT1RESU", "T1", "T1NIVEAU" };
    objs_list = global_ws_tbl->search(var_name);
    EXPECT_EQ(objs_list, expected_tbl);

    std::vector<std::string> expected_vars = { var_name };
    objs_list = global_ws_var->search(var_name);
    EXPECT_EQ(objs_list, expected_vars);
}

TEST_F(KDBVariablesTest, Extrapolate)
{
    global_ws_var->clear();
    global_ws_var->set_sample("1995Y1", "2020Y1");
    global_ws_var->add("ACAF", "t");

    // Y := Y[-1], if Y null or NA
    global_ws_var->set_var("ACAF", "2000Y1", IODE_NAN);
    global_ws_var->set_var("ACAF", "2002Y1", IODE_NAN);
    global_ws_var->extrapolate(VariablesInitialization::VAR_INIT_TM1, "2000Y1", "2020Y1");
    EXPECT_DOUBLE_EQ(global_ws_var->get_var("ACAF", "1999Y1"), 4.0);
    EXPECT_DOUBLE_EQ(global_ws_var->get_var("ACAF", "2000Y1"), 4.0);
    EXPECT_DOUBLE_EQ(global_ws_var->get_var("ACAF", "2001Y1"), 6.0);
    EXPECT_DOUBLE_EQ(global_ws_var->get_var("ACAF", "2002Y1"), 6.0);

    // Y := Y[-1], always
    global_ws_var->update("ACAF", "t");
    global_ws_var->set_var("ACAF", "2000Y1", IODE_NAN);
    global_ws_var->set_var("ACAF", "2002Y1", IODE_NAN);
    global_ws_var->extrapolate(VariablesInitialization::VAR_INIT_TM1_A, "2000Y1", "2020Y1");
    EXPECT_DOUBLE_EQ(global_ws_var->get_var("ACAF", "1999Y1"), 4.0);
    EXPECT_DOUBLE_EQ(global_ws_var->get_var("ACAF", "2000Y1"), 4.0);
    EXPECT_DOUBLE_EQ(global_ws_var->get_var("ACAF", "2001Y1"), 4.0);
    EXPECT_DOUBLE_EQ(global_ws_var->get_var("ACAF", "2002Y1"), 4.0);

    // Y := extrapolation, if Y null or NA
    global_ws_var->update("ACAF", "t");
    global_ws_var->set_var("ACAF", "2000Y1", IODE_NAN);
    global_ws_var->set_var("ACAF", "2002Y1", IODE_NAN);
    global_ws_var->extrapolate(VariablesInitialization::VAR_INIT_EXTRA, "2000Y1", "2020Y1");
    EXPECT_DOUBLE_EQ(global_ws_var->get_var("ACAF", "1999Y1"), 4.0);
    EXPECT_DOUBLE_EQ(global_ws_var->get_var("ACAF", "2000Y1"), 5.0);
    EXPECT_DOUBLE_EQ(global_ws_var->get_var("ACAF", "2001Y1"), 6.0);
    EXPECT_DOUBLE_EQ(global_ws_var->get_var("ACAF", "2002Y1"), 7.0);

    // Y := extrapolation, always
    global_ws_var->update("ACAF", "t");
    global_ws_var->set_var("ACAF", "2000Y1", IODE_NAN);
    global_ws_var->set_var("ACAF", "2002Y1", IODE_NAN);
    global_ws_var->extrapolate(VariablesInitialization::VAR_INIT_EXTRA_A, "2000Y1", "2020Y1");
    EXPECT_DOUBLE_EQ(global_ws_var->get_var("ACAF", "1999Y1"), 4.0);
    EXPECT_DOUBLE_EQ(global_ws_var->get_var("ACAF", "2000Y1"), 5.0);
    EXPECT_DOUBLE_EQ(global_ws_var->get_var("ACAF", "2001Y1"), 6.0);
    EXPECT_DOUBLE_EQ(global_ws_var->get_var("ACAF", "2002Y1"), 7.0);

    // Y unchanged
    global_ws_var->update("ACAF", "t");
    global_ws_var->set_var("ACAF", "2000Y1", IODE_NAN);
    global_ws_var->set_var("ACAF", "2002Y1", IODE_NAN);
    global_ws_var->extrapolate(VariablesInitialization::VAR_INIT_ASIS, "2000Y1", "2020Y1");
    EXPECT_DOUBLE_EQ(global_ws_var->get_var("ACAF", "1999Y1"), 4.0);
    EXPECT_DOUBLE_EQ(global_ws_var->get_var("ACAF", "2000Y1"), IODE_NAN);
    EXPECT_DOUBLE_EQ(global_ws_var->get_var("ACAF", "2001Y1"), 6.0);
    EXPECT_DOUBLE_EQ(global_ws_var->get_var("ACAF", "2002Y1"), IODE_NAN);

    // Y := Y[-1], if Y = NA
    global_ws_var->update("ACAF", "t");
    global_ws_var->set_var("ACAF", "2000Y1", IODE_NAN);
    global_ws_var->set_var("ACAF", "2002Y1", IODE_NAN);
    global_ws_var->extrapolate(VariablesInitialization::VAR_INIT_TM1_NA, "2000Y1", "2020Y1");
    EXPECT_DOUBLE_EQ(global_ws_var->get_var("ACAF", "1999Y1"), 4.0);
    EXPECT_DOUBLE_EQ(global_ws_var->get_var("ACAF", "2000Y1"), 4.0);
    EXPECT_DOUBLE_EQ(global_ws_var->get_var("ACAF", "2001Y1"), 6.0);
    EXPECT_DOUBLE_EQ(global_ws_var->get_var("ACAF", "2002Y1"), 6.0);

    // Y := extrapolation, if Y = NA
    global_ws_var->update("ACAF", "t");
    global_ws_var->set_var("ACAF", "2000Y1", IODE_NAN);
    global_ws_var->set_var("ACAF", "2002Y1", IODE_NAN);
    global_ws_var->extrapolate(VariablesInitialization::VAR_INIT_EXTRA_NA, "2000Y1", "2020Y1");
    EXPECT_DOUBLE_EQ(global_ws_var->get_var("ACAF", "1999Y1"), 4.0);
    EXPECT_DOUBLE_EQ(global_ws_var->get_var("ACAF", "2000Y1"), 5.0);
    EXPECT_DOUBLE_EQ(global_ws_var->get_var("ACAF", "2001Y1"), 6.0);
    EXPECT_DOUBLE_EQ(global_ws_var->get_var("ACAF", "2002Y1"), 7.0);
}

TEST_F(KDBVariablesTest, Hash)
{
    std::size_t hash_val = hash_value(*global_ws_var);

    // change a name
    global_ws_var->rename("ACAF", "NEW_NAME");
    std::size_t hash_val_modified = hash_value(*global_ws_var);
    EXPECT_NE(hash_val, hash_val_modified);
    std::cout << "(rename variable) orignal vs modified hash: " << std::to_string(hash_val) << " vs " << std::to_string(hash_val_modified) << std::endl;

    // modify an entry
    hash_val = hash_val_modified;
    std::string lec = "10 + t";
    global_ws_var->update("NEW_NAME", lec);
    hash_val_modified = hash_value(*global_ws_var);
    EXPECT_NE(hash_val, hash_val_modified);
    std::cout << "(modify variable) orignal vs modified hash: " << std::to_string(hash_val) << " vs " << std::to_string(hash_val_modified) << std::endl;

    // remove an entry
    hash_val = hash_val_modified;
    global_ws_var->remove("NEW_NAME");
    hash_val_modified = hash_value(*global_ws_var);
    EXPECT_NE(hash_val, hash_val_modified);
    std::cout << "(delete variable) orignal vs modified hash: " << std::to_string(hash_val) << " vs " << std::to_string(hash_val_modified) << std::endl;

    // add an entry
    hash_val = hash_val_modified;
    lec = "20 + t";
    global_ws_var->add("NEW_ENTRY", lec);
    hash_val_modified = hash_value(*global_ws_var);
    EXPECT_NE(hash_val, hash_val_modified);   
    std::cout << "(new    variable) orignal vs modified hash: " << std::to_string(hash_val) << " vs " << std::to_string(hash_val_modified) << std::endl; 
}
