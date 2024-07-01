#include "print_file_dialog.h"


PrintFileDialog::PrintFileDialog(QWidget* parent, const QString& outputFile, const QChar& format): 
    IodeSettingsDialog(parent)
{
    setupUi(this);

    QList<QString> q_print_format;
    for(const std::string& format: vPrintFormats) q_print_format << QString::fromStdString(format);

    wPrintFormat = new WrapperComboBox(label_print_format->text(), *comboBox_print_format, REQUIRED_FIELD, q_print_format);
    wOutputFile = new WrapperFileChooser(label_output_file->text(), *fileChooser_output_file, REQUIRED_FIELD, 
        IodeFileType::FILE_A2M, EnumFileMode::FILE_MAY_EXIST);

    comboBox_print_format->setCurrentIndex(0);
    connect(comboBox_print_format, &QComboBox::currentIndexChanged, this, &PrintFileDialog::updateOutputFileFormat);
    
    mapFields["Print_Format"] = wPrintFormat;

    className = "PRINT_FILE_DIALOG";
    loadSettings();

    if(!outputFile.isEmpty())
        setOutputFile(outputFile);

    if(!format.isNull())
        setFormat(format);
}

PrintFileDialog::~PrintFileDialog()
{
    delete wPrintFormat;
    delete wOutputFile;
}

void PrintFileDialog::updateOutputFileFormat(int index)
{
    fileChooser_output_file->setFileType(static_cast<IodeFileType>(FILE_A2M + index));
}

// Will automatically save to settings -> see IodeSettingsDialog::closeEvent()
void PrintFileDialog::apply()
{
    this->accept();
}

void PrintFileDialog::set_print_options()
{
    FilePrintPreferences dialog(static_cast<QWidget*>(parent()));
    dialog.exec();
}

void PrintFileDialog::help()
{
	QDesktopServices::openUrl(url_manual);
}
