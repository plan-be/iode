#include "file_export.h"


/* NOTE FOR THE DEVELOPERS:
 * The constructor of a Menu Dialog class deriving from MixingSettings MUST:
 * 1. instantiate the pointer *ui,
 * 2. call ui->setupUi(this),
 * 3. use the Wrapper classes defined in the wrapper_classes.h header file to wrap the input field items,
 * 4. fill the mapFields member (inherited from MixinSettings) with the wrapped field items,
 * 5. initialize the inherited className member (using typeid(this).name())
 * 6. call the loadSettings() method (inherited from MixinSettings).
 */
FileExportDialog::FileExportDialog(QSettings& settings, QWidget* parent, Qt::WindowFlags f) : MixinSettings(settings, parent, f)
{
    ui = new Ui::file_export();
    ui->setupUi(this);

    variable_file = new WrapperFileChooser(ui->label_variable_file->text(), *ui->fileChooser_variable_file, Required, VariablesFile, NewFile);
    comment_file = new WrapperFileChooser(ui->label_comment_file->text(), *ui->fileChooser_comment_file, Required, CommentsFile, NewFile);
    rule_file = new WrapperFileChooser(ui->label_rule_file->text(), *ui->fileChooser_rule_file, Optional, AnyFile, ExistingFile);
    
    sample_from = new WrapperSampleEdit(ui->label_sample->text() + QString("From"), *ui->sampleEdit_sample_from, Required);
    sample_to = new WrapperSampleEdit(ui->label_sample->text() + QString("To"), *ui->sampleEdit_sample_to, Required);
    format = new WrapperComboBox(ui->label_format->text(), *ui->comboBox_format, Required, exportFormatAsInt);
    save_file = new WrapperFileChooser(ui->label_save_file->text(), *ui->fileChooser_save_file, Optional, AnyFile, NewFile);
    csvNaN = new WrapperQLineEdit(ui->label_nan->text(), *ui->lineEdit_nan, Required);
    separator = new WrapperQLineEdit(ui->label_separator->text(), *ui->lineEdit_separator, Required);
    debug_file = new WrapperFileChooser(ui->label_debug->text(), *ui->fileChooser_debug_file, Optional, LogsFile, NewFile);

    mapFields["VariableFile"] = variable_file;
    mapFields["CommentFile"] = comment_file;
    mapFields["RuleFile"] = rule_file;
    mapFields["SampleFrom"] = sample_from;
    mapFields["SampleTo"] = sample_to;
    mapFields["Format"] = format;
    mapFields["SaveFile"] = save_file;
    mapFields["CSVNaN"] = csvNaN;
    mapFields["Separator"] = separator;
    mapFields["DebugFile"] = debug_file;

    // TODO: if possible, find a way to initialize className inside MixingSettings
    // NOTE FOR DEVELOPPERS: we cannot simply call the line below from the constructor of MixingSettings 
    //                       since in that case this refers to MixingSettings and NOT the derived class
    className = QString::fromStdString(typeid(this).name());
    loadSettings();
}

FileExportDialog::~FileExportDialog()
{
	delete ui;

    delete variable_file;
    delete comment_file;
    delete rule_file;

    delete sample_from;
    delete sample_to;
    delete format;
    delete save_file;
    delete csvNaN;
    delete separator;
    delete debug_file;
}

void FileExportDialog::save()
{
    try
    {
        std::string variable_filepath = variable_file->extractAndVerify().toStdString();
        std::string comment_filepath = comment_file->extractAndVerify().toStdString();
        std::string rule_filepath = rule_file->extractAndVerify().toStdString();

        std::string from = sample_from->extractAndVerify().toStdString();
        std::string to = sample_to->extractAndVerify().toStdString();
        int fmt = format->extractAndVerify();
        std::string save_filepath = save_file->extractAndVerify().toStdString();
        std::string na = csvNaN->extractAndVerify().toStdString();
        std::string sep = separator->extractAndVerify().toStdString();
        std::string debug_filepath = debug_file->extractAndVerify().toStdString();

        CPP_X_RuleExport(debug_filepath, rule_filepath, save_filepath, variable_filepath, comment_filepath, from, to, na, sep, fmt);
        this->accept();
    }
    catch (const std::runtime_error& e)
    {
        QMessageBox::critical(this, tr("ERROR"), tr(e.what()));
    }
}

