#pragma once

#include <QWidget>
#include <QString>

#include <string>

#include "ui_add_object.h"
#include "iode_objs/edit/edit_vars_sample.h"
#include "utils.h"
#include "wrapper_classes.h"
#include "iode_objs/models/variables_model.h"


/* NOTE FOR THE DEVELOPERS:
 * This Dialog class MUST
 * 1. use the extractAndVerify() method to extract input data from fields,
 * 2. call the Q_OBJECT macro at the beginning of the class to allow slots and signals (see documentation of Qt).
 */

class AddVariableDialog : public QDialog, public Ui::AddObjectDialog
{
    Q_OBJECT
    KDBVariables* database;

    WrapperIodeNameEdit* lineName;
    WrapperQLineEdit* lineDefinition;

public:
    AddVariableDialog(KDBVariables* database, QWidget* parent = Q_NULLPTR);

signals:
    void newObjectInserted(QString);

public slots:
    void add();
    void help();
};
