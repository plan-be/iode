#pragma once

#include <QWidget>
#include <QString>
#include <QTableView>

#include "ui_edit_tables.h"
#include "utils.h"
#include "settings.h"
#include "wrapper_classes.h"
#include "text_edit/completer.h"
#include "tabs/iode_objs/tab_computed_table.h"


/* NOTE FOR THE DEVELOPERS:
 * All Menu Dialog classes MUST
 * 1. inherit from IodeSettingsDialog,
 * 2. use the extractAndVerify() method to extract input data from fields,
 * 3. call the Q_OBJECT macro at the beginning of the class to allow slots and signals (see documentation of Qt).
 */

class MenuDataEditTables: public IodeSettingsDialog, public Ui::MenuDataEditTables
{
    Q_OBJECT
    
    QVector<ComputedTableNumericalDialog*> tableViews;

    WrapperQPlainTextEdit*   wTablesNames;

    IodeCompleter*          completer;

public:
	MenuDataEditTables(QWidget* parent = nullptr);
	~MenuDataEditTables();

public slots:
	void display();
    void help();
};
