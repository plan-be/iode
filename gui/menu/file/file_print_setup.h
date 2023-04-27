#pragma once

#include <QWidget>
#include <QList>

#include "ui_file_print_setup.h"
#include "utils.h"
#include "settings.h"
#include "wrapper_classes.h"
#include "file_print_preferences.h"


class QIodeMenuFilePrintSetup : public QIodeSettings, public Ui::QIodeMenuFilePrintSetup
{
    Q_OBJECT

    WrapperComboBox* wPrintDest;

public:
    const static QString KEY_SETTINGS_PRINT_DEST;

public:
    QIodeMenuFilePrintSetup(const QString& settings_filepath, QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~QIodeMenuFilePrintSetup();

public slots:
    void apply();
    void set_print_options();
    void help();
};
