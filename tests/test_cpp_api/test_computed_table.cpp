#include "pch.h"


class ComputedTableTest : public KDBTest, public ::testing::Test
{
protected:
    std::string var_file;
    std::string ref_file;

    void SetUp() override
    {
        global_ws_tbl->load(input_test_dir + "fun.at");

        var_file = input_test_dir + "fun.av";
        global_ws_var->load(var_file);

        // slightly modify variables
        std::string pattern = "Q_F;Q_I;KNFF;KLFHP;TFPFHP_";
        ref_file = input_test_dir + "ref2.av";
        global_ws_var->load(input_test_dir + "fun.av");
        KDBVariables* kdb_ref = new KDBVariables(global_ws_var.get(), pattern, true);
        
        double value;
        for(int t=0; t < kdb_ref->get_nb_periods(); t++)
        {
            for(const std::string& name: kdb_ref->get_names())
            {
                value = kdb_ref->get_var(name, t) * 0.98;
                kdb_ref->set_var(name, t, value);
            }
        }
        kdb_ref->save(ref_file);

        load_reference_kdb(2, ref_file);
    }

    // void TearDown() override {}
};


TEST_F(ComputedTableTest, GSample)
{
    COLS* cls;
    std::string column_name;
    std::vector<std::string> cols_names;
    std::vector<std::string> expected_cols_names;
    char* expand  = "#s";
    int nb_files;
    int dim;
    int res;
    std::string gsample;

    std::vector<std::string> variables = {"Q_I", "Q_F", "Q_I/Q_F", "KNFF[-1]"};
    std::string title = "";
    int nb_lines = 4;
    std::string sample;
    Table* ref_table = new Table(2, "", variables, false, false, false);

    /*
    // simple time series (current workspace) - 5 observations
    gsample = "2000:5";
    nb_files = 1;
    cls = COL_cc((char*) gsample.c_str());
    EXPECT_TRUE(cls != NULL);
    EXPECT_EQ(cls->cl_nb, 5);
    dim = COL_resize(ref_table, cls);
    EXPECT_EQ(dim, 6);
    res = COL_exec(ref_table, 5, cls);
    EXPECT_EQ(res, 0);

    expected_cols_names = {"00", "01", "02", "03", "04"};
    for(int col=1; col < cls->cl_nb; col+=2)
    {
        column_name = std::string(COL_text(&cls->cl_cols[col], expand, nb_files));
        cols_names.push_back(column_name);
    }
    EXPECT_EQ(cols_names.size(), expected_cols_names.size());
    EXPECT_EQ(cols_names, expected_cols_names);

    expected_cols_names.clear();
    cols_names.clear();
    COL_free_cols(cls); 

    // two time series (current workspace) - 5 observations
    gsample = "(2010;2010/2009):5";
    nb_files = 1;
    cls = COL_cc((char*) gsample.c_str());
    EXPECT_TRUE(cls != NULL);
    EXPECT_EQ(cls->cl_nb, 2 * 5);
    dim = COL_resize(ref_table, cls);
    EXPECT_EQ(dim, 11);
    res = COL_exec(ref_table, 5, cls);
    EXPECT_EQ(res, 0);

    expected_cols_names = {"10", "10/09", "11", "11/10", "12", "12/11", "13", "13/12", 
                           "14", "14/13"};
    for(int col=1; col < cls->cl_nb; col+=2)
    {
        column_name = std::string(COL_text(&cls->cl_cols[col], expand, nb_files));
        cols_names.push_back(column_name);
    }
    EXPECT_EQ(cols_names.size(), expected_cols_names.size());
    EXPECT_EQ(cols_names, expected_cols_names);

    expected_cols_names.clear();
    cols_names.clear();
    COL_free_cols(cls);

    // simple time series (one extra file) - 5 observations
    gsample = "2010[1;2]:5";
    nb_files = 2;
    cls = COL_cc((char*) gsample.c_str());
    EXPECT_TRUE(cls != NULL);
    EXPECT_EQ(cls->cl_nb, 2 * 5);
    dim = COL_resize(ref_table, cls);
    EXPECT_EQ(dim, 11);
    res = COL_exec(ref_table, 5, cls);
    EXPECT_EQ(res, 0);

    expected_cols_names = {"10[1]", "10[2]", "11[1]", "11[2]", "12[1]", "12[2]",
                           "13[1]", "13[2]", "14[1]", "14[2]"};
    for(int col=1; col < cls->cl_nb; col+=2)
    {
        column_name = std::string(COL_text(&cls->cl_cols[col], expand, nb_files));
        cols_names.push_back(column_name);
    }
    EXPECT_EQ(cols_names.size(), expected_cols_names.size());
    EXPECT_EQ(cols_names, expected_cols_names);

    expected_cols_names.clear();
    cols_names.clear();
    COL_free_cols(cls);
    */

    // list of patterns (one extra file) - 6 observations
    gsample = "2000;2002;2004//2003;2006[1;2];2008[1+2];2010/2009[1^2]";
    nb_files = 2;
    cls = COL_cc((char*) gsample.c_str());
    EXPECT_TRUE(cls != NULL);
    // "2000;2002;2004//2003" + "2006[1;2]" + "2008[1+2];2010/2009[1^2]"
    EXPECT_EQ(cls->cl_nb, 3 + 2 + 2);
    dim = COL_resize(ref_table, cls);
    EXPECT_EQ(dim, 8);
    res = COL_exec(ref_table, 5, cls);
    EXPECT_EQ(res, 0);

    expected_cols_names = {"00[1]", "02[1]", "04//03[1]", "06[1]", "06[2]", 
                           "08[1+2]", "10/09[1^2]"};
    for(int col=1; col < cls->cl_nb; col+=2)
    {
        column_name = std::string(COL_text(&cls->cl_cols[col], expand, nb_files));
        cols_names.push_back(column_name);
    }
    EXPECT_EQ(cols_names.size(), expected_cols_names.size());
    EXPECT_EQ(cols_names, expected_cols_names); 
    
    expected_cols_names.clear(); 
    cols_names.clear();
    COL_free_cols(cls);
}

TEST_F(ComputedTableTest, BuildFromTable)
{
    std::string gsample;
    std::string table_name = "C8_1";
    std::string title = "Déterminants de l'output potentiel";
    int nb_lines = 4;
    std::string sample;
    std::vector<double> values;
    
    Table* ref_table = global_ws_tbl->get(table_name);

    // simple time series (current workspace) - 10 observations
    gsample = "2000:10";
    sample = "2000Y1:2009Y1";
    ComputedTable table_simple(ref_table, gsample);
    EXPECT_EQ(table_simple.get_nb_lines(), nb_lines);
    EXPECT_EQ(table_simple.get_nb_columns(), 10);
    EXPECT_EQ(table_simple.get_nb_files(), 1);
    EXPECT_EQ(table_simple.get_list_files()[0], var_file);
    EXPECT_EQ(table_simple.get_sample()->to_string(), sample);
    EXPECT_EQ(table_simple.get_nb_files_ops(), 1);
    EXPECT_EQ(table_simple.get_title(), title);
    EXPECT_EQ(table_simple.get_line_name(0), "Output potentiel");
    EXPECT_EQ(table_simple.get_line_name(1), "Stock de capital");
    EXPECT_EQ(table_simple.get_line_name(2), "Intensité de capital");
    EXPECT_EQ(table_simple.get_line_name(3), "Productivité totale des facteurs");
    EXPECT_EQ(table_simple.get_column_name(0), "00");
    EXPECT_EQ(table_simple.get_column_name(1), "01");
    EXPECT_EQ(table_simple.get_column_name(5), "05");
    EXPECT_EQ(table_simple.get_column_name(8), "08");
    EXPECT_EQ(table_simple.get_column_name(9), "09");
    EXPECT_DOUBLE_EQ(table_simple.get_value(0, 0), 5495.21);
    EXPECT_DOUBLE_EQ(table_simple.get_value(0, 5), 6103.63);
    EXPECT_DOUBLE_EQ(table_simple.get_value(3, 0), 0.99);
    EXPECT_DOUBLE_EQ(table_simple.get_value(3, 5), 1.04);

    ComputedTableGraph graph_simple(ref_table, gsample);
    EXPECT_EQ(graph_simple.get_nb_series(), nb_lines);
    EXPECT_EQ(graph_simple.get_series_name(0, 0), "Output potentiel");
    EXPECT_EQ(graph_simple.get_series_name(1, 0), "Stock de capital");
    EXPECT_EQ(graph_simple.get_series_name(2, 0), "Intensité de capital");
    EXPECT_EQ(graph_simple.get_series_name(3, 0), "Productivité totale des facteurs");
    values = graph_simple.get_series_values(0, 0);
    EXPECT_DOUBLE_EQ(round(values[0] * 100) / 100, 5495.21);
    EXPECT_DOUBLE_EQ(round(values[5] * 100) / 100, 6103.63);
    values = graph_simple.get_series_values(3, 0);
    EXPECT_DOUBLE_EQ(round(values[0] * 100) / 100, 0.99);
    EXPECT_DOUBLE_EQ(round(values[5] * 100) / 100, 1.04);

    // two time series (current workspace) - 5 observations
    gsample = "(2010;2010/2009):5";
    sample = "2010Y1:2014Y1";
    ComputedTable table_grt(ref_table, gsample);
    EXPECT_EQ(table_grt.get_nb_lines(), nb_lines);
    EXPECT_EQ(table_grt.get_nb_columns(), 5 * 2);
    EXPECT_EQ(table_grt.get_nb_files(), 1);
    EXPECT_EQ(table_grt.get_list_files()[0], var_file);
    EXPECT_EQ(table_grt.get_sample()->to_string(), sample);
    EXPECT_EQ(table_grt.get_nb_files_ops(), 1);
    EXPECT_EQ(table_grt.get_column_name(0), "10");
    EXPECT_EQ(table_grt.get_column_name(1), "10/09");
    EXPECT_EQ(table_grt.get_column_name(5), "12/11");
    EXPECT_EQ(table_grt.get_column_name(8), "14");
    EXPECT_EQ(table_grt.get_column_name(9), "14/13");
    EXPECT_DOUBLE_EQ(table_grt.get_value(0, 0), 6936.11);
    EXPECT_DOUBLE_EQ(table_grt.get_value(0, 5), 1.65);
    EXPECT_DOUBLE_EQ(table_grt.get_value(3, 0), 1.1);
    EXPECT_DOUBLE_EQ(table_grt.get_value(3, 5), 1.0);

    ComputedTableGraph graph_grt(ref_table, gsample);
    EXPECT_EQ(graph_grt.get_nb_series(), nb_lines);
    EXPECT_EQ(graph_grt.get_series_name(0, 0), "Output potentiel");
    EXPECT_EQ(graph_grt.get_series_name(1, 0), "Stock de capital");
    EXPECT_EQ(graph_grt.get_series_name(2, 0), "Intensité de capital");
    EXPECT_EQ(graph_grt.get_series_name(3, 0), "Productivité totale des facteurs");
    values = graph_grt.get_series_values(0, 0);
    EXPECT_DOUBLE_EQ(round(values[0] * 100) / 100, 1.74);   // 10/09
    EXPECT_DOUBLE_EQ(round(values[2] * 100) / 100, 1.65);   // 12/11
    values = graph_grt.get_series_values(2, 0);
    EXPECT_DOUBLE_EQ(round(values[0] * 100) / 100, -2.17);  // 10/09
    EXPECT_DOUBLE_EQ(round(values[2] * 100) / 100, -1.91);  // 12/11

    // simple time series (one extra file) - 5 observations
    gsample = "2010[1;2]:5";
    sample = "2010Y1:2014Y1";
    ComputedTable table_2_files(ref_table, gsample);
    EXPECT_EQ(table_2_files.get_nb_lines(), nb_lines);
    EXPECT_EQ(table_2_files.get_nb_columns(), 5 * 2);
    EXPECT_EQ(table_2_files.get_nb_files(), 2);
    EXPECT_EQ(table_2_files.get_list_files()[0], var_file);
    EXPECT_EQ(table_2_files.get_list_files()[1], ref_file);
    EXPECT_EQ(table_2_files.get_sample()->to_string(), sample);
    EXPECT_EQ(table_2_files.get_nb_files_ops(), 2);
    EXPECT_EQ(table_2_files.get_column_name(0), "10[1]");
    EXPECT_EQ(table_2_files.get_column_name(1), "10[2]");
    EXPECT_EQ(table_2_files.get_column_name(5), "12[2]");
    EXPECT_EQ(table_2_files.get_column_name(8), "14[1]");
    EXPECT_EQ(table_2_files.get_column_name(9), "14[2]");
    EXPECT_DOUBLE_EQ(table_2_files.get_value(0, 0), 6936.11);
    EXPECT_DOUBLE_EQ(table_2_files.get_value(0, 5), 7018.31);
    EXPECT_DOUBLE_EQ(table_2_files.get_value(3, 0), 1.1);
    EXPECT_DOUBLE_EQ(table_2_files.get_value(3, 5), 1.1);

    ComputedTableGraph graph_2_files(ref_table, gsample);
    EXPECT_EQ(graph_2_files.get_nb_series(), nb_lines * 2);
    EXPECT_EQ(graph_2_files.get_series_name(0, 0), "Output potentiel [1]");
    EXPECT_EQ(graph_2_files.get_series_name(0, 1), "Output potentiel [2]");
    EXPECT_EQ(graph_2_files.get_series_name(1, 0), "Stock de capital [1]");
    EXPECT_EQ(graph_2_files.get_series_name(1, 1), "Stock de capital [2]");
    EXPECT_EQ(graph_2_files.get_series_name(2, 0), "Intensité de capital [1]");
    EXPECT_EQ(graph_2_files.get_series_name(2, 1), "Intensité de capital [2]");
    EXPECT_EQ(graph_2_files.get_series_name(3, 0), "Productivité totale des facteurs [1]");
    EXPECT_EQ(graph_2_files.get_series_name(3, 1), "Productivité totale des facteurs [2]");
    values = graph_2_files.get_series_values(0, 0);
    EXPECT_DOUBLE_EQ(round(values[0] * 100) / 100, 6936.11);
    values = graph_2_files.get_series_values(0, 1);
    EXPECT_DOUBLE_EQ(round(values[2] * 100) / 100, 7018.31);
    values = graph_2_files.get_series_values(3, 0);
    EXPECT_DOUBLE_EQ(round(values[0] * 100) / 100, 1.1);
    values = graph_2_files.get_series_values(3, 1);
    EXPECT_DOUBLE_EQ(round(values[2] * 100) / 100, 1.1);

    // list of patterns (one extra file) - 6 observations
    gsample = "2000;2002;2004//2003;2006[1;2];2008[1+2];2010/2009[1^2]";
    sample = "2000Y1:2010Y1";
    ComputedTable table_multi_patterns(ref_table, gsample);
    EXPECT_EQ(table_multi_patterns.get_nb_lines(), nb_lines);
    EXPECT_EQ(table_multi_patterns.get_nb_columns(), 7);
    EXPECT_EQ(table_multi_patterns.get_nb_files(), 2);
    EXPECT_EQ(table_multi_patterns.get_list_files()[0], var_file);
    EXPECT_EQ(table_multi_patterns.get_list_files()[1], ref_file);
    EXPECT_EQ(table_multi_patterns.get_sample()->to_string(), sample);
    EXPECT_EQ(table_multi_patterns.get_nb_files_ops(), 4);
    EXPECT_EQ(table_multi_patterns.get_column_name(0), "00[1]");
    EXPECT_EQ(table_multi_patterns.get_column_name(1), "02[1]");
    EXPECT_EQ(table_multi_patterns.get_column_name(2), "04//03[1]");
    EXPECT_EQ(table_multi_patterns.get_column_name(3), "06[1]");
    EXPECT_EQ(table_multi_patterns.get_column_name(4), "06[2]");
    EXPECT_EQ(table_multi_patterns.get_column_name(5), "08[1+2]");
    EXPECT_EQ(table_multi_patterns.get_column_name(6), "10/09[1^2]");
    EXPECT_DOUBLE_EQ(table_multi_patterns.get_value(0, 0), 5495.21);
    EXPECT_DOUBLE_EQ(table_multi_patterns.get_value(0, 1), 5748.78);
    EXPECT_DOUBLE_EQ(table_multi_patterns.get_value(0, 2), 1.85);
    EXPECT_DOUBLE_EQ(table_multi_patterns.get_value(0, 3), 6275.47);
    EXPECT_DOUBLE_EQ(table_multi_patterns.get_value(0, 4), 6149.96);
    EXPECT_DOUBLE_EQ(table_multi_patterns.get_value(0, 5), 13177.87);
    EXPECT_DOUBLE_EQ(table_multi_patterns.get_value(0, 6), 1.74);
    EXPECT_DOUBLE_EQ(table_multi_patterns.get_value(3, 0), 0.99);
    EXPECT_DOUBLE_EQ(table_multi_patterns.get_value(3, 1), 1.01);
    EXPECT_DOUBLE_EQ(table_multi_patterns.get_value(3, 2), 1.0);
    EXPECT_DOUBLE_EQ(table_multi_patterns.get_value(3, 3), 1.05);
    EXPECT_DOUBLE_EQ(table_multi_patterns.get_value(3, 4), 1.03);
    EXPECT_DOUBLE_EQ(table_multi_patterns.get_value(3, 5), 2.13);
    EXPECT_DOUBLE_EQ(table_multi_patterns.get_value(3, 6), 1.0);

    ComputedTableGraph graph_multi_patterns(ref_table, gsample);
    EXPECT_EQ(graph_multi_patterns.get_nb_series(), nb_lines * 4);
    EXPECT_EQ(graph_multi_patterns.get_series_name(0, 0), "Output potentiel [1]");
    EXPECT_EQ(graph_multi_patterns.get_series_name(0, 1), "Output potentiel [2]");
    EXPECT_EQ(graph_multi_patterns.get_series_name(1, 0), "Stock de capital [1]");
    EXPECT_EQ(graph_multi_patterns.get_series_name(1, 1), "Stock de capital [2]");
    EXPECT_EQ(graph_multi_patterns.get_series_name(2, 0), "Intensité de capital [1]");
    EXPECT_EQ(graph_multi_patterns.get_series_name(2, 1), "Intensité de capital [2]");
    EXPECT_EQ(graph_multi_patterns.get_series_name(3, 0), "Productivité totale des facteurs [1]");
    EXPECT_EQ(graph_multi_patterns.get_series_name(3, 1), "Productivité totale des facteurs [2]");
    values = graph_multi_patterns.get_series_values(0, 0);
    EXPECT_DOUBLE_EQ(round(values[0] * 100) / 100, 5495.21);
    values = graph_multi_patterns.get_series_values(3, 0);
    EXPECT_DOUBLE_EQ(round(values[0] * 100) / 100, 0.99);

    // wrong number of decimals (must in range [0, 99])
    EXPECT_THROW(ComputedTable(ref_table, gsample, -1), std::invalid_argument);
    EXPECT_THROW(ComputedTable(ref_table, gsample, 100), std::invalid_argument);
}

TEST_F(ComputedTableTest, BuildFromVariables)
{
    std::string gsample;
    std::vector<std::string> variables = {"Q_I", "Q_F", "Q_I/Q_F", "KNFF[-1]"};
    std::string title = "";
    int nb_lines = 4;
    std::string sample;
    std::vector<double> values;

    Table ref_table(2, "", variables, false, false, false);

    // simple time series (current workspace) - 10 observations
    gsample = "2000:10";
    sample = "2000Y1:2009Y1";
    ComputedTable table_simple(&ref_table, gsample);
    EXPECT_EQ(table_simple.get_nb_lines(), nb_lines);
    EXPECT_EQ(table_simple.get_nb_columns(), 10);
    EXPECT_EQ(table_simple.get_nb_files(), 1);
    EXPECT_EQ(table_simple.get_list_files()[0], var_file);
    EXPECT_EQ(table_simple.get_sample()->to_string(), sample);
    EXPECT_EQ(table_simple.get_nb_files_ops(), 1);
    EXPECT_EQ(table_simple.get_title(), title);
    EXPECT_EQ(table_simple.get_line_name(0), "Q_I"); 
    EXPECT_EQ(table_simple.get_line_name(1), "Q_F"); 
    EXPECT_EQ(table_simple.get_line_name(2), "Q_I/Q_F"); 
    EXPECT_EQ(table_simple.get_line_name(3), "KNFF[-1]"); 
    EXPECT_EQ(table_simple.get_column_name(0), "2000");
    EXPECT_EQ(table_simple.get_column_name(1), "2001");
    EXPECT_EQ(table_simple.get_column_name(5), "2005");
    EXPECT_EQ(table_simple.get_column_name(8), "2008");
    EXPECT_EQ(table_simple.get_column_name(9), "2009");
    EXPECT_DOUBLE_EQ(table_simple.get_value(0, 0), 916.5);
    EXPECT_DOUBLE_EQ(table_simple.get_value(0, 5), 996.92);
    EXPECT_DOUBLE_EQ(table_simple.get_value(3, 0), 8083.55);
    EXPECT_DOUBLE_EQ(table_simple.get_value(3, 5), 9468.89);

    ComputedTableGraph graph_simple(&ref_table, gsample);
    EXPECT_EQ(graph_simple.get_nb_series(), nb_lines);
    EXPECT_EQ(graph_simple.get_series_name(0, 0), "Q_I");
    EXPECT_EQ(graph_simple.get_series_name(1, 0), "Q_F"); 
    EXPECT_EQ(graph_simple.get_series_name(2, 0), "Q_I/Q_F"); 
    EXPECT_EQ(graph_simple.get_series_name(3, 0), "KNFF[-1]"); 
    values = graph_simple.get_series_values(0, 0);
    EXPECT_DOUBLE_EQ(round(values[0] * 100) / 100, 916.5);
    EXPECT_DOUBLE_EQ(round(values[5] * 100) / 100, 996.92);
    values = graph_simple.get_series_values(3, 0);
    EXPECT_DOUBLE_EQ(round(values[0] * 100) / 100, 8083.55);
    EXPECT_DOUBLE_EQ(round(values[5] * 100) / 100, 9468.89);

    // two time series (current workspace) - 5 observations
    gsample = "(2010;2010/2009):5";
    sample = "2010Y1:2014Y1";
    ComputedTable table_grt(&ref_table, gsample);
    EXPECT_EQ(table_grt.get_nb_lines(), nb_lines);
    EXPECT_EQ(table_grt.get_nb_columns(), 5 * 2);
    EXPECT_EQ(table_grt.get_nb_files(), 1);
    EXPECT_EQ(table_grt.get_list_files()[0], var_file);
    EXPECT_EQ(table_grt.get_sample()->to_string(), sample);
    EXPECT_EQ(table_grt.get_nb_files_ops(), 1);
    EXPECT_EQ(table_grt.get_column_name(0), "2010");
    EXPECT_EQ(table_grt.get_column_name(1), "2010/2009");
    EXPECT_EQ(table_grt.get_column_name(5), "2012/2011");
    EXPECT_EQ(table_grt.get_column_name(8), "2014");
    EXPECT_EQ(table_grt.get_column_name(9), "2014/2013");
    EXPECT_DOUBLE_EQ(table_grt.get_value(0, 0), 1093.37);
    EXPECT_DOUBLE_EQ(table_grt.get_value(0, 5), 1.94);
    EXPECT_DOUBLE_EQ(table_grt.get_value(3, 0), 11293.85);
    EXPECT_DOUBLE_EQ(table_grt.get_value(3, 5), 1.84);

    ComputedTableGraph graph_grt(&ref_table, gsample);
    EXPECT_EQ(graph_grt.get_nb_series(), nb_lines);
    EXPECT_EQ(graph_grt.get_series_name(0, 0), "Q_I");
    EXPECT_EQ(graph_grt.get_series_name(1, 0), "Q_F");
    EXPECT_EQ(graph_grt.get_series_name(2, 0), "Q_I/Q_F");
    EXPECT_EQ(graph_grt.get_series_name(3, 0), "KNFF[-1]");
    values = graph_grt.get_series_values(0, 0);
    EXPECT_DOUBLE_EQ(round(values[0] * 100) / 100, 0.89);   // 2010/2009
    EXPECT_DOUBLE_EQ(round(values[2] * 100) / 100, 1.94);   // 2012/2011
    values = graph_grt.get_series_values(3, 0);
    EXPECT_DOUBLE_EQ(round(values[0] * 100) / 100, 2.82);   // 2010/2009
    EXPECT_DOUBLE_EQ(round(values[2] * 100) / 100, 1.84);   // 2012/2011

    // simple time series (one extra file) - 5 observations
    gsample = "2010[1;2]:5";
    sample = "2010Y1:2014Y1";
    ComputedTable table_2_files(&ref_table, gsample);
    EXPECT_EQ(table_2_files.get_nb_lines(), nb_lines);
    EXPECT_EQ(table_2_files.get_nb_columns(), 5 * 2);
    EXPECT_EQ(table_2_files.get_nb_files(), 2);
    EXPECT_EQ(table_2_files.get_list_files()[0], var_file);
    EXPECT_EQ(table_2_files.get_list_files()[1], ref_file);
    EXPECT_EQ(table_2_files.get_sample()->to_string(), sample);
    EXPECT_EQ(table_2_files.get_nb_files_ops(), 2);
    EXPECT_EQ(table_2_files.get_column_name(0), "2010[1]");
    EXPECT_EQ(table_2_files.get_column_name(1), "2010[2]");
    EXPECT_EQ(table_2_files.get_column_name(5), "2012[2]");
    EXPECT_EQ(table_2_files.get_column_name(8), "2014[1]");
    EXPECT_EQ(table_2_files.get_column_name(9), "2014[2]");
    EXPECT_DOUBLE_EQ(table_2_files.get_value(0, 0), 1093.37);
    EXPECT_DOUBLE_EQ(table_2_files.get_value(0, 5), 1113.46);
    EXPECT_DOUBLE_EQ(table_2_files.get_value(3, 0), 11293.85);
    EXPECT_DOUBLE_EQ(table_2_files.get_value(3, 5), 11502.05);

    ComputedTableGraph graph_2_files(&ref_table, gsample);
    EXPECT_EQ(graph_2_files.get_nb_series(), nb_lines * 2);
    EXPECT_EQ(graph_2_files.get_series_name(0, 0), "Q_I [1]");
    EXPECT_EQ(graph_2_files.get_series_name(0, 1), "Q_I [2]");
    EXPECT_EQ(graph_2_files.get_series_name(1, 0), "Q_F [1]");
    EXPECT_EQ(graph_2_files.get_series_name(1, 1), "Q_F [2]");
    EXPECT_EQ(graph_2_files.get_series_name(2, 0), "Q_I/Q_F [1]");
    EXPECT_EQ(graph_2_files.get_series_name(2, 1), "Q_I/Q_F [2]");
    EXPECT_EQ(graph_2_files.get_series_name(3, 0), "KNFF[-1] [1]");
    EXPECT_EQ(graph_2_files.get_series_name(3, 1), "KNFF[-1] [2]");
    values = graph_2_files.get_series_values(0, 0);
    EXPECT_DOUBLE_EQ(round(values[0] * 100) / 100, 1093.37);
    values = graph_2_files.get_series_values(0, 1);
    EXPECT_DOUBLE_EQ(round(values[2] * 100) / 100, 1113.46);
    values = graph_2_files.get_series_values(3, 0);
    EXPECT_DOUBLE_EQ(round(values[0] * 100) / 100, 11293.85);
    values = graph_2_files.get_series_values(3, 1);
    EXPECT_DOUBLE_EQ(round(values[2] * 100) / 100, 11502.05);

    // list of patterns (one extra file) - 6 observations
    gsample = "2000;2002;2004//2003;2006[1;2];2008[1+2];2010/2009[1^2]";
    sample = "2000Y1:2010Y1";
    ComputedTable table_multi_patterns(&ref_table, gsample);
    EXPECT_EQ(table_multi_patterns.get_nb_lines(), nb_lines);
    EXPECT_EQ(table_multi_patterns.get_nb_columns(), 7);
    EXPECT_EQ(table_multi_patterns.get_nb_files(), 2);
    EXPECT_EQ(table_multi_patterns.get_list_files()[0], var_file);
    EXPECT_EQ(table_multi_patterns.get_list_files()[1], ref_file);
    EXPECT_EQ(table_multi_patterns.get_sample()->to_string(), sample);
    EXPECT_EQ(table_multi_patterns.get_nb_files_ops(), 4);
    EXPECT_EQ(table_multi_patterns.get_column_name(0), "2000[1]");
    EXPECT_EQ(table_multi_patterns.get_column_name(1), "2002[1]");
    EXPECT_EQ(table_multi_patterns.get_column_name(2), "2004//2003[1]");
    EXPECT_EQ(table_multi_patterns.get_column_name(3), "2006[1]");
    EXPECT_EQ(table_multi_patterns.get_column_name(4), "2006[2]");
    EXPECT_EQ(table_multi_patterns.get_column_name(5), "2008[1+2]");
    EXPECT_EQ(table_multi_patterns.get_column_name(6), "2010/2009[1^2]"); 
    EXPECT_DOUBLE_EQ(table_multi_patterns.get_value(0, 0), 916.50);
    EXPECT_DOUBLE_EQ(table_multi_patterns.get_value(0, 1), 949.49);
    EXPECT_DOUBLE_EQ(table_multi_patterns.get_value(0, 2), 1.54);
    EXPECT_DOUBLE_EQ(table_multi_patterns.get_value(0, 3), 1019.72);
    EXPECT_DOUBLE_EQ(table_multi_patterns.get_value(0, 4), 999.32);
    EXPECT_DOUBLE_EQ(table_multi_patterns.get_value(0, 5), 2111.62);
    EXPECT_DOUBLE_EQ(table_multi_patterns.get_value(0, 6), 0.89);
    EXPECT_DOUBLE_EQ(table_multi_patterns.get_value(3, 0), 8083.55);
    EXPECT_DOUBLE_EQ(table_multi_patterns.get_value(3, 1), 8647.94);
    EXPECT_DOUBLE_EQ(table_multi_patterns.get_value(3, 2), 2.98);
    EXPECT_DOUBLE_EQ(table_multi_patterns.get_value(3, 3), 9822.45);
    EXPECT_DOUBLE_EQ(table_multi_patterns.get_value(3, 4), 9626.00);
    EXPECT_DOUBLE_EQ(table_multi_patterns.get_value(3, 5), 21009.68);
    EXPECT_DOUBLE_EQ(table_multi_patterns.get_value(3, 6), 2.82);

    ComputedTableGraph graph_multi_patterns(&ref_table, gsample);
    EXPECT_EQ(graph_multi_patterns.get_nb_series(), nb_lines * 4);
    EXPECT_EQ(graph_multi_patterns.get_series_name(0, 0), "Q_I [1]");
    EXPECT_EQ(graph_multi_patterns.get_series_name(0, 1), "Q_I [2]");
    EXPECT_EQ(graph_multi_patterns.get_series_name(1, 0), "Q_F [1]");
    EXPECT_EQ(graph_multi_patterns.get_series_name(1, 1), "Q_F [2]");
    EXPECT_EQ(graph_multi_patterns.get_series_name(2, 0), "Q_I/Q_F [1]");
    EXPECT_EQ(graph_multi_patterns.get_series_name(2, 1), "Q_I/Q_F [2]");
    EXPECT_EQ(graph_multi_patterns.get_series_name(3, 0), "KNFF[-1] [1]");
    EXPECT_EQ(graph_multi_patterns.get_series_name(3, 1), "KNFF[-1] [2]");
    values = graph_multi_patterns.get_series_values(0, 0);
    EXPECT_DOUBLE_EQ(round(values[0] * 100) / 100, 916.5);
    values = graph_multi_patterns.get_series_values(3, 0);
    EXPECT_DOUBLE_EQ(round(values[0] * 100) / 100, 8083.55);

    // wrong number of decimals (must in range [0, 99])
    EXPECT_THROW(ComputedTable(&ref_table, gsample, -1), std::invalid_argument);
    EXPECT_THROW(ComputedTable(&ref_table, gsample, 100), std::invalid_argument);
}

TEST_F(ComputedTableTest, EditTable)
{
    double value;
    std::string sample;
    std::string gsample;

    // new table (to be edited)
    std::vector<std::string> lecs;
    lecs.push_back("Q_F");              // row 0
    lecs.push_back("Q_I");              // row 1
    lecs.push_back("Q_F/Q_I");          // row 2
    lecs.push_back("ln(Q_I+Q_F)");      // row 3
    lecs.push_back("KNFF");             // row 4
    lecs.push_back("KNFF[-1]");         // row 5
    lecs.push_back("3+ln(10)");         // row 6
    lecs.push_back("0+KNFF");           // row 7

    Table ref_table(2, "Test table to be edited", lecs, false, false, false);

    // ---- prepare tables ----

    // simple time series (current workspace) - 10 observations
    gsample = "2000:10";
    sample = "2000Y1:2009Y1";
    ComputedTable table_simple(&ref_table, gsample);
    EXPECT_EQ(table_simple.get_nb_columns(), 10);

    // two time series (current workspace) - 5 observations
    gsample = "(2010;2010/2009):5";
    sample = "2010Y1:2014Y1";
    ComputedTable table_grt(&ref_table, gsample);
    EXPECT_EQ(table_grt.get_nb_columns(), 5 * 2);

    // simple time series (one extra file) - 5 observations
    gsample = "2010[1;2]:5";
    sample = "2010Y1:2014Y1";
    ComputedTable table_2_files(&ref_table, gsample);
    EXPECT_EQ(table_2_files.get_nb_columns(), 5 * 2);

    // one time series - 10 observations - values only from external file
    gsample = "2000[2]:10";
    sample = "2000Y1:2009Y1";
    ComputedTable table_file_2(&ref_table, gsample);
    EXPECT_EQ(table_file_2.get_nb_columns(), 5 * 2);

    // ---- check if editable ----

    // RULE 1: A cell cannot be updated if the corresponding column (COL object)
    //         - contains on operation on periods or files
    //         - does not refer to the current workspace

    // 1) two time series (current workspace) - 5 observations
    EXPECT_FALSE(table_grt.is_editable(0, 5));    // "Q_F"
    EXPECT_FALSE(table_grt.is_editable(1, 5));    // "Q_I"
    EXPECT_FALSE(table_grt.is_editable(2, 5));    // "Q_F/Q_I"
    EXPECT_FALSE(table_grt.is_editable(3, 5));    // "ln(Q_I+Q_F)"
    EXPECT_FALSE(table_grt.is_editable(4, 5));    // "KNFF"
    EXPECT_FALSE(table_grt.is_editable(5, 5));    // "KNFF[-1]"

    // 2) simple time series (one extra file) - 5 observations
    EXPECT_FALSE(table_2_files.is_editable(0, 5));    // "Q_F"
    EXPECT_FALSE(table_2_files.is_editable(1, 5));    // "Q_I"
    EXPECT_FALSE(table_2_files.is_editable(2, 5));    // "Q_F/Q_I"
    EXPECT_FALSE(table_2_files.is_editable(3, 5));    // "ln(Q_I+Q_F)"
    EXPECT_FALSE(table_2_files.is_editable(4, 5));    // "KNFF"
    EXPECT_FALSE(table_2_files.is_editable(5, 5));    // "KNFF[-1]"

    // 3) one time series - 10 observations - values only from external file
    EXPECT_FALSE(table_file_2.is_editable(0, 5));    // "Q_F"
    EXPECT_FALSE(table_file_2.is_editable(1, 5));    // "Q_I"
    EXPECT_FALSE(table_file_2.is_editable(2, 5));    // "Q_F/Q_I"
    EXPECT_FALSE(table_file_2.is_editable(3, 5));    // "ln(Q_I+Q_F)"
    EXPECT_FALSE(table_file_2.is_editable(4, 5));    // "KNFF"
    EXPECT_FALSE(table_file_2.is_editable(5, 5));    // "KNFF[-1]"

    // RULE 2: A cell cannot be updated if the corresponding LEC expression from the 
    //         reference table starts with 0+
    // -> simple time series (current workspace) - 10 observations
    EXPECT_FALSE(table_simple.is_editable(7, 5));    // "0+KNFF"

    // RULE 3: A cell cannot be updated if the corresponding LEC expression from the 
    //         reference table does not refer to at least one variable
    // -> simple time series (current workspace) - 10 observations
    EXPECT_FALSE(table_simple.is_editable(6, 5));    // "3+ln(10)"

    // OK to edit
    // -> simple time series (current workspace) - 10 observations
    EXPECT_TRUE(table_simple.is_editable(0, 5));    // "Q_F"
    EXPECT_TRUE(table_simple.is_editable(1, 5));    // "Q_I"
    EXPECT_TRUE(table_simple.is_editable(2, 5));    // "Q_F/Q_I"
    EXPECT_TRUE(table_simple.is_editable(3, 5));    // "ln(Q_I+Q_F)"
    EXPECT_TRUE(table_simple.is_editable(4, 5));    // "KNFF"
    EXPECT_TRUE(table_simple.is_editable(5, 5));    // "KNFF[-1]"

    // ---- check values ----

    // -> simple time series (current workspace) - 10 observations
    //    gsample = "2000:10";
    //    sample = "2000Y1:2009Y1";
    double Q_F = global_ws_var->get_var("Q_F", "2005Y1");
    double Q_I = global_ws_var->get_var("Q_I", "2005Y1");
    double KNFF = global_ws_var->get_var("KNFF", "2005Y1");
    double KNFF_1 = global_ws_var->get_var("KNFF", "2004Y1");

    EXPECT_DOUBLE_EQ(table_simple.get_value(0, 5), round(Q_F * 100.) / 100.);               // "Q_F"
    EXPECT_DOUBLE_EQ(table_simple.get_value(1, 5), round(Q_I * 100.) / 100.);               // "Q_I"
    EXPECT_DOUBLE_EQ(table_simple.get_value(2, 5), round((Q_F/Q_I) * 100.) / 100.);         // "Q_F/Q_I"
    EXPECT_DOUBLE_EQ(table_simple.get_value(3, 5), round(log(Q_I + Q_F) * 100.) /100.);     // "ln(Q_I+Q_F)"
    EXPECT_DOUBLE_EQ(table_simple.get_value(4, 5), round(KNFF * 100.) / 100.);              // "KNFF"
    EXPECT_DOUBLE_EQ(table_simple.get_value(5, 5), round(KNFF_1 * 100.) / 100.);            // "KNFF[-1]"
    EXPECT_DOUBLE_EQ(table_simple.get_value(6, 5), round((3.0 + log(10)) * 100.) / 100.);   // "3+ln(10)"
    EXPECT_DOUBLE_EQ(table_simple.get_value(7, 5), round(KNFF * 100.) / 100.);              // "0+KNFF"

    // ---- modify values ----

    // A) modify Q_F
    table_simple.set_value(0, 5, Q_F * 0.9);

    // propagate
    Q_F *= 0.9;
    value = round(global_ws_var->get_var("Q_F", "2005Y1") * 1e4) / 1e4;
    EXPECT_DOUBLE_EQ(value, round(Q_F * 1e4) / 1e4);

    EXPECT_DOUBLE_EQ(table_simple.get_value(0, 5), round(Q_F * 100.) / 100.);               // "Q_F"
    EXPECT_DOUBLE_EQ(table_simple.get_value(1, 5), round(Q_I * 100.) / 100.);               // "Q_I"
    EXPECT_DOUBLE_EQ(table_simple.get_value(2, 5), round((Q_F/Q_I) * 100.) / 100.);         // "Q_F/Q_I"
    EXPECT_DOUBLE_EQ(table_simple.get_value(3, 5), round(log(Q_I + Q_F) * 100.) /100.);     // "ln(Q_I+Q_F)"
    EXPECT_DOUBLE_EQ(table_simple.get_value(4, 5), round(KNFF * 100.) / 100.);              // "KNFF"
    EXPECT_DOUBLE_EQ(table_simple.get_value(5, 5), round(KNFF_1 * 100.) / 100.);            // "KNFF[-1]"
    EXPECT_DOUBLE_EQ(table_simple.get_value(6, 5), round((3.0 + log(10)) * 100.) / 100.);   // "3+ln(10)"
    EXPECT_DOUBLE_EQ(table_simple.get_value(7, 5), round(KNFF * 100.) / 100.);              // "0+KNFF"

    // 2) modify KNFF
    table_simple.set_value(4, 5, KNFF * 0.9);

    // propagate
    KNFF *= 0.9;
    value = round(global_ws_var->get_var("KNFF", "2005Y1") * 1e4) / 1e4;
    EXPECT_DOUBLE_EQ(value, round(KNFF * 1e4) / 1e4);

    EXPECT_DOUBLE_EQ(table_simple.get_value(0, 5), round(Q_F * 100.) / 100.);               // "Q_F"
    EXPECT_DOUBLE_EQ(table_simple.get_value(1, 5), round(Q_I * 100.) / 100.);               // "Q_I"
    EXPECT_DOUBLE_EQ(table_simple.get_value(2, 5), round((Q_F/Q_I) * 100.) / 100.);         // "Q_F/Q_I"
    EXPECT_DOUBLE_EQ(table_simple.get_value(3, 5), round(log(Q_I + Q_F) * 100.) /100.);     // "ln(Q_I+Q_F)"
    EXPECT_DOUBLE_EQ(table_simple.get_value(4, 5), round(KNFF * 100.) / 100.);              // "KNFF"
    EXPECT_DOUBLE_EQ(table_simple.get_value(5, 5), round(KNFF_1 * 100.) / 100.);            // "KNFF[-1]"
    EXPECT_DOUBLE_EQ(table_simple.get_value(6, 5), round((3.0 + log(10)) * 100.) / 100.);   // "3+ln(10)"
    EXPECT_DOUBLE_EQ(table_simple.get_value(7, 5), round(KNFF * 100.) / 100.);              // "0+KNFF"

    // 3) modify Q_F/Q_I -> remember: only the value of the first variable is modified
    value = (Q_F/Q_I) * 0.9;
    table_simple.set_value(2, 5, value);

    // propagate
    // Q_F/Q_I = new_value 
    // Q_F = new_value * Q_I 
    Q_F = value * Q_I;

    EXPECT_DOUBLE_EQ(table_simple.get_value(0, 5), round(Q_F * 100.) / 100.);               // "Q_F"
    EXPECT_DOUBLE_EQ(table_simple.get_value(1, 5), round(Q_I * 100.) / 100.);               // "Q_I"
    EXPECT_DOUBLE_EQ(table_simple.get_value(2, 5), round((Q_F/Q_I) * 100.) / 100.);         // "Q_F/Q_I"
    EXPECT_DOUBLE_EQ(table_simple.get_value(3, 5), round(log(Q_I + Q_F) * 100.) /100.);     // "ln(Q_I+Q_F)"
    EXPECT_DOUBLE_EQ(table_simple.get_value(4, 5), round(KNFF * 100.) / 100.);              // "KNFF"
    EXPECT_DOUBLE_EQ(table_simple.get_value(5, 5), round(KNFF_1 * 100.) / 100.);            // "KNFF[-1]"
    EXPECT_DOUBLE_EQ(table_simple.get_value(6, 5), round((3.0 + log(10)) * 100.) / 100.);   // "3+ln(10)"
    EXPECT_DOUBLE_EQ(table_simple.get_value(7, 5), round(KNFF * 100.) / 100.);              // "0+KNFF"

    // 4) modify ln(Q_I + Q_F) -> remember: only the value of the first variable is modified
    value = log(Q_I + Q_F) * 1.1;
    table_simple.set_value(3, 5, value);

    // propagate
    // ln(Q_I + Q_F) = new_value 
    // Q_I + Q_F = exp(new_value) 
    // Q_I = exp(new_value) - Q_F
    Q_I = exp(value) - Q_F;

    EXPECT_DOUBLE_EQ(table_simple.get_value(0, 5), round(Q_F * 100.) / 100.);               // "Q_F"
    EXPECT_DOUBLE_EQ(table_simple.get_value(1, 5), round(Q_I * 100.) / 100.);               // "Q_I"
    EXPECT_DOUBLE_EQ(table_simple.get_value(2, 5), round((Q_F/Q_I) * 100.) / 100.);         // "Q_F/Q_I"
    EXPECT_DOUBLE_EQ(table_simple.get_value(3, 5), round(log(Q_I + Q_F) * 100.) /100.);     // "ln(Q_I+Q_F)"
    EXPECT_DOUBLE_EQ(table_simple.get_value(4, 5), round(KNFF * 100.) / 100.);              // "KNFF"
    EXPECT_DOUBLE_EQ(table_simple.get_value(5, 5), round(KNFF_1 * 100.) / 100.);            // "KNFF[-1]"
    EXPECT_DOUBLE_EQ(table_simple.get_value(6, 5), round((3.0 + log(10)) * 100.) / 100.);   // "3+ln(10)"
    EXPECT_DOUBLE_EQ(table_simple.get_value(7, 5), round(KNFF * 100.) / 100.);              // "0+KNFF"

    // 5) trying to modify a cell value for which there is no reference of any variable
    EXPECT_THROW(table_simple.set_value(6, 5, 4.0 + log(10)), std::runtime_error);

    // 6) trying to modify a cell value for which the corresponding formula starts with "0+"
    EXPECT_THROW(table_simple.set_value(7, 5, KNFF * 0.9), std::runtime_error);

    // 7) trying to modify a cell value by passing a NaN value
    EXPECT_THROW(table_simple.set_value(1, 5, IODE_NAN), std::invalid_argument);
}

TEST_F(ComputedTableTest, InitializePrinting)
{
    ComputedTable::initialize_printing("file.html", 'H');
    EXPECT_EQ(ComputedTable::get_destination_file(), "file.html");

    // extension is automatically added based on the format
    ComputedTable::initialize_printing("file", 'C');
    EXPECT_EQ(ComputedTable::get_destination_file(), "file.csv");

    // wrong extension -> if filepath is given with an extension, 
    //                    the argument 'format' is ignored
    ComputedTable::initialize_printing("file.csv", 'R');
    EXPECT_EQ(ComputedTable::get_destination_file(), "file.csv");

    // no extension + no format given -> default to A2M format
    ComputedTable::initialize_printing("file");
    EXPECT_EQ(ComputedTable::get_destination_file(), "file.a2m");

    // invalid format
    EXPECT_THROW(ComputedTable::initialize_printing("file", 'Z'), std::invalid_argument);
}

TEST_F(ComputedTableTest, PrintToFile)
{
    // WARNING: B_PrintTable() resets global_ref_xxx (reference files)

    int res;
    std::string arg;
    std::string gsample;
    std::string table_name = "C8_1";
    std::string title = "Déterminants de l'output potentiel";
    std::vector<std::string> v_titles = { "Output potentiel", 
                                          "Stock de capital", "Intensité de capital", 
                                          "Productivité totale des facteurs" };
    std::vector<std::string> v_lecs = { "Q_F+Q_I", "KNFF[-1]", "KLFHP", "TFPFHP_" };

    Table* ref_table = global_ws_tbl->get(table_name); 

    int i = 0;
    TableLine* line;
    TableCell* cell;

    // divider lines 
    // --- divider line ---
    TableLine line_div = ref_table->divider_line;
    ASSERT_EQ(line_div.cells.size(), 2);
    cell = &(line_div.cells[0]);
    ASSERT_EQ(cell->get_type(), TABLE_CELL_LEC);
    ASSERT_EQ(cell->get_content(), "1");
    cell = &(line_div.cells[1]);
    ASSERT_EQ(cell->get_type(), TABLE_CELL_LEC);
    ASSERT_EQ(cell->get_content(), "");
    // --- title line ---
    line = &(ref_table->lines[i++]);
    cell = &(line->cells[0]);
    ASSERT_EQ(cell->get_content(), title);
    // --- separator line ---
    line = &(ref_table->lines[i++]);
    ASSERT_EQ(line->get_type(), TABLE_LINE_SEP);
    // --- first line ---
    line = &(ref_table->lines[i++]);
    ASSERT_EQ(line->cells.size(), 2);
    cell = &(line->cells[0]);
    ASSERT_EQ(cell->get_type(), TABLE_CELL_STRING);
    ASSERT_EQ(cell->get_content(), "");
    cell = &(line->cells[1]);
    ASSERT_EQ(cell->get_type(), TABLE_CELL_STRING);
    ASSERT_EQ(cell->get_content(), "#s");
    // --- separator line ---
    line = &(ref_table->lines[i++]);
    ASSERT_EQ(line->get_type(), TABLE_LINE_SEP);
    // --- other lines ---
    for (size_t l = 0; l < v_titles.size(); l++)
    {
        line = &(ref_table->lines[i++]);
        ASSERT_EQ(line->cells.size(), 2);

        cell = &(line->cells[0]);
        ASSERT_EQ(cell->get_type(), TABLE_CELL_STRING);
        ASSERT_EQ(cell->get_content(), v_titles[l]);

        cell = &(line->cells[1]);
        ASSERT_EQ(cell->get_type(), TABLE_CELL_LEC);
        ASSERT_EQ(cell->get_content(), v_lecs[l]);
    }

    // simple time series (current workspace) - 10 observations
    gsample = "2000:10";
    ComputedTable table_simple(ref_table, gsample, 4);

    // ---- CSV format ----
    arg = output_test_dir + "c_api_file.csv C";
    res = B_PrintDest(arg.data());
    res = B_PrintNbDec("4");
    arg = gsample + " " + table_name;
    res = B_PrintTbl(to_char_array(arg));
    EXPECT_EQ(res, 0);

    load_reference_kdb(2, ref_file);
    table_simple.print_to_file(output_test_dir + "cpp_api_file.csv", 'C');
    compare_files(output_test_dir + "c_api_file.csv", output_test_dir + "cpp_api_file.csv");
    
    // ---- HTML format ----
    arg = output_test_dir + "c_api_file.html H";
    res = B_PrintDest(arg.data());
    res = B_PrintNbDec("4");
    arg = gsample + " " + table_name;
    res = B_PrintTbl(to_char_array(arg));
    EXPECT_EQ(res, 0);
    load_reference_kdb(2, ref_file);
    table_simple.print_to_file(output_test_dir + "cpp_api_file.html", 'H');
    compare_files(output_test_dir + "c_api_file.html", output_test_dir + "cpp_api_file.html");

    // two time series (current workspace) - 5 observations
    gsample = "(2010;2010/2009):5";
    ComputedTable table_grt(ref_table, gsample, 4);

    // ---- CSV format ----
    arg = output_test_dir + "c_api_file.csv C";
    res = B_PrintDest(arg.data());
    res = B_PrintNbDec("4");
    arg = gsample + " " + table_name;
    res = B_PrintTbl(to_char_array(arg));
    EXPECT_EQ(res, 0);

    load_reference_kdb(2, ref_file);
    table_grt.print_to_file(output_test_dir + "cpp_api_file.csv", 'C');
    compare_files(output_test_dir + "c_api_file.csv", output_test_dir + "cpp_api_file.csv");

    // ---- HTML format ----
    arg = output_test_dir + "c_api_file.html H";
    res = B_PrintDest(arg.data());
    res = B_PrintNbDec("4");
    arg = gsample + " " + table_name;
    res = B_PrintTbl(to_char_array(arg));
    EXPECT_EQ(res, 0);

    load_reference_kdb(2, ref_file);
    table_grt.print_to_file(output_test_dir + "cpp_api_file.html", 'H');
    compare_files(output_test_dir + "c_api_file.html", output_test_dir + "cpp_api_file.html");

    // simple time series (current workspace + one extra file) - 5 observations
    gsample = "2010[1-2]:5";
    ComputedTable table_2_files(ref_table, gsample, 4);

    // ---- CSV format ----
    arg = output_test_dir + "c_api_file.csv C";
    res = B_PrintDest(arg.data());
    res = B_PrintNbDec("4");
    arg = gsample + " " + table_name;
    res = B_PrintTbl(to_char_array(arg));
    EXPECT_EQ(res, 0);

    load_reference_kdb(2, ref_file);
    table_2_files.print_to_file(output_test_dir + "cpp_api_file.csv", 'C');
    compare_files(output_test_dir + "c_api_file.csv", output_test_dir + "cpp_api_file.csv");

    // ---- HTML format ----
    arg = output_test_dir + "c_api_file.html H";
    res = B_PrintDest(arg.data());
    res = B_PrintNbDec("4");
    arg = gsample + " " + table_name;
    res = B_PrintTbl(to_char_array(arg));
    EXPECT_EQ(res, 0);

    load_reference_kdb(2, ref_file);
    table_2_files.print_to_file(output_test_dir + "cpp_api_file.html", 'H');
    compare_files(output_test_dir + "c_api_file.html", output_test_dir + "cpp_api_file.html");

    // ---- binary table files ----

    KDBTables* bin_kdb_tables = new KDBTables(false);
    bin_kdb_tables->load(input_test_dir + "fun.tbl");
    
    Table* bin_ref_table = bin_kdb_tables->get(table_name);

    TableCell* bin_cell;
    
    EXPECT_EQ(ref_table->lines.size(), bin_ref_table->lines.size());
    EXPECT_EQ(ref_table->nb_columns, bin_ref_table->nb_columns);

    // divider lines 
    TableLine bin_line_div = bin_ref_table->divider_line;
    EXPECT_EQ(line_div.get_type(), bin_line_div.get_type());
    ASSERT_EQ(line_div.cells.size(), 2);
    for(int j=0; j<ref_table->nb_columns; j++)
    {
        cell = &(line_div.cells[j]);
        bin_cell = &(bin_line_div.cells[j]);
        EXPECT_EQ(cell->get_type(), bin_cell->get_type());
        EXPECT_EQ(cell->get_content(), bin_cell->get_content());
        EXPECT_EQ(cell->get_attribute(), bin_cell->get_attribute());
    }

    // lines
    for(int i=0; i < ref_table->lines.size(); i++)
    {
        TableLine line = ref_table->lines[i];
        TableLine bin_line = bin_ref_table->lines[i];
        EXPECT_EQ(line.get_type(), bin_line.get_type());
        if(line.get_type() == TABLE_LINE_TITLE)
        {
            cell = &line.cells[0];
            bin_cell = &bin_line.cells[0];
            EXPECT_EQ(cell->get_type(), bin_cell->get_type());
            EXPECT_EQ(cell->get_content(), bin_cell->get_content());
            EXPECT_EQ(cell->get_attribute(), bin_cell->get_attribute());
        }
        else if(line.get_type() == TABLE_LINE_CELL)
        {
            ASSERT_EQ(line.cells.size(), 2);
            for(int j=0; j<ref_table->nb_columns; j++)
            {
                cell = &line.cells[j];
                bin_cell = &bin_line.cells[j];
                if(!bin_cell->is_null())
                    EXPECT_EQ(cell->get_type(), bin_cell->get_type());
                EXPECT_EQ(cell->get_content(), bin_cell->get_content());
                EXPECT_EQ(cell->get_attribute(), bin_cell->get_attribute());
            }
        }
        else
            continue;
    }

    delete ref_table;

    // simple time series (current workspace) - 10 observations
    gsample = "2000:10";
    ComputedTable table_simple_bin(bin_ref_table, gsample, 4);

    // ---- CSV format ----
    arg = output_test_dir + "bin_file.csv C";
    res = B_PrintDest(arg.data());
    res = B_PrintNbDec("4");
    arg = gsample + " " + table_name;
    res = B_PrintTbl(to_char_array(arg));
    EXPECT_EQ(res, 0);

    load_reference_kdb(2, ref_file);
    table_simple_bin.print_to_file(output_test_dir + "bin_cpp_file.csv", 'C');
    compare_files(output_test_dir + "bin_file.csv", output_test_dir + "bin_cpp_file.csv");
    
    // ---- HTML format ----
    arg = output_test_dir + "bin_file.html H";
    res = B_PrintDest(arg.data());
    res = B_PrintNbDec("4");
    arg = gsample + " " + table_name;
    res = B_PrintTbl(to_char_array(arg));
    EXPECT_EQ(res, 0);
    load_reference_kdb(2, ref_file);
    table_simple_bin.print_to_file(output_test_dir + "bin_cpp_file.html", 'H');
    compare_files(output_test_dir + "bin_file.html", output_test_dir + "bin_cpp_file.html");

    delete bin_ref_table;
    delete bin_kdb_tables;

    // ---- titles with special character '#' ----
    
    gsample = "2000:10";
    title = "Test table with special char # in titles";
    std::vector<std::string> variables = {"Q_I"};
    Table test_table(2, title, variables, false, false, false);

    test_table.add_title("title with special char # in #it");
    test_table.add_line_separator();

    TableLine* new_line = test_table.add_line_with_cells();
    TableCell* first_cell = &new_line->cells[0];
    first_cell->set_text("Q_F");
    TableCell* second_cell = &new_line->cells[1];
    second_cell->set_lec("Q_F");

    ComputedTable computed_table_sharp(&test_table, gsample, 4);
    computed_table_sharp.print_to_file(output_test_dir + "cpp_api_sharp.a2m", 'A');
    computed_table_sharp.print_to_file(output_test_dir + "cpp_api_sharp.html", 'H');
    computed_table_sharp.print_to_file(output_test_dir + "cpp_api_sharp.csv", 'C');
}
