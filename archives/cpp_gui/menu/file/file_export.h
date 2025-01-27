#pragma once

#include <QWidget>

#include "ui_file_export.h"
#include "utils.h"
#include "settings.h"
#include "wrapper_classes.h"


/* NOTE FOR THE DEVELOPERS:
 * All Menu Dialog classes MUST
 * 1. inherit from IodeSettingsDialog,
 * 2. use the extractAndVerify() method to extract input data from fields,
 * 3. call the Q_OBJECT macro at the beginning of the class to allow slots and signals (see documentation of Qt).
 */

class MenuFileExport: public IodeSettingsDialog, public Ui::MenuFileExport
{
    Q_OBJECT

    WrapperFileChooser*  wVarFile;
    WrapperFileChooser*  wCmtFile;
    WrapperFileChooser*  wRuleFile;
	WrapperSampleEdit*   wFrom;
	WrapperSampleEdit*   wTo;
	WrapperComboBox*     wExportFormat;
    WrapperFileChooser*  wExportFile;
	WrapperQLineEdit*    wNan;
	WrapperQLineEdit*    wSeparator;
    WrapperFileChooser*  wDebugFile;

public:
	MenuFileExport(QWidget* parent = nullptr);
	~MenuFileExport();

public slots:
	void save();
    void help();
};
