#include "file_export.h"


MenuFileExport::MenuFileExport(QWidget* parent) :
    IodeSettingsDialog(parent)
{
    setupUi(this);

    QList<QString> q_formats;
    for(const std::string& format: vExportFormats) q_formats << QString::fromStdString(format);

    wVarFile = new WrapperFileChooser(label_variable_file->text(), *fileChooser_variable_file, OPTIONAL_FIELD, FILE_ANY, EXISTING_FILE);
    wCmtFile = new WrapperFileChooser(label_comment_file->text(), *fileChooser_comment_file, OPTIONAL_FIELD, FILE_ANY, EXISTING_FILE);
    wRuleFile = new WrapperFileChooser(label_rule_file->text(), *fileChooser_rule_file, REQUIRED_FIELD, FILE_ANY, EXISTING_FILE);

    wFrom = new WrapperSampleEdit(label_sample_from->text(), *sampleEdit_sample_from, OPTIONAL_FIELD);
    wTo = new WrapperSampleEdit(label_sample_to->text(), *sampleEdit_sample_to, OPTIONAL_FIELD);
    wExportFormat = new WrapperComboBox(label_format->text(), *comboBox_format, REQUIRED_FIELD, q_formats);
    wExportFile = new WrapperFileChooser(label_save_file->text(), *fileChooser_save_file, REQUIRED_FIELD, FILE_VARIABLES, NEW_FILE);
    wNan = new WrapperQLineEdit(label_nan->text(), *lineEdit_nan, REQUIRED_FIELD);
    wSeparator = new WrapperQLineEdit(label_separator->text(), *lineEdit_separator, REQUIRED_FIELD);
    wDebugFile = new WrapperFileChooser(label_debug->text(), *fileChooser_debug_file, OPTIONAL_FIELD, FILE_LOG, NEW_FILE);

    mapFields["VarFile"]      = wVarFile;
    mapFields["CmtFile"]      = wCmtFile;
    mapFields["RuleFile"]     = wRuleFile;
    mapFields["From"]         = wFrom;
    mapFields["To"]           = wTo;
    mapFields["ExportFormat"] = wExportFormat;
    mapFields["ExportFile"]   = wExportFile;
    mapFields["Nan"]          = wNan;
    mapFields["Separator"]    = wSeparator;
    mapFields["DebugFile"]    = wDebugFile;

    className = "MENU_FILE_EXPORT";
    loadSettings();
}

MenuFileExport::~MenuFileExport()
{
    delete wVarFile;
    delete wCmtFile;
    delete wRuleFile;
    delete wFrom;
    delete wTo;
    delete wExportFormat;
    delete wExportFile;
    delete wNan;
    delete wSeparator;
    delete wDebugFile;
}

void MenuFileExport::save()
{
    try
    {
        std::string var_file = wVarFile->extractAndVerify().toStdString();
        std::string cmt_file = wCmtFile->extractAndVerify().toStdString();
        std::string rule_file = wRuleFile->extractAndVerify().toStdString();

        std::string from = wFrom->extractAndVerify().toStdString();
        std::string to = wTo->extractAndVerify().toStdString();
        IodeExportFormat format = (IodeExportFormat) wExportFormat->extractAndVerify();
        std::string save_file = wExportFile->extractAndVerify().toStdString();
        std::string nan = wNan->extractAndVerify().toStdString();
        std::string separator = wSeparator->extractAndVerify().toStdString();
        std::string debug_file = wDebugFile->extractAndVerify().toStdString();

        // Note: export is a C++ reserved keyword
        export_as(var_file, cmt_file, from, to, format, save_file, rule_file, nan, separator, debug_file);
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, tr("ERROR"), tr(e.what()));
        return;
    }

    this->accept();
}

void MenuFileExport::help()
{
	
	QDesktopServices::openUrl(url_manual);
}
