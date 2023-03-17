#pragma once

#include <QObject>

#include "abstract_table_model.h"
#include "abstract_table_model.cpp"


class ScalarsModel : public QIodeTemplateTableModel<KDBScalars>
{
	Q_OBJECT

public:
	ScalarsModel(QObject* parent = nullptr) : QIodeTemplateTableModel({"Value", "Relax", "Std", "T-Stat"}, parent) {};

private:
	QVariant dataCell(const int row, const int col) const override;

	bool setValue(const int row, const int column, const QVariant& value) override;

public slots:
	void reset() { resetModel(); };
};
