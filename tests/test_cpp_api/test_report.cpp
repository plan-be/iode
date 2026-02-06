#include "pch.h"

class ReportTest : public KDBTest, public ::testing::Test
{
protected:
    std::vector<std::string> parameters;

	void SetUp() override 
    {
        // remove trailing backslash 
        std::string input_test_dir_ = input_test_dir;
        input_test_dir_.pop_back();
        std::string output_test_dir_ = output_test_dir;
        output_test_dir_.pop_back();

        // set parameters
        parameters.push_back(input_test_dir_);
        parameters.push_back(input_test_dir_);
        parameters.push_back(output_test_dir_);
    }

	// void TearDown() override {}
};


TEST_F(ReportTest, REPORT_LINE)
{
    std::string cmt_filepath = input_test_dir + "fun.cmt";
    std::string lst_filepath = input_test_dir + "fun.lst";
    std::string tbl_filepath = input_test_dir + "fun.tbl";
    std::string var_filepath = input_test_dir + "fun.var";

    global_ws_cmt->clear();
    global_ws_lst->clear();
    global_ws_tbl->clear();
    global_ws_var->clear();

    // U_test_suppress_kmsg_msgs();

    // ---- execute several report commands at once ----

    std::vector<std::string> commands;
    commands.push_back("$WsClearVar");
    commands.push_back("$WsLoadCmt " + cmt_filepath);
    commands.push_back("$WsLoadLst " + lst_filepath);
    commands.push_back("$WsLoadTbl " + tbl_filepath);
    commands.push_back("$WsLoadVar " + var_filepath);

    execute_report_line(commands);
    EXPECT_EQ(global_ws_cmt->size(), 317);
    EXPECT_EQ(global_ws_lst->size(), 17);
    EXPECT_EQ(global_ws_tbl->size(), 46);
    EXPECT_EQ(global_ws_var->size(), 394);

    // ---- test $settime and $incrtime report commands ----

    execute_report_line("$settime 2000Y1");
    EXPECT_EQ(Period(RP_PER), Period("2000Y1"));

    execute_report_line("$incrtime 2");
    EXPECT_EQ(Period(RP_PER), Period("2002Y1"));

    // ---- test $PrintTbl report command ----

    std::string filename_cpp;
    std::string filename_iode_report;
    std::filesystem::path filepath_cpp;
    std::filesystem::path filepath_iode_report;
    std::filesystem::path output_dir(output_test_dir);

    std::string generalized_sample = "1990Y1:2";
    int nb_dec = 2;

    Table* tbl;
    std::string command;
    KDBTables* subset_tbl = new KDBTables(global_ws_tbl.get(), "A*", false);
    for(const std::string& name : subset_tbl->get_names())
    {
        // **** C++ API ****
        filename_cpp = to_lower(name) + ".csv";
        filepath_cpp = output_dir / filename_cpp;
        if(std::filesystem::exists(filepath_cpp))
            std::filesystem::remove(filepath_cpp);
        tbl = subset_tbl->get_obj_ptr(name);
        ComputedTable computed_tbl(tbl, generalized_sample, nb_dec);
        computed_tbl.print_to_file(filepath_cpp.string(), 'C');
        EXPECT_TRUE(std::filesystem::exists(filepath_cpp));

        // **** IODE report syntax ****
        filename_iode_report = "report_" + filename_cpp;
        filepath_iode_report = output_dir / filename_iode_report;
        if(std::filesystem::exists(filepath_iode_report))
            std::filesystem::remove(filepath_iode_report);
        command = "$PrintDest " + filepath_iode_report.string() + " CSV";
        execute_report_line(command);
        command = "$PrintNbdec " + std::to_string(nb_dec);
        execute_report_line(command);
        command = "$PrintTbl " + generalized_sample + " " + name;
        execute_report_line(command);
        // flush and close the output file
        execute_report_line("$PrintDest");
        // check that file was created
        EXPECT_TRUE(std::filesystem::exists(filepath_iode_report));

        // compare content
        compare_files(filepath_cpp.string(), filepath_iode_report.string());
    }

    // U_test_reset_kmsg_msgs();
}

TEST_F(ReportTest, REPORT_ENGINE)
{
    std::string filepath = report_test_dir + "rep_expand.rep";

    // U_test_suppress_kmsg_msgs();

    // Calls to B_ReportExec(reportfile)
    // Report rep_expand.rep: expand %% {lec}, {$cmd}, {$!cmd} and @fn().
    std::cout << "execute report '" << filepath << "'" << std::endl;
    execute_report(filepath, parameters);
    compare_files(input_test_dir + "rep_expand.a2m", output_test_dir + "rep_expand.ref.a2m");

    // U_test_reset_kmsg_msgs();
}

TEST_F(ReportTest, REPORT_FUNCS)
{
    std::string filepath = report_test_dir + "rep_fns.rep";

    // U_test_suppress_kmsg_msgs();

    // Execution of the report rep_fns.rep
    std::cout << "execute report '" << filepath << "'" << std::endl;
    execute_report(filepath, parameters);
    compare_files(input_test_dir + "rep_fns.a2m", output_test_dir + "rep_fns.ref.a2m");

    // U_test_reset_kmsg_msgs();
}

TEST_F(ReportTest, REPORT_PROC)
{
    std::string filepath = report_test_dir + "rep_proc.rep";

    // U_test_suppress_kmsg_msgs();

    std::cout << "execute report '" << filepath << "'" << std::endl;
    execute_report(filepath, parameters);
    compare_files(input_test_dir + "rep_proc.a2m", output_test_dir + "rep_proc.ref.a2m");

    // U_test_reset_kmsg_msgs();
}
