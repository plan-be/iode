#pragma once 

#include <QCompleter>
#include <QStringListModel>
#include <QRegularExpression>

#include "utils.h"


class QIodeCompleter: public QCompleter
{
    Q_OBJECT

    QStringListModel* listModel;

    QStringList reportCommandsList;
    QStringList lecFunctionsList;
    QVector<KDBAbstract*> iodeDatabases;

    int iodeType;
    bool reportCommands;
    bool lecFunctions;

public:
    QIodeCompleter(const bool reportCommands = true, const bool lecFunctions = false, 
        const int iodeType = -1, QObject *parent = nullptr);
    ~QIodeCompleter();

    void setIodeType(const int iodeType, const bool updateModel = true)
    {
        iodeDatabases.clear();

        switch (iodeType)
        {
        case I_COMMENTS:
            iodeDatabases.push_back(new KDBComments());
            break;
        case I_EQUATIONS:
            iodeDatabases.push_back(new KDBEquations());
            break;
        case I_IDENTITIES:
            iodeDatabases.push_back(new KDBIdentities());
            break;
        case I_LISTS:
            iodeDatabases.push_back(new KDBLists());
            break;
        case I_SCALARS:
            iodeDatabases.push_back(new KDBScalars());
            break;
        case I_TABLES:
            iodeDatabases.push_back(new KDBTables());
            break;
        case I_VARIABLES:
            iodeDatabases.push_back(new KDBVariables());
            break;
        default:
            iodeDatabases.push_back(new KDBComments());
            iodeDatabases.push_back(new KDBEquations());
            iodeDatabases.push_back(new KDBIdentities());
            iodeDatabases.push_back(new KDBLists());
            iodeDatabases.push_back(new KDBScalars());
            iodeDatabases.push_back(new KDBTables());
            iodeDatabases.push_back(new KDBVariables());
            break;
        }

        this->iodeType = iodeType;

        if(updateModel)
            updateIodeOjectsListNames();
    }

    void includeIodeCommand(const bool ok, const bool updateModel = true)
    {
        reportCommands = ok;

        if(updateModel)
            updateIodeOjectsListNames();
    }

    void includeLecFunctions(const bool ok, const bool updateModel = true)
    {
        lecFunctions = ok;

        if(updateModel)
            updateIodeOjectsListNames();
    }

public slots:
	/**
	 * @brief Update the list of IUode object names currently loaded in the memory 
	 * 
	 */
    void updateIodeOjectsListNames();
};
