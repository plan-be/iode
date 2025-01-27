#include "search_text.h"

MenuDataSearchText::MenuDataSearchText(QWidget* parent)
    : IodeSettingsDialog(parent)
{
    setupUi(this);

    QStringList listIodeType;
    for(const std::string& iode_type : v_iode_types) listIodeType << QString::fromStdString(iode_type);

    wText = new WrapperQLineEdit(label_text->text(), *lineEdit_text, REQUIRED_FIELD);
    wSaveList = new WrapperQLineEdit(label_save_list->text(), *lineEdit_save_list, REQUIRED_FIELD);
    wComboIodeType = new WrapperComboBox(label_objs_to_scan->text(), *comboBox_iode_types, REQUIRED_FIELD, listIodeType);
    wWholeWord = new WrapperCheckBox(checkBox_whole_word->text(), *checkBox_whole_word, REQUIRED_FIELD);
    wExactCase = new WrapperCheckBox(checkBox_exact_case->text(), *checkBox_exact_case, REQUIRED_FIELD);
    wSearchInNames = new WrapperCheckBox(checkBox_search_names->text(), *checkBox_search_names, REQUIRED_FIELD);
    wSearchInFormulas = new WrapperCheckBox(checkBox_search_formulas->text(), *checkBox_search_formulas, REQUIRED_FIELD);
    wSearchInFreeText = new WrapperCheckBox(checkBox_search_free_text->text(), *checkBox_search_free_text, REQUIRED_FIELD);
    wResults = new WrapperQTextEdit(label_result->text(), *textEdit_result, OPTIONAL_FIELD);

    mapFields["Text"]      = wText;
    mapFields["SaveList"]  = wSaveList;
    mapFields["IodeType"]  = wComboIodeType;
    mapFields["WholeWord"] = wWholeWord;
    mapFields["ExactCase"] = wExactCase;
    mapFields["SearchInNames"] = wSearchInNames;
    mapFields["SearchInFormulas"] = wSearchInFormulas;
    mapFields["SearchInFreeText"] = wSearchInFreeText;

    className = "MENU_DATA_SEARCH_TEXT";
    loadSettings();
}

MenuDataSearchText::~MenuDataSearchText()
{
    delete wText;
    delete wSaveList;
    delete wComboIodeType;
    delete wWholeWord;
    delete wExactCase;
    delete wSearchInNames;
    delete wSearchInFormulas;
    delete wSearchInFreeText;
    delete wResults;
}

// TODO ALD: implement a search_text() method in KDBAbstract + tests
// Test: search Kost in Comments with search_in_free_text = true;
void MenuDataSearchText::search()
{
    try
    {
        std::string text = wText->extractAndVerify().toStdString();
        std::string save_list = wSaveList->extractAndVerify().toStdString();
        int iode_type = wComboIodeType->extractAndVerify();
        bool whole_word = wWholeWord->extractAndVerify();
        bool exact_case = wExactCase->extractAndVerify();
        bool search_in_names = wSearchInNames->extractAndVerify();
        bool search_in_formulas = wSearchInFormulas->extractAndVerify();
        bool search_in_free_text = wSearchInFreeText->extractAndVerify();

        std::string buf = text + " ";
        buf += whole_word ? "1 " : "0 ";
        buf += exact_case ? "1 " : "0 ";
        buf += search_in_names ? "1 " : "0 ";
        buf += search_in_formulas ? "1 " : "0 ";
        buf += search_in_free_text ? "1 " : "0 ";
        buf += save_list;

        int res = B_DataSearch(buf.data(), iode_type);
        if(res < 0) 
            B_display_last_error();
        else 
        {
            List res = Lists.get(save_list);
            wResults->setQValue(QString::fromStdString(res));
        }

    }
    catch(const std::exception& e)
    {
        QMessageBox::warning(nullptr, "WARNING", QString(e.what()));
    }
}

void MenuDataSearchText::help()
{
	QDesktopServices::openUrl(url_manual);
}
