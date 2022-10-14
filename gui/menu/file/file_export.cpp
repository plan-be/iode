#include "file_export.h"


QIodeMenuFileExport::QIodeMenuFileExport(const QString& project_settings_filepath, QWidget* parent, Qt::WindowFlags f) :
    QIodeSettings(project_settings_filepath, parent, f)
{
    setupUi(this);

    QList<QString> q_formats;
    for(const std::string& format: vExportFormats) q_formats << QString::fromStdString(format);

    wVarFile = new WrapperFileChooser(label_variable_file->text(), *fileChooser_variable_file, OPTIONAL_FIELD, I_ANY_FILE, EXISTING_FILE);
    wCmtFile = new WrapperFileChooser(label_comment_file->text(), *fileChooser_comment_file, OPTIONAL_FIELD, I_ANY_FILE, EXISTING_FILE);
    wRuleFile = new WrapperFileChooser(label_rule_file->text(), *fileChooser_rule_file, REQUIRED_FIELD, I_ANY_FILE, EXISTING_FILE);

    wFrom = new WrapperSampleEdit(label_sample_from->text(), *sampleEdit_sample_from, OPTIONAL_FIELD);
    wTo = new WrapperSampleEdit(label_sample_to->text(), *sampleEdit_sample_to, OPTIONAL_FIELD);
    wExportFormat = new WrapperComboBox(label_format->text(), *comboBox_format, REQUIRED_FIELD, q_formats);
    wExportFile = new WrapperFileChooser(label_save_file->text(), *fileChooser_save_file, REQUIRED_FIELD, I_VARIABLES_FILE, NEW_FILE);
    wNan = new WrapperQLineEdit(label_nan->text(), *lineEdit_nan, REQUIRED_FIELD);
    wSeparator = new WrapperQLineEdit(label_separator->text(), *lineEdit_separator, REQUIRED_FIELD);
    wDebugFile = new WrapperFileChooser(label_debug->text(), *fileChooser_debug_file, OPTIONAL_FIELD, I_LOGS_FILE, NEW_FILE);

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

    // TODO: if possible, find a way to initialize className inside MixingSettings
    // NOTE FOR DEVELOPPERS: we cannot simply call the line below from the constructor of MixingSettings 
    //                       since in that case this refers to MixingSettings and NOT the derived class
    className = QString::fromStdString(typeid(this).name());
    loadSettings();
}

QIodeMenuFileExport::~QIodeMenuFileExport()
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

void QIodeMenuFileExport::save()
{
    try
    {
        std::string var_file = wVarFile->extractAndVerify().toStdString();
        std::string cmt_file = wCmtFile->extractAndVerify().toStdString();
        std::string rule_file = wRuleFile->extractAndVerify().toStdString();

        std::string from = wFrom->extractAndVerify().toStdString();
        std::string to = wTo->extractAndVerify().toStdString();
        EnumIodeExportFormat format = (EnumIodeExportFormat) wExportFormat->extractAndVerify();
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

void QIodeMenuFileExport::help()
{
	QUrl url = get_url_iode_manual();
	QDesktopServices::openUrl(url);
}
