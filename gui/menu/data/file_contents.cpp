#include "file_contents.h"


MenuDataFileContents::MenuDataFileContents(QWidget* parent)
    : IodeSettingsDialog(parent)
{
    setupUi(this);

    QStringList listIodeType;
    for(const std::string& iode_type : v_iode_types) listIodeType << QString::fromStdString(iode_type);

    wSearch = new WrapperQLineEdit(label_search->text(), *lineEdit_search, REQUIRED_FIELD);
    wComboIodeType = new WrapperComboBox(label_iode_types->text(), *comboBox_iode_types, REQUIRED_FIELD, listIodeType);
    wInputFile = new WrapperFileChooser(label_input_file->text(), *fileChooser_input_file, OPTIONAL_FIELD, FILE_COMMENTS, EXISTING_FILE);
    wSaveList = new WrapperQLineEdit(label_save_list->text(), *lineEdit_save_list, REQUIRED_FIELD);
    wResult = new WrapperQTextEdit(label_result->text(), *textEdit_result, OPTIONAL_FIELD);

    mapFields["Search"]    = wSearch;
    mapFields["IodeType"] = wComboIodeType;
    mapFields["InputFile"] = wInputFile;
    mapFields["SaveList"]  = wSaveList;

    connect(&wComboIodeType->getQField(), &QComboBox::currentIndexChanged, this, &MenuDataFileContents::updateFileChooser);

    className = "MENU_DATA_FILE_CONTENTS";
    loadSettings();
}

MenuDataFileContents::~MenuDataFileContents()
{
    delete wSearch;
    delete wComboIodeType;
    delete wInputFile;
    delete wSaveList;
    delete wResult;
}

// TODO ALD: implement a file_contents() method in KDBAbstract + tests
// Tests: search = A*, iode type = COMMENTS, file = empty
//        search = A*, iode type = COMMENTS, file = fun2.ac
void MenuDataFileContents::search()
{
    try
    {
        std::string pattern = wSearch->extractAndVerify().toStdString();
        int iode_type = wComboIodeType->extractAndVerify();
        std::string input_file = wInputFile->extractAndVerify().toStdString();
        std::string save_list = wSaveList->extractAndVerify().toStdString();

        std::string buf = save_list + " " + pattern + " " + input_file;
        int res = B_DataList(buf.data(), iode_type);
        if(res < 0)
            B_display_last_error();

        List list = Lists.get(save_list);
        wResult->setQValue(QString::fromStdString(list));
        int nb_elements = B_DataListCount(save_list.data());
        label_nb_elements->setText(QString::number(nb_elements) + " Elements");
    }
    catch(const std::exception& e)
    {
        QMessageBox::warning(nullptr, "WARNING", QString(e.what()));
    }
}

void MenuDataFileContents::help()
{
	QDesktopServices::openUrl(url_manual);
}
