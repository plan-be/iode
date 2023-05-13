#include "print_abstract.h"


void QIodeMenuPrintAbstract::printTableOrVariable(const bool isTable, const QString& names)
{
    QSettings* project_settings = QIodeProjectSettings::getProjectSettings();
    bool printToFile = project_settings->value(QIodeMenuFilePrintSetup::KEY_SETTINGS_PRINT_DEST).toBool();

    QString outputFile;
    QChar format;
    if(printToFile)
    {
        // ask the user to set the output file and format
        QIodePrintFileDialog dialog(this);
        if(dialog.exec() == QDialog::Rejected)
            return;

        // extract the output file
        outputFile = project_settings->value(QIodePrintFileDialog::KEY_SETTINGS_PRINT_OUTPUT_FILE).toString();
    
        // extract the format of the output file
        format = QIodePrintFileDialog::getFormat(project_settings);
    }

    // extract the number of decimals
    int NbDecimals = wNbDecimals->extractAndVerify();

    // extract the language
    EnumLang lang = (EnumLang) wLanguage->extractAndVerify();

    // extract the generalized sample and the list of tables
    QString gsample = wSample->extractAndVerify();

    // extract tables files
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

    if(printToFile)
    {
        if(isTable)
            printTableToFile(outputFile, format, names, gsample, files, NbDecimals, lang);
        else
            printVariableToFile(outputFile, format, names, gsample, files, NbDecimals, lang);
    }
    else
    {
        QPrintPreviewDialog dialog(&printer);
        connect(&dialog, &QPrintPreviewDialog::paintRequested, this, &QIodeMenuPrintAbstract::renderForPrinting);

        if(isTable)
            printTable(dialog, document, names, gsample, files, NbDecimals, lang);
        else
            printVariable(dialog, document, names, gsample, files, NbDecimals, lang);
    }
}
