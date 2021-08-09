#pragma once

#include <QObject>
#include <QTableView>

#include "abstract_qtableview.h"
#include "../models/variables_model.h"


class VariablesView : public AbstractTableView<VariablesModel>
{
	Q_OBJECT

public:
	VariablesView(QWidget* parent = nullptr) : AbstractTableView(parent) {};
};
