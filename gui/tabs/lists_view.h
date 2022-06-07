#pragma once

#include <QObject>

#include "abstract_table_view.h"
#include "lists_model.h"
#include "lists_delegate.h"
#include "popup/add_list.h"


class ListsView : public AbstractTableView<ListsModel>
{
	Q_OBJECT

public:
	ListsView(QWidget* parent = nullptr) : AbstractTableView(I_LISTS, new ListsDelegate(parent), parent) {};

public slots:
	void filter() { filter_and_update(); }
	void new_list();
};
