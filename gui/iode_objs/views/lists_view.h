#pragma once

#include <QObject>

#include "abstract_table_view.h"
#include "iode_objs/models/lists_model.h"
#include "iode_objs/delegates/lists_delegate.h"
#include "iode_objs/new/add_list.h"


class ListsView : public IodeAbstractTableView
{
	Q_OBJECT

public:
	ListsView(QWidget* parent = nullptr) : IodeAbstractTableView(I_LISTS, new ListsDelegate(parent), parent) {};

public slots:
	void new_obj();
};
