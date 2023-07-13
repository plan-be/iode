#pragma once

#include <QObject>

#include "numerical_view.h"
#include "iode_objs/models/scalars_model.h"
#include "iode_objs/delegates/scalars_delegate.h"
#include "iode_objs/new/add_scalar.h"


class ScalarsView : public TemplateNumericalTableView<ScalarsModel>
{
	Q_OBJECT

public:
	ScalarsView(QWidget* parent = nullptr) 
		: TemplateNumericalTableView(I_SCALARS, new ScalarsDelegate(parent), parent) {};

public slots:
	void filter() { filter_and_update(); }
	void print();
	void new_obj();
};
