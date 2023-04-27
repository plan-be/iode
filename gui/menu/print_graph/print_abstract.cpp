#include "print_abstract.h"


void QIodeMenuPrintAbstract::printTableOrVariable(const bool isTable, const QString& names)
{
        bool printToFile = project_settings->value(QIodeMenuFilePrintSetup::KEY_SETTINGS_PRINT_DEST).toBool();

        // ask user to set the output filepath + file type
        if(printToFile)
        {
            QIodePrintFileDialog dialog(project_settings->fileName(), this);
            if(dialog.exec() == QDialog::Rejected)
                return;
        }

        // set output file if any
        QString outputFile;
        if(printToFile)
            outputFile = project_settings->value(QIodePrintFileDialog::KEY_SETTINGS_PRINT_OUTPUT_FILE).toString();
        setPrintDest(outputFile);
        
        // set the number of decimals
        setNbDecimals(wNbDecimals->extractAndVerify());

        // set the language
        setLang((EnumLang) wLanguage->extractAndVerify());

        // set the generalized sample and the list of tables
        QString gsample = wSample->extractAndVerify();

        char** c_names = B_ainit_chk(names.toStdString().data(), NULL, 0);
        int nb_names = SCR_tbl_size((unsigned char**) c_names);
        QStringList args_names;
        for(int i=0; i < nb_names; i++)
            args_names <<  QString(c_names[i]);
        QString args = setTablesOrVariablesArgs(args_names.join(";"), gsample);

        // set tables files
        QStringList files;
        QString file;

        file = wFile2->extractAndVerify();
        if(!file.isEmpty())
            files << file;
        file = wFile3->extractAndVerify();
        if(!file.isEmpty())
            files << file;
        file = wFile4->extractAndVerify();
        if(!file.isEmpty())
            files << file;
        file = wFile5->extractAndVerify();
        if(!file.isEmpty())
            files << file;
        
        setTablesFiles(files);

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

void QIodeMenuPrintAbstract::setPrintDest(const QString& outputFile)
{
    // set output file
    // output_file = "<filepath> <format>"
    // if windows printer -> output_file = ""
    // see C_FilePrintFile() from sb_file.c from the old GUI
    std::string output_file = "";

    if(!outputFile.isEmpty())
    {
        int i_format = project_settings->value(QIodePrintFileDialog::KEY_SETTINGS_PRINT_FORMAT).toInt();
        char format;
        switch(i_format) 
        {
            case 0:
                format = 'A';
                break;
            case 1:  
                format = 'R'; 
                break;
            case 2:  
                format = 'H'; 
                break;
            case 3:  
                format = 'M'; 
                break;
            case 4:  
                format = 'C'; 
                break;
            default: 
                break;
        }

        output_file = outputFile.toStdString() + " " + format;
    }

    int res = B_PrintDest(output_file.data());
    if(res < 0)
        B_display_last_error();
}


void QIodeMenuPrintAbstract::setNbDecimals(const int nbDecimals)
{
    // set number of decimals to print
    std::string nb_decimals = QString::number(nbDecimals).toStdString();
    int res = B_PrintNbDec(nb_decimals.data());
    if(res < 0)
        B_display_last_error();
}

void QIodeMenuPrintAbstract::setLang(const EnumLang lang)
{
    // set lang
    char tlang[2];
    tlang[0] = "EFD"[lang];
    tlang[1] = 0;
    
    int res = B_PrintLang(tlang);
    if(res < 0)
        B_display_last_error();
}

QString QIodeMenuPrintAbstract::setTablesOrVariablesArgs(const QString& names, const QString& gsample)
{
    return gsample + " " + names;
}

void QIodeMenuPrintAbstract::setTablesFiles(const QStringList& files)
{
    int res;
    QString args;
    int i=2;
    foreach(const QString file, files)
    {
        args = QString::number(i) + " " + file;
        res = B_ViewTblFile(args.toStdString().data());
        if(res < 0)
        {
            IodeExceptionFunction error("Function B_ViewTblFile returned an error", "Unknown");
            error.add_argument("file number", QString::number(i).toStdString());
            error.add_argument("filepath", file.toStdString());
            throw error;
        }
        i++;
    }
}