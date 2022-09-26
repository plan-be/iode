#pragma once

#include <QObject>

#include "iode_objs/edit/popup_view.h"
#include "iode_objs/delegates/tables_delegate.h"
#include "iode_objs/models/tables_model.h"
#include "iode_objs/edit/edit_table.h"
#include "iode_objs/new/add_table.h"


class TablesView : public PopupView<TablesModel, QIodeEditTable>
{
	Q_OBJECT

public:
	TablesView(QWidget* parent = nullptr) : PopupView(I_TABLES, new TablesDelegate(parent), parent)
	{
		connect(this, &TablesView::activated, this, &TablesView::popup_edit_window);
	}

public slots:
	void filter() { filter_and_update(); }
	void new_table();
	void popup_edit_window(const QModelIndex& index) { open_popup_edit_window(index); }
};
