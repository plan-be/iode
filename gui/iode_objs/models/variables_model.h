#pragma once

#include <QObject>

#include "abstract_table_model.h"
#include "abstract_table_model.cpp"
#include "numerical_table_model.h"


class VariablesModel : public IodeTemplateTableModel<KDBVariables>, public NumericalTableModel
{
	Q_OBJECT

	IodeVarMode mode;

public:
	VariablesModel(QObject* parent = nullptr, KDBVariables* database = nullptr, const int nb_digits = 2, 
		const IodeVarMode mode = VAR_MODE_LEVEL): 
		IodeTemplateTableModel({"Value", "Relax", "Std", "T-Stat"}, database, parent), 
		NumericalTableModel(nb_digits), mode(mode) {}

	QVariant dataCell(const int row, const int col) const override;

	bool setValue(const int row, const int column, const QVariant& value) override;

	void updateDisplayValues() override
	{
		reset();
	}

public slots:
	void reset() override;
	void setMode(const IodeVarMode mode)
	{
		this->mode = mode;
		this->reset();
	}
};
