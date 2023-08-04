#pragma once

#include <QObject>

#include "abstract_table_model.h"
#include "abstract_table_model.cpp"
#include "numerical_table_model.h"


class ScalarsModel : public QIodeTemplateTableModel<KDBScalars>, public NumericalTableModel
{
	Q_OBJECT

public:
	ScalarsModel(QObject* parent = nullptr, KDBScalars* kdb_external = nullptr, const int nb_digits = 2): 
		QIodeTemplateTableModel({"Value", "Relax", "Std", "T-Stat"}, parent, kdb_external),
		NumericalTableModel(nb_digits)
	{}

	QVariant dataCell(const int row, const int col) const override;

	bool setValue(const int row, const int column, const QVariant& value) override;
};
