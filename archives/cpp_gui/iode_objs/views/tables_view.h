#pragma once

#include <QObject>
#include <QShortcut>

#include "abstract_table_view.h"
#include "iode_objs/delegates/tables_delegate.h"
#include "iode_objs/models/tables_model.h"
#include "iode_objs/edit/edit_table.h"
#include "iode_objs/new/add_table.h"
#include "plot/plot_table.h"
#include "tabs/iode_objs/tab_computed_table.h"


class TablesView : public IodeAbstractTableView, public TableViewEditObj<TablesModel, EditTableDialog>, public TableViewAddObj<TablesModel, AddTableDialog>
{
	Q_OBJECT

	QShortcut* shortcutDisplay;
	QShortcut* shortcutPlot;

public:
	TablesView(QWidget* parent = nullptr) : 
		IodeAbstractTableView(TABLES, new TablesDelegate(parent), parent), TableViewEditObj(this), TableViewAddObj(this)
	{
		shortcutDisplay = new QShortcut(QKeySequence(Qt::Key_F7), this);
		shortcutDisplay->setContext(Qt::WidgetWithChildrenShortcut);
		connect(shortcutDisplay, &QShortcut::activated, this, &TablesView::display);

		shortcutPlot = new QShortcut(QKeySequence(Qt::Key_F8), this);
		shortcutPlot->setContext(Qt::WidgetWithChildrenShortcut);
		connect(shortcutPlot, &QShortcut::activated, this, &TablesView::plot);
	}

	~TablesView()
	{
		delete shortcutDisplay;
		delete shortcutPlot;
	}

public slots:
	void new_obj()  { openAddDialog(); }
	void edit_obj() { openEditDialog(); }
	void display();
	void plot();
};
