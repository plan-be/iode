#include "qiode_completer.h"


QIodeCompleter::QIodeCompleter(const bool reportCommands, const bool lecFunctions, const int iodeType, QObject *parent) 
    : QCompleter(parent), reportCommands(reportCommands), lecFunctions(lecFunctions)
{
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

    reportCommandsList.sort();

    // LEC functions list
    func_list = build_lec_functions_list();
    for(const std::string& func_name: func_list) lecFunctionsList << QString::fromStdString(func_name);

    lecFunctionsList = lecFunctionsList;

    setIodeType(iodeType, false);

    listModel = new QStringListModel(this);
    updateIodeOjectsListNames();
    
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
    QStringList list;

    // concatenate report commands/functions and Iode object names
    if(reportCommands)
        list += reportCommandsList;

    if(lecFunctions)
        list += lecFunctionsList;

    // build list of all Iode objects
    QStringList objectNames;
    for(int i=0; i < iodeDatabases.size(); i++)
        for(const std::string& obj_name: iodeDatabases[i]->get_names()) objectNames << QString::fromStdString(obj_name);
    objectNames.sort();
    objectNames.removeDuplicates();
    list += objectNames;

    // update completer
    listModel->setStringList(list);
}
