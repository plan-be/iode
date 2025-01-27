#include "list_calculus.h"

MenuDataListCalculus::MenuDataListCalculus(QWidget* parent)
    : IodeSettingsDialog(parent)
{
    setupUi(this);

    completer = new IodeCompleter(false, false, LISTS, this);
    lineEdit_list1->setCompleter(completer);
    lineEdit_list2->setCompleter(completer);

    QStringList listOperators;
    listOperators << "+" << "*" << "-" << "x";

    wList1 = new WrapperQLineEdit("list1", *lineEdit_list1, REQUIRED_FIELD);
    wList2 = new WrapperQLineEdit("list2", *lineEdit_list2, REQUIRED_FIELD);
    wListRes = new WrapperQLineEdit("listRes", *lineEdit_list_res, REQUIRED_FIELD);
    wComboOperator = new WrapperComboBox("operator", *comboBox_operator, REQUIRED_FIELD, listOperators);
    wResults = new WrapperQTextEdit("results", *textEdit_res, OPTIONAL_FIELD);

    mapFields["List1"] = wList1;
    mapFields["List2"] = wList2;
    mapFields["ListRes"] = wListRes;

    className = "MENU_DATA_LIST_CALCULUS";
    loadSettings();
}

MenuDataListCalculus::~MenuDataListCalculus()
{
    delete wList1;
    delete wList2;
    delete wListRes;
    delete wComboOperator;
    delete wResults;

    delete completer;
}

// TODO ALD: implement a calculus() method in KDBLists + tests
//    B_DataUpdate("LST1 A,B,C", LISTS);
//    B_DataUpdate("LST2 C,D,E", LISTS);
//
//    rc = B_DataCalcLst("_RES LST1 + LST2");
//    cond = (rc == 0) && U_cmp_strs(KLPTR("_RES"), "A;B;C;D;E");
//    S4ASSERT(cond == 1, "B_DataCalcLst(\"_RES LST1 + LST2\") = '%s'", KLPTR("_RES"));
//
//    rc = B_DataCalcLst("_RES LST1 * LST2");
//    cond = (rc == 0) && U_cmp_strs(KLPTR("_RES"), "C");
//    S4ASSERT(cond == 1, "B_DataCalcLst(\"_RES LST1 * LST2\") = '%s'", KLPTR("_RES"));
//
//    rc = B_DataCalcLst("_RES LST1 - LST2");
//    cond = (rc == 0) && U_cmp_strs(KLPTR("_RES"), "A;B");
//    S4ASSERT(cond == 1, "B_DataCalcLst(\"_RES LST1 - LST2\") = '%s'", KLPTR("_RES"));
//
//    rc = B_DataCalcLst("_RES LST1 x LST2");
//    cond = (rc == 0) && U_cmp_strs(KLPTR("_RES"), "AC;AD;AE;BC;BD;BE;CC;CD;CE");
//    S4ASSERT(cond == 1, "B_DataCalcLst(\"_RES LST1 x LST2\") = '%s'", KLPTR("_RES"));
void MenuDataListCalculus::calculus()
{
    try
    {
        std::string list1 = wList1->extractAndVerify().toStdString();
        std::string list2 = wList2->extractAndVerify().toStdString();
        std::string list_res = wListRes->extractAndVerify().toStdString();
        std::string op = comboBox_operator->currentText().toStdString();

        std::string buf = list_res + " " + list1 + " " + op + " " + list2;

        int res = B_DataCalcLst(buf.data());
        if(res < 0) 
            B_display_last_error();
        else 
        {
            List res = Lists.get(list_res);
            wResults->setQValue(QString::fromStdString(res));

            int count = B_DataListCount(list_res.data());
            label_count->setText(QString::number(count));
        }
    }
    catch(const std::exception& e)
    {
        QMessageBox::warning(nullptr, "WARNING", QString(e.what()));
    }
}

void MenuDataListCalculus::help()
{
	QDesktopServices::openUrl(url_manual);
}