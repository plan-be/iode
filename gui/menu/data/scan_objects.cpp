#include "scan_objects.h"


MenuDataScanObjects::MenuDataScanObjects(QWidget* parent)
    : IodeSettingsDialog(parent)
{
    setupUi(this);

    completer = new IodeCompleter(false, false, COMMENTS, this);
    textEdit_lists_to_scan->setCompleter(completer);
    comboBox_iode_types->setCurrentIndex(0);

    QStringList listIodeTypes;
    for(const std::string& iode_type : v_iode_types) listIodeTypes << QString::fromStdString(iode_type);

    wComboIodeTypes = new WrapperComboBox(label_objs_to_scan->text(), *comboBox_iode_types, REQUIRED_FIELD, listIodeTypes);
    wListsToScan = new WrapperQPlainTextEdit(label_lists_to_scan->text(), *textEdit_lists_to_scan, OPTIONAL_FIELD);

    mapFields["IodeType"]    = wComboIodeTypes;
    mapFields["ListsToScan"] = wListsToScan;

    connect(comboBox_iode_types, &QComboBox::currentIndexChanged, textEdit_lists_to_scan, &IodeAutoCompleteTextEdit::setIodeType);

    className = "MENU_DATA_SCAN_OBJECTS";
    loadSettings();
}

MenuDataScanObjects::~MenuDataScanObjects()
{
    delete wComboIodeTypes;
    delete wListsToScan;

    delete completer;
}

// TODO ALD: implement a scan_objects() method in KDBAbstract + tests
// Tests: iode type = EQUATIONS, list to scan = ???
void MenuDataScanObjects::scan()
{
    try
    {
        int iode_type = wComboIodeTypes->extractAndVerify();
        std::string list_to_scan = wListsToScan->extractAndVerify().toStdString();

        int res = B_DataScan(list_to_scan.data(), iode_type);
        if(res < 0)
            B_display_last_error();

        this->accept();
    }
    catch(const std::exception& e)
    {
        QMessageBox::warning(nullptr, "WARNING", QString(e.what()));
    }
}

void MenuDataScanObjects::help()
{
	QDesktopServices::openUrl(url_manual);
}
