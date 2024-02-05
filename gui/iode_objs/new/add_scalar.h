#pragma once

#include <QWidget>
#include <QString>

#include <string>

#include "ui_add_scalar.h"
#include "utils.h"
#include "settings.h"
#include "wrapper_classes.h"
#include "iode_objs/models/scalars_model.h"


/* NOTE FOR THE DEVELOPERS:
 * This Dialog class MUST
 * 1. use the extractAndVerify() method to extract input data from fields,
 * 2. call the Q_OBJECT macro at the beginning of the class to allow slots and signals (see documentation of Qt).
 */

class AddScalarDialog : public IodeSettingsDialog, public Ui::AddScalarDialog
{
    Q_OBJECT
    KDBScalars* database;

    WrapperIodeNameEdit* lineName;
    WrapperQLineEdit* lineValue;
    WrapperDoubleSpinBox* spinBoxRelax;

public:
    AddScalarDialog(KDBScalars* database, QWidget* parent = Q_NULLPTR);
    ~AddScalarDialog();

signals:
    void newObjectInserted(QString);

public slots:
    void add();
    void help();
};
