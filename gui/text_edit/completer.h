#pragma once 

#include <Map>
#include <QVector>
#include <QCompleter>
#include <QStringListModel>
#include <QRegularExpression>

#include "utils.h"


class IodeCompleter: public QCompleter
{
    Q_OBJECT

    QStringListModel* listModel;

    QStringList reportCommandsList;
    QStringList lecFunctionsList;
    QMap<EnumIodeType, KDBAbstract*> iodeDatabases;

    QVector<int> iodeTypes;
    bool reportCommands;
    bool lecFunctions;

private:
    void buildReportCommandsList()
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
    }

    void buidlLecFunctionsList()
    {
        // LEC functions list
        std::vector<std::string> func_list = build_lec_functions_list();
        for(const std::string& func_name: func_list) lecFunctionsList << QString::fromStdString(func_name);

        lecFunctionsList.sort();
    }

    void setListModel()
    {
        listModel = new QStringListModel(this);
        this->setModel(listModel);
        this->setCaseSensitivity(Qt::CaseInsensitive);
        this->setCompletionMode(QCompleter::PopupCompletion);
    }

public:
    IodeCompleter(const bool reportCommands = true, const bool lecFunctions = false, 
        const int iodeType = -1, QObject *parent = nullptr);
    IodeCompleter(const bool reportCommands = true, const bool lecFunctions = false, 
        const QVector<int>& iodeTypes = QVector<int>(), QObject *parent = nullptr);
    ~IodeCompleter();

    void addIodeType(const int iodeType, const bool updateListModel = true)
    {
        switch (iodeType)
        {
        case COMMENTS:
            iodeDatabases.insert(COMMENTS, &Comments);
            break;
        case I_EQUATIONS:
            iodeDatabases.insert(I_EQUATIONS, &Equations);
            break;
        case I_IDENTITIES:
            iodeDatabases.insert(I_IDENTITIES, &Identities);
            break;
        case I_LISTS:
            iodeDatabases.insert(I_LISTS, &Lists);
            break;
        case I_SCALARS:
            iodeDatabases.insert(I_SCALARS, &Scalars);
            break;
        case I_TABLES:
            iodeDatabases.insert(I_TABLES, &Tables);
            break;
        case I_VARIABLES:
            iodeDatabases.insert(I_VARIABLES, &Variables);
            break;
        default:
            break;
        }

        if(updateListModel)
            updateIodeOjectsListNames();

        return;
    }

    void setIodeType(const int iodeType, const bool updateListModel = true)
    {
        iodeDatabases.clear();

        if(iodeType > 0)
            addIodeType(iodeType, false);
        else
        {
            iodeDatabases.insert(COMMENTS, &Comments);
            iodeDatabases.insert(I_EQUATIONS, &Equations);
            iodeDatabases.insert(I_IDENTITIES, &Identities);
            iodeDatabases.insert(I_LISTS, &Lists);
            iodeDatabases.insert(I_SCALARS, &Scalars);
            iodeDatabases.insert(I_TABLES, &Tables);
            iodeDatabases.insert(I_VARIABLES, &Variables);
        }

        if(updateListModel)
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
