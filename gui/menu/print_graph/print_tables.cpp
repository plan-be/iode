#include "print_tables.h"


MenuPrintTables::MenuPrintTables(QWidget* parent) : MenuPrintAbstract(parent)
{
    setupUi(this);

    completer = new IodeCompleter(false, false, TABLES, textEdit_table_names);
    textEdit_table_names->setCompleter(completer);

    QList<QString> q_langs;
    for(const std::string& lang: v_table_langs) q_langs << QString::fromStdString(lang);

    wTablesNames = new WrapperQPlainTextEdit(label_table_names->text(), *textEdit_table_names, REQUIRED_FIELD);
    wSample = new WrapperQTextEdit(label_sample->text(), *textEdit_sample, REQUIRED_FIELD);
    wFile2 = new WrapperFileChooser(label_file_2->text(), *fileChooser_file_2, OPTIONAL_FIELD, FILE_VARIABLES, EXISTING_FILE);
    wFile3 = new WrapperFileChooser(label_file_3->text(), *fileChooser_file_3, OPTIONAL_FIELD, FILE_VARIABLES, EXISTING_FILE);
    wFile4 = new WrapperFileChooser(label_file_4->text(), *fileChooser_file_4, OPTIONAL_FIELD, FILE_VARIABLES, EXISTING_FILE);
    wFile5 = new WrapperFileChooser(label_file_5->text(), *fileChooser_file_5, OPTIONAL_FIELD, FILE_VARIABLES, EXISTING_FILE);
    wLanguage = new WrapperComboBox(label_language->text(), *comboBox_language, REQUIRED_FIELD, q_langs);
    wNbDecimals = new WrapperSpinBox(label_nb_decimals->text(), *spinBox_nb_decimals, REQUIRED_FIELD);
    
    mapFields["TablesNames"] = wTablesNames;
    mapFields["Sample"]      = wSample;
    mapFields["File2"]       = wFile2;
    mapFields["File3"]       = wFile3;
    mapFields["File4"]       = wFile4;
    mapFields["File5"]       = wFile5;
    mapFields["Language"]    = wLanguage;
    mapFields["NbDecimals"]  = wNbDecimals;

    className = "MENU_PRINT_TABLES";
    loadSettings();
}

MenuPrintTables::~MenuPrintTables()
{
    clear_all_reference_kdbs();
    foreach(ComputedTableNumericalDialog* view, tableViews) view->close();
    tableViews.clear();

    delete wTablesNames;
    delete wSample;
    delete wFile2;
    delete wFile3;
    delete wFile4;
    delete wFile5;
    delete wLanguage;
    delete wNbDecimals;

    delete completer;
}

void MenuPrintTables::display()
{
    try
    {
        clear_all_reference_kdbs();

        // build new gsample table name
        QString tableNames = wTablesNames->extractAndVerify();

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

        // Parses a string and replaces @filename and $listname by their contents
        char** c_table_names = B_ainit_chk(tableNames.toStdString().data(), NULL, 0);
        int nb_names = SCR_tbl_size((unsigned char**) c_table_names);
        for(int i=0; i < nb_names; i++)
        {
            QString tableName(c_table_names[i]); 
            ComputedTableNumericalDialog* view = new ComputedTableNumericalDialog(tableName, gsample, nb_decimals, "", this);
            tableViews.append(view);
            view->open();
        }
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, tr("ERROR"), tr(e.what()));
        return;
    }
}

/**
 * @brief 
 * 
 * @note see C_ViewPrintGrTblVar() in sb_view.c from the old GUI
 * 
 */
void MenuPrintTables::print()
{
    try
    {
        QString tableNames = wTablesNames->extractAndVerify();
        printTableOrVariable(true, tableNames);
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, tr("ERROR"), tr(e.what()));
        return;
    }
}

void MenuPrintTables::setup()
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

void MenuPrintTables::help()
{
	QDesktopServices::openUrl(url_manual);
}
