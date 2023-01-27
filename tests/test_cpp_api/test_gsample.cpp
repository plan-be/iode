#include "pch.h"


class GSampleTest : public KDBTest, public ::testing::Test
{
protected:
    KDBTables kdb;
    std::string var_file;
    std::string ref_file;

    void SetUp() override
    {
        load_global_kdb(I_TABLES, input_test_dir + "fun.tbl");

        var_file = input_test_dir + "fun.var";
        // C: -> c:
        var_file[0] = tolower(var_file[0]);
        load_global_kdb(I_VARIABLES, var_file);

        ref_file = input_test_dir + "ref.av";
        // slightly modify variables
        double value;
        KDBVariables kdb_ref(EnumIodeKDBType::KDB_LOCAL, "Q_F;Q_I;KNFF;KLFHP;TFPFHP_");
        for(int t=0; t < kdb_ref.get_nb_periods(); t++)
        {
            for(const std::string& name: kdb_ref.get_names())
            {
                value = kdb_ref.get_var(name, t) * 0.98;
                kdb_ref.set_var(name, t, value);
            }
        }
        kdb_ref.dump(ref_file);

        // C: -> c:
        ref_file[0] = tolower(ref_file[0]);
        load_reference_kdb(2, I_VARIABLES_FILE, ref_file);
    }

    // void TearDown() override {}
};


TEST_F(GSampleTest, BuildFromTable)
{
    std::string gsample;
    std::string table_name = "C8_1";
    std::string title = "Déterminants de l'output potentiel";
    int nb_lines = 4;
    std::string sample;

    // simple time series (current workspace) - 10 obsversations
    gsample = "2000:10";
    sample = "2000Y1:2009Y1";
    GSampleTable table_simple(table_name, gsample);
    EXPECT_EQ(table_simple.get_nb_lines(), nb_lines);
    EXPECT_EQ(table_simple.get_nb_columns(), 10);
    EXPECT_EQ(table_simple.get_nb_files(), 1);
    EXPECT_EQ(table_simple.get_list_files()[0], var_file);
    EXPECT_EQ(table_simple.get_sample()->to_string(), sample);
    EXPECT_EQ(table_simple.get_title(), title);
    EXPECT_EQ(table_simple.get_line_name(0), "Output potentiel");
    EXPECT_EQ(table_simple.get_line_name(1), "Stock de capital");
    EXPECT_EQ(table_simple.get_line_name(2), "Intensité de capital");
    EXPECT_EQ(table_simple.get_line_name(3), "Productivité totale des facteurs");
    EXPECT_EQ(table_simple.get_column_name(0), "00");
    EXPECT_EQ(table_simple.get_column_name(1), "01");
    EXPECT_EQ(table_simple.get_column_name(8), "08");
    EXPECT_EQ(table_simple.get_column_name(9), "09");
    EXPECT_DOUBLE_EQ(table_simple.get_value(0, 0), 5495.21);
    EXPECT_DOUBLE_EQ(table_simple.get_value(0, 5), 6103.63);
    EXPECT_DOUBLE_EQ(table_simple.get_value(3, 0), 0.99);
    EXPECT_DOUBLE_EQ(table_simple.get_value(3, 5), 1.04);

    // two time series (current workspace) - 5 obsversations
    gsample = "(2010;2010/2009):5";
    sample = "2010Y1:2014Y1";
    GSampleTable table_grt(table_name, gsample);
    EXPECT_EQ(table_grt.get_nb_lines(), nb_lines);
    EXPECT_EQ(table_grt.get_nb_columns(), 5 * 2);
    EXPECT_EQ(table_grt.get_nb_files(), 1);
    EXPECT_EQ(table_grt.get_list_files()[0], var_file);
    EXPECT_EQ(table_grt.get_sample()->to_string(), sample);
    EXPECT_EQ(table_grt.get_column_name(0), "10");
    EXPECT_EQ(table_grt.get_column_name(1), "10/09");
    EXPECT_EQ(table_grt.get_column_name(8), "14");
    EXPECT_EQ(table_grt.get_column_name(9), "14/13");
    EXPECT_DOUBLE_EQ(table_grt.get_value(0, 0), 6936.11);
    EXPECT_DOUBLE_EQ(table_grt.get_value(0, 5), 1.65);
    EXPECT_DOUBLE_EQ(table_grt.get_value(3, 0), 1.1);
    EXPECT_DOUBLE_EQ(table_grt.get_value(3, 5), 1.0);

    // simple time series (one extra file) - 5 obsversations
    gsample = "2010[1;2]:5";
    sample = "2010Y1:2014Y1";
    GSampleTable table_2_files(table_name, gsample);
    EXPECT_EQ(table_2_files.get_nb_lines(), nb_lines);
    EXPECT_EQ(table_2_files.get_nb_columns(), 5 * 2);
    EXPECT_EQ(table_2_files.get_nb_files(), 2);
    EXPECT_EQ(table_2_files.get_list_files()[0], var_file);
    EXPECT_EQ(table_2_files.get_list_files()[1], ref_file);
    EXPECT_EQ(table_2_files.get_sample()->to_string(), sample);
    EXPECT_EQ(table_2_files.get_column_name(0), "10[1]");
    EXPECT_EQ(table_2_files.get_column_name(1), "10[2]");
    EXPECT_EQ(table_2_files.get_column_name(8), "14[1]");
    EXPECT_EQ(table_2_files.get_column_name(9), "14[2]");
    EXPECT_DOUBLE_EQ(table_2_files.get_value(0, 0), 6936.11);
    EXPECT_DOUBLE_EQ(table_2_files.get_value(0, 5), 7018.31);
    EXPECT_DOUBLE_EQ(table_2_files.get_value(3, 0), 1.1);
    EXPECT_DOUBLE_EQ(table_2_files.get_value(3, 5), 1.1);
}

TEST_F(GSampleTest, BuildFromVariables)
{
    std::string gsample;
    std::string table_name = "_GSAMPLE_";
    std::string variables_list = "Q_I;Q_F;Q_I/Q_F;KNFF[-1]";
    std::string title = "";
    int nb_lines = 4;
    std::string sample;

    kdb.add(table_name, 2, "", variables_list, false, false, false);

    // simple time series (current workspace) - 10 obsversations
    gsample = "2000:10";
    sample = "2000Y1:2009Y1";
    GSampleTable table_simple(table_name, gsample);
    EXPECT_EQ(table_simple.get_nb_lines(), nb_lines);
    EXPECT_EQ(table_simple.get_nb_columns(), 10);
    EXPECT_EQ(table_simple.get_nb_files(), 1);
    EXPECT_EQ(table_simple.get_list_files()[0], var_file);
    EXPECT_EQ(table_simple.get_sample()->to_string(), sample);
    EXPECT_EQ(table_simple.get_title(), title);
    EXPECT_EQ(table_simple.get_line_name(0), "Q_I");
    EXPECT_EQ(table_simple.get_line_name(1), "Q_F");
    EXPECT_EQ(table_simple.get_line_name(2), "Q_I/Q_F");
    EXPECT_EQ(table_simple.get_line_name(3), "KNFF[-1]");
    EXPECT_EQ(table_simple.get_column_name(0), "2000");
    EXPECT_EQ(table_simple.get_column_name(1), "2001");
    EXPECT_EQ(table_simple.get_column_name(8), "2008");
    EXPECT_EQ(table_simple.get_column_name(9), "2009");
    EXPECT_DOUBLE_EQ(table_simple.get_value(0, 0), 916.5);
    EXPECT_DOUBLE_EQ(table_simple.get_value(0, 5), 996.92);
    EXPECT_DOUBLE_EQ(table_simple.get_value(3, 0), 8083.55);
    EXPECT_DOUBLE_EQ(table_simple.get_value(3, 5), 9468.89);

    // two time series (current workspace) - 5 obsversations
    gsample = "(2010;2010/2009):5";
    sample = "2010Y1:2014Y1";
    GSampleTable table_grt(table_name, gsample);
    EXPECT_EQ(table_grt.get_nb_lines(), nb_lines);
    EXPECT_EQ(table_grt.get_nb_columns(), 5 * 2);
    EXPECT_EQ(table_grt.get_nb_files(), 1);
    EXPECT_EQ(table_grt.get_list_files()[0], var_file);
    EXPECT_EQ(table_grt.get_sample()->to_string(), sample);
    EXPECT_EQ(table_grt.get_column_name(0), "2010");
    EXPECT_EQ(table_grt.get_column_name(1), "2010/2009");
    EXPECT_EQ(table_grt.get_column_name(8), "2014");
    EXPECT_EQ(table_grt.get_column_name(9), "2014/2013");
    EXPECT_DOUBLE_EQ(table_grt.get_value(0, 0), 1093.37);
    EXPECT_DOUBLE_EQ(table_grt.get_value(0, 5), 1.94);
    EXPECT_DOUBLE_EQ(table_grt.get_value(3, 0), 11293.85);
    EXPECT_DOUBLE_EQ(table_grt.get_value(3, 5), 1.84);

    // simple time series (one extra file) - 5 obsversations
    gsample = "2010[1;2]:5";
    sample = "2010Y1:2014Y1";
    GSampleTable table_2_files(table_name, gsample);
    EXPECT_EQ(table_2_files.get_nb_lines(), nb_lines);
    EXPECT_EQ(table_2_files.get_nb_columns(), 5 * 2);
    EXPECT_EQ(table_2_files.get_nb_files(), 2);
    EXPECT_EQ(table_2_files.get_list_files()[0], var_file);
    EXPECT_EQ(table_2_files.get_list_files()[1], ref_file);
    EXPECT_EQ(table_2_files.get_sample()->to_string(), sample);
    EXPECT_EQ(table_2_files.get_column_name(0), "2010[1]");
    EXPECT_EQ(table_2_files.get_column_name(1), "2010[2]");
    EXPECT_EQ(table_2_files.get_column_name(8), "2014[1]");
    EXPECT_EQ(table_2_files.get_column_name(9), "2014[2]");
    EXPECT_DOUBLE_EQ(table_2_files.get_value(0, 0), 1093.37);
    EXPECT_DOUBLE_EQ(table_2_files.get_value(0, 5), 1113.46);
    EXPECT_DOUBLE_EQ(table_2_files.get_value(3, 0), 11293.85);
    EXPECT_DOUBLE_EQ(table_2_files.get_value(3, 5), 11502.05);

    kdb.remove(table_name);
}