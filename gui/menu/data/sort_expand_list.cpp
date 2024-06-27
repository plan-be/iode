#include "sort_expand_list.h"

MenuDataSortExpandList::MenuDataSortExpandList(QWidget* parent)
    : IodeSettingsDialog(parent)
{
    setupUi(this);

    completer = new IodeCompleter(false, false, LISTS, this);
    lineEdit_sort->setCompleter(completer);

    wLineSort = new WrapperQLineEdit(label_sort->text(), *lineEdit_sort, REQUIRED_FIELD);
    wLineSave = new WrapperQLineEdit(label_save->text(), *lineEdit_save, REQUIRED_FIELD);

    mapFields["Sort"] = wLineSort;
    mapFields["Save"] = wLineSave;

    className = "MENU_DATA_SORT_EXPLAND_LIST";
    loadSettings();
}

MenuDataSortExpandList::~MenuDataSortExpandList()
{
    delete wLineSort;
    delete wLineSave;

    delete completer;
}

// TODO ALD: implement a sort() method in KDBLists
// for the tests:
//     rc = K_add(KL_WS, "LIST1", "A;C;B");
//    S4ASSERT(K_find(KL_WS, "LIST1") >= 0,  "K_add(KL_WS, \"LIST1\", \"A;C;B\")");
//    rc = B_DataListSort("LIST1 LIST2");
//    lst = KLPTR("LIST2");
//    S4ASSERT(U_cmp_strs(lst, "A;B;C"), "B_DataListSort(\"LIST1 LIST2\") = \"%s\"", lst);
void MenuDataSortExpandList::sort()
{
    try
    {
        std::string sort_list = wLineSort->extractAndVerify().toStdString();
        std::string save_list = wLineSave->extractAndVerify().toStdString();

        std::string buf = sort_list + " " + save_list;
        int res = B_DataListSort(buf.data());
        if(res < 1)
            B_display_last_error();

        this->accept();
    }
    catch(const std::exception& e)
    {
        QMessageBox::warning(nullptr, "WARNING", QString(e.what()));
    }
}

void MenuDataSortExpandList::help()
{
	QDesktopServices::openUrl(url_manual);
}