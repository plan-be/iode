#include "print_abstract.h"


void MenuPrintAbstract::printTableOrVariable(const bool isTable, const QString& names)
{
    QSettings* project_settings = ProjectSettings::getProjectSettings();
    bool printToFile = project_settings->value(MenuFileSettings::KEY_SETTINGS_PRINT_DEST).toBool();

    QString outputFile;
    QChar format;
    if(printToFile)
    {
        // ask the user to set the output file and format
        PrintFileDialog dialog(this);
        if(dialog.exec() == QDialog::Accepted)
        {
            outputFile = dialog.getOutputFile();
            format = dialog.getFormat();
        }
        else
            return;
    }

    // extract the number of decimals
    int NbDecimals = wNbDecimals->extractAndVerify();

    // extract the language
    TableLang lang = (TableLang) wLanguage->extractAndVerify();

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
        connect(&dialog, &QPrintPreviewDialog::paintRequested, this, &MenuPrintAbstract::renderForPrinting);

        if(isTable)
            printTable(dialog, document, names, gsample, files, NbDecimals, lang);
        else
            printVariable(dialog, document, names, gsample, files, NbDecimals, lang);
    }
}
