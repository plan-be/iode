#pragma once

#include <QWidget>
#include <QList>

#include "ui_file_print_preferences.h"
#include "utils.h"
#include "settings.h"
#include "wrapper_classes.h"


class QIodeMenuFilePrintPref : public QIodeSettings, public Ui::QIodeMenuFilePrintPref
{
    Q_OBJECT

public:
	QIodeMenuFilePrintPref(const QString& settings_filepath, QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
	~QIodeMenuFilePrintPref();

private:
    void set_printer_preferences();
    void set_A2M_preferences();
    void set_MIF_preferences();
    void set_RTF_preferences();
    void set_HTML_preferences();

public slots:
    void set_print_preferences();
    void help();
};
