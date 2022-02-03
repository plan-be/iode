#pragma once

#include <QWidget>
#include <QString>
#include <QMap>

#include <string>

#include "../../utils.h"
#include "../../wrapper_classes.h"


/* NOTE FOR THE DEVELOPERS:
 * This Dialog class MUST
 * 1. use the extractAndVerify() method to extract input data from fields,
 * 2. call the Q_OBJECT macro at the beginning of the class to allow slots and signals (see documentation of Qt).
 */


class QIodeEditTable : public QDialog
{
    Q_OBJECT

    KDBTables kdb;

    QMap<QString, BaseWrapper*> map_items;

public:
    QIodeEditTable(const QString& equationName, QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
    ~QIodeEditTable();

public slots:
    void edit();
    void help();
};
