#pragma once

#include <QWidget>
#include <QList>

#include "ui_file_print_setup.h"
#include "utils.h"
#include "settings.h"
#include "wrapper_classes.h"
#include "print/file_print_preferences.h"


class MenuFilePrintSetup : public IodeSettingsDialog, public Ui::MenuFilePrintSetup
{
    Q_OBJECT

    WrapperComboBox* wPrintDest;

public:
    const static QString KEY_SETTINGS_PRINT_DEST;

public:
    MenuFilePrintSetup(QWidget* parent = nullptr);
    ~MenuFilePrintSetup();

public slots:
    void apply();
    void set_print_options();
    void help();
};
