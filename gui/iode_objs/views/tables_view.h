#pragma once

#include <QObject>
#include <QShortcut>

#include "abstract_table_view.h"
#include "iode_objs/delegates/tables_delegate.h"
#include "iode_objs/models/tables_model.h"
#include "iode_objs/edit/edit_table.h"
#include "iode_objs/new/add_table.h"


class TablesView : public TemplateTableView<TablesModel>
{
	Q_OBJECT

	QShortcut* editShortcutEnter;
	QShortcut* editShortcutReturn;

public:
	TablesView(QWidget* parent = nullptr) : TemplateTableView(I_TABLES, new TablesDelegate(parent), parent)
	{
		editShortcutEnter = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Enter), this);
		connect(editShortcutEnter, &QShortcut::activated, this, &TablesView::edit_obj);
		
		editShortcutReturn = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Return), this);
		connect(editShortcutReturn, &QShortcut::activated, this, &TablesView::edit_obj);
	}

	~TablesView()
	{
		delete editShortcutEnter;
		delete editShortcutReturn;
	}

public slots:
	void filter() { filter_and_update(); }
	void new_obj();
	void edit_obj();
};
