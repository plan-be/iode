#pragma once

#include <QObject>

#include "abstract_table_view.h"
#include "iode_objs/models/scalars_model.h"
#include "iode_objs/delegates/scalars_delegate.h"
#include "iode_objs/new/add_scalar.h"


class ScalarsView : public AbstractTableView<ScalarsModel>
{
	Q_OBJECT

public:
	ScalarsView(QWidget* parent = nullptr) : AbstractTableView(I_SCALARS, new ScalarsDelegate(parent), parent) {};

public slots:
	void filter() { filter_and_update(); }
	void new_obj();
};
