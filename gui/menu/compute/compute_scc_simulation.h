#pragma once

#include <QWidget>

#include "ui_compute_scc_simulation.h"
#include "utils.h"
#include "settings.h"
#include "wrapper_classes.h"


/* NOTE FOR THE DEVELOPERS:
 * All Menu Dialog classes MUST
 * 1. inherit from QIodeSettings,
 * 2. use the extractAndVerify() method to extract input data from fields,
 * 3. call the Q_OBJECT macro at the beginning of the class to allow slots and signals (see documentation of Qt).
 */

class QIodeMenuComputeSCCSimulation : public QIodeSettings, public Ui::QIodeMenuComputeSCCSimulation
{
    Q_OBJECT

    QList<QString> qInitialisationList;

    WrapperSampleEdit* qFrom;
    WrapperSampleEdit* qTo;
    WrapperQLineEdit* qConvergence;
    WrapperSpinBox* qMaxIterations;
    WrapperCheckBox* qCheckDebug;
    WrapperQLineEdit* qRelaxation;
    WrapperComboBox* qSimInitialisation;
    WrapperQLineEdit* qPreRecursiveListName;
    WrapperQLineEdit* qInterRecursiveListName;
    WrapperQLineEdit* qPostRecursiveListName;

public:
	QIodeMenuComputeSCCSimulation(const QString& project_settings_filepath, QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~QIodeMenuComputeSCCSimulation();

public slots:
    void compute();
};
