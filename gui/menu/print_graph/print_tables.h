#pragma once

#include <QWidget>
#include <QString>
#include <QTableView>

#include "ui_print_tables.h"
#include "utils.h"
#include "settings.h"
#include "wrapper_classes.h"
#include "print_abstract.h"
#include "iode_objs/views/gsample_table_view.h"
#include "custom_widgets/qiode_completer.h"


/* NOTE FOR THE DEVELOPERS:
 * All Menu Dialog classes MUST
 * 1. inherit from QIodeSettings,
 * 2. use the extractAndVerify() method to extract input data from fields,
 * 3. call the Q_OBJECT macro at the beginning of the class to allow slots and signals (see documentation of Qt).
 */

class QIodeMenuPrintTables: public QIodeMenuPrintAbstract, public Ui::QIodeMenuPrintTables
{
    Q_OBJECT
    
    QVector<QIodeGSampleTableView*> tableViews;

    WrapperQPlainTextEdit*   wTablesNames;

    QIodeCompleter*          completer;

public:
	QIodeMenuPrintTables(const QString& settings_filepath, QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
	~QIodeMenuPrintTables();

public slots:
	void display();
	void print();
	void setup();
    void help();
};
