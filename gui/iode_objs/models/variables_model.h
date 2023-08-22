#pragma once

#include <QObject>

#include "abstract_table_model.h"
#include "abstract_table_model.cpp"
#ifndef TEMPLATE_NUMERICAL_MODEL_HEADER
#include "numerical_table_model.h"
#endif


class VariablesModel : public TemplateNumericalTableModel<KDBVariables>
{
	Q_OBJECT

	EnumIodeVarMode mode;

public:
	VariablesModel(QObject* parent = nullptr, KDBVariables* kdb_external = nullptr, const int nb_digits = 2, 
		const EnumIodeVarMode mode = I_VAR_MODE_LEVEL): TemplateNumericalTableModel({"Value", "Relax", "Std", "T-Stat"}, 
		parent, nb_digits, kdb_external), mode(mode)
	{}

	QVariant dataCell(const int row, const int col) const override;

	bool setValue(const int row, const int column, const QVariant& value) override;

public slots:
	void reset() override;
	void setMode(const EnumIodeVarMode mode)
	{
		this->mode = mode;
		this->reset();
	}
};
