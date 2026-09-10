#include "pch.h"

class ReportTest : public TestAbstract, public ::testing::Test 
{
protected:
    char* input_test_dir;
    char* output_test_dir;
    char* report_test_dir;
    std::vector<std::string> parameters;

	void SetUp() override 
    {
        // remove trailing backslash 
        std::string _input_test_dir_ = str_input_test_dir;
        _input_test_dir_.pop_back();
        std::string _output_test_dir_ = str_output_test_dir;
        _output_test_dir_.pop_back();
        std::string _report_test_dir_ = str_report_test_dir;
        _report_test_dir_.pop_back();

        input_test_dir = copy_string_to_char(_input_test_dir_);
        output_test_dir = copy_string_to_char(_output_test_dir_);
        report_test_dir = copy_string_to_char(_report_test_dir_);

        // set parameters
        parameters.push_back(_input_test_dir_);
        parameters.push_back(_input_test_dir_);
        parameters.push_back(_output_test_dir_);
    }

	// void TearDown() override {}
};

TEST_F(ReportTest, Tests_B_REP_LINE)
{
    int     rc;
    char    fullfilename[256], cmd[1024];

    std::cout << "Tests B_ReportLine()" << std::endl;
    skip_message(true);

    // Simple test of a call to B_ReportLine(). 
    // More elaborate commands are tested with B_ReportExec()
    sprintf(fullfilename,  "%s/%s", input_test_dir, "fun.av");
    sprintf(cmd, "\n"
                 "$WsClearVar\n"
                 "$WsLoadVar %s\n", fullfilename);

    rc = B_ReportLine(cmd, 1);
    EXPECT_EQ(rc, 0);
    EXPECT_EQ(global_ws_var->size(), 394);

    rc = B_ReportLine("$settime 2000Y1", 0);
    EXPECT_EQ(rc, 0);
    EXPECT_EQ(RP_PER.to_string(), "2000Y1");

    rc = B_ReportLine("$incrtime 2", 0);
    EXPECT_EQ(rc, 0);
    EXPECT_EQ(RP_PER.to_string(), "2002Y1");
}

TEST_F(ReportTest, Tests_B_REP_ENGINE)
{
    int     rc;
    char    cmd[1024];

    std::cout << "Tests B_ReportExec(\"rep_expand.rep\")" << std::endl;
    skip_message(true);

    // Report rep_expand.rep: expand %% {lec}, {$cmd}, {$!cmd} and @fn().
    RP_STDOUT = 1;      // Enable report to stdout for this test
    sprintf(cmd,  "%s/rep_expand.rep %s %s", report_test_dir, input_test_dir, output_test_dir);
    rc = B_ReportExec(cmd);
    EXPECT_EQ(rc, 0);
    compare_files(output_test_dir, "/rep_expand.a2m", output_test_dir, "/rep_expand.ref.a2m");
}

TEST_F(ReportTest, Tests_B_REP_OEM850)
{
    int     rc;
    char    cmd[1024];

    std::cout << "Tests B_ReportExec(\"oem850.rep\")" << std::endl;
    skip_message(true);

    // Execution of the report oem850.rep
    RP_STDOUT = 1;      // Enable report to stdout for this test
    sprintf(cmd,  "%s/oem850.rep %s %s", report_test_dir, input_test_dir, output_test_dir);
    rc = B_ReportExec(cmd);
    EXPECT_EQ(rc, 0);
    compare_files(output_test_dir, "/oem850.a2m", output_test_dir, "/oem850.ref.a2m");
}

TEST_F(ReportTest, Tests_B_REP_FNS)
{
    int     rc;
    char    cmd[1024];

    std::cout << "Tests B_ReportExec(\"rep_fns.rep\")" << std::endl;
    skip_message(true);

    // Execution of the report rep_fns.rep
    RP_STDOUT = 1;      // Enable report to stdout for this test
    sprintf(cmd,  "%s/rep_fns.rep %s %s", report_test_dir, input_test_dir, output_test_dir);
    rc = B_ReportExec(cmd);
    EXPECT_EQ(rc, 0);
    compare_files(output_test_dir, "/rep_fns.a2m", output_test_dir, "/rep_fns.ref.a2m");
}


TEST_F(ReportTest, Tests_B_REP_PROC)
{
    int     rc;
    char    cmd[1024];

    std::cout << "Tests B_ReportExec(\"rep_proc.rep\")" << std::endl;
    skip_message(true);

    // Execution of the report rep_fns.rep
    RP_STDOUT = 1;      // Enable report to stdout for this test
    sprintf(cmd,  "%s/rep_proc.rep %s %s", report_test_dir, input_test_dir, output_test_dir);
    rc = B_ReportExec(cmd);
    EXPECT_EQ(rc, 0);
    compare_files(output_test_dir, "/rep_proc.a2m", output_test_dir, "/rep_proc.ref.a2m");
}

TEST_F(ReportTest, Tests_Data_Exist)
{
    char cmd[1024];

    std::cout << "Testing IODE command $DataExistXxx" << std::endl;

    // Execution of the report rep_fns.rep
    RP_STDOUT = 1;      // Enable report to stdout for this test
    sprintf(cmd,  "%s/rep_data_exist.rep %s %s", report_test_dir, input_test_dir, output_test_dir);
    int rc = B_ReportExec(cmd);
    EXPECT_EQ(rc, 0);
    compare_files(output_test_dir, "/rep_data_exist.a2m", output_test_dir, "/rep_data_exist.ref.a2m");
}

TEST_F(ReportTest, Tests_Print_Eqs)
{
    char cmd[1024];
    skip_message(true);

    std::cout << "Testing IODE command $PrintObjDefEqs" << std::endl;

    B_PrintObjLec("0");
    EXPECT_EQ(B_EQS_LEC, 0);
    B_PrintObjLec("1");
    EXPECT_EQ(B_EQS_LEC, 1);
    B_PrintObjLec("2");
    EXPECT_EQ(B_EQS_LEC, 2);

    B_PrintObjEqsInfos("0");
    EXPECT_EQ(B_EQS_INFOS, 0);
    B_PrintObjEqsInfos("1");
    EXPECT_EQ(B_EQS_INFOS, 1);
    B_PrintObjEqsInfos("2");
    EXPECT_EQ(B_EQS_INFOS, 2);

    B_PrintObjLec("0");
    B_PrintObjEqsInfos("0");

    // Execution of the report rep_fns.rep
    RP_STDOUT = 1;      // Enable report to stdout for this test
    sprintf(cmd,  "%s/rep_print_def_eqs.rep %s %s", report_test_dir, input_test_dir, output_test_dir);
    int rc = B_ReportExec(cmd);
    EXPECT_EQ(rc, 0);
    compare_files(output_test_dir, "/rep_print_def_eqs.a2m", output_test_dir, "/rep_print_def_eqs.ref.a2m");

    skip_message(false);
}

TEST_F(ReportTest, Tests_Print_Tbl)
{
    char cmd[1024];
    skip_message(true);

    std::cout << "Testing IODE command $PrintObjDefTbl" << std::endl;

    B_PrintObjTblTitle("0");
    EXPECT_EQ(B_TABLE_TITLE, 0);
    B_PrintObjTblTitle("1");
    EXPECT_EQ(B_TABLE_TITLE, 1);

    B_PrintObjTblTitle("0");

    // Execution of the report rep_fns.rep
    RP_STDOUT = 1;      // Enable report to stdout for this test
    sprintf(cmd,  "%s/rep_print_def_tbl.rep %s %s", report_test_dir, input_test_dir, output_test_dir);
    int rc = B_ReportExec(cmd);
    EXPECT_EQ(rc, 0);
    compare_files(output_test_dir, "/rep_print_def_tbl.a2m", output_test_dir, "/rep_print_def_tbl.ref.a2m");

    skip_message(false);
}

TEST_F(ReportTest, Tests_Print_Tbl_Var)
{
    char cmd[1024];

    std::cout << "Testing IODE commands $PrintTbl and $PrintVar" << std::endl;

    // Execution of the report rep_fns.rep
    RP_STDOUT = 1;      // Enable report to stdout for this test
    sprintf(cmd,  "%s/rep_print_view_tbl_var.rep %s %s", report_test_dir, input_test_dir, output_test_dir);
    int rc = B_ReportExec(cmd);
    EXPECT_EQ(rc, 0);
    
    compare_files(output_test_dir, "/tbl_c8_1.csv", output_test_dir, "/tbl_c8_1.ref.csv");
    
    // skiping lines 10 and 11 because they contain the path to the file used to load the 
    // Variables workspace and the date when the output file was generated, which can be 
    // different from the reference output file 
    std::set<int> ignore_lines = {10, 11};
    compare_files(output_test_dir, "/tbl_vars.csv", output_test_dir, "/tbl_vars.ref.csv", ignore_lines);
}

TEST_F(ReportTest, REPORT_LINE)
{
    std::string cmt_filepath = str_input_test_dir + "fun.cmt";
    std::string lst_filepath = str_input_test_dir + "fun.lst";
    std::string tbl_filepath = str_input_test_dir + "fun.tbl";
    std::string var_filepath = str_input_test_dir + "fun.var";

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

    std::string command;
    std::shared_ptr<Table> tbl_ptr;
    KDBTablesPtr subset_tbl = global_ws_tbl->get_subset("A*", false);
    for(const std::string& name : subset_tbl->get_names())
    {
        // **** C++ API ****
        filename_cpp = to_lower(name) + ".csv";
        filepath_cpp = output_dir / filename_cpp;
        if(std::filesystem::exists(filepath_cpp))
            std::filesystem::remove(filepath_cpp);
        tbl_ptr = subset_tbl->get_obj_ptr(name);
        ComputedTable computed_tbl(tbl_ptr.get(), generalized_sample, nb_dec);
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
    std::string filepath = str_report_test_dir + "rep_expand.rep";

    // U_test_suppress_kmsg_msgs();

    // Calls to B_ReportExec(reportfile)
    // Report rep_expand.rep: expand %% {lec}, {$cmd}, {$!cmd} and @fn().
    std::cout << "execute report '" << filepath << "'" << std::endl;
    execute_report(filepath, parameters);
    compare_files(str_input_test_dir + "rep_expand.a2m", str_output_test_dir + "rep_expand.ref.a2m");

    // U_test_reset_kmsg_msgs();
}

TEST_F(ReportTest, REPORT_FUNCS)
{
    std::string filepath = str_report_test_dir + "rep_fns.rep";

    // U_test_suppress_kmsg_msgs();

    // Execution of the report rep_fns.rep
    std::cout << "execute report '" << filepath << "'" << std::endl;
    execute_report(filepath, parameters);
    compare_files(str_input_test_dir + "rep_fns.a2m", str_output_test_dir + "rep_fns.ref.a2m");

    // U_test_reset_kmsg_msgs();
}

TEST_F(ReportTest, REPORT_PROC)
{
    std::string filepath = str_report_test_dir + "rep_proc.rep";

    // U_test_suppress_kmsg_msgs();

    std::cout << "execute report '" << filepath << "'" << std::endl;
    execute_report(filepath, parameters);
    compare_files(str_input_test_dir + "rep_proc.a2m", str_output_test_dir + "rep_proc.ref.a2m");

    // U_test_reset_kmsg_msgs();
}
