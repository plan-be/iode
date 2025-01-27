#include "print_variables.h"


MenuPrintVariables::MenuPrintVariables(QWidget* parent) : MenuPrintAbstract(parent)
{
    setupUi(this);

    completer = new IodeCompleter(false, false, VARIABLES, textEdit_variable_names);
    textEdit_variable_names->setCompleter(completer);

    QList<QString> q_langs;
    for(const std::string& lang: v_table_langs) q_langs << QString::fromStdString(lang);

    wVariablesNames = new WrapperQPlainTextEdit(label_variable_names->text(), *textEdit_variable_names, REQUIRED_FIELD);
    wSample = new WrapperQTextEdit(label_sample->text(), *textEdit_sample, REQUIRED_FIELD);
    wFile2 = new WrapperFileChooser(label_file_2->text(), *fileChooser_file_2, OPTIONAL_FIELD, FILE_VARIABLES, EXISTING_FILE);
    wFile3 = new WrapperFileChooser(label_file_3->text(), *fileChooser_file_3, OPTIONAL_FIELD, FILE_VARIABLES, EXISTING_FILE);
    wFile4 = new WrapperFileChooser(label_file_4->text(), *fileChooser_file_4, OPTIONAL_FIELD, FILE_VARIABLES, EXISTING_FILE);
    wFile5 = new WrapperFileChooser(label_file_5->text(), *fileChooser_file_5, OPTIONAL_FIELD, FILE_VARIABLES, EXISTING_FILE);
    wLanguage = new WrapperComboBox(label_language->text(), *comboBox_language, REQUIRED_FIELD, q_langs);
    wNbDecimals = new WrapperSpinBox(label_nb_decimals->text(), *spinBox_nb_decimals, REQUIRED_FIELD);
    
    mapFields["VariablesNames"] = wVariablesNames;
    mapFields["Sample"]         = wSample;
    mapFields["File2"]          = wFile2;
    mapFields["File3"]          = wFile3;
    mapFields["File4"]          = wFile4;
    mapFields["File5"]          = wFile5;
    mapFields["Language"]       = wLanguage;
    mapFields["NbDecimals"]     = wNbDecimals;

    className = "MENU_PRINT_VARIABLES";
    loadSettings();
}

MenuPrintVariables::~MenuPrintVariables()
{
    clear_all_reference_kdbs();

    delete wVariablesNames;
    delete wSample;
    delete wFile2;
    delete wFile3;
    delete wFile4;
    delete wFile5;
    delete wLanguage;
    delete wNbDecimals;

    delete completer;
}

void MenuPrintVariables::display()
{
    try
    {
        clear_all_reference_kdbs();

        // build new gsample table name
        QString tableName = "_VARS_TBL_";

        QString variables = wVariablesNames->extractAndVerify();

        QString gsample = wSample->extractAndVerify();

        std::string file_2 = wFile2->extractAndVerify().toStdString();
        std::string file_3 = wFile3->extractAndVerify().toStdString();
        std::string file_4 = wFile4->extractAndVerify().toStdString();
        std::string file_5 = wFile5->extractAndVerify().toStdString();

        TableLang lang = TableLang (wLanguage->extractAndVerify());

        int nb_decimals = wNbDecimals->extractAndVerify();

        if(!file_2.empty()) load_reference_kdb(2, VARIABLES, file_2);
        if(!file_3.empty()) load_reference_kdb(3, VARIABLES, file_3);
        if(!file_4.empty()) load_reference_kdb(4, VARIABLES, file_4);
        if(!file_5.empty()) load_reference_kdb(5, VARIABLES, file_5);

        ComputedTableNumericalDialog view(tableName, gsample, nb_decimals, variables, this);
        view.exec();
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, tr("ERROR"), tr(e.what()));
        return;
    }
}

void MenuPrintVariables::print()
{
    try
    {
        QString variables = wVariablesNames->extractAndVerify();
        printTableOrVariable(false, variables);
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, tr("ERROR"), tr(e.what()));
        return;
    }
}

void MenuPrintVariables::setup()
{
    try
    {
        MenuFileSettings dialog(this);
        dialog.showPrintTab();
        dialog.exec();
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, tr("ERROR"), tr(e.what()));
        return;
    }
}

void MenuPrintVariables::help()
{
	QDesktopServices::openUrl(url_manual);
}
