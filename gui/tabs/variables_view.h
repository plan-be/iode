#pragma once

#include <QObject>

#include "abstract_table_view.h"
#include "variables_model.h"
#include "variables_delegate.h"


class VariablesView : public AbstractTableView<VariablesModel>
{
	Q_OBJECT

public:
	VariablesView(QWidget* parent = nullptr) : AbstractTableView(I_VARIABLES, new VariablesDelegate(parent), parent) {};
};
