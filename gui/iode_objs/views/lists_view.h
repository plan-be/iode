#pragma once

#include <QObject>

#include "abstract_table_view.h"
#include "iode_objs/models/lists_model.h"
#include "iode_objs/delegates/lists_delegate.h"
#include "iode_objs/new/add_list.h"
#include "iode_objs/edit/edit_iode_obj.h"


class ListsView : public IodeTemplateTableView<ListsModel, EditListDialog>
{
	Q_OBJECT

public:
	ListsView(QWidget* parent = nullptr) : IodeTemplateTableView(I_LISTS, new ListsDelegate(parent), parent) {};

public slots:
	void new_obj();
	void edit_obj() { openEditDialog(); }
};
