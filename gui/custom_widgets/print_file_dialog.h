#pragma once

#include <QWidget>

#include "ui_print_file_dialog.h"
#include "utils.h"
#include "settings.h"
#include "wrapper_classes.h"
#include "menu/file/file_print_preferences.h"


class QIodePrintFileDialog : public QIodeSettings, public Ui::QIodePrintFileDialog
{
    Q_OBJECT

    WrapperComboBox*    wPrintFormat;
    WrapperFileChooser* wOutputFile;

public:
    const static QString KEY_SETTINGS_PRINT_FORMAT;
    const static QString KEY_SETTINGS_PRINT_OUTPUT_FILE;

public:
    QIodePrintFileDialog(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~QIodePrintFileDialog();

    static QChar getFormat(const QSettings* project_settings)
    {
        QChar format;
        int i_format = project_settings->value(KEY_SETTINGS_PRINT_FORMAT).toInt();

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

private slots:
    void updateOutputFileFormat(int index);

public slots:
    void apply();
    void set_print_options();
    void help();
};
