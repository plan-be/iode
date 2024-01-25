#pragma once

#include <QObject>

#include "abstract_table_model.h"
#include "abstract_table_model.cpp"
#include "numerical_table_model.h"


class ScalarsModel : public IodeTemplateTableModel<KDBScalars>, public NumericalTableModel
{
	Q_OBJECT

public:
	ScalarsModel(QObject* parent = nullptr, KDBScalars* database = nullptr, const int nb_digits = 2): 
		IodeTemplateTableModel({"Value", "Relax", "Std", "T-Stat"}, database, parent), 
		NumericalTableModel(nb_digits) {}

	QVariant dataCell(const int row, const int col) const override;

	bool setValue(const int row, const int column, const QVariant& value) override;

	void updateDisplayValues() override
	{
		reset();
	}
};
