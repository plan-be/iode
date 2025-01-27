#include "file_import_comments.h"


MenuFileImportComments::MenuFileImportComments(QWidget* parent) : 
    IodeSettingsDialog(parent)
{
    setupUi(this);

    QList<QString> q_langs;
    for(const std::string& lang: v_table_langs) q_langs << QString::fromStdString(lang);

    wInputFile = new WrapperFileChooser(label_input_file->text(), *fileChooser_input_file, REQUIRED_FIELD, FILE_ANY, EXISTING_FILE);
    wRuleFile = new WrapperFileChooser(label_input_file->text(), *fileChooser_input_file, REQUIRED_FIELD, FILE_ANY, EXISTING_FILE);

    wSaveFile = new WrapperFileChooser(label_save->text(), *fileChooser_save_file, REQUIRED_FIELD, FILE_VARIABLES, NEW_FILE);
    wLanguage = new WrapperComboBox(label_language->text(), *comboBox_language, REQUIRED_FIELD, q_langs);

    wDebugFile = new WrapperFileChooser(label_debug->text(), *fileChooser_debug_file, OPTIONAL_FIELD, FILE_LOG, NEW_FILE);

    mapFields["InputFile"]   = wInputFile;
    mapFields["RuleFile"]    = wRuleFile;
    mapFields["SaveFile"]    = wSaveFile;
    mapFields["Language"]    = wLanguage;
    mapFields["DebugFile"]   = wDebugFile;

    className = "MENU_FILE_IMPORT_COMMENTS";
    loadSettings();
}

MenuFileImportComments::~MenuFileImportComments()
{
    delete wInputFile;
    delete wRuleFile;
    delete wSaveFile;
    delete wLanguage;
    delete wDebugFile;
}

void MenuFileImportComments::import()
{
    try
    {
        std::string input_file = wInputFile->extractAndVerify().toStdString();
        std::string rule_file = wRuleFile->extractAndVerify().toStdString();

        std::string save_file = wSaveFile->extractAndVerify().toStdString();
        TableLang lang = TableLang (wLanguage->extractAndVerify());
        std::string debug_file = wDebugFile->extractAndVerify().toStdString();

        import_cmt(input_file, save_file, rule_file, lang, debug_file);
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, tr("ERROR"), tr(e.what()));
        return;
    }

    this->accept();
}

void MenuFileImportComments::help()
{
	
	QDesktopServices::openUrl(url_manual);
}
