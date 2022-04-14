#pragma once

#include <QObject>

#include "abstract_table_model.h"


class EquationsModel : public IODEAbstractTableModel<KDBEquations>
{
	Q_OBJECT

public:
	EquationsModel(QObject* parent = nullptr) : IODEAbstractTableModel({ "Name", "Equation" }, parent) {};

private:
	QVariant dataCell(const int row, const int col) const
	{
		if (col == 0)
			return QVariant(QString::fromStdString(kdb.get_name(row)));
		else
			return QVariant(QString::fromStdString(kdb.get_lec(row)));
	}

public slots:
	void reset() { resetModel(); };
};
