#pragma once

#include <QWidget>

#include "ui_print_file_dialog.h"
#include "utils.h"
#include "settings.h"
#include "wrapper_classes.h"
#include "file_print_preferences.h"


class PrintFileDialog : public IodeSettingsDialog, public Ui::PrintFileDialog
{
    Q_OBJECT

    WrapperComboBox*    wPrintFormat;
    WrapperFileChooser* wOutputFile;

public:
    PrintFileDialog(QWidget* parent = nullptr, const QString& outputFile = "", const QChar& format = QChar::Null);
    ~PrintFileDialog();

    QChar getFormat() const
    {
        QChar format;
        int i_format = wPrintFormat->extractAndVerify();

        switch(i_format) 
        {
        case 0:
            format = 'A';
            break;
        case 1:  
            format = 'R'; 
            break;
        case 2:  
            format = 'H'; 
            break;
        case 3:  
            format = 'M'; 
            break;
        case 4:  
            format = 'C'; 
            break;
        default: 
            format = QChar::Null;
            break;
        }

        return format;
    }

    void setFormat(const QChar& format)
    {
        char c_format = format.toLatin1();
        switch(c_format) 
        {
        case 'A':
            wPrintFormat->setQValue(0);
            break;
        case 'R':  
            wPrintFormat->setQValue(1);
            break;
        case 'H':  
            wPrintFormat->setQValue(2);
            break;
        case 'M':  
            wPrintFormat->setQValue(3);
            break;
        case 'C':  
            wPrintFormat->setQValue(4); 
            break;
        default: 
            break;
        }
    }

    QString getOutputFile() const
    {
        return wOutputFile->extractAndVerify();
    }

    void setOutputFile(const QString& outputFile)
    {
        wOutputFile->setQValue(outputFile);
    }

private slots:
    void updateOutputFileFormat(int index);

public slots:
    void apply();
    void set_print_options();
    void help();
};
