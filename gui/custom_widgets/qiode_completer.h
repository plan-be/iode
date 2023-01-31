#pragma once 

#include <QCompleter>
#include <QStringListModel>

#include "utils.h"


class QIodeCompleter: public QCompleter
{
    Q_OBJECT

    QStringListModel* listModel;
    QStringList reportCommandsList;
    QStringList lecFunctionsList;
    QVector<KDBAbstract*> iodeDatabases;

public:
    QIodeCompleter(QObject *parent = nullptr);
    ~QIodeCompleter();

public slots:
	/**
	 * @brief Update the list of IUode object names currently loaded in the memory 
	 * 
	 */
    void updateIodeOjectsListNames();
};
