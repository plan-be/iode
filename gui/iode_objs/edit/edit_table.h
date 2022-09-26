#pragma once

#include <QWidget>
#include <QString>

#include <string>

#include "ui_edit_table.h"
#include "utils.h"
#include "wrapper_classes.h"


/* NOTE FOR THE DEVELOPERS:
 * This Dialog class MUST
 * 1. use the extractAndVerify() method to extract input data from fields,
 * 2. call the Q_OBJECT macro at the beginning of the class to allow slots and signals (see documentation of Qt).
 */

class QIodeEditTable : public QDialog, public Ui::QIodeEditTable
{
    Q_OBJECT

public:
    QIodeEditTable(const QString& tableName, const QString& settings_filepath, QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());

public slots:
    void edit();
    void help();
};
