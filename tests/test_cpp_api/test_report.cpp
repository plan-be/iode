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


// Simple test of a call to B_ReportLine(). More elaborate commands are tested with B_ReportExec()
TEST_F(ReportTest, REPORT_LINE)
{
    std::string var_filepath = input_test_dir + "fun.av";

    // U_test_suppress_kmsg_msgs();

    std::vector<std::string> commands;
    commands.push_back("$WsClearVar");
    commands.push_back("$WsLoadVar " + var_filepath);

    execute_report_line(commands);
    EXPECT_EQ(Variables.count(), 394);

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
