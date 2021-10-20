#pragma once

#include <QObject>

#include "abstract_table_model.h"


class EquationsModel : public IODEAbstractTableModel<Equations>
{
	Q_OBJECT

public:
	EquationsModel(QObject* parent = nullptr) : IODEAbstractTableModel({ "Name", "Equation" }, parent) {};

private:
	QVariant dataCell(const int row, const int col) const
	{
		if (col == 0)
			return QVariant(QString(iodeItems.getObjectName(row)));
		else
			return QVariant(QString(iodeItems.getLec(row)));
	}

public slots:
	void reset() { resetModel(); };
};
