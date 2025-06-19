#include "report.h"


void execute_report(const std::string& filepath, const std::string& parameters)
{
    B_PrintDest("dummy D");

    std::string filepath_ = check_filepath(filepath, FILE_REP, "execute_report", true);

    // WARNING: we need to put the filepath between double quotes in case the filepath contains withespaces
    std::string args = "\"" + filepath_ + "\" " + parameters;
    int success = B_ReportExec(to_char_array(args));
    W_close();
    if(success != 0)
        throw std::runtime_error("Execution of report " + filepath_ + " " + parameters + " has failed");
}

void execute_report(const std::string& filepath, const std::vector<std::string>& parameters)
{
    std::string s_parameters = join(parameters, " ");
    execute_report(filepath, s_parameters);
}

void execute_report_line(const std::string& commands)
{
    int success = B_ReportLine(to_char_array(commands), 0);
    if(success != 0)
        throw std::runtime_error("Execution of report command(s):\n" + commands + "\n\nhas failed");
}

void execute_report_line(const std::vector<std::string>& commands)
{
    std::string s_commands = join(commands, "\n");
    execute_report_line(s_commands);
}
