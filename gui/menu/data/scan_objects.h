#pragma once

#include <QWidget>

#include "ui_scan_objects.h"
#include "utils.h"
#include "settings.h"
#include "wrapper_classes.h"


class QIodeMenuDataScanObjects : public QIodeSettings, public Ui::QIodeMenuDataScanObjects
{
    Q_OBJECT

    WrapperComboBox*  wComboIodeTypes;
    WrapperQTextEdit* wListsToScan;

public:
	QIodeMenuDataScanObjects(const QString& project_settings_filepath, QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~QIodeMenuDataScanObjects();

public slots:
    void scan();
    void help();
};
