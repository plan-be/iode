#pragma once

#include <QObject>

#include "abstract_table_model.h"


class TablesModel : public IODEAbstractTableModel<Tables>
{
	Q_OBJECT

public:
	TablesModel(QObject* parent = nullptr) : IODEAbstractTableModel({ "Name", "Table" }, parent) {};

private:
	QVariant dataCell(const int row, const int col) const
	{
		if (col == 0)
			return QVariant(QString(kdb.getObjectName(row)));
		else
			return QVariant(kdb.getTitle(row));
	}

public slots:
	void reset() { resetModel(); };
};
