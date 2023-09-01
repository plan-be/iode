#pragma once

#include <QObject>

#include "iode_objs/models/scalars_model.h"
#include "iode_objs/delegates/scalars_delegate.h"
#include "iode_objs/new/add_scalar.h"
#ifndef TEMPLATE_NUMERICAL_VIEW_HEADER
#include "numerical_view.h"
#endif


class ScalarsView : public TemplateNumericalTableView
{
	Q_OBJECT

public:
	ScalarsView(QWidget* parent = nullptr) 
		: TemplateNumericalTableView(I_SCALARS, new ScalarsDelegate(parent), parent) {};

public slots:
	void new_obj();
};
