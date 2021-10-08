#pragma once

#include <QObject>

#include "abstract_table_view.h"
#include "variables_model.h"


class VariablesView : public AbstractTableView<VariablesModel>
{
	Q_OBJECT

public:
	VariablesView(QWidget* parent = nullptr) : AbstractTableView(parent) {};
};
