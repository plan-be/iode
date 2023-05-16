#pragma once

#include <QList>
#include <QWidget>
#include <QString>
#include <QShortcut>

#include <string>

#include "ui_edit_table.h"
#include "utils.h"
#include "settings.h"
#include "wrapper_classes.h"


enum EnumInsertWhere
{
    INSERT_AT_THE_END,
    INSERT_AFTER_CURRENT,
    INSERT_BEFORE_CURRENT
};

const static QList<QString> list_insert_where = {"At the end", "After current", "Before current"};


/* NOTE FOR THE DEVELOPERS:
 * This Dialog class MUST
 * 1. use the extractAndVerify() method to extract input data from fields,
 * 2. call the Q_OBJECT macro at the beginning of the class to allow slots and signals (see documentation of Qt).
 */

class QIodeEditTable : public QIodeSettings, public Ui::QIodeEditTable
{
    Q_OBJECT

    QShortcut* shortcutDelete;

    WrapperComboBox* wInsertLineType;
    WrapperComboBox* wInsertWhere;

    QList<QString> list_insert_types;

public:
    QIodeEditTable(const QString& tableName, QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
    ~QIodeEditTable();

public slots:
    void edit();
    void insert_line();
    void delete_line();
    void help();
};
