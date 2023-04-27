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
    QIodePrintFileDialog(const QString& settings_filepath, QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~QIodePrintFileDialog();

private slots:
    void updateOutputFileFormat(int index);

public slots:
    void apply();
    void set_print_options();
    void help();
};
