#include "graph_tables.h"


QIodeMenuGraphTables::QIodeMenuGraphTables(const QString& settings_filepath, QWidget* parent, Qt::WindowFlags f) : 
    QIodeSettings(settings_filepath, parent, f)
{
    setupUi(this);

    completer = new QIodeCompleter(false, false, I_TABLES, textEdit_table_names);
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

    // TODO: if possible, find a way to initialize className inside MixingSettings
    // NOTE FOR DEVELOPPERS: we cannot simply call the line below from the constructor of MixingSettings 
    //                       since in that case this refers to MixingSettings and NOT the derived class
    className = QString::fromStdString(typeid(this).name());
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
        QList<QString> qTablesList;
        for(int i=0; i<nb_tables; i++) qTablesList << QString::fromStdString(std::string(list_tables_names[i]));

        std::string sample = wSample->extractAndVerify().toStdString();

        std::string file_2 = wFile2->extractAndVerify().toStdString();
        std::string file_3 = wFile3->extractAndVerify().toStdString();
        std::string file_4 = wFile4->extractAndVerify().toStdString();
        std::string file_5 = wFile5->extractAndVerify().toStdString();

        EnumLang lang = EnumLang (wLanguage->extractAndVerify() + IT_ENGLISH);

        if(!file_2.empty()) load_reference_kdb(2, I_VARIABLES_FILE, file_2);
        if(!file_3.empty()) load_reference_kdb(3, I_VARIABLES_FILE, file_3);
        if(!file_4.empty()) load_reference_kdb(4, I_VARIABLES_FILE, file_4);
        if(!file_5.empty()) load_reference_kdb(5, I_VARIABLES_FILE, file_5);

        QMessageBox::warning(this, "WARNING", "Display is not yet implemented");

        //QIodePlotDialog* plotDialog = new QIodePlotDialog();
        //connect(plotDialog, &QIodePlotDialog::finished, this, QIodeMenuGraphTables::freeReferenceKDBs);
        //plotDialog->plot(qVarsList, from, to, chartType, axisType, logScale, xTicks, yTicks, minY, maxY);
        //emit newPlot(plotDialog);
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, tr("ERROR"), tr(e.what()));
        return;
    }
}

void QIodeMenuGraphTables::freeReferenceKDBs()
{
    clear_all_reference_kdbs();
}

void QIodeMenuGraphTables::print()
{
    try
    {
        QMessageBox::warning(this, "WARNING", "Print is not yet implemented");
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, tr("ERROR"), tr(e.what()));
        return;
    }
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
        QMessageBox::warning(this, "WARNING", "Setup is not yet implemented");
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
