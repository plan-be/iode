#include "print_file_dialog.h"


const QString QIodePrintFileDialog::KEY_SETTINGS_PRINT_FORMAT = "PRINT_FILE_DIALOG/Print_Format";
const QString QIodePrintFileDialog::KEY_SETTINGS_PRINT_OUTPUT_FILE = "PRINT_FILE_DIALOG/Output_File";


QIodePrintFileDialog::QIodePrintFileDialog(const QString& settings_filepath, QWidget* parent, Qt::WindowFlags f)
    : QIodeSettings(settings_filepath, parent, f)
{
    setupUi(this);

    QList<QString> q_print_format;
    for(const std::string& format: vPrintFormats) q_print_format << QString::fromStdString(format);

    wPrintFormat = new WrapperComboBox(label_print_format->text(), *comboBox_print_format, REQUIRED_FIELD, q_print_format);
    wOutputFile = new WrapperFileChooser(label_output_file->text(), *fileChooser_output_file, REQUIRED_FIELD, 
        EnumIodeFile::I_A2M_FILE, EnumFileMode::FILE_MAY_EXIST);

    comboBox_print_format->setCurrentIndex(0);
    connect(comboBox_print_format, &QComboBox::currentIndexChanged, this, &QIodePrintFileDialog::updateOutputFileFormat);
    
    mapFields["Print_Format"] = wPrintFormat;
    mapFields["Output_File"] = wOutputFile;

    className = "PRINT_FILE_DIALOG";
    loadSettings();
}

QIodePrintFileDialog::~QIodePrintFileDialog()
{
    delete wPrintFormat;
    delete wOutputFile;
}

void QIodePrintFileDialog::updateOutputFileFormat(int index)
{
    fileChooser_output_file->setFileType(static_cast<EnumIodeFile>(I_A2M_FILE + index));
}

// Will automatically save to settings -> see QIodeSettings::closeEvent()
void QIodePrintFileDialog::apply()
{
    this->accept();
}

void QIodePrintFileDialog::set_print_options()
{
    QIodeMenuFilePrintPref dialog(project_settings->fileName(), static_cast<QWidget*>(parent()));
    dialog.exec();
}

void QIodePrintFileDialog::help()
{
	QDesktopServices::openUrl(url_manual);
}
