#include "file_print_setup.h"

const QString MenuFilePrintSetup::KEY_SETTINGS_PRINT_DEST = "MENU_FILE_PRINT_SETUP/Print_Dest";


MenuFilePrintSetup::MenuFilePrintSetup(QWidget* parent)
    : IodeSettingsDialog(parent)
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

MenuFilePrintSetup::~MenuFilePrintSetup()
{
    delete wPrintDest;
}

// Will call saveSettings -> see QSettings::closeEvent
void MenuFilePrintSetup::apply()
{
    this->accept();
}

void MenuFilePrintSetup::set_print_options()
{
    FilePrintPreferences dialog(static_cast<QWidget*>(parent()));
    dialog.exec();
}

void MenuFilePrintSetup::help()
{
	QDesktopServices::openUrl(url_manual);
}
