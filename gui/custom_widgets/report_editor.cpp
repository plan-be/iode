#include "report_editor.h"


ReportEditor::ReportEditor(QWidget *parent) : TextEditor(parent), output(nullptr)
{
    highlighter = new QIodeHighlighter(this->document());
}

ReportEditor::~ReportEditor()
{
    delete highlighter;
}

void ReportEditor::run(const QString& filepath, const QString& parameters, const int nbDecimals, const EnumLang language)
{
    QString msg;
    int success = -1;

    QString dummyFilepath;
    QString currentProjectDir = QDir::currentPath();

    try
    {
        if(output)
        {
            output->setTextColor(Qt::darkBlue);
            output->append("\nRunning report " + filepath + " ...\n");
            output->setTextColor(Qt::black);
        }

        // Save the current content of the report tab because the user may have 
        // made some modifications in the report editor which are not saved yet.
        QDir reportDir = QFileInfo(filepath).absoluteDir();
        dummyFilepath = reportDir.absoluteFilePath(TMP_FILENAME + ".rep");
        QFile file(dummyFilepath);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QMessageBox::critical(nullptr, "ERROR", file.errorString());
            return;
        }
        QTextStream out(&file);
        out << this->toPlainText() << "\n";
        file.close();

        // see function C_ReportExec() from the file sb_rep.c from the old GUI. 
        setPrintDest(dummyFilepath, 'D');
        
        // set the number of decimals
        setNbDecimals(nbDecimals);

        // set the language
        setLang(language);

        // updates current directory execution (chdir)
        QDir::setCurrent(reportDir.absolutePath());

        // executes IODE report
        QString args = dummyFilepath + " " + parameters;
        success = B_ReportExec(args.toStdString().data());

        W_close();

        // reset current directory execution to project directory (chdir)
        QDir::setCurrent(currentProjectDir);

        // removes the temporary file
        file.remove();

        // display error if any
        if(success != 0)
            B_display_last_error();
    }
    catch(const std::exception& e)
    {
        // deletes dummy file if any
        QFile file(dummyFilepath);
        if(file.exists())
            file.remove();
        
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