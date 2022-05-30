#pragma once

#include <QObject>

#include "abstract_table_model.h"
#include "abstract_table_model.cpp"


class EquationsModel : public IODEAbstractTableModel<KDBEquations>
{
	Q_OBJECT

public:
	EquationsModel(QObject* parent = nullptr) : IODEAbstractTableModel({ "Name", "Equation" }, parent) {};

private:
	QVariant dataCell(const int row, const int col) const;

public slots:
	void reset() { resetModel(); };
};
