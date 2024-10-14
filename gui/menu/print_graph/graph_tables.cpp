#include "graph_tables.h"


MenuGraphTables::MenuGraphTables(QWidget* parent) : 
    IodeSettingsDialog(parent)
{
    setupUi(this);

    completer = new IodeCompleter(false, false, TABLES, textEdit_table_names);
    textEdit_table_names->setCompleter(completer);

    QList<QString> q_langs;
    for(const std::string& lang: v_table_langs) q_langs << QString::fromStdString(lang);

    wTableNames = new WrapperQPlainTextEdit(label_table_names->text(), *textEdit_table_names, REQUIRED_FIELD);
    wSample = new WrapperQTextEdit(label_sample->text(), *textEdit_sample, REQUIRED_FIELD);
    wFile2 = new WrapperFileChooser(label_file_2->text(), *fileChooser_file_2, OPTIONAL_FIELD, FILE_VARIABLES, EXISTING_FILE);
    wFile3 = new WrapperFileChooser(label_file_3->text(), *fileChooser_file_3, OPTIONAL_FIELD, FILE_VARIABLES, EXISTING_FILE);
    wFile4 = new WrapperFileChooser(label_file_4->text(), *fileChooser_file_4, OPTIONAL_FIELD, FILE_VARIABLES, EXISTING_FILE);
    wFile5 = new WrapperFileChooser(label_file_5->text(), *fileChooser_file_5, OPTIONAL_FIELD, FILE_VARIABLES, EXISTING_FILE);
    wLanguage = new WrapperComboBox(label_language->text(), *comboBox_language, REQUIRED_FIELD, q_langs);
    
    mapFields["TableNames"] = wTableNames;
    mapFields["Sample"] = wSample;
    mapFields["File2"] = wFile2;
    mapFields["File3"] = wFile3;
    mapFields["File4"] = wFile4;
    mapFields["File5"] = wFile5;
    mapFields["Language"] = wLanguage;

    className = "MENU_GRAPH_TABLES";
    loadSettings();
}

MenuGraphTables::~MenuGraphTables()
{
    delete wTableNames;
    delete wSample;
    delete wFile2;
    delete wFile3;
    delete wFile4;
    delete wFile5;
    delete wLanguage;

    delete completer;
}

void MenuGraphTables::display()
{
    try
    {
        std::string pattern = wTableNames->extractAndVerify().toStdString();
        std::vector<std::string> list_names = Tables.get_names(pattern);
        QStringList qTablesList;
        for(const std::string& name : list_names) 
            qTablesList << QString::fromStdString(name);

        std::string gsample = wSample->extractAndVerify().toStdString();

        std::string file_2 = wFile2->extractAndVerify().toStdString();
        std::string file_3 = wFile3->extractAndVerify().toStdString();
        std::string file_4 = wFile4->extractAndVerify().toStdString();
        std::string file_5 = wFile5->extractAndVerify().toStdString();

        TableLang lang = TableLang (wLanguage->extractAndVerify());

        if(!file_2.empty()) load_reference_kdb(2, VARIABLES, file_2);
        if(!file_3.empty()) load_reference_kdb(3, VARIABLES, file_3);
        if(!file_4.empty()) load_reference_kdb(4, VARIABLES, file_4);
        if(!file_5.empty()) load_reference_kdb(5, VARIABLES, file_5);

        Table* table_obj;
        MainWindowAbstract* main_window = static_cast<MainWindowAbstract*>(get_main_window_ptr());
        foreach(const QString& tableName, qTablesList)
        {
            table_obj = Tables.get(tableName.toStdString());
            ComputedTableGraph* gSampleGraph = new ComputedTableGraph(table_obj, gsample);
            PlotTableDialog* plotDialog = new PlotTableDialog(gSampleGraph);
            main_window->appendPlot(plotDialog);
            delete table_obj;
        }
    }
    catch (const std::exception& e)
    {
        QMessageBox::warning(this, "WARNING", "" + QString::fromStdString(e.what()));
        return;
    }
}

void MenuGraphTables::freeReferenceKDBs()
{
    clear_all_reference_kdbs();
}

void MenuGraphTables::apply()
{
    try
    {
        QMessageBox::warning(this, "WARNING", "Apply is not yet implemented");
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, tr("ERROR"), tr(e.what()));
        return;
    }
}

void MenuGraphTables::setup()
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

void MenuGraphTables::help()
{
	QDesktopServices::openUrl(url_manual);
}
