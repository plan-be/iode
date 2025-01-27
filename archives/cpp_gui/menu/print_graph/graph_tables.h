#pragma once

#include <QWidget>

#include "ui_graph_tables.h"
#include "utils.h"
#include "settings.h"
#include "wrapper_classes.h"
#include "plot/plot_table.h"
#include "main_window_abstract.h"
#include "text_edit/completer.h"
#include "menu/file/file_settings.h"


/* NOTE FOR THE DEVELOPERS:
 * All Menu Dialog classes MUST
 * 1. inherit from IodeSettingsDialog,
 * 2. use the extractAndVerify() method to extract input data from fields,
 * 3. call the Q_OBJECT macro at the beginning of the class to allow slots and signals (see documentation of Qt).
 */

class MenuGraphTables: public IodeSettingsDialog, public Ui::MenuGraphTables
{
    Q_OBJECT

    WrapperQPlainTextEdit*   wTableNames;
    WrapperQTextEdit*        wSample;
    WrapperFileChooser*      wFile2;
    WrapperFileChooser*      wFile3;
    WrapperFileChooser*      wFile4;
    WrapperFileChooser*      wFile5;
    WrapperComboBox*         wLanguage;

    IodeCompleter*          completer;

public:
	MenuGraphTables(QWidget* parent = nullptr);
	~MenuGraphTables();

signals:
    void newPlot(QDialog* plotDialog);

public slots:
	void display();
	void apply();
	void setup();
    void help();

private slots:
    void freeReferenceKDBs();
};
