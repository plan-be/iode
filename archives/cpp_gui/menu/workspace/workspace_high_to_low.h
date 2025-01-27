#pragma once

#include <QWidget>

#include "ui_workspace_high_to_low.h"
#include "utils.h"
#include "settings.h"
#include "wrapper_classes.h"


/* NOTE FOR THE DEVELOPERS:
 * All Menu Dialog classes MUST
 * 1. inherit from IodeSettingsDialog,
 * 2. use the extractAndVerify() method to extract input data from fields,
 * 3. call the Q_OBJECT macro at the beginning of the class to allow slots and signals (see documentation of Qt).
 */


class MenuWorkspaceHighToLow : public IodeSettingsDialog, public Ui::MenuWorkspaceHighToLow
{
    Q_OBJECT

    WrapperFileChooser* wInputFile;
    WrapperQTextEdit*   wLastObs;
    WrapperQTextEdit*   wMean;
    WrapperQTextEdit*   wSum;

public:
	MenuWorkspaceHighToLow(QWidget* parent = nullptr);
	~MenuWorkspaceHighToLow();

public slots:
    void slot_high_to_low();
    void help();
};