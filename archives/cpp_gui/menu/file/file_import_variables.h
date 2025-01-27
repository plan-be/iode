#pragma once

#include <QWidget>

#include "ui_file_import_variables.h"
#include "utils.h"
#include "settings.h"
#include "wrapper_classes.h"


/* NOTE FOR THE DEVELOPERS:
 * All Menu Dialog classes MUST
 * 1. inherit from IodeSettingsDialog,
 * 2. use the extractAndVerify() method to extract input data from fields,
 * 3. call the Q_OBJECT macro at the beginning of the class to allow slots and signals (see documentation of Qt).
 */

class MenuFileImportVariables: public IodeSettingsDialog, public Ui::MenuFileImportVariables
{
    Q_OBJECT

    WrapperFileChooser*  wInputFile;
    WrapperFileChooser*  wRuleFile;
    WrapperFileChooser*  wSaveFile;
    WrapperSampleEdit*   wFrom;
    WrapperSampleEdit*   wTo;
    WrapperFileChooser*  wDebugFile;

public:
	MenuFileImportVariables(QWidget* parent = nullptr);
	~MenuFileImportVariables();

public slots:
	void import();
    void help();
};
