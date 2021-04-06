#include "file_import.h"


/* NOTE FOR THE DEVELOPERS:
 * The constructor of a Menu Dialog class deriving from MixingSettings MUST:
 * 1. instantiate the pointer *ui,
 * 2. call ui->setupUi(this),
 * 3. use the Wrapper classes defined in the wrapper_classes.h header file to wrap the input field items,
 * 4. fill the mapFields member (inherited from MixinSettings) with the wrapped field items,
 * 5. initialize the inherited className member (using typeid(this).name())
 * 6. call the loadSettings() method (inherited from MixinSettings).
 */
FileImportDialog::FileImportDialog(QSettings& settings, QWidget* parent, Qt::WindowFlags f) : MixinSettings(settings, parent, f)
{
    ui = new Ui::file_import();
    ui->setupUi(this);

    input_type = new WrapperComboBox(ui->label_input_type->text(), *ui->comboBox_input_type, Required, qmapIodeTypes);
    input_format = new WrapperComboBox(ui->label_input_format->text(), *ui->comboBox_input_format, Required, mapImportFormat);
    input_file = new WrapperFileChooser(ui->label_input_file->text(), *ui->fileChooser_input_file, Required, AnyFile, ExistingFile);
    rule_file = new WrapperFileChooser(ui->label_input_file->text(), *ui->fileChooser_input_file, Optional, AnyFile, ExistingFile);

    EnumIodeFile saveType = static_cast<EnumIodeFile>(ui->comboBox_input_type->currentIndex());
    save_file = new WrapperFileChooser(ui->label_save->text(), *ui->fileChooser_save_file, Required, saveType, NewFile);
    sample_from = new WrapperSampleEdit(ui->label_sample->text() + QString("From"), *ui->sampleEdit_sample_from, Required);
    sample_to = new WrapperSampleEdit(ui->label_sample->text() + QString("To"), *ui->sampleEdit_sample_to, Required);
    language = new WrapperComboBox(ui->label_language->text(), *ui->comboBox_language, Required, qmapLangs);

    debug_file = new WrapperFileChooser(ui->label_debug->text(), *ui->fileChooser_debug_file, Optional, LogsFile, NewFile);

    mapFields["InputType"] = input_type;
    mapFields["InputFormat"] = input_format;
    mapFields["InputFile"] = input_file;
    mapFields["RuleFile"] = rule_file;
    mapFields["SaveFile"] = save_file;
    mapFields["SampleFrom"] = sample_from;
    mapFields["SampleTo"] = sample_to;
    mapFields["Language"] = language;
    mapFields["DebugFile"] = debug_file;

    // TODO: if possible, find a way to initialize className inside MixingSettings
    // NOTE FOR DEVELOPPERS: we cannot simply call the line below from the constructor of MixingSettings 
    //                       since in that case this refers to MixingSettings and NOT the derived class
    className = QString::fromStdString(typeid(this).name());
    loadSettings();
}

FileImportDialog::~FileImportDialog()
{
    delete ui;
    
    delete input_type;
    delete input_format;
    delete input_file;
    delete rule_file;

    delete save_file;
    delete sample_from;
    delete sample_to;
    delete language;

    delete debug_file;
}

void FileImportDialog::import()
{
    try
    {
        int type = input_type->extractAndVerify();
        int fmt = input_format->extractAndVerify();
        std::string asc = input_file->extractAndVerify().toStdString();
        std::string rule = rule_file->extractAndVerify().toStdString();

        std::string ode = save_file->extractAndVerify().toStdString();
        std::string from = sample_from->extractAndVerify().toStdString();
        std::string to = sample_to->extractAndVerify().toStdString();
        int lang = language->extractAndVerify();
        std::string trace = debug_file->extractAndVerify().toStdString();

        CPP_XodeRuleImport(type, trace, rule, ode, asc, from, to, fmt, lang);
        this->accept();
    }
    catch (const std::runtime_error& e)
    {
        QMessageBox::critical(this, tr("ERROR"), tr(e.what()));
    }
}

