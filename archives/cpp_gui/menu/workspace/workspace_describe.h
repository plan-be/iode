#pragma once

#include <QWidget>
#include <QList>

#include "ui_workspace_describe.h"
#include "utils.h"
#include "settings.h"
#include "wrapper_classes.h"


/* NOTE FOR THE DEVELOPERS:
 * All Menu Dialog classes MUST
 * 1. inherit from IodeSettingsDialog,
 * 2. use the extractAndVerify() method to extract input data from fields,
 * 3. call the Q_OBJECT macro at the beginning of the class to allow slots and signals (see documentation of Qt).
 */


class MenuWorkspaceDescribe : public IodeSettingsDialog, public Ui::MenuWorkspaceDescribe
{
    Q_OBJECT

    WrapperQLineEdit*    wCommentsDescription;
    WrapperQLineEdit*    wEquationsDescription;
    WrapperQLineEdit*    wIdentitiesDescription;
    WrapperQLineEdit*    wListsDescription;
    WrapperQLineEdit*    wScalarsDescription;
    WrapperQLineEdit*    wTablesDescription;
    WrapperQLineEdit*    wVariablesDescription;

public:
    MenuWorkspaceDescribe(QWidget* parent = nullptr);
    ~MenuWorkspaceDescribe();

public slots:
	void update_descriptions();
    void help();
};
