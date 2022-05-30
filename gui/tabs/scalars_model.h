#pragma once

#include <QObject>

#include "abstract_table_model.h"
#include "abstract_table_model.cpp"


class ScalarsModel : public IODEAbstractTableModel<KDBScalars>
{
	Q_OBJECT

public:
	ScalarsModel(QObject* parent = nullptr) : IODEAbstractTableModel({ "Name", "Value", "Relax", "Std", "T-Stat" }, parent) {};

private:
	QVariant dataCell(const int row, const int col) const;

public slots:
	void reset() { resetModel(); };
};
