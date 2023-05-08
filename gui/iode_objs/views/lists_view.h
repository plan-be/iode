#pragma once

#include <QObject>

#include "abstract_table_view.h"
#include "iode_objs/models/lists_model.h"
#include "iode_objs/delegates/lists_delegate.h"
#include "iode_objs/new/add_list.h"


class ListsView : public TemplateTableView<ListsModel>
{
	Q_OBJECT

public:
	ListsView(QWidget* parent = nullptr) : TemplateTableView(I_LISTS, new ListsDelegate(parent), parent) {};

public slots:
	void filter() { filter_and_update(); }
	void print() { QMessageBox::warning(nullptr, "WARNING", "Not yet implemented"); }
	void new_obj();
};
