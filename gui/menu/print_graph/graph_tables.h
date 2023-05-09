#pragma once

#include <QWidget>

#include "ui_graph_tables.h"
#include "utils.h"
#include "settings.h"
#include "wrapper_classes.h"
#include "plot/plot.h"
#include "custom_widgets/completer.h"
#include "menu/file/file_print_setup.h"


/* NOTE FOR THE DEVELOPERS:
 * All Menu Dialog classes MUST
 * 1. inherit from QIodeSettings,
 * 2. use the extractAndVerify() method to extract input data from fields,
 * 3. call the Q_OBJECT macro at the beginning of the class to allow slots and signals (see documentation of Qt).
 */

class QIodeMenuGraphTables: public QIodeSettings, public Ui::QIodeMenuGraphTables
{
    Q_OBJECT

    WrapperQPlainTextEdit*   wTableNames;
    WrapperQTextEdit*        wSample;
    WrapperFileChooser*      wFile2;
    WrapperFileChooser*      wFile3;
    WrapperFileChooser*      wFile4;
    WrapperFileChooser*      wFile5;
    WrapperComboBox*         wLanguage;

    QIodeCompleter*          completer;

public:
	QIodeMenuGraphTables(const QString& settings_filepath, QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
	~QIodeMenuGraphTables();

signals:
    void newPlot(QIodePlotDialog* plotDialog);

public slots:
	void display();
	void apply();
	void setup();
    void help();

private slots:
    void freeReferenceKDBs();
};
