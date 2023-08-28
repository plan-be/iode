#include "report_editor.h"


ReportEditor::ReportEditor(QWidget *parent) : IodeTextEditor(parent), output(nullptr)
{
    highlighter = new IodeHighlighter(this->document());
}

ReportEditor::~ReportEditor()
{
}

void ReportEditor::run(const QString& filepath, const QString& parameters, const int nbDecimals, const EnumLang language)
{
    QString msg;
    int success = -1;

    QString currentProjectDir = QDir::currentPath();

    try
    {
        if(output)
        {
            output->setTextColor(Qt::darkBlue);
            output->append("\nRunning report " + filepath + " ...\n");
            output->setTextColor(Qt::black);
        }

        // see function C_ReportExec() from the file sb_rep.c from the old GUI. 
        setPrintDest(filepath, 'D');
        
        // set the number of decimals
        setNbDecimals(nbDecimals);

        // set the language
        setLang(language);

        // updates current directory execution (chdir)
        QDir::setCurrent(QFileInfo(filepath).absolutePath());

        // executes IODE report
        QString args = "\"" + filepath + "\" " + parameters;
        success = B_ReportExec(args.toStdString().data());

        W_close();

        // reset current directory execution to project directory (chdir)
        QDir::setCurrent(currentProjectDir);

        // display error if any
        if(success != 0)
            B_display_last_error();
    }
    catch(const std::exception& e)
    {        
        // reset current directory execution to project directory (chdir)
        QDir::setCurrent(currentProjectDir);

        msg = QString::fromStdString(e.what());
        QMessageBox::warning(nullptr, "WARNING", msg + "\n");
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