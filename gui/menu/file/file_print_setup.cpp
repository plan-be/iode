#include "file_print_setup.h"

const QString QIodeMenuFilePrintSetup::KEY_SETTINGS_PRINT_DEST = "MENU_FILE_PRINT_SETUP/Print_Dest";


QIodeMenuFilePrintSetup::QIodeMenuFilePrintSetup(QWidget* parent)
    : QIodeSettings(parent)
{
    setupUi(this);

    QList<QString> q_print_dest;
    for(const std::string& dest: vPrintDest) q_print_dest << QString::fromStdString(dest);

    wPrintDest = new WrapperComboBox(label_print_dest->text(), *comboBox_print_dest, REQUIRED_FIELD, q_print_dest);
    mapFields["Print_Dest"] = wPrintDest;
    comboBox_print_dest->setCurrentIndex(0);

    className = "MENU_FILE_PRINT_SETUP";
    loadSettings();
}

QIodeMenuFilePrintSetup::~QIodeMenuFilePrintSetup()
{
    delete wPrintDest;
}

// Will call saveSettings -> see QSettings::closeEvent
void QIodeMenuFilePrintSetup::apply()
{
    this->accept();
}

void QIodeMenuFilePrintSetup::set_print_options()
{
    QIodeMenuFilePrintPref dialog(static_cast<QWidget*>(parent()));
    dialog.exec();
}

void QIodeMenuFilePrintSetup::help()
{
	QDesktopServices::openUrl(url_manual);
}
