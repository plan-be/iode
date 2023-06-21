#pragma once

#include <QWidget>

#include "ui_file_contents.h"
#include "utils.h"
#include "settings.h"
#include "wrapper_classes.h"


class QIodeMenuDataFileContents : public QIodeSettings, public Ui::QIodeMenuDataFileContents
{
    Q_OBJECT

    WrapperQLineEdit*   wSearch;
    WrapperComboBox*    wComboIodeTypes;
    WrapperFileChooser* wInputFile;
    WrapperQLineEdit*   wSaveList;
    WrapperQTextEdit*   wResult;

public:
	QIodeMenuDataFileContents(QWidget* parent = nullptr);
    ~QIodeMenuDataFileContents();

public slots:
    void search();
    void help();
    void updateFileChooser(int fileType)
    {
        wInputFile->setFileType((EnumIodeFile) fileType);
    }
};
