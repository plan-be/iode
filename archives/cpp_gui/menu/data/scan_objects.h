#pragma once

#include <QWidget>

#include "ui_scan_objects.h"
#include "utils.h"
#include "settings.h"
#include "wrapper_classes.h"
#include "text_edit/completer.h"


class MenuDataScanObjects : public IodeSettingsDialog, public Ui::MenuDataScanObjects
{
    Q_OBJECT

    WrapperComboBox*       wComboIodeType;
    WrapperQPlainTextEdit* wListsToScan;

    IodeCompleter* completer;

public:
	MenuDataScanObjects(QWidget* parent = nullptr);
    ~MenuDataScanObjects();

public slots:
    void scan();
    void help();
};
