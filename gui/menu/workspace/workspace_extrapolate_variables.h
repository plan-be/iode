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
 * 1. inherit from QIodeSettings,
 * 2. use the extractAndVerify() method to extract input data from fields,
 * 3. call the Q_OBJECT macro at the beginning of the class to allow slots and signals (see documentation of Qt).
 */


class QIodeMenuWorkspaceExtrapolateVariables : public QIodeSettings, public Ui::QIodeMenuWorkspaceExtrapolateVariables
{
    Q_OBJECT

    WrapperSampleEdit*      wFrom;
    WrapperSampleEdit*      wTo;
    WrapperComboBox*        wMethod;
    WrapperQPlainTextEdit*  wVarsList;

    IodeCompleter*         completer;

public:
    QIodeMenuWorkspaceExtrapolateVariables(QWidget* parent = nullptr);
    ~QIodeMenuWorkspaceExtrapolateVariables();

public slots:
	void extrapolate_variables();
    void help();
};
