#include "qiode_completer.h"

QIodeCompleter::QIodeCompleter(QObject *parent) : QCompleter(parent)
{
    iodeDatabases.push_back(new KDBComments());
    iodeDatabases.push_back(new KDBEquations());
    iodeDatabases.push_back(new KDBIdentities());
    iodeDatabases.push_back(new KDBLists());
    iodeDatabases.push_back(new KDBScalars());
    iodeDatabases.push_back(new KDBTables());
    iodeDatabases.push_back(new KDBVariables());

    std::vector<std::string> func_list;
    // $-functions
    func_list = build_command_functions_list(-1);
    for(const std::string& func_name: func_list) reportCommandsList << QString::fromStdString(func_name);
    // #-functions
    func_list = build_command_functions_list(-1, true);
    for(const std::string& func_name: func_list) reportCommandsList << QString::fromStdString(func_name);
    // @-functions
    func_list = build_report_functions_list();
    for(const std::string& func_name: func_list) reportCommandsList << QString::fromStdString(func_name);

    // LEC functions list
    func_list = build_lec_functions_list();
    for(const std::string& func_name: func_list) lecFunctionsList << QString::fromStdString(func_name);

    reportCommandsList.sort();

    listModel = new QStringListModel(reportCommandsList + lecFunctionsList, this);
    this->setModel(listModel);

    this->setCaseSensitivity(Qt::CaseInsensitive);
    this->setCompletionMode(QCompleter::PopupCompletion);
}

QIodeCompleter::~QIodeCompleter()
{
    delete listModel;
    iodeDatabases.clear();
}

void QIodeCompleter::updateIodeOjectsListNames()
{
    // build list of all Iode objects
    QStringList objectNames;
    for(int i=0; i < I_NB_TYPES; i++)
        for(const std::string& obj_name: iodeDatabases[i]->get_names()) objectNames << QString::fromStdString(obj_name);
    objectNames.sort();
    objectNames.removeDuplicates();

    // concatenate report commands/functions and Iode object names
    QStringList list = reportCommandsList + lecFunctionsList + objectNames;

    // update completer
    listModel->setStringList(list);
}
