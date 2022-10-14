#pragma once

#include <QWidget>

#include "ui_compute_identities.h"
#include "utils.h"
#include "settings.h"
#include "wrapper_classes.h"


/* NOTE FOR THE DEVELOPERS:
 * All Menu Dialog classes MUST
 * 1. inherit from QIodeSettings,
 * 2. use the extractAndVerify() method to extract input data from fields,
 * 3. call the Q_OBJECT macro at the beginning of the class to allow slots and signals (see documentation of Qt).
 */

class QIodeMenuComputeIdentities : public QIodeSettings, public Ui::QIodeMenuComputeIdentities
{
    Q_OBJECT

    
	WrapperSampleEdit* qFrom; 
	WrapperSampleEdit* qTo;
    WrapperQTextEdit*  qIdentitiesList;
    WrapperQTextEdit*  qVariablesFiles;
    WrapperQTextEdit*  qScalarsFiles;
    WrapperCheckBox*   qTrace;
    WrapperComboBox*   qLanguage;

public:
	QIodeMenuComputeIdentities(const QString& project_settings_filepath, QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~QIodeMenuComputeIdentities();

public slots:
    void compute();
};
