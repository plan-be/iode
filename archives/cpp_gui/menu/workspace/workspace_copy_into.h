#pragma once

#include <QWidget>
#include <QList>

#include "ui_workspace_copy_into.h"
#include "utils.h"
#include "settings.h"
#include "wrapper_classes.h"


/* NOTE FOR THE DEVELOPERS:
 * All Menu Dialog classes MUST
 * 1. inherit from IodeSettingsDialog,
 * 2. use the extractAndVerify() method to extract input data from fields,
 * 3. call the Q_OBJECT macro at the beginning of the class to allow slots and signals (see documentation of Qt).
 */


class MenuWorkspaceCopyInto : public IodeSettingsDialog, public Ui::MenuWorkspaceCopyInto
{
    Q_OBJECT

    WrapperComboBox*    wComboBoxObjectType;
    WrapperFileChooser* wFileChooserInputFile;
    WrapperSampleEdit*  wSampleEditSampleFrom;
    WrapperSampleEdit*  wSampleEditSampleTo;
    WrapperQTextEdit*   wTextEditObjectNames;

public:
    MenuWorkspaceCopyInto(QWidget* parent = nullptr);
    ~MenuWorkspaceCopyInto();

public slots:
	void copy_into_workspace();
    void help();
    void updateFileChooser(int fileType)
    {
        wFileChooserInputFile->setFileType((IodeFileType) fileType);
    }
};
