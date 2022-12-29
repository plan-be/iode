#pragma once

#include <QWidget>

#include "ui_file_import_comments.h"
#include "utils.h"
#include "settings.h"
#include "wrapper_classes.h"


/* NOTE FOR THE DEVELOPERS:
 * All Menu Dialog classes MUST
 * 1. inherit from QIodeSettings,
 * 2. use the extractAndVerify() method to extract input data from fields,
 * 3. call the Q_OBJECT macro at the beginning of the class to allow slots and signals (see documentation of Qt).
 */

class QIodeMenuFileImportComments: public QIodeSettings, public Ui::QIodeMenuFileImportComments
{
    Q_OBJECT

    WrapperFileChooser*  wInputFile;
    WrapperFileChooser*  wRuleFile;
    WrapperFileChooser*  wSaveFile;
    WrapperComboBox*     wLanguage;
    WrapperFileChooser*  wDebugFile;

public:
	QIodeMenuFileImportComments(const QString& settings_filepath, QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
	~QIodeMenuFileImportComments();

public slots:
	void import();
    void help();
};
