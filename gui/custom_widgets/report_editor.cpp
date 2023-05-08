#include "report_editor.h"


ReportEditor::ReportEditor(QWidget *parent) : TextEditor(parent), output(nullptr)
{
    highlighter = new QIodeHighlighter(this->document());
}

ReportEditor::~ReportEditor()
{
    delete highlighter;
}

void ReportEditor::run(const QString& filepath)
{
    QString msg;
    int success = -1;

    try
    {
        if(output)
        {
            output->setTextColor(Qt::darkBlue);
            output->append("\nRunning report " + filepath + " ...\n");
            output->setTextColor(Qt::black);
        }
        // We do not use B_ReportExec() because the user may have made some 
        // modifications in the report editor which are not yet saved in the 
        // report file
        success = B_ReportLine(this->toPlainText().toStdString().data());
        if(success != 0)
            B_display_last_error();
    }
    catch(const std::exception& e)
    {
        msg = QString::fromStdString(e.what());
        QMessageBox::warning(get_main_window_ptr(), "Warning", msg + "\n");
    }

    if(output)
    {
        if(success == 0)
        {
            output->setTextColor(Qt::darkGreen);
            msg = "Successful";
        }
        else
        {
            output->setTextColor(Qt::red);
            msg = "Failed";
        }
        output->append("\nExecution of report " + filepath + ": " + msg + "\n");
        output->setTextColor(Qt::black);
    }
}