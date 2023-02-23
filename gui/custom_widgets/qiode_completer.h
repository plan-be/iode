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

    bool lecFunctions;

public:
    QIodeCompleter(const bool lecFunctions = false, QObject *parent = nullptr);
    ~QIodeCompleter();

public slots:
	/**
	 * @brief Update the list of IUode object names currently loaded in the memory 
	 * 
	 */
    void updateIodeOjectsListNames();
};
