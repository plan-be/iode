#pragma once

#include <QWidget>
#include <QString>

#include <string>

#include "ui_add_table.h"
#include "utils.h"
#include "settings.h"
#include "wrapper_classes.h"
#include "text_edit/completer.h"
#include "iode_objs/models/tables_model.h"


/* NOTE FOR THE DEVELOPERS:
 * This Dialog class MUST
 * 1. use the extractAndVerify() method to extract input data from fields,
 * 2. call the Q_OBJECT macro at the beginning of the class to allow slots and signals (see documentation of Qt).
 */

class AddTableDialog : public IodeSettingsDialog, public Ui::AddTableDialog
{
    Q_OBJECT
    KDBTables* database;

    WrapperIodeNameEdit*   lineName;
    WrapperSpinBox*        spinNbColumns;
    WrapperQLineEdit*      lineTitle;
    WrapperQPlainTextEdit* textLEC;
    WrapperCheckBox*       checkBoxMode;
    WrapperCheckBox*       checkBoxFiles;
    WrapperCheckBox*       checkBoxDate;

    IodeCompleter* completer;

public:
    AddTableDialog(KDBTables* database, QWidget* parent = Q_NULLPTR);
    ~AddTableDialog();

signals:
    void newObjectInserted(QString);

public slots:
    void add();
    void help();
};