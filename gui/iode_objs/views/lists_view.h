#pragma once

#include <QObject>

#include "abstract_table_view.h"
#include "iode_objs/models/lists_model.h"
#include "iode_objs/delegates/lists_delegate.h"
#include "iode_objs/new/add_list.h"
#include "iode_objs/edit/edit_list.h"


class ListsView : public IodeAbstractTableView, public TableViewEditObj<ListsModel, EditListDialog>, public TableViewAddObj<ListsModel, AddListDialog>
{
	Q_OBJECT

public:
	ListsView(QWidget* parent = nullptr) : 
		IodeAbstractTableView(LISTS, new ListsDelegate(parent), parent), 
		TableViewEditObj(this), TableViewAddObj(this) {};

public slots:
	void new_obj()  { openAddDialog(); }
	void edit_obj() { openEditDialog(); }
};
