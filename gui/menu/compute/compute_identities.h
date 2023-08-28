#pragma once

#include <QWidget>
#include <QStringList>

#include "ui_compute_identities.h"
#include "utils.h"
#include "settings.h"
#include "wrapper_classes.h"
#include "text_edit/completer.h"


/* NOTE FOR THE DEVELOPERS:
 * All Menu Dialog classes MUST
 * 1. inherit from QIodeSettings,
 * 2. use the extractAndVerify() method to extract input data from fields,
 * 3. call the Q_OBJECT macro at the beginning of the class to allow slots and signals (see documentation of Qt).
 */

class QIodeMenuComputeIdentities : public QIodeSettings, public Ui::QIodeMenuComputeIdentities
{
    Q_OBJECT

    
	WrapperSampleEdit*      qFrom; 
	WrapperSampleEdit*      qTo;
    WrapperQPlainTextEdit*  qIdentitiesList;
    WrapperFileChooser*     qVariablesFile1;
    WrapperFileChooser*     qVariablesFile2;
    WrapperFileChooser*     qVariablesFile3;
    WrapperFileChooser*     qScalarsFile1;
    WrapperFileChooser*     qScalarsFile2;
    WrapperFileChooser*     qScalarsFile3;
    WrapperCheckBox*        qTrace;
    WrapperComboBox*        qLanguage;

    QIodeCompleter*         completer;

public:
	QIodeMenuComputeIdentities(QWidget* parent = nullptr);
    ~QIodeMenuComputeIdentities();

    void setIdentitiesNames(const QStringList& identitiesNames)
    {
        QString inputString;
        foreach(const QString& name, identitiesNames) 
            inputString += name + ";";
        qIdentitiesList->setQValue(inputString);
    }

public slots:
    void compute();
};
