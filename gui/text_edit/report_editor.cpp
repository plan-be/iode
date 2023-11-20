#include "report_editor.h"


IodeReportEditor::IodeReportEditor(QWidget *parent) : IodeTextEditor(parent), output(nullptr)
{
    highlighter = new IodeHighlighter(this->document());
}

IodeReportEditor::~IodeReportEditor()
{
}

void IodeReportEditor::run(const QString& filepath, const QString& parameters, const int nbDecimals, const EnumLang language)
{
    QString msg;
    bool success;

    QString currentProjectDir = QDir::currentPath();

    try
    {
        if(output)
        {
            output->setTextColor(Qt::darkBlue);
            output->append("\nRunning report " + filepath + " ...\n");
            output->setTextColor(Qt::black);
        }

        // set the number of decimals
        setNbDecimals(nbDecimals);

        // set the language
        setLang(language);

        // updates current directory execution (chdir)
        QDir::setCurrent(QFileInfo(filepath).absolutePath());

        // executes IODE report
        execute_report(filepath.toLocal8Bit().toStdString(), parameters.toLocal8Bit().toStdString());

        // reset current directory execution to project directory (chdir)
        QDir::setCurrent(currentProjectDir);

        success = true;
    }
    catch(const std::exception& e)
    {        
        // reset current directory execution to project directory (chdir)
        QDir::setCurrent(currentProjectDir);

        success = false;

        msg = QString::fromStdString(e.what());
        QMessageBox::warning(nullptr, "WARNING", msg + "\n");
    }

    if(output)
    {
        if(success)
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