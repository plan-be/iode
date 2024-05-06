#pragma once

#include <QWidget>
#include <QString>
#include <QTableView>

#include "ui_print_tables.h"
#include "utils.h"
#include "settings.h"
#include "wrapper_classes.h"
#include "print_abstract.h"
#include "text_edit/completer.h"
#include "tabs/iode_objs/tab_computed_table.h"

/* NOTE FOR THE DEVELOPERS:
 * All Menu Dialog classes MUST
 * 1. inherit from IodeSettingsDialog,
 * 2. use the extractAndVerify() method to extract input data from fields,
 * 3. call the Q_OBJECT macro at the beginning of the class to allow slots and signals (see documentation of Qt).
 */


class MenuPrintTables: public MenuPrintAbstract, public Ui::MenuPrintTables
{
    Q_OBJECT
    
    QVector<ComputedTableNumericalDialog*> tableViews;

    WrapperQPlainTextEdit*   wTablesNames;

    IodeCompleter*          completer;

public:
	MenuPrintTables(QWidget* parent = nullptr);
	~MenuPrintTables();

public slots:
	void display();
	void print();
	void setup();
    void help();
};
