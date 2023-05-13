#pragma once

#include <QWidget>
#include <QDoubleValidator>

#include "ui_file_compare.h"
#include "utils.h"
#include "settings.h"
#include "wrapper_classes.h"


class QIodeMenuDataFileCompare : public QIodeSettings, public Ui::QIodeMenuDataFileCompare
{
    Q_OBJECT

    WrapperComboBox*    wComboIodeTypes;
    WrapperQLineEdit*   wThreshold;
    WrapperFileChooser* wInputFile;
    WrapperQLineEdit*   wResultWSOnly;
    WrapperQLineEdit*   wResultFileOnly;
    WrapperQLineEdit*   wResultBothEqual;
    WrapperQLineEdit*   wResultBothDifferent;

public:
	QIodeMenuDataFileCompare(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~QIodeMenuDataFileCompare();

public slots:
    void compare();
    void help();
    void updateFileChooser(int fileType)
    {
        wInputFile->setFileType((EnumIodeFile) fileType);
    }
    void updateThreshold(int iode_type)
    {
        if(iode_type == I_VARIABLES)
            wThreshold->getQField().setEnabled(true);
        else
            wThreshold->getQField().setEnabled(false);
    }
};
