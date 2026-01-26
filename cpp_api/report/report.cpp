#include "report.h"


void execute_report(const std::string& filepath, const std::string& parameters)
{
    if(RP_DEBUG == 0)
        RP_STDOUT = 1;      // write the report output to stdout

    std::string filepath_ = check_filepath(filepath, FILE_REP, "execute_report", true);

    // WARNING: we need to put the filepath between double quotes in case the filepath contains withespaces
    std::string args = "\"" + filepath_ + "\" " + parameters;
    int success = B_ReportExec(to_char_array(args));
    W_close();

    RP_STDOUT = 0;          // reset global variable RP_STDOUT to default value (write to file)

    if(success != 0)
    {
        std::string error_msg = "Execution of report " + filepath_ + " " + parameters + " has failed";
        std::string last_error = error_manager.get_last_error();
        if (!last_error.empty())
            error_msg += ":\n" + last_error;
        throw std::runtime_error(error_msg);
    }
}

void execute_report(const std::string& filepath, const std::vector<std::string>& parameters)
{
    std::string s_parameters = join(parameters, " ");
    execute_report(filepath, s_parameters);
}

void execute_report_line(const std::string& commands)
{
    if(RP_DEBUG == 0)
        RP_STDOUT = 1;      // force output of the report line to be written to stdout

    int success = B_ReportLine(to_char_array(commands), 0);

    RP_STDOUT = 0;          // reset global variable RP_STDOUT to default value (write to file)

    if(success != 0)
    {
        std::string error_msg = "Execution of report command(s):\n" + commands + "\n\nhas failed";
        std::string last_error = error_manager.get_last_error();
        if (!last_error.empty())
            error_msg += ":\n" + last_error;
        throw std::runtime_error(error_msg);
    }
}

void execute_report_line(const std::vector<std::string>& commands)
{
    std::string s_commands = join(commands, "\n");
    execute_report_line(s_commands);
}
