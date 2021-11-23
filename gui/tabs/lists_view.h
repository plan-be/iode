#pragma once

#include <QObject>

#include "abstract_table_view.h"
#include "lists_model.h"
#include "text_delegate.h"


class ListsView : public AbstractTableView<ListsModel>
{
	Q_OBJECT

public:
	ListsView(QWidget* parent = nullptr) : AbstractTableView(LISTS, new TextDelegate(parent), parent) {};
};
