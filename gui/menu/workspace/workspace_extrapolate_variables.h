#pragma once

#include <QWidget>
#include <QList>

#include "ui_workspace_extrapolate_variables.h"
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


class MenuWorkspaceExtrapolateVariables : public IodeSettingsDialog, public Ui::MenuWorkspaceExtrapolateVariables
{
    Q_OBJECT

    WrapperSampleEdit*      wFrom;
    WrapperSampleEdit*      wTo;
    WrapperComboBox*        wMethod;
    WrapperQPlainTextEdit*  wVarsList;

    IodeCompleter*         completer;

public:
    MenuWorkspaceExtrapolateVariables(QWidget* parent = nullptr);
    ~MenuWorkspaceExtrapolateVariables();

public slots:
	void extrapolate_variables();
    void help();
};
