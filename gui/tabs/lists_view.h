#pragma once

#include <QObject>

#include "abstract_table_view.h"
#include "lists_model.h"


class ListsView : public AbstractTableView<ListsModel>
{
	Q_OBJECT

public:
	ListsView(QWidget* parent = nullptr) : AbstractTableView(parent) {};
};
