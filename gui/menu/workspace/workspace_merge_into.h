#pragma once

#include <QWidget>
#include <QList>

#include "ui_workspace_merge_into.h"
#include "utils.h"
#include "settings.h"
#include "wrapper_classes.h"


/* NOTE FOR THE DEVELOPERS:
 * All Menu Dialog classes MUST
 * 1. inherit from QIodeSettings,
 * 2. use the extractAndVerify() method to extract input data from fields,
 * 3. call the Q_OBJECT macro at the beginning of the class to allow slots and signals (see documentation of Qt).
 */


class MenuWorkspaceMergeInto : public QIodeSettings, public Ui::MenuWorkspaceMergeInto
{
    Q_OBJECT

    WrapperComboBox*    wComboBoxObjectType;
    WrapperFileChooser* wFileChooserInputFile;

public:
    MenuWorkspaceMergeInto(QWidget* parent = nullptr);
    ~MenuWorkspaceMergeInto();

public slots:
	void merge_into_workspace();
    void help();
    void updateFileChooser(int fileType)
    {
        wFileChooserInputFile->setFileType((EnumIodeFile) fileType);
    }
};
