#pragma once

#include <QWidget>

#include "ui_compute_scc_decomposition.h"
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

class MenuComputeSCCDecomposition : public IodeSettingsDialog, public Ui::MenuComputeSCCDecomposition
{
    Q_OBJECT

    WrapperQPlainTextEdit*  qEquationsList;
    WrapperSpinBox*         qTriangulationIterations;
    WrapperQLineEdit*       qPreRecursiveListName;
    WrapperQLineEdit*       qInterRecursiveListName;
    WrapperQLineEdit*       qPostRecursiveListName;

    IodeCompleter*         completer;

public:
	MenuComputeSCCDecomposition(QWidget* parent = nullptr);
    ~MenuComputeSCCDecomposition();

public slots:
    void compute();
};