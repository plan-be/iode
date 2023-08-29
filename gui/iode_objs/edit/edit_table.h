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

#include "plot/plot_table.h"
#include "main_window_abstract.h"
#include "iode_objs/models/gsample_table_model.h"


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

class EditTableDialog : public IodeSettingsDialog, public Ui::EditTableDialog
{
    Q_OBJECT

    QString tableName;

    QShortcut* shortcutDelete;

    WrapperComboBox* wInsertLineType;
    WrapperComboBox* wInsertWhere;

	QShortcut* fullScreenShortcut;

    QList<QString> list_insert_types;

public:
    EditTableDialog(const QString& tableName, QWidget* parent = Q_NULLPTR);
    ~EditTableDialog();

signals:
    void newPlot(QDialog* plotDialog);
    void tableModified();

public slots:
    void edit();
    void plot();
    void insert_line();
    void delete_line();
    void help();
};
