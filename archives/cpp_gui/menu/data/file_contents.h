#pragma once

#include <QWidget>

#include "ui_file_contents.h"
#include "utils.h"
#include "settings.h"
#include "wrapper_classes.h"


class MenuDataFileContents : public IodeSettingsDialog, public Ui::MenuDataFileContents
{
    Q_OBJECT

    WrapperQLineEdit*   wSearch;
    WrapperComboBox*    wComboIodeType;
    WrapperFileChooser* wInputFile;
    WrapperQLineEdit*   wSaveList;
    WrapperQTextEdit*   wResult;

public:
	MenuDataFileContents(QWidget* parent = nullptr);
    ~MenuDataFileContents();

public slots:
    void search();
    void help();
    void updateFileChooser(int fileType)
    {
        wInputFile->setFileType((IodeFileType) fileType);
    }
};
