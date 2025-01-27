#pragma once

#include <QWidget>
#include <QDoubleValidator>

#include "ui_file_compare.h"
#include "utils.h"
#include "settings.h"
#include "wrapper_classes.h"


class MenuDataFileCompare : public IodeSettingsDialog, public Ui::MenuDataFileCompare
{
    Q_OBJECT

    WrapperComboBox*    wComboIodeType;
    WrapperQLineEdit*   wThreshold;
    WrapperFileChooser* wInputFile;
    WrapperQLineEdit*   wResultWSOnly;
    WrapperQLineEdit*   wResultFileOnly;
    WrapperQLineEdit*   wResultBothEqual;
    WrapperQLineEdit*   wResultBothDifferent;

public:
	MenuDataFileCompare(QWidget* parent = nullptr);
    ~MenuDataFileCompare();

public slots:
    void compare();
    void help();
    void updateFileChooser(int fileType)
    {
        wInputFile->setFileType((IodeFileType) fileType);
    }
    void updateThreshold(int iode_type)
    {
        if(iode_type == VARIABLES)
            wThreshold->getQField().setEnabled(true);
        else
            wThreshold->getQField().setEnabled(false);
    }
};
