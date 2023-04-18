#pragma once

#include <QWidget>
#include <QList>

#include "ui_workspace_copy_into.h"
#include "utils.h"
#include "settings.h"
#include "wrapper_classes.h"


/* NOTE FOR THE DEVELOPERS:
 * All Menu Dialog classes MUST
 * 1. inherit from QIodeSettings,
 * 2. use the extractAndVerify() method to extract input data from fields,
 * 3. call the Q_OBJECT macro at the beginning of the class to allow slots and signals (see documentation of Qt).
 */


class QIodeMenuWorkspaceCopyInto : public QIodeSettings, public Ui::QIodeMenuWorkspaceCopyInto
{
    Q_OBJECT

    WrapperComboBox*    wComboBoxObjectType;
    WrapperFileChooser* wFileChooserInputFile;
    WrapperSampleEdit*  wSampleEditSampleFrom;
    WrapperSampleEdit*  wSampleEditSampleTo;
    WrapperQTextEdit*   wTextEditObjectNames;

public:
    QIodeMenuWorkspaceCopyInto(const QString& settings_filepath, QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~QIodeMenuWorkspaceCopyInto();

public slots:
	void copy_into_workspace();
    void help();
    void updateFileChooser(int fileType)
    {
        wFileChooserInputFile->setFileType((EnumIodeFile) fileType);
    }
};
