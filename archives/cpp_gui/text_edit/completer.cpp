#include "completer.h"


IodeCompleter::IodeCompleter(const bool reportCommands, const bool lecFunctions, const int iodeType, QObject *parent) 
    : QCompleter(parent), reportCommands(reportCommands), lecFunctions(lecFunctions)
{
    buildReportCommandsList();
    buidlLecFunctionsList();
    setListModel();

    setIodeType(iodeType, true);
}

IodeCompleter::IodeCompleter(const bool reportCommands, const bool lecFunctions, const QVector<int>& iodeTypes, QObject *parent) 
    : QCompleter(parent), reportCommands(reportCommands), lecFunctions(lecFunctions)
{
    buildReportCommandsList();
    buidlLecFunctionsList();
    setListModel();

    foreach(const int iodeType, iodeTypes)
        addIodeType(iodeType, false);
    updateIodeOjectsListNames();
}

IodeCompleter::~IodeCompleter()
{
    delete listModel;
    iodeDatabases.clear();
}

void IodeCompleter::updateIodeOjectsListNames()
{
    QStringList list;

    // concatenate report commands/functions and Iode object names
    if(reportCommands)
        list += reportCommandsList;

    if(lecFunctions)
        list += lecFunctionsList;

    // build list of all Iode objects
    QStringList objectNames;
    foreach(KDBAbstract* kdb, iodeDatabases)
        for(const std::string& obj_name: kdb->get_names()) 
            objectNames << QString::fromStdString(obj_name);
    objectNames.sort();
    objectNames.removeDuplicates();
    list += objectNames;

    // update completer
    listModel->setStringList(list);
}
