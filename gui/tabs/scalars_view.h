#pragma once

#include <QObject>

#include "abstract_table_view.h"
#include "scalars_model.h"
#include "scalars_delegate.h"
#include "popup/add_scalar.h"


class ScalarsView : public AbstractTableView<ScalarsModel>
{
	Q_OBJECT

public:
	ScalarsView(QWidget* parent = nullptr) : AbstractTableView(I_SCALARS, new ScalarsDelegate(parent), parent) {};

public slots:
	void new_scalar();
};
