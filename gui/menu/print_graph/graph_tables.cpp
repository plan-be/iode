#include "graph_tables.h"


QIodeMenuGraphTables::QIodeMenuGraphTables(QWidget* parent) : 
    QIodeSettings(parent)
{
    setupUi(this);

    completer = new IodeCompleter(false, false, I_TABLES, textEdit_table_names);
    textEdit_table_names->setCompleter(completer);

    QList<QString> q_langs;
    for(const std::string& lang: vLangs) q_langs << QString::fromStdString(lang);

    wTableNames = new WrapperQPlainTextEdit(label_table_names->text(), *textEdit_table_names, REQUIRED_FIELD);
    wSample = new WrapperQTextEdit(label_sample->text(), *textEdit_sample, REQUIRED_FIELD);
    wFile2 = new WrapperFileChooser(label_file_2->text(), *fileChooser_file_2, OPTIONAL_FIELD, I_VARIABLES_FILE, EXISTING_FILE);
    wFile3 = new WrapperFileChooser(label_file_3->text(), *fileChooser_file_3, OPTIONAL_FIELD, I_VARIABLES_FILE, EXISTING_FILE);
    wFile4 = new WrapperFileChooser(label_file_4->text(), *fileChooser_file_4, OPTIONAL_FIELD, I_VARIABLES_FILE, EXISTING_FILE);
    wFile5 = new WrapperFileChooser(label_file_5->text(), *fileChooser_file_5, OPTIONAL_FIELD, I_VARIABLES_FILE, EXISTING_FILE);
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

QIodeMenuGraphTables::~QIodeMenuGraphTables()
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

void QIodeMenuGraphTables::display()
{
    try
    {
        std::string tables_names = wTableNames->extractAndVerify().toStdString();
        char** list_tables_names = filter_kdb_names(I_TABLES, tables_names);
        int nb_tables = SCR_tbl_size((unsigned char**) list_tables_names);
        QStringList qTablesList;
        for(int i=0; i<nb_tables; i++) qTablesList << QString::fromStdString(std::string(list_tables_names[i]));

        std::string gsample = wSample->extractAndVerify().toStdString();

        std::string file_2 = wFile2->extractAndVerify().toStdString();
        std::string file_3 = wFile3->extractAndVerify().toStdString();
        std::string file_4 = wFile4->extractAndVerify().toStdString();
        std::string file_5 = wFile5->extractAndVerify().toStdString();

        EnumLang lang = EnumLang (wLanguage->extractAndVerify() + IT_ENGLISH);

        if(!file_2.empty()) load_reference_kdb(2, I_VARIABLES_FILE, file_2);
        if(!file_3.empty()) load_reference_kdb(3, I_VARIABLES_FILE, file_3);
        if(!file_4.empty()) load_reference_kdb(4, I_VARIABLES_FILE, file_4);
        if(!file_5.empty()) load_reference_kdb(5, I_VARIABLES_FILE, file_5);

        MainWindowAbstract* main_window = static_cast<MainWindowAbstract*>(get_main_window_ptr());
        foreach(const QString& tableName, qTablesList)
        {
            GSampleTable* gSampleTable = new GSampleTable(tableName.toStdString(), gsample);
            QIodePlotTableDialog* plotDialog = new QIodePlotTableDialog(gSampleTable);
            main_window->appendPlot(plotDialog);
        }
    }
    catch (const std::exception& e)
    {
        QMessageBox::warning(this, "WARNING", "" + QString::fromStdString(e.what()));
        return;
    }
}

void QIodeMenuGraphTables::freeReferenceKDBs()
{
    clear_all_reference_kdbs();
}

void QIodeMenuGraphTables::apply()
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

void QIodeMenuGraphTables::setup()
{
    try
    {
        MenuFilePrintSetup dialog(this);
        dialog.exec();
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, tr("ERROR"), tr(e.what()));
        return;
    }
}

void QIodeMenuGraphTables::help()
{
	QDesktopServices::openUrl(url_manual);
}
