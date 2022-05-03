#pragma once

#include <QObject>
#include <QMessageBox>

#include "abstract_table_view.h"
#include "variables_model.h"
#include "variables_delegate.h"
#include "popup/add_variable.h"

#include "../../utils.h"


class VariablesView : public AbstractTableView<VariablesModel>
{
	Q_OBJECT

public:
	VariablesView(QWidget* parent = nullptr) : AbstractTableView(I_VARIABLES, new VariablesDelegate(parent), parent) {};

public slots:
	void new_variable();
};
