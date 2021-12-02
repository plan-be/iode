#pragma once

#include <QObject>

#include "abstract_table_view.h"
#include "lists_model.h"
#include "lists_delegate.h"


class ListsView : public AbstractTableView<ListsModel>
{
	Q_OBJECT

public:
	ListsView(QWidget* parent = nullptr) : AbstractTableView(LISTS, new ListsDelegate(parent), parent) {};
};
