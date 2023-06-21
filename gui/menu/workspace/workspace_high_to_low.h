#pragma once

#include <QWidget>

#include "ui_workspace_high_to_low.h"
#include "utils.h"
#include "settings.h"
#include "wrapper_classes.h"


/* NOTE FOR THE DEVELOPERS:
 * All Menu Dialog classes MUST
 * 1. inherit from QIodeSettings,
 * 2. use the extractAndVerify() method to extract input data from fields,
 * 3. call the Q_OBJECT macro at the beginning of the class to allow slots and signals (see documentation of Qt).
 */


class QIodeMenuWorkspaceHighToLow : public QIodeSettings, public Ui::QIodeMenuWorkspaceHighToLow
{
    Q_OBJECT

    WrapperFileChooser* wInputFile;
    WrapperQTextEdit*   wLastObs;
    WrapperQTextEdit*   wMean;
    WrapperQTextEdit*   wSum;

public:
	QIodeMenuWorkspaceHighToLow(QWidget* parent = nullptr);
	~QIodeMenuWorkspaceHighToLow();

public slots:
    void slot_high_to_low();
    void help();
};