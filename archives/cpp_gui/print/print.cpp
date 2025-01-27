#include "print.h"


static void printTblOrVarToFile(const bool isTable, const QString& outputFile, const QChar& format, 
    const QString& names, const QString& gsample, const QStringList& files, const int nbDecimals, 
    const TableLang language)
{
    QString args;
    QStringList args_names;

    try
    {
        // set output file
        setPrintDest(outputFile, format);
        
        // set the number of decimals
        setNbDecimals(nbDecimals);

        // set the language
        setLang(language);

        // set the generalized sample and the list of tables
        char** c_names = B_ainit_chk(names.toStdString().data(), NULL, 0);
        int nb_names = SCR_tbl_size((unsigned char**) c_names);
        for(int i=0; i < nb_names; i++)
            args_names <<  QString(c_names[i]);
        args = gsample + " " + args_names.join(";");

        // set tables files
        setTablesFiles(files);
    }
    catch(const std::exception& e)
    {
        QMessageBox::warning(nullptr, "WARNING", QString::fromStdString(e.what()));
    }

    // print the tables or variables
    int res;
    std::string string_args = args.toStdString();
    if(isTable)
        res = B_PrintTbl(string_args.data());
    else
        res = B_PrintVar(string_args.data());
    W_close();
    if(res < 0)
        B_display_last_error();
    
    QString obj_type = isTable ? "tables" : "variables";
    QMessageBox::information(nullptr, "SUCCESS", "succedeed to export " + obj_type + " " 
        + args_names.join(", ") + " to the file " + outputFile);

}

static void printTblOrVar(const bool isTable, QPrintPreviewDialog& dialog, QTextDocument& document, 
    const QString& names, const QString& gsample, const QStringList& files, const int nbDecimals, 
    const TableLang language)
{
    QString args;

    try
    {
        // set output file
        // Note: here the table is first saved in a temporary HTML file to be then 
        // loaded by a QTextDocument object (see below)
        QString outputFile = TMP_FILENAME + ".htm";
        setPrintDest(outputFile, 'H');
        
        // set the number of decimals
        setNbDecimals(nbDecimals);

        // set the language
        setLang(language);

        // set the generalized sample and the list of tables
        char** c_names = B_ainit_chk(names.toStdString().data(), NULL, 0);
        int nb_names = SCR_tbl_size((unsigned char**) c_names);
        QStringList args_names;
        for(int i=0; i < nb_names; i++)
            args_names <<  QString(c_names[i]);
        args = gsample + " " + args_names.join(";");

        // set tables files
        setTablesFiles(files);
    }
    catch(const std::exception& e)
    {
        QMessageBox::warning(nullptr, "WARNING", QString::fromStdString(e.what()));
    }

    // print the tables or variables
    int res;
    std::string string_args = args.toStdString();
    if(isTable)
        res = B_PrintTbl(string_args.data());
    else
        res = B_PrintVar(string_args.data());
    W_close();
    if(res < 0)
        B_display_last_error();
    
    try
    {
        document.clear();
        
        // load the content of the generated HTML file and then removed the file
        QFile file(TMP_FILENAME + ".htm");
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QMessageBox::critical(nullptr, "ERROR", file.errorString());
            file.remove();
            return;
        }
        QTextStream in(&file);
        document.setHtml(in.readAll());
        file.remove();

        // print the document
        dialog.exec(); 
    }
    catch(const std::exception& e)
    {
        QMessageBox::warning(nullptr, "WARNING", QString::fromStdString(e.what()));
    }
}

void printTableToFile(const QString& outputFile, const QChar& format, const QString& names, 
    const QString& gsample, const QStringList& files, const int nbDecimals, const TableLang language)
{
    printTblOrVarToFile(true, outputFile, format, names, gsample, files, nbDecimals, language);
}

void printTable(QPrintPreviewDialog& dialog, QTextDocument& document, 
    const QString& names, const QString& gsample, const QStringList& files, 
    const int nbDecimals, const TableLang language)
{
    printTblOrVar(true, dialog, document, names, gsample, files, nbDecimals, language);
}

void printVariableToFile(const QString& outputFile, const QChar& format, const QString& names, 
    const QString& gsample, const QStringList& files, const int nbDecimals, const TableLang language)
{
    printTblOrVarToFile(false, outputFile, format, names, gsample, files, nbDecimals, language);
}

void printVariable(QPrintPreviewDialog& dialog, QTextDocument& document, 
    const QString& names, const QString& gsample, const QStringList& files, 
    const int nbDecimals, const TableLang language)
{
    printTblOrVar(false, dialog, document, names, gsample, files, nbDecimals, language);
}
