#pragma once

#include <QObject>

#include "abstract_table_view.h"
#include "iode_objs/models/scalars_model.h"
#include "iode_objs/delegates/scalars_delegate.h"
#include "iode_objs/new/add_scalar.h"


class ScalarsView : public TemplateTableView<ScalarsModel>
{
	Q_OBJECT

public:
	ScalarsView(QWidget* parent = nullptr) : TemplateTableView(I_SCALARS, new ScalarsDelegate(parent), parent) {};

public slots:
	void filter() { filter_and_update(); }
	void print();
	void new_obj();
};
