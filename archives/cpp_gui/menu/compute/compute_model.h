#pragma once

#include <QWidget>

#include "ui_compute_model.h"
#include "utils.h"
#include "settings.h"
#include "wrapper_classes.h"
#include "text_edit/completer.h"


/* NOTE FOR THE DEVELOPERS:
 * All Menu Dialog classes MUST
 * 1. inherit from IodeSettingsDialog,
 * 2. use the extractAndVerify() method to extract input data from fields,
 * 3. call the Q_OBJECT macro at the beginning of the class to allow slots and signals (see documentation of Qt).
 */

class MenuComputeModel : public IodeSettingsDialog, public Ui::MenuComputeModel
{
    Q_OBJECT

    WrapperQPlainTextEdit* qEquationsList;
    IodeCompleter*        completer;

public:
	MenuComputeModel(QWidget* parent = nullptr);
    ~MenuComputeModel();

public slots:
    void compute();
};
