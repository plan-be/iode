#pragma once

#include <QObject>

#include "abstract_table_model.h"


class TablesModel : public IODEAbstractTableModel<KDBTables>
{
	Q_OBJECT

public:
	TablesModel(QObject* parent = nullptr) : IODEAbstractTableModel({ "Name", "Table" }, parent) {};

private:
	QVariant dataCell(const int row, const int col) const
	{
		if (col == 0)
			return QVariant(QString::fromStdString(kdb.getName(row)));
		else
			return QVariant(QString::fromStdString(kdb.getTitle(row)));
	}

public slots:
	void reset() { resetModel(); };
};
