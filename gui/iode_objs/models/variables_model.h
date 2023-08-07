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

public:
	VariablesModel(QObject* parent = nullptr, KDBVariables* kdb_external = nullptr, const int nb_digits = 2): 
		TemplateNumericalTableModel({"Value", "Relax", "Std", "T-Stat"}, parent, nb_digits, kdb_external)
	{}

	QVariant dataCell(const int row, const int col) const override;

	bool setValue(const int row, const int column, const QVariant& value) override;

public slots:
	void reset() override;
};
