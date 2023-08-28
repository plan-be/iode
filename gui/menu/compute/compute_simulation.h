#pragma once

#include <QWidget>

#include "ui_compute_simulation.h"
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

class MenuComputeSimulation : public IodeSettingsDialog, public Ui::MenuComputeSimulation
{
    Q_OBJECT

    QList<QString>   qInitialisationList;
    QList<QString>   qSortAlgoList;
    IodeCompleter*  completer_eqs;
    IodeCompleter*  completer_var;

    WrapperQPlainTextEdit* qEquationsList;
    WrapperQPlainTextEdit* qExchange;
    WrapperSampleEdit*     qFrom;
    WrapperSampleEdit*     qTo;
    WrapperQLineEdit*      qConvergence;
    WrapperSpinBox*        qMaxIterations;
    WrapperCheckBox*       qCheckDebug;
    WrapperQLineEdit*      qRelaxation;
    WrapperComboBox*       qSimInitialisation;
    WrapperComboBox*       qSortAlgorithm;
    WrapperSpinBox*        qNbPasses;

public:
	MenuComputeSimulation(QWidget* parent = nullptr);
    ~MenuComputeSimulation();

public slots:
    void compute();
};
