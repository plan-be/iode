#pragma once

#include <QObject>

#include "abstract_table_model.h"


class ListsModel : public IODEAbstractTableModel<Lists>
{
	Q_OBJECT

public:
	ListsModel(QObject* parent = nullptr) : IODEAbstractTableModel({ "Name", "List" }, parent) {};

private:
	QVariant dataCell(const int row, const int col) const
	{
		if (col == 0)
			return QVariant(QString(iodeItems.getObjectName(row)));
		else
			return QVariant(QString(iodeItems.getObjectValue(row)));
	}

public slots:
	void reset() { resetModel(); };
};
