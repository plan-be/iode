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
    kmsg_toggle(1);

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
    kmsg_toggle(1);

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
    kmsg_toggle(1);

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
    kmsg_toggle(1);

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
    kmsg_toggle(1);

    // Execution of the report rep_fns.rep
    RP_STDOUT = 1;      // Enable report to stdout for this test
    sprintf(cmd,  "%s/rep_proc.rep %s %s", report_test_dir, input_test_dir, output_test_dir);
    rc = B_ReportExec(cmd);
    EXPECT_EQ(rc, 0);
    compare_files(output_test_dir, "/rep_proc.a2m", output_test_dir, "/rep_proc.ref.a2m");
}
