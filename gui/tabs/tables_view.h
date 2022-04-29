#pragma once

#include <QObject>

#include "popup_view.h"
#include "tables_delegate.h"
#include "tables_model.h"
#include "popup/edit_table.h"
#include "popup/add_table.h"


class TablesView : public PopupView<TablesModel, QIodeEditTable>
{
	Q_OBJECT

public:
	TablesView(QWidget* parent = nullptr) : PopupView(I_TABLES, new TablesDelegate(parent), parent) 
	{
		connect(this, &TablesView::activated, this, &TablesView::popup_edit_window);
	}

public slots:
	void new_table();
	void popup_edit_window(const QModelIndex& index) { open_popup_edit_window(index); }
};
