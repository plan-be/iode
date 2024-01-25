#pragma once

#include <QObject>

#include "abstract_table_model.h"
#include "abstract_table_model.cpp"


class EquationsModel : public IodeTemplateTableModel<KDBEquations>
{
	Q_OBJECT

public:
	EquationsModel(QObject* parent = nullptr, KDBEquations* database = nullptr) 
		: IodeTemplateTableModel({"Equation"}, database, parent) {};

	QVariant dataCell(const int row, const int col) const override;

	bool setValue(const int row, const int column, const QVariant& value) override;
};
