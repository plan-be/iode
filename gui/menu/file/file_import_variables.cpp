#include "file_import_variables.h"


QIodeMenuFileImportVariables::QIodeMenuFileImportVariables(QWidget* parent) : 
    QIodeSettings(parent)
{
    setupUi(this);

    QList<QString> q_langs;
    for(const std::string& lang: vLangs) q_langs << QString::fromStdString(lang);

    wInputFile = new WrapperFileChooser(label_input_file->text(), *fileChooser_input_file, REQUIRED_FIELD, I_ANY_FILE, EXISTING_FILE);
    wRuleFile = new WrapperFileChooser(label_input_file->text(), *fileChooser_input_file, REQUIRED_FIELD, I_ANY_FILE, EXISTING_FILE);

    wSaveFile = new WrapperFileChooser(label_save->text(), *fileChooser_save_file, REQUIRED_FIELD, I_COMMENTS_FILE, NEW_FILE);
    wFrom = new WrapperSampleEdit(label_sample_from->text(), *sampleEdit_sample_from, REQUIRED_FIELD);
    wTo = new WrapperSampleEdit(label_sample_to->text(), *sampleEdit_sample_to, REQUIRED_FIELD);

    wDebugFile = new WrapperFileChooser(label_debug->text(), *fileChooser_debug_file, OPTIONAL_FIELD, I_LOGS_FILE, NEW_FILE);

    mapFields["InputFile"]   = wInputFile;
    mapFields["RuleFile"]    = wRuleFile;
    mapFields["SaveFile"]    = wSaveFile;
    mapFields["From"]        = wFrom;
    mapFields["To"]          = wTo;
    mapFields["DebugFile"]   = wDebugFile;

    className = "MENU_FILE_IMPORT_VARIABLES";
    loadSettings();
}

QIodeMenuFileImportVariables::~QIodeMenuFileImportVariables()
{
    delete wInputFile;
    delete wRuleFile;
    delete wSaveFile;
    delete wFrom;
    delete wTo;
    delete wDebugFile;
}

void QIodeMenuFileImportVariables::import()
{
    try
    {
        std::string input_file = wInputFile->extractAndVerify().toStdString();
        std::string rule_file = wRuleFile->extractAndVerify().toStdString();

        std::string save_file = wSaveFile->extractAndVerify().toStdString();
        std::string from = wFrom->extractAndVerify().toStdString();
        std::string to = wTo->extractAndVerify().toStdString();
        std::string debug_file = wDebugFile->extractAndVerify().toStdString();

        import_var(input_file, save_file, from, to, rule_file, debug_file);
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, tr("ERROR"), tr(e.what()));
        return;
    }

    this->accept();
}

void QIodeMenuFileImportVariables::help()
{
	
	QDesktopServices::openUrl(url_manual);
}
