#pragma once

#include <QWidget>

#include <vector>

#include "ui_workspace_low_to_high.h"
#include "utils.h"
#include "settings.h"
#include "wrapper_classes.h"


/* NOTE FOR THE DEVELOPERS:
 * All Menu Dialog classes MUST
 * 1. inherit from QIodeSettings,
 * 2. use the extractAndVerify() method to extract input data from fields,
 * 3. call the Q_OBJECT macro at the beginning of the class to allow slots and signals (see documentation of Qt).
 */


class QIodeMenuWorkspaceLowToHigh : public QIodeSettings, public Ui::QIodeMenuWorkspaceLowToHigh
{
    Q_OBJECT

    WrapperFileChooser* wInputFile;
    WrapperComboBox* wMethod;
    WrapperQTextEdit* wStockSeries;
    WrapperQTextEdit* wFlowSeries;

public:
	QIodeMenuWorkspaceLowToHigh(const QString& settings_filepath, QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
	~QIodeMenuWorkspaceLowToHigh();

public slots:
    void slot_low_to_high();
    void help();
};