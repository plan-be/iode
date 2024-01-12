#include "file_settings.h"

const QString MenuFileSettings::KEY_SETTINGS_PRINT_DEST = "MENU_FILE_SETTINGS/Print_Dest";


MenuFileSettings::MenuFileSettings(QWidget* parent)
    : IodeSettingsDialog(parent)
{
    setupUi(this);

    QList<QString> q_print_dest;
    for(const std::string& dest: vPrintDest) q_print_dest << QString::fromStdString(dest);

    wPrintDest = new WrapperComboBox(label_print_dest->text(), *comboBox_print_dest, REQUIRED_FIELD, q_print_dest);
    mapFields["Print_Dest"] = wPrintDest;
    comboBox_print_dest->setCurrentIndex(0);

    className = "MENU_FILE_SETTINGS";
    loadSettings();
}

MenuFileSettings::~MenuFileSettings()
{
    delete wPrintDest;
}

// Will call saveSettings -> see QSettings::closeEvent
void MenuFileSettings::apply()
{
    this->accept();
}

void MenuFileSettings::set_print_options()
{
    FilePrintPreferences dialog(static_cast<QWidget*>(parent()));
    dialog.exec();
}

void MenuFileSettings::help()
{
	QDesktopServices::openUrl(url_manual);
}
