#pragma once

#include <QWidget>
#include <QList>

#include "ui_file_settings.h"
#include "utils.h"
#include "settings.h"
#include "wrapper_classes.h"
#include "print/file_print_preferences.h"


class MenuFileSettings : public IodeSettingsDialog, public Ui::MenuFileSettings
{
    Q_OBJECT

    WrapperComboBox*    wPrintDest;
    WrapperRadioButton* wRunReportsFromProjectDir;
    WrapperRadioButton* wRunReportsFromParentDir;

public:
    const static QString KEY_SETTINGS_RUN_REPORTS_FROM_PROJECT_DIR;
    const static QString KEY_SETTINGS_PRINT_DEST;

public:
    MenuFileSettings(QWidget* parent = nullptr);
    ~MenuFileSettings();

    void showPrintTab()
    {
        tabWidget->setCurrentWidget(tab_reports);
    }

public slots:
    void apply();
    void set_print_options();
    void help();
};
