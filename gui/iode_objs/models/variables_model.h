#pragma once

#include <QObject>

#include "abstract_table_model.h"
#include "abstract_table_model.cpp"
#include "numerical_table_model.h"


class VariablesModel : public NumericalTableModel<KDBVariables>
{
	Q_OBJECT

public:
	VariablesModel(QObject* parent = nullptr, KDBVariables* kdb_external = nullptr, 
		const int nb_digits = 2) : NumericalTableModel({}, parent, kdb_external, nb_digits) {}

	QVariant dataCell(const int row, const int col) const override;

	bool setValue(const int row, const int column, const QVariant& value) override;

public slots:
	void reset() override;
};
