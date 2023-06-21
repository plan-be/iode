#pragma once

#include <QWidget>

#include "ui_scan_objects.h"
#include "utils.h"
#include "settings.h"
#include "wrapper_classes.h"
#include "custom_widgets/completer.h"


class QIodeMenuDataScanObjects : public QIodeSettings, public Ui::QIodeMenuDataScanObjects
{
    Q_OBJECT

    WrapperComboBox*       wComboIodeTypes;
    WrapperQPlainTextEdit* wListsToScan;

    QIodeCompleter* completer;

public:
	QIodeMenuDataScanObjects(QWidget* parent = nullptr);
    ~QIodeMenuDataScanObjects();

public slots:
    void scan();
    void help();
};
