#include "pch.h"


class KDBVariablesTest : public KDBTest, public ::testing::Test
{
protected:
    IodeVarMode mode = VAR_MODE_LEVEL;
    int pos = 0;
    int t = 10;
    int t_nan = 5;

    void SetUp() override
    {
        KDBVariables kdb_var(input_test_dir + "fun.av");
    }

    // void TearDown() override {}
};

class KDBVariablesEmptyTest : public KDBTest, public ::testing::Test
{
};


TEST_F(KDBVariablesTest, Load)
{
    KDBVariables kdb(input_test_dir + prefix_filename + "fun.var");
    EXPECT_EQ(kdb.count(), 394);
}

TEST_F(KDBVariablesTest, Subset)
{
    std::string pattern = "A*";
    Variable var = Variables.get("ACAF");
    std::string lec = "10 + t";
    Variable new_var;
    new_var.reserve(var.size());
    for (int p = 0; p < var.size(); p++) new_var.push_back(10.0 + p);

    // GLOBAL KDB
    KDBVariables kdb_global;
    EXPECT_EQ(kdb_global.count(), 394);
    EXPECT_TRUE(kdb_global.is_global_database());

    // DEEP COPY SUBSET
    KDBVariables* kdb_subset_deep_copy = kdb_global.subset(pattern, true);
    std::vector<std::string> names = kdb_global.get_names(pattern);
    EXPECT_EQ(kdb_subset_deep_copy->count(), names.size());
    EXPECT_TRUE(kdb_subset_deep_copy->is_local_database());
    kdb_subset_deep_copy->update("ACAF", lec);
    EXPECT_EQ(kdb_global.get("ACAF"), var);
    EXPECT_EQ(kdb_subset_deep_copy->get("ACAF"), new_var);

    // SHALLOW COPY SUBSET
    KDBVariables* kdb_subset_shallow_copy = kdb_global.subset(pattern, false);
    EXPECT_EQ(kdb_subset_shallow_copy->count(), names.size());
    EXPECT_TRUE(kdb_subset_shallow_copy->is_shallow_copy_database());
    kdb_subset_shallow_copy->update("ACAF", lec);
    EXPECT_EQ(kdb_global.get("ACAF"), new_var);
    EXPECT_EQ(kdb_subset_shallow_copy->get("ACAF"), new_var);
}

TEST_F(KDBVariablesTest, Save)
{
    // save in binary format
    Variables.save(output_test_dir + "fun.var");

    // save in ascii format
    Variables.save(output_test_dir + "fun.av");
}

TEST_F(KDBVariablesTest, GetSetVar)
{
    double value;
    double new_value;
    int nb_periods = Variables.get_nb_periods();
    std::string name = Variables.get_name(pos + 1);
    Period start = Variables.get_sample()->start_period();

    // period as int
    value = 1.2130001;
    EXPECT_DOUBLE_EQ(Variables.get_var(pos, t), value);
    new_value = 10.0;
    Variables.set_var(pos, t, new_value);
    EXPECT_DOUBLE_EQ(Variables.get_var(pos, t), new_value);

    value = -11.028999;
    EXPECT_DOUBLE_EQ(Variables.get_var(name, t), value);
    new_value = 20.0;
    Variables.set_var(name, t, new_value);
    EXPECT_DOUBLE_EQ(Variables.get_var(name, t), new_value);

    // period as Period object
    Period period = start.shift(t + 1);

    value = 5.2020001;
    EXPECT_DOUBLE_EQ(Variables.get_var(pos, period), value);
    new_value = 30.0;
    Variables.set_var(pos, period, new_value);
    EXPECT_DOUBLE_EQ(Variables.get_var(pos, period), new_value);

    value = -15.847;
    EXPECT_DOUBLE_EQ(Variables.get_var(name, period), value);
    new_value = 40.0;
    Variables.set_var(name, period, new_value);
    EXPECT_DOUBLE_EQ(Variables.get_var(name, period), new_value);

    // period as string
    std::string s_period = start.shift(t + 2).to_string();

    value = 9.184;
    EXPECT_DOUBLE_EQ(Variables.get_var(pos, s_period), value);
    new_value = 50.0;
    Variables.set_var(pos, s_period, new_value);
    EXPECT_DOUBLE_EQ(Variables.get_var(pos, s_period), new_value);

    value = -19.288002;
    EXPECT_DOUBLE_EQ(Variables.get_var(name, s_period), value);
    new_value = 60.0;
    Variables.set_var(name, s_period, new_value);
    EXPECT_DOUBLE_EQ(Variables.get_var(name, s_period), new_value);

    // get pointers to a variable's data
    double* data = Variables.get_var_ptr(pos);
    EXPECT_TRUE(data != NULL);
    value = Variables.get_var(pos, 0);
    EXPECT_DOUBLE_EQ(*data, value);
    EXPECT_DOUBLE_EQ(data[0], value);
    value = Variables.get_var(pos, t);
    EXPECT_DOUBLE_EQ(*(data + t), value);
    EXPECT_DOUBLE_EQ(data[t], value);
}

TEST_F(KDBVariablesEmptyTest, Sample)
{
    Sample* sample_undef = Variables.get_sample();
    EXPECT_EQ(sample_undef->nb_periods(), 0);
    EXPECT_EQ(sample_undef->to_string(), ":");

    EXPECT_THROW(Variables.set_sample("", "2015Y1"), std::invalid_argument);
    EXPECT_THROW(Variables.set_sample("1960Y1", ""), std::invalid_argument);

    // set sample
    Variables.set_sample("1960Y1", "2015Y1");
    EXPECT_EQ(Variables.get_sample()->to_string(), "1960Y1:2015Y1");
}

TEST_F(KDBVariablesTest, Sample)
{
    Sample* sample = Variables.get_sample();
    Sample expected_sample("1960Y1", "2015Y1");
    EXPECT_EQ(sample->to_string(), expected_sample.to_string());

    // does nothing
    Variables.set_sample("", "");
    EXPECT_EQ(sample->to_string(), expected_sample.to_string());

    // update 'to'
    Variables.set_sample("", "2016Y1");
    EXPECT_EQ(Variables.get_sample()->to_string(), "1960Y1:2016Y1");

    // update 'from'
    Variables.set_sample("1959Y1", "");
    EXPECT_EQ(Variables.get_sample()->to_string(), "1959Y1:2016Y1");

    // update sample
    Variables.set_sample("1950Y1", "2020Y1");
    EXPECT_EQ(Variables.get_sample()->to_string(), "1950Y1:2020Y1");
}

TEST_F(KDBVariablesTest, GetNbPeriods)
{
    int expected_nb_periods = 56;
    EXPECT_EQ(Variables.get_nb_periods(), expected_nb_periods);
}

TEST_F(KDBVariablesTest, GetPeriod)
{
    std::string expected_period = "1970Y1";
    EXPECT_EQ(Variables.get_period(t), expected_period);

    float expected_fperiod = 1970.0;
    EXPECT_FLOAT_EQ(Variables.get_period_as_float(t), expected_fperiod);
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
    EXPECT_EQ(Variables.get_list_periods(), expected_periods);
    // as float
    for(int y = start; y <= end; y++)
        expected_fperiods.push_back((float) y);
    EXPECT_EQ(Variables.get_list_periods_as_float(), expected_fperiods);

    // subset of periods (from)
    start = 1970;
    end = last_year;
    // as string
    expected_periods.clear();
    for(int y = start; y <= end; y++)
        expected_periods.push_back(std::to_string(y) + "Y1");
    EXPECT_EQ(Variables.get_list_periods("1970Y1"), expected_periods);
    // as float
    expected_fperiods.clear();
    for(int y = start; y <= end; y++)
        expected_fperiods.push_back((float) y);
    EXPECT_EQ(Variables.get_list_periods_as_float("1970Y1"), expected_fperiods);

    // subset of periods (to)
    start = first_year;
    end = 2010;
    // as string
    expected_periods.clear();
    for(int y = start; y <= end; y++)
        expected_periods.push_back(std::to_string(y) + "Y1");
    EXPECT_EQ(Variables.get_list_periods("", "2010Y1"), expected_periods);
    // as float
    expected_fperiods.clear();
    for(int y = start; y <= end; y++)
        expected_fperiods.push_back((float) y);
    EXPECT_EQ(Variables.get_list_periods_as_float("", "2010Y1"), expected_fperiods);
}

TEST_F(KDBVariablesTest, Get)
{
    int nb_periods = Variables.get_nb_periods();
    std::string name = Variables.get_name(pos);
    Variable var;
    Variable expected_var;
    
    expected_var.reserve(nb_periods);
    for (int p = 0; p < nb_periods; p++)
        expected_var.push_back(Variables.get_var(name, p, mode));

    // by position
    var = Variables.get(pos);
    EXPECT_EQ(var, expected_var);

    // by name
    var = Variables.get(name);
    EXPECT_EQ(var, expected_var);
}

TEST_F(KDBVariablesTest, GetNames)
{
    std::vector<std::string> expected_names;
    for (int i=0; i < Variables.count(); i++) expected_names.push_back(Variables.get_name(i));
    std::vector<std::string> names = Variables.get_names();
    EXPECT_EQ(names, expected_names);
}

TEST_F(KDBVariablesTest, CreateRemove)
{
    std::string new_name = "NEW_VAR";
    int nb_periods = Variables.get_nb_periods();

    // pass a vector with values
    Variable new_var;
    new_var.reserve(nb_periods);
    for (int p = 0; p < nb_periods; p++) 
        new_var.push_back(10. + p);

    Variables.add(new_name, new_var);
    EXPECT_EQ(Variables.get(new_name), new_var);

    // --- already existing name
    std::fill(new_var.begin(), new_var.end(), 1);
    EXPECT_THROW(Variables.add(new_name, new_var), std::invalid_argument);

    Variables.remove(new_name);
    EXPECT_THROW(Variables.get(new_name), std::invalid_argument);

    // --- wrong vector size
    new_var.pop_back();
    EXPECT_THROW(Variables.add(new_name, new_var), std::length_error);

    // pass a LEC expression
    new_var.clear();
    for (int p = 0; p < nb_periods; p++) 
        new_var.push_back(10. + p);
    Variables.add(new_name, "10 + t");
    EXPECT_EQ(Variables.get(new_name), new_var);
    Variables.remove(new_name);
    
    new_var[0] = IODE_NAN;
    for (int p = 1; p < nb_periods; p++) 
        new_var[p] = 1. / p;
    Variables.add(new_name, "1 / t");
    EXPECT_EQ(Variables.get(new_name), new_var);
    Variables.remove(new_name);

    std::fill(new_var.begin(), new_var.end(), IODE_NAN);
    Variables.add(new_name, "");
    EXPECT_EQ(Variables.get(new_name), new_var);
    Variables.remove(new_name);

    // --- using function that does not exist
    EXPECT_THROW(Variables.add("FAILS", "func(t)"), std::runtime_error);
    // --- using variable that does not exist
    EXPECT_THROW(Variables.add("FAILS", "ln Z"), std::runtime_error);
}

TEST_F(KDBVariablesTest, Update)
{
    std::string name;
    std::string lec;
    Variable expected_var;
    int nb_periods = Variables.get_nb_periods();
    Sample* sample = Variables.get_sample();

    Variable values;
    for(int y=1980; y <= 1990; y++)
        values.push_back((double) y);

    std::string first_period = "1980Y1";
    std::string last_period = "1990Y1";
    int t_first = sample->get_period_position(first_period);
    int t_last = sample->get_period_position(last_period);
    
    int t_last_wrong = t_last + 1;
    std::string last_period_wrong = "1991Y1";

    // ---- by position ----

    // 1) pass a vector with values
    for (int p = 0; p < nb_periods; p++) 
        expected_var.push_back(10.0 + p);
    Variables.update(pos, expected_var);
    EXPECT_EQ(Variables.get(pos), expected_var);
    // -- wrong size of the passed vector of new values
    expected_var.pop_back();
    EXPECT_THROW(Variables.update(pos, expected_var), std::range_error);

    // 2) for a given range of periods (as int)
    expected_var = Variables.get(pos+1);
    for(int i=0; i < values.size(); i++)
        expected_var[t_first + i] = values[i];
    Variables.update(pos+1, values, t_first, t_last);
    EXPECT_EQ(Variables.get(pos+1), expected_var);
    // -- wrong last period
    EXPECT_THROW(Variables.update(pos+1, values, t_first, t_last_wrong), std::range_error);

    // 3) for a given range of periods (as string)
    expected_var = Variables.get(pos+2);
    for(int i=0; i < values.size(); i++)
        expected_var[t_first + i] = values[i];
    Variables.update(pos+2, values, first_period, last_period);
    EXPECT_EQ(Variables.get(pos+2), expected_var);
    // -- wrong last period
    EXPECT_THROW(Variables.update(pos+2, values, first_period, last_period_wrong), std::range_error);

    // 4) pass a LEC expression
    expected_var.clear();
    for(int t=0; t < nb_periods; t++)
        expected_var.push_back(10.0 + t);
    lec = "10 + t";
    Variables.update(pos+3, lec);
    EXPECT_EQ(Variables.get(pos+3), expected_var);

    // 5) pass a LEC expression for a given range of periods (as int)
    expected_var = Variables.get(pos+3);
    for(int i=0; i < values.size(); i++)
        expected_var[t_first + i] = 20.0 + t_first + i;
    lec = "20 + t";
    Variables.update(pos+3, lec, first_period, last_period);
    EXPECT_EQ(Variables.get(pos+3), expected_var);

    // ---- by name ----

    // 1) pass a vector with values
    name = Variables.get_name(pos);
    expected_var.clear();
    for (int p = 0; p < nb_periods; p++) 
        expected_var.push_back(20.0 + p);
    Variables.update(name, expected_var);
    EXPECT_EQ(Variables.get(name), expected_var);
    // -- wrong size of the passed vector of new values
    expected_var.pop_back();
    EXPECT_THROW(Variables.update(name, expected_var), std::range_error);

    // 2) for a given range of periods (as int)
    name = Variables.get_name(pos+1);
    expected_var = Variables.get(name);
    for(int i=0; i < values.size(); i++)
        expected_var[t_first + i] = values[i];
    Variables.update(name, values, t_first, t_last);
    EXPECT_EQ(Variables.get(name), expected_var);
    // -- wrong last period
    EXPECT_THROW(Variables.update(name, values, t_first, t_last_wrong), std::range_error);

    // 3) for a given range of periods (as string)
    name = Variables.get_name(pos+2);
    expected_var = Variables.get(name);
    for(int i=0; i < values.size(); i++)
        expected_var[t_first + i] = values[i];
    Variables.update(name, values, first_period, last_period);
    EXPECT_EQ(Variables.get(name), expected_var);
    // -- wrong last period
    EXPECT_THROW(Variables.update(name, values, first_period, last_period_wrong), std::range_error);

    // 4) pass a LEC expression
    name = Variables.get_name(pos+3);
    expected_var.clear();
    for(int t=0; t < nb_periods; t++)
        expected_var.push_back(30.0 + t);
    lec = "30 + t";
    Variables.update(name, lec);
    EXPECT_EQ(Variables.get(name), expected_var);

    // 5) pass a LEC expression for a given range of periods (as int)
    expected_var = Variables.get(pos+3);
    for(int i=0; i < values.size(); i++)
        expected_var[t_first + i] = 40.0 + t_first + i;
    lec = "40 + t";
    Variables.update(pos+3, lec, first_period, last_period);
    EXPECT_EQ(Variables.get(pos+3), expected_var);
}

TEST_F(KDBVariablesTest, Copy)
{
    std::string name = Variables.get_name(pos);
    Variable var;
    Variable copy_var;

    // by position
    var = Variables.get(pos);
    copy_var = Variables.copy(pos);
    EXPECT_EQ(copy_var, var);

    // by name
    var = Variables.get(name);
    copy_var = Variables.copy(name);
    EXPECT_EQ(copy_var, var);

    // add copy
    Variables.add("DUP_" + name, copy_var);
}

TEST_F(KDBVariablesTest, Filter)
{
    std::string pattern = "A*;*_";
    std::vector<std::string> expected_names;
    KDBVariables* kdb_subset;

    std::vector<std::string> all_names;
    for (int p = 0; p < Variables.count(); p++) all_names.push_back(Variables.get_name(p));

    int nb_total_variables = Variables.count();
    // A*
    for (const std::string& name : all_names) if (name.front() == 'A') expected_names.push_back(name);
    // *_
    for (const std::string& name : all_names) if (name.back() == '_') expected_names.push_back(name);

    // remove duplicate entries
    // NOTE: std::unique only removes consecutive duplicated elements, 
    //       so the vector needst to be sorted first
    std::sort(expected_names.begin(), expected_names.end());
    std::vector<std::string>::iterator it = std::unique(expected_names.begin(), expected_names.end());  
    expected_names.resize(std::distance(expected_names.begin(), it));

    // create local kdb
    kdb_subset = Variables.subset(pattern);
    EXPECT_EQ(kdb_subset->count(), expected_names.size());
    EXPECT_EQ(kdb_subset->get_names(), expected_names);

    // modify an element of the local KDB and check if the 
    // corresponding element of the global KDB also changes
    std::string name = "ACAF";
    int nb_periods = Variables.get_nb_periods();
    std::string lec = "10 + t";
    Variable updated_var;
    updated_var.reserve(nb_periods);
    for (int p = 0; p < nb_periods; p++) updated_var.push_back(10. + p);
    kdb_subset->update(name, lec);
    EXPECT_EQ(kdb_subset->get(name), updated_var);
    EXPECT_EQ(Variables.get(name), updated_var);

    // add an element to the local KDB and check if it has also 
    // been added to the global KDB
    std::string new_name = "NEW_VARIABLE";
    Variable new_var;
    new_var.reserve(nb_periods);
    for (int p = 0; p < nb_periods; p++) new_var.push_back(10. + p);
    kdb_subset->add(new_name, new_var);
    EXPECT_EQ(kdb_subset->get(new_name), new_var);
    EXPECT_EQ(Variables.get(new_name), new_var);

    // rename an element in the local KDB and check if the 
    // corresponding element has also been renamed in the global KDB
    std::string old_name = new_name;
    new_name = "VARIABLE_NEW";
    kdb_subset->rename(old_name, new_name);
    EXPECT_EQ(kdb_subset->get(new_name), new_var);
    EXPECT_EQ(Variables.get(new_name), new_var);

    // delete an element from the local KDB and check if it has also 
    // been deleted from the global KDB
    kdb_subset->remove(new_name);
    EXPECT_FALSE(kdb_subset->contains(new_name));
    EXPECT_FALSE(Variables.contains(new_name));

    // try to add an element to the local KDB which is already present 
    // in the global KDB
    EXPECT_THROW(kdb_subset->add("BENEF", new_var), std::invalid_argument);

    // delete local kdb
    delete kdb_subset;
    EXPECT_EQ(Variables.count(), nb_total_variables);
    EXPECT_EQ(Variables.get(name), updated_var);

    // wrong pattern
    pattern = "anjfks";
    EXPECT_THROW(Variables.subset(pattern), std::runtime_error);
}

TEST_F(KDBVariablesTest, DeepCopy)
{
    std::string pattern = "A*;*_";
    std::vector<std::string> expected_names;
    KDBVariables* kdb_subset;

    std::vector<std::string> all_names;
    for (int p = 0; p < Variables.count(); p++) all_names.push_back(Variables.get_name(p));

    int nb_total_variables = Variables.count();
    // A*
    for (const std::string& name : all_names) if (name.front() == 'A') expected_names.push_back(name);
    // *_
    for (const std::string& name : all_names) if (name.back() == '_') expected_names.push_back(name);

    // remove duplicate entries
    // NOTE: std::unique only removes consecutive duplicated elements, 
    //       so the vector needst to be sorted first
    std::sort(expected_names.begin(), expected_names.end());
    std::vector<std::string>::iterator it = std::unique(expected_names.begin(), expected_names.end());  
    expected_names.resize(std::distance(expected_names.begin(), it));

    // create local kdb
    kdb_subset = Variables.subset(pattern, true);
    EXPECT_EQ(kdb_subset->count(), expected_names.size());
    EXPECT_EQ(kdb_subset->get_names(), expected_names);

    // modify an element of the local KDB and check if the 
    // corresponding element of the global KDB didn't changed
    std::string name = "ACAF";
    Variable var = Variables.get(name);
    int nb_periods = Variables.get_nb_periods();
    std::string lec = "10 + t";
    Variable updated_var;
    updated_var.reserve(nb_periods);
    for (int p = 0; p < nb_periods; p++) updated_var.push_back(10. + p);
    kdb_subset->update(name, lec);
    EXPECT_EQ(kdb_subset->get(name), updated_var);
    EXPECT_EQ(Variables.get(name), var);

    // add an element to the local KDB and check if it has not 
    // been added to the global KDB
    std::string new_name = "NEW_VARIABLE";
    Variable new_var;
    new_var.reserve(nb_periods);
    for (int p = 0; p < nb_periods; p++) new_var.push_back(10. + p);
    kdb_subset->add(new_name, new_var);
    EXPECT_TRUE(kdb_subset->contains(new_name));
    EXPECT_EQ(kdb_subset->get(new_name), new_var);
    EXPECT_FALSE(Variables.contains(new_name));

    // rename an element in the local KDB and check if the 
    // corresponding element has not been renamed in the global KDB
    name = "ACAG";
    new_name = "VARIABLE_NEW";
    kdb_subset->rename(name, new_name);
    EXPECT_TRUE(kdb_subset->contains(new_name));
    EXPECT_FALSE(Variables.contains(new_name));

    // delete an element from the local KDB and check if it has not 
    // been deleted from the global KDB
    name = "AOUC";
    kdb_subset->remove(name);
    EXPECT_FALSE(kdb_subset->contains(name));
    EXPECT_TRUE(Variables.contains(name));

    // delete local kdb
    delete kdb_subset;
    EXPECT_EQ(Variables.count(), nb_total_variables);

    // wrong pattern
    pattern = "anjfks";
    EXPECT_THROW(Variables.subset(pattern, true), std::runtime_error);
}

TEST_F(KDBVariablesTest, CopyFrom)
{
    std::string filename = input_test_dir + prefix_filename + "fun.var";
    int expected_nb_comments = Variables.count();
    std::vector<std::string> v_expected_names;

    // Copy entire file
    Variables.clear();
    Variables.copy_from(filename, "", "", "*");
    EXPECT_EQ(Variables.count(), expected_nb_comments); 
    EXPECT_NEAR(Variables.get_var("ACAF", "1992Y1"), 30.159, 1e-3);
    EXPECT_NEAR(Variables.get_var("ACAG", "1992Y1"), -40.286, 1e-3);

    // copy partial WS (92-93) on an existing one
    Variables.clear();
    Variables.set_sample("1990Y1", "2000Y1");
    Variables.add("ACAF", "t");
    Variables.copy_from(filename, "1992Y1", "1993Y1", "ACAF ACAG");
    EXPECT_DOUBLE_EQ(Variables.get_var("ACAF", "1991Y1"), 1.0); 
    EXPECT_NEAR(Variables.get_var("ACAF", "1992Y1"), 30.159, 1e-3); 
    EXPECT_DOUBLE_EQ(Variables.get_var("ACAG", "1991Y1"), IODE_NAN);
    EXPECT_NEAR(Variables.get_var("ACAG", "1992Y1"), -40.286, 1e-3);

    // copy partial WS on an existing one without specifying the sample
    Variables.clear();
    Variables.set_sample("1990Y1", "2000Y1");
    Variables.copy_from(filename, "", "", "ACAF ACAG");
    EXPECT_NEAR(Variables.get_var("ACAF", "1992Y1"), 30.159, 1e-3);
    EXPECT_NEAR(Variables.get_var("ACAG", "1992Y1"), -40.286, 1e-3);
}

TEST_F(KDBVariablesTest, Merge)
{
    std::string pattern = "A*";

    // create deep copies kdb
    KDBVariables* kdb0 = Variables.subset(pattern, true);
    KDBVariables* kdb1 = Variables.subset(pattern, true);
    KDBVariables* kdb_to_merge = Variables.subset(pattern, true);

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

    KDBComments kdb_cmt(input_test_dir + "fun.ac");
    KDBEquations kdb_eqs(input_test_dir + "fun.ae");
    KDBIdentities kdb_idt(input_test_dir + "fun.ai");
    KDBLists kdb_lst(input_test_dir + "fun.al");
    KDBScalars kdb_scl(input_test_dir + "fun.as");
    KDBTables kdb_tbl(input_test_dir + "fun.at");
    KDBVariables kdb_var(input_test_dir + "fun.av");

    std::vector<std::string> expected_cmts = { var_name };
    objs_list = Comments.search(var_name);
    EXPECT_EQ(objs_list, expected_cmts);

    std::vector<std::string> expected_eqs = { var_name, "PC", "PIF", "PXS", "QXAB" };
    objs_list = Equations.search(var_name);
    EXPECT_EQ(objs_list, expected_eqs);

    std::vector<std::string> expected_idt = { var_name };
    objs_list = Identities.search(var_name);
    EXPECT_EQ(objs_list, expected_idt);

    std::vector<std::string> expected_lts = { "COPY0", "ENDO0", "TOTAL0", "_RES" };
    objs_list = Lists.search(var_name);
    EXPECT_EQ(objs_list, expected_lts);

    objs_list = Scalars.search(var_name);
    EXPECT_EQ(objs_list.size(), 0);

    std::vector<std::string> expected_tbl = { "ANAPRIX", "MULT1FR", "MULT1RESU", "T1", "T1NIVEAU" };
    objs_list = Tables.search(var_name);
    EXPECT_EQ(objs_list, expected_tbl);

    std::vector<std::string> expected_vars = { var_name };
    objs_list = Variables.search(var_name);
    EXPECT_EQ(objs_list, expected_vars);
}

TEST_F(KDBVariablesTest, Extrapolate)
{
    Variables.clear();
    Variables.set_sample("1995Y1", "2020Y1");
    Variables.add("ACAF", "t");

    // Y := Y[-1], if Y null or NA
    Variables.set_var("ACAF", "2000Y1", IODE_NAN);
    Variables.set_var("ACAF", "2002Y1", IODE_NAN);
    Variables.extrapolate(VariablesInitialization::VAR_INIT_TM1, "2000Y1", "2020Y1");
    EXPECT_DOUBLE_EQ(Variables.get_var("ACAF", "1999Y1"), 4.0);
    EXPECT_DOUBLE_EQ(Variables.get_var("ACAF", "2000Y1"), 4.0);
    EXPECT_DOUBLE_EQ(Variables.get_var("ACAF", "2001Y1"), 6.0);
    EXPECT_DOUBLE_EQ(Variables.get_var("ACAF", "2002Y1"), 6.0);

    // Y := Y[-1], always
    Variables.update("ACAF", "t");
    Variables.set_var("ACAF", "2000Y1", IODE_NAN);
    Variables.set_var("ACAF", "2002Y1", IODE_NAN);
    Variables.extrapolate(VariablesInitialization::VAR_INIT_TM1_A, "2000Y1", "2020Y1");
    EXPECT_DOUBLE_EQ(Variables.get_var("ACAF", "1999Y1"), 4.0);
    EXPECT_DOUBLE_EQ(Variables.get_var("ACAF", "2000Y1"), 4.0);
    EXPECT_DOUBLE_EQ(Variables.get_var("ACAF", "2001Y1"), 4.0);
    EXPECT_DOUBLE_EQ(Variables.get_var("ACAF", "2002Y1"), 4.0);

    // Y := extrapolation, if Y null or NA
    Variables.update("ACAF", "t");
    Variables.set_var("ACAF", "2000Y1", IODE_NAN);
    Variables.set_var("ACAF", "2002Y1", IODE_NAN);
    Variables.extrapolate(VariablesInitialization::VAR_INIT_EXTRA, "2000Y1", "2020Y1");
    EXPECT_DOUBLE_EQ(Variables.get_var("ACAF", "1999Y1"), 4.0);
    EXPECT_DOUBLE_EQ(Variables.get_var("ACAF", "2000Y1"), 5.0);
    EXPECT_DOUBLE_EQ(Variables.get_var("ACAF", "2001Y1"), 6.0);
    EXPECT_DOUBLE_EQ(Variables.get_var("ACAF", "2002Y1"), 7.0);

    // Y := extrapolation, always
    Variables.update("ACAF", "t");
    Variables.set_var("ACAF", "2000Y1", IODE_NAN);
    Variables.set_var("ACAF", "2002Y1", IODE_NAN);
    Variables.extrapolate(VariablesInitialization::VAR_INIT_EXTRA_A, "2000Y1", "2020Y1");
    EXPECT_DOUBLE_EQ(Variables.get_var("ACAF", "1999Y1"), 4.0);
    EXPECT_DOUBLE_EQ(Variables.get_var("ACAF", "2000Y1"), 5.0);
    EXPECT_DOUBLE_EQ(Variables.get_var("ACAF", "2001Y1"), 6.0);
    EXPECT_DOUBLE_EQ(Variables.get_var("ACAF", "2002Y1"), 7.0);

    // Y unchanged
    Variables.update("ACAF", "t");
    Variables.set_var("ACAF", "2000Y1", IODE_NAN);
    Variables.set_var("ACAF", "2002Y1", IODE_NAN);
    Variables.extrapolate(VariablesInitialization::VAR_INIT_ASIS, "2000Y1", "2020Y1");
    EXPECT_DOUBLE_EQ(Variables.get_var("ACAF", "1999Y1"), 4.0);
    EXPECT_DOUBLE_EQ(Variables.get_var("ACAF", "2000Y1"), IODE_NAN);
    EXPECT_DOUBLE_EQ(Variables.get_var("ACAF", "2001Y1"), 6.0);
    EXPECT_DOUBLE_EQ(Variables.get_var("ACAF", "2002Y1"), IODE_NAN);

    // Y := Y[-1], if Y = NA
    Variables.update("ACAF", "t");
    Variables.set_var("ACAF", "2000Y1", IODE_NAN);
    Variables.set_var("ACAF", "2002Y1", IODE_NAN);
    Variables.extrapolate(VariablesInitialization::VAR_INIT_TM1_NA, "2000Y1", "2020Y1");
    EXPECT_DOUBLE_EQ(Variables.get_var("ACAF", "1999Y1"), 4.0);
    EXPECT_DOUBLE_EQ(Variables.get_var("ACAF", "2000Y1"), 4.0);
    EXPECT_DOUBLE_EQ(Variables.get_var("ACAF", "2001Y1"), 6.0);
    EXPECT_DOUBLE_EQ(Variables.get_var("ACAF", "2002Y1"), 6.0);

    // Y := extrapolation, if Y = NA
    Variables.update("ACAF", "t");
    Variables.set_var("ACAF", "2000Y1", IODE_NAN);
    Variables.set_var("ACAF", "2002Y1", IODE_NAN);
    Variables.extrapolate(VariablesInitialization::VAR_INIT_EXTRA_NA, "2000Y1", "2020Y1");
    EXPECT_DOUBLE_EQ(Variables.get_var("ACAF", "1999Y1"), 4.0);
    EXPECT_DOUBLE_EQ(Variables.get_var("ACAF", "2000Y1"), 5.0);
    EXPECT_DOUBLE_EQ(Variables.get_var("ACAF", "2001Y1"), 6.0);
    EXPECT_DOUBLE_EQ(Variables.get_var("ACAF", "2002Y1"), 7.0);
}

TEST_F(KDBVariablesTest, Hash)
{
    std::size_t hash_val = hash_value(Variables);

    // change a name
    Variables.rename("ACAF", "NEW_NAME");
    std::size_t hash_val_modified = hash_value(Variables);
    EXPECT_NE(hash_val, hash_val_modified);
    std::cout << "(rename variable) orignal vs modified hash: " << std::to_string(hash_val) << " vs " << std::to_string(hash_val_modified) << std::endl;

    // modify an entry
    hash_val = hash_val_modified;
    std::string lec = "10 + t";
    Variables.update("NEW_NAME", lec);
    hash_val_modified = hash_value(Variables);
    EXPECT_NE(hash_val, hash_val_modified);
    std::cout << "(modify variable) orignal vs modified hash: " << std::to_string(hash_val) << " vs " << std::to_string(hash_val_modified) << std::endl;

    // remove an entry
    hash_val = hash_val_modified;
    Variables.remove("NEW_NAME");
    hash_val_modified = hash_value(Variables);
    EXPECT_NE(hash_val, hash_val_modified);
    std::cout << "(delete variable) orignal vs modified hash: " << std::to_string(hash_val) << " vs " << std::to_string(hash_val_modified) << std::endl;

    // add an entry
    hash_val = hash_val_modified;
    lec = "20 + t";
    Variables.add("NEW_ENTRY", lec);
    hash_val_modified = hash_value(Variables);
    EXPECT_NE(hash_val, hash_val_modified);   
    std::cout << "(new    variable) orignal vs modified hash: " << std::to_string(hash_val) << " vs " << std::to_string(hash_val_modified) << std::endl; 
}
